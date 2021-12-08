package main

import (
	"github.com/eclipse/paho.golang/paho"

	"bytes"
	"context"
	"fmt"
	"log"
	"net"
	"os"
	"os/signal"
	"runtime"
	"sync"
	"syscall"
	"time"
)

type msg struct {
	t       time.Time
	topic   string
	payload []byte
}

var (
	server = "127.0.0.1:1883"
	qos    = 0
	wg     sync.WaitGroup
)

func main() {

	mqttChan := make(chan *paho.Publish)
	msgChan := make(chan *msg, 10)
	quitChan := make(chan bool)

	wg.Add(1)
	go func() {
		handler(msgChan, quitChan)
		wg.Done()
	}()

	conn, err := net.Dial("tcp", server)
	x(err)

	c := paho.NewClient(paho.ClientConfig{
		Router: paho.NewSingleHandlerRouter(func(m *paho.Publish) {
			mqttChan <- m
		}),
		Conn: conn,
	})
	cp := &paho.Connect{
		KeepAlive:  30,
		ClientID:   "koikoi",
		CleanStart: true,
	}
	ca, err := c.Connect(context.Background(), cp)
	x(err)
	if ca.ReasonCode != 0 {
		x(fmt.Errorf("Failed to connect to %s : %d - %s", server, ca.ReasonCode, ca.Properties.ReasonString))
	}

	ic := make(chan os.Signal, 1)
	signal.Notify(ic, syscall.SIGHUP, syscall.SIGINT, syscall.SIGQUIT, syscall.SIGTERM)
	go func() {
		sig := <-ic
		close(quitChan)
		if c != nil {
			d := &paho.Disconnect{ReasonCode: 0}
			c.Disconnect(d)
		}
		x(fmt.Errorf("signal received, exiting: %v", sig))
		wg.Wait()
		os.Exit(0)
	}()

	sa, err := c.Subscribe(context.Background(), &paho.Subscribe{
		Subscriptions: map[string]paho.SubscribeOptions{
			"boven/#": {QoS: byte(qos)},
			"pc/#":    {QoS: byte(qos)},
			"tab/#":   {QoS: byte(qos)},
		},
	})
	x(err)
	if sa.Reasons[0] != byte(qos) {
		x(fmt.Errorf("Failed to subscribe: %d", sa.Reasons[0]))
	}

	for m := range mqttChan {
		msgChan <- &msg{
			t:       time.Now().UTC(),
			topic:   m.Topic,
			payload: m.Payload,
		}
	}
}

func handler(msgChan chan *msg, quitChan chan bool) {
	hour := -1
	min := -1
	var err error
	var fp *os.File
	for {
		select {
		case m := <-msgChan:
			if hr, mn := m.t.Hour(), m.t.Minute()/10; hr != hour || mn != min {
				hour = hr
				min = mn
				if fp != nil {
					fp.Close()
				}
				filename := fmt.Sprintf("logger-%d%02d%02d-%02d%d0.txt", m.t.Year(), int(m.t.Month()), m.t.Day(), hr, mn)
				if _, e := os.Stat(filename); e == nil {
					fp, err = os.OpenFile(filename, os.O_WRONLY|os.O_APPEND, 0644)
				} else {
					fp, err = os.Create(filename)
				}
				x(err)
			}
			fmt.Fprintf(fp, "%v\t%s\t%s\n", m.t.Unix(), m.topic, string(m.payload))
		case <-quitChan:
			if fp != nil {
				fp.Close()
			}
			return
		}
	}
}

func makeErr(err error, msg ...interface{}) string {
	var b bytes.Buffer
	_, filename, lineno, ok := runtime.Caller(2)
	if ok {
		b.WriteString(fmt.Sprintf("%v:%v: %v", filename, lineno, err))
	} else {
		b.WriteString(err.Error())
	}
	if len(msg) > 0 {
		b.WriteString(",")
		for _, m := range msg {
			b.WriteString(fmt.Sprintf(" %v", m))
		}
	}
	return b.String()
}

func x(err error, msg ...interface{}) {
	if err == nil {
		return
	}
	log.Fatalln(makeErr(err, msg...))
}

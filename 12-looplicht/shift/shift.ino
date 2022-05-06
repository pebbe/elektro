
#define TINY

#ifdef TINY
#define PRINT(x)
#define PRINTLN(x)
#define DTA 1  // fysieke pin 6
#define CLK 0  // fysieke pin 5
#else
#define PRINT(x) Serial.print(x)
#define PRINTLN(x) Serial.println(x)
#define DTA 3
#define CLK 6
#endif

void setup() {

    pinMode(A1, INPUT);
    pinMode(A3, INPUT);
    pinMode(DTA, OUTPUT);
    pinMode(CLK, OUTPUT);
    
#ifdef DEBUG
    Serial.begin(9600);
    while (!Serial)
        ;
#endif

}

void loop() {

    int speed = analogRead(A1);
    int activ = analogRead(A3);
    PRINT(speed);
    PRINT("  ");
    PRINTLN(activ);

    digitalWrite(DTA, random(0, 1023) < activ ? HIGH : LOW);
    delay(2);
    digitalWrite(CLK, HIGH);
    delay(2);
    digitalWrite(CLK, LOW);

    delay(speed);
}

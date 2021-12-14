## Project 10 — Pi Zero 2 met MQTT-broker

Als je met je zelfgebouwde IoT-dingetjes in huis gegevens wilt
verzamelen, dan moet je die gegevens ergens opslaan. Daar zijn
verschillende mogelijkheden voor.

Je kunt een internet-dienst gebruiken. Ik hou daar niet van. Je bent
afhankelijk, en je geeft je data uit handen.

Je kunt een service opzetten op je PC. Maar dan moet die PC wel 24 uur
per dag aan staan. Dat wil je niet met zo'n energieslurper.

Wat overblijft is een mini-computer of een *single board computer*, een
apparaat dat heel weinig energie verbruikt.

Ik wilde een [Rasberry Pi 4B](https://www.raspberrypi.com/products/raspberry-pi-4-model-b/) gebruiken. Die is al een flinke tijd voor
me in bestelling. Nog steeds niet gearriveerd. Nergens leverbaar. De
vraag is veel groter dan het aanbod. Gelukkig wist ik m'n hand te
leggen op een [Raspberry Pi Zero 2](https://www.raspberrypi.com/products/raspberry-pi-zero-2-w/).

Een Raspberry met Ubuntu Server 64bits, en daar dan [Mosquitto](https://mosquitto.org/) op
draaien (een *broker* voor [MQTT](https://mqtt.org/)), [InfluxDB](https://www.influxdata.com/products/influxdb/) (een *time series database*),
[Telegram](https://www.influxdata.com/time-series-platform/telegraf/) (om gegevens vanuit Mosquitto in InfluxDB te krijgen), en
[Grafana](https://grafana.com/oss/grafana/) (voor de weergave van data uit InfluxDB in grafieken die beter
zijn dan de grafieken die InfluxDB zelf kan produceren).

Dit alles bleek niet mogelijk. Op een Pi 4B had het gekund, met 4 of 8
Gb geheugen, maar de Zero 2 heeft maar een half Gb. InfluxDB en co vreten
geheugen. Deze set-up zal moeten wachten tot (als) m'n Pi 4B
arriveert.

Ik heb nu een minimale set-up. Een server met Moquitto, en een klein
programmaatje dat de gegevens die binnenkomen via Mosquitto opslaat in
tekstbestanden. Als m'n PC aan staat, dan worden de gegevens
regelmatig van de Pi naar de PC verplaatst, en daar ingevoerd in
InfluxDB. Grafana draait ook op m'n PC zodat ik daar de grafieken kan
bekijken, maar ook op m'n tablet, mits de PC aanstaat. Er zit wel een
vertraging van 12 tot 22 minuten in tot de gegevens op deze manier
beschikbaar zijn.

Actuele gegevens (de meest actuele waardes) zijn wel direct te zien op m'n PC
(met [mosquitto_sub](https://mosquitto.org/man/mosquitto_sub-1.html)) en op m'n tablet
(met [MQTT Dash](https://play.google.com/store/apps/details?id=net.routix.mqttdash),
ook als de PC uit staat).

### MQTT

[MQTT](https://mqtt.org/) is een prachtige techniek, een zeer eenvoudige standaard die
breed wordt ondersteund, waarmee je makkelijk data-bronnen en
data-gebruikers aan elkaar kunt koppelen. Alles gaat via een centraal
punt, de broker, die bij mij op de Pi draait (Mosquitto). Bronnen sturen data
daarheen zonder zich verder hoeven te bekommeren om waar de data
uiteindelijk heen moet om gebruikt te worden. M'n Nano IoT uit [project
08](../08-nano-iot/) stuurt de gegevens van de weersensor daarheen. M'n PC en m'n tablet
sturen allebei gegevens over wanneer het scherm aan is, zodat ik
inzicht krijg ik m'n schermgebruik (dat blijkt meer dan verstandig).
En de Pi stuurt meetgegevens over zichzelf naar zichzelf, over
geheugengebruik, temperatuur van de processor, etc.


### Rapsberry Pi Zero 2: Set-up

Algemene instructies: [How to install Ubuntu Server on your Raspberry Pi](https://ubuntu.com/tutorials/how-to-install-ubuntu-on-your-raspberry-pi)

Als besturingssysteem op de Pi Zero 2 gebruik ik Ubuntu Server. Ik
gebruik de 64-bits versie zodat ik ook Go-programma's kan draaien, die
je trouwens gewoon op je PC kunt compileren, dankzij de
cross-compileermogelijkheid van Go.
(InfluxDB draait ook alleen onder 64 bits.) De Pi Zero 2 wordt (nog?)
niet officieel ondersteund door Ubuntu, maar met een simpele
truc kun je het er wel op installeren.

Onderstaande instructies heb ik van https://waldorf.waveform.org.uk/2021/the-pi-zero-2.html

 * Flash a fresh Ubuntu Impish image to your choice of storage media
   (SD card, SSD, etc). Probably the easiest means of doing this is
   the [Raspberry Pi Imaging utility](http://rptl.io/imager) (you’ll find Ubuntu under “Other
   general purpose OS”).
 * Once complete, safely remove the medium and re-insert. The boot
   partition (and potentially the root partition if you’re on Linux)
   will auto-mount.
 * Open the boot partition in your file-browser and find the
   `bcm2710-rpi-3-b.dtb` file. Make a copy of this file and rename it to
   `bcm2710-rpi-zero-2.dtb`. The trick here is simply that the Zero 2’s
   hardware is close enough to the 3B (minus the ethernet port, some
   RAM, and the DSI port) that the 3B’s device-tree works.
 * You’re done! You can now boot the card on your new Zero 2. Once
   you’ve done that, go through the usual `sudo apt update`, `sudo apt
   upgrade` dance to get the “proper” device-tree installed.

Dit werkte, bijna goed. Ik kreeg een foutmelding bij `apt upgrade`.

```
Setting up linux-firmware (1.201.1) ...
update-initramfs: Generating /boot/initrd.img-5.13.0-1008-raspi
Killed
E: mkinitramfs failure zstd -q -19 -T0 137
update-initramfs: failed for /boot/initrd.img-5.13.0-1008-raspi with 1.
dpkg: error processing package linux-firmware (--configure):
 installed linux-firmware package post-installation script subprocess returned error exit status 1

Errors were encountered while processing:
 linux-firmware
needrestart is being skipped since dpkg has failed
```

Een oplossing voor dit probleem vond ik elders. In
`/etc/initramfs-tools/initramfs.conf` moet je `zstd` vervangen door
`lz4`. En dan:

```
sudo apt install lz4
sudo apt upgrade
```

#### Mosquitto

Mosquitto installeren is simpel:

```
sudo apt install mosquitto mosquitto-clients
```

Je moet dan nog wel de configuratie aanpassen om Mosquitto bereikbaar
te maken voor andere apparaten in je lokale netwerk. Maak het bestand
`/etc/mosquitto/conf.d/local.conf` met deze inhoud:

```
allow_anonymous true
listener 1883
```

Herstarten:

```
sudo systemctl restart mosquitto
```

Ik heb twee extra services aangemaakt (zie bronbestanden op github),
`mqtt-logger.service` en `mqtt-status.service`. De eerste verzamelt
data die binnenkomt in Mosquitto en slaat die op in tekstbestanden. De tweede
stuurt statusinformatie over de Pi naar Mosquitto. 

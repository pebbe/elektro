## Project 08 — Arduino Nano 33 IoT

M'n eerste project met een microcontroller met toegang tot het
internet. Nog geen echte IoT-toepassing. Er wordt geen data opgeslagen
via het internet. De verbinding is er alleen om gegevens op te vragen.

Ik had een ESP32 kunnen gebruiken, maar ik koos voor de drie keer zo
dure Arduino Nano 33 IoT. Ik dacht, met een echte Arduino loop ik
minder risico op problemen met library's.

Het idee was dat ik een klokje had waarop ik 's nachts zou kunnen zien
hoe lang het nog zou duren voor de zon op kwam. Tegenwoordig kan ik
dat direct op m'n tablet zien, maar die wil ik 's nachts niet
gebruiken.

Ik heb het idee wat uitgebreid. Tussen zonsondergang en zonsopkomst
laat het niet alleen zien hoe lang het nog duurt tot de zon opkomt,
maar ook hoe lang het geleden is dat de zon onderging. En tussen
zonsopkomst en zonsondergang het omgekeerde. Daarnaast laat het ook de
huidige tijd zien. En ik dacht, laat ik er nog een sensor bijzetten
voor temperatuur en luchtvochtigheid.

Het display is slechts 1,3 inch groot. Daar past alle info op als je
het klein weergeeft, maar ik wilde de waardes van een afstandje kunnen
lezen, zonder bril. Daarom laat het afwisselend steeds een van de
waardes zien.

De hoeveelheid licht dat de letters op het display standaard geven is
aardig sterk, te veel voor nachtelijk gebruik. De maximale waarde voor
helderheid is 255. Ik gebruik nu 's nachts de waarde 30, en overdag 100.

De tijd wordt gesynchroniseerd via NTP. De tijd wordt opgevraagd van
m'n modem die tevens router en NTP-server is.

De tijden waarop op mijn lokatie de zon opkomt en ondergaat worden
opgevraagd van een server. De code om die tijden te berekenen zijn te
groot om door de microcontroller zelf te laten berekenen.

Op de foto's is de eerste opzet te zien. Er is een LED die brandt als
er verbinding met het internet wordt gemaakt om gegevens op te vragen.
De tijden van zonsopkomst en ondergang worden twee keer per dag
opgevraagd. En ik dacht dat een of twee keer per dag de tijd opvragen
voldoende zou zijn om de interne klok van de Arduino mee te
synchroniseren. Dat laatste viel tegen. Afgaand op de waardes geleverd
door de functie `millis()` loopt de boel na een aantal uur een
kwartier achter. Nu is er een continue verbinding met het internet, en
wordt elke minuut de tijd opgevraagd. De LED heb ik een andere functie
gegeven, die gaat nu even branden op het hele en het halve uur, als een
visuele bel.

Voor temperatuur en luchtvochtigheid gebruikte ik aanvankelijk een
goedkope DHT11-sensor, nog te zien op de foto. Voor temperatuur is die
goed genoeg. Maar de waardes voor luchtvochtigheid begonnen al snel af
te wijken van de waardes van twee weerstations waarmee ik hem
vergeleek. De waardes waren zo'n 20% hoger.

Ik heb de sensor vervangen door een BME280. Duurder, nauwkeuriger, en
hij geeft ook de luchtdruk aan, dus die waarde wordt nu ook
weergegeven.


### Libraries

 * [WiFiNINA](https://www.arduino.cc/en/Reference/WiFiNINA)  → WiFiSSLClient, WiFiUDP
 * [NTPClient](https://github.com/arduino-libraries/NTPClient)  → NTPClient
 * [BME280](https://github.com/finitespace/BME280)  → sensor
 * [u8g2](https://github.com/olikraus/u8g2)  → display


### Docs

 * [Arduino Language Reference](https://www.arduino.cc/reference/en/)
 * [Arduino Nano 33 IoT](https://docs.arduino.cc/hardware/nano-33-iot)

### Display

VMA437 met chip SH1106

| display | Nano |
| ------- | ---- |
| GND     | GND  |
| VCC     | +3V3 |
| CLK (sck) | D4 |
| MOSI    | D5 |
| RES     | RESET |
| DC (a0) | D7   |
| CS      | D6   |


 * [u8g2](https://github.com/olikraus/u8g2)
     * [SH1106 128X64_NONAME](https://github.com/olikraus/u8g2/wiki/u8g2setupcpp#sh1106-128x64_noname)

```c
// SW SPI Com: SCK = 4, MOSI = 5, CS = 6, A0 = 7
U8G2_SH1106_128X64_NONAME_1_4W_SW_SPI u8g2(U8G2_R2, 4, 5, 6, 7);
```

### Sensor

VMA335 met chip BME280

| sensor | Nano |
| ------ | ---- |
| Vin    | +3V3 |
| Gnd    | GND  |
| SDA    | A4   |
| SCL    | A5   |


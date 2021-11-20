## Project 09 — Cijferloos

Van mijn vorige project had ik een sensor over die alleen de
temperatuur correct meet. Kon ik daar iets mee? Ik bedacht een
thermometer die kleurcodes gebruikt om de temperatuur weer te geven.
Nuttig? Je kunt de temperatuur van grote afstand aflezen, zonder bril.
Verder is het gewoon een leuk speeltje.

Mijn eerste idee was een RGB-LED te gebruiken, met veel verschillende
tinten. Op een beeldscherm kun je met verschillende RGB-waardes een
groot aantal goed te onderscheiden kleuren maken. Met een RGB-LED valt
dat erg tegen. Zuiver rood, groen en blauw zijn goed te onderscheiden,
maar ga je kleuren mengen, dan krijg je al snel *het is niet wit, maar
wat is het dan wel?*

Het volgende idee was om meerdere, gewone LEDs van verschillende
kleuren te gebruiken. Na wat dingen proberen ben ik uitgekomen op het
volgende. Er zijn vier LEDs, blauw, groen, geel en rood. Afhankelijk
van de temperatuur geeft een van de LEDs, om de vier seconde, één,
twee of drie korte flitsen. Hiermee kun je vrij simpel het
temperatuurbereik van tien graden tot op de graad nauwkeurig
weergeven.

| code | temperatuur | | code | temperatuur |
|------|-------|------|------|-------------|
| 1× blauw | kouder dan 16° | | 1× geel | 21° |
| 2× blauw | 16° | | 2× geel | 22° |
| 3× blauw | 17° | | 3× geel | 23° |
| 1× groen | 18° | | 1× rood | 24° |
| 2× groen | 19° | | 2× rood | 25° |
| 3× groen | 20° | | 3× rood | 26° of warmer |

Ik maakte een prototype met een Arduino Nano. Hierbij gebruikte ik de
`DHT sensor library` van Adafruit die op zijn beurt gebruikt maakt van de
`Adafruit Unified Sensor Library`. Dit werkte prima. Maar toen ik de
Nano verving door een ATtiny85 werkte het niet meer, met geen enkele
pin van de Tiny voor de sensor. Na even zoeken vond ik dat er wel
mensen waren die een DHT-sensor gebruiken met de Tiny, maar die
gebruiken een andere library, `DHTlib`. Daarmee werkt het bij mij ook,
en de code is veel kleiner.

Het was even puzzelen hoe ik alle componenten moest rangschikken op
een mini-breadboard van 170 gaten, maar dat is uiteindelijk gelukt.
Breadboard en batterijhouder (3×AAA) passen samen in een transparant
visitekaartdoosje, met nog een hoop ruimte over.

Op de foto worden de kleuren van de brandende LED en de LED ernaast
niet goed weergegeven. De LEDs van links naar rechts zijn blauw,
groen, geel, rood. Het breadboard zelf is geel, niet oranje. Een
digitale camera is niet berekend op licht van een brandende LED.

## Library

 * [DHTlib](https://github.com/RobTillaart/DHTlib)

## Sensor

 DHT11 van CNT5

## Onderdelen op breadboard

| label | type | | label | type |
|----|----|----|----|----|
| S  | DHT11 | | IC | ATtiny85 |
| D1 | blauw | | R1 | 1kΩ |
| D2 | groen | | R2 | 1kΩ |
| D3 | geel  | | R3 | 680Ω |
| D4 | rood  | | R4 | 680Ω |
| C  | 0.1µF | | R5 | 10kΩ |

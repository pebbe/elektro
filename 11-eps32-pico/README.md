
## Project 11 — ESP32: vergelijking van platforms

Ik heb een ESP32 Pico gekocht. Ik heb er nog geen toepassing voor.
Eerst wil ik de verschillende manieren waarop je hem kunt
programmeren uitproberen.

De Pico is smal genoeg om op een enkel
breadboard te gebruiken. Voor grotere ESP32's heb je twee breadboards
naast elkaar nodig.

### Tests

Met behulp van verschillende platforms maakte ik een demo met deze functies:

 * Systeeminformatie printen op seriële monitor
 * WiFi gebruiken
 * NTP gebruiken
 * Tijd omzetten van UTC naar lokale tijd
     * Tijdzone Europe/Amsterdam: `CET-1CEST,M3.5.0/2,M10.5.0/3`
     * Met tussenpauzes actuele tijd printen op seriële monitor
 * MQTT
     * Publish tijd van opstarten (lokale tijd)
     * Subscribe
     * Last will and testament
 * Twee LEDs die afwisselend knipperen
     * Snelheid instelbaar via MQTT
 * Een LED aansturen met PWM
     * Helderheid instelbaar via MQTT

#### 1. Arduino IDE

Meer [hier](arduino).

De eerste versie `demo` was snel klaar. Ik heb het programma opgezet
zoals ik gewend ben met Arduino, met een standaard `setup()` en `loop()`.
Voor NTP en MQTT heb ik de library's gebruikt die ik eerder al in
[project 08](../08-nano-iot) gebruikte.

Maar m'n code voor het omzetten van UTC naar lokale tijd kon ik
achterwege laten. De library's voor ESP kunnen overweg met tijdzones.

#### 2. Espressif

Meer [hier](espressif).

ESP-IDF is de *officiële* ontwikkelsoftware voor ESP van Espressif.
Gebruik dit als je alles uit je ESP32 wilt halen. Beheer van het
project gebeurt grotendeels via de command line.

Toen ik tutorials zocht op Youtube kwam ik al snel het begrip RTOS
tegen. Espressif maakt gebruik van (een aangepaste vorm van) FreeRTOS.
Het is de moeite waard je daarin te verdiepen. Voor simpele projecten
volstaat de gebruikelijk opzet van Arduino met `setup()` en `loop()`,
maar voor complexere projecten, waarin je ESP veel processen tegelijk
moet aansturen, volstaat dat niet. Dan wil je een RTOS gebruiken, met
z'n aparte taken, queues, en wat er nog meer bij komt kijken. Het doet
denken aan Go met z'n goroutines en channels, dus ik voelde me er direct
vertrouwd mee.

De documentatie is goed, op één ding na: er wordt bij functies niet
vermeld welke headers je moet gebruiken. Dus moet je zoeken in de
headerfiles waar een functie is gedeclareerd.

Ik bleek geen externe library's nodig te hebben. Alle functionaliteit
die ik nodig had wordt door Espressif zelf geboden.

#### 3. Arduino IDE met RTOS

Meer [hier](arduino).

Na m'n implementatie met Espressif wilde ik kijken of ik dezelfde
principes van RTOS kon toepassen in de Arduino IDE (uiteraard alleen
voor ESP). Dat werd `demo2`. Het bleek goed te doen. Ook hier bleek dat ik
in plaats van de externe library's voor NTP en MQTT gewoon Espressif
zelf kon gebruiken.

In Arduino IDE gebruik je niet de standaard library's van Espressif, maar
de library's die door Espressif voor de Arduino IDE zijn ontwikkeld.
Ik weet niet in hoeverre dit dezelfde functionaliteit biedt als de
standaardversie van Espressif. Je hebt niet de mogelijkheid om alle
parameters voor het compileerproces te tunen. Misschien is dit een
voordeel.

En je hoeft je minder zorgen te maken om het gebruik van headers. Dat
gaat voor een groot deel vanzelf.

#### 4. MicroPython

Meer [hier](micropython).

Dit werkt makkelijk. Als je MicroPython eenmaal hebt geïnstalleerd op
je ESP32 kun je met `rshell` de REPL starten of bestanden van en naar
je ESP kopiëren.

Maar het is en blijft Python. Ik ben geen fan. Dat gedoe met spaties
leidt onherroepelijk tot problemen bij grotere projecten.

Verder is de functionaliteit beperkt. Tijdzones kent MicroPython niet.
Voor MQTT moest ik een externe implementatie gebruiken. In MicroPython
is wel ondersteuning voor de protocollen die gebruikt worden in
microcontrollers, zoals I²C, SPI, UART, etc, maar daarmee kun je nog
niet elke sensor of display dat gebruik maakt van die protocollen
besturen. Je moet een implementatie hebben voor dat specifieke
apparaat, ergens een library downloaden, of zelf schrijven. 

Hetzelfde geldt natuurlijk voor Arduino, maar voor Arduino zijn al
heel veel library's geschreven. Met MicroPython weet ik dat zo niet.

#### 5. NodeMCU

Meer [hier](nodemcu).

Dit is beter dan MicroPython. Hetzelfde idee, maar met Lua als
programmeertaal in plaats van Python. Dat is natuurlijk een groot
voordeel.

Alle functionaliteit die ik voor dit project nodig had is aanwezig in
NodeMCU. Inclusief NTP, lokale tijd via tijdzones, en MQTT.

Er is niet één kernel van NodeMCU voor je ESP. Je compileert er zelf
een nadat je hebt besloten welke modules je wilt gebruiken. Daardoor
kan NodeMCU veel modules aanbieden, terwijl de uiteindelijke kernel
niet groter wordt dan nodig. Een groot voordeel op een apparaat waar
het geheugen zeer beperkt is.

#### 6. Lua RTOS

Meer [hier](luartos).

Een andere variant van Lua voor de ESP. Het gebruik is net iets
simpeler dan met NodeMCU, maar dat komt doordat er maar één kernel is.
En omdat alles wat Lua RTOS te bieden heeft in die ene kernel moet is
er uiteraard een kleiner aanbod van modules. 

Lua RTOS ondersteunt geen tijdzones, dus hiervoor moest ik weer m'n
eigen code bakken.

MQTT ondersteunt geen *last will and testament*.

#### 7. Mongoose OS

Meer [hier](mongooseos).

Mongoose OS is een ramp. Ergens had ik gelezen dat dit het ideale OS voor
de ESP32 zou moeten zijn. Niet dus. Je kunt programmaatjes schrijven
in iets wat lijkt op JavaScript, maar waarin je niet eens twee strings
met elkaar kunt vergelijken. Het is me niet gelukt mijn demo in
Mongoose te implementeren.

Bovendien is er geen REPL, dus de helft van motivatie om een
geïnterpreteerde taal te gebruiken ontbreekt.

Je kunt in Mongoose OS ook in C programmeren, maar waarom zou je?
Dan heb je geen REPL en geen snelle uploads van scripts.

#### 8. Zephyr

Wat is [Zephyr](https://www.zephyrproject.org/)? *The Zephyr® Project
strives to deliver the best-in-class RTOS for connected
resource-constrained devices, built to be secure and safe.* Het is
vooral een ton aan software die je moet downloaden, waarbij weinig
gaat zoals de handleiding veronderstelt, voordat je in staat bent om
het simpelste programmaatje te compileren (een blink-programma), dat
dan ook nog niet lukt. Een *hello world*, dat kreeg ik wel aan de
praat.

Uiteindelijk komt het neer op het schrijven van een programma in C,
waarna je een build doet, en dan alles naar je apparaat moet flashen.
Net als Arduino IDE en Espressif, maar dan veel ingewikkelder.

Wat is het doel? Standaardisatie? Iets dat op alle IoT-apparaten zou
moeten draaien? Dat lijkt mij een illusie. Met microcontrollers en zo
wil je geen ballast. 

#### 9. CircuitPython

Ik had het vage idee dat CircuitPython beter (verder) ontwikkeld zou
zijn dan MicroPython (de eerste is immers niet voor niets een vork van
de laatste), dus wilde ik kijken wat CircuitPython te bieden heeft.

CircuitPython wordt niet ondersteund voor de ESP32. Wel voor de
ESP32-S2. Zou ik een kernel voor de ESP32-S2 op de ESP32 kunnen laten
draaien? Mij is het niet gelukt. Bij mijn pogingen om het voor elkaar
te krijgen raakte ik verstrikt in de documentatie, die extreem slecht
georganiseerd en verwarrend is. 

Ik denk niet dat als ik eenmaal een board heb die wel door
CircuitPython wordt ondersteund, om dan alsnog CircuitPython te
gebruiken.

Het is en blijft toch Python.

### Conclusies

Welk platform kun je nu het best gebruiken voor het programmeren van
de ESP32? Wat mij betreft blijven er twee duidelijke favorieten over.

**Arduino IDE**

Voor kleine of grote eindtoepassing gebruik je de Arduino IDE.

Je kunt
je programma's op twee manieren vorm geven. De traditionele
Arduino-manier, met z'n setup en loop. Dit is alleen geschikt voor
simpele dingen, maar je kunt het wel makkelijker exporteren naar
andere apparaten.

Of je gebruikt de FreeRTOS in Espressif, wat simpele
dingen een klein beetje complexer maakt (hoewel dat meevalt), en
grotere projecten een stuk handelbaarder. Je programma exporteren naar
een ander soort board wordt dan wel ingewikkeld.

Het nadeel van het werken met de Arduino IDE (of direct met Espressif) is dat je voor
elke verandering in de code die je wilt testen het complete programma
moet compileren en flashen naar je apparaat. Dat is tijdrovend. En het
is niet goed voor je apparaat om heel vaak een groot deel van het
flashgeheugen te overschrijven.

**NodeMCU**

Voor graduele ontwikkeling, dingen testen, met kleine stukjes scripts
werken, direct commando's naar je board sturen, voor dit alles is een
scripttaal en een REPL ideaal, en wat mij betreft is NodeMCU de
duidelijke winnaar.

Behalve de snellere ontwikkeling met NodeMCU (in vergelijking met
Arduino IDE) levert dit programmacode op die een stuk compacter en
leesbaarder is dan de C++-code voor Arduino. Als je een complete
toepassing voor NodeMCU hebt geschreven, en het werkt prima, en
snelheid is niet echt een issue, dan kun je besluiten geen C++-versie
te maken, maar gewoon de Lua-versie te blijven gebruiken.

Het is ook veel sneller om een Lua-programma te vervangen dan een
C++-programma te moeten compileren en flashen.


### Vervolg

Mijn testtoepassing is erg simpel.

Volstaat voor optimaal programmeren van de ESP32 de Arduino IDE, of
heb je voor sommige dingen toch de ontwikkelomgeving van Espressif nodig?

Lua kent coroutines. Hoe gebruik je die effectief in NodeMCU?

Wordt NodeMCU actief doorontwikkeld? Hoe zit het met ondersteuning
voor verschillende soorten displays en sensors en zo in NodeMCU? 

Dat laatste is iets wat ik in de toekomst wil testen.


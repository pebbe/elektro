## Project 05 — Voeding

Schakelingen met ICs werken gewoonlijk op 5 volt. Die kun je dus niet
direct op een batterij aansluiten. Er zijn wel converters en ICs
waarmee je een voltage kunt verhogen of verlagen. Met een IC moeten er
gewoonlijk nog wat componenten bij om de schakeling compleet te
maken.

Wat voor batterijen kun je het best gebruiken? Welke invoerspanning?
Hieronder een overzicht van de verschillen in capaciteit van een
aantal opties.

Capaciteit volgens [Wikipedia](https://nl.wikipedia.org/wiki/Batterij_(elektrisch)#Modellen)

| spanning | batterij | capaciteit |
| --------:|:--------:|-----------:|
| 1,5 V    | AAA      |  1.500 mAh |
|          | AA       |  3.330 mAh |
|          | C        | 10.000 mAh |
|          | D        | 18.000 mAh |
| 3 V      | 2× AAA   |  3.000 mAh |
|          | 2× AA    |  6.660 mAh |
|          | 2× C     | 20.000 mAh |
|          | 2× D     | 36.000 mAh |
| 9 V      | blokje   |  1.000 mAh |

Op de [Engelse Wikipediapagina](https://en.wikipedia.org/wiki/List_of_battery_sizes#Cylindrical_batteries) worden (veel) lagere waardes gegeven.

Geen enkele omzetting van voltage is zonder verlies, en bij de ene
omzetting is het verlies groter dan bij de andere. Maar hier heb ik me
niet door laten leiden.

Ik heb gekozen voor twee AA-batterijen, bijna even compact als een
blokje van 9 volt, maar met een veel grotere capaciteit.
Bovendien
vond ik een IC voor de omzetting die blijft werken tot de
invoerspanning is gedaald tot 1,8 volt. Je kunt hiermee de batterijen
dus aardig leeg trekken.

Benodigde onderdelen:

----

### 3 V → 5 V


| onderdeel      | | type       | prijs  |
|----------------|-|:----------:|-------:|
| IC             | | LT1301CN8  | € 6,90 |
| transistor     | | 2N4403     | € 0,45 |
| Schottky diode | | 1N5817     | € 0,45 |
| spoel          | | 22µH       | € 3,45 |
| condensator    | 2× | 100 µF  |        |
| condensator    | | 0,1 µF     |        |
| weerstand      | | 470 Ω      |        |

Minimum invoerspanning 1,8 V

Uitvoer 5 V, 200 mA (met 2× AA als voeding)

----

In het schema is de SELECT-pin niet aangesoten, maar in de tekst van de
datasheet staat dat je die met aarde (GND) kunt verbinden. Dat heb ik
maar gedaan. Verbind je de SELECT-pin met de plus, dan is de uitvoer
niet 5 maar 12 volt.

Okaphone had een mooie dichte behuizing voor twee AAs, inclusief
schakelaar, ook een reden om deze optie te kiezen.

De datasheet
raadt aan een spoel met een weerstand van minder dan 0,03Ω te gebruiken.
Maar ik had er een nodig die op een breadboard kan, en die was alleen
beschikbaar van 0,09Ω. Dat zou minder efficiënt zijn, maar het werkt.

Voor het eerst heb ik een mini-breadboard gebruikt. Dus zonder die
comfortabele voedingsrails over de lengte. Dat maakte het een
gepriegel om uit te vinden hoe de componenten het beste geplaatst
konden worden.

Het resultaat is een voeding die op twee AA-batterijen 5,15 volt
levert.

Maar het rendement valt vies tegen. Gebruik ik de voeding voor m'n
morse-schakeling dan houdt die het maar een dag vol. Met alle LEDs uit
verbruikt die schakeling zo'n 22 mA, maar de voeding verbruikt dan 76
mA. De schakeling met de ATtiny verbuikt 8.1 mA, maar de voeding
verbruikt daarvoor 25 mA. M'n voedingsschakeling heeft dus een
rendement van niet meer dan rond de 30%. Volgens de grafiek uit de
datasheet zou het rendement tussen 83% en 87% moeten liggen.


Hieronder de andere opties die ik bekeken heb.

----

### 1,5 V → 5 V

| onderdeel      | type      | prijs  |
|----------------|:---------:|-------:|
| IC             | MAX756CPA | € 7,50 |
| Schottky diode | 1N5817    | € 0,45 |
| spoel          | 22 µH     | € 3,45 |
| condensator    | 150 µF    |        |
| condensator    | 100 µF    |        |
| condensator    | 0,1 µF    |        |


Invoerspanning 0,7 V tot 1,8 V

Uitvoer  5 V, 200 mA

----

### 9 V → 5 V

| onderdeel         | type    | prijs  |
|-------------------|:-------:|-------:|
| spanningsregelaar | 7805CV  | € 0,85 |
| condensator       | 0,33 µF |        |
| condensator       | 0,1 µF  |        |

Invoerspanning 7 V tot 25 V

Uitvoer 500 mA ?

----

### 9 V → 5 V

| onderdeel | type | prijs |
|-------------------|:-------:|-------:|
| IC | LT1054CP | € 3,95 |
| condensator       | 100 µF  |        |
| condensator       | 10 µF  |        |
| condensator       | 2 µF  |        |
| condensator       | 0,002 µF  |        |
| weerstand | ? | |
| weerstand | ? | |

Zie figuur 15 in datasheet.

Invoerspanning 3,5 V tot 15 V

Uitvoer -5 V, 100 mA

Wat betekent dat, -5 V uitvoer?

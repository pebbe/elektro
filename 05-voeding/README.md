## Project 05 — Voeding

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

----

### Batterijhouders

AAA
: [houder met soldeerlippen](https://www.okaphone.com/artikel.asp?id=436527)

AA
: [houder met aansluitdraden](https://www.okaphone.com/artikel.asp?id=460155)

C
: [houder met soldeerlippen](https://www.okaphone.com/artikel.asp?id=1357)

D
: [houder met soldeerlippen](https://www.okaphone.com/artikel.asp?id=1351)

2× AAA
: [houder met aansluitdraden](https://www.okaphone.com/artikel.asp?id=469708)
: [houder met schakelaar en en JST connector](https://www.okaphone.com/artikel.asp?id=488753)

2× AA
: [houder met aansluitdraden](https://www.okaphone.com/artikel.asp?id=455909)
: [houder met schakelaar en draden](https://www.okaphone.com/artikel.asp?id=456159)

2× C
: [houder met snap-aansluiting](https://www.okaphone.com/artikel.asp?id=1358)

2× D
: [houder met snap-aansluiting](https://www.okaphone.com/artikel.asp?id=1354)

blokje
: [batterijclip](https://www.okaphone.com/artikel.asp?id=435836)
: [houder met aansluitdraden](https://www.okaphone.com/artikel.asp?id=435562)

Past [deze batterijclip](https://www.okaphone.com/artikel.asp?id=435836) op de houders voor 2× C en 2× D?

----

### 1,5 V → 5 V

| onderdeel      | type      | prijs  |
|----------------|:---------:|-------:|
| IC             | MAX756CPA | € 7,50 |
| Schottky diode | 1N5817    | € 0,45 |
| [spoel](https://www.okaphone.com/artikel.asp?id=463594)          | 22 µH     | € 3,45 |
| condensator    | 150 µF    |        |
| condensator    | 100 µF    |        |
| condensator    | 0,1 µF    |        |


Invoerspanning 0,7 V tot 1,8 V

Uitvoer  5 V, 200 mA

Spoel zou weerstand van 0,03 Ω of minder moeten hebben:

 * 22 μH, ≤ 410 mA, 0,84 Ω — € 0,50
 * 22 μH, ≤ 1,08 A, 0,09 Ω — € 3,45
 * 22 μH, ≤ 1,8 A, 0,051 Ω — € 1,85 — (te?) korte pootjes
 * 22 μH, ≤ 11 A, 0,012 Ω — € 8,95 — (te?) korte pootjes, niet op voorraad

----

### 3 V → 5 V

| onderdeel      | | type       | prijs  |
|----------------|-|:----------:|-------:|
| IC             | | LT1301CN8  | € 6,90 |
| transistor     | | 2N4403     | € 0,45 |
| Schottky diode | | 1N5817     | € 0,45 |
|  [spoel](https://www.okaphone.com/artikel.asp?id=463594) | | 10–22µH    | € 3,45 |
| condensator    | 2× | 100 µF  |        |
| condensator    | | 0,1 µF     |        |
| weerstand      | | 470 Ω      |        |

Minimum invoerspanning 1,8 V

Uitvoer 5 V, 200 mA (met 2× AA als voeding)

Spoel zou weerstand van 0,03 Ω of minder moeten hebben:

 * 10 μH, ≤ 680 mA, 0,49 Ω — € 0,50
 * 10 μH, ≤ 2,5 A, 0,027 Ω — € 1,85 — (te?) korte pootjes
 * 22 μH, ≤ 1,08 A, 0,09 Ω — € 3,45

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

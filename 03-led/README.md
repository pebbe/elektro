## Project 03 — LED-waardes

Ik wilde bepalen hoeveel vermogen verschillend gekleurde LEDs
verbruiken, met de vraag, welke voorschakelweerstand zou je moeten
gebruiken bij een spanning van 5 volt.

Ik heb getest met drie verschillende weerstanden, van 220, 470 en 680
ohm. Ik heb de weerstanden gemeten, en de waardes wijken ietsje af.

Met LED en weerstand in serie heb ik het stroomverbruik gemeten.

Daarna heb ik de volgende waardes berekend:

 * De spanning over de weerstand is weerstand maal stroomsterkte
 * De spanning over de LED is 5V min spanning over de weerstand
 * Het gebruikte vermogen van de LED is stroomsterkte maal spanning
  over de LED

De resultaten staan in onderstaande tabel:


kleur | R (Ω) | I (mA) | U<sub>R</sub> (V) | U<sub>D</sub> (V) | P<sub>D</subd (mW)
------|-------|--------|-------------------|-------------------|-------------------
rood  | 217 | 13.4 |  2.91 | 2.09 | 28.0
..    | 467 | 6.48 |  3.03 | 1.97 | 12.8
..    | 671 | 4.56 |  3.06 | 1.94 | 8.85
geel  | 217 | 13.0 |  2.82 | 2.18 | 28.3
..    | 467 | 6.28 |  2.93 | 2.07 | 13.0
..    | 671 | 4.41 |  2.96 | 2.04 | 9.00
groen | 217 | 9.32 |  2.02 | 2.98 | 27.8
..    | 467 | 4.87 |  2.27 | 2.73 | 13.3
..    | 671 | 3.53 |  2.37 | 2.63 | 9.28
blauw | 217 | 9.31 |  2.02 | 2.98 | 27.7
..    | 467 | 4.70 |  2.19 | 2.81 | 13.2
..    | 671 | 3.35 |  2.25 | 2.75 | 9.21
wit   | 217 | 9.94 |  2.16 | 2.84 | 28.2
..    | 467 | 4.85 |  2.26 | 2.74 | 13.3
..    | 671 | 3.43 |  2.30 | 2.70 | 9.27

De vraag is natuurlijk: hoeveel vermogen kan een LED verdragen? In
schakelingen zie ik vaak 220 ohm gebruikt als voorschakelweerstand.
Maar gebruik je 470 ohm, dan gaat het vermogen met meer dan de helft
omlaag, en voor het oog brandt de LED vrijwel niet zwakker.

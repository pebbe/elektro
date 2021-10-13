## Project 04 — Flitsende Joule Thief

Verder prutsen aan de Joule Thief (zie ook
[04](../04-joule-thief/README.md) en
[06](../06-solderen/README.md)).

De printplaatversie bleek het toch te doen (zwak) als ik een extra
condensator over de al aanwezige condensator hield. Condensator stuk?
Oude er uit gehaald om er een nieuwe in te zetten. Hij deed niks.
Beide condensatoren samen. Weer niets. Het blijft mij een raadsel.

De boel weer op een breadboard gezet. Werkte prima.

Hoewel de Joule Thief zou moeten werken zonder condensator krijg ik 'm
zonder niet aan het branden. Ik heb nog wat andere types transistoren
geprobeerd. Andere weerstanden. Niks. Met sommige transistoren deed ie
het ook niet met condensator.

Terug naar de beginopstelling. Ik kwam er achter dat m'n multimeter
ook frequenties kan meten. Dus heb ik eens gemeten hoe snel de
transistor (BC547B) schakelt met verschillende weerstanden en
condensatoren. Het bleek ook verschil te maken wat voor soort LED ik
gebruik.

| LED | R | C | frequentie  |
| ---- | ----: | ----: | ----: |
| rood | 10kΩ |  1nF | 7,3MHz |
| wit  | 10kΩ |  1nF | 8,0MHz |
| wit  |  1kΩ | 1nF | 5,4MHz |
| rood |  1kΩ | 560pF | 7,4MHz |
| wit (zwak) | 1kΩ | 560pF | 7,4MHz |
| wit | 1kΩ | 10nF | 4,6 MHz |

Dat is zijn zeer hoge frequenties, een factor 10 groter dan wat hier
en daar in documentatie wordt genoemd. Ligt het aan m'n type
transistor? Aan m'n spoel?

Je kunt die frequenties omlaag brengen met een zware weerstand, en met
een zware condensator erbij. Dan brandt de Joule Thief niet meer
constant, maar laat korte flitsen zien, met korte of langere pauzes
ertussen.

| LED | R | C | periode  |
| ---- | ----: | ----: | ----: |
| rood | 1MΩ |  22μF | 4,4s |
| wit  | 1MΩ |  22μF | 10,3s |

Ik heb nu een flitsende Joule Thief met een rode LED, die dag en nacht
aan staat. Ik ben benieuwd hoe lang die het blijft doen. 
[Eman2000](https://www.youtube.com/channel/UCElHlMVHMMpixDtpTxipGog) op
YouTube heeft
[een exemplaar dat het al vijf jaar volhoudt](https://www.youtube.com/watch?v=AWDX3L07qyc),
waarbij de spanning van de batterij is teruggelopen van 1,6 volt naar
1,35 volt.

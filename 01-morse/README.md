## Project 01 — Morse

Dit is m'n eerste project op een breadboard.

De schakeling stuurt LEDs aan voor het tonen van berichten
gecodeerd als reeksen van lichtsignalen. De eerste versie stuurde één
LED aan die een SOS uitzond.

De uiteindelijke versie heeft zeven LEDs waarmee signaalreeksen van maximaal
128 stappen weergegeven kunnen worden. Er is plaats voor 64
verschillende reeksen, die geselecteerd kunnen worden met
DIP-schakelaars.

De schakeling bestaat uit de volgende delen:

 1. Een timer rond de 555-chip, naar het voorbeeld van Ben Eater dat hij
    maakte voor zijn 8-bit computer. Zie
    [Clock module](https://eater.net/8bit/clock). Ik gebruik alleen het
    automatische deel. Met de potmeter is de frequentie van de timer te
    variëren van ongeveer 1Hz tot 21Hz.
 2. De timer stuurt een teller van 7 bits aan. Deze telt dus tot en met
    127, en springt daarna weer op nul.
 3. De teller stuurt de eerste zeven adreslijnen van een EEPROM aan, die
    beschikt over dertien adreslijnen. De overige zes adreslijnen worden
    bediend met DIP-schakelaars. <BR>
    Van de 8bit uitvoer van de EEPROM wordt de eerste bit gebruikt om
    de teller terug naar nul te zetten. De overige zeven bits gaan
    naar zeven LEDs.
    
De EEPROM wordt geprogrammeerd met een Arduino Nano, naar het
voorbeeld van Ben Eater. Zie
[Build an Arduino EEPROM programmer](https://www.youtube.com/watch?v=K88pgWhEb1M).
Ben Eater gebruikte een EEPROM met elf adreslijnen. Die chip is niet
meer leverbaar. Ik gebruik de opvolger met dertien adreslijnen.
Afgezien van twee extra adreslijnen, en de iets andere ordening van de
pinnen, is mijn programmeermodule gelijk aan het voorbeeld.


In het geheugen is plaats voor 64 patronen van elk maximaal 128
stappen. De bits 2 t/m 7 van elke stap sturen de zeven LEDs aan. Een
patroon dat korter is dan 128 stappen wordt afgesloten met een byte
waarvan alleen het eerste bit gezet is. Hiermee wordt de teller teruggezet
naar de waarde nul. De resterende van de 128 bytes krijgen dezelfde
waarde, zodat bij het schakelen van een lang patroon naar een kort
patroon, waarbij de teller al voorbij de lengte van het korte patroon
is, altijd terug gesprongen wordt naar het begin.

De DIP-schakelaars staan ondersteboven, omdat wanneer een schakelaar op
aan staat dat leidt tot een nul op de EEPROM. 

Aanvankelijk werkte het niet. Ik kreeg rare signalen van de LEDs te
zien, niet het patroon dat ik geprogrammeerd had. Ik gebruikte toen
nog maar vier adreslijnen van de EEPROM. De overige moesten op aarde
aangesloten worden, maar in de kluwen van draden had ik er eentje over
het hoofd gezien. Een adreslijn die niet is aangesloten kan blijkbaar
willekeurige waardes aannemen. Toen ik dit probleem had verholpen
werkte het precies zoals ik wilde. 

Ik ben tevreden over het resultaat. Het is een propvol breadboard
geworden. De potmeter en de DIP-schakelaars hebben weinig grip, en
moet je goed tegenhouden bij de bediening, anders vallen ze eruit.
Misschien kan ik daar nog wat op verzinnen. 

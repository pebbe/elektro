## Project 02 — Tiny

Dit is m'n eerste experiment met een microcontroller-chip, de **ATtiny85**.

Op de foto, op de rechter helft van het breadboard zie je de uiteindelijke
schakeling van de ATtiny85 die drie LEDs aanstuurt. Op de linker helft
zie je een **Arduino Nano**, inclusief drie controle-LEDs, waarmee de tiny
is geprogrammeerd. De Nano is nu losgekoppeld van de tiny. (Bij dit
breadboard zijn de powerrails halverwege onderbroken.)

Je kunt de ATtiny85 heel simpel programmeren met een Arduino. In de
IDE van Arduino zit hiervoor bij de voorbeelden een kant-en-klare
sketch `ArduinoISP`. Gewoon alles aansluiten volgens de
documentatie... wat toch niet vanzelf ging.

Je wordt aangeraden om de Arduino met de tiny te verbinden vanaf de
zes ICSP/SPI-pinnen, die je op de Nano rechts ziet. Ik had wel een datasheet
van de Nano, waarin ook de zes ICSP/SPI-pinnen staan vermeld in een schema, maar ik had
niet door dat dat schema de onderkant van de Nano weergeeft. Ik keek
naar de bovenkant, en had daarom de volgorde van de pinnen verkeerd
om. Dat werkte dus niet. Omdat ik dit nog niet doorhad heb ik de oude
manier van aansluiten gebruikt, via de standaard datapinnen, zoals je
op de foto kunt zien. Hiervoor moet je in de sketch
`USE_OLD_STYLE_WIRING` definiëren.

Ook was me niet direct duidelijk wat bedoeld werd met *Pin 10 is used
to reset the target microcontroller*. Niet de fysieke pin 10 van de
Arduino, dat was me direct duidelijk. Maar welke dan wel? Bedoeld werd
de pin die met `D10` is gemarkeerd.

Nadat de sketch `ArduinoISP` geladen is op de Nano moet je een
condensator van 10µF zetten tussen GND (-) en RST (+) van de Arduino.
Dit wordt niet vermeld in de documentatie in de sketch, maar moet wel,
volgens de diverse video's hierover die op Youtube zijn te vinden
(zoals [deze](https://www.youtube.com/watch?v=gXXdoeu7yWw)).

Dan, in de Arduino IDE moet je de
[ATtiny Core](https://github.com/SpenceKonde/ATTinyCore) installeren.
Vervolgens onder `Hulpmiddelen` de volgens instellingen kiezen:

 * Board: ATtiny25/45/85 (No bootloader)
 * Chip: ATtiny85
 * Clock: 8 MHz (internal)
 * Programmer: Arduino as ISP (ATTinyCore)
 
Daarna, ook onder `Hulpmiddelen` kies je `Bootloader branden`.

Hier ging het bij mij eerst mis. Ik kreeg de gevreesde *Device signature = 0x000000
Yikes!  Invalid device signature*, het signaal dat je iets verkeerd
hebt aangesloten. Eenmaal opgelost kun je een sketch uploaden vanuit
de Arduino IDE naar de tiny, precies zoals je anders een sketch naar
een Arduino stuurt.

Als je klaar bent, en de IDE weer wilt gebruiken om een Arduino te
programmeren, dan moet je de instellingen herstellen. Voor de Nano:

 * Board: Arduino Nano
 * Processor: ATmega328P
 * Programmer: AVRISP mkII
 
Om het gebruik van de tiny te testen wilde ik iets interessanters dan
de standaard Blink. De tiny heeft drie pinnen die pulse-width
modulation (PWM) ondersteunen. Hiermee kun je LEDs aansturen en de
lichtsterkte variëren. Ik heb een sketch gemaakt dat op willekeurige
momenten, een willekeurige LED laat oplichten, met een willekeurige
lichtintensiteit, waarna de LED langzaam uitdooft. 

Voor het normale gebruik van de tiny zit er een pull-upweerstand van
10kΩ op de reset-pin, die je kunt laten zitten tijdens het
programmeren. Tussen Vcc en GND zit een condensator van 0.1µF om de
spanning te stabiliseren.

De schakeling wordt gevoed met 5 volt, maar blijkt ook op 3 volt te
werken, al branden de LEDs dan een stuk zwakker.

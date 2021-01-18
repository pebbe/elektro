#define SHIFT_DATA 2
#define SHIFT_CLK 3
#define SHIFT_LATCH 4
#define EEPROM_D0 5
#define EEPROM_D7 12
#define WRITE_EN 13

/*
 * Output the address bits and outputEnable signal using shift registers.
 */
void setAddress(int address, bool outputEnable) {

    shiftOut(SHIFT_DATA, SHIFT_CLK, MSBFIRST, (address >> 8) | (outputEnable ? 0x00 : 0x80));
    shiftOut(SHIFT_DATA, SHIFT_CLK, MSBFIRST, address);

    digitalWrite(SHIFT_LATCH, LOW);
    digitalWrite(SHIFT_LATCH, HIGH);
    digitalWrite(SHIFT_LATCH, LOW);
}

/*
 * Write a byte to the EEPROM at the specified address.
 */
void writeEEPROM(int address, byte data) {
    setAddress(address, /*outputEnable*/ false);
    for (int pin = EEPROM_D0; pin <= EEPROM_D7; pin += 1) {
        pinMode(pin, OUTPUT);
    }

    for (int pin = EEPROM_D0; pin <= EEPROM_D7; pin += 1) {
        digitalWrite(pin, data & 1);
        data = data >> 1;
    }
    digitalWrite(WRITE_EN, LOW);
    delayMicroseconds(1);
    digitalWrite(WRITE_EN, HIGH);
    delay(10);
}

void setup() {
    // put your setup code here, to run once:

    pinMode(SHIFT_DATA, OUTPUT);
    pinMode(SHIFT_CLK, OUTPUT);
    pinMode(SHIFT_LATCH, OUTPUT);
    digitalWrite(WRITE_EN, HIGH);
    pinMode(WRITE_EN, OUTPUT);
    Serial.begin(57600);

    // Program data bytes
    Serial.print("Setting default patterns on EEPROM");
    for (int address = 0; address < 8192; address += 1) {
        byte b;
        switch (address & 0x7F) {
        case 0:  b =   2; break;
        case 1:  b =   4; break;
        case 2:  b =   8; break;
        case 3:  b =  16; break;
        case 4:  b =  32; break;
        case 5:  b =  64; break;
        case 6:  b = 128; break;
        default: b =   1;
        }
        writeEEPROM(address, b);
        if (address % 256 == 0) {
            Serial.print(".");
        }
    }
    Serial.println(" done");
}

void loop() {
    // put your main code here, to run repeatedly:

}

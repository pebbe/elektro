#define SHIFT_DATA 2
#define SHIFT_CLK 3
#define SHIFT_LATCH 4
#define EEPROM_D0 5
#define EEPROM_D7 12
#define WRITE_EN 13

#define PATTERN 32

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
 * Read a byte from the EEPROM at the specified address.
 */
byte readEEPROM(int address) {
    for (int pin = EEPROM_D0; pin <= EEPROM_D7; pin += 1) {
	pinMode(pin, INPUT);
    }
    setAddress(address, /*outputEnable*/ true);

    byte data = 0;
    for (int pin = EEPROM_D7; pin >= EEPROM_D0; pin -= 1) {
	data = (data << 1) + digitalRead(pin);
    }
    return data;
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

#if PATTERN == 0

byte data[] = {
	       0b00000100,
	       0b00000000,
	       0b00000100,
	       0b00010000,
	       0b00010100,
	       0b00010000,
	       0b01010000,
	       0b01010000,
	       0b01010100,
	       0b01010100,
	       0b01010100,
	       0b01010000,
	       0b01010100,
	       0b01000100,
	       0b01000100,
	       0b01000000,
	       0b00000100,
	       0b00000100,
	       0b00000100,
	       0b00000000,
	       0b00000000,
	       0b00000000,
	       0b00000100,
	       0b00010000,
	       0b00010100,
	       0b00010000,
	       0b01010100,
	       0b01010000,
	       0b01010000,
	       0b01010000,
	       0b01010000,
	       0b01010000,
	       0b01010000,
	       0b01000000,
	       0b01000000,
	       0b01000000,
	       0b00000000,
	       0b00000000,
	       0b00000000,
	       0b00000000
};

#endif

#if PATTERN == 1

byte data[] = {
	       0b10,
	       0b00
};

#endif

#if PATTERN == 2

byte data[] = {
	       0b100,
	       0b000
};

#endif

#if PATTERN == 4

byte data[] = {
	       0b1000,
	       0b0000
};

#endif

#if PATTERN == 8

byte data[] = {
	       0b10000,
	       0b00000
};

#endif

#if PATTERN == 16

byte data[] = {
	       0b100000,
	       0b000000
};

#endif

#if PATTERN == 32

byte data[] = {
	       0b1000000,
	       0b0000000
};

#endif

/*
 * Read the contents of the EEPROM and print them to the serial monitor.
 */
void printContents() {
    for (int base = PATTERN << 7; base < (PATTERN << 7) + 128; base += 16) {
	byte data[16];
	for (int offset = 0; offset <= 15; offset += 1) {
	    data[offset] = readEEPROM(base + offset);
	}

	char buf[80];
	sprintf(buf, "%03x:  %02x %02x %02x %02x %02x %02x %02x %02x   %02x %02x %02x %02x %02x %02x %02x %02x",
		base, data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7],
		data[8], data[9], data[10], data[11], data[12], data[13], data[14], data[15]);
	Serial.println(buf);
    }
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
    Serial.print("Programming EEPROM");
    for (int address = 0; address < 128; address += 1) {
        writeEEPROM(address + (PATTERN << 7), address < sizeof(data) ? data[address] : 1);
        if (address % 16 == 0) {
            Serial.print(".");
        }
    }
    Serial.println(" done");

    // Read and print out the contents of the EERPROM
    Serial.println("Reading EEPROM");
    printContents();
}


void loop() {
    // put your main code here, to run repeatedly:

}

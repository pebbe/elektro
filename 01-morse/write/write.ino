#define SHIFT_DATA 2
#define SHIFT_CLK 3
#define SHIFT_LATCH 4
#define EEPROM_D0 5
#define EEPROM_D7 12
#define WRITE_EN 13

#define PATTERN 56

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

#if PATTERN == 3

byte data[] = {
        4, 28, 46, 58, 126, 26, 112, 140, 46, 92, 224, 72, 150, 56,
        90, 184, 62, 178, 72, 160, 154, 76, 206, 98, 10, 230, 90,
        240, 132, 136, 114, 46, 168, 224, 66, 26, 70, 252, 180, 2,
        218, 26, 246, 70, 108, 234, 8, 184, 166, 200, 104, 122, 90,
        78, 46, 36, 104, 114, 82, 16, 154, 178, 174, 36, 206, 164,
        202, 6, 204, 238, 194, 20, 110, 86, 204, 6, 6, 28, 136, 42,
        140, 78, 104, 38, 64, 172, 226, 218, 196, 152, 136, 52, 40,
        114, 142, 112, 212, 172, 168, 244, 32, 216, 120, 200, 102,
        244, 182, 82, 252, 16, 22, 98, 146, 54, 238, 124, 2, 26, 214,
        208, 28, 122, 150, 218, 252, 24, 176, 116
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

#if PATTERN == 48

byte data[] = {
	       0b0000000,
	       0b0000000
};

#endif

#if PATTERN == 56

byte data[] = {
	       0b11111110,
	       0b11111111
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

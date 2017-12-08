#include <Arduino.h>

#include <OneWire.h>


#define RELAY_ON_TIME 4000
#define OFF_TIME 3000
#define READ_DELAY 5

#define SERIAL_LENGTH 6

#define CODE_NO_INPUT 0
#define CODE_INVALID_CRC 1
#define CODE_INVALID_FAMILY 2
#define CODE_SUCCESS 3

#define PIN_RELAY 13
#define PIN_1WIRE 6

OneWire  ds(PIN_1WIRE);
byte addr[1 + SERIAL_LENGTH + 1];  // 8 bit CRC, 48 bit serial number, 8 bit family code

byte valid_key[SERIAL_LENGTH] = {0xa3, 0x78, 0x69, 0x00, 0x00, 0x00};

byte get_key_code(byte *key);
bool is_valid(byte key[8]);


void setup(void) {
    Serial.begin(9600);
    pinMode(PIN_RELAY, OUTPUT);
    digitalWrite(PIN_RELAY, HIGH);
}

void loop(void) {
    byte input_key[SERIAL_LENGTH];
    byte result_code = get_key_code(input_key);

    if(result_code == CODE_SUCCESS) {
        byte i;
        for(i = SERIAL_LENGTH; i > 0; i--) {
            Serial.print(" ");
            Serial.print(input_key[i-1], HEX);
        }
        Serial.println("");
        if(is_valid(input_key)) {
            Serial.println("valid key");
            digitalWrite(PIN_RELAY, LOW);
            delay(RELAY_ON_TIME);
            digitalWrite(PIN_RELAY, HIGH);
            delay(OFF_TIME);
        } else {
            Serial.println("invalid key");
        }
    }
    else if (result_code == CODE_INVALID_CRC) {
        Serial.println("invalid crc");
    }
    else if (result_code == CODE_INVALID_FAMILY) {
        Serial.println("invalid family");
    }

    delay(READ_DELAY);
}

byte get_key_code(byte *key) {
    if ( !ds.search(addr)) {
        ds.reset_search();
        return CODE_NO_INPUT;
    }

    // check crc
    if ( OneWire::crc8( addr, 7) != addr[7]) {
        return CODE_INVALID_CRC;
    }

    // check family code
    if ( addr[0] != 0x18) {
        return CODE_INVALID_FAMILY;
    }

//    ds.reset();
//    ds.reset_search();
    memcpy(key, addr + 1*sizeof(byte), SERIAL_LENGTH * sizeof(byte));

    /*
    // READ MEMORY
    ds.write(0xf0); // read memory command
    ds.write(0x00); // TA1
    ds.write(0x00); // TA2

#define BUF_LEN (32*22)
    uint8_t buf[BUF_LEN];
    ds.read_bytes(buf, BUF_LEN);
    int i = 0;
    while(i < BUF_LEN) {
        Serial.print("0x");
        Serial.print(i, HEX);
        Serial.print(" to 0x");
        Serial.print(i+31, HEX);
        Serial.print(":");
        int j;
        for(j = 0; j < 32; j++) {
            Serial.print(" 0x");
            Serial.print(buf[i++], HEX);
        }
        Serial.println();
    }
*/

    // read PRNG counter
    ds.write(0xf0); // read memory command
    ds.write(0xa0); // TA1
    ds.write(0x02); // TA2

    uint8_t buf[4];
    ds.read_bytes(buf, 4);
    Serial.print("PRNG counter:");
    int j;
    for(j = 0; j < 4; j++) {
        Serial.print(" 0x");
        Serial.print(buf[j], HEX);
    }
    Serial.println();


    ds.reset();

    ds.write(0xa5); // resume

    // compute challenge
    uint8_t commands[4];
    commands[0] = 0x33; // compute SHA
    commands[1] = 0xa0; // TA1
    commands[2] = 0x02; // TA2
    commands[3] = 0x33; // compute challenge
    ds.write_bytes(commands, 4);
    uint8_t crc_buf[2];
    ds.read_bytes(crc_buf, 2); // read CRC

    Serial.print("read crc: ");
    uint16_t crc16_buf = (uint16_t) crc_buf[0];
    crc16_buf = (crc16_buf << 8) + (uint16_t)  crc_buf[1];
    Serial.println(crc16_buf, HEX);
    Serial.println();

    if(!OneWire::check_crc16(commands, 4, crc_buf)) {
        Serial.print("CRC check failed");
        ds.reset();
        return CODE_INVALID_CRC;
    }
    Serial.println("CRC check succeeded");



    ds.reset();

    ds.write(0xa5); // resume

    // read PRNG counter
    ds.write(0xf0); // read memory
    ds.write(0xa0); // TA1
    ds.write(0x02); // TA2
    ds.read_bytes(buf, 4);
    Serial.print("PRNG counter:");
    for(j = 0; j < 4; j++) {
        Serial.print(" 0x");
        Serial.print(buf[j], HEX);
    }
    Serial.println();


    ds.reset();

    return CODE_SUCCESS;
}

bool is_valid(byte key[8]) {
    // TODO: read from sd card

    if (memcmp(key, valid_key, SERIAL_LENGTH) == 0) {
        return true;
    }

    return false;
}

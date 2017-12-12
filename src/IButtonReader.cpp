#include "IButtonReader.h"


IButtonReader::IButtonReader(uint8_t onewire_pin,
                             unsigned int search_interval_ms,
                             unsigned int keep_ibutton_interval_ms)
        : ibutton_is_available(false),
          ibutton(onewire_pin),
          search_interval(search_interval_ms),
          keep_ibutton_interval(keep_ibutton_interval_ms),
          last_millis_ibutton_search(0),
          last_millis_ibutton_found(0) {
    _ibutton_forget();
}

void IButtonReader::update() {
    unsigned long current_millis = millis();

    if (current_millis - last_millis_ibutton_search >= search_interval) {
        last_millis_ibutton_search = current_millis;
        bool ibutton_found = _ibutton_search();
        if(ibutton_found) {
            last_millis_ibutton_found = current_millis;
        }
    }
    if (ibutton_is_available && current_millis - last_millis_ibutton_found >= keep_ibutton_interval) {
        _ibutton_forget();
    }
}

bool IButtonReader::_ibutton_search() {
    uint8_t addr[1 + SERIAL_NUMBER_LENGTH + 1];  // 8 bit CRC, 48 bit serial number, 8 bit family code

    // search OneWire device
    if (!ibutton.search(addr)) {
        ibutton.reset_search();
        return false;
    }

    // check crc
    if (OneWire::crc8(addr, 7) != addr[7]) {
        return false;
    }

    _reverse_uint8_cpy(serial_number, addr + 1 * sizeof(uint8_t), SERIAL_NUMBER_LENGTH * sizeof(uint8_t));
    memcpy(&family_code, addr, 1);
    ibutton_is_available = true;
    return true;
}

void IButtonReader::_reverse_uint8_cpy(uint8_t *dst, const uint8_t *src, size_t n) {
    size_t i;
    for (i=0; i < n; ++i) {
        dst[n-1-i] = src[i];
    }
}

void IButtonReader::_ibutton_forget() {
    ibutton_is_available = false;
    family_code = 0;
    memset(serial_number, 0, sizeof(serial_number));
}

/*
// READ MEMORY
ibutton.write(0xf0); // read memory command
ibutton.write(0x00); // TA1
ibutton.write(0x00); // TA2

#define BUF_LEN (32*22)
uint8_t buf[BUF_LEN];
ibutton.read_bytes(buf, BUF_LEN);
int i = 0;
while(i < BUF_LEN) {
    debug_print("0x");
    debug_print(i, HEX);
    debug_print(" to 0x");
    debug_print(i+31, HEX);
    debug_print(":");
    int j;
    for(j = 0; j < 32; j++) {
        debug_print(" 0x");
        debug_print(buf[i++], HEX);
    }
    debug_println();
}
*/

/*
    // read PRNG counter
    ibutton.write(0xf0); // read memory command
    ibutton.write(0xa0); // TA1
    ibutton.write(0x02); // TA2

    uint8_t buf[4];
    ibutton.read_bytes(buf, 4);
    debug_print("PRNG counter:");
    int j;
    for (j = 0; j < 4; j++) {
        debug_print(" 0x");
        debug_print(buf[j], HEX);
    }
    debug_println();


    ibutton.reset();

    ibutton.write(0xa5); // resume

    // compute challenge
    uint8_t commands[4];
    commands[0] = 0x33; // compute SHA
    commands[1] = 0xa0; // TA1
    commands[2] = 0x02; // TA2
    commands[3] = 0x33; // compute challenge
    ibutton.write_bytes(commands, 4);
    uint8_t crc_buf[2];
    ibutton.read_bytes(crc_buf, 2); // read CRC

    debug_print("read crc: ");
    uint16_t crc16_buf = (uint16_t) crc_buf[0];
    crc16_buf = (crc16_buf << 8) + (uint16_t) crc_buf[1];
    debug_println(crc16_buf, HEX);
    debug_println();

    if (!OneWire::check_crc16(commands, 4, crc_buf)) {
        debug_print("CRC check failed");
        ibutton.reset();
        return ERR_INVALID_CRC;
    }
    debug_println("CRC check succeeded");


    ibutton.reset();

    ibutton.write(0xa5); // resume

    // read PRNG counter
    ibutton.write(0xf0); // read memory
    ibutton.write(0xa0); // TA1
    ibutton.write(0x02); // TA2
    ibutton.read_bytes(buf, 4);
    debug_print("PRNG counter:");
    for (j = 0; j < 4; j++) {
        debug_print(" 0x");
        debug_print(buf[j], HEX);
    }
    debug_println();

    ibutton.reset();

    return ERR_SUCCESS;
}
*/
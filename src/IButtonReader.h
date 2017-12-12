#ifndef IBUTTON_SCHLIESSSYSTEM_IBUTTONREADER_H
#define IBUTTON_SCHLIESSSYSTEM_IBUTTONREADER_H

#include <avr/io.h>
#include <OneWire.h>

#define SERIAL_NUMBER_LENGTH 6

class IButtonReader {
public:
    IButtonReader(uint8_t onewire_pin, unsigned int search_interval_ms,
                      unsigned int keep_ibutton_interval_ms);
    void update();

    bool ibutton_is_available;
    uint8_t serial_number[SERIAL_NUMBER_LENGTH];
    uint8_t family_code;

private:
    bool _ibutton_search();
    void _reverse_uint8_cpy(uint8_t *dst, const uint8_t *src, size_t n);
    void _ibutton_forget();

    OneWire ibutton;
    unsigned int search_interval;
    unsigned int keep_ibutton_interval;
    unsigned long last_millis_ibutton_search;
    unsigned long last_millis_ibutton_found;
};


#endif //IBUTTON_SCHLIESSSYSTEM_IBUTTONREADER_H

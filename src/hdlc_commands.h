#ifndef IBUTTON_SCHLIESSSYSTEM_HDLC_COMMANDS_H
#define IBUTTON_SCHLIESSSYSTEM_HDLC_COMMANDS_H

#define PROTOCOL_VERSION 1
#define PING_SIZE 8

enum hdlc_commands {
    COMMAND_PING = 0,
    COMMAND_GET_STATUS,
    COMMAND_UNLOCK_DOOR,
    COMMAND_REJECT_KEY
};

typedef struct {
    uint8_t command_type = COMMAND_PING;
    uint8_t ping_data[PING_SIZE];
} CommandPing;

typedef struct {
    uint8_t command_type = COMMAND_GET_STATUS;
} CommandGetStatus;

typedef struct {
    uint8_t command_type = COMMAND_UNLOCK_DOOR;
} CommandUnlockDoor;

typedef struct {
    uint8_t command_type = COMMAND_REJECT_KEY;
} CommandRejectKey;

enum hdlc_responses {
    RESPONSE_PONG = 0,
    RESPONSE_STATUS,
    RESPONSE_UNLOCK_DOOR,
    RESPONSE_REJECT_KEY
};

typedef struct {
    uint8_t response_type = RESPONSE_PONG;
    uint8_t pong_data[PING_SIZE];
} ResponsePong;

typedef struct {
    uint8_t response_type = RESPONSE_STATUS;
    bool ibutton_available;
    uint8_t family_code;
    uint8_t serial_number[SERIAL_NUMBER_LENGTH];
} ResponseStatus;

typedef struct {
    uint8_t response_type = RESPONSE_UNLOCK_DOOR;
} ResponseUnlockDoor;

typedef struct {
    uint8_t response_type = RESPONSE_REJECT_KEY;
} ResponseRejectKey;


#endif //IBUTTON_SCHLIESSSYSTEM_HDLC_COMMANDS_H

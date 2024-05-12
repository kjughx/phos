#ifndef _PS2_KEYBOARD_H_
#define _PS2_KEYBOARD_H_

#define PS2_PORT 0x64
#define PS2_CMD_ENABLE_FIRST_PORT 0xAE

#define PS2_KEY_RELEASED 0x80
enum Modifiers {
    LSHIFT = 0x2A,
    RSHIFT = 0x36,
    CAPSLOCK = 0x3A,
};

struct keyboard* ps2_init();

#endif /* _PS2_KEYBOARD_H_ */

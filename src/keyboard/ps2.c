#include "keyboard/ps2.h"
#include "common.h"
#include "idt/idt.h"
#include "io/io.h"
#include "kernel.h"
#include "keyboard/keyboard.h"
#include "task/task.h"

int ps2_keyboard_init();
struct keyboard ps2_keyboard = {
    .name = "PS/2",
    .init = ps2_keyboard_init,
};

struct keyboard* ps2_init() { return &ps2_keyboard; }

void ps2_keyboard_handle_interrupt();
int ps2_keyboard_init() {
    outb(PS2_PORT, PS2_CMD_ENABLE_FIRST_PORT);
    idt_register_intr_cb(ISR_KEYBOARD_INTERRUPT, ps2_keyboard_handle_interrupt);

    return 0;
}

static uint8_t ps2_scan_set_one[] = {
    0x00, 0x1B, '1',  '2',  '3',  '4',  '5',  '6',  '7',  '8',  '9',  '0',  '-',  '=',
    0x08, '\t', 'Q',  'W',  'E',  'R',  'T',  'Y',  'U',  'I',  'O',  'P',  '[',  ']',
    0x0d, 0x00, 'A',  'S',  'D',  'F',  'G',  'H',  'J',  'K',  'L',  ';',  '\'', '`',
    0x00, '\\', 'Z',  'X',  'C',  'V',  'B',  'N',  'M',  ',',  '.',  '/',  0x00, '*',
    0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, '7',  '8',
    '9',  '-',  '4',  '5',  '6',  '+',  '1',  '2',  '3',  '0',  '.'};

#define PS2_SCAN_SET_SIZE sizeof(ps2_scan_set_one)

uint8_t ps2_keyboard_scancode_to_char(uint8_t scan_code) {
    if (scan_code > PS2_SCAN_SET_SIZE)
        return 0;

    /* TODO: Handle modifiers */
    return ps2_scan_set_one[scan_code];
}

void ps2_keyboard_handle_interrupt() {
    uint8_t scan_code = 0;
    uint8_t character;

    kernel_page();

    scan_code = insb(KEYBOARD_INPUT_PORT);
    insb(KEYBOARD_INPUT_PORT); /* Rogue byte */

    if (scan_code & PS2_KEY_RELEASED)
        return;

    if ((character = ps2_keyboard_scancode_to_char(scan_code)))
        keyboard_push(character);

    task_page();
}

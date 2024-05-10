#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_

#define ISR_KEYBOARD_INTERRUPT 0x21
#define KEYBOARD_INPUT_PORT 0x60

struct process;

typedef int (*KEYBOARD_INIT_FUNCTION)();

struct keyboard {
    KEYBOARD_INIT_FUNCTION init;
    char name[20];
    struct keyboard* next;
};

void keyboard_init();
void keyboard_backspace(struct process* process);
void keyboard_push(char c);
char keyboard_pop();

#endif /* _KEYBOARD_H_ */

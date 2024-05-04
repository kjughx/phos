#include "isr80h/misc.h"
#include "common.h"
#include "idt/idt.h"
#include "string/string.h"

void* isr80h_command0_sum(struct interrupt_frame* frame) {
    print("Hello from isr80h_command0_sum");
    return NULL;
}

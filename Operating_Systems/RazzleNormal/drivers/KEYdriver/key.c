#include "key.h"

static inline unsigned char inb(unsigned short port) {
    unsigned char ret;
    __asm__ __volatile__ ("inb %1, %0" : "=a"(ret) : "dN"(port));
    return ret;
}

static inline unsigned char kbd_status() {
    return inb(0x64);
}

static const char scancode_azerty[128] = {
    0,   27,  '&', '\x82', '"', '\'', '(', '-', '\x88', '_', '\x87', '\x85',
    ')', '=', '\b', '\t',
   'a', 'z', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '^', '$', '\n', 0,   'q', 's',
   'd', 'f', 'g', 'h', 'j', 'k', 'l', 'm', '\xF9', '*', 0,   '<', 'w', 'x', 'c', 'v',
   'b', 'n', ',', ';', ':', '!', 0,   '*', 0,   ' ', 0,   0,   0,   0,   0,   0,
    0,  0,   0,   0,   0,   0,   0,   '7', '8', '9', '-', '4', '5', '6', '+', '1',
   '2', '3', '0', '.', 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0
};

void keyboard_init() {
}

unsigned char get_key() {
    if ((kbd_status() & 0x01) == 0) {
        return 0;
    }

    unsigned char scancode = inb(0x60);

    if (scancode & 0x80) {
        return 0;
    }

    if (scancode >= sizeof(scancode_azerty)) {
        return 0;
    }

    char ch = scancode_azerty[scancode];

    if (ch == 0) {
        return 0;
    }

    if (ch == '\n' || ch == '\b' || ch == '\t' || ch == 27) {
        return ch;
    }

    {
        unsigned char u = (unsigned char)ch;
        if (u >= 32 && u <= 126) return ch;
        if (u >= 128) return ch;
    }
    return 0;
}

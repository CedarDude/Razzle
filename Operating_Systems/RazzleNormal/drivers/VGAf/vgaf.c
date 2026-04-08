#include "vgaf.h"
#include "../KEYdriver/key.h"

static unsigned short* vga_buffer = (unsigned short*) VGA_BUFFER;
static int cursor_x = 0;
static int cursor_y = 0;
static unsigned char text_color = VGA_COLOR_WHITE;

static void vga_scroll_up() {
    for (int y = 0; y < VGA_HEIGHT - 1; y++) {
        for (int x = 0; x < VGA_WIDTH; x++) {
            int src_index = (y + 1) * VGA_WIDTH + x;
            int dst_index = y * VGA_WIDTH + x;
            vga_buffer[dst_index] = vga_buffer[src_index];
        }
    }
    
    for (int x = 0; x < VGA_WIDTH; x++) {
        int index = (VGA_HEIGHT - 1) * VGA_WIDTH + x;
        vga_buffer[index] = ' ' | (text_color << 8);
    }
}

void vga_init() {
    colorscreen(VGA_COLOR_BLACK);
    cursor_x = 0;
    cursor_y = 0;
}

void printv(const char* str) {
    while (*str) {
        if (*str == '\n') {
            cursor_x = 0;
            cursor_y++;
            if (cursor_y >= VGA_HEIGHT) {
                vga_scroll_up();
                cursor_y = VGA_HEIGHT - 1;
            }
        } else {
            int index = cursor_y * VGA_WIDTH + cursor_x;
            vga_buffer[index] = (*str) | (text_color << 8);
            cursor_x++;
            if (cursor_x >= VGA_WIDTH) {
                cursor_x = 0;
                cursor_y++;
                if (cursor_y >= VGA_HEIGHT) {
                    vga_scroll_up();
                    cursor_y = VGA_HEIGHT - 1;
                }
            }
        }
        str++;
    }
}

void vga_putc(char c) {
    if (c == '\n') {
        cursor_x = 0;
        cursor_y++;
        if (cursor_y >= VGA_HEIGHT) {
            vga_scroll_up();
            cursor_y = VGA_HEIGHT - 1;
        }
        return;
    }

    if (c == '\b') {
        if (cursor_x > 0) {
            cursor_x--;
        } else if (cursor_y > 0) {
            cursor_y--;
            cursor_x = VGA_WIDTH - 1;
        }
        int index = cursor_y * VGA_WIDTH + cursor_x;
        vga_buffer[index] = ' ' | (text_color << 8);
        return;
    }

    int index = cursor_y * VGA_WIDTH + cursor_x;
    vga_buffer[index] = c | (text_color << 8);
    cursor_x++;
    if (cursor_x >= VGA_WIDTH) {
        cursor_x = 0;
        cursor_y++;
        if (cursor_y >= VGA_HEIGHT) {
            vga_scroll_up();
            cursor_y = VGA_HEIGHT - 1;
        }
    }
}

void colorscreen(unsigned char color) {
    unsigned short attr = color << 8;
    for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
        vga_buffer[i] = ' ' | attr;
    }
    cursor_x = 0;
    cursor_y = 0;
}

void colortext(unsigned char color) {
    text_color = color;
}

void printv_color(const char* str, unsigned char color) {
    unsigned char old_color = text_color;
    text_color = color;
    printv(str);
    text_color = old_color;
}

char scanv() {
    return get_key();
}

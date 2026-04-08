#include "shell.h"
#include "../drivers/VGAf/vgaf.h"
#include "../drivers/KEYdriver/key.h"

#define BUFFER_SIZE 256

static int string_equal(const char* a, const char* b) {
    while (*a && *b) {
        if (*a != *b) return 0;
        a++;
        b++;
    }
    return *a == *b;
}

static int string_equal_ci(const char* a, const char* b) {
    while (*a && *b) {
        char ca = *a;
        char cb = *b;
        if (ca >= 'A' && ca <= 'Z') ca += 'a' - 'A';
        if (cb >= 'A' && cb <= 'Z') cb += 'a' - 'A';
        if (ca != cb) return 0;
        a++;
        b++;
    }
    return *a == *b;
}

static inline unsigned char inb(unsigned short port) {
    unsigned char result;
    __asm__ __volatile__("inb %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}

static inline void outb(unsigned short port, unsigned char value) {
    __asm__ __volatile__("outb %0, %1" : : "a"(value), "Nd"(port));
}

static unsigned int cmos_read(unsigned char addr) {
    outb(0x70, addr | 0x80);
    return (unsigned int)inb(0x71);
}

static void print_uint(unsigned int n) {
    char buf[12];
    int i = 0;
    if (n == 0) {
        vga_putc('0');
        return;
    }
    while (n) {
        buf[i++] = '0' + (n % 10);
        n /= 10;
    }
    while (i > 0) vga_putc(buf[--i]);
}

static void cmd_ver(void) {
    printv_color("Razzle codename Litewave [Version 0.1.54 BETA, Build 150]\n", 0x0B);
    printv_color("Razzle Shell - Copyright (c) 2026 Jad\n", 0x0B);
    printv_color("Open-source software licensed under GNU.\n", 0x0B);
}

static void cmd_clear(void) {
    colorscreen(0x00);
}

static void cmd_print(const char* rest) {
    if (rest && *rest) printv(rest);
    printv("\n");
}

static void cmd_sysinfo(void) {
    printv_color("CPU =======================================================\n", 0x0E);
    {
        unsigned int ebx, ecx, edx;
        __asm__ __volatile__(
            "cpuid"
            : "=b"(ebx), "=c"(edx), "=d"(ecx)
            : "a"(0)
        );
        char v[13];
        v[0] = (char)(ebx & 0xff);
        v[1] = (char)((ebx >> 8) & 0xff);
        v[2] = (char)((ebx >> 16) & 0xff);
        v[3] = (char)((ebx >> 24) & 0xff);
        v[4] = (char)(ecx & 0xff);
        v[5] = (char)((ecx >> 8) & 0xff);
        v[6] = (char)((ecx >> 16) & 0xff);
        v[7] = (char)((ecx >> 24) & 0xff);
        v[8] = (char)(edx & 0xff);
        v[9] = (char)((edx >> 8) & 0xff);
        v[10] = (char)((edx >> 16) & 0xff);
        v[11] = (char)((edx >> 24) & 0xff);
        v[12] = '\0';
        printv("Vendor: ");
        printv(v);
        printv("\n");
    }
    printv_color("RAM =======================================================\n", 0x0E);
    {
        unsigned int low = cmos_read(0x17) | (cmos_read(0x18) << 8);
        unsigned int high = cmos_read(0x30) | (cmos_read(0x31) << 8);
        unsigned int total_kb = low + high;
        if (total_kb == 0) total_kb = low;
        unsigned int total_mb = total_kb / 1024;
        printv("Extended: ");
        print_uint(total_kb);
        printv(" KB (");
        print_uint(total_mb);
        printv(" MB)\n");
    }
    printv_color("Storage ===================================================\n", 0x0E);
    printv("Boot device: floppy / disk 0\n");
}

static void cmd_help(void) {
    printv_color("***************************************\n", 0x0A);
    printv_color("*        Razzle Shell Commands        *\n", 0x0A);
    printv_color("*        Copyright Jad 2026           *\n", 0x0A);
    printv_color("***************************************\n", 0x0A);
    printv_color("help          - Show this help message\n", 0x0A);
    printv_color("about         - Show information about Razzle\n", 0x0A);
    printv_color("version       - Show version information\n", 0x0A);
    printv_color("systeminfo    - Show basic system information\n", 0x0A);
    printv_color("dir           - List files in current directory (not implemented)\n", 0x0A);
    printv_color("cls / clear   - Clear the screen\n", 0x0A);
    printv_color("echo <text>   - Print the specified text\n", 0x0A);
}

static void cmd_about(void) {
    colorscreen(0x00);
    colortext(0x0F);
    printv_color("Razzle codename Litewave - Lightweight small OS, [fallback mode] powered by MangoKernel1 (MK1)\n", 0x0D);
    printv_color("Made because idk i wanted it.\n", 0x0D);
    printv_color("Creators: JAD and ORCA\n", 0x0D);
    printv_color("Commands: help, about, sysinfo, dir, cls, reboot.\n", 0x0D);
    colortext(0x0F);
}

static void cmd_dir(void) {
    printv_color("Directory listing not implemented yet.\n", 0x09);
}

static void shell_execute_command(char* buffer) {
    int i = 0;
    while (buffer[i] == ' ') i++;
    int cmd_start = i;
    while (buffer[i] && buffer[i] != ' ') i++;

    if (string_equal_ci("version", buffer + cmd_start)) {
        cmd_ver();
        return;
    }
    if (string_equal_ci("print", buffer + cmd_start) || string_equal_ci("echo", buffer + cmd_start)) {
        while (buffer[i] == ' ') i++;
        cmd_print(buffer + i);
        return;
    }
    if (string_equal_ci("systeminfo", buffer + cmd_start)) {
        cmd_sysinfo();
        return;
    }
    if (string_equal_ci("cls", buffer + cmd_start) || string_equal_ci("clear", buffer + cmd_start)) {
        cmd_clear();
        return;
    }
    if (string_equal_ci("help", buffer + cmd_start)) {
        cmd_help();
        return;
    }
    if (string_equal_ci("dir", buffer + cmd_start)) {
        cmd_dir();
        return;
    }
    if (string_equal_ci("rputil reboot", buffer + cmd_start)) {
        printv_color("Rebooting...\n", 0x0C);
        __asm__ __volatile__("cli; movb $0xFE, %al; outb %al, $0x64; hlt");
        return;
    }
    if (string_equal_ci("about", buffer + cmd_start)) {
        cmd_about();
        return;
    }
    printv_color("? No such command IS here\n", 0x0C);
}

#define HISTORY_SIZE 10

void shell_main() {
    char buffer[BUFFER_SIZE];
    char history[HISTORY_SIZE][BUFFER_SIZE];
    int history_count = 0;
    int history_idx = -1;
    int buf_idx = 0;

    for (int i = 0; i < HISTORY_SIZE; i++) {
        history[i][0] = '\0';
    }

    printv("Razzle Shell\n\n");

    while (1) {
        colortext(0x0E);
        printv("SHELL/> ");
        colortext(0x0F);

        buf_idx = 0;
        buffer[0] = '\0';
        history_idx = -1;

        while (1) {
            unsigned char key = get_key();
            if (key) {
                if (key == '\r' || key == '\n') {
                    buffer[buf_idx] = '\0';
                    vga_putc('\n');
                    break;
                } else if (key == '\b' && buf_idx > 0) {
                    buf_idx--;
                    vga_putc('\b');
                    vga_putc(' ');
                    vga_putc('\b');
                } else if (key == 0x48) {
                    if (history_count > 0) {
                        history_idx++;
                        if (history_idx >= history_count) history_idx = history_count - 1;
                        
                        for (int i = 0; i < buf_idx; i++) {
                            vga_putc('\b');
                            vga_putc(' ');
                            vga_putc('\b');
                        }
                        
                        buf_idx = 0;
                        while (history[history_idx][buf_idx]) {
                            buffer[buf_idx] = history[history_idx][buf_idx];
                            vga_putc(buffer[buf_idx]);
                            buf_idx++;
                        }
                    }
                } else if (key == 0x50) {
                    if (history_idx > 0) {
                        history_idx--;
                        
                        for (int i = 0; i < buf_idx; i++) {
                            vga_putc('\b');
                            vga_putc(' ');
                            vga_putc('\b');
                        }
                        
                        buf_idx = 0;
                        while (history[history_idx][buf_idx]) {
                            buffer[buf_idx] = history[history_idx][buf_idx];
                            vga_putc(buffer[buf_idx]);
                            buf_idx++;
                        }
                    } else if (history_idx == -1 && history_count > 0) {
                        for (int i = 0; i < buf_idx; i++) {
                            vga_putc('\b');
                            vga_putc(' ');
                            vga_putc('\b');
                        }
                        buf_idx = 0;
                    }
                } else if (buf_idx < BUFFER_SIZE - 1) {
                    buffer[buf_idx++] = key;
                    vga_putc(key);
                }
            }
        }

        if (buf_idx == 0) continue;
        
        if (history_count < HISTORY_SIZE) {
            for (int i = 0; i <= buf_idx; i++) {
                history[history_count][i] = buffer[i];
            }
            history_count++;
        } else {
            for (int i = 0; i < HISTORY_SIZE - 1; i++) {
                for (int j = 0; j < BUFFER_SIZE; j++) {
                    history[i][j] = history[i + 1][j];
                }
            }
            for (int i = 0; i <= buf_idx; i++) {
                history[HISTORY_SIZE - 1][i] = buffer[i];
            }
        }
        
        shell_execute_command(buffer);
    }
}
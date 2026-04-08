#include "../drivers/VGAf/vgaf.h"
#include "../drivers/KEYdriver/key.h"
#include "../Shell/shell.h"

extern void init_idt(void);

static void boot_screen() {
    colorscreen(0x01);
    colortext(0x0F);
    printv("Booting...\n");
    printv("COPYRIGHT JAD 2026\n\n");
    printv("RRRRR   AAAAA   ZZZZZZZ   ZZZZZ   L      EEEEEEE\n");
    printv("R   R   A   A       Z       Z    L      E      \n");
    printv("RRRR    AAAAA      Z       Z     L      EEEEEE \n");
    printv("R   R   A   A     Z       Z      L      E      \n");
    printv("R   R   A   A   ZZZZZZZ ZZZZZ   LLLLLL  EEEEEEE\n\n");
    printv("razzle Operating System!\n\n");
}

void kernel_main() {
    vga_init();
    keyboard_init();
    init_idt();
    boot_screen();
    colorscreen(0x00);
    shell_main();
}
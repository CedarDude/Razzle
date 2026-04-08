.section .text
.global keyboard_handler
.extern keyboard_handler_c

keyboard_handler:
    pusha
    call keyboard_handler_c
    movb $0x20, %al
    outb %al, $0x20
    popa
    iret

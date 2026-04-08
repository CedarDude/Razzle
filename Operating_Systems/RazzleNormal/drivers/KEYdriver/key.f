.global keyboard_handler
.global last_key
.global keyboard_init
.global get_key

.section .data
last_key: .byte 0

.section .text
keyboard_init:
    # IDT set elsewhere
    ret

get_key:
    movb last_key, %al
    movb $0, last_key
    ret

keyboard_handler:
    pusha
    inb $0x60, %al
    # Simple mapping
    cmpb $0x10, %al
    jl .skip
    cmpb $0x19, %al
    jg .skip
    subb $0x10, %al
    addb $'q', %al
    movb %al, last_key
.skip:
    movb $0x20, %al
    outb %al, $0x20
    popa
    iret
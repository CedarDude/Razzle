.section .data
idt:
    .rept 256
    .word 0
    .word 0x08
    .byte 0
    .byte 0x8E
    .word 0
    .endr
idt_ptr:
    .word 256*8-1
    .long idt
.section .text
.global idt_load
.global idt_init

.extern keyboard_handler

dummy_handler:
    pusha
    movb $0x20, %al
    outb %al, $0x20
    popa
    iret

idt_load:
    lidt idt_ptr
    ret
idt_init:
    # Remap PIC
    movb $0x11, %al
    outb %al, $0x20
    movb $0x20, %al
    outb %al, $0x21
    movb $0x04, %al
    outb %al, $0x21
    movb $0x01, %al
    outb %al, $0x21

    movb $0x11, %al
    outb %al, $0xA0
    movb $0x28, %al
    outb %al, $0xA1
    movb $0x02, %al
    outb %al, $0xA1
    movb $0x01, %al
    outb %al, $0xA1

    # Mask interrupts, enable only keyboard
    movb $0xFD, %al
    outb %al, $0x21
    movb $0xFF, %al
    outb %al, $0xA1

    # Set keyboard interrupt (33)
    movl $keyboard_handler, %eax
    movw %ax, idt+33*8
    shr $16, %eax
    movw %ax, idt+33*8+6
    call idt_load
    sti
    ret
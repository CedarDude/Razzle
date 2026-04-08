bits 32

section .data
idt:
    times 256 dq 0
idt_ptr:
    dw 256*8-1
    dd idt

section .text
    global idt_load
    global idt_init
    extern keyboard_handler

idt_load:
    lidt [idt_ptr]
    ret

idt_init:
    cli
    mov al, 0x11
    out 0x20, al
    mov al, 0x20
    out 0x21, al
    mov al, 0x04
    out 0x21, al
    mov al, 0x01
    out 0x21, al

    ; wtf
    mov al, 0x11
    out 0xA0, al
    mov al, 0x28
    out 0xA1, al
    mov al, 0x02
    out 0xA1, al
    mov al, 0x01
    out 0xA1, al

    ; Mask interrupts
    mov al, 0xFD
    out 0x21, al
    mov al, 0xFF
    out 0xA1, al

    mov eax, keyboard_handler
    mov word [idt + 33*8], ax
    shr eax, 16
    mov word [idt + 33*8 + 6], ax

    call idt_load
    sti
    ret

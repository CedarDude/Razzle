bits 32

extern keyboard_handler_c

section .text
    global keyboard_handler

    keyboard_handler:
        pusha                       
        call keyboard_handler_c     
        mov al, 0x20                
        out 0x20, al
        popa 
        iret
section .note.GNU-stack progbits


.section .multiboot, "a"
.align 4
.long 0x1BADB002          /* Multiboot magic number */
.long 0x00000003          /* Flags (request memory info + aligned modules) */
.long -(0x1BADB002 + 0x00000003)  /* Checksum */

.section .text
.global _start
.extern kernel_main

_start:
    cli
    mov $kernel_stack_top, %esp
    call kernel_main
    cli
    hlt
    jmp .

.section .bss
.align 16
kernel_stack_bottom:
    .skip 16384
kernel_stack_top:


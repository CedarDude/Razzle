.section .multiboot
.align 4
.long 0x1BADB002
.long 0x00000003
.long -(0x1BADB002 + 0x00000003)

.section .text
.global _start
_start:
    cli
    mov $kernel_stack, %esp
    call kernel_main
    hlt

.section .bss
.align 16
kernel_stack_bottom:
.skip 16384
kernel_stack:
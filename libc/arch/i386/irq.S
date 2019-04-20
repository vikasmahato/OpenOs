# Define the hardware Interrupt Requests

.global irq0
.global irq1
.global irq2
.global irq3
.global irq4
.global irq5
.global irq6
.global irq7
.global irq8
.global irq9
.global irq10
.global irq11
.global irq12
.global irq13
.global irq14
.global irq15

# We will install the IRQs in the IDT indices [32-47]

#  IRQ0
irq0:
    cli
    push $0
    push $32
    jmp irq_common_stub

#  IRQ1
irq1:
    cli
    push $0
    push $33
    jmp irq_common_stub

#  IRQ2
irq2:
    cli
    push $0
    push $34
    jmp irq_common_stub

#  IRQ3
irq3:
    cli
    push $0
    push $35
    jmp irq_common_stub

#  IRQ4
irq4:
    cli
    push $0
    push $36
    jmp irq_common_stub

#  IRQ5
irq5:
    cli
    push $0
    push $37
    jmp irq_common_stub

#  IRQ6
irq6:
    cli
    push $0
    push $38
    jmp irq_common_stub

#  IRQ7
irq7:
    cli
    push $0
    push $39
    jmp irq_common_stub

#  IRQ8
irq8:
    cli
    push $0
    push $40
    jmp irq_common_stub

#  IRQ9
irq9:
    cli
    push $0
    push $41
    jmp irq_common_stub

#  IRQ10
irq10:
    cli
    push $0
    push $42
    jmp irq_common_stub

#  IRQ11
irq11:
    cli
    push $0
    push $43
    jmp irq_common_stub

#  IRQ12
irq12:
    cli
    push $0
    push $44
    jmp irq_common_stub

#  IRQ13
irq13:
    cli
    push $0
    push $45
    jmp irq_common_stub

#  IRQ14
irq14:
    cli
    push $0
    push $46
    jmp irq_common_stub

#  IRQ15
irq15:
    cli
    push $0
    push $47
    jmp irq_common_stub

.extern irq_handler

irq_common_stub:
    pusha
    push %ds
    push %es
    push %fs
    push %gs
    mov $0x10, %ax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs
    mov %esp, %eax  # Push us the stack
    push %eax
    call irq_handler # A special call, preserves the 'eip' register
    pop %eax
    pop %gs
    pop %fs
    pop %es
    pop %ds
    popa
    add $8, %esp   # Cleans up the pushed error code and pushed ISR number
    sti
    iret           # pops 5 things at once: CS, EIP, EFLAGS, SS, and ESP!
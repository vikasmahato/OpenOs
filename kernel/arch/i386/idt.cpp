#include <stdio.h>
#include <string.h>
#include <kernel/idt.h>

#ifdef __cplusplus
extern "C"
{
#endif

// Defines an IDT entry
struct idt_entry {
    uint16_t base_lo;
    uint16_t sel;
    uint8_t always0;
    uint8_t flags;
    uint16_t base_hi;
} __attribute__((packed));

struct idt_ptr {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

// Declare an IDT of 256 entries. Although we will only use the
// first 32 entries in this tutorial, the rest exists as a bit
// of a trap. If any undefined IDT entry is hit, it normally
// will cause an "Unhandled Interrupt" exception. Any descriptor
// for which the 'presence' bit is cleared (0) will generate an
// "Unhandled Interrupt" exception
struct idt_entry idt[256];
struct idt_ptr idtp;

// Function arch/i386/idt.S, loads IDT from a pointer to an idt_ptr
extern void idt_load(struct idt_ptr* idt_ptr_addr);

void idt_set_gate(uint8_t num, uint64_t base, uint16_t sel, uint8_t flags) {
    idt[num].base_lo = base & 0xFFFF;
    idt[num].base_hi = (base >> 16) & 0xFFFF;
    idt[num].always0 = 0;
    idt[num].flags = flags;
    idt[num].sel = sel;
}

// Installs the IDT
void idt_install() {
    // Sets the special IDT pointer up
    idtp.limit = (sizeof (struct idt_entry) * 256) - 1;
    idtp.base = (uint32_t) &idt;

    // Clear out the entire IDT, initializing it to zeros
    memset(&idt, 0, sizeof(struct idt_entry) * 256);

    // Points the processor's internal register to the new IDT
    idt_load(&idtp);
    printf("IDT installed.\n");
}

#ifdef __cplusplus
}
#endif
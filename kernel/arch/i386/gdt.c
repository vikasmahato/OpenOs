#include <arch/i386/gdt.h>
#include <stdint.h>
#include <stdio.h>
/* References: 
 * https://wiki.osdev.org/GDT_Tutorial
 * http://www.jamesmolloy.co.uk/tutorial_html/4.-The%20GDT%20and%20IDT.html 
 */

/*
 * Format of Access flags:
 * -------------------------------------------------------------------------------------------------------------
 * | Present bit (must be 1) | Ring level (0-3) | Always 1  | Executable | Dir/confo Bit | RW bit | Access Bit |
 * |          1 bit          |      2 bits      |  1 bit    |   1 bit    |     1 bit     | 1 bit  |    1 bit   |
 * -------------------------------------------------------------------------------------------------------------
   
 *  Present: Must be 1
 *  Ring level: 0-3
 *  Executable: Marks if it is executable. 1 = CODE selector, 0 = DATA selector
 *  Direction / Conforming bit: 
 *      For DATA selectors, Tells the direction. 0 the segment grows up. 1 the
 *          segment grows down, ie. the offset has to be greater than the base.
 *      For CODE selectors, If 1 code in this segment can be executed from an
 *          equal or lower privilege level. For example, code in ring 3 can
 *          far-jump to conforming code in a ring 2 segment. The privl-bits
 *          represent the highest privilege level that is allowed to execute the
 *          segment. For example, code in ring 0 cannot far-jump to a conforming
 *          code segment with privl==0x2, while code in ring 2 and 3 can. Note
 *          that the privilege level remains the same, ie. a far-jump form ring 3
 *          to a privl==2-segment remains in ring 3 after the jump. If 0 code in
 *          this segment can only be executed from the ring set in privl.
 *  RW Bit:
 *       Readable bit for code selectors: Whether read access for this segment is
 *          allowed. Write access is never allowed for code segments.
 *       Writable bit for data selectors: Whether write access for this segment is
 *          allowed. Read access is always allowed for data segments.
 *  Access Bit: 
 *       Just set to 0. The CPU sets this to 1 when the segment is accessed
 */

/*
 * Format of Granularity:
   -------------------------------------------------------------------------------------------------------------------------
   | G (0 = 1byte, 1=4kbytes( | D (operand size 0= 16 bit, 1 = 32 bit)) | Always 0  | Avail (should be 0) | Segment Length |
   |          1 bit           |                  1 bit                  |  1 bit    |        1 bit        |    4 bits      |
   -------------------------------------------------------------------------------------------------------------------------
*/

struct gdt_entry {
  uint16_t limit_low;     /* The lower 16 bits of the limit. */
  uint16_t base_low;      /* The lower 16 bits of the base. */
  uint8_t base_middle;    /* The next 8 bits of the base. */
  uint8_t access;         /* Access flags, determine what ring this segment can be used in. */
  uint8_t granularity;    /* Granularity */
  uint8_t base_high;      /* The last 8 bits of the base. */
} __attribute__((packed));

/* Special pointer which includes the limit: The max bytes
 * taken up by the GDT, minus 1. 
 */
struct gdt_ptr {
  uint16_t limit;           /* The Global Descriptor Table limit. (table size -1) */
  uint32_t base;            /* The address of the first gdt_entry_t struct. */
} __attribute__((packed));  /* prevents compiler to optimize struct */

/* Our GDT, with 3 entries, and finally our special GDT pointer */
struct gdt_entry gdt[3];
struct gdt_ptr gp;

/* Function kernel/arch/i386/gdt_asm.S, loads GDT from the pointer of a gdt_ptr */
extern void gdt_flush(struct gdt_ptr* gdt_ptr_addr);

/* Setup a descriptor in the Global Descriptor Table */
void gdt_set_gate(int32_t num, uint32_t base, uint32_t limit, uint8_t access,
                  uint8_t gran) {
  /* Setup the descriptor base address */
  gdt[num].base_low = (base & 0xFFFF);
  gdt[num].base_middle = (base >> 16) & 0xFF;
  gdt[num].base_high = (base >> 24) & 0xFF;

  /* Setup the descriptor limits */
  gdt[num].limit_low = (limit & 0xFFFF);
  gdt[num].granularity = ((limit >> 16) & 0x0F);

  /* Finally, set up the granularity and access flags */
  gdt[num].granularity |= (gran & 0xF0);
  gdt[num].access = access;
}

/* Should be called by the kernal on initializaiton. This will setup the
 * special GDT pointer, set up the first 3 entries in our GDT, and then
 * finally call gdt_flush() in our assembler file in order to tell the
 * processor where the new GDT is and update the new segment registers
 */
void gdt_install() {
  /* Setup the GDT pointer and limit */
  gp.limit = (sizeof(struct gdt_entry) * 3) - 1;
  gp.base = (uint32_t)&gdt;

  gdt_set_gate(0, 0, 0, 0, 0);                                    /* Our NULL descriptor */
  gdt_set_gate(1, 0, 0xFFFFFFFF, CODE_SELECTOR, FLATMODEL_GRAN);  /* Kernel code segment */
  gdt_set_gate(2, 0, 0xFFFFFFFF, DATA_SELECTOR, FLATMODEL_GRAN);  /* Kernel data segment */

  /* Flush out the old GDT and install the new changes! */
  gdt_flush(&gp);

  printf("GDT flushed and loaded.\n");
}
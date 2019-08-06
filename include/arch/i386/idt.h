#ifndef _KERNEL_IDT_H_
#define _KERNEL_IDT_H_

#include <stdint.h>

#define IDT_NUM_ENTRIES 256

/*
 * The processor will sometimes need to signal your kernel. 
 * Something major may have happened, such as a divide-by-zero, or a page fault. 
 * To do this, it uses the first 32 interrupts. It is therefore doubly important 
 * that all of these are mapped and non-NULL - else the CPU will triple-fault and 
 * reset (bochs will panic with an 'unhandled exception' error).
 * The special, CPU-dedicated interrupts are shown below.

 * 0 - Division by zero exception
 * 1 - Debug exception
 * 2 - Non maskable interrupt
 * 3 - Breakpoint exception
 * 4 - 'Into detected overflow'
 * 5 - Out of bounds exception
 * 6 - Invalid opcode exception
 * 7 - No coprocessor exception
 * 8 - Double fault (pushes an error code)
 * 9 - Coprocessor segment overrun
 * 10 - Bad TSS (pushes an error code)
 * 11 - Segment not present (pushes an error code)
 * 12 - Stack fault (pushes an error code)
 * 13 - General protection fault (pushes an error code)
 * 14 - Page fault (pushes an error code)
 * 15 - Unknown interrupt exception
 * 16 - Coprocessor fault
 * 17 - Alignment check exception
 * 18 - Machine check exception
 * 19-31 - Reserved
 */

/* Sets up the IDT, should be called on early initialization */
void idt_install();

#endif  // _KERNEL_IDT_H_
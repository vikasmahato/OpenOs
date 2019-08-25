#ifndef _INTERRUPT_H_
#define _INTERRUPT_H_ 1

#include <stdbool.h>
#include <string.h>
#include <stddef.h>
#include <arch/i386/idt.h>

#define TIMER_IDT_INDEX 32
#define KEYBOARD_IDT_INDEX 33
#define SYSCALL_IDT_INDEX 128

#ifdef __cplusplus
extern "C"
{
#endif

/* Holds the registers at the time of the interrupt */
struct regs {
  uint32_t gs, fs, es, ds;                          /* pushed the segs last */
  uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;  /* pushed by pusha */
  uint32_t idt_index, err_code;                     /* manually pushed */
  uint32_t eip, cs, eflags, useresp, ss;            /* pushed automatically by CPU */
};

typedef void (*interrupt_handler_t)(struct regs* r);

class InterruptHandler {
  private:
    /* Hold upto 256 interrupt handlers. */
    static interrupt_handler_t interrupt_handlers[IDT_NUM_ENTRIES];
    static void fault_handler(struct regs *r);
    static void irq_handler(struct regs *r);
  public:
    InterruptHandler();
    static void runInterruptHandler(struct regs* r);
    static bool register_interrupt_handler(uint32_t idt_index,
	                            interrupt_handler_t handler);
  protected:
    static void divideByZeroExceptionHandler(struct regs* r);
    static void debugExceptionHandler(struct regs* r);                      /* Trap  */
    static void nonMaskableInterruptExceptionHandler(struct regs* r);       /* Trap  */
    static void breakpointExceptionHandler(struct regs* r);                 /* Trap  */
    static void intoDetectedOverflowExceptionHandler(struct regs* r);       /* Trap  */
    static void outOfBoundsExceptionHandler(struct regs* r);                /* Fault */
    static void invalidOpcodeExceptionHandler(struct regs* r);              /* Fault */
    static void noCoprocessorExceptionHandler(struct regs* r);              /* Fault */
    static void doubleFaultExceptionHandler(struct regs* r);                /* Abort */
    static void coprocessorSegmentOverrunExceptionHandler(struct regs* r);  /* Fault */
    static void badTssExceptionHandler(struct regs* r);                     /* Fault */
    static void segmentNotPresentExceptionHandler(struct regs* r);          /* Fault */
    static void stackFaultExceptionHandler(struct regs* r);                 /* Fault */
    static void generalProtectionFaultExceptionHandler(struct regs* r);     /* Fault */
    static void pageFaultExceptionHandler(struct regs* r);                  /* Fault */
    static void unkownInterruptExceptionHandler(struct regs* r);
    static void coprocessorFaultExceptionHandler(struct regs* r);           /* Fault */
    static void alignmentCheckExceptionHandler(struct regs* r);             /* Fault */
    static void machineCheckExceptionHandler(struct regs* r);               /* Abort */
};

void run_interrupt_handler(struct regs* r);

#ifdef __cplusplus
}
#endif

#endif /* INTERRUPT_H */
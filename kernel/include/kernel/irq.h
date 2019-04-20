#ifndef _KERNEL_IRQ_H_
#define _KERNEL_IRQ_H_

#include "register.h"

#ifdef __cplusplus
extern "C"
{
#endif

// Sets up the IRQs, should be called on early initialization
void irq_install();

// Adds an IRQ handler to the IDT
void irq_install_handler(int irq, void (*handler) (struct regs *r));

// Removes an IRQ from the IDT
void irq_uninstall_handler(int irq);

#ifdef __cplusplus
}
#endif

#endif  // _KERNEL_IRQ_H_
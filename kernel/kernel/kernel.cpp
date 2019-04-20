#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include <asm.h>
#include <kernel/gdt.h>
#include <kernel/idt.h>
#include <kernel/irq.h>
#include <kernel/isrs.h>
#include <kernel/timer.h>
#include <kernel/tty.h>
#include <kernel/kb.h>

#ifdef __cplusplus
extern "C"
{
#endif

void kernel_early(void) {
  terminal_initialize();
  gdt_install();
  idt_install();
  isrs_install();
  irq_install();
  timer_install();
  keyboard_install();
  enable_interrupts();
}

void kernel_main(void) {
  int i = 0;
  printf("Hello, kernel World!\n");
  // printf("%d\n", 1 / 0);

 for(;;) {
    asm("hlt");
 }
}

#ifdef __cplusplus
}
#endif
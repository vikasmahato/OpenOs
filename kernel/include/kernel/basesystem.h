#ifndef _KERNEL_BASESYSTEM_H_
#define _KERNEL_BASESYSTEM_H_

#include "constants.h"
#include "multiboot.h"

#ifdef __cplusplus
extern "C"
{
#endif

class BaseSystem {
    private:
        void print_early_boot_info(multiboot_info* mb);
        bool init_gdt();
        bool init_idt();
        bool init_isr();
        bool init_timer();
        bool init_keyboard();
        bool init_irq();
    public:
        void init(multiboot_info* mb);
};

#ifdef __cplusplus
}
#endif

#endif  // _KERNEL_GDT_H_
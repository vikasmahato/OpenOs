#ifndef _KERNEL_BASESYSTEM_H_
#define _KERNEL_BASESYSTEM_H_ 1

#include <external/multiboot.h>
#include <devices/driver.h>
#include <arch/i386/interrupts.h>

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
        bool initializeDrivers(DriverManager* driverManager,  InterruptHandler* interruptHandler);
        bool init_irq();
    public:
        void init(multiboot_info* mb);
};

#ifdef __cplusplus
}
#endif

#endif  // _KERNEL_BASESYSTEM_H_
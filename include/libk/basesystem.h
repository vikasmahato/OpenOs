#ifndef _KERNEL_BASESYSTEM_H_
#define _KERNEL_BASESYSTEM_H_

#include <external/multiboot.h>
#include <devices/driver.h>

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
        bool initializeDrivers(DriverManager* driverManager);
        bool init_irq();
    public:
        void init(multiboot_info* mb);
};

#ifdef __cplusplus
}
#endif

#endif  // _KERNEL_GDT_H_
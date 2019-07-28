#ifndef _KERNEL_CONSTANTS_H_
#define _KERNEL_CONSTANTS_H_

#include <stdint.h>

#define TICKS_PER_SECOND 100

// Constants defined in the linker
extern uint32_t kernel_phys_start;
extern uint32_t kernel_phys_end;
extern uint32_t kernel_virt_start;
extern uint32_t kernel_virt_end;

#define KERNEL_START_ADDR ((uint32_t) &kernel_phys_start)
#define KERNEL_END_ADDR ((uint32_t) &kernel_phys_end)
#define KERNEL_SIZE (KERNEL_END_ADDR-KERNEL_START_ADDR)

#define KERNEL_VIRTUAL_START_ADDR ((uint32_t) &kernel_virt_start)
#define KERNEL_VIRTUAL_END_ADDR ((uint32_t) &kernel_virt_end)

typedef uint32_t VirtualAddress;
typedef uint32_t PhysicalAddress;

#ifndef PAGE_SIZE
#define PAGE_SIZE 0x1000
#define BLOCK_SIZE PAGE_SIZE
#define PAGE_MASK 0xFFFFF000
#define BLOCK_MASK PAGE_MASK
#endif

#define PMM_STACK_ADDR 0xB0000000

#endif //_KERNEL_CONSTANTS_H_

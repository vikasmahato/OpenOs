#ifndef _KERNEL_CONSTANTS_H_
#define _KERNEL_CONSTANTS_H_

#define TICKS_PER_SECOND 100

// Constants defined in the linker
extern uint32_t kernel_start;
extern uint32_t kernel_end;

#define KERNEL_START_ADDR ((uint32_t) &kernel_start)
#define KERNEL_END_ADDR ((uint32_t) &kernel_end)
#define KERNEL_SIZE (KERNEL_START_ADDR - KERNEL_END_ADDR)

#endif //_KERNEL_CONSTANTS_H_

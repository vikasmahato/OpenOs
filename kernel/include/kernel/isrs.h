#ifndef _KERNEL_ISRS_H_
#define _KERNEL_ISRS_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

// Holds the registers at the time of the interrupt
struct regs {
    uint32_t gs, fs, es, ds;  // pushed the segs last
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;  // pushed by pusha
    uint32_t int_no, err_code;  // manually pushed
    uint32_t eip, cs, eflags, useresp, ss;  // pushed automatically
};

// Install the ISRS into the IDT, should be called on early initialization
void isrs_install();

#ifdef __cplusplus
}
#endif

#endif  // _KERNEL_ISRS_H_
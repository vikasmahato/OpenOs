#ifndef _KERNEL_ISRS_H_
#define _KERNEL_ISRS_H_

#include <stdint.h>
#include "register.h"

#ifdef __cplusplus
extern "C"
{
#endif

// Install the ISRS into the IDT, should be called on early initialization
void isrs_install();

#ifdef __cplusplus
}
#endif

#endif  // _KERNEL_ISRS_H_
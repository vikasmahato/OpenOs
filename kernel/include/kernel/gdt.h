#ifndef _KERNEL_GDT_H_
#define _KERNEL_GDT_H_

#ifdef __cplusplus
extern "C"
{
#endif

// Sets up the GDT, should be called on early initialization
void gdt_install();

#ifdef __cplusplus
}
#endif

#endif  // _KERNEL_GDT_H_
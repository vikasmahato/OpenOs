#ifndef _KERNEL_GDT_H_
#define _KERNEL_GDT_H_

/*       Pr   R 1 E D/C RW A */
/* Code: 1  000 1 1 0   1  0 --> 0x9A*/
/* Data: 1  000 1 0 0   1  0 --> 0x92*/
#define CODE_SELECTOR 0x9A
#define DATA_SELECTOR 0x92

/*                   G D 0 A SegLen */
/* Flat-Model Priv:  1 1 0 1   1111 --> 0xCF*/
#define FLATMODEL_GRAN  0xCF

/* Sets up the GDT, should be called on early initialization */
void gdt_install();

#endif  // _KERNEL_GDT_H_
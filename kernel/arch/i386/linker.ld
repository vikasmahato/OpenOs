/* The bootloader will look at this image and start execution at the symbol
   designated at the entry point. */
ENTRY(_loader)

/* Tell where the various sections of the object files will be put in the final
   kernel image. */
SECTIONS
{
  /* The kernel will live at 3GB + 1MB in the virtual
     address space, which will be mapped to 1MB in the
     physical address space. */
  . = 0xC0100000;

  kernel_virt_start = .;
  kernel_phys_start = . - 0xC0000000;

  /* First put the multiboot header, as it is required to be put very early
     early in the image or the bootloader won't recognize the file format.
     Next we'll put the .text section. */
  .text : AT(ADDR(.text) - 0xC0000000) {
    *(.multiboot)
    *(.text)
    . = ALIGN(0x1000);
  }

  /* Read-only data. */
  .rodata : AT(ADDR(.rodata) - 0xC0000000) {
    *(.rodata*)
    . = ALIGN(0x1000);
  }

  /* Read-write data (initialized) */
  .data : AT(ADDR(.data) - 0xC0000000) {
    *(.data)
    . = ALIGN(0x1000);
  }

  /* Read-write data (uninitialized) and stack */
  .bss : AT(ADDR(.bss) - 0xC0000000) {
    *(COMMON)
    *(.bss)
    *(.bootstrap_stack)
    . = ALIGN(0x1000);
  }

  /* The compiler may produce other sections, put them in the proper place in
     in this file, if you'd like to include them in the final kernel. */
    kernel_virt_end = .;
    kernel_phys_end = . - 0xC0000000;
}

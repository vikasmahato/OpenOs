#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <kernel/basesystem.h>
#include <asm.h>
#include <kernel/multiboot.h>
#include <kernel/tty.h>

#ifdef __cplusplus
extern "C"
{
#endif

void kernel_early() {
    //terminal_initialize();


}

void kernel_main(uint32_t magic, struct multiboot_info* mb) {

    BaseSystem baseSystem;
    baseSystem.init(mb);

    printf("Magic number: %lx\n", magic);
    

  printf("Hello, kernel World!\n");

 for(;;) {
    asm("hlt");
 }
}

#ifdef __cplusplus
}
#endif
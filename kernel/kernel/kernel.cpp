#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <kernel/basesystem.h>


#ifdef __cplusplus
extern "C"
{
#endif

void kernel_early(void) {

}

void kernel_main(void) {

    BaseSystem baseSystem;
    baseSystem.init();

  printf("Hello, kernel World!\n");

 for(;;) {
    asm("hlt");
 }
}

#ifdef __cplusplus
}
#endif
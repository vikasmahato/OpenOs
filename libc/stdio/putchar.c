#include <stdio.h>

#if defined(__is_openos_kernel)
#include <arch/i386/tty.h>
#endif

int putchar(int ic) {
#if defined(__is_openos_kernel)
  char c = (char)ic;
  t_write(&c, sizeof(c));
#else
// TODO: You need to implement a write system call.
#endif
  return ic;
}

#ifndef _LIBC_ASERT_H_
#define _LIBC_ASERT_H_ 1

#ifdef __cplusplus
extern "C" {
#endif

void panic_assert(const char *file, unsigned int line, const char *desc);

#define assert(b) ((b) ? (void)0 : panic_assert(__FILE__, __LINE__, #b))

#ifdef __cplusplus
}
#endif

#endif  // _LIBC_ASERT_H_

#ifndef LIB_STR_STR_H
#define LIB_STR_STR_H

#include <stdint.h>
#include <stddef.h>

int strcmp(const char* s1, const char* s2);
int32_t strncmp(const char* s1, const char* s2, int32_t n);
void memset(void* ptr, int32_t x, size_t n);

#endif /* LIB_STR_STR_H */

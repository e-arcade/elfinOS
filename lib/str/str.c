#include "str.h"

int32_t strcmp(const char* s1, const char* s2) {
  for (; *s1 != '\0' || *s2 != '\0';) {
    if ((*s1 == '\0') ^ (*s2 == '\0'))
      return 1;
    if (*s1 != *s2)
      return 1;

    s1++; s2++;
  }

  return 0;
}

int32_t strncmp(const char* s1, const char* s2, int32_t n) {
  while (n && *s1 && *s2 && *s1 == *s2) {
    s1++; s2++; n--;
  }
  if (!n || (*s1 == '\0' && *s2 == '\0'))
    return 0;

  return 1;
}

void memset(void* ptr, int32_t x, size_t n) {
  int32_t* tmp = (int32_t*)ptr;
  
  for (int i = 0; i < n; i++) {
    tmp[i] = x;
  }
}

#include "str.h"

void memset(void* ptr, int x, size_t n) {
  int* tmp = (int*)ptr;
  
  for (int i = 0; i < n; i++) {
    tmp[i] = x;
  }
}

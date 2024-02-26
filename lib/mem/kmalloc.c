#include "mem.h"

static void* freeptr;

void* kmalloc(size_t size) {
  // points to 0x100000 (1MB)
  if (!freeptr) {
    freeptr = (void*)(1 << 20);
  }
  void* result = freeptr;
  freeptr += size;

  return result;
}

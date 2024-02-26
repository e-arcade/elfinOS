#ifndef KERNEL_ELF_H
#define KERNEL_ELF_H

#include <stdint.h>

enum {
  EI_NIDENT = 16,
};

// "\x7FELF" in little endian
#define ELF_MAGIC 0x464C457FU  

typedef struct {
  uint32_t      magic;                    // must equal ELF_MAGIC
  unsigned char e_ident[EI_NIDENT - 4];
  uint16_t      e_type;
  uint16_t      e_machine;
  uint32_t      e_version;
  uint32_t      e_entry;
  uint32_t      e_phoff;
  uint32_t      e_shoff;
  uint32_t      e_flags;
  uint16_t      e_ehsize;
  uint16_t      e_phentsize;
  uint16_t      e_phnum;
  uint16_t      e_shentsize;
  uint16_t      e_shnum;
  uint16_t      e_shstrndx;
} elf32_hdr;

typedef struct {
  uint32_t p_type;
  uint32_t p_offset;
  uint32_t p_vaddr;
  uint32_t p_paddr;
  uint32_t p_filesz;
  uint32_t p_memsz;
  uint32_t p_flags;
  uint32_t p_align;
} elf32_phdr;

#endif /* KERNEL_ELF_H */

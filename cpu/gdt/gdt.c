#include "gdt.h"
#include "../../lib/str/str.h"

// null segment, two ring 0 segments, two ring 3 segments, TSS segment
static seg_descriptor gdt[6];

void init_gdt() {
  // 0x100000 in kernelspace = 0x100000
  gdt[SEG_KCODE] = SEG(STA_R|STA_X, 0x0, 0xffffffff, 0);
  gdt[SEG_KDATA] = SEG(STA_W, 0x0, 0xffffffff, 0);
  // program entry point in userspace = USER_BASE + entry point address
  gdt[SEG_UCODE] = SEG(STA_R|STA_X, USER_BASE, 0xffffffff - USER_BASE, DPL_USER);
  gdt[SEG_UDATA] = SEG(STA_W, USER_BASE, 0xffffffff - USER_BASE, DPL_USER);
}

void load_gdt() {
  init_gdt();

  gdt_descriptor gdtr;
  gdtr.size = sizeof(gdt) - 1;
  gdtr.offset = &gdt;

  __asm__("lgdt %0" :: "m"(gdtr));
}

void install_tss(task_state* tss, void* esp) {
  // Ensure the TSS is initially zero'd.
  memset(tss, 0, sizeof(*tss));
  
  gdt[SEG_TSS] = SEG16(STS_T32A, tss, sizeof(*tss) - 1, 0);
  // seting up system descriptor
  gdt[SEG_TSS].s = 0;

  tss->ss0  = SEG_KDATA << 3;
  tss->esp0 = (uint32_t)esp;
  // setting IOPL=0 in eflags *and* iomb beyond the tss segment limit
  // forbids I/O instructions (e.g., inb and outb) from user space
  tss->iomb = (uint16_t)0xFFFF;

  __asm__("ltr %0" :: "r"((uint16_t)(SEG_TSS << 3)));
}

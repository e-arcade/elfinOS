#include "fs.h"
#include "../drivers/ata/ata.h"
#include "../lib/mem/mem.h"
#include "../lib/io/io.h"
#include "../lib/str/str.h"

int32_t get_stat(const char* name, stat *buf) {
  return 1;
}

int32_t read_file(const char* name, void* buf, uint32_t buf_size) {
  int32_t fs_buf = (intptr_t)kmalloc(sector_size);
  // read first fs sector (zero sector for mbr)
  read_sectors_ATA_PIO(fs_buf, base_sector, 1);
  
  int32_t* fs_buf_p = (int32_t*)fs_buf;
  // skip fs reserved (32 bytes)
  fs_buf_p += 8;

  for (int8_t i = 0; i < ents_in_dir; i++) {
    // file offset in sectors
    int32_t offset = *fs_buf_p;

    // skip file offset
    fs_buf_p += 1;
    if (*fs_buf_p > buf_size)
      return -1;
    // file size in sectors
    int32_t size = (*fs_buf_p >> 9) + 1;

    // skip offset/size/reserved (12 bytes)
    fs_buf_p += 2;
    char* filename = (char*)fs_buf_p;
    if (!strcmp(filename, name)) { // we find needed file      
      int32_t buf_address = (intptr_t)buf;
      
      // read file into buffer at 0x400000 address (4BM)
      read_sectors_ATA_PIO(buf_address, base_sector + offset, size);
      return size;
    }

    // skip filename (20 bytes)
    fs_buf_p += 5;
  }

  return -1;
}

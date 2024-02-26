#ifndef FS_FS_H
#define FS_FS_H

#include <stdint.h>

/* directory structure:

   32-byte entries
   ┌───────────────────────────────┐
   │Reserved                       │
   │char[32]                       │
   ├──────┬──────┬────────┬────────┤
   │Offset│Size  │Reserved│Name    │
   │uint32│uint32│uint32  │char[20]│
   ├──────┼──────┼────────┼────────┤
   │ ...  │      │        │        │

   offset is in sectors (zero-based),
   size is in bytes, name is 0-terminated.

*/

enum {
  sector_size = 512,  // sector size
  base_sector = 1,    // number of the fs directory sector relative to which
                      // offsets of the files will be calculated
  ents_in_dir = 15,   // the fs can store 15 files
};

typedef struct {
  uint32_t offset_sectors;
  uint32_t size_bytes;
  uint32_t reserved;
  char name[20];
} dirent;

typedef struct {
  char reserved[32];
  dirent entries[ents_in_dir];
} dir;

typedef struct {
  uint32_t size;
  uint32_t reserved[3];
} stat;

/* find file by name and fill information in buf.
 * returns zero on success, nonzero on failure. */
int32_t get_stat(const char* name, stat *buf);

/* find file by name and read it into buffer with size bufsize.
 * at most (bufsize & ~511) bytes will be read.
 * return number of sectors read or -1 on failure. */
int32_t read_file(const char* name, void* buf, uint32_t buf_size);

#endif /* FS_FS_H */

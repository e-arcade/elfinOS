#include "../fs/fs.h"

#include <stdio.h>
#include <string.h>

char* basename(char* path) {
  char* c = strrchr(path, '/');
  if (c && *c) {
    return c + 1;
  }
  return path;
}

// argv[1] = fs.img (second sector)
// argv[2] = kernel.bin (third sector)
int main(int argc, char* argv[]) {
  char sector[sector_size];
  dir dir = {{0}};

  if (argc < 3) {
    fprintf(stderr, "Usage: %s OUT.FS KERNEL.BIN [FILES...]\n", argv[0]);
    return 1;
  }

  FILE* image = fopen(argv[1], "wb");
  /* if (!image) { */
  /*     perror(argv[1]); */
  /*     return 1; */
  /* } */

  // write 512 zeroed bytes to the fs.img file
  if (fwrite(&dir, sizeof(dir), 1, image) < 1) {
    perror("fwrite");
    return 1;
  }
  uint32_t sector_offset = 1;

  for (int i = 2; i < argc; ++i) {
    // name = kernel.bin
    char* name = argv[i];
    dirent *dirent = &dir.entries[i-2];
    dirent->offset_sectors = sector_offset;
    dirent->size_bytes = 0;

    // open created kernel.bin file
    FILE* file = fopen(name, "rb");
    if (!file) {
      perror(name);
      return 1;
    }

    size_t read_size;
    while ((read_size = fread(sector, 1, sizeof(sector), file))) {
      // append 512 bytes of kernel.bin to the fs.img file
      if (fwrite(sector, 1, sizeof(sector), image) != sizeof(sector)) {
	perror(name);
	return 1;
      }
      sector_offset++;
      dirent->size_bytes += read_size;
    }

    if (fclose(file)) {
      perror(name);
      return 1;
    }

    dirent->reserved = 0;
    dirent->name[sizeof(dirent->name) - 1] = '\0';
    strncpy(dirent->name, basename(name), sizeof(dirent->name) - 1);
  }

  fseek(image, 0, SEEK_SET);
  if (fwrite(&dir, sizeof(dir), 1, image) < 1) {
    perror("fwrite");
    return 1;
  }

  if (fclose(image)) {
    perror(argv[0]);
    return 1;
  }

  return 0;
}

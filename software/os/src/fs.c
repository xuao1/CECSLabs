#include <fs.h>
#include <debug.h>
#include <device.h>

typedef size_t (*ReadFn) (void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn) (const void *buf, size_t offset, size_t len);

typedef struct {
  char *  name;
  size_t  size;
  size_t  disk_offset;
  bool    is_open;
  ReadFn  read;
  WriteFn write;
  size_t  open_offset;
} Finfo;

enum {FD_STDIN, FD_STDOUT, FD_STDERR, FD_EVENTS, FD_FB, FD_DISPINFO};

/* This is the information about all files in disk. */
static Finfo file_table[] __attribute__((used)) = {
  [FD_STDIN]    = {"stdin",           0, 0, 0, invalid_read,  invalid_write},
  [FD_STDOUT]   = {"stdout",          0, 0, 0, invalid_read,  invalid_write},
  [FD_STDERR]   = {"stderr",          0, 0, 0, invalid_read,  invalid_write},
  [FD_EVENTS]   = {"/dev/events",     0, 0, 0, invalid_read,  invalid_write},
  [FD_FB]       = {"/dev/fb",         0, 0, 0, invalid_read,  invalid_write},
  [FD_DISPINFO] = {"/proc/dispinfo",  0, 0, 0, invalid_read,  invalid_write},
  #include "files.h"
};

/* Init the file system. */
void init_fs() {
  Log("Initializing file system...");
  DEV_GPU_CONFIG_T gpu_config = io_read(DEV_GPU_CONFIG);
  file_table[FD_FB].size = gpu_config.height * gpu_config.width * 4;
}

/* Open a file and return the size of file. */
int fs_open(const char *pathname, int flags, int mode) {
  // Lab7 TODO: traverse the file_table and find the file
  int index = -1;
  for (int i = 0; i < sizeof(file_table) / sizeof(Finfo); i++) {
    if (strcmp(file_table[i].name, pathname) == 0) {
      index = i;
    }
  }
  if (index == -1) return -1;
  file_table[index].is_open = true;
  file_table[index].open_offset = 0;
  return index;
}

/* Read `len` bytes of data from the file with `fd` into `buf` */
size_t fs_read(int fd, void *buf, size_t len) {
  // Lab7 TODO: read the file with fd from offset into buf
  if(fd < 0 || fd >= sizeof(file_table) / sizeof(Finfo) || !file_table[fd].is_open) return -1;
  Finfo *f = &file_table[fd];
  if(f->open_offset + len > f->size) len = f->size - f->open_offset;
  size_t bytes_read = f->read(buf, f->disk_offset + f->open_offset, len);
  f->open_offset += bytes_read;
  return bytes_read;
}


/* Write `len` bytes of data from `buf` into the file with `fd` */
size_t fs_write(int fd, const void *buf, size_t len) {
  // Lab7 TODO: write the file with fd from offset by buf
  if(fd < 0 || fd >= sizeof(file_table) / sizeof(Finfo) || !file_table[fd].is_open) return -1;
  Finfo *f = &file_table[fd];
  if(f->open_offset + len > f->size) len = f->size - f->open_offset;
  size_t bytes_written = f->write(buf, f->disk_offset + f->open_offset, len);
  f->open_offset += bytes_written;
  return bytes_written;
}

/* Seek the file with `fd` from the `offset` based on `whence` */
size_t fs_lseek(int fd, size_t offset, int whence) {
  // Lab7 TODO: seek the file with fd from the offset based on whence
  if(fd < 0 || fd >= sizeof(file_table) / sizeof(Finfo) || !file_table[fd].is_open) return -1;
  Finfo *f = &file_table[fd];
  size_t new_offset = f->open_offset;
  switch (whence) {
    case SEEK_SET: new_offset = offset; break;
    case SEEK_CUR: new_offset += offset; break;
    case SEEK_END: new_offset = f->size + offset; break;
    default: return -1;
  }
  if(new_offset < 0 || new_offset > f->size) return -1;
  f->open_offset = new_offset;
  return new_offset;
}

/* Close the file with `fd` */
int fs_close(int fd){
  // Lab7 TODO: close the file with fd
  if(fd < 0 || fd >= sizeof(file_table) / sizeof(Finfo) || !file_table[fd].is_open) return -1;
  file_table[fd].is_open = false;
  file_table[fd].open_offset = 0;
  return 0;
}

uint64_t get_file_offset(const char *filename){
  for (int i = 0; i < sizeof(file_table) / sizeof(Finfo); i++) {
    if (strcmp(file_table[i].name, filename) == 0) {
      return (uint64_t)file_table[i].disk_offset;
    }
  }
  return (uint64_t) -1; 
}




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

  file_table[FD_STDOUT].write = serial_write;
  file_table[FD_STDERR].write = serial_write;
  file_table[FD_EVENTS].read = keyboard_read;
  file_table[FD_FB].write = fb_write;
  file_table[FD_DISPINFO].read = dispinfo_read;
}

// size_t serial_write(const void *buf, size_t offset, size_t len) {
//   char *p = (char *)buf;
//   for (int i = 0; i < len; i++) {
//     putch(p[i]);
//   }
//   return len;
// }

// size_t keyboard_read(void *buf, size_t offset, size_t len) {
//   DEV_INPUT_KEYBRD_T kbd = io_read(DEV_INPUT_KEYBRD);
//   size_t read_len = 0;
//   if (kbd.keydown) {
//     read_len += sprintf(buf, "kd %s\n", keyname[kbd.keycode]);
//   } else if (kbd.keycode != KEY_NONE) {
//     read_len += sprintf(buf, "ku %s\n", keyname[kbd.keycode]);
//   }
//   return read_len;
// }

// size_t dispinfo_read(void *buf, size_t offset, size_t len) {
//   DEV_GPU_CONFIG_T gpu_config = io_read(DEV_GPU_CONFIG);
//   return sprintf(buf, "WIDTH: %d\nHEIGHT: %d", gpu_config.width, gpu_config.height);
// }

// size_t fb_write(const void *buf, size_t offset, size_t len) {
//   DEV_GPU_CONFIG_T gpu_config = io_read(DEV_GPU_CONFIG);
//   offset /= 4;
//   len /= 4;
//   int x = offset % gpu_config.width;
//   int y = offset / gpu_config.width;
//   io_write(DEV_GPU_FBDRAW, x, y, (void*)buf, len, 1, true);
//   return len;
// }

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

  size_t bytes_read = 0;
  if(f->read == keyboard_read){
    bytes_read = f->read(buf, 0, len);
  } 
  else if(f->read == dispinfo_read){
    bytes_read = f->read(buf, 0, len);
  }
  else {
    if(f->open_offset + len > f->size) len = f->size - f->open_offset;
    // bytes_read = f->read(buf, f->disk_offset + f->open_offset, len);
    bytes_read = ramdisk_read(buf, f->disk_offset + f->open_offset, len);
    f->open_offset += bytes_read;
  }
  
  return bytes_read;
}


/* Write `len` bytes of data from `buf` into the file with `fd` */
size_t fs_write(int fd, const void *buf, size_t len) {
  // Lab7 TODO: write the file with fd from offset by buf
  if(fd < 0 || fd >= sizeof(file_table) / sizeof(Finfo) || !file_table[fd].is_open) return -1;
  Finfo *f = &file_table[fd];

  size_t bytes_written = 0;
  if(f->write == serial_write){
    bytes_written = f->write(buf, 0, len);
  }
  else if(f->write == fb_write){
    bytes_written = f->write(buf, 0, len);
  }
  else{
    if(f->open_offset + len > f->size) len = f->size - f->open_offset;
    // bytes_written = f->write(buf, f->disk_offset + f->open_offset, len);
    bytes_written = ramdisk_write(buf, f->disk_offset + f->open_offset, len);
    f->open_offset += bytes_written;
  }

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




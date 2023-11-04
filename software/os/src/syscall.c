#include <debug.h>
#include "syscall.h"
#include <sys/time.h>
#include <mem.h>
#include <fs.h>
#include <proc.h>

void syscall_handle(Context *c) {
  uintptr_t a[4];
  a[0] = c->SYS_NUM;
  a[1] = c->SYS_ARG1;
  a[2] = c->SYS_ARG2;
  a[3] = c->SYS_ARG3;
  switch (a[0]) {
    case SYS_exit: {
      // Lab7 TODO: exit the current process
      halt(0);
      break;
    }
    case SYS_yield: {
      // Lab7 TODO: yield the current 
      yield();
      break;
    }
    case SYS_write: {
      // Lab7 TODO: use `fs_write` to write the file with fd from offset by buf
      // size_t fs_write(int fd, const void *buf, size_t len)
      int fd = (int)a[1];
      const void *buf = (const void *)a[2];
      size_t len = (size_t)a[3];
      size_t bytes_written = fs_write(fd, buf, len);
      c->SYS_RET = bytes_written;
      break;
    }
    case SYS_open: {
      // Lab7 TODO: open the file with name = (char *)a[1]
      // int fs_open(const char *pathname, int flags, int mode)
      char *filename = (char *)a[1];
      int flags = (int)a[2];
      int mode = (int)a[3];
      int fd = fs_open(filename, flags, mode); 
      if(fd == -1) panic("open file failed");
      c->SYS_RET = fd;
      break;
    }
    case SYS_read: {
      // Lab7 TODO: read the file with fd from offset into buf
      // size_t fs_read(int fd, void *buf, size_t len)
      int fd = (int)a[1];
      void *buf = (void *)a[2];
      size_t len = (size_t)a[3];
      size_t bytes_read = fs_read(fd, buf, len);
      c->SYS_RET = bytes_read;
      break;
    }
    case SYS_lseek: {
      // Lab7 TODO: seek the file with fd from the offset based on whence
      // size_t fs_lseek(int fd, size_t offset, int whence)
      int fd = (int)a[1];
      size_t offset = (size_t)a[2];
      int whence = (int)a[3];
      size_t new_offset = fs_lseek(fd, offset, whence);
      c->SYS_RET = new_offset;
      break;
    }
    case SYS_close: {
      // Lab7 TODO: close the file with fd
      // int fs_close(int fd)
      int fd = (int)a[1];
      int ret = fs_close(fd);
      c->SYS_RET = ret;
      break;
    }
    case SYS_gettimeofday: {
      size_t time = io_read(DEV_TIMER_UPTIME).us;
      ((struct timeval *)a[1])->tv_sec = time / 1000000;
      ((struct timeval *)a[1])->tv_usec = time % 1000000;
      c->SYS_RET = 0;
      break;
    }
    case SYS_execve: {
      user_naive_load((const char *) a[1]);
      c->SYS_RET = 0;
      break;
    }
    case SYS_brk: {
      c->SYS_RET = mm_brk(a[1]); 
      break;
    }
    default: panic("Unhandled syscall ID = %d", a[0]);
  }
}

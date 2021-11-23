#include <fs.h>

typedef size_t (*ReadFn) (void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn) (const void *buf, size_t offset, size_t len);

size_t ramdisk_read(void *buf, size_t offset, size_t len);
size_t ramdisk_write(const void *buf, size_t offset, size_t len);

typedef struct {
  char *name;
  size_t size;
  size_t disk_offset;
  ReadFn read;
  WriteFn write;
  size_t open_offset;
} Finfo;

enum {FD_STDIN, FD_STDOUT, FD_STDERR, FD_FB};

size_t invalid_read(void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

size_t invalid_write(const void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

/* This is the information about all files in disk. */
static Finfo file_table[] __attribute__((used)) = {
  [FD_STDIN]  = {"stdin", 0, 0, invalid_read, invalid_write},
  [FD_STDOUT] = {"stdout", 0, 0, invalid_read, invalid_write},
  [FD_STDERR] = {"stderr", 0, 0, invalid_read, invalid_write},
#include "files.h"
};

void init_fs() {
  // TODO: initialize the size of /dev/fb
}

int fs_open(const char *pathname, int flags, int mode){
  // flags, mode not used!
  for (int i = 0; i < LENGTH(file_table); ++i){
    if (strcmp(pathname, file_table[i].name) == 0){
      file_table[i].open_offset = 0;
      return i;
    } 
  }
  assert(0);
}

int fs_close(int fd){
  return 0;
}

size_t fs_read(int fd, void *buf, size_t len){
  assert(fd >= 0 && fd < LENGTH(file_table));
  if (fd == FD_STDIN || fd == FD_STDOUT || fd == FD_STDERR) {return len;} // not sure

  assert(file_table[fd].open_offset + len <= file_table[fd].size);
  ramdisk_read(buf, file_table[fd].disk_offset + file_table[fd].open_offset, len);
  file_table[fd].open_offset += len;
  return len;
}

size_t fs_write(int fd, const void *buf, size_t len){
  assert(fd >= 0 && fd < LENGTH(file_table));
  if (fd == FD_STDOUT || fd == FD_STDERR){
    for (size_t i = 0; i < len; ++i)
      putch(((char *)buf)[i]);
  }
  else {
    assert(file_table[fd].open_offset + len <= file_table[fd].size);
    ramdisk_write(buf, file_table[fd].disk_offset + file_table[fd].open_offset, len);
    file_table[fd].open_offset += len;
  } 
  return len;
}

size_t fs_lseek(int fd, size_t offset, int whence){
  assert(fd >= 0 && fd < LENGTH(file_table));
  if (fd == FD_STDIN || fd == FD_STDOUT || fd == FD_STDERR) {return file_table[fd].open_offset;} // not sure

  switch (whence) {
  case SEEK_SET: file_table[fd].open_offset = offset;                       break;
  case SEEK_CUR: file_table[fd].open_offset += offset;                      break;
  case SEEK_END: file_table[fd].open_offset = offset + file_table[fd].size; break;
  default: assert(0); break;
  }  
  assert(file_table[fd].open_offset <= file_table[fd].size);
  return file_table[fd].open_offset;
}
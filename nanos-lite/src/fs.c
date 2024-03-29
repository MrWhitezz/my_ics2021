#include <fs.h>

typedef size_t (*ReadFn) (void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn) (const void *buf, size_t offset, size_t len);

size_t ramdisk_read(void *buf, size_t offset, size_t len);
size_t ramdisk_write(const void *buf, size_t offset, size_t len);
size_t serial_write(const void *buf, size_t offset, size_t len);
size_t events_read(void *buf, size_t offset, size_t len);
size_t dispinfo_read(void *buf, size_t offset, size_t len);
size_t fb_write(const void *buf, size_t offset, size_t len);

typedef struct {
  char *name;
  size_t size;
  size_t disk_offset;
  ReadFn read;
  WriteFn write;
  size_t open_offset;
} Finfo;

enum {FD_STDIN, FD_STDOUT, FD_STDERR, FD_EVENT, FD_FB, FD_DISP}; // add FD_EVENT

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
  [FD_STDOUT] = {"stdout", 0, 0, invalid_read, serial_write},
  [FD_STDERR] = {"stderr", 0, 0, invalid_read, serial_write},
  [FD_EVENT]  = {"/dev/events", 0, 0, events_read, invalid_write},
  [FD_FB]     = {"/dev/fb", 0, 0, invalid_read, fb_write},
  [FD_DISP]   = {"/proc/dispinfo", 0, 0, dispinfo_read, invalid_write},
#include "files.h"
};

void init_fs() {
  // TODO: initialize the size of /dev/fb
  AM_GPU_CONFIG_T cfg = io_read(AM_GPU_CONFIG);
  file_table[FD_FB].size = cfg.width * cfg.height * sizeof(uint32_t);
}

int fs_open(const char *pathname, int flags, int mode){
  // flags, mode not used!
  for (int i = 0; i < LENGTH(file_table); ++i){
    if (strcmp(pathname, file_table[i].name) == 0){
      file_table[i].open_offset = 0;
      // if (i != FD_EVENT && i != FD_FB)
      //   printf("Successly open file %s\n", pathname);
      return i;
    } 
  }
  printf("Fail to open file %s\n", pathname);
  return -1;
}

int fs_close(int fd){
  return 0;
}

size_t fs_read(int fd, void *buf, size_t len){
  assert(fd >= 0 && fd < LENGTH(file_table));
  if (file_table[fd].read != NULL) {
    // This should only be reached by event_read()
    return file_table[fd].read(buf, file_table[fd].open_offset, len);
  }

  size_t read_len = len;
  if (len > file_table[fd].size - file_table[fd].open_offset)
    read_len = file_table[fd].size - file_table[fd].open_offset;
  ramdisk_read(buf, file_table[fd].disk_offset + file_table[fd].open_offset, read_len);
  file_table[fd].open_offset += read_len;
  return read_len;
}

size_t fs_write(int fd, const void *buf, size_t len){
  assert(fd >= 0 && fd < LENGTH(file_table));
  if (file_table[fd].write != NULL){
    // This should be reached only when stdout, stderr, fb_write
    return file_table[fd].write(buf, file_table[fd].open_offset, len);
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
  if (fd == FD_STDIN || fd == FD_STDOUT || fd == FD_STDERR || fd == FD_EVENT || fd == FD_DISP)
    {assert(0); return -1;} // not sure

  switch (whence) {
  case SEEK_SET: file_table[fd].open_offset = offset;                       break;
  case SEEK_CUR: file_table[fd].open_offset += offset;                      break;
  case SEEK_END: file_table[fd].open_offset = offset + file_table[fd].size; break;
  default: assert(0); break;
  }  
  assert(file_table[fd].open_offset <= file_table[fd].size);
  return file_table[fd].open_offset;
}

// for strace
void get_filename(int fd, char *target) {
  assert(fd >= 0 && fd < LENGTH(file_table));
  strcpy(target, file_table[fd].name);
}
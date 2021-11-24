#include <common.h>
#include "syscall.h"
#include <sys/time.h>

int fs_open(const char *pathname, int flags, int mode);
int fs_close(int fd);
size_t fs_read(int fd, void *buf, size_t len);
size_t fs_write(int fd, const void *buf, size_t len);
size_t fs_lseek(int fd, size_t offset, int whence);

static void sys_yield(Context *c){
  yield();
  c->GPRx = 0;
}

static void sys_exit(Context *c){
  halt(c->GPR2);
}

static void sys_open(Context *c, const char *filename, int flags, int mode){
  int fd = fs_open(filename, flags, mode);
  c->GPRx = fd;
}

static void sys_read(Context *c, int fd, void *buf, size_t len){
  c->GPRx = fs_read(fd, buf, len);
}

static void sys_write(Context *c, int fd, void *buf, size_t count) { // bug here
  c->GPRx = fs_write(fd, buf, count);
}

static void sys_close(Context *c, int fd){
  c->GPRx = fs_close(fd);
}

static void sys_lseek(Context *c, int fd, size_t offset, int whence){
  c->GPRx = fs_lseek(fd, offset, whence);
}

static void sys_brk(Context *c) {
  c->GPRx = 0;
}

static void sys_gettimeofday(Context *c, struct timeval *tv, struct timezone *tz){
  // not sure
  assert(tv != NULL);
  uint64_t us =  io_read(AM_TIMER_UPTIME).us;
  printf("us = %x %x\n", (uint32_t)(us >> 32), (uint32_t)us);
  tv->tv_sec = us / 1000000ULL;
  tv->tv_usec = us % 1000000ULL;
  if (tv->tv_sec != 0){
    // printf("tv_sec is read as %d\n", (uint32_t)tv->tv_sec);
  }
  printf("sec is read as %x\n", (uint32_t)tv->tv_sec);
  // printf("usec is read as %x\n", tv->tv_usec);
  c->GPRx = 0;
}

char trace_filename[64];
void get_filename(int fd, char *target);

void strace(Context *c, uintptr_t a7, uintptr_t a1, uintptr_t a2, uintptr_t a3){
  switch (a7){
    case SYS_read:
    case SYS_write:
    case SYS_close:
    case SYS_lseek: get_filename(a1, trace_filename); break;
    default: break; 
  }

  switch (a7){
    case SYS_exit:  printf("System Call: exit\n");  break;
    case SYS_yield: printf("System Call: yield\n"); break;
    case SYS_open:  printf("System Call: open %s\n", (char *) a1);  break;
    case SYS_read:  printf("System Call: read %s %d bytes\n", trace_filename, c->GPRx);  break;//wrong
    case SYS_write: printf("System Call: write %s %d bytes\n", trace_filename, c->GPRx); break;
    case SYS_close: printf("System Call: close %s\n", trace_filename); break;
    case SYS_lseek: printf("System Call: lseek %s offset = %d\n", trace_filename, c->GPRx); break;
    case SYS_brk:   printf("System Call: brk\n");   break;
    case SYS_gettimeofday: printf("System Call: gettimeofday\n"); break;
    

    default: printf("Unknown System Call\n");
  }

}
void do_syscall(Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1; // a7
  a[1] = c->GPR2;
  a[2] = c->GPR3;
  a[3] = c->GPR4;


  switch (a[0]) {
    case SYS_exit:  sys_exit(c);  break;
    case SYS_yield: sys_yield(c); break;
    case SYS_open:  sys_open(c, (char *)a[1], a[2], a[3]); break;
    case SYS_read:  sys_read(c, a[1], (void *)a[2], a[3]); break;
    case SYS_write: sys_write(c, a[1], (void *)a[2], a[3]); break;
    case SYS_close: sys_close(c, a[1]); break;
    case SYS_lseek: sys_lseek(c, a[1], a[2], a[3]); break;
    case SYS_brk:   sys_brk(c);   break;
    case SYS_gettimeofday: sys_gettimeofday(c, (struct timeval *)a[1], (struct timezone *)a[2]); break;
    case -1       : printf("Hit the Strange yield!\n"); break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }
  // STRACE
  // strace(c, a[0], a[1], a[2], a[3]);
}
#include <common.h>
#include "syscall.h"

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
  // if (fd == 1 || fd == 2) {
  //   // printf("count = %d\n", count);
  //   for (int i = 0; i < count; ++i)
  //     {putch(((char *)buf)[i]);}
  // }
  // c->GPRx = count;
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

void strace(uintptr_t a7){
  switch (a7){
    case SYS_exit:  printf("System Call: exit\n");  break;
    case SYS_yield: printf("System Call: yield\n"); break;
    case SYS_open:  printf("System Call: open\n");  break;
    case SYS_read:  printf("System Call: read\n");  break;
    case SYS_write: printf("System Call: write\n"); break;
    case SYS_close: printf("System Call: close\n"); break;
    case SYS_lseek: printf("System Call: lseek\n"); break;
    case SYS_brk:   printf("System Call: brk\n");   break;
    

    default: printf("Unknown System Call\n");
  }

}
void do_syscall(Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1; // a7
  a[1] = c->GPR2;
  a[2] = c->GPR3;
  a[3] = c->GPR4;


  // printf("R[a7] = 0x%x\n", a[0]);
  // printf("R[a0] = 0x%x\n", a[1]);
  // printf("R[a1] = 0x%x\n", a[2]);
  // printf("R[a2] = 0x%x\n", a[3]);

  // STRACE
  strace(a[0]);
  

  switch (a[0]) {
    case SYS_exit:  sys_exit(c);  break;
    case SYS_yield: sys_yield(c); break;
    case SYS_open:  sys_open(c, (char *)a[1], a[2], a[3]); break;
    case SYS_read:  sys_read(c, a[1], (void *)a[2], a[3]); break;
    case SYS_write: sys_write(c, a[1], (void *)a[2], a[3]); break;
    case SYS_close: sys_close(c, a[1]); break;
    case SYS_lseek: sys_lseek(c, a[1], a[2], a[3]); break;
    case SYS_brk:   sys_brk(c);   break;
    case -1       : printf("Hit the Strange yield!\n"); break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }
}
#include <common.h>
#include "syscall.h"

enum {SYS_EXIT = 0, SYS_YIELD, SYS_WRITE = 4};

static void sys_yield(Context *c){
  yield();
  c->GPRx = 0;
}

static void sys_exit(Context *c){
  halt(c->GPR2);
}

static void sys_write(Context *c, int fd, void *buf, size_t count) {
  if (fd == 1 || fd == 2) {
    for (int i = 0; i < count; ++i)
      putch(((char *)buf)[i]);
  }
  c->GPRx = count;
}

void strace(uintptr_t a7){
  switch (a7){
    case SYS_EXIT:  printf("System Call: exit\n");  break;
    case SYS_YIELD: printf("System Call: yield\n"); break;
    case SYS_WRITE: printf("System Call: write\n"); break;
    

    default: printf("Unknown System Call\n");
  }

}
void do_syscall(Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1; // a7
  a[1] = c->GPR2;
  a[2] = c->GPR3;
  a[3] = c->GPR4;


  printf("R[a7] = 0x%x\n", a[0]);
  printf("R[a0] = 0x%x\n", a[1]);
  printf("R[a1] = 0x%x\n", a[2]);
  printf("R[a2] = 0x%x\n", a[3]);

  // STRACE
  strace(a[0]);
  

  switch (a[0]) {
    case SYS_EXIT:  sys_exit(c);  break;
    case SYS_YIELD: sys_yield(c); break;
    case SYS_WRITE: sys_write(c, a[1], (void *)a[2], a[3]); break;
    case -1       : printf("Hit the Strange yield!\n"); break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }
}
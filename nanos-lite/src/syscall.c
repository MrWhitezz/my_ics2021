#include <common.h>
#include "syscall.h"

enum {
  SYS_exit, SYS_yield,
  SYS_open, SYS_read, SYS_write,
  SYS_kill,
  SYS_getpid,
  SYS_close,
  SYS_lseek,
  SYS_brk,
  SYS_fstat,
  SYS_time,
  SYS_signal,
  SYS_execve,
  SYS_fork,
  SYS_link,
  SYS_unlink,
  SYS_wait,
  SYS_times, SYS_gettimeofday
};

static void sys_yield(Context *c){
  yield();
  c->GPRx = 0;
}

static void sys_exit(Context *c){
  halt(c->GPR2);
}

static void sys_write(Context *c, int fd, void *buf, size_t count) { // bug here
  if (fd == 1 || fd == 2) {
    // printf("count = %d\n", count);
    for (int i = 0; i < count; ++i)
      {putch(((char *)buf)[i]);}
  }
  c->GPRx = count;
}

static void sys_brk(Context *c) {
  c->GPRx = 0;
}

void strace(uintptr_t a7){
  switch (a7){
    case SYS_exit:  printf("System Call: exit\n");  break;
    case SYS_yield: printf("System Call: yield\n"); break;
    case SYS_write: printf("System Call: write\n"); break;
    case SYS_brk:   printf("System Call: brk\n"); break;
    

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
    case SYS_write: sys_write(c, a[1], (void *)a[2], a[3]); break;
    case SYS_brk:   sys_brk(c);   break;
    case -1       : printf("Hit the Strange yield!\n"); break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }
}
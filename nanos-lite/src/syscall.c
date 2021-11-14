#include <common.h>
#include "syscall.h"

static void sys_yield(Context *c){
  yield();
  c->GPRx = 0;
}

static void sys_exit(Context *c){
  halt(c->GPR2);
}

void strace(uintptr_t a7){
  switch (a7){
    case EVENT_NULL:  printf("System Call: exit\n");  break;
    case EVENT_YIELD: printf("System Call: yield\n"); break;

    default: printf("Unknown System Call\n");
  }

}
void do_syscall(Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1; // a7
  a[1] = c->GPR2;
  a[2] = c->GPR3;
  a[2] = c->GPR4;


  // printf("R[a7] = 0x%x\n", a[0]);
  // printf("R[a0] = 0x%x\n", a[1]);
  // printf("R[a1] = 0x%x\n", a[2]);
  // printf("R[a2] = 0x%x\n", a[3]);

  // STRACE
  strace(a[0]);
  

  switch (a[0]) {
    case EVENT_NULL:  sys_exit(c);  break;
    case EVENT_YIELD: sys_yield(c); break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }
}
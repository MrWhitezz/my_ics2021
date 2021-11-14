#include <common.h>
#include "syscall.h"

static void sys_yield(Context *c){
  yield();
  c->GPRx = 0;
}

static void sys_exit(Context *c){
  halt(0);
}

void do_syscall(Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;

  switch (a[0]) {
    case EVENT_NULL:  sys_exit(c);  break;
    case EVENT_YIELD: sys_yield(c); break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }
}
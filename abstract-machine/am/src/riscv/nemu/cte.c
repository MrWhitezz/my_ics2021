#include <am.h>
#include <riscv/riscv.h>
#include <klib.h>

static Context* (*user_handler)(Event, Context*) = NULL;

void ecall_judge(Context *c, Event *e){
  enum {SYS_EXIT = 0, SYS_YIELD, SYS_WRITE = 4};
  assert(c->mcause == 0xb);
  switch (c->GPR1){
    case -1:        e->event = EVENT_YIELD;   break;
    case SYS_EXIT:
    case SYS_YIELD:
    case SYS_WRITE: e->event = EVENT_SYSCALL; break;
    default:        e->event = EVENT_ERROR;   break;
  }
}

Context* __am_irq_handle(Context *c) {
  printf("SR[cause] = 0x%x\n", c->mcause);
  printf("SR[epc] = 0x%x\n", c->mepc);
  printf("SR[status] = 0x%d\n", c->mstatus);
  if (user_handler) {
    Event ev = {0};
    switch (c->mcause) {
      case 0xb: ecall_judge(c, &ev);       break;
      default:  ev.event = EVENT_ERROR;    break;
    }
    

    c = user_handler(ev, c);
    assert(c != NULL);
  }

  return c;
}

extern void __am_asm_trap(void);

bool cte_init(Context*(*handler)(Event, Context*)) {
  // initialize exception entry
  asm volatile("csrw mtvec, %0" : : "r"(__am_asm_trap));

  // register event handler
  user_handler = handler;

  return true;
}

Context *kcontext(Area kstack, void (*entry)(void *), void *arg) {
  return NULL;
}

void yield() {
  asm volatile("li a7, -1; ecall");
}

bool ienabled() {
  return false;
}

void iset(bool enable) {
}

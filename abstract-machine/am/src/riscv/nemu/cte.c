#include <am.h>
#include <riscv/riscv.h>
#include <klib.h>

static Context* (*user_handler)(Event, Context*) = NULL;

Context* __am_irq_handle(Context *c) {
  if (user_handler) {
    Event ev = {0};
    switch (c->mcause) {
      case 0xb: ev.event = EVENT_YIELD; break;
      default: ev.event = EVENT_ERROR; printf("EVENT_ERROR!\n"); break;
    }

    c = user_handler(ev, c);
    assert(c != NULL);
  }
  // test for Context
  for (int i = 0; i < 32; ++i)
      printf("R[%d] = 0x%d\t", i, c->gpr[i]);
  printf("SR[cause] = 0x%d\n", c->mcause);
  printf("SR[epc] = 0x%d\n", c->mepc);
  printf("SR[status] = 0x%d\n", c->mstatus);

  printf("test for __am_irq_handle\n");

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

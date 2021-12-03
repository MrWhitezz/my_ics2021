#include <am.h>
#include <riscv/riscv.h>
#include <klib.h>

static Context* (*user_handler)(Event, Context*) = NULL;

void ecall_judge(Context *c, Event *e){
  assert(c->mcause == 0xb);
  switch (c->GPR1){
    case -1:        e->event = EVENT_YIELD;   break;
    default:        e->event = EVENT_ERROR;   break;
  }
  if (c->GPR1 >=0 && c->GPR1 <= 19) {e->event = EVENT_SYSCALL;}
}

Context* __am_irq_handle(Context *c) {
  // printf("SR[cause] = 0x%x\n", c->mcause);
  // printf("SR[epc] = 0x%x\n", c->mepc);
  // printf("SR[status] = 0x%d\n", c->mstatus);
  if (user_handler) {
    Event ev = {0};
    switch (c->mcause) {
      case 0xb: ecall_judge(c, &ev);       break;
      default:  ev.event = EVENT_ERROR;    break;
    }
    

    c = user_handler(ev, c);
    assert(c != NULL);
  }
  assert(c != NULL);
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

#define CONTEXT_SIZE  (32 + 3 + 1)
#define OFFSET_SP     2 
#define OFFSET_CAUSE  32
#define OFFSET_STATUS 33
#define OFFSET_EPC    34


Context *kcontext(Area kstack, void (*entry)(void *), void *arg) {
  uint32_t *stack_p = kstack.end;
  // uint32_t *heap_p  = kstack.start;
  stack_p -= CONTEXT_SIZE;
  *(stack_p + OFFSET_EPC) = (uintptr_t)entry - 4;
  *(stack_p + OFFSET_SP) = (uintptr_t)stack_p;
  
  // Context cp is set in nanos
  // *heap_p = (uintptr_t)stack_p;
  return (Context *)stack_p;
}

void yield() {
  asm volatile("li a7, -1; ecall");
}

bool ienabled() {
  return false;
}

void iset(bool enable) {
}

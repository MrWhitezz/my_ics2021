#include <am.h>
#include <riscv/riscv.h>
#include <klib.h>

void __am_get_cur_as(Context *c);
void __am_switch(Context *c);

static Context* (*user_handler)(Event, Context*) = NULL;

#define IRQ_TIMER 0x80000007

void ecall_judge(Context *c, Event *e){
  assert(c->mcause == 0xb);
  switch (c->GPR1){
    case -1:        e->event = EVENT_YIELD;   break;
    default:        e->event = EVENT_ERROR;   break;
  }
  if (c->GPR1 >=0 && c->GPR1 <= 19) {e->event = EVENT_SYSCALL;}
}

Context* __am_irq_handle(Context *c, uintptr_t old_sp) {
  assert(c != NULL);
  __am_get_cur_as(c); 
  if (user_handler) {
    Event ev = {0};
    switch (c->mcause) {
      case 0xb      : ecall_judge(c, &ev);        break;
      case IRQ_TIMER: ev.event = EVENT_IRQ_TIMER; c->mepc -= 4; break;
      default       : ev.event = EVENT_ERROR;     break;
    }
    c = user_handler(ev, c);
    assert(c != NULL);
  }
  // printf("before switch\n");
  __am_switch(c);
  // printf("tried to switch with satp %p\n", c->pdir);
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

#define CONTEXT_SIZE  (32 + 3 + 1 + 1)
#define OFFSET_SP     2 
#define OFFSET_A0     10
#define OFFSET_CAUSE  32
#define OFFSET_STATUS 33
#define OFFSET_EPC    34
#define OFFSET_PDIR   35
#define OFFSET_NP     36


Context *kcontext(Area kstack, void (*entry)(void *), void *arg) {
  uint32_t *stack_p = kstack.end;
  printf("kstack: %x to %x\n", kstack.start, kstack.end);
  // uint32_t *heap_p  = kstack.start;
  stack_p -= CONTEXT_SIZE;
  *(stack_p + OFFSET_EPC)    = (uintptr_t)entry - 4;
  *(stack_p + OFFSET_A0)     = (uintptr_t)arg;
  *(stack_p + OFFSET_SP)     = (uintptr_t)stack_p;
  *(stack_p + OFFSET_STATUS) = (uintptr_t)0x1808;
  *(stack_p + OFFSET_PDIR)   = (uintptr_t)0;
  
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

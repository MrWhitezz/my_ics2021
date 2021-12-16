#include <common.h>

void do_syscall(Context *c);
Context* schedule(Context *prev);

static Context* do_event(Event e, Context* c) {
  switch (e.event) {
    case EVENT_YIELD: /*printf("Hit the good yield!\n"); */  return schedule(c); break;
    case EVENT_SYSCALL:  do_syscall(c); break;
    case EVENT_IRQ_TIMER: Log("Hey timer!\n"); return schedule(c); break;
    case EVENT_NULL:
    case EVENT_IRQ_IODEV:
    default: panic("Unhandled event ID = %d", e.event);
  }

  return c;
}

void init_irq(void) {
  Log("Initializing interrupt/exception handler...");
  cte_init(do_event);
}

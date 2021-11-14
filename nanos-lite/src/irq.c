#include <common.h>

void do_syscall(Context *c);

static Context* do_event(Event e, Context* c) {
  switch (e.event) {
    case EVENT_YIELD: printf("Hit the good yield!\n"); break;
    case EVENT_SYSCALL: printf("System CALL\n"); do_syscall(c); break;
    default: panic("Unhandled event ID = %d", e.event);
  }

  return c;
}

void init_irq(void) {
  Log("Initializing interrupt/exception handler...");
  cte_init(do_event);
}

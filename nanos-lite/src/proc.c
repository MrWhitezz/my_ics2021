#include <proc.h>

#define MAX_NR_PROC 4

static PCB pcb[MAX_NR_PROC] __attribute__((used)) = {};
static PCB pcb_boot = {};
PCB *current = NULL;
void naive_uload(PCB *pcb, const char *filename);
uintptr_t loader(PCB *pcb, const char *filename);

void context_kload(PCB *pcb1, void(* func)(void *), void *arg){
  Area pcb_stack = RANGE(pcb1, (void *)pcb1 + sizeof(PCB));
  Context *c = kcontext(pcb_stack, func, arg);
  pcb1->cp = c;
}

void context_uload(PCB *pcb1, const char *fname) {
  // tmp load
  uintptr_t entry = loader(pcb, fname);
  if (entry == -1){
    printf("Fail to context_uload!!!\n");
  }

  Area pcb_stack = RANGE(pcb1, (void *)pcb1 + sizeof(PCB));
  Context *c = ucontext(NULL, pcb_stack, (void *)entry); 
  // Context *c = kcontext(pcb_stack, (void *)entry, NULL); 
  c->GPRx = (uintptr_t)heap.end;
  printf("heap.end = %p\n", heap.end);
  pcb1->cp = c;
}

void switch_boot_pcb() {
  current = &pcb_boot;
}

void hello_fun(void *arg) {
  int j = 1;
  while (1) {
    Log("Hello World from Nanos-lite with arg '%p' for the %dth time!", (uintptr_t)arg, j);
    j ++;
    yield();
  }
}

void init_proc() {
  context_kload(&pcb[0], hello_fun, (void *)0x1);
  // context_uload(&pcb[0], "/bin/hello");
  context_uload(&pcb[1], "/bin/timer-test");
  // context_kload(&pcb[1], hello_fun, (void *)0x2);
  switch_boot_pcb();
  yield();

  Log("Initializing processes...");

  // load program here
  naive_uload(NULL, "/bin/nterm");

}

Context* schedule(Context *prev) {
  current->cp = prev;

  current = (current == &pcb[0] ? &pcb[1] : &pcb[0]);

  assert(current->cp != NULL);

  return current->cp;
}

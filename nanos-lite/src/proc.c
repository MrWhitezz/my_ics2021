#include <proc.h>

#define MAX_NR_PROC 4
#define UNSIPICIED_SZ 128 // 128 + 128 < 8 * 4096
#define POINTER_BYTES 4

// why should pcb and pcb_boot be static?
PCB pcb[MAX_NR_PROC] __attribute__((used)) = {};
PCB pcb_boot = {};
PCB *current = NULL;

void context_kload(PCB *pcb1, void(* func)(void *), void *arg){
  Area pcb_stack = RANGE(pcb1, (void *)pcb1 + sizeof(PCB));
  Context *c = kcontext(pcb_stack, func, arg);
  pcb1->cp = c;
}

int context_uload(PCB *pcb1, const char *fname, char *const argv[], char *const envp[]){
  if (envp != NULL) {
    printf("Enter the loop\n");
    while (envp[0] != NULL) {
      printf("QAQ\n");
      printf("envp: %p\n", envp);
      printf("envp addr: %p\n", envp[0]);
      printf("envp first: %s\n", envp[0]);
      printf("envp[%d] = %s\n", 0, envp[0]);
      printf("qaq\n");
      break;
    }
  }
 
  // tmp load
  uintptr_t entry = loader(pcb, fname);
  if (entry == -1){
    printf("Fail to context_uload!!!\n");
    return -1;
  }

  Area pcb_stack = RANGE(pcb1, (void *)pcb1 + sizeof(PCB));
  Context *c = ucontext(NULL, pcb_stack, (void *)entry); 

  // uint8_t *u_stack = heap.end;
  uint8_t *u_stack = new_page(8);
  int argc = 0, envc = 0;
  int str_area_sz = 0;
  printf("Native debug\n");
  // calculate space for string area and argc,envc
  if (argv != NULL) 
    while (argv[argc] != NULL) {
      str_area_sz += strlen(argv[argc]) + 1;
      argc++;
    }
  printf("Native debug\n");
  if (envp != NULL) {
    printf("Enter the loop\n");
    while (envp[envc] != NULL) {
      printf("QAQ\n");
      printf("envc = %d\n", envc);
      printf("envp: %p\n", envp);
      printf("envp addr: %p\n", envp[0]);
      printf("envp first: %s\n", envp[0]);
      printf("envp[%d] = %s\n", envc, envp[envc]);
      str_area_sz += strlen(envp[envc]) + 1;
      printf("qaq\n");
      envc++;
    }
  }
  printf("Native debug\n");
  char **u_argv = malloc(argc * sizeof(char *));
  char **u_envp = malloc(envc * sizeof(char *));
  // Need give space for stack
  u_stack -= UNSIPICIED_SZ * 2 + str_area_sz + (envc + 1 + argc + 1) * POINTER_BYTES + sizeof(int);
  // assert(UNSIPICIED_SZ * 2 + str_area_sz + (envc + 1 + argc + 1) * POINTER_BYTES + sizeof(int) < 8 * PGSIZE);
  printf("Native debug\n");
  uintptr_t u_sp_ret = (uintptr_t)u_stack;
  printf("ustack = %p\n",u_stack);
  printf("heap.end = %p\n", heap.end);
  *(int *)(u_stack) = argc;
  printf("Native debug\n");
  int stack_off = 0;
  stack_off = envc + 1 + argc + 1;
  u_stack += sizeof(int);

  printf("Native debug\n");


  // assign string
  u_stack += stack_off * POINTER_BYTES + UNSIPICIED_SZ;
  for (int i = 0; i < argc; ++i) {
    u_argv[i] = (char *)u_stack;
    strcpy((char *)u_stack, argv[i]);
    u_stack += strlen(argv[i]) + 1;
  }
  for (int i = 0; i < envc; ++i) {
    u_envp[i] = (char *)u_stack;
    strcpy((char *)u_stack, envp[i]);
    u_stack += strlen(envp[i]) + 1;
  }
  
  
  // assign argv, envp
  u_stack = (uint8_t *)u_sp_ret + sizeof(int);
  stack_off = 0;

  for (int i = 0; i < argc; ++i){
    *(char **)(u_stack + stack_off * POINTER_BYTES) = u_argv[i]; // wrong
    stack_off++;
  }
  *(char **)(u_stack + (stack_off) * POINTER_BYTES) = NULL;
  stack_off++;
  for (int i = 0; i < envc; ++i){
    *(char **)(u_stack + stack_off * POINTER_BYTES) = u_envp[i]; // wrong
    stack_off++;
  }
  *(char **)(u_stack + stack_off * POINTER_BYTES) = NULL;
  stack_off++;

  free(u_argv);
  free(u_envp);
  c->GPRx = u_sp_ret;
  // for native(GPR4 == rcx, GPRx == rax), I don't know why rax do not work
  c->GPR4 = u_sp_ret;
  pcb1->cp = c;
  return 0;
}

void switch_boot_pcb() {
  current = &pcb_boot;
}

void hello_fun(void *arg) {
  int j = 1;
  while (1) {
    if (j % 10000 == 0)
      Log("Hello World from Nanos-lite with arg '%p' for the %dth time!", (uintptr_t)arg, j);
    j ++;
    yield();
  }
}
char skip[10] = "--skip";
char *argv_pal[1];

void init_proc() {
  context_kload(&pcb[0], hello_fun, (void *)0x1);
  // context_uload(&pcb[0], "/bin/hello");
  argv_pal[0] = skip;
  // context_uload(&pcb[1], "/bin/pal", argv_pal, NULL);
  // context_kload(&pcb[1], hello_fun, (void *)0x2);
  switch_boot_pcb();
  // yield();

  Log("Initializing processes...");

  // load program here
  naive_uload(&pcb[1], "/bin/nterm");

}

Context* schedule(Context *prev) {
  current->cp = prev;

  current = (current == &pcb[0] ? &pcb[1] : &pcb[0]);

  assert(current->cp != NULL);

  return current->cp;
}

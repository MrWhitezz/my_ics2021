#include <proc.h>

#define MAX_NR_PROC 4
#define UNSIPICIED_SZ 128 // 128 + 128 < 8 * 4096
#define POINTER_BYTES sizeof(char *)
#define PROTECT_ENV
// #define ARGS_MANAGE

// why should pcb and pcb_boot be static?
int fg_pcb = 1;

PCB pcb[MAX_NR_PROC] __attribute__((used)) = {};
PCB pcb_boot = {};
PCB *current = NULL;

void context_kload(PCB *pcb1, void(* func)(void *), void *arg){
  Area pcb_stack = RANGE(pcb1, (void *)pcb1 + sizeof(PCB));
  Context *c = kcontext(pcb_stack, func, arg);
  pcb1->cp = c;
}

int context_uload(PCB *pcb1, const char *fname, char *const argv[], char *const envp[]){
  // printf("This is context_uload\n");
  // if (envp != NULL){
  //   printf("envp: %p\n", envp);
  //   if (envp[0] != NULL){
  //     printf("envp[0]: %p\n", envp[0]);
  //   }
  // }
  // if (argv != NULL){
  //   printf("argv: %p\n", argv);
  //   if (argv[0] != NULL){
  //     printf("argv[0]: %p\n", argv[0]);
  //   }
  // }

  #ifndef PROTECT_ENV
  // tmp load
  printf("Native debug\n");
  uintptr_t entry = loader(pcb, fname);
  printf("Native debug\n");
  if (entry == -1){
    printf("Fail to context_uload!!!\n");
    return -1;
  }

  Area pcb_stack = RANGE(pcb1, (void *)pcb1 + sizeof(PCB));
  Context *c = ucontext(NULL, pcb_stack, (void *)entry); 
  #endif
 
  // not sure
  // printf("Native debug\n");
  protect(&pcb1->as); 
  // printf("Native debug\n");
  void *u_stack_end = pcb1->as.area.end;
  void *u_stack_beg = u_stack_end - 8 * PGSIZE;
  void *p_page = pg_alloc(8);
  for (int i = 0; i < 8; ++i){
    map(&pcb1->as, u_stack_beg + i * PGSIZE, p_page + i * PGSIZE, MMAP_READ | MMAP_WRITE);
  }
  uint8_t *u_stack = u_stack_end;

  #ifdef ARGS_MANAGE

  int argc = 0, envc = 0;
  int str_area_sz = 0;
  // printf("Native debug\n");

  // calculate space for string area and argc,envc
  if (argv != NULL) 
    while (argv[argc] != NULL) {
      str_area_sz += strlen(argv[argc]) + 1;
      argc++;
    }
  // printf("Native debug\n");

  if (envp != NULL) {
    while (envp[envc] != NULL) {
      str_area_sz += strlen(envp[envc]) + 1;
      envc++;
    }
  }
  char **u_argv = malloc(argc * sizeof(char *));
  char **u_envp = malloc(envc * sizeof(char *));
  // Need give space for stack
  u_stack -= UNSIPICIED_SZ * 2 + str_area_sz + (envc + 1 + argc + 1) * POINTER_BYTES + sizeof(int);
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
  
  // printf("Native debug\n");
  
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
  #endif
  #ifndef ARGS_MANAGE
  uintptr_t u_sp_ret = (uintptr_t)u_stack - 2 * UNSIPICIED_SZ;
  #endif

  #ifdef PROTECT_ENV
  // tmp load
  // printf("Native debug\n");
  uintptr_t entry = loader(pcb1, fname);
  // printf("Native debug\n");
  if (entry == -1){
    printf("Fail to context_uload!!!\n");
    return -1;
  }

  Area pcb_stack = RANGE(pcb1, (void *)pcb1 + sizeof(PCB));
  Context *c = ucontext(&pcb1->as, pcb_stack, (void *)entry); 

  #endif

  c->GPRx = u_sp_ret;
  // for native(GPR4 == rcx, GPRx == rax), I don't know why rax do not work
  c->GPR4 = u_sp_ret;
  pcb1->cp = c;
  printf("succss load!\n");
  // printf("load with pdir %p\n", pcb1->cp->pdir);
  return 0;
}

void switch_boot_pcb() {
  current = &pcb_boot;
}

void hello_fun(void *arg) {
  int j = 1;
  while (1) {
    // if (j % 10 == 0)
      Log("Hello World from Nanos-lite with arg '%p' for the %dth time!", (uintptr_t)arg, j);
    j ++;
    yield();
  }
}
char skip[10] = "--skip";
char exec_arg[15] = "/bin/exec-test";
char *argv_pal[1];

void init_proc() {
  fg_pcb = 1;
  // context_kload(&pcb[0], hello_fun, (void *)0x1);
  context_uload(&pcb[0], "/bin/hello", NULL, NULL);
  argv_pal[0] = exec_arg;
  // context_kload(&pcb[1], hello_fun, (void *)0x2);
  context_uload(&pcb[1], "/bin/pal", argv_pal, NULL);

  context_uload(&pcb[2], "/bin/bird", argv_pal, NULL);
  switch_boot_pcb();
  yield();

  Log("Initializing processes...");

  // load program here
  // naive_uload(&pcb[1], "/bin/exec-test");

}

uint32_t schedule_cnt = 1;

Context* schedule(Context *prev) {
  // ATTENSION: The first process cannot be kernal process!!!
  current->cp = prev;

  // current = (current == &pcb[1] ? &pcb[0] : &pcb[1]);
  if (schedule_cnt++ % 100 == 0){
    current = &pcb[0];
    // Log("Switch to hello\n");
  }
  else {
    // Log("Switch to pal\n");
    current = &pcb[fg_pcb];
  }
  assert(current->cp != NULL);

  return current->cp;
}

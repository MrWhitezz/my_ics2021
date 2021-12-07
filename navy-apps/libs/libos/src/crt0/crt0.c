#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

int main(int argc, char *argv[], char *envp[]);
extern char **environ;
void call_main(uintptr_t *args) {
  printf("call_main! with args %p\n", args);
  uint32_t *u_stack = args;
  int argc = *(int *)u_stack;
  char **argv = (char **)(u_stack + 1);
  char **envp = (char **)(u_stack + 1 + argc);
  // char *empty[] =  {NULL };
  printf("argc = %d\n", argc);
  for (int i = 0; i < argc; ++i){
    printf("arcv[%d] = %s\n", i, argv[i]);
  }
  environ = envp;
  assert(0);
  exit(main(argc, argv, envp));
  assert(0);
}

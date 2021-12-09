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
  printf("size of char *: %d\n", sizeof(char *));
  printf("argv = %p\n", (void *)argv);
  printf("argc = %d\n", argc);
  for (int i = 0; i < argc; ++i){
    printf("arcv[%d] at %p\n", i, argv[i]);
    printf("arcv[%d] = %s\n", i, argv[i]);
  }
  if (envp != NULL){
    printf("envp: %p\n", envp);
    if (envp[0] != NULL){
      printf("envp[0]: %p\n", envp[0]);
    }
  }
  environ = envp;
  exit(main(argc, argv, envp));
  assert(0);
}

#include <nterm.h>
#include <stdarg.h>
#include <unistd.h>
#include <SDL.h>

char handle_key(SDL_Event *ev);

static void sh_printf(const char *format, ...) {
  static char buf[256] = {};
  va_list ap;
  va_start(ap, format);
  int len = vsnprintf(buf, 256, format, ap);
  va_end(ap);
  term->write(buf, len);
}

static void sh_banner() {
  sh_printf("Built-in Shell in NTerm (NJU Terminal)\n\n");
}

static void sh_prompt() {
  sh_printf("sh> ");
}

const char *cmd_exit = "exit\n";
#define MAX_ARG_NUMS 3
#define MAX_CMD_LEN 64
char new_cmd[MAX_CMD_LEN];
char *argv_cmd[MAX_ARG_NUMS];
static const char* cmd_rm_endl(const char *cmd){
  int arg_num = 0;
  bool is_proper_cmd = false;
  for (int i = 0; i < MAX_CMD_LEN; ++i){
    new_cmd[i] = cmd[i];
    if (cmd[i] == '\n'){
      new_cmd[i] = '\0';
      is_proper_cmd = true;
      break;
    }
  }
  if (is_proper_cmd == false){
    printf("The Command is TOO LONG!!!\n");
    return NULL;
  }
  argv_cmd[arg_num++] = new_cmd;
  for (int i = 1; i < MAX_CMD_LEN; ++i){
    if (new_cmd[i] == '\0') break;
    if (new_cmd[i] == ' '){
      new_cmd[i] = '\0';
      if (arg_num < MAX_ARG_NUMS){
        argv_cmd[arg_num] = new_cmd + (i + 1);
        printf("argnum = %d\n", arg_num);
      }
      else {
        printf("Too many args!!!\n");
        break;
      }
      arg_num++;
    }
  }
  return new_cmd;
}

static void sh_handle_cmd(const char *cmd) {
  if (strcmp(cmd_exit, cmd) == 0){
    exit(0);
  }
  const char *new_cmd = cmd_rm_endl(cmd);
  // execve(new_cmd, NULL, NULL);
  execvp(new_cmd, argv_cmd);
  printf("Fail to execute %s", cmd);
}

void builtin_sh_run() {
  sh_banner();
  sh_prompt();

  printf("environ = %p\n", environ);
  assert(setenv("PATH", "/bin:/usr/bin", 0) == 0);
  // assert(putenv("PATH=/bin") == 0);
  printf("environ = %p\n", environ);

  while (1) {
    SDL_Event ev;
    if (SDL_PollEvent(&ev)) {
      if (ev.type == SDL_KEYUP || ev.type == SDL_KEYDOWN) {
        const char *res = term->keypress(handle_key(&ev));
        if (res) {
          sh_handle_cmd(res);
          sh_prompt();
        }
      }
    }
    refresh_terminal();
  }
}

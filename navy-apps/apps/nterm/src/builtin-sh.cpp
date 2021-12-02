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
char new_cmd[64];
static const char* cmd_rm_endl(const char *cmd){
  for (int i = 0; i < 64; ++i){
    new_cmd[i] = cmd[i];
    if (cmd[i] == '\n'){
      new_cmd[i] = '\0';
      return new_cmd;
    }
  }
  printf("The Command is TOO LONG!!!\n");
  return NULL;
}

static void sh_handle_cmd(const char *cmd) {
  if (strcmp(cmd_exit, cmd) == 0){
    exit(0);
  }
  const char *new_cmd = cmd_rm_endl(cmd);
  execve(new_cmd, NULL, NULL);
}

void builtin_sh_run() {
  sh_banner();
  sh_prompt();

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

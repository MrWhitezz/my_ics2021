#include <isa.h>
#include <cpu/cpu.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "sdb.h"
#include "memory/paddr.h"

static int is_batch_mode = false;

void init_regex();
void init_wp_pool();

/* We use the `readline' library to provide more flexibility to read from stdin. */
static char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}


static int cmd_q(char *args) {
  
  return -1;
}

static int cmd_help(char *args);

static int cmd_si(char* args);

static int cmd_info(char *args);

static int cmd_x(char *args);

static int cmd_p(char *args);

static struct {
  const char *name;
  const char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display informations about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },
  { "si", "Single instruction excuted", cmd_si},
  { "info", "Print the state of program", cmd_info},
  { "x", "Examine the memory", cmd_x},
  { "p", "Calculate the expression", cmd_p},
  /* TODO: Add more commands */

};

#define NR_CMD ARRLEN(cmd_table)

static int cmd_si(char* args){
  static int instr_num = 1;
  if (args != NULL)
    instr_num = atoi(args);
    //excute instructions for instr_num times
  cpu_exec(instr_num);
  return 0;
}

static int cmd_info(char *args){
    assert(args != NULL);
    if (args[0] == 'r')
      isa_reg_display();
    else if (args[0] == 'w'){
      //TODO: print watch point info

    }
    else assert(0);
    return 0;
}

static int cmd_x(char *args){
  // TODO: need to examine memory

  char *mem_num = strtok(args, " ");
  char *mem_expr = strtok(NULL, " ");

  assert(strcmp(mem_expr, ""));
  int num = atoi(mem_num);
  paddr_t mem_addr = (paddr_t)cmd_p(mem_expr);//cmd_p must be ensured correct

  for (int i = 0; i < num; ++i){
    paddr_t mem_visit = mem_addr + (paddr_t)i * 4;
    //word_t mem_val = paddr_read(mem_visit, 4);
    printf("0x%8x:\t", mem_visit);
    for (int j = 0; j < 4; ++j)
      printf("%02x ", paddr_read(mem_visit + j, 1));
    printf("\n");
  }
  
  

  return 0;
}

static int cmd_p(char *args){
  //TODO
    bool is_success = true;
    printf("%d\n", expr(args, &is_success));
    return strtol(args + 2, NULL, 16);
}

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

void sdb_set_batch_mode() {
  is_batch_mode = true;
}

void sdb_mainloop() {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  for (char *str; (str = rl_gets()) != NULL; ) {
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef CONFIG_DEVICE
    extern void sdl_clear_event_queue();
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}

void init_sdb() {
  /* Compile the regular expressions. */
  init_regex();

  /* Initialize the watchpoint pool. */
  init_wp_pool();
}

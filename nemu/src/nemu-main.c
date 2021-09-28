#include <common.h>
#include "monitor/sdb/sdb.h"
void init_monitor(int, char *[]);
void am_init_monitor();
void engine_start();
int is_exit_status_bad();

int main(int argc, char *argv[]) {
  /* Initialize the monitor. */
#ifdef CONFIG_TARGET_AM
  am_init_monitor();
#else
  init_monitor(argc, argv);
#endif
  FILE *fp = fopen("/home/xyq/ics2021/nemu/tools/gen-expr/input", "r");
  assert(fp != NULL);
  char  buff[500] = {}; unsigned res = 0, times = 0; bool success = true;
  while (fscanf(fp, "%u", &res) != EOF && fscanf(fp, "%[^\n]", buff) != EOF){
    if (buff[0] == '\0') printf("Correct %d times\n", times);
    unsigned my_res = expr(buff, &success);
    if (my_res != res)
      Log("Wrong found! res = %u\tmy_res = %u\texpr = %s", res, my_res, buff);
    else ++times;
    memset(buff, 0, sizeof(buff));
  }

  /* Start engine. */
  engine_start();

  return is_exit_status_bad();
}

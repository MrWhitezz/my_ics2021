#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>
// this should be enough
static char buf[65536 + 16] = {};
static char code_buf[65536 + 128] = {}; // a little larger than `buf`
static char *code_format =
"#include <stdio.h>\n"
"int main() { "
"  unsigned result = %s; "
"  printf(\"%%u\", result); "
"  return 0; "
"}"; 
int pos_buf = 0; int recursive_level = 0;
static int choose(int n){
    return rand() % n;
}

static void gen_num(){
    assert(pos_buf < 65536);
    unsigned num = (rand() % 9 + 1); assert(num >= 0 && num <= 9);
    buf[pos_buf++] = num + '0'; buf[pos_buf] = '\0';
 
}
static void gen(char ch){
    assert(pos_buf < 65536);
    buf[pos_buf++] = ch; buf[pos_buf] = '\0';
}

static void gen_rand_op(){
    switch(choose(4)){
      case 0: assert(pos_buf < 65536); buf[pos_buf++] = '+'; break; 
      case 1: assert(pos_buf < 65536); buf[pos_buf++] = '-'; break;
      case 2: assert(pos_buf < 65536); buf[pos_buf++] = '*'; break;
      case 3: assert(pos_buf < 65536); buf[pos_buf++] = '/'; break;
      default: printf("Cannot generate random op!\n");
    }
}

static void gen_rand_expr() {
  switch (choose(3)) {
    case 0: {int num_size = choose(4) + 1; while(num_size--) gen_num(); break;}
    case 1: gen('('); gen_rand_expr(); gen(')'); break;
    default: if (recursive_level++ <= 8) {gen_rand_expr(); gen_rand_op();} gen_rand_expr(); break;
  }
}

int main(int argc, char *argv[]) {
  int seed = time(0);
  srand(seed);
  int loop = 1;
  if (argc > 1) {
    sscanf(argv[1], "%d", &loop);
  }
  int i;
  for (i = 0; i < loop; i ++) {
    pos_buf = 0; recursive_level = 0;
    gen_rand_expr();
    //printf("buf = %-15s\npos = %d\n", buf, pos_buf);
    sprintf(code_buf, code_format, buf);

    FILE *fp = fopen("/tmp/.code.c", "w");
    assert(fp != NULL);
    fputs(code_buf, fp);
    fclose(fp);

    int ret = system("gcc -Wall -Werror /tmp/.code.c -o /tmp/.expr");
    if (ret != 0) continue;

    fp = popen("/tmp/.expr", "r");
    assert(fp != NULL);

    int result;
    fscanf(fp, "%d", &result);
    pclose(fp);

    printf("%u %s\n", result, buf);
  }
  return 0;
}

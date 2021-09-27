#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>

// this should be enough
static char buf[65536 + 10] = {};
static char code_buf[65536 + 128] = {}; // a little larger than `buf`
static char *code_format =
"#include <stdio.h>\n"
"int main() { "
"  unsigned result = %s; "
"  printf(\"%%u\", result); "
"  return 0; "
"}";
int pos_buf = 0;
static unsigned choose(unsigned n){
    //srand((unsigned)time(0));
    return rand() % n;
}
// static void gen_rand_expr() {
//   buf[0] = '\0';
// }
static unsigned gen_num(){
    assert(pos_buf < 65536);
    //srand((unsigned)time(0));
    unsigned num = (rand() % 10); assert(num >= 0 && num <= 9);
    buf[pos_buf++] = num + '0'; buf[pos_buf] = '\0';
    return num;    
}
static void gen(char ch){
    assert(pos_buf < 65536);
    buf[pos_buf++] = ch; buf[pos_buf] = '\0';
    return;
}
static char gen_rand_op(){
    switch(choose(4)){
      case 0: assert(pos_buf < 65536); buf[pos_buf++] = '+'; return '+'; 
      case 1: assert(pos_buf < 65536); buf[pos_buf++] = '-'; return '-'; 
      case 2: assert(pos_buf < 65536); buf[pos_buf++] = '*'; return '*'; 
      case 3: assert(pos_buf < 65536); buf[pos_buf++] = '/'; return '/'; 
      default: printf("Cannot generate random op!\n");
    }
}
static int gen_rand_expr() {
  switch (choose(3)) {

    case 0: {
            gen('('); 
            unsigned res = gen_rand_expr(); 
            gen(')'); 
            //printf("buf = %s\n", buf);
            assert(pos_buf < 65536); buf[pos_buf] = '\0';
            return res; break;}
    case 1: {  unsigned val1; val1 = gen_rand_expr(); 
                char ch; ch = gen_rand_op(); 
                unsigned val2; val2 = gen_rand_expr();
                //if(val2 == 0) {pos_buf = 0; return gen_rand_expr();}
                assert(pos_buf < 65536); buf[pos_buf] = '\0';
                switch(ch){
                  case '+': return val1 + val2;
                  case '-': return val1 - val2;
                  case '*': return val1 * val2;
                  case '/': return val1 / val2;
                  default: printf("Cannot return gen_rand_expr\n");}
             }
    default: {int num = gen_num(); 
              assert(pos_buf < 65536); buf[pos_buf] = '\0';
              return num; break;}
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
    pos_buf = 0;
    gen_rand_expr();
    printf("buf = %s\n", buf);
    sprintf(code_buf, code_format, buf);

    FILE *fp = fopen("/tmp/.code.c", "w");
    assert(fp != NULL);
    fputs(code_buf, fp);
    fclose(fp);

    int ret = system("gcc /tmp/.code.c -o /tmp/.expr");
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

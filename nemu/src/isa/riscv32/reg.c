#include <isa.h>
#include "local-include/reg.h"

const char *regs[] = {
  "$0", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
  "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
  "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
  "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"
};

void isa_reg_display() {
  static int reg_num = sizeof(regs) / sizeof(regs[0]);
  for (int i = 0; i < reg_num; ++i){
    printf("R[%s] = 0x%08X\t", regs[i], gpr(i));
    if (i % 4 == 3) printf("\n");
  }
}

word_t isa_reg_str2val(const char *s, bool *success) {
  for (int i = 0; i < sizeof(regs); ++i){
    if (strcmp(s, regs[i]) == 0) {
      *success = true;
      return gpr(i);
    }
  }
  *success = false;
  Log("Register %s not found\n", s);
  return 0;
}

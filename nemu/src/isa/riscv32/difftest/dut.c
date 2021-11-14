#include <isa.h>
#include <cpu/difftest.h>
#include "../local-include/reg.h"

bool isa_difftest_checkregs(CPU_state *ref_r, vaddr_t pc) {
  for (int i = 0; i < 32; ++i)
    if (gpr(i) != ref_r->gpr[i]._32){
      printf("R[%s] should be 0x%x\n", reg_name(i, 4), ref_r->gpr[i]._32);
      return false;
    }
    assert(cpu.mepc == ref_r->mepc);
  return true;
}

void isa_difftest_attach() {
}

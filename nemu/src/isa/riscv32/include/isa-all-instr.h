#include <cpu/decode.h>
#include "../local-include/rtl.h"

#define INSTR_LIST(f) f(add) f(sub) f(slt) f(sltu) \
f(mul) \
f(addi) f(sltiu) f(andi) f(lui) f(auipc) \
f(jal) f(jalr) f(lw) f(sw) f(beq) f(bne) f(blt) f(bge) f(bltu) \
f(inv) f(nemu_trap)

def_all_EXEC_ID();

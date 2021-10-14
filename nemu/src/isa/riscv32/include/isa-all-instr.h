#include <cpu/decode.h>
#include "../local-include/rtl.h"

#define INSTR_LIST(f) f(add) f(sub) f(sll) f(slt) f(sltu) f(xor) f(or) f(and) \
f(mul) f(mulu) f(div) f(rem) \
f(addi) f(sltiu) f(xori) f(andi) f(slli) f(srai) f(lui) f(auipc) \
f(jal) f(jalr) \
f(lbu) f(lw) f(sb) f(sw) \
f(beq) f(bne) f(blt) f(bge) f(bltu) \
f(inv) f(nemu_trap)

def_all_EXEC_ID();

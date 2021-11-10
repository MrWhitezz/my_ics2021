#include <cpu/decode.h>
#include "../local-include/rtl.h"

#define INSTR_LIST(f) f(add) f(sub) f(sll) f(slt) f(sltu) f(xor) f(srl) f(sra) f(or) f(and) \
f(mul) f(mulu) f(mulhu) f(div) f(divu) f(rem) f(remu) \
f(addi) f(slti) f(sltiu) f(xori) f(ori) f(andi) f(slli) f(srli) f(srai) f(lui) f(auipc) \
f(jal) f(jalr) \
f(lb) f(lh) f(lbu) f(lw) f(lhu) f(sb) f(sh) f(sw) \
f(beq) f(bne) f(blt) f(bge) f(bltu) f(bgeu) \
f(csrrw) f(csrrs) f(ecall) f(mret)\
f(inv) f(nemu_trap)

def_all_EXEC_ID();

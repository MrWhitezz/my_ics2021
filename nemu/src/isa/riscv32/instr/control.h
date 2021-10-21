void ftrace_print(word_t addr_caller, word_t addr_to, bool is_call);
def_EHelper(beq) {
    rtl_jrelop(s, RELOP_EQ, dsrc1, ddest, s->pc + id_src2->imm);
}

def_EHelper(bne) {
   rtl_jrelop(s, RELOP_NE, dsrc1, ddest, s->pc + id_src2->imm); // here use dest as src2, due to the decode of B 
}

def_EHelper(blt) {
    rtl_jrelop(s, RELOP_LT, dsrc1, ddest, s->pc + id_src2->imm);
}

def_EHelper(bge) {
    rtl_jrelop(s, RELOP_GE, dsrc1, ddest, s->pc + id_src2->imm);
}

def_EHelper(bltu) {
    rtl_jrelop(s, RELOP_LTU, dsrc1, ddest, s->pc + id_src2->imm);
}

def_EHelper(bgeu) {
    rtl_jrelop(s, RELOP_GEU, dsrc1, ddest, s->pc + id_src2->imm);
}

def_EHelper(jal) {
    rtl_addi(s, ddest, rz, s->snpc);
    if (ddest == &cpu.gpr[1]._32) {ftrace_print(s->pc, id_src1->imm + s->pc, 1);}
    rtl_j(s, id_src1->imm + s->pc);
}

def_EHelper(jalr) {
    // using s0 for temporiry register
    rtl_addi(s, ddest, rz, s->snpc); 
    // sword_t imm_12_sext = (id_src2->imm >> 11) ?
    //                     0xfffff000 | id_src2->imm : id_src2->imm;
    rtl_addi(s, s0, id_src1->preg, id_src2->imm);
    rtl_andi(s, s0, s0, 0xfffffffe); // set the least-significant bit to 0    
    rtl_jr(s, s0);
}
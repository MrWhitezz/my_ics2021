def_EHelper(bne) {
   rtl_jrelop(s, RELOP_NE, id_src1->preg, id_dest->preg, s->pc + id_src2->imm); // here use dest as src2, due to the decode of B 
}

def_EHelper(jal) {
    //TODO();
    rtl_addi(s, ddest, rz, s->snpc);
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
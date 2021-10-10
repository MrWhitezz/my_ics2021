def_EHelper(jal) {
    //TODO();
    rtl_addi(s, ddest, rz, s->snpc);
    rtl_j(s, id_src1->imm + s->pc);
}

def_EHelper(jalr) {
    rtl_addi(s, ddest, rz, s->snpc); 
    word_t imm_12_sext = (id_src2->imm >> 11) ?
                        0xfffff000 | id_src2->imm : id_src2->imm;
    rtl_addi(s, s0, id_src1->preg, imm_12_sext);
    rtl_andi(s, s0, s0, 0xfffffffe); // set the least-significant bit to 0    
    rtl_jr(s, s0);
}
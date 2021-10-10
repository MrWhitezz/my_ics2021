def_EHelper(jal) {
    TODO();
    // maybe we need some rtl_? to change s->dnpc
    rtl_j(s, id_src1->imm + s->pc);
    rtl_addi(s, ddest, rz, s->snpc);
}
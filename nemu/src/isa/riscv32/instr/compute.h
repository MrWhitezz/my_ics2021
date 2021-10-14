def_EHelper(add) {
  rtl_add(s, ddest, dsrc1, dsrc2);
}

def_EHelper(sub) {
  rtl_sub(s, ddest, dsrc1, dsrc2);
}

def_EHelper(slt) {
  rtl_setrelop(s, RELOP_LT, ddest, id_src1->preg, id_src2->preg);
}

def_EHelper(lui) {
  rtl_li(s, ddest, id_src1->imm);
}

def_EHelper(auipc) {
  rtl_li(s, ddest, id_src1->imm + s->pc);
}

def_EHelper(addi) {
  rtl_addi(s, ddest, id_src1->preg, id_src2->imm);
}

def_EHelper(andi) {
  rtl_andi(s, ddest, id_src1->preg, id_src2->imm);
}

def_EHelper(sltiu) {
  rtl_setrelopi(s, RELOP_LTU, ddest, id_src1->preg, id_src2->imm);
}

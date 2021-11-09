def_EHelper(csrrw) {
   rtl_addi(s, ddest, dsrc2, 0);
   rtl_addi(s, dsrc2, dsrc1, 0); // dsrc2 is csr
}

def_EHelper(ecall) {
   vaddr_t target = isa_raise_intr(-1, s->dnpc);// not sure which pc
   rtl_j(s, target);
}
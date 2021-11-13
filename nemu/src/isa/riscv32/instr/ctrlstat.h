def_EHelper(csrrw) {
   rtl_addi(s, ddest, dsrc2, 0);
   rtl_addi(s, dsrc2, dsrc1, 0); // dsrc2 is csr
}

def_EHelper(csrrs) {
   rtl_addi(s, ddest, dsrc2, 0);
   rtl_or(s, dsrc2, dsrc2, dsrc1); // set the bits in rs1 which is high
}

def_EHelper(ecall) {
   #ifdef CONFIG_ETRACE
      printf("Exception given: yield\n");
   #endif
   vaddr_t target = isa_raise_intr(0xb, s->pc);// not sure which pc
   rtl_j(s, target);
}

def_EHelper(mret) {

   // omit some change to mstatus
   rtl_j(s, cpu.mepc);

}
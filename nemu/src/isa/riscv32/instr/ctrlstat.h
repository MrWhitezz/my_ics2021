def_EHelper(csrrw) {
   rtl_addi(s, ddest, id_src2, 0);
   rtl_addi(s, id_src2, id_src1, 0); // id_src2 is csr
}
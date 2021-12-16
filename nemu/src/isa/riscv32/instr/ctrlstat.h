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
      Log("Exception given: \n");
   #endif
   // vaddr_t target = isa_raise_intr(cpu.gpr[17]._32, s->pc);
   vaddr_t target = isa_raise_intr(0xb, s->pc); // WRONG!!!

   rtl_j(s, target);
}

#define MASK_MIE  0x8
#define MASK_MPIE 0x80
#define UNMASK_M  (~MASK_MIE) & (~MASK_MPIE)

def_EHelper(mret) {
   cpu.mstatus = (cpu.mstatus & UNMASK_M) + MASK_MPIE + 
                ((cpu.mstatus & MASK_MPIE) != 0) ? MASK_MIE : 0;

   // omit some change to mstatus
   rtl_j(s, cpu.mepc);

}
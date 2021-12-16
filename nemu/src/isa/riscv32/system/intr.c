#include <isa.h>

#define IRQ_TIMER 0x80000007
#define MASK_MIE  0x8
#define MASK_MPIE 0x80
#define UNMASK_M  (~MASK_MIE) & (~MASK_MPIE)
// MIE == 0, close intr

word_t isa_raise_intr(word_t NO, vaddr_t epc) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * Then return the address of the interrupt/exception vector.
   */
  cpu.mepc = epc;
  cpu.mcause = NO;
  uint32_t mie = cpu.mstatus & MASK_MIE; 
  cpu.mstatus = (cpu.mstatus & UNMASK_M) + (((cpu.mstatus & MASK_MIE) != 0) ? MASK_MPIE : 0);
  assert((cpu.mstatus & MASK_MIE) == 0);
  assert(((cpu.mstatus & MASK_MPIE) == 0) == (mie == 0));

  // return the entry address of interrupt
  return cpu.mtvec;
}

word_t isa_query_intr() {
  if ((cpu.mstatus & MASK_MIE) != 0 && cpu.INTR == true){
    cpu.INTR = false;
    return IRQ_TIMER;
  }
  return INTR_EMPTY;
}

#ifndef __ISA_RISCV32_H__
#define __ISA_RISCV32_H__

#include <common.h>

typedef struct {
  struct {
    rtlreg_t _32;
  } gpr[32];

  vaddr_t pc;
  // add cte related system regs
  rtlreg_t mepc;
  rtlreg_t mstatus;
  rtlreg_t mcause;
  rtlreg_t mtvec;// God tm only 3 regs?
  
} riscv32_CPU_state;

// decode
typedef struct {
  union {
    struct {
      uint32_t opcode1_0 : 2;
      uint32_t opcode6_2 : 5;
      uint32_t rd        : 5;
      uint32_t funct3    : 3;
      uint32_t rs1       : 5;
      uint32_t rs2       : 5;
      uint32_t funct7    : 7;
    } r;
    struct {
      uint32_t opcode1_0 : 2;
      uint32_t opcode6_2 : 5;
      uint32_t rd        : 5;
      uint32_t funct3    : 3;
      uint32_t rs1       : 5;
      uint32_t simm11_0  :12;//change to uint
    } i;
    struct {
      uint32_t opcode1_0 : 2;
      uint32_t opcode6_2 : 5;
      uint32_t imm4_0    : 5;
      uint32_t funct3    : 3;
      uint32_t rs1       : 5;
      uint32_t rs2       : 5;
      uint32_t simm11_5  : 7;
    } s;
    struct {
      uint32_t opcode1_0 : 2;
      uint32_t opcode6_2 : 5;
      uint32_t imm11     : 1;
      uint32_t imm4_1    : 4;
      uint32_t funct3    : 3;
      uint32_t rs1       : 5;
      uint32_t rs2       : 5;
      uint32_t imm10_5   : 6;
      uint32_t imm12     : 1;
    } b; // attenion!!! I don't understand the meaning of using int32_t and uint32_t, why can't all imm be int32_t???
    struct {
      uint32_t opcode1_0 : 2;
      uint32_t opcode6_2 : 5;
      uint32_t rd        : 5;
      uint32_t imm31_12  :20;
    } u;
    struct {
      uint32_t opcode1_0: 2;
      uint32_t opcode6_2: 5;
      uint32_t rd       : 5;
      uint32_t imm19_12 : 8;
      uint32_t imm11    : 1;
      uint32_t imm10_1  :10;
      uint32_t imm20    : 1;
    } j;
    struct {
      uint32_t opcode1_0: 2;
      uint32_t opcode6_2: 5;
      uint32_t rd       : 5;
      uint32_t funct3   : 3;
      uint32_t rs1      : 5;
      uint32_t csr      :12;
    } CSR;
    uint32_t val;
  } instr;
} riscv32_ISADecodeInfo;

#define isa_mmu_check(vaddr, len, type) (MMU_DIRECT)

#endif

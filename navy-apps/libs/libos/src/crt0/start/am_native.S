.globl  _start
_start:
  // try to set stack pointer
  // rax fail!!! we use rcx
  // movq $0x9000000, %rsp
  movq %rcx, %rsp
  movq $0, %rbp
  // (rsp + 8) should be multiple of 16 when
  // control is transfered to the function entry point.
  // See amd64 ABI manual for more details
  andq $0xfffffffffffffff0, %rsp

  // movq %rax, %rdi
  movq %rcx, %rdi
  call call_main

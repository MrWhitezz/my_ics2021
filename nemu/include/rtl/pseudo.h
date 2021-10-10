#ifndef __RTL_PSEUDO_H__
#define __RTL_PSEUDO_H__

#ifndef __RTL_RTL_H__
#error "Should be only included by <rtl/rtl.h>"
#endif

/* RTL pseudo instructions */

static inline def_rtl(li, rtlreg_t* dest, const rtlreg_t imm) {
  rtl_addi(s, dest, rz, imm);
}

static inline def_rtl(mv, rtlreg_t* dest, const rtlreg_t *src1) {
  rtl_addi(s, dest, src1, 0);
}

static inline def_rtl(not, rtlreg_t *dest, const rtlreg_t* src1) {
  // dest <- ~src1
  rtl_xori(s, dest, src1, -1);// use xor -1 (int)
}

static inline def_rtl(neg, rtlreg_t *dest, const rtlreg_t* src1) {
  // dest <- -src1
  rtl_sub(s, dest, rz, src1); // use 0 - src1
}

static inline def_rtl(sext, rtlreg_t* dest, const rtlreg_t* src1, int width) {
  // dest <- signext(src1[(width * 8 - 1) .. 0])
  assert(width <= 4 && width >= 1);
  rtl_slli(s, dest, src1, (32 - width * 8));
  rtl_srai(s, dest, dest, (32 - width * 8));
}

static inline def_rtl(zext, rtlreg_t* dest, const rtlreg_t* src1, int width) {
  // dest <- zeroext(src1[(width * 8 - 1) .. 0])
  assert(width <= 4 && width >= 1);
  rtl_slli(s, dest, src1, (32 - width * 8));
  rtl_srli(s, dest, dest, (32 - width * 8));
}

static inline def_rtl(msb, rtlreg_t* dest, const rtlreg_t* src1, int width) {
  // dest <- src1[width * 8 - 1]
  assert(width <= 4 && width >= 1);
  int shift_num = 
  if (width == 1) rtl_andi(s, dest, src1, 0xff);
  
  else rtl_andi(s, dest, src1, )
}
#endif

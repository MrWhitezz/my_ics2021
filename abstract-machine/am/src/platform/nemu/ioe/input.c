#include <am.h>
#include <nemu.h>

#define KEYDOWN_MASK 0x8000

void __am_input_keybrd(AM_INPUT_KEYBRD_T *kbd) {
  enum {_KEY_NONE = 0};
  uint32_t key_info = inl(KBD_ADDR);
  kbd->keydown = key_info & KEYDOWN_MASK; 
  kbd->keycode = kbd->keydown ? key_info ^ KEYDOWN_MASK : key_info;
}

#include <common.h>

#if defined(MULTIPROGRAM) && !defined(TIME_SHARING)
# define MULTIPROGRAM_YIELD() yield()
#else
# define MULTIPROGRAM_YIELD()
#endif

#define NAME(key) \
  [AM_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
  [AM_KEY_NONE] = "NONE",
  AM_KEYS(NAME)
};

size_t serial_write(const void *buf, size_t offset, size_t len) {
  // offset not used
  for (size_t i = 0; i < len; ++i)
    putch(((char *)buf)[i]);
  return len;
}

size_t events_read(void *buf, size_t offset, size_t len) {
  AM_INPUT_KEYBRD_T ev = io_read(AM_INPUT_KEYBRD);
  if (ev.keycode == AM_KEY_NONE)
    return 0;
  else{
    if (ev.keydown == true){
      return sprintf(buf, "kd %s\n", keyname[ev.keycode]);
    }
    else{
      return sprintf(buf, "ku %s\n", keyname[ev.keycode]);
    }
  }
  return 0;
}

size_t dispinfo_read(void *buf, size_t offset, size_t len) {
  AM_GPU_CONFIG_T cfg = io_read(AM_GPU_CONFIG);
  int wid = cfg.width;
  int hgt = cfg.height;
  int len1 = sprintf(buf, "WIDTH : %d\n", wid);
  int len2 = sprintf(buf + len1, "HEIGHT: %d\n", hgt);
  assert(len1 + len2 <= len);
  return len1 + len2;
}

size_t fb_write(const void *buf, size_t offset, size_t len) {
  AM_GPU_CONFIG_T cfg = io_read(AM_GPU_CONFIG);
  int wid = cfg.width;
  int x = offset % wid;
  int y = offset / wid;

  io_write(AM_GPU_FBDRAW, x, y, (uint32_t *)buf, len, 1, true);
  return len;
}

void init_device() {
  Log("Initializing devices...");
  ioe_init();
}

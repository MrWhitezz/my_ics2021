#include <am.h>
#include <nemu.h>

#define AUDIO_FREQ_ADDR      (AUDIO_ADDR + 0x00)
#define AUDIO_CHANNELS_ADDR  (AUDIO_ADDR + 0x04)
#define AUDIO_SAMPLES_ADDR   (AUDIO_ADDR + 0x08)
#define AUDIO_SBUF_SIZE_ADDR (AUDIO_ADDR + 0x0c)
#define AUDIO_INIT_ADDR      (AUDIO_ADDR + 0x10)
#define AUDIO_COUNT_ADDR     (AUDIO_ADDR + 0x14)
#define AUDIO_SBUF_HEAD      (AUDIO_ADDR + 0X18)

void __am_audio_init() {
}

void __am_audio_config(AM_AUDIO_CONFIG_T *cfg) {
  cfg->present = true;// when finish, change it to true
  cfg->bufsize = inl(AUDIO_SBUF_SIZE_ADDR);
}

void __am_audio_ctrl(AM_AUDIO_CTRL_T *ctrl) {
  outl(AUDIO_INIT_ADDR, 1);
  outl(AUDIO_FREQ_ADDR, ctrl->freq);
  outl(AUDIO_CHANNELS_ADDR, ctrl->channels);
  outl(AUDIO_SAMPLES_ADDR, ctrl->samples);
}

void __am_audio_status(AM_AUDIO_STATUS_T *stat) {
  stat->count = inl(AUDIO_COUNT_ADDR);
}

static void buf_write(uint8_t *buf, int len1){
  int head = inl(AUDIO_SBUF_HEAD);
  int bufsize = inl(AUDIO_SBUF_SIZE_ADDR);
  for (int i = 0; i < len1; ++i){
    int offset = (head + i) % bufsize;
    outl(AUDIO_SBUF_ADDR + offset, buf[i]);
  }
}

static void audio_write(uint8_t *buf, int len){
  int nwrite = 0;
  int bufsize = inl(AUDIO_SBUF_SIZE_ADDR);
  while (nwrite < len){
    int count = inl(AUDIO_COUNT_ADDR);
    int remain = bufsize - count;
    int write_tmp = len - nwrite;
    if (write_tmp > remain) write_tmp = remain;
    buf_write(buf + nwrite, write_tmp);
    nwrite += write_tmp;
    outl(AUDIO_COUNT_ADDR, count + write_tmp);
  }
}

void __am_audio_play(AM_AUDIO_PLAY_T *ctl) {
  int len = ctl->buf.end - ctl->buf.start;
  audio_write(ctl->buf.start, len);
}

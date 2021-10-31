#include <common.h>
#include <device/map.h>
#include <SDL2/SDL.h>

enum {
  reg_freq,
  reg_channels,
  reg_samples,
  reg_sbuf_size,
  reg_init,
  reg_count,
  reg_sbuf_head,
  nr_reg
};

SDL_AudioSpec s = {};
static uint8_t *sbuf = NULL;
static uint32_t *audio_base = NULL;

static void audio_play(void *userdata, uint8_t *stream, int len){
  int nread = len;
  if (audio_base[reg_count] < len) nread = audio_base[reg_count];

  int head = audio_base[reg_sbuf_head];
  for (int i = 0; i < nread; ++i){
    int offset = (head + i) % CONFIG_SB_SIZE; 
    stream[i] = sbuf[offset];
  }
  audio_base[reg_sbuf_head] = (head + nread) % CONFIG_SB_SIZE;

  audio_base[reg_count] -= nread;
  if (len > nread){
    memset(stream + nread, 0, len - nread);
  }
}

static void audio_io_handler(uint32_t offset, int len, bool is_write) {
  if (is_write == true && audio_base[reg_init] != 0){
    audio_base[reg_init] = 0;// init only once
    s.format = AUDIO_S16SYS;
    s.userdata = NULL;
    s.freq = audio_base[reg_freq];
    s.channels = audio_base[reg_channels];
    s.samples = audio_base[reg_samples];
    // TODO: write callback
    s.callback = audio_play;
    SDL_InitSubSystem(SDL_INIT_AUDIO);
    SDL_OpenAudio(&s, NULL);
    SDL_PauseAudio(0);
  }
}

void init_audio() {
  uint32_t space_size = sizeof(uint32_t) * nr_reg;
  audio_base = (uint32_t *)new_space(space_size);
#ifdef CONFIG_HAS_PORT_IO
  add_pio_map ("audio", CONFIG_AUDIO_CTL_PORT, audio_base, space_size, audio_io_handler);
#else
  add_mmio_map("audio", CONFIG_AUDIO_CTL_MMIO, audio_base, space_size, audio_io_handler);
#endif

  sbuf = (uint8_t *)new_space(CONFIG_SB_SIZE);
  audio_base[reg_sbuf_size] = CONFIG_SB_SIZE;
  audio_base[reg_sbuf_head] = 0;
  add_mmio_map("audio-sbuf", CONFIG_SB_ADDR, sbuf, CONFIG_SB_SIZE, NULL);
}

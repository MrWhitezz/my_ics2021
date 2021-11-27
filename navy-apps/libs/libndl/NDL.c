#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <assert.h>

static int evtdev = -1;
static int fbdev = -1;
static int screen_w = 0, screen_h = 0;
static int canvas_w = 0, canvas_h = 0;

static struct timeval tv;
uint32_t NDL_GetTicks() {
  gettimeofday(&tv, NULL); 
  return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

int NDL_PollEvent(char *buf, int len) {
  int fd = open("/dev/events", 0, 0);
  int real_len = read(fd, buf, len);
  close(fd);
  for (int i = 0; i < real_len; ++i){
    if (buf[i] == '\n') return 1;
  }
  return 0;
}

void NDL_OpenCanvas(int *w, int *h) {
  int fd_info = open("/proc/dispinfo", 0, 0);
  char info[64];
  int real_len = read(fd_info, info, sizeof(info));
  if (real_len){
    // ugly code
    char *pos = info;
    while (*pos > '9' || *pos < '0') ++pos;
    screen_w = atoi(pos);
    while (*pos <= '9' && *pos >= '0') ++pos;
    while (*pos > '9' || *pos < '0') ++pos;
    screen_h = atoi(pos);
    printf("screen_w = %d\n", screen_w);
    printf("screen_h = %d\n", screen_h);
    assert(*w <= screen_w && *h <= screen_h);
    if (*w == 0 && *h == 0){
      *w = screen_w; *h = screen_h;
    }
  }
  // do not know what is NWM
  if (getenv("NWM_APP")) {
    int fbctl = 4; // why 4 ?
    fbdev = 5;
    screen_w = *w; screen_h = *h;
    char buf[64];
    int len = sprintf(buf, "%d %d", screen_w, screen_h);
    // let NWM resize the window and create the frame buffer
    write(fbctl, buf, len);
    while (1) {
      // 3 = evtdev
      int nread = read(3, buf, sizeof(buf) - 1);
      if (nread <= 0) continue;
      buf[nread] = '\0';
      if (strcmp(buf, "mmap ok") == 0) break;
    }
    close(fbctl);
    printf("test on NWM\n");
  }
  canvas_w = *w; canvas_h = *h;
}

void NDL_DrawRect(uint32_t *pixels, int x, int y, int w, int h) {

  int fd_fb = open("/dev/fb", 0, 0);
  x += (screen_w - canvas_w) / 2;
  y += (screen_h - canvas_h) / 2;
  for (int j = 0; j < h; ++j){
    lseek(fd_fb, ((y + j) * screen_w + x) * sizeof(uint32_t), SEEK_SET);
    write(fd_fb, pixels + j * w, w * sizeof(uint32_t));
  }
  close(fd_fb); 
}

void NDL_OpenAudio(int freq, int channels, int samples) {
}

void NDL_CloseAudio() {
}

int NDL_PlayAudio(void *buf, int len) {
  return 0;
}

int NDL_QueryAudio() {
  return 0;
}

int NDL_Init(uint32_t flags) {
  gettimeofday(&tv, NULL); // Get time for the first time
  if (getenv("NWM_APP")) {
    evtdev = 3;
  }
  return 0;
}

void NDL_Quit() {
}

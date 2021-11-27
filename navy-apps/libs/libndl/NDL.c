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

int _read(int fd, void *buf, size_t count);
int _write(int fd, void *buf, size_t count);
off_t _lseek(int fd, off_t offset, int whence);
int _open(const char *path, int flags, mode_t mode);
int _close(int fd);
static struct timeval tv;
uint32_t NDL_GetTicks() {
  gettimeofday(&tv, NULL); 
  return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

int NDL_PollEvent(char *buf, int len) {
  // int fd = _open("/dev/event", 0, 0);
  // int real_len = _read(fd, buf, len);
  // _close(fd);
  int fd = open("/dev/event", 0, 0);
  int real_len = read(fd, buf, len);
  close(fd);

  return real_len ? 1 : 0;
}

void NDL_OpenCanvas(int *w, int *h) {
  // int fd_info = _open("/proc/dispinfo", 0, 0);
  int fd_info = open("/proc/dispinfo", 0, 0);
  char info[64];
  // int real_len = _read(fd_info, info, sizeof(info));
  int real_len = read(fd_info, info, sizeof(info));
  if (real_len){
    // ugly code
    char *pos = info;
    while (*pos > '9' || *pos < '0') ++pos;
    screen_w = atoi(pos);
    while (*pos <= '9' && *pos >= '0') ++pos;
    while (*pos > '9' || *pos < '0') ++pos;
    screen_h = atoi(pos);
    // printf("screen_w = %d\n", screen_w);
    // printf("screen_h = %d\n", screen_h);
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
  }
  canvas_w = *w; canvas_h = *h;
}

void NDL_DrawRect(uint32_t *pixels, int x, int y, int w, int h) {
  // int fd_fb = _open("/dev/fb", 0, 0);
  int fd_fb = open("/dev/fb", 0, 0);
  // x += (screen_w - canvas_w) / 2;
  // y += (screen_h - canvas_h) / 2;
  for (int j = 0; j < h; ++j){
    // _lseek(fd_fb, ((y + j) * screen_w + x), SEEK_SET);
    // _write(fd_fb, pixels + j * w, w);
    lseek(fd_fb, ((y + j) * screen_w + x), SEEK_SET);
    write(fd_fb, pixels + j * w, w);
  }
  // _close(fd_fb); 
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

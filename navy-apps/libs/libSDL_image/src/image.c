#define SDL_malloc  malloc
#define SDL_free    free
#define SDL_realloc realloc

#define SDL_STBIMAGE_IMPLEMENTATION
#include "SDL_stbimage.h"

SDL_Surface* IMG_Load_RW(SDL_RWops *src, int freesrc) {
  assert(src->type == RW_TYPE_MEM);
  assert(freesrc == 0);
  assert(0);
  return NULL;
}

SDL_Surface* IMG_Load(const char *filename) {
  //try
  FILE *fp = fopen(filename, "r");
  
  assert(fp);
  fseek(fp, 0, SEEK_END);
  int sz = ftell(fp);
  fseek(fp, 0, SEEK_SET);
  char *buf = malloc(sz + 1);
  fread(buf, 1, sz, fp);
  fclose(fp);
  int fd = open(filename, 0, 0);
  
  buf[sz] = 0;
  SDL_Surface *surface = STBIMG_LoadFromMemory(buf, sz);
  printf("sz = %d\n", sz);
  return surface;
  return NULL;
}

int IMG_isPNG(SDL_RWops *src) {
  return 0;
}

SDL_Surface* IMG_LoadJPG_RW(SDL_RWops *src) {
  return IMG_Load_RW(src, 0);
}

char *IMG_GetError() {
  return "Navy does not support IMG_GetError()";
}

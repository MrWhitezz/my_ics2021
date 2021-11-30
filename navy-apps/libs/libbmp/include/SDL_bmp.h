#ifndef __SDL_BMP_H__
#define __SDL_BMP_H__

#include <SDL.h>

#ifdef __cplusplus
extern "C" {
#endif

SDL_Surface* SDL_LoadBMP(const char *filename);
void NDL_DrawRect(uint32_t *pixels, int x, int y, int w, int h);

#ifdef __cplusplus
}
#endif

#endif

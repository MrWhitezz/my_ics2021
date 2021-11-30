#include <NDL.h>
#include <SDL.h>
#include <assert.h>
#include <string.h> // I need to strcmp

#define keyname(k) #k,

static const char *keyname[] = {
  "NONE",
  _KEYS(keyname)
};

int SDL_PushEvent(SDL_Event *ev) {
  return 0;
}

char buf_event[64];
int SDL_PollEvent(SDL_Event *event) {
  // copied from SDL_WaitEvent, share buf_event
  if (NDL_PollEvent(buf_event, 64)){
    int pos = 0;
    assert(buf_event[pos] == 'k' && buf_event[pos + 2] == ' ');
    assert(buf_event[pos + 1] == 'd' || buf_event[pos + 1] == 'u');
    event->key.type = buf_event[pos + 1] == 'd' ? SDL_KEYDOWN : SDL_KEYUP;
    event->type = event->key.type;
    while(buf_event[pos] != '\n') pos++;
    assert(buf_event[pos] == '\n');
    buf_event[pos] = '\0';
    for (int i = 0; i < sizeof(keyname) / sizeof(keyname[0]); ++i){
      if (strcmp(buf_event + 3, keyname[i]) == 0){
        event->key.keysym.sym = i;
        break;
      }
    } 
    return 1;
  }
  return 0;
}

int SDL_WaitEvent(SDL_Event *event) {
  while (1){
    if (NDL_PollEvent(buf_event, 64)){
      int pos = 0;
      assert(buf_event[pos] == 'k' && buf_event[pos + 2] == ' ');
      assert(buf_event[pos + 1] == 'd' || buf_event[pos + 1] == 'u');
      event->key.type = buf_event[pos + 1] == 'd' ? SDL_KEYDOWN : SDL_KEYUP;
      event->type = event->key.type;
      while(buf_event[pos] != '\n') pos++;
      assert(buf_event[pos] == '\n');
      buf_event[pos] = '\0';
      for (int i = 0; i < sizeof(keyname) / sizeof(keyname[0]); ++i){
        if (strcmp(buf_event + 3, keyname[i]) == 0){
          event->key.keysym.sym = i;
          break;
        }
      } 
      break;
    } 
  // else{
  //   event->key.type = SDL_USEREVENT; // NOT sure
  //   event->key.keysym.sym = 0;
  // }
  }
  return 1;
}

int SDL_PeepEvents(SDL_Event *ev, int numevents, int action, uint32_t mask) {
  return 0;
}

uint8_t* SDL_GetKeyState(int *numkeys) {
  return NULL;
}

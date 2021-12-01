#include <NDL.h>
#include <SDL.h>
#include <assert.h>
#include <string.h> // I need to strcmp

#define keyname(k) #k,

static const char *keyname[] = {
  "NONE",
  _KEYS(keyname)
};
int keyname_len = sizeof(keyname) / sizeof(keyname[0]);
uint8_t key_queue[96];

int SDL_PushEvent(SDL_Event *ev) {
  assert(0);
  return 0;
}

char buf_event[64];
int debug_cnt = 0;
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
    for (int i = 0; i < keyname_len; ++i){
      if (strcmp(buf_event + 3, keyname[i]) == 0){
        event->key.keysym.sym = i;
        break;
      }
    } 
    printf("Find some event\n");
    return 1;
  }
  // if (++debug_cnt % 10000 == 0)
  //   printf("Find no event %d times\n", debug_cnt);
  return 0;
}

int SDL_WaitEvent(SDL_Event *event) {
  while (SDL_PollEvent(event) == 0){
    // if (NDL_PollEvent(buf_event, 64)){
    //   int pos = 0;
    //   assert(buf_event[pos] == 'k' && buf_event[pos + 2] == ' ');
    //   assert(buf_event[pos + 1] == 'd' || buf_event[pos + 1] == 'u');
    //   event->key.type = buf_event[pos + 1] == 'd' ? SDL_KEYDOWN : SDL_KEYUP;
    //   event->type = event->key.type;
    //   while(buf_event[pos] != '\n') pos++;
    //   assert(buf_event[pos] == '\n');
    //   buf_event[pos] = '\0';
    //   for (int i = 0; i < sizeof(keyname) / sizeof(keyname[0]); ++i){
    //     if (strcmp(buf_event + 3, keyname[i]) == 0){
    //       event->key.keysym.sym = i;
    //       break;
    //     }
    //   } 
    //   break;
    // } 
  // else{
  //   event->key.type = SDL_USEREVENT; // NOT sure
  //   event->key.keysym.sym = 0;
  // }
  }
  return 1;
}

int SDL_PeepEvents(SDL_Event *ev, int numevents, int action, uint32_t mask) {
  assert(0);
  return 0;
}

uint8_t* SDL_GetKeyState(int *numkeys) {
  if (numkeys != NULL) {assert(*numkeys < keyname_len);}
  assert(keyname_len < 96); // ensure the size of key_queue enough
  return key_queue;
  assert(0);
}

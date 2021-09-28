#include "sdb.h"

#define NR_WP 32

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;

  /* TODO: Add more members if necessary */

} WP;

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = &wp_pool[i + 1];
  }
  wp_pool[NR_WP - 1].next = NULL;

  head = NULL;
  free_ = wp_pool;
}
WP* new_wp(){
  if (free_ == NULL) {Log("No free watch point\n"); assert(0);}
  WP* wp_ = free_;
  free_ = free_->next;
  return wp_;
}
void free_wp(WP *wp){
  if (wp == NULL){
    Log("No watchpoint to free\n");
    return;
  }
  WP *wp_pos = wp;
  while (wp_pos->next != NULL) wp_pos = wp_pos->next;
  wp_pos->next = free_;
  free_ = wp;
  return;
}
/* TODO: Implement the functionality of watchpoint */


#include "sdb.h"

#define NR_WP 32
#define NR_WP_EXPR 100
typedef struct watchpoint {
  int NO;
  struct watchpoint *next;
  char _expr[NR_WP_EXPR];
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
  wp->next = free_;
  free_ = wp;
  return;
}
/* TODO: Implement the functionality of watchpoint */
int cmd_w(char *args){
  WP* wp_1 = new_wp();
  assert(strlen(args) < NR_WP_EXPR);
  strcpy(wp_1->_expr, args);
  return 0;
}

int cmd_d(char *args){
  int wp_num = atoi(args);
  free_wp(&wp_pool[wp_num]);
  return 0;
}

#include "sdb.h"

#define NR_WP 32
#define NR_WP_EXPR 100
typedef struct watchpoint {
  int NO;
  struct watchpoint *next;
  char _expr[NR_WP_EXPR];
  word_t _val;
  bool is_used;
  /* TODO: Add more members if necessary */

} WP;

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = &wp_pool[i + 1];
    wp_pool[i]._val = 0;
    wp_pool[i].is_used = false;
  }
  wp_pool[NR_WP - 1].next = NULL;

  head = NULL;
  free_ = wp_pool;
}
WP* new_wp(){
  if (free_ == NULL) {Log("No free watch point\n"); assert(0);}
  WP* wp_ = free_;
  wp_->is_used = true;
  free_ = free_->next;
  return wp_;
}
void free_wp(WP *wp){
  if (wp == NULL){
    Log("No watchpoint to free\n");
    return;
  }
  wp->is_used = false;
  wp->next = free_;
  free_ = wp;
  return;
}
/* TODO: Implement the functionality of watchpoint */
int cmd_w(char *args){
  WP* wp_1 = new_wp();
  assert(strlen(args) < NR_WP_EXPR);
  strcpy(wp_1->_expr, args);
  bool is_success = true;
  wp_1->_val = expr(wp_1->_expr, &is_success);
  wp_1->is_used = true;// not need?
  return 0;
}

int cmd_d(char *args){
  int wp_num = atoi(args);
  free_wp(&wp_pool[wp_num]);
  return 0;
}

int cmd_info(char *args){
    assert(args != NULL);
    if (args[0] == 'r')
      isa_reg_display();
    else if (args[0] == 'w'){
      //TODO: print watch point info
      printf("Num\tExpr\t\tValue\n");
      for (int i = 0; i < NR_WP; ++i){
        if (wp_pool[i].is_used == true){
          bool is_success = true;
          wp_pool[i]._val = expr(wp_pool[i]._expr, &is_success);
          printf("%d\t%s\t\t%u\n", wp_pool[i].NO, wp_pool[i]._expr, wp_pool[i]._val);
        }
      }
    }
    else Log("info args not found\n");
    return 0;
}

bool examine_wp(){
  for (int i = 0; i < NR_WP; ++i){
    if (wp_pool[i].is_used == true){
      bool is_success = true;
      word_t val = expr(wp_pool[i]._expr, &is_success);
      if (val != wp_pool[i]._val){
        wp_pool[i]._val = val;
        printf("%s has new value %u", wp_pool[i]._expr, wp_pool[i]._val);
        return true;
      }
    }
  }
  return false;
}

#include <isa.h>

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <regex.h>
#include "memory/paddr.h"

enum {
  TK_NOTYPE = 256, TK_EQ, TK_NUM, TK_HEX, TK_REG, TK_NEQ, TK_AND, TK_DEREF, TK_NEG,

  /* TODO: Add more token types */

};

static struct rule {
  const char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   * but why?
   */
  {"0x[0-9]+", TK_HEX}, //hexadecimal-number
  {"\\$[a-z0-9]+", TK_REG},  // registers
  {" +", TK_NOTYPE},    // spaces
  {"[0-9]+", TK_NUM}, // num  
  {"\\(", '('},
  {"\\)", ')'},
  {"\\+", '+'},         // plus
  {"\\-", '-'},         // minus
  {"\\*", '*'},         // multiply
  {"\\/", '/'} ,        // divide
  {"==", TK_EQ},        // equal
  {"!=", TK_NEQ},       // not equal
  {"&&", TK_AND},       // and
  {"\\*", TK_DEREF},     // dereference
  {"\\-", TK_NEG}       // negative numbers
};

#define NR_REGEX ARRLEN(rules)

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;
#define Tok_Size 500
static Token tokens[Tok_Size] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);

        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */
        if (rules[i].token_type != TK_NOTYPE){
          assert(substr_len < 32);
          tokens[nr_token].str[0] = '\0';
          strncat(tokens[nr_token].str, substr_start, substr_len);
          //strncpy(tokens[nr_token].str, substr_start, substr_len);  strncpy is something dangerous
          // so instead, I use strncat
          tokens[nr_token++].type = rules[i].token_type;
        }
        switch (rules[i].token_type) {
          case TK_HEX: break;
          case TK_REG: break;
          case '*': break;
          case '/': break;
          case '(': break;
          case ')': break;
          case TK_NUM: break;
          case TK_NOTYPE: break;
          default: break;//TODO(); assert(0); 
        }

        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}

word_t eval (int p, int q);

bool is_bi_op(int type){
    return type == '+' || type == '-' || type == '*' || type == '/' 
            || type == TK_EQ || type == TK_NEQ || type == TK_AND;
}
word_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }
  if (nr_token == 0 && tokens[0].type == TK_NOTYPE){
    Log("String with only spaces found");
    return 0;
  }
  if (nr_token >= Tok_Size){
    Log("The expression is too long");
    return 0;
  }
  //test for dereference
  for (int i = 0; i < nr_token; i++) {
      
      if (tokens[i].type == '*' && (i == 0 || is_bi_op(tokens[i - 1].type))) {
        tokens[i].type = TK_DEREF;
      }
      if (tokens[i].type == '-' && (i == 0 || is_bi_op(tokens[i - 1].type))){
        tokens[i].type = TK_NEG;
      }
  }

  /* TODO: Insert codes to evaluate the expression. */

  return eval(0, nr_token - 1);
}

bool check_parentheses(int p, int q){
    assert(p < q && q < Tok_Size && p >= 0);
    if (tokens[p].type != '(' || tokens[q].type != ')')
      return false;
      
    int parenthese_num = 1;
    for (int i = p + 1; i <= q - 1; ++i){
      if (tokens[i].type == '(') ++parenthese_num;
      else if (tokens[i].type == ')') --parenthese_num;
      
      if (parenthese_num == 0)
        return false;
    }
    if (parenthese_num != 1){
      Log("Very Bad Expression");
      return false;
    }
    return true;
}
int find_op(int p, int q){
  enum {P_DEREF = 1, P_NUM, P_MUL_OR_DIV, P_ADD_OR_SUB, P_EQ, P_AND};
  int parentheses_num = 0; int cur_precedence = 0; int cur_op = p;
  for (int i = p; i <= q; ++i){
    if (tokens[i].type == '(') ++parentheses_num;
    else if (tokens[i].type == ')') --parentheses_num;
    else if (parentheses_num == 0){
      int i_precedence = 0;
      if (tokens[i].type == TK_DEREF) i_precedence = P_DEREF;
      if (tokens[i].type == TK_NUM) i_precedence = P_NUM;
      if (tokens[i].type == '*' || tokens[i].type == '/') i_precedence = P_MUL_OR_DIV;
      if (tokens[i].type == '+' || tokens[i].type == '-') i_precedence = P_ADD_OR_SUB;
      if (tokens[i].type == TK_EQ || tokens[i].type == TK_NEG) i_precedence = P_EQ;
      if (tokens[i].type == TK_AND) i_precedence = P_AND;

      if (i_precedence >= cur_precedence){
        cur_precedence = i_precedence;
        cur_op = i;
      }

    }
  }
  return cur_op;
}

word_t eval (int p, int q){
    if (p > q){
      Log("This expression is bad");
      return 0;
    }
    else if (p == q){
      if (tokens[p].type == TK_NUM){
        return atoi(tokens[p].str);
      }
      else if (tokens[p].type == TK_HEX)
        return strtol(tokens[p].str + 2, NULL, 16);
      else if (tokens[p].type == TK_REG){
          bool is_success;
          if (strcmp(tokens[p].str, "$0") == 0)
            return isa_reg_str2val("$0", &is_success);
          else return isa_reg_str2val(tokens[p].str + 1, &is_success);
        }
      // TODO: other types value
      Log("This expression is bad and strange");
        return 0;
    }
    else if (check_parentheses(p ,q))
        return eval(p + 1, q - 1);
    else{
      int op = find_op(p ,q);
      if (tokens[op].type == TK_DEREF) return paddr_read((eval(op + 1, q)), 4);
      int val1 = eval(p, op - 1);
      int val2 = eval(op + 1, q);
      switch (tokens[op].type)
      {
      case '+': return val1 + val2;
      case '-': return val1 - val2;
      case '*': return val1 * val2;
      case '/': return val1 / val2;
      case TK_EQ: return val1 == val2;
      case TK_NEG: return val1 != val2;
      case TK_AND: return val1 && val2;
      default: Log("Bad op"); break;
      }
    }
    return 0;
}

#ifndef __SDB_H__
#define __SDB_H__

#include <common.h>
#include <isa.h>

word_t expr(char *e, bool *success);

int cmd_w(char *args);

int cmd_d(char *args);

int cmd_info(char *args);

#endif

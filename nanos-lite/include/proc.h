#ifndef __PROC_H__
#define __PROC_H__

#include <common.h>
#include <memory.h>

#define STACK_SIZE (8 * PGSIZE)

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))


typedef union {
  uint8_t stack[STACK_SIZE] PG_ALIGN;
  struct {
    Context *cp;
    AddrSpace as;
    // we do not free memory, so use `max_brk' to determine when to call _map()
    uintptr_t max_brk;
    // add new
    uintptr_t program_brk;
  };
} PCB;

extern PCB *current;
extern PCB pcb[];
extern PCB pcb_boot;

void naive_uload(PCB *pcb, const char *filename);
uintptr_t loader(PCB *pcb, const char *filename);

void context_kload(PCB *pcb1, void(* func)(void *), void *arg);
int context_uload(PCB *pcb1, const char *fname, char *const argv[], char *const envp[]);
void switch_boot_pcb();

#endif

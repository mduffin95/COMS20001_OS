#ifndef __SCHEDULER_H
#define __SCHEDULER_H

#include "types.h"
#include <string.h>

#define CHUNK 0x1000
#define PID_MAX 10
// #define ROUND_ROBIN
#define FIXED_PRIO

typedef void (*entry_point)();

void scheduler( ctx_t *ctx );
pid_t new_proc( entry_point e );
pid_t copy_proc( pcb_t *source );
uint32_t get_stack_addr( pid_t pid );
void free_pid( pid_t pid );

void push( pcb_t *pcb );
pcb_t *pop();

extern pcb_t process_table[ PID_MAX ];
extern pcb_t *current;
extern pcb_t *fg;
// extern pcb_t idle_pcb;

extern int idle_flag;

#endif

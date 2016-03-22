#ifndef __SCHEDULER_H
#define __SCHEDULER_H

#include "types.h"
#include "queue.h"
#include <string.h>

#define CHUNK 0x1000
#define PID_MAX 10


typedef void (*entry_point)();

void scheduler( ctx_t *ctx );
pid_t new_proc( pcb_t *pcb, entry_point e );
pid_t copy_proc( pcb_t *dest, pcb_t *source );
uint32_t get_stack_addr( pid_t pid );
void free_pid( pid_t pid );

extern queue_t queue;
extern pcb_t current;
extern pcb_t idle_pcb;
extern pid_t foreground;
extern int idle_flag;

#endif

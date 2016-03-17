#ifndef __SCHEDULER_H
#define __SCHEDULER_H

#include "types.h"
#include "queue.h"
#include <string.h>

#define CHUNK 0x00001000


typedef void (*entry_point)();

void scheduler( ctx_t *ctx );
void new_proc( pcb_t *pcb, entry_point e );
uint32_t copy_proc( pcb_t *dest, ctx_t *source );
void shell();

extern queue_t queue;
extern pcb_t current;
extern pcb_t idle_pcb;
extern pid_t foreground;
extern int idle_flag;

#endif

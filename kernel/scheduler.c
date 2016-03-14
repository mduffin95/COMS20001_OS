#include "scheduler.h"

uint32_t new_stack();
void idle_proc();

queue_t queue = {0};
pcb_t current;
int idle_flag = 1;
pcb_t idle_pcb;
int pid_count = 0;
int offset = 0;

void scheduler( ctx_t *ctx ) {
  memcpy( &current.ctx, ctx, sizeof( pcb_t ) ); //Write to current pcb
  push( &queue, &current ); //Push current pcb onto queue
  pop( &queue, &current );  //Get new pcb from queue to start executing
}

extern uint32_t tos;

uint32_t new_stack() {
  return (uint32_t) (&tos + CHUNK * offset++);
}

void new_proc( pcb_t *pcb, entry_point e ) {
  memset(pcb, 0, sizeof(pcb_t));
  pcb->pid      = pid_count++;
  pcb->ctx.cpsr = 0x50; //User mode
  pcb->ctx.pc   = ( uint32_t )( e );
  pcb->ctx.sp   = new_stack(); //Dynamically allocate space on the stack.
}

uint32_t copy_proc( pcb_t *dest, ctx_t *source ) {
  memset( dest, 0, sizeof( pcb_t ) );
  dest->pid      = pid_count;
  dest->ctx      = *source;
  dest->ctx.sp   = new_stack();

  //Copy stack from parent to child
  memcpy( (uint32_t *) dest->ctx.sp, (uint32_t *) source->sp, CHUNK );
  return pid_count++;
}

void idle_proc() {
  while (1) {
    asm volatile( "nop" );
  }
}

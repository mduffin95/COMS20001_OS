#include "scheduler.h"

uint32_t get_tos( pid_t pid );

queue_t queue = {0};
pcb_t current;
pid_t foreground;
int offset = 0;
uint8_t pids[PID_MAX] = {0};

void scheduler( ctx_t *ctx ) {
  memcpy( &current.ctx, ctx, sizeof( pcb_t ) ); //Write to current pcb
  push( &queue, &current ); //Push current pcb onto queue
  pop( &queue, &current );  //Get new pcb from queue to start executing
}

extern uint32_t tos;

uint32_t get_stack_addr( pid_t pid ) {
  return  ((uint32_t) &tos) + CHUNK * pid;
}

pid_t get_pid() {
  int i = 0;
  while( pids[i]==1 && i<PID_MAX ) i++;
  if( i<PID_MAX ) {
    pids[i] = 1;
    return i;
  }
  return -1;//Error, none left
}

void free_pid( pid_t pid ) {
  pids[pid] = 0;
}

pid_t new_proc( pcb_t *pcb, entry_point e ) {
  memset(pcb, 0, sizeof(pcb_t));
  pcb->pid      = get_pid();
  pcb->ctx.cpsr = 0x50; //User mode
  pcb->ctx.pc   = ( uint32_t )( e );
  pcb->ctx.sp   = get_stack_addr( pcb->pid ); //Dynamically allocate space on the stack.
}

pid_t copy_proc( pcb_t *dest, pcb_t *source ) {
  memset( dest, 0, sizeof( pcb_t ) );
  dest->pid      = get_pid();
  dest->ctx      = source->ctx;
  dest->ctx.sp   = (get_stack_addr( dest->pid ) & 0xFFFFF000) + (source->ctx.sp & 0xFFF);

  //Copy stack from parent to child
  //Not necessary to copy CHUNK every time but it's easier.
  //This will run over into empty section of memory.
  memcpy( (uint32_t *) (get_stack_addr(dest->pid) - CHUNK), (uint32_t *) (get_stack_addr(source->pid) - CHUNK), CHUNK );
}

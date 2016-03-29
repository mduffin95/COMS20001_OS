#include "scheduler.h"
#include "queue.h"
#include "heap.h"

uint32_t get_tos( pid_t pid );

pcb_t process_table[PID_MAX] = {0};
pcb_t *current;
pcb_t *fg;
int offset = 0;
/*
 * Keeps track of which entries in the process table are used and which
 * are empty. Each PID maps to one unique entry in the process table.
 */
uint8_t pids[PID_MAX] = {0};

queue_t queue = {0};
#ifdef FIXED_PRIO
uint8_t current_prty = 0;
heap_t heap = {0};
#endif

void push( pcb_t *pcb ) {
  #ifdef ROUND_ROBIN
  queue_push( &queue, pcb );
  #endif
  #ifdef FIXED_PRIO
  if( pcb->prty == current_prty ) {
    queue_push( &queue, pcb );
  }
  else {
    heap_push( &heap, pcb );
  }
  #endif
}

pcb_t *pop() {
  #ifdef FIXED_PRIO
  if( heap.count > 0 ) {
    uint8_t heap_prty = heap_peek( &heap )->prty;
    if( heap_prty >= current_prty ) {
      if( heap_prty > current_prty ) { //When prty is superseded by new task on heap
        current_prty = heap_prty;
        while( queue.count > 0 ) {
          heap_push( &heap, queue_pop( &queue ) ); //Move from queue to heap
        }
      }
      while( heap.count > 0 && heap_peek( &heap )->prty == current_prty ) {
        queue_push( &queue, heap_pop( &heap ) ); //Move from heap to queue
      }
    }
    pcb_t *result = queue_pop( &queue );
    if( !result ) {
      current_prty = 0;
      return pop();
    }
    return result;
  }
  #endif
  return queue_pop( &queue );
}

void scheduler( ctx_t *ctx ) {
  current->ctx = *ctx;
  push( current ); //Push current pcb
  current = pop();  //Get new pcb to start executing
  *ctx = current->ctx;
}

extern uint32_t tos;

/*
 * Returns the top of the stack for the given PID when that stack is empty.
 * This becomes the bottom of the stack as soon as anything is placed on it.
 */
uint32_t get_stack_addr( pid_t pid ) {
  return  ((uint32_t) &tos) + CHUNK * pid;
}

/*
 * Returns a new process id. The maximum number of available PIDs
 * can be set using the PID_MAX macro.
 */
pid_t get_pid() {
  int i = 0;
  while( pids[i]==1 && i<PID_MAX ) i++;
  if( i<PID_MAX ) {
    pids[i] = 1;
    return i;
  }
  return -1;//Error, none left
}

/*
 * Frees one of the previously taken PIDs.
 * This writes a zero to the pids array.
 */
void free_pid( pid_t pid ) {
  pids[pid] = 0;
}


/*
 * Creates a new user mode process. Starts at entry point e.
 * Returns the PID of the new process.
 */
pid_t new_proc( entry_point e ) {
  pid_t pid = get_pid();
  memset(&process_table[ pid ], 0, sizeof(pcb_t));
  process_table[ pid ].pid      = pid;
  process_table[ pid ].ctx.cpsr = 0x50; //User mode
  process_table[ pid ].ctx.pc   = ( uint32_t )( e );
  process_table[ pid ].ctx.sp   = get_stack_addr( pid ); //Dynamically allocate space on the stack.
  return pid;
}

/*
 * Copy the source process to a new entry in the process table. Creates
 * a new PID (which is returned) and new stack space. Copies across the source stack.
 */
pid_t copy_proc( pcb_t *source ) {
  pid_t pid = get_pid();
  memset( &process_table[ pid ], 0, sizeof( pcb_t ) );
  process_table[ pid ].pid      = pid;
  process_table[ pid ].prty     = source->prty;
  process_table[ pid ].ctx      = source->ctx;
  process_table[ pid ].ctx.sp   = (get_stack_addr( pid ) & 0xFFFFF000) \
                                  + (source->ctx.sp & 0xFFF);

  //Copy stack from parent to child
  memcpy( (uint32_t *) (get_stack_addr( pid ) - CHUNK), \
  (uint32_t *) (get_stack_addr( source->pid ) - CHUNK), CHUNK );
  return pid;
}

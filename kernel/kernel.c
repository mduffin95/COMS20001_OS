#include "kernel.h"
#include "queue.h"
/* Since we *know* there will be 2 processes, stemming from the 2 user
 * programs, we can
 *
 * - allocate a fixed-size process table (of PCBs), and use a pointer
 *   to keep track of which entry is currently executing, and
 * - employ a fixed-case of round-robin scheduling: no more processes
 *   can be created, and neither is able to complete.
 */

 #include   "GIC.h"
 #include "PL011.h"
 #include "SP804.h"

 #include "interrupt.h"
 #include "types.h"
 // Include functionality from newlib, the embedded standard C library.

 #include <string.h>

 // Include definitions relating to the 2 user programs.
 #include "P0.h"
 #include "P1.h"


queue_t queue = {0};
pcb_t current;
int pid_count = 0;
int offset = 0;

void scheduler( ctx_t *ctx ) {
  memcpy( &current.ctx, ctx, sizeof( pcb_t ) ); //Write to current pcb
  push( &queue, &current ); //Push current pcb onto queue
  pop( &queue, &current );  //Get new pcb from queue to start executing
}

uint32_t new_stack() {
  return (uint32_t) (&tos + CHUNK * offset++);
}

void kernel_handler_rst( ctx_t* ctx ) {
  /* Initialise PCBs representing processes stemming from execution of
   * the two user programs.  Note in each case that
   *
   * - the CPSR value of 0x50 means the processor is switched into USR
   *   mode, with IRQ interrupts enabled, and
   * - the PC and SP values matche the entry point and top of stack.
   */

  TIMER0->Timer1Load     = 0x00100000; // select period = 2^20 ticks ~= 1 sec
  TIMER0->Timer1Ctrl     = 0x00000002; // select 32-bit   timer
  TIMER0->Timer1Ctrl    |= 0x00000040; // select periodic timer
  TIMER0->Timer1Ctrl    |= 0x00000020; // enable          timer interrupt
  TIMER0->Timer1Ctrl    |= 0x00000080; // enable          timer

  UART0->IMSC           |= 0x00000010; // enable UART    (Rx) interrupt
  UART0->CR              = 0x00000301; // enable UART (Tx+Rx)

  GICC0->PMR             = 0x000000F0; // unmask all            interrupts
  GICD0->ISENABLER[ 1 ] |= 0x00001010; // enable timer and UART (Rx) interrupts
  GICC0->CTLR            = 0x00000001; // enable GIC interface
  GICD0->CTLR            = 0x00000001; // enable GIC distributor

  pcb_t new;
  memset( &new, 0, sizeof( pcb_t ) );
  new.pid      = pid_count++;
  new.ctx.cpsr = 0x50;
  new.ctx.pc   = ( uint32_t )( entry_P0 );
  new.ctx.sp   = new_stack(); //Dynamically allocate space on the stack.
  current = new;

  //The new process will be restored when the function returns.
  *ctx = current.ctx;

  irq_enable();
  return;
}

void kernel_handler_svc( ctx_t* ctx, uint32_t id ) {
  /* Based on the identified encoded as an immediate operand in the
   * instruction,
   *
   * - read  the arguments from preserved usr mode registers,
   * - perform whatever is appropriate for this system call,
   * - write any return value back to preserved usr mode registers.
   */

  switch( id ) {
    case 0x00 : { // yield()
      scheduler( ctx );
      break;
    }
    case 0x01 : { // write( fd, x, n )
      int   fd = ( int   )( ctx->gpr[ 0 ] );
      char*  x = ( char* )( ctx->gpr[ 1 ] );
      int    n = ( int   )( ctx->gpr[ 2 ] );

      for( int i = 0; i < n; i++ ) {
        PL011_putc( UART0, *x++ );
      }

      ctx->gpr[ 0 ] = n;
      break;
    }
    case 0x02 : { // fork
      pcb_t new;
      memset( &new, 0, sizeof( pcb_t ) );
      new.pid      = pid_count;
      new.ctx      = *ctx;
      new.ctx.sp   = new_stack();
      new.ctx.gpr[ 0 ] = 0;
      push( &queue, &new );

      ctx->gpr[ 0 ] = pid_count;
      pid_count++;
      break;
    }
    case 0x03 : { // exit
      pop( &queue, &current );
      *ctx = current.ctx;
      break;
    }
    default   : { // unknown
      break;
    }
  }

  return;
}

void kernel_handler_irq(ctx_t *ctx) { //ctx_t* ctx
  // Step 2: read  the interrupt identifier so we know the source.

  uint32_t id = GICC0->IAR;

  // Step 4: handle the interrupt, then clear (or reset) the source.
  switch( id ) {
    case GIC_SOURCE_TIMER0: {
      scheduler( ctx );
      *ctx = current.ctx;
      TIMER0->Timer1IntClr = 0x01;
      break;
    }
    case GIC_SOURCE_UART0: {
      uint8_t x = PL011_getc( UART0 );

      if ( x == 'f' ) {

      }
      else if ( x == 'e' ) {

      }

      PL011_putc( UART0, 'K' );
      PL011_putc( UART0, '<' );
      PL011_putc( UART0,  x  );
      PL011_putc( UART0, '>' );

      UART0->ICR = 0x10;
      break;
    }
  }

  // Step 5: write the interrupt identifier to signal we're done.

  GICC0->EOIR = id;
}

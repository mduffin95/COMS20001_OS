#include "kernel.h"
#include "queue.h"

#include   "GIC.h"
#include "PL011.h"
#include "SP804.h"

#include "interrupt.h"
#include "types.h"
#include "scheduler.h"
// Include functionality from newlib, the embedded standard C library.

#include <string.h>

// Include definitions relating to the 2 user programs.
#include "P0.h"
#include "P1.h"

void kernel_handler_rst( ctx_t* ctx ) {
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

  new_proc(&idle_pcb, &idle_proc);

  *ctx = idle_pcb.ctx;

  irq_enable();
  return;
}

void kernel_handler_svc( ctx_t* ctx, uint32_t id ) {
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
      uint32_t pid_new = copy_proc( &new, ctx );

      new.ctx.gpr[ 0 ] = 0; //Set return value to zero (for child)
      push( &queue, &new );

      ctx->gpr[ 0 ] = pid_new; //Set return value to pid of child (for parent)
      break;
    }
    case 0x03 : { // exit
      if (pop( &queue, &current )) {
        *ctx = idle_pcb.ctx;
        current = idle_pcb;
        idle_flag = 1;
      }
      else {
        *ctx = current.ctx;
      }
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
      if (!idle_flag) {
        scheduler( ctx );
        *ctx = current.ctx;
      }
      TIMER0->Timer1IntClr = 0x01;
      break;
    }
    case GIC_SOURCE_UART0: {
      uint8_t x = PL011_getc( UART0 );

      if ( x == 'n' ) {
        if (!idle_flag) {
          push( &queue, &current );
        }
        else idle_flag = 0;
        new_proc( &current, entry_P0 );
        *ctx = current.ctx;
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

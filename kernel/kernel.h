#ifndef __KERNEL_H
#define __KERNEL_H

#include <types.h>
/* The kernel source code is made simpler by three type definitions:
 *
 * - a type that captures each component of an execution context (i.e.,
 *   processor state) in a compatible order wrt. the low-level handler
     preservation and restoration prologue and epilogue,
 * - a type that captures a process identifier, which is basically an
 *   integer, and
 * - a type that captures a process PCB.
 */

void scheduler( ctx_t *ctx );
void kernel_handler_rst( ctx_t* ctx );
void kernel_handler_svc( ctx_t* ctx, uint32_t id );
void kernel_handler_irq(ctx_t* ctx);

extern uint32_t tos;
#endif

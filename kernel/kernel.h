#ifndef __KERNEL_H
#define __KERNEL_H

#include "types.h"

void kernel_handler_rst( ctx_t* ctx );
void kernel_handler_svc( ctx_t* ctx, uint32_t id );
void kernel_handler_irq(ctx_t* ctx);

#endif

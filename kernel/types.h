#ifndef __TYPES_H
#define __TYPES_H

#include <stdint.h>
//#include <stddef.h>

typedef struct {
  uint32_t cpsr, pc, gpr[ 13 ], sp, lr;
} ctx_t;

typedef int pid_t;

typedef struct {
  pid_t pid;
  uint8_t prty;
  ctx_t ctx;
} pcb_t;

#endif

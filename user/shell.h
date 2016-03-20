#ifndef __SHELL_H
#define __SHELL_H

#include <stddef.h>
#include <stdint.h>

#define SZ_INST_BUFFER 256

void shell();
int process_input(uint8_t data);
int inst_add(uint8_t data);
void inst_del();
void inst_clear();

// define symbols for shell entry point
extern void (*entry_shell)();

#endif

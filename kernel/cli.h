#ifndef __CLI_H
#define __CLI_H

#include <stdint.h>

#define MAX_ARGS 10

//Number of arguments followed by the arguments
typedef void (*command)(uint8_t, uint16_t*);
typedef struct commandStruct {
  char const *name;
  command execute;
  uint8_t args;
  char const *help;
} commandStruct;

void exec_command( char *buffer );
void test_func(uint8_t argc, uint16_t *argv);
#endif

#ifndef __CLI_H
#define __CLI_H

#include <stdint.h>

#define MAX_ARGS 10

//Number of arguments followed by the arguments
typedef void (*command)(uint8_t, char**);
typedef struct commandStruct {
  char const *name;
  command execute;
  uint8_t args;
  char const *help;
} commandStruct;


void process_command( char *buffer );
void test_func(uint8_t argc, char **argv);
void run( uint8_t argc, char **argv );
void run_p( uint8_t argc, char **argv );
void prty_run( int prty, uint8_t argc, char **argv );
#endif

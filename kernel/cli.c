#include "cli.h"
#include "PL011.h"
#include <string.h>
#include <stdlib.h>

const commandStruct commands[] = {
  {"Test", test_func, 0, "A test function."},
  {"",0,0,""}
};

void test_func(uint8_t argc, uint16_t *argv) {
  PL011_putc( UART0, 'E' );
}

void exec_command( char *buffer ) {
  uint16_t args[MAX_ARGS] = {0};
  uint8_t num_args = 0;
  char *inst, *token;

  inst = strtok(buffer, " ");
  token = strtok(NULL, " ");

  int i;
  for(i=0; token != NULL && i < MAX_ARGS; i++) {
    args[i] = atoi(token);
    num_args++;
    token = strtok(NULL, " ");
  }

  uint16_t cmdCount = 0;
  while(strcmp(commands[cmdCount].name,"")) {
    if(!strcmp(commands[cmdCount].name,inst)) {
      commands[cmdCount].execute(num_args, args);
      break;
    }
  }
}

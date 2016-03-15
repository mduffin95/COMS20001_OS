#ifndef __INST_H
#define __INST_H

#define SZ_INST_BUFFER 256

int inst_process(uint8_t data);
int inst_add(uint8_t data);
void inst_del();
void inst_clear();

#endif

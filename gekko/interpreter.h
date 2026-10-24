#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "../global/type.h"

//#define INTERPRETER_PRINT

/* SPR reference table */
extern u32 ** sprtable;

/* Opcode execute tables */
extern void (**opcode_executetablea)(u32);
extern void (**opcode_executetableb)(u32);
extern void (**opcode_executetablec)(u32);
extern void (**opcode_executetabled)(u32);


void interpreter_setup(void);
void interpreter_free(void);
void interpreter_setup_opcodeexecutetables(void);

u32 rotmask(u32 mb, u32 me);

#endif

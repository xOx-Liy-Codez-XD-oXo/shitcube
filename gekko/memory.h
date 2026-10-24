#ifndef MEMORY_H
#define MEMORY_H
#include "../global/type.h"


extern void * memoryhole;
extern void * mem_arenaA;
extern void * mem_arenaB;

void memory_setupArenas(int system);
void memory_freeArenas(void);

u32 memory_read32(u32 addr);
u16 memory_read16(u32 addr);
u8 memory_read8(u32 addr);
void memory_write32(u32 addr, u32 data);
void memory_write16(u32 addr, u16 data);
void memory_write8(u32 addr, u8 data);

u32 memory_usread32(u32 addr);
void memory_uswrite32(u32 addr, u32 data);
void memory_uswrite16(u32 addr, u16 data);

#endif

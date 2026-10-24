#include "memory.h"
#include "../global/type.h"
#include "../global/endian.h"
#include "hardware_reg.h"
#include <stdlib.h>
#include <stdio.h>

void * memoryhole = NULL;
void * mem_arenaA = NULL;
void * mem_arenaB = NULL;

void memory_setupArenas(int system) {
	switch(system) {
		case SYSTEM_GAMECUBE:
			memoryhole = malloc((40*1024*1024)+127);
			mem_arenaA = (void *)(((uip)memoryhole + 63) & ~63);
			mem_arenaB = (void *)((uip)mem_arenaA + (24*1024*1024));
			break;
	}
}

void memory_freeArenas(void) {
	if(!memoryhole) return;
	free(memoryhole);
	memoryhole = NULL;
	mem_arenaA = NULL;
	mem_arenaB = NULL;
}



u32 memory_read32(u32 addr) {
	/* printf("MEMORY read32 %08x  ", addr); */
	if(((addr >> 24) & 0xFF) == 0xCC) {
		hardwarereg_read32(addr);
	}
	addr &= ~0xF0000000;
	if(addr > 25165824) {
		printf("\nMEMORY unimpl read32  %08x", addr);
		return 0x00000000;
	}
	return swaponle_u32(*(u32 *)((u8 *)mem_arenaA + addr));
}

u16 memory_read16(u32 addr) {
	/* printf("MEMORY read16 %08x  ", addr); */
	if(((addr >> 24) & 0xFF) == 0xCC) {
		return hardwarereg_read16(addr);
	}
	addr &= ~0xF0000000;
	if(addr > 25165824) {
		printf("\nMEMORY unimpl read16  %08x", addr);
		return 0x00000000;
	}
	return swaponle_u16(*(u16 *)((u8 *)mem_arenaA + addr));
}

u8 memory_read8(u32 addr) {
	/* printf("MEMORY read16 %08x  ", addr); */
	if(((addr >> 24) & 0xFF) == 0xCC) {
		return hardwarereg_read16(addr);
	}
	addr &= ~0xF0000000;
	if(addr > 25165824) {
		printf("\nMEMORY unimpl read8   %08x", addr);
		return 0x00000000;
	}
	return *(u8 *)((u8 *)mem_arenaA + addr);
}

void memory_write32(u32 addr, u32 data) {
	/* printf("MEMORY write32 %08x  ", addr); */
	if(((addr >> 24) & 0xFF) == 0xCC) {
		hardwarereg_write32(addr, data);
		return;
	}
	addr &= ~0xF0000000;
	if(addr > 25165824) {
		printf("\nMEMORY unimpl write32 %08x", addr);
		return;
	}
	((u32 *)((u8 *)mem_arenaA + addr))[0] = swaponle_u32(data);
}

void memory_write16(u32 addr, u16 data) {
	/* printf("MEMORY write16 %08x  ", addr); */
	if(((addr >> 24) & 0xFF) == 0xCC) {
		hardwarereg_write16(addr, data);
		return;
	}
	addr &= ~0xF0000000;
	if(addr > 25165824) {
		printf("\nMEMORY unimpl write16 %08x", addr);
		return;
	}
	((u16 *)((u8 *)mem_arenaA + addr))[0] = swaponle_u32(data);
}

void memory_write8(u32 addr, u8 data) {
	/* printf("MEMORY write8 %08x  ", addr); */
	if(((addr >> 24) & 0xFF) == 0xCC) printf("\nREGBLOCK WRITE8");
	addr &= ~0xF0000000;
	if(addr > 25165824) {
		printf("\nMEMORY unimpl write8  %08x", addr);
		return;
	}
	((u8 *)((u8 *)mem_arenaA + addr))[0] = data;
}


/* us = "unswapped" */
u32 memory_usread32(u32 addr) {
	addr &= ~0xF0000000;
	if(addr > 25165824) {
		printf("\nMEMORY unimpl read");
		return 0x00000000;
	}
	return *(u32 *)((u8 *)mem_arenaA + addr);
}

void memory_uswrite32(u32 addr, u32 data) {
	addr &= ~0xF0000000;
	if(addr > 25165824) {
		printf("\nMEMORY unimpl write");
		return;
	}
	((u32 *)((u8 *)mem_arenaA + addr))[0] = data;
}

void memory_uswrite16(u32 addr, u16 data) {
	addr &= ~0xF0000000;
	if(addr > 25165824) {
		printf("\nMEMORY unimpl write");
		return;
	}
	((u16 *)((u8 *)mem_arenaA + addr))[0] = data;
}

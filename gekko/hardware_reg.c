#include "memory.h"
#include "../global/endian.h"
#include "hardware_reg.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

u32 * hardwareregs = NULL;

u16 * cpregs;
u16 * peregs;
u32 * viregs;
u32 * piregs;
u32 * miregs;
u16 * dspiregs;
u32 * diregs;
u32 * siregs;
u32 * exiregs;
u32 * airegs;

void hardwarereg_setup(void) {
	if(!hardwareregs) hardwareregs = malloc(2048);
	cpregs  =  (u16 *)&(((u8 *)hardwareregs)[0]);
	peregs  =  (u16 *)&(((u8 *)hardwareregs)[0x80]);
	viregs  =  (u32 *)&(((u8 *)hardwareregs)[0x180]);
	piregs  =  (u32 *)&(((u8 *)hardwareregs)[0x280]);
	miregs  =  (u32 *)&(((u8 *)hardwareregs)[0x380]);
	dspiregs = (u16 *)&(((u8 *)hardwareregs)[0x400]);
	diregs  =  (u32 *)&(((u8 *)hardwareregs)[0x600]);
	siregs  =  (u32 *)&(((u8 *)hardwareregs)[0x640]);
	exiregs =  (u32 *)&(((u8 *)hardwareregs)[0x740]);
	exiregs =  (u32 *)&(((u8 *)hardwareregs)[0x780]);
	memset(hardwareregs, 0, 2048);
}

void hardwarereg_free(void) {
	if(hardwareregs) free(hardwareregs);
	hardwareregs = NULL;
}

void hardwarereg_write32(u32 addr, u32 data) {
	u32 maskaddr = addr & 0x0FFF;
	switch((addr >> 8) & 0xFF) {
		case 0x20:
			((u32 *)((u8 *)viregs + maskaddr))[0] = data;
			break;
	}
	
	((u32 *)((u8 *)hardwareregs + maskaddr))[0] = swaponle_u32(data);
}

void hardwarereg_write16(u32 addr, u16 data) {
	u32 maskaddr = addr & 0x0FFF;
	switch((addr >> 8) & 0xFF) {
		case 0x20:
			((u16 *)((u8 *)viregs + maskaddr))[0] = data;
			break;
	}

	((u16 *)((u8 *)hardwareregs + maskaddr))[0] = swaponle_u16(data);
}

int dspread = 0;

u16 hardwarereg_read16(u32 addr) {
	u32 maskaddr = addr & 0x0FFF;
	switch((addr >> 8) & 0xFF) {
		case 0x50:
			/* todo forward to a dsp interpreter ig */
			switch(addr & 0xFF) {
				case 0x04:
					dspread ^= 1;
					if(dspread) return 0x8000;
					else return 0x0000;
				case 0x06:
					return 0x8000;
				case 0x0a:
					return 0x0020;
			}
	}
	return swaponle_u16(*(u16 *)((u8 *)hardwareregs + maskaddr));
}

u32 hardwarereg_read32(u32 addr) {
	u32 maskaddr = addr & 0x0FFF;
	switch((addr >> 8) & 0xFF) {
		case 0x50:
			return 0x80000000;
	}
	
	return swaponle_u32(*(u32 *)((u8 *)hardwareregs + maskaddr));
}

void hardwarereg_printall(void) {
	int i;
	for(i = 0; i < 64; i++) { 
		printf("%d, %08x, \n", i, viregs[i]);
	}
}


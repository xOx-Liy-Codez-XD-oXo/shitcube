#ifndef HARDWARE_REG_H
#define HARDWARE_REG_H

#include "../global/type.h"

/* 
extern u32 * hardwareregs;
 total regs..
80h b  - cp  cc000000
100h b - pe  cc001000
100h b - vi  cc002000
100h b - pi  cc003000
80h b  - mi  cc004000
200h b - dsp cc005000
40h b  - di  cc006000
100h b - si  cc006400
40h b  - exi cc006800
20h b  - ai  cc006c00
we'll have gpu regs elsewhere
= 1952 bytes
lets just say 2048

*/

extern u16 * cpregs;
extern u16 * peregs;
extern u32 * viregs;
extern u32 * piregs;
extern u32 * miregs;
extern u16 * dspiregs;
extern u32 * diregs;
extern u32 * siregs;
extern u32 * exiregs;
extern u32 * airegs;

void hardwarereg_setup(void);
void hardwarereg_free(void);

void hardwarereg_write32(u32 addr, u32 data);
void hardwarereg_write16(u32 addr, u16 data);
u32 hardwarereg_read32(u32 addr);
u16 hardwarereg_read16(u32 addr);

void hardwarereg_printall(void);

#endif

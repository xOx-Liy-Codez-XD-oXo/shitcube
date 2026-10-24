#include <stdio.h>
#include <stdlib.h>
#include "gekko/dol.h"
#include "gekko/elf.h"
#include "gekko/hardware_reg.h"
#include "gekko/interpreter.h"
#include "gekko/interpreter_reg.h"
#include "gekko/memory.h"
#include "gekko/opcode.h"
#include "global/type.h"
#include "global/endian.h"
#include "global/liybmp.h"

FILE * indol;

int main(void) {
	float r1, g1, b1, r2, g2, b2, y1, y2, cb, cr;
	int ir1, ig1, ib1, ir2, ig2, ib2;
	int h, w;
	u64 i;
	u32 opcode, swopcode;
	void * xfbbase, * xfbConv;
	FILE * ramdump;/* , * xfbdump; */

	indol = fopen("consoletest.elf", "rb");
	if(!indol) return -1;
	
	printf("huzzah\n");
	memory_setupArenas(SYSTEM_GAMECUBE);
	interpreter_setup();
	hardwarereg_setup();

	elf_loadFromFile(indol);
	fclose(indol);

	for(i = 0; i < 486000000; i++) {
		nia = pc + 4;

		

		if (i % 4) {
			tb--;
			tbl = tb & 0xFFFFFFFF;
			tbu = (tb >> 32) & 0xFFFFFFFF;
		}
		opcode = memory_usread32(pc);
		swopcode = swaponle_u32(opcode);
		#ifdef INTERPRETER_PRINT
		printf("%08x, %08x \n", pc, swopcode);
		#endif
		opcode_execute(opcode);
		#ifdef INTERPRETER_PRINT
		putchar('\n');
		#endif
		pc = nia;
	}

	ramdump = fopen("out.raw", "wb");
	fwrite(mem_arenaA, 24*1024, 1024, ramdump);
	fclose(ramdump);
	/*xfbdump = fopen("xfb.raw", "wb");
	fwrite((u8 *)mem_arenaA + (viregs[7]), 640*480, 2, xfbdump);
	fclose(xfbdump);*/

	/* hardwarereg_printall(); */

	xfbConv = malloc(640*480*3);
	if(viregs[7] & 0x10000000) /* page offset bit */
		xfbbase = (u8 *)mem_arenaA + ((viregs[7] & 0x0FFFFFFF) << 5);
	else
		xfbbase = (u8 *)mem_arenaA + (viregs[7]);
	for(h = 0; h < 480; h++) {
		for(w = 0; w < 320; w++) {
			int faralong = (h*320)+w;
			int ycbcrfaralong = faralong * 4;
			int rgbfaralong = faralong * 6;
			
			y1 = ((u8 *)((u8 *)xfbbase + ycbcrfaralong))[0];
			cb = ((u8 *)((u8 *)xfbbase + ycbcrfaralong))[1];
			y2 = ((u8 *)((u8 *)xfbbase + ycbcrfaralong))[2];
			cr = ((u8 *)((u8 *)xfbbase + ycbcrfaralong))[3];
			
			r1 = y1 + 1.371 * (cr - 128.0f);
			g1 = y1 - 0.698 * (cr - 128.0f) - 0.336 * (cb - 128.0f);
			b1 = y1 + 1.732 * (cb - 128.0f);
			r2 = y2 + 1.371 * (cr - 128.0f);
			g2 = y2 - 0.698 * (cr - 128.0f) - 0.336 * (cb - 128.0f);
			b2 = y2 + 1.732 * (cb - 128.0f);

			r1 = r1 > 255 ? 255 : r1;
			g1 = g1 > 255 ? 255 : g1;
			b1 = b1 > 255 ? 255 : b1;
			r2 = r2 > 255 ? 255 : r2;
			g2 = g2 > 255 ? 255 : g2;
			b2 = b2 > 255 ? 255 : b2;

			ir1 = r1 < 0 ? 0 : r1;
			ig1 = g1 < 0 ? 0 : g1;
			ib1 = b1 < 0 ? 0 : b1;
			ir2 = r2 < 0 ? 0 : r2;
			ig2 = g2 < 0 ? 0 : g2;
			ib2 = b2 < 0 ? 0 : b2;

			((u8 *)xfbConv + rgbfaralong)[0] = ir1;
			((u8 *)xfbConv + rgbfaralong)[1] = ig1;
			((u8 *)xfbConv + rgbfaralong)[2] = ib1;
			((u8 *)xfbConv + rgbfaralong)[3] = ir2;
			((u8 *)xfbConv + rgbfaralong)[4] = ig2;
			((u8 *)xfbConv + rgbfaralong)[5] = ib2;
		}
	}
	liybmp_save("xfb.bmp", 640, 480, xfbConv);
	free(xfbConv);

	memory_freeArenas();
	interpreter_free();
	hardwarereg_free();
	return 0;
}

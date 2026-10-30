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
#include "graphics/xfb.h"

FILE * indol;

int main(void) {
	float y1, y2, cb, cr;
	int ir1, ig1, ib1, ir2, ig2, ib2;
	int h, w;
	u64 i;
	u32 opcode, swopcode;
	void * xfbbase, * xfbConv;
	FILE * ramdump;/* , * xfbdump; */

	indol = fopen("panda.dol", "rb");
	if(!indol) return -1;
	
	printf("huzzah\n");
	
	xfb_generatelut();
	memory_setupArenas(SYSTEM_GAMECUBE);
	interpreter_setup();
	hardwarereg_setup();

	dol_loadFromFile(indol);
	fclose(indol);

	for(i = 0; i < 486000; i++) {
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
			
			ir1 = xfb_lutlookupr(y1, cr);
			ig1 = xfb_lutlookupg(y1, cr, cb);
			ib1 = xfb_lutlookupb(y1, cb);
			ir2 = xfb_lutlookupr(y2, cr);
			ig2 = xfb_lutlookupg(y2, cr, cb);
			ib2 = xfb_lutlookupb(y2, cb);

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

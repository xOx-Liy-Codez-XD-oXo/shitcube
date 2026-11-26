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

#ifdef SHITCUBE_SDL
#include <SDL2/SDL.h>
#endif

FILE * inrom;

int main(int argc, char ** argv) {
	float y1, y2, cb, cr;
	int ir1, ig1, ib1, ir2, ig2, ib2;
	int h, w;
	u64 numblanks = 0;
	u32 opcode, swopcode;
	void * xfbbase, * xfbConv;
	FILE * ramdump;/* , * xfbdump; */

	if(argc != 2) {
		printf("Usage:\n%s input.dol\nor\n%s input.elf\n", argv[0], argv[0]);
		return -1;
	}

	int iself = 2;
	{
		int numchars = 0;
		char lastchar = 0, newchar = 0;
		while(1) {
			newchar = argv[1][numchars];
			if(newchar == '\0') break;
			numchars++;
			lastchar = newchar;
		}
		if(lastchar == 'F' || lastchar == 'f') iself = 1;
		if(lastchar == 'L' || lastchar == 'l') iself = 0;
	}
	if(iself == 2) {
		printf("I don't know what this file is!\n");
		return -2;
	}
	
	inrom = fopen(argv[1], "rb");
	if(!inrom) {
		printf("I can't open that!\n");
		return -1;
	}
	
	printf("huzzah\n");
	
	xfb_generatelut();
	memory_setupArenas(SYSTEM_GAMECUBE);
	interpreter_setup();
	hardwarereg_setup();

	if(iself) elf_loadFromFile(inrom);
	else dol_loadFromFile(inrom);
	fclose(inrom);

	/* Start setup window now that We are READY to STARTTT */

	#ifdef SHITCUBE_SDL
	SDL_Surface * windowSurface = NULL;
	SDL_Window * window = NULL;
	if(SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("I couldn't init SDL... %s\n", SDL_GetError());
		return -3;
	}
	window = SDL_CreateWindow("SHIT CUBE", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_SHOWN);
	if(!window) {
		printf("I couldn't create SDL window... %s\n", SDL_GetError());
		return -3;
	}
	windowSurface = SDL_GetWindowSurface(window);
	SDL_Event event;
	#endif

	//Assume a large xfb area .. boo
	xfbConv = malloc(720*576*3);

	printf("Time to start cycling .. \n");
	while(1) {
		if(cycles > 486000000) break;

		if(cycles > ((486000000/60) * numblanks)) { //Time for blank
			printf("Blank\n");
			numblanks++;
			if((msr >> 12) & 1) { //exception enable bit 
				srr[0] = nia;
				srr[1] = msr;
				memory_write32(0xcc003000, 0x00000100);
				memory_write32(0xcc003004, 0x00000100);
				nia = 0x80000500;
			}

			u32 swapaddr = swaponle_u32(viregs[7]);
			if(swapaddr & 0x10000000) xfbbase = (u8 *)mem_arenaA + ((swapaddr & 0x0FFFFFFF) << 5); /* page offset bit */
			else xfbbase = (u8 *)mem_arenaA + (swapaddr);

			//printf("%08x\n", viregs[7]);
			//printf("%08x\n", xfbbase);

			for(h = 0; h < 480; h++)
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

			//char framedumpname[100];
			//snprintf(framedumpname, 100, "%llu.bmp", numblanks);
			//liybmp_save(framedumpname, 640, 480, xfbConv);

			#ifdef SHITCUBE_SDL
			while(SDL_PollEvent(&event) != 0) {
				if(event.type == SDL_QUIT) break;
				if(event.type == SDL_WINDOWEVENT) if(event.window.event == SDL_WINDOWEVENT_CLOSE) break;
			}
			//printf("%s\n", SDL_GetError());
			windowSurface = SDL_GetWindowSurface(window);
			if(!windowSurface) {
				printf("I couldn't aquire SDL window surface... %s\n", SDL_GetError());
				return -3;
			}
			//SDL_FillRect(windowSurface, NULL, SDL_MapRGB(windowSurface->format, 0, 0, 0));
			for(int i = 0; i < (640*480); i++) {
				int i3 = i * 3;
				int i4 = i * 4; //assume RGBA32 erroneously
				((u8 *)(windowSurface->pixels))[i4  ] = ((u8 *)xfbConv)[i3+2];
				((u8 *)(windowSurface->pixels))[i4+1] = ((u8 *)xfbConv)[i3+1];
				((u8 *)(windowSurface->pixels))[i4+2] = ((u8 *)xfbConv)[i3  ];
				((u8 *)(windowSurface->pixels))[i4+3] = 255;
			}
			SDL_UpdateWindowSurface(window);
			#endif
		}

		tb = ~(u64)0 - cycles;
		tbl = tb & 0xFFFFFFFF;
		tbu = (tb >> 32) & 0xFFFFFFFF;

		opcode = memory_usread32(nia);
		pc = nia;
		nia = pc + 4;
		swopcode = swaponle_u32(opcode);
		#ifdef INTERPRETER_PRINT
		printf("%08x, %08x ", pc, swopcode);
		#endif
		opcode_execute(opcode, swopcode);
		#ifdef INTERPRETER_PRINT
		putchar('\n');
		for(int i = 0; i < 32; i++) printf("r%d %x, ", i, gpr[i]);
		putchar('\n');
		#endif
	}
	printf("I'm done cycling!!\n\n");

	ramdump = fopen("out.raw", "wb");
	fwrite(mem_arenaA, 24*1024, 1024, ramdump);
	fclose(ramdump);
	/*xfbdump = fopen("xfb.raw", "wb");
	fwrite((u8 *)mem_arenaA + (viregs[7]), 640*480, 2, xfbdump);
	fclose(xfbdump);*/

	/* hardwarereg_printall(); */
	free(xfbConv);

	memory_freeArenas();
	interpreter_free();
	hardwarereg_free();

	#ifdef SHITCUBE_SDL
	SDL_DestroyWindow(window);
	SDL_Quit();
	#endif
	return 0;
}

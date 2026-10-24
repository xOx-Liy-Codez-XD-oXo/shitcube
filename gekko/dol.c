#include "memory.h"
#include "dol.h"
#include "../global/endian.h"
#include "interpreter_reg.h"

void dol_loadFromFile(FILE * dolfile) {
	dolheader thedol;
	int i, j;

	if(!mem_arenaA) return;
	for(i = 0; i < 7; i++) {
		fread(&(thedol.filetextoff[i]), 4, 1, dolfile);
		thedol.filetextoff[i] = swaponle_u32(thedol.filetextoff[i]);
	}
	for(i = 0; i < 11; i++) {
		fread(&(thedol.filedataoff[i]), 4, 1, dolfile);
		thedol.filedataoff[i] = swaponle_u32(thedol.filedataoff[i]);
	}
	for(i = 0; i < 18; i++) {
		fread(&(thedol.loadaddrs[i]), 4, 1, dolfile);
		thedol.loadaddrs[i] = swaponle_u32(thedol.loadaddrs[i]);
	}
	for(i = 0; i < 18; i++) {
		fread(&(thedol.sectionsizes[i]), 4, 1, dolfile);
		thedol.sectionsizes[i] = swaponle_u32(thedol.sectionsizes[i]);
	}
	fread(&(thedol.bssaddr), 4, 1, dolfile);
	thedol.bssaddr = swaponle_u32(thedol.bssaddr);
	fread(&(thedol.bsssize), 4, 1, dolfile);
	thedol.bsssize = swaponle_u32(thedol.bsssize);
	fread(&(thedol.entrypoint), 4, 1, dolfile);
	thedol.entrypoint = swaponle_u32(thedol.entrypoint);
	/* Dol header prepared and workable now */
	
	for(i = 0; i < 7; i++) {
		if(thedol.sectionsizes[i] == 0) continue;
		printf("section %d\n0x%08x bytes large\nat offset 0x%08x\nloadaddr 0x%08x\n", 
		i, thedol.sectionsizes[i], thedol.filetextoff[i], thedol.loadaddrs[i]); 
		fseek(dolfile, thedol.filetextoff[i], SEEK_SET);
		fread((u8 *)mem_arenaA + (thedol.loadaddrs[i] & ~0x80000000), thedol.sectionsizes[i], 1, dolfile);
	}
	for(i = 0; i < 11; i++) {
		j = i + 7;
		if(thedol.sectionsizes[j] == 0) continue;
		printf("section %d\n0x%08x bytes large\nat offset 0x%08x\nloadaddr 0x%08x\n", 
		j, thedol.sectionsizes[j], thedol.filedataoff[i], thedol.loadaddrs[j]); 
		fseek(dolfile, thedol.filedataoff[i], SEEK_SET);
		fread((u8 *)mem_arenaA + (thedol.loadaddrs[j] & ~0x80000000), thedol.sectionsizes[j], 1, dolfile);
	}
	pc = thedol.entrypoint;
}

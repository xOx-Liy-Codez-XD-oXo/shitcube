#include "memory.h"
#include "elf.h"
#include "../global/endian.h"
#include "interpreter_reg.h"

void elf_loadFromFile(FILE * elffile) {
	elfheader theelf;
	/* elfsectionheader thesh; */
	elfprogramheader theph;
	int i;

	if(!mem_arenaA) return;

	for(i = 0; i < 16; i++) fread(&(theelf.ident[i]), 1, 1, elffile);
	fread(&(theelf.type), 2, 1, elffile);
	theelf.type = swaponle_u16(theelf.type);
	fread(&(theelf.machine), 2, 1, elffile);
	theelf.machine = swaponle_u16(theelf.machine);
	fread(&(theelf.version), 4, 1, elffile);
	theelf.version = swaponle_u32(theelf.version);
	fread(&(theelf.entry), 4, 1, elffile);
	theelf.entry = swaponle_u32(theelf.entry);
	fread(&(theelf.phoff), 4, 1, elffile);
	theelf.phoff = swaponle_u32(theelf.phoff);
	fread(&(theelf.shoff), 4, 1, elffile);
	theelf.shoff = swaponle_u32(theelf.shoff);
	fread(&(theelf.flags), 4, 1, elffile);
	theelf.flags = swaponle_u32(theelf.flags);
	fread(&(theelf.ehsize), 2, 1, elffile);
	theelf.ehsize = swaponle_u16(theelf.ehsize);
	fread(&(theelf.phentsize), 2, 1, elffile);
	theelf.phentsize = swaponle_u16(theelf.phentsize);
	fread(&(theelf.phnum), 2, 1, elffile);
	theelf.phnum = swaponle_u16(theelf.phnum);
	fread(&(theelf.shentsize), 2, 1, elffile);
	theelf.shentsize = swaponle_u16(theelf.shentsize);
	fread(&(theelf.shnum), 2, 1, elffile);
	theelf.shnum = swaponle_u16(theelf.shnum);
	fread(&(theelf.shstrndx), 2, 1, elffile);
	theelf.shstrndx = swaponle_u16(theelf.shstrndx);

	/* Checks....... */
	if(theelf.type != 0x02) return; /* EXEC */
	if(theelf.machine != 0x14) return; /* PowerPC */
	
	printf("Yay!\n");


	/* Load program header */

	for(i = 0; i < theelf.phnum; i++) {
		fseek(elffile, theelf.phoff + (i * 0x20), SEEK_SET);

		fread(&(theph.type), 4, 1, elffile);
		theph.type = swaponle_u32(theph.type);
		fread(&(theph.offset), 4, 1, elffile);
		theph.offset = swaponle_u32(theph.offset);
		fread(&(theph.vaddr), 4, 1, elffile);
		theph.vaddr = swaponle_u32(theph.vaddr);
		fread(&(theph.paddr), 4, 1, elffile);
		theph.paddr = swaponle_u32(theph.paddr);
		fread(&(theph.filesz), 4, 1, elffile);
		theph.filesz = swaponle_u32(theph.filesz);
		fread(&(theph.memsz), 4, 1, elffile);
		theph.memsz = swaponle_u32(theph.memsz);
		fread(&(theph.flags), 4, 1, elffile);
		theph.flags = swaponle_u32(theph.flags);
		fread(&(theph.align), 4, 1, elffile);
		theph.align = swaponle_u32(theph.align);

		fseek(elffile, theph.offset, SEEK_SET);
		fread((u8 *)mem_arenaA + (theph.vaddr & ~0x80000000), theph.filesz, 1, elffile);
	}

	/* We dont do section header yet */

	pc = theelf.entry;
}

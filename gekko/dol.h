#ifndef DOL_H
#define DOL_H

#include <stdio.h>
#include "../global/type.h"

typedef struct {
	u32 filetextoff[7];
	u32 filedataoff[11];
	u32 loadaddrs[18];
	u32 sectionsizes[18];
	u32 bssaddr;
	u32 bsssize;
	u32 entrypoint;
} dolheader;

void dol_loadFromFile(FILE * dolfile);

#endif

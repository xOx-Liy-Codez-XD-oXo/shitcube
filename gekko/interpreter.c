#include "interpreter.h"
#include "interpreter_reg.h"
#include <stdlib.h>
#include <stdio.h>
#include "instruction.h"

u32 * regs;

u32 pc; /* Program counter */
u32 nia; /* next instruction address */


/* Architecture defined registers */

u32
cr,    /* Condition register */
fpscr, /* Floating point status/control register */
* gpr, /* 32 general purpose register */
msr,   /* Machine state register */
* sr;  /* 16 segment registers */


/* 32 floating point registers */

u64 * fpr;


/* Special purpose registers */

u64 tb;

u32 
lr,     /* Link register */
* bat,  /* 32 block address translation regs */
ctr,    /* CTR */
dabr,   /* data address breakpoint register */
dar,    /* Data address register */
dec,    /* Decrementer register */
dsisr,  /* data exception cause register */
ear,    /* External access register */
pvr,    /* Processor version register */
sdr1,   /* page table address and size */
* srr,  /* 2 machine status save/restore regs */
* sprg, /* 4 reg operating system use */
tbu,    /* time base upper */
tbl,    /* time base lower */
xer,    /* arithmetic bits and some count stuff */


/* Implementation specific registers */

dmal,    /* DMA lower */
dmau,    /* DMA upper */
* gqr,   /* 8 quantization registers */
* hid,   /* 4 HID registers for shit */
iabr,    /* instruction addr breakpoint reg */
ictc,    /* icache throttling control register */
l2cr,    /* l2 cache control */
* mmcr,  /* 2 monitor mode control registers */
* pmc,   /* 4 performance monitor counter reg */
sia,     /* sampled instruction addr reg */
* thrm,  /* thermal control reg */
* ummcr, /* 2 user monitor mode controls */
* upmc,  /* 4 user performance monitor counter */
usia,    /* user sampled instruction addr reg */
wpar;    /* write gather pipe addr reg */

void * tablehole;

/* SPR reference table */
u32 ** sprtable;

/* Opcode execute tables */
void (**opcode_executetablea)(u32);
void (**opcode_executetableb)(u32);
void (**opcode_executetablec)(u32);
void (**opcode_executetabled)(u32);

void interpreter_setup_tables(void) {
	int i;
	/* Interpreter opcode execute tables */
	for(i = 0; i < 1024; i++) opcode_executetablea[i] = ii_invalid;
	opcode_executetablea[16] = ii_bclr;
	opcode_executetablea[50] = ii_rfi;
	opcode_executetablea[150] = ii_isync;
	opcode_executetablea[528] = ii_bcctr;

	for(i = 0; i < 1024; i++) opcode_executetableb[i] = ii_invalid;
	opcode_executetableb[0] = ii_cmp;
	opcode_executetableb[8] = ii_subfc;
	opcode_executetableb[10] = ii_addc;
	opcode_executetableb[11] = ii_mulhwu;
	opcode_executetableb[19] = ii_mfcr;
	opcode_executetableb[23] = ii_lwzx;
	opcode_executetableb[24] = ii_slw;
	opcode_executetableb[26] = ii_cntlzw;
	opcode_executetableb[28] = ii_and;
	opcode_executetableb[32] = ii_cmpl;
	opcode_executetableb[40] = ii_subf;
	opcode_executetableb[60] = ii_andc;
	opcode_executetableb[75] = ii_mulhw;
	opcode_executetableb[83] = ii_mfmsr;
	opcode_executetableb[86] = ii_dcbf;
	opcode_executetableb[87] = ii_lbzx;
	opcode_executetableb[104] = ii_neg;
	opcode_executetableb[124] = ii_nor;
	opcode_executetableb[136] = ii_fnabs;
	opcode_executetableb[138] = ii_adde;
	opcode_executetableb[144] = ii_mtcrf;
	opcode_executetableb[146] = ii_mtmsr;
	opcode_executetableb[151] = ii_stwx;
	opcode_executetableb[202] = ii_addze;
	opcode_executetableb[210] = ii_mtsr;
	opcode_executetableb[215] = ii_stbx;
	opcode_executetableb[234] = ii_addme;
	opcode_executetableb[235] = ii_mullw;
	opcode_executetableb[266] = ii_add;
	opcode_executetableb[279] = ii_lhzx;
	opcode_executetableb[316] = ii_xor;
	opcode_executetableb[339] = ii_mfspr;
	opcode_executetableb[371] = ii_mftb;
	opcode_executetableb[407] = ii_sthx;
	opcode_executetableb[444] = ii_or;
	opcode_executetableb[459] = ii_divwu;
	opcode_executetableb[467] = ii_mtspr;
	opcode_executetableb[470] = ii_dcbi;
	opcode_executetableb[536] = ii_srw;
	opcode_executetableb[598] = ii_sync;
	opcode_executetableb[792] = ii_sraw;
	opcode_executetableb[824] = ii_srawi;
	opcode_executetableb[854] = ii_eieio;
	opcode_executetableb[922] = ii_extsb;
	opcode_executetableb[954] = ii_extsb;
	opcode_executetableb[982] = ii_icbi;

	for(i = 0; i < 1024; i++) opcode_executetablec[i] = ii_invalid;
	for(i = 0; i < 1024; i++) opcode_executetabled[i] = ii_invalid;
	opcode_executetabled[38] = ii_mtfsb1;
	opcode_executetabled[72] = ii_fmr;
	opcode_executetabled[711] = ii_mtfsf;

	/* SPR references */
	sprtable[1] = &xer;
	sprtable[8] = &lr;
	sprtable[9] = &ctr;
	sprtable[18] = &dsisr;
	sprtable[19] = &dar;
	sprtable[22] = &dec;
	sprtable[25] = &dec;
	sprtable[26] = &(srr[0]);
	sprtable[27] = &(srr[1]);
	sprtable[272] = &(sprg[0]);
	sprtable[273] = &(sprg[1]);
	sprtable[274] = &(sprg[2]);
	sprtable[275] = &(sprg[3]);
	sprtable[282] = &ear;
	sprtable[268] = &tbl;
	sprtable[269] = &tbu;
	sprtable[284] = &tbl;
	sprtable[285] = &tbu;
	/* IBAT 0 - 7 */
	sprtable[528] = &(bat[0]);
	sprtable[529] = &(bat[1]);
	sprtable[530] = &(bat[2]);
	sprtable[531] = &(bat[3]);
	sprtable[532] = &(bat[4]);
	sprtable[533] = &(bat[5]);
	sprtable[534] = &(bat[6]);
	sprtable[535] = &(bat[7]);
	sprtable[560] = &(bat[8]);
	sprtable[561] = &(bat[9]);
	sprtable[562] = &(bat[10]);
	sprtable[563] = &(bat[11]);
	sprtable[564] = &(bat[12]);
	sprtable[565] = &(bat[13]);
	sprtable[566] = &(bat[14]);
	sprtable[567] = &(bat[15]);
	/* DBAT 0 - 7 */
	sprtable[536] = &(bat[16]);
	sprtable[537] = &(bat[17]);
	sprtable[538] = &(bat[18]);
	sprtable[539] = &(bat[19]);
	sprtable[540] = &(bat[20]);
	sprtable[541] = &(bat[21]);
	sprtable[542] = &(bat[22]);
	sprtable[543] = &(bat[23]);
	sprtable[568] = &(bat[24]);
	sprtable[569] = &(bat[25]);
	sprtable[570] = &(bat[26]);
	sprtable[571] = &(bat[27]);
	sprtable[572] = &(bat[28]);
	sprtable[573] = &(bat[29]);
	sprtable[574] = &(bat[30]);
	sprtable[575] = &(bat[31]);
	sprtable[912] = &(gqr[0]);
	sprtable[913] = &(gqr[1]);
	sprtable[914] = &(gqr[2]);
	sprtable[915] = &(gqr[3]);
	sprtable[916] = &(gqr[4]);
	sprtable[917] = &(gqr[5]);
	sprtable[918] = &(gqr[6]);
	sprtable[919] = &(gqr[7]);
	sprtable[920] = &(hid[2]);
	sprtable[921] = &wpar;
	sprtable[922] = &dmau;
	sprtable[923] = &dmal;
	sprtable[936] = &(ummcr[0]);
	sprtable[952] = &(mmcr[0]);
	sprtable[953] = &(pmc[0]); /* pmc indx -1 ..*/
	sprtable[954] = &(pmc[1]);
	sprtable[956] = &(mmcr[1]);
	sprtable[957] = &(pmc[2]);
	sprtable[958] = &(pmc[3]);
	sprtable[1008] = &(hid[0]);
	sprtable[1009] = &(hid[1]);
	sprtable[1011] = &(hid[3]); /* HID4 */
	
	sprtable[1017] = &l2cr;
}

void interpreter_setup(void) {
	if(!regs) regs = malloc(256 * sizeof(u32));
	fpr = (u64 *)&(regs[0]); /* 32 * 2 */
	gpr = &(regs[64]);       /* 32 */
	sr = &(regs[96]);        /* 16 */
	bat = &(regs[112]);      /* 32 */
	srr = &(regs[144]);      /* 2 */
	sprg = &(regs[146]);     /* 4 */
	gqr = &(regs[150]);      /* 8 */
	hid = &(regs[158]);      /* 4 */
	mmcr = &(regs[162]);     /* 2 */
	pmc = &(regs[164]);      /* 4 */
	thrm = &(regs[168]);     /* 3 but i dont want odds so 4 */
	ummcr = &(regs[172]);    /* 2 */
	upmc = &(regs[174]);     /* 4 */

	if(!tablehole) tablehole = malloc((8*1024*sizeof(void*))+127);
	sprtable = (void *)(((uip)tablehole + 63) & ~63);
	opcode_executetablea = ((void (**)(u32))sprtable + (1024*sizeof(void*)));
	opcode_executetableb = opcode_executetablea + (1024*sizeof(void*));
	opcode_executetablec = opcode_executetableb + (1024*sizeof(void*));
	opcode_executetabled = opcode_executetablec + (1024*sizeof(void*));

	interpreter_setup_tables();
	/* printf("INTERPRETER SETUP!\n"); */

	msr = 0x00002032;
}

void interpreter_free(void) {
	if(regs) free(regs);
	regs = NULL;
	if(tablehole) free(sprtable);
	tablehole = NULL;
}

u32 rotmask(u32 mb, u32 me) {
	u32 start = 0xFFFFFFFF >> mb;
	u32 end = 0x7FFFFFFF >> me;
	u32 mask = start ^ end;
	if (me < mb) return ~mask;
	return mask;
}

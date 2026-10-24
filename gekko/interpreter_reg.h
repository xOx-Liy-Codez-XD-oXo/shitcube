#ifndef INTERPRETER_REG_H
#define INTERPRETER_REG_H

#include "../global/type.h"

extern u32 pc; /* Program counter */
extern u32 nia; /* next instruction address */

/* Architecture defined registers */

extern u32
cr,    /* Condition register */
fpscr, /* Floating point status/control register */
* gpr, /* 32 general purpose register */
msr,   /* Machine state register */
* sr;  /* 16 segment registers */


/* 32 floating point registers */

extern u64 * fpr;


/* Special purpose registers */

extern u64 tb;

extern u32 
lr,     /* Link register */
* bat,  /* 16 block address translation regs */
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

#endif

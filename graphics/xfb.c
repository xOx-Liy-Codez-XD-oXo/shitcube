#include <stdlib.h>
#include <stdio.h>
#include "xfb.h"

u8 * xfblutr = NULL;
u8 * xfblutb = NULL;
u8 * xfblutg = NULL;
u8 * xfblutgcb = NULL;

void xfb_generatelut(void) {
	unsigned int i;
	float r1, g1, b1, y1, cb, cr;
	int ir1, ig1, ib1;
	if(!xfblutr) xfblutr = malloc(64*1024);
	if(!xfblutb) xfblutb = malloc(64*1024);
	if(!xfblutg) xfblutg = malloc(64*1024);
	if(!xfblutgcb) xfblutgcb = malloc(256);
	
	for(i = 0; i < 64*1024; i++) {
		y1 = i / 256;
		cr = i % 256;
		r1 = y1 + 1.371 * (cr - 128.0f);
		r1 = r1 > 255 ? 255 : r1;
		ir1 = r1 < 0 ? 0 : r1;
		xfblutr[i] = ir1;
	}

	for(i = 0; i < 64*1024; i++) {
		y1 = i / 256;
		cb = i % 256;
		b1 = y1 + 1.732 * (cb - 128.0f);
		b1 = b1 > 255 ? 255 : b1;
		ib1 = b1 < 0 ? 0 : b1;
		xfblutb[i] = ib1;
	}

	for(i = 0; i < 64*1024; i++) {
		y1 = i / 256;
		cr = i % 256;
		g1 = y1 - 0.698 * (cr - 128.0f);
		g1 = g1 > 255 ? 255 : g1;
		ig1 = g1 < 0 ? 0 : g1;
		xfblutg[i] = ig1;
	}

	for(i = 0; i < 256; i++) {
		cb = 0.336f * ((float)i - 128.0f);
		cb = cb > 255 ? 255 : cb;
		xfblutgcb[i] = cb < 0 ? 0 : cb;
	}
}

u8 xfb_lutlookupr(int y, int cr) {
	return xfblutr[(y << 8) | cr];
}

u8 xfb_lutlookupb(int y, int cb) {
	return xfblutb[(y << 8) | cb];
}

u8 xfb_lutlookupgcb(int cb) {
	return xfblutgcb[cb];
}

u8 xfb_lutlookupg(int y, signed int cr, int cb) {
	int a;
	a = xfblutg[(y << 8) | cr] - xfb_lutlookupgcb(cb);
	return a > 0 ? a : 0;
}

void xfb_freelut(void) {
	if(xfblutr) free(xfblutr);
	if(xfblutb) free(xfblutb);
	if(xfblutg) free(xfblutg);
	if(xfblutgcb) free(xfblutgcb);
}
#ifndef LIYBMP_H
#define LIYBMP_H

#include <string.h>
#include "type.h"
#include "../gekko/hardware_reg.h"

struct liybmp_fileheader {
	unsigned char magic[2];
	long filesize;
	short reserved1;
	short reserved2;
	unsigned long offset;
};

struct liybmp_imgheader {
	unsigned long headersize;
	unsigned long w;
	unsigned long h;
	unsigned short planes;
	unsigned short bitcount;
	unsigned long compressed;
	unsigned long imagesize;
	unsigned long ppmX;
	unsigned long ppmY;
	unsigned long colorused;
	unsigned long colorimportant;
};

void liybmp_writefh(struct liybmp_fileheader * fh, FILE * outfile) {
	if(!outfile) return;
	fwrite(&fh->magic, 1, 2, outfile);
	fwrite(&fh->filesize, 1, 4, outfile);
	fwrite(&fh->reserved1, 1, 2, outfile);
	fwrite(&fh->reserved2, 1, 2, outfile);
	fwrite(&fh->offset, 1, 4, outfile);
}

void liybmp_writeih(struct liybmp_imgheader * ih, FILE * outfile) {
	if(!outfile) return;
	fwrite(&ih->headersize, 1, 4, outfile);
	fwrite(&ih->w, 1, 4, outfile);
	fwrite(&ih->h, 1, 4, outfile);
	fwrite(&ih->planes, 1, 2, outfile);
	fwrite(&ih->bitcount, 1, 2, outfile);
	fwrite(&ih->compressed, 1, 4, outfile);
	fwrite(&ih->imagesize, 1, 4, outfile);
	fwrite(&ih->ppmX, 1, 4, outfile);
	fwrite(&ih->ppmY, 1, 4, outfile);
	fwrite(&ih->colorused, 1, 4, outfile);
	fwrite(&ih->colorimportant, 1, 4, outfile);
	
}

int liybmp_save(const char *filename, unsigned long w, unsigned long h, unsigned char * pels) {
	FILE *outfile;
	unsigned long size = w*h;
	unsigned long filesize = (3*size) + 54;
	unsigned long ppm = (unsigned long)(72.0f * 39.375f); /* pixels per meter */
	unsigned long i, j, pos, pos3;
	struct liybmp_fileheader bfh;
	struct liybmp_imgheader bih;

	memset(&bfh, 0, sizeof(struct liybmp_fileheader));
	memset(&bih, 0, sizeof(struct liybmp_imgheader));

	bfh.magic[0] = 'B'; bfh.magic[1] = 'M';
	bfh.filesize = filesize;
	bfh.reserved1 = 0x494c; /* YL */
	bfh.reserved2 = 0x594c; /* IL */
	bfh.offset = 54;

	bih.headersize = 40;
	bih.w = w; bih.h = h; bih.planes = 1; bih.bitcount = 24;
	bih.ppmX = ppm; bih.ppmY = ppm;

	outfile = fopen(filename, "wb");
	if(outfile == NULL) return -1;

	liybmp_writefh(&bfh, outfile);
	liybmp_writeih(&bih, outfile);

	for(i = 0; i < h; i++) {
		for(j = 0; j < w; j++) {
			pos = ((h-i)*w)+j;
			pos3 = pos * 3;
			fwrite(&pels[pos3+2], 1, 1, outfile);
			fwrite(&pels[pos3+1], 1, 1, outfile);
			fwrite(&pels[pos3  ], 1, 1, outfile);
		}
	}

	fclose(outfile);
	return 0;
}

/*int liybmp_saveefb(const char *filename) {
	FILE *outfile;
	int w = 640;
	int h = 528 * 2;
	unsigned long size = w*h;
	unsigned long filesize = (4*size) + 54;
	unsigned long ppm = (unsigned long)(72.0f * 39.375f); //pixels per meter
	unsigned long i, j, pos, pos3;
	struct liybmp_fileheader bfh;
	struct liybmp_imgheader bih;

	memset(&bfh, 0, sizeof(struct liybmp_fileheader));
	memset(&bih, 0, sizeof(struct liybmp_imgheader));

	bfh.magic[0] = 'B'; bfh.magic[1] = 'M';
	bfh.filesize = filesize;
	bfh.reserved1 = 0x494c; //YL
	bfh.reserved2 = 0x594c; //IL
	bfh.offset = 54;

	bih.headersize = 40;
	bih.w = w; bih.h = h; bih.planes = 1; bih.bitcount = 32;
	bih.ppmX = ppm; bih.ppmY = ppm;

	outfile = fopen(filename, "wb");
	if(outfile == NULL) return -1;

	liybmp_writefh(&bfh, outfile);
	liybmp_writeih(&bih, outfile);

	for(i = 0; i < h; i++) {
		for(j = 0; j < w; j++) {
			liycol a;
			efb_readrgbadirect(j, h-i, &a);
			fwrite(&a.b, 1, 1, outfile);
			fwrite(&a.g, 1, 1, outfile);
			fwrite(&a.r, 1, 1, outfile);
			fwrite(&a.a, 1, 1, outfile);
		}
	}

	fclose(outfile);
	return 0;
}*/

#endif

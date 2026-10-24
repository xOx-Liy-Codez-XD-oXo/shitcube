#include "interpreter_reg.h"
#include "interpreter.h"
#include "../global/type.h"
#include "../global/endian.h"
#include "memory.h"
#include <stdio.h>
#include <stdlib.h>

/* ii = interpreter instruction */

#ifdef INTERPRETER_PRINT
#define II_PRINT
#endif

void ii_invalid(u32 opcode) {
	printf("Invalid\n");
	exit(10000000);
}

void ii_nop(u32 opcode) {
	printf("No implement\n");
	exit(10000000);
}

void ii_cr0thing(s32 value) {
	int c = 0;
	u32 so, unfield;
	if(value == 0) c |= 2;
	else if(value > 0) c |= 4;
	     else c |= 8;
	so = (xer >> 31) & 1;
	c |= (u32)so;
	unfield = cr & 0x0FFFFFFF;
	cr = c << 28;
	cr |= unfield;
	#ifdef II_PRINT
	printf("Cr0thing inval is %d.. ", value);
	#endif
}

void ii_xersetoverflow(void) {
	xer |= 0x40000000; /* OV */
	xer |= 0x80000000; /* SO */
}

void ii_checkoverflow(u32 a, u32 b, u32 c) {
	int d = (((a ^ c) & (b ^ c)) >> 31) != 0;
	if(d) ii_xersetoverflow();
}

void ii_add(u32 opcode) {
	u32 swopcode = swaponle_u32(opcode);
	u8 rd = (swopcode >> 21) & 0x1F, ra = (swopcode >> 16) & 0x1F, rb = (swopcode >> 11) & 0x1f,
	oe = (swopcode >> 10) & 1, rc = swopcode & 1;
	
	((u32 *)gpr)[rd] = ((u32 *)gpr)[ra] + ((u32 *)gpr)[rb];

	if(oe) ii_checkoverflow(gpr[ra], gpr[rb], gpr[rd]);
	if(rc) ii_cr0thing(((s32 *)gpr)[rd]);

	#ifdef II_PRINT
	printf("Add, r%d now %08x", rd, ((u32 *)gpr)[rd]);
	#endif
}

void ii_addc(u32 opcode) {
	u32 swopcode = swaponle_u32(opcode);
	u8 rd = (swopcode >> 21) & 0x1F, ra = (swopcode >> 16) & 0x1F, rb = (swopcode >> 11) & 0x1f,
	oe = (swopcode >> 10) & 1, rc = swopcode & 1;
	
	((s32 *)gpr)[rd] = ((s32 *)gpr)[ra] + ((s32 *)gpr)[rb];

	if(oe) {
		ii_checkoverflow(gpr[ra], gpr[rb], gpr[rd]);
		printf("Addc: Carry bookkeeping not implemented\n"); exit(-500);
	}
	if(rc) ii_cr0thing(((s32 *)gpr)[rd]);

	#ifdef II_PRINT
	printf("Addc, r%d now %08x", rd, ((s32 *)gpr)[rd]);
	#endif
}

void ii_adde(u32 opcode) {
	u32 swopcode = swaponle_u32(opcode);
	u8 rd = (swopcode >> 21) & 0x1F, ra = (swopcode >> 16) & 0x1F, rb = (swopcode >> 11) & 0x1f,
	oe = (swopcode >> 10) & 1, rc = swopcode & 1;
	
	((s32 *)gpr)[rd] = ((s32 *)gpr)[ra] + ((s32 *)gpr)[rb];

	if(oe) {
		ii_checkoverflow(gpr[ra], gpr[rb], gpr[rd]);
		printf("Adde: Carry bookkeeping not implemented\n"); exit(-500);
	}
	if(rc) ii_cr0thing(((s32 *)gpr)[rd]);

	#ifdef II_PRINT
	printf("Adde, r%d now %08x. Not doing xer stuff..", rd, ((s32 *)gpr)[rd]);
	#endif
}

void ii_addi(u32 opcode) {
	u32 swopcode = swaponle_u32(opcode);
	u8 rd = (swopcode >> 21) & 0x1F, ra = (swopcode >> 16) & 0x1F;
	s16 simm = (swopcode & 0xFFFF);
	if(ra == 0) {
		((u32 *)gpr)[rd] = (u32)simm;
	} else {
		((u32 *)gpr)[rd] = ((u32 *)gpr)[ra] + (u32)simm;
	}
	#ifdef II_PRINT
	printf("Addi, rd 0x%x, ra 0x%x, simm 0x%x. r%d now %08x", rd, ra, simm, rd, ((u32 *)gpr)[rd]);
	#endif
}

void ii_addic(u32 opcode) {
	u32 swopcode = swaponle_u32(opcode);
	u8 rd = (swopcode >> 21) & 0x1F, ra = (swopcode >> 16) & 0x1F;
	s16 simm = (swopcode & 0xFFFF);
	
	((u32 *)gpr)[rd] = ((u32 *)gpr)[ra] + (u32)simm;

	#ifdef II_PRINT
	printf("Addic, rd 0x%x, ra 0x%x, simm 0x%x. r%d now %08x. Can't carry yet ..", rd, ra, simm, rd, ((u32 *)gpr)[rd]);
	#endif
}

void ii_addis(u32 opcode) {
	u32 swopcode = swaponle_u32(opcode);
	u8 rd = (swopcode >> 21) & 0x1F, ra = (swopcode >> 16) & 0x1F;
	s16 simm = swopcode & 0xFFFF;
	if(ra == 0) {
		((u32 *)gpr)[rd] = ((u32)simm << 16);
	} else {
		((s32 *)gpr)[rd] = ((s32 *)gpr)[ra] + ((s32)simm << 16);
	}
	#ifdef II_PRINT
	printf("Addis, rd 0x%x, ra 0x%x, simm 0x%x. r%d now %08x", rd, ra, simm << 16, rd, ((s32 *)gpr)[rd]);
	#endif
}

void ii_addme(u32 opcode) {
	u32 swopcode = swaponle_u32(opcode);
	u8 rd = (swopcode >> 21) & 0x1F, ra = (swopcode >> 16) & 0x1F;
	
	((u32 *)gpr)[rd] = ((u32 *)gpr)[ra] - 1;

	#ifdef II_PRINT
	printf("Addme, rd 0x%x, ra 0x%x, r%d now %08x. Can't carry yet ..", rd, ra, rd, ((u32 *)gpr)[rd]);
	#endif
}

void ii_addze(u32 opcode) {
	u32 swopcode = swaponle_u32(opcode);
	u8 rd = (swopcode >> 21) & 0x1F, ra = (swopcode >> 16) & 0x1F;
	
	((u32 *)gpr)[rd] = ((u32 *)gpr)[ra];

	#ifdef II_PRINT
	printf("Addze, rd 0x%x, ra 0x%x, r%d now %08x. Can't carry yet ..", rd, ra, rd, ((u32 *)gpr)[rd]);
	#endif
}

void ii_and(u32 opcode) {
	u32 swopcode = swaponle_u32(opcode);
	u8 ra = (swopcode >> 16) & 0x1F, rb = (swopcode >> 11) & 0x1F, rs = (swopcode >> 21) & 0x1F,
	rc = swopcode & 1;

	((u32*)gpr)[ra] = ((u32*)gpr)[rs] & ((u32*)gpr)[rb];

	if(rc) ii_cr0thing(((s32 *)gpr)[rs]);

	#ifdef II_PRINT
	printf("And, ");
	#endif
}

void ii_andc(u32 opcode) {
	u32 swopcode = swaponle_u32(opcode);
	u8 ra = (swopcode >> 16) & 0x1F, rb = (swopcode >> 11) & 0x1F, rs = (swopcode >> 21) & 0x1F,
	rc = swopcode & 1;

	((u32*)gpr)[ra] = ((u32*)gpr)[rs] & (~gpr[rb]);

	if(rc) ii_cr0thing(((s32 *)gpr)[rs]);

	#ifdef II_PRINT
	printf("Andc, ");
	#endif
}

void ii_andi(u32 opcode) {
	u32 swopcode = swaponle_u32(opcode);
	u8 rs = (swopcode >> 21) & 0x1F, ra = (swopcode >> 16) & 0x1F;
	u16 uimm = swopcode & 0xFFFF;
	((u32 *)gpr)[ra] = ((u32 *)gpr)[rs] & ((u32)uimm);
	ii_cr0thing(((s32 *)gpr)[ra]);
	#ifdef II_PRINT
	printf("Andi, rs 0x%x, ra 0x%x, uimm 0x%08x", rs, ra, (u32)uimm);
	#endif
}

void ii_andis(u32 opcode) {
	u32 swopcode = swaponle_u32(opcode);
	u8 rs = (swopcode >> 21) & 0x1F, ra = (swopcode >> 16) & 0x1F;
	u16 uimm = swopcode & 0xFFFF;
	((u32 *)gpr)[ra] = ((u32 *)gpr)[rs] & ((u32)uimm << 16);
	ii_cr0thing(((s32 *)gpr)[ra]);
	#ifdef II_PRINT
	printf("Andis, rs 0x%x, ra 0x%x, uimm 0x%x", rs, ra, uimm << 16);
	#endif
}

void ii_b(u32 opcode) {
	u32 swopcode = swaponle_u32(opcode);
	u8 aa = (swopcode >> 1) & 1, lk = swopcode & 1;
	s32 li = (swopcode >> 2) & 0x00FFFFFF;
	u32 cia = nia - 4;
	li = li << 8;
	li = li >> 6;
	if(lk == 1) {
		lr = nia;
	}
	if(aa == 1) {
		nia = li;
	} else {
		nia = cia + li;
	}
	#ifdef II_PRINT
	printf("B, ");
	#endif
}

void ii_bc(u32 opcode) {
	u32 swopcode, target;
	u8 bo, bi, aa, lk;
	s16 bd;
	s32 extbd;
	int ctr_ok, crbi, cond_ok;

	swopcode = swaponle_u32(opcode);
	aa = (swopcode >> 1) & 1, lk = swopcode & 1;
	bo = (swopcode >> 21) & 0x1f;
	bi = (swopcode >> 16) & 0x1f;
	bd = (swopcode >> 2) & 0x3FFF;
	if(bd & 0x2000) bd |= 0xc000;
	extbd = bd;
	
	if(!((bo >> 2) & 1)) ctr--;
	ctr_ok = ((bo >> 2) & 1) | ((ctr != 0) ^ ((bo >> 1) & 1));
	
	crbi = (cr >> (31 - bi)) & 1;
	cond_ok = ((bo >> 4) & 1) | (crbi == ((bo >> 3) & 1));
	
	/* i think the manual is unclear, about whether nia being placed into lr is dependent on the branch being taken 
	it does say "the address of the instruction following the branch instruction" so taht means we should um do it this way*/

	if(lk) lr = nia;

	target = 0;
	if(ctr_ok & cond_ok) {
		target = aa ? (u32)(extbd * 4) : nia + (u32)(extbd * 4) - 4;
		/* if(lk) lr = nia; */
		nia = target;
	}
	
	#ifdef II_PRINT
	printf("Bc, Bo %x, bi %x, bd %08x, aa %d lk %d. Ctr is %d. Cr %08x, Target %08x, THE LR IS 0x%08x!!!!!!!", 
	        bo, bi, extbd, aa, lk, ctr, cr, target, lr);
	#endif
}

void ii_bclr(u32 opcode) {
	u32 swopcode, target, truelr;
	u8 bo, bi, lk;
	int ctr_ok, crbi, cond_ok;

	swopcode = swaponle_u32(opcode);
	lk = swopcode & 1;
	bo = (swopcode >> 21) & 0x1f;
	bi = (swopcode >> 16) & 0x1f;
	
	if(!((bo >> 2) & 1)) ctr--;
	ctr_ok = ((bo >> 2) & 1) | ((ctr != 0) ^ ((bo >> 1) & 1));
	
	crbi = (cr >> (31 - bi)) & 1;
	cond_ok = ((bo >> 4) & 1) | (crbi == ((bo >> 3) & 1));
	
	truelr = lr;
	if(lk) lr = nia;

	target = 0;
	if(ctr_ok & cond_ok) {
		target = truelr & ~3;
		/* if(lk) lr = nia; */
		nia = target;
	}
	
	#ifdef II_PRINT
	printf("Bclr, Bo %x, bi %x, lk %d. Ctr is %d. Target %08x", bo, bi, lk, ctr, target);
	#endif
}

void ii_bcctr(u32 opcode) {
	u32 swopcode, target;
	u8 bo, bi, lk;
	int crbi, cond_ok;

	swopcode = swaponle_u32(opcode);
	lk = swopcode & 1;
	bo = (swopcode >> 21) & 0x1f;
	bi = (swopcode >> 16) & 0x1f;
	
	crbi = (cr >> (31 - bi)) & 1;
	cond_ok = ((bo >> 4) & 1) | (crbi == ((bo >> 3) & 1));
	
	if(lk) lr = nia;

	target = 0;
	if(cond_ok) {
		target = ctr & ~3;
		nia = target;
	}
	
	#ifdef II_PRINT
	printf("Bcctr, Bo %x, bi %x, lk %d. Ctr is %d. Target %08x", bo, bi, lk, ctr, target);
	#endif
}

void ii_cmp(u32 opcode) {
	u32 swopcode;
	u8 ra, rb, crfd, c;
	s32 a, b, field, shift;

	swopcode = swaponle_u32(opcode);
	ra = (swopcode >> 16) & 0x1F, rb = (swopcode >> 11) & 0x1f, crfd = (swopcode >> 23) & 7;
	a = ((s32 *)gpr)[ra], b = ((s32 *)gpr)[rb];

	if (a < b) c = 8;
	else if (a > b) c = 4; /* think these are right */
	else c = 2;

	if((xer >> 31) & 1) c |= 1;

	shift = (crfd * -4) + 28;
	field = cr & ~(0x0Fu << shift);
	field |= (u32)c << shift;
	cr = field;
	
	#ifdef II_PRINT
	printf("Cmp, crfd %d, c %d, cr now %08x", crfd, c, cr);
	#endif
}

void ii_cmpi(u32 opcode) {
	u32 swopcode, field;
	u8 ra, crfd, c;
	s32 a, shift;
	s16 simm;

	swopcode = swaponle_u32(opcode);
	ra = (swopcode >> 16) & 0x1F, crfd = (swopcode >> 23) & 7;
	simm = swopcode & 0xFFFF;
	a = ((s32 *)gpr)[ra];

	if (a < simm) c = 8;
	else if (a > simm) c = 4;
	else c = 2;

	if((xer >> 31) & 1) c |= 1;

	shift = (crfd * -4) + 28;
	field = cr & ~(0x0Fu << shift);
	field |= (u32)c << shift;
	cr = field;
	
	#ifdef II_PRINT
	printf("Cmpi, crfd %d, c %d, cr now %08x", crfd, c, cr);
	#endif
}

void ii_cmpl(u32 opcode) {
	u32 swopcode;
	u8 ra, rb, crfd, c;
	u32 a, b, field, shift;

	swopcode = swaponle_u32(opcode);
	ra = (swopcode >> 16) & 0x1F, rb = (swopcode >> 11) & 0x1f, crfd = (swopcode >> 23) & 7;
	a = ((u32 *)gpr)[ra], b = ((u32 *)gpr)[rb];

	if (a < b) c = 8;
	else if (a > b) c = 4; /* think these are right */
	else c = 2;

	if((xer >> 31) & 1) c |= 1;

	shift = (crfd * -4) + 28;
	field = cr & ~(0x0Fu << shift);
	field |= (u32)c << shift;
	cr = field;
	
	#ifdef II_PRINT
	printf("Cmpl, crfd %d, c %d, cr now %08x", crfd, c, cr);
	#endif
}

void ii_cmpli(u32 opcode) {
	u32 swopcode, field, a;
	u8 ra, crfd, c;
	s32 shift;
	u16 uimm;

	swopcode = swaponle_u32(opcode);
	ra = (swopcode >> 16) & 0x1F, crfd = (swopcode >> 23) & 7;
	uimm = swopcode & 0xFFFF;
	a = ((s32 *)gpr)[ra];

	if (a < uimm) c = 8;
	else if (a > uimm) c = 4;
	else c = 2;

	if((xer >> 31) & 1) c |= 1;

	shift = (crfd * -4) + 28;
	field = cr & ~(0x0Fu << shift);
	field |= (u32)c << shift;
	cr = field;
	
	#ifdef II_PRINT
	printf("Cmpli, crfd %d, c %d, cr now %08x", crfd, c, cr);
	#endif
}

void ii_cntlzw(u32 opcode) {
	u32 swopcode = swaponle_u32(opcode);
	u8 rs = (swopcode >> 21) & 0x1F, ra = (swopcode >> 16) & 0x1F;
	
	int n = 0;
	while(n < 32) {
		if((gpr[rs] >> (31 - n)) & 1) break;	
		n++;
	}
	gpr[ra] = n;

	ii_cr0thing(((s32 *)gpr)[ra]);
	#ifdef II_PRINT
	printf("Cntlzw, rs 0x%x, ra 0x%x", rs, ra);
	#endif
}

void ii_dcbf(u32 opcode) {
	#ifdef II_PRINT
	printf("Dcbf, ");
	#endif
	/* There is no cache.. do nothing */
}

void ii_dcbi(u32 opcode) {
	#ifdef II_PRINT
	printf("Dcbi, ");
	#endif
	/* There is no cache.. do nothing */
}

void ii_divwu(u32 opcode) {
	u32 swopcode = swaponle_u32(opcode);
	u8 ra = (swopcode >> 16) & 0x1F, rb = (swopcode >> 11) & 0x1F, rd = (swopcode >> 21) & 0x1F,
	oe = (swopcode >> 10) & 1, rc = swopcode & 1;

	((u32*)gpr)[rd] = ((u32*)gpr)[ra] / ((u32*)gpr)[rb];

	if(rc) ii_cr0thing(((s32 *)gpr)[rd]);
	if(oe && (gpr[rb] == 0)) ii_xersetoverflow();

	#ifdef II_PRINT
	printf("Divwu, ");
	#endif
}

void ii_eieio(u32 opcode) {
	#ifdef II_PRINT
	printf("Eieio, ");
	#endif
	/* Already in order.. do nothing */
}

void ii_extsb(u32 opcode) {
	u32 swopcode = swaponle_u32(opcode);
	u8 rs = (swopcode >> 21) & 0x1F, ra = (swopcode >> 16) & 0x1F;
	
	s8 tiny_liy = ((s32 *)gpr)[rs] & 0xFF;
	((s32 *)gpr)[ra] = tiny_liy;

	#ifdef II_PRINT
	printf("Extsb, rd 0x%x, ra 0x%x, r%d now %08x.", rs, ra, rs, gpr[rs]);
	#endif
}

void ii_extsh(u32 opcode) {
	u32 swopcode = swaponle_u32(opcode);
	u8 rs = (swopcode >> 21) & 0x1F, ra = (swopcode >> 16) & 0x1F;
	
	s16 BIG_liy = ((s32 *)gpr)[rs] & 0xFFFF;
	((s32 *)gpr)[ra] = BIG_liy;

	#ifdef II_PRINT
	printf("Extsh, rd 0x%x, ra 0x%x, r%d now %08x.", rs, ra, rs, gpr[rs]);
	#endif
}

void ii_fmr(u32 opcode) {
	/* Todo actually do stuff */
	#ifdef II_PRINT
	printf("Fmr, (STUB)");
	#endif
}

void ii_fneg(u32 opcode) {
	/* Todo actually do stuff */
	#ifdef II_PRINT
	printf("Fneg, (STUB)");
	#endif
}

void ii_fnabs(u32 opcode) {
	/* Todo actually do stuff */
	#ifdef II_PRINT
	printf("Fnabs, (STUB)");
	#endif
}

void ii_isync(u32 opcode) {
	#ifdef II_PRINT
	printf("Isync, ");
	#endif
	/* Already in sync.. do nothing */
}

void ii_icbi(u32 opcode) {
	printf("LIY! ");
	/* NO CACHE!.. do nothing */
}

void ii_lbz(u32 opcode) {
	u32 swopcode = swaponle_u32(opcode);
	u8 ra = (swopcode >> 16) & 0x1F, rd = (swopcode >> 21) & 0x1F;
	s16 d = swopcode & 0xFFFF;
	u32 b, ea;
	if(ra == 0) b = 0;
	else b = ((u32 *)gpr)[ra];
	ea = b + d;
	gpr[rd] = memory_read8(ea);
	#ifdef II_PRINT
	printf("Lbz, ra 0x%x, rd 0x%x, d %d (0x%04x), ea 0x%08x. r%d now %08x", ra, rd, d, d, ea, rd, gpr[rd]);
	#endif
}

void ii_lbzu(u32 opcode) {
	u32 swopcode = swaponle_u32(opcode);
	u8 ra = (swopcode >> 16) & 0x1F, rd = (swopcode >> 21) & 0x1F;
	s16 d = swopcode & 0xFFFF;
	u32 b, ea;
	if(ra == 0) b = 0;
	else b = ((u32 *)gpr)[ra];
	ea = b + d;
	gpr[rd] = memory_read8(ea);
	gpr[ra] = ea;
	#ifdef II_PRINT
	printf("Lbz, ra 0x%x, rd 0x%x, d %d (0x%04x), ea 0x%08x. r%d now %08x", ra, rd, d, d, ea, rd, gpr[rd]);
	#endif
}

void ii_lbzx(u32 opcode) {
	u32 swopcode = swaponle_u32(opcode);
	u8 ra = (swopcode >> 16) & 0x1F, rb = (swopcode >> 11) & 0x1F, rd = (swopcode >> 21) & 0x1F;
	u32 b, ea;
	if(ra == 0) b = 0;
	else b = gpr[ra];
	ea = b + gpr[rb];
	gpr[rd] = memory_read8(ea);
	#ifdef II_PRINT
	printf("Lbzx, ra 0x%x, rd 0x%x, rb 0x%04x, ea 0x%08x. r%d now %08x", ra, rd, rb, ea, rd, gpr[rd]);
	#endif
}

void ii_lfd(u32 opcode) {
	/* Todo actually do stuff */
	/*u32 swopcode = swaponle_u32(opcode);
	u8 ra = (swopcode >> 16) & 0x1F, rd = (swopcode >> 21) & 0x1F;
	s16 d = swopcode & 0xFFFF;
	u32 b, ea;
	if(ra == 0) b = 0;
	else b = ((u32 *)gpr)[ra];
	ea = b + d;
	gpr[rd] = memory_read32(ea);*/
	#ifdef II_PRINT
	printf("Lfd, (STUB)");
	#endif
}

void ii_lha(u32 opcode) {
	u32 swopcode = swaponle_u32(opcode);
	u8 ra = (swopcode >> 16) & 0x1F, rd = (swopcode >> 21) & 0x1F;
	s16 d = swopcode & 0xFFFF, micro_liys;
	u32 b, ea;
	if(ra == 0) b = 0;
	else b = gpr[ra];
	ea = b + d;
	micro_liys = memory_read16(ea);
	((s32 *)gpr)[rd] = *((s16 *)&micro_liys);

	#ifdef II_PRINT
	printf("Lha, ra 0x%x, rd 0x%x, d %d (0x%04x), ea 0x%08x. r%d now %08x", ra, rd, d, d, ea, rd, gpr[rd]);
	#endif
}

void ii_lhz(u32 opcode) {
	u32 swopcode = swaponle_u32(opcode);
	u8 ra = (swopcode >> 16) & 0x1F, rd = (swopcode >> 21) & 0x1F;
	s16 d = swopcode & 0xFFFF;
	u32 b, ea;
	if(ra == 0) b = 0;
	else b = ((u32 *)gpr)[ra];
	ea = b + d;
	gpr[rd] = memory_read16(ea);
	#ifdef II_PRINT
	printf("Lhz, ra 0x%x, rd 0x%x, d %d (0x%04x), ea 0x%08x. r%d now %08x", ra, rd, d, d, ea, rd, gpr[rd]);
	#endif
}

void ii_lhzu(u32 opcode) {
	u32 swopcode = swaponle_u32(opcode);
	u8 ra = (swopcode >> 16) & 0x1F, rd = (swopcode >> 21) & 0x1F;
	s16 d = swopcode & 0xFFFF;
	u32 b, ea;
	if(ra == 0) b = 0;
	else b = ((u32 *)gpr)[ra];
	ea = b + d;
	gpr[rd] = memory_read16(ea);
	gpr[ra] = ea;
	#ifdef II_PRINT
	printf("Lhzu, ra 0x%x, rd 0x%x, d %d (0x%04x), ea 0x%08x. r%d now %08x", ra, rd, d, d, ea, rd, gpr[rd]);
	#endif
}

void ii_lhzx(u32 opcode) {
	u32 swopcode = swaponle_u32(opcode);
	u8 ra = (swopcode >> 16) & 0x1F, rb = (swopcode >> 11) & 0x1F, rd = (swopcode >> 21) & 0x1F;
	u32 b, ea;
	if(ra == 0) b = 0;
	else b = gpr[ra];
	ea = b + gpr[rb];
	gpr[rd] = memory_read16(ea);
	#ifdef II_PRINT
	printf("Lhzx, ra 0x%x, rd 0x%x, rb 0x%04x, ea 0x%08x. r%d now %08x", ra, rd, rb, ea, rd, gpr[rd]);
	#endif
}

void ii_lmw(u32 opcode) {
	u32 swopcode = swaponle_u32(opcode);
	u8 ra = (swopcode >> 16) & 0x1F, rd = (swopcode >> 21) & 0x1F, r = 0;
	s16 d = swopcode & 0xFFFF;
	u32 b, ea;
	if(ra == 0) b = 0;
	else b = ((u32 *)gpr)[ra];
	ea = b + d;
	r = rd;
	while(1) {
		if(r > 31) break;
		((u32 *)gpr)[r] = memory_read32(ea);
		r++;
		ea += 4;
	}

	#ifdef II_PRINT
	printf("Lmw, ra 0x%x, rd 0x%x, d 0x%04x, ea 0x%08x", ra, rd, d, ea);
	#endif
}

void ii_lwz(u32 opcode) {
	u32 swopcode = swaponle_u32(opcode);
	u8 ra = (swopcode >> 16) & 0x1F, rd = (swopcode >> 21) & 0x1F;
	s16 d = swopcode & 0xFFFF;
	u32 b, ea;
	if(ra == 0) b = 0;
	else b = ((u32 *)gpr)[ra];
	ea = b + d;
	gpr[rd] = memory_read32(ea);
	#ifdef II_PRINT
	printf("Lwz, ra 0x%x, rd 0x%x, d %d (0x%04x), ea 0x%08x. r%d now %08x", ra, rd, d, d, ea, rd, gpr[rd]);
	#endif
}

void ii_lwzu(u32 opcode) {
	u32 swopcode = swaponle_u32(opcode);
	u8 ra = (swopcode >> 16) & 0x1F, rd = (swopcode >> 21) & 0x1F;
	s16 d = swopcode & 0xFFFF;
	
	u32 ea = ((u32 *)gpr)[ra] + d;
	gpr[rd] = memory_read32(ea);
	((u32 *)gpr)[ra] = ea;
	#ifdef II_PRINT
	printf("Lwzu, ra 0x%x, rd 0x%x, d 0x%04x, ea 0x%08x. r%d now %08x and r%d now %08x", 
	        ra, rd, d, ea, rd, gpr[rd], ra, gpr[ra]);
	#endif
}

void ii_lwzx(u32 opcode) {
	u32 swopcode = swaponle_u32(opcode);
	u8 ra = (swopcode >> 16) & 0x1F, rb = (swopcode >> 11) & 0x1F, rd = (swopcode >> 21) & 0x1F;
	u32 b, ea;
	if(ra == 0) b = 0;
	else b = ((u32 *)gpr)[ra];
	ea = b + gpr[rb];
	gpr[rd] = memory_read32(ea);
	#ifdef II_PRINT
	printf("Lwzx, ra 0x%x, rd 0x%x, rb 0x%04x, ea 0x%08x. r%d now %08x", ra, rd, rb, ea, rd, gpr[rd]);
	#endif
}

void ii_mfcr(u32 opcode) {
	u32 swopcode = swaponle_u32(opcode);
	u8 rd = (u32)(swopcode >> 21) & 0x1F;

	#ifdef II_PRINT
	printf("Mfcr moves CR (%08x) to gpr %d.", cr, rd);
	#endif
	((u32 *)gpr)[rd] = cr;
}

void ii_mfmsr(u32 opcode) {
	u32 swopcode = swaponle_u32(opcode);
	u8 rd = (u32)(swopcode >> 21) & 0x1F;

	#ifdef II_PRINT
	printf("Mfmsr moves MSR (%08x) to gpr %d.", msr, rd);
	#endif
	((u32 *)gpr)[rd] = msr;
}

void ii_mfspr(u32 opcode) {
	u32 swopcode = swaponle_u32(opcode);
	u16 na = (u32)(swopcode >> 11) & 0x1f, nb = (u32)(swopcode >> 16) & 0x1f;
	u32 n = (na << 5) | nb;
	u8 rd = (u32)(swopcode >> 21) & 0x1F;
	#ifdef II_PRINT
	printf("Mfspr moves SPR reg %d (%08x) to gpr %d. %d %d", n, *(sprtable[n]), rd, na, nb);
	#endif
	((u32 *)gpr)[rd] = *(sprtable[n]);
}

void ii_mftb(u32 opcode) {
	u32 swopcode = swaponle_u32(opcode);
	u16 na = (u32)(swopcode >> 11) & 0x1f, nb = (u32)(swopcode >> 16) & 0x1f;
	u32 n = (na << 5) | nb;
	u8 rd = (u32)(swopcode >> 21) & 0x1F;
	#ifdef II_PRINT
	printf("Mftb moves SPR reg %d (%08x) to gpr %d. %d %d", n, *(sprtable[n]), rd, na, nb);
	#endif
	((u32 *)gpr)[rd] = *(sprtable[n]);
}

void ii_mtcrf(u32 opcode) {
	u32 swopcode = swaponle_u32(opcode), mask, i;
	u8 rs = (swopcode >> 21) & 0x1F, crm = (swopcode >> 12);
	
	if(crm == 0xFF) cr = gpr[rs];
	else {
		mask = 0;
		for(i = 0; i < 8; i++) if(crm & (1u << i)) mask |= 0xFu << (i * 4);
		cr = (cr & ~mask) | (gpr[rs] & mask);
	}

	#ifdef II_PRINT
	printf("Mtcrf, ");
	#endif
}

void ii_mtsr(u32 opcode) {
	/* for memory arrangement? we use hardcoded sensible defaults */
	/*
	u32 swopcode = swaponle_u32(opcode);
	u8 rs = (swopcode >> 21) & 0x1F;
	u8 whichseg = (swopcode >> 16) & 0x0f;
	sr[whichseg] = ((u32 *)gpr)[rs];
	#ifdef II_PRINT
	printf("Mtsr, ");
	#endif
	*/
}

void ii_mtmsr(u32 opcode) {
	u32 swopcode = swaponle_u32(opcode);
	u8 rs = (swopcode >> 21) & 0x1F;
	msr = ((u32 *)gpr)[rs];
	#ifdef II_PRINT
	printf("Mtmsr, ");
	#endif
}

void ii_mtfsf(u32 opcode) {
	#ifdef II_PRINT
	printf("Mtfsf, (STUB)");
	#endif
}

void ii_mtfsb1(u32 opcode) {
	#ifdef II_PRINT
	printf("Mtfsb1, (STUB)");
	#endif
}

void ii_mtspr(u32 opcode) {
	u32 swopcode = swaponle_u32(opcode);
	u16 na = (u32)(swopcode >> 11) & 0x1f, nb = (u32)(swopcode >> 16) & 0x1f;
	u32 n = (na << 5) | nb;
	u8 rs = (swopcode >> 21) & 0x1F;
	*(sprtable[n]) = ((u32 *)gpr)[rs];
	#ifdef II_PRINT
	printf("Mtspr moves gpr %d to SPR reg %d (now %08x)", rs, n, *(sprtable[n]));
	#endif
}

void ii_mulli(u32 opcode) {
	u32 swopcode = swaponle_u32(opcode);
	u8 ra = (swopcode >> 16) & 0x1F, rd = (swopcode >> 21) & 0x1F;
	s16 simm = swopcode & 0xFFFF;

	s64 prod = (s64)((s32 *)gpr)[ra] * (s64)simm;
	((s32 *)gpr)[rd] = prod;
	#ifdef II_PRINT
	printf("Mulli, r%d now %08x", rd, ((s32 *)gpr)[rd]);
	#endif
}

void ii_mullw(u32 opcode) {
	s64 prod;
	u32 swopcode = swaponle_u32(opcode), prodb;
	u8 ra = (swopcode >> 16) & 0x1F, rb = (swopcode >> 11) & 0x1F, rd = (swopcode >> 21) & 0x1F,
	oe = (swopcode >> 10) & 1, rc = swopcode & 1;
	prod = ((s32*)gpr)[ra] * ((s32*)gpr)[rb];
	prodb = (u32)prod & 0xFFFFFFFF;

	((s32*)gpr)[rd] = prodb;

	if(rc) ii_cr0thing(((s32 *)gpr)[rd]);
	if(oe) {
		printf("Mullw: Overflow bookkeeping not implemented\n"); exit(-500);
	}

	#ifdef II_PRINT
	printf("Mullw, ");
	#endif
}

void ii_mulhw(u32 opcode) {
	u32 swopcode = swaponle_u32(opcode);
	u8 ra = (swopcode >> 16) & 0x1F, rb = (swopcode >> 11) & 0x1F, rd = (swopcode >> 21) & 0x1F,
	oe = (swopcode >> 10) & 1, rc = swopcode & 1;
	s64 prod;
	prod = ((s32*)gpr)[ra] * ((s32*)gpr)[rb];

	((s32*)gpr)[rd] = prod >> 32;

	if(rc) ii_cr0thing(((s32 *)gpr)[rd]);
	if(oe) {
		printf("Mulhw: Overflow bookkeeping not implemented\n"); exit(-500);
	}

	#ifdef II_PRINT
	printf("Mulhw, ");
	#endif
}

void ii_mulhwu(u32 opcode) {
	u32 swopcode = swaponle_u32(opcode);
	u8 ra = (swopcode >> 16) & 0x1F, rb = (swopcode >> 11) & 0x1F, rd = (swopcode >> 21) & 0x1F,
	oe = (swopcode >> 10) & 1, rc = swopcode & 1;
	u64 prod;
	prod = ((u32*)gpr)[ra] * ((u32*)gpr)[rb];

	((u32*)gpr)[rd] = prod >> 32;

	if(rc) ii_cr0thing(((s32 *)gpr)[rd]);
	if(oe) {
		printf("Mulhwu: Overflow bookkeeping not implemented\n"); exit(-500);
	}

	#ifdef II_PRINT
	printf("Mulhwu, ");
	#endif
}

void ii_neg(u32 opcode) {
	u32 swopcode = swaponle_u32(opcode);
	u8 ra = (swopcode >> 16) & 0x1F, oe = (swopcode >> 10) & 1,
	rd = (swopcode >> 21) & 0x1F, rc = swopcode & 1; 

	gpr[rd] = (~gpr[ra]) + 1;
	#ifdef II_PRINT
	printf("Neg, ");
	#endif
	
	if(oe && (gpr[ra] == 0x80000000)) ii_xersetoverflow();
	if(rc) ii_cr0thing(gpr[ra]);
}

void ii_nor(u32 opcode) {
	u32 swopcode = swaponle_u32(opcode);
	u8 ra = (swopcode >> 16) & 0x1F, rb = (swopcode >> 11) & 0x1f, 
	rs = (swopcode >> 21) & 0x1F, rc = swopcode & 1; 

	((u32 *)gpr)[ra] = ~(((u32 *)gpr)[rs] | ((u32 *)gpr)[rb]);
	#ifdef II_PRINT
	printf("Nor, ra 0x%x, rb 0x%x rs 0x%x. r%d now %08x", ra, rb, rs, ra, ((u32 *)gpr)[ra]);
	#endif
	
	if(rc) ii_cr0thing(gpr[ra]);
}

void ii_or(u32 opcode) {
	u32 swopcode = swaponle_u32(opcode);
	u8 ra = (swopcode >> 16) & 0x1F, rb = (swopcode >> 11) & 0x1f, 
	rs = (swopcode >> 21) & 0x1F, rc = swopcode & 1; 

	((u32 *)gpr)[ra] = ((u32 *)gpr)[rs] | ((u32 *)gpr)[rb];
	#ifdef II_PRINT
	printf("Or, ra 0x%x, rb 0x%x rs 0x%x. r%d now %08x", ra, rb, rs, ra, ((u32 *)gpr)[ra]);
	#endif
	
	if(rc) ii_cr0thing(gpr[ra]);
}

void ii_ori(u32 opcode) {
	u32 swopcode = swaponle_u32(opcode);
	u8 ra = (swopcode >> 16) & 0x1F, rs = (swopcode >> 21) & 0x1F;
	u16 uimm = swopcode & 0xFFFF;
	
	((u32 *)gpr)[ra] = ((u32 *)gpr)[rs] | (u32)uimm;
	#ifdef II_PRINT
	printf("Ori, ra 0x%x, rs 0x%x, uimm 0x%08x. r%d now %08x", 
	        ra, rs, (u32)uimm, ra, ((u32 *)gpr)[ra]);
	#endif
}

void ii_oris(u32 opcode) {
	u32 swopcode = swaponle_u32(opcode);
	u8 ra = (swopcode >> 16) & 0x1F, rs = (swopcode >> 21) & 0x1F;
	u16 uimm = swopcode & 0xFFFF;
	
	((u32 *)gpr)[ra] = ((u32 *)gpr)[rs] | (u32)(uimm << 16);
	#ifdef II_PRINT
	printf("Oris, ra 0x%x, rs 0x%x, uimm 0x%08x. r%d now %08x", 
	        ra, rs, (u32)uimm, ra, ((u32 *)gpr)[ra]);
	#endif
}

void ii_ps_mr(u32 opcode) {
	#ifdef II_PRINT
	printf("Ps_mr, (STUB)");
	#endif
}

void ii_psq_l(u32 opcode) {
	#ifdef II_PRINT
	printf("Psq_l, (STUB)");
	#endif
}

void ii_rfi(u32 opcode) {
	/* todo: take pending exceptions */
	msr = srr[1];
	nia = srr[0] & ~0x03;

	#ifdef II_PRINT
	printf("Rfi");
	#endif
}

void ii_rlwimi(u32 opcode) {
	u32 swopcode = swaponle_u32(opcode), r, m;
	u8 rs = (swopcode >> 21) & 0x1F, ra = (swopcode >> 16) & 0x1F, n,
	sh = (swopcode >> 11) & 0x1f, mb = (swopcode >> 6) & 0x1f, me = (swopcode >> 1) & 0x1f,
	rc = swopcode & 1;
	
	n = sh;
	if(n) r = (((u32 *)gpr)[rs] << n) | (((u32 *)gpr)[rs] >> (32 - n));
	else r = gpr[rs];
	m = rotmask(mb, me);
	((u32 *)gpr)[ra] = (r & m) | (gpr[ra] & ~m);;
	
	if(rc) {
		ii_cr0thing(((s32 *)gpr)[ra]);
	}
	
	#ifdef II_PRINT
	printf("Rlwimi %02x, %02x, %02x, %02x, %02x, %d, %08x, out %08x", 
	        rs, ra, sh, mb, me, rc, cr, ((u32 *)gpr)[ra]);
	#endif
}

void ii_rlwinm(u32 opcode) {
	u32 swopcode = swaponle_u32(opcode), r, m;
	u8 rs = (swopcode >> 21) & 0x1F, ra = (swopcode >> 16) & 0x1F, n,
	sh = (swopcode >> 11) & 0x1f, mb = (swopcode >> 6) & 0x1f, me = (swopcode >> 1) & 0x1f,
	rc = swopcode & 1;
	
	n = sh;
	if(n) r = (((u32 *)gpr)[rs] << n) | (((u32 *)gpr)[rs] >> (32 - n));
	else r = gpr[rs];
	m = rotmask(mb, me);
	((u32 *)gpr)[ra] = r & m;
	
	if(rc) {
		ii_cr0thing(((s32 *)gpr)[ra]);
	}
	
	#ifdef II_PRINT
	printf("Rlwinm %02x, %02x, %02x, %02x, %02x, %d, %08x, out %08x", 
	        rs, ra, sh, mb, me, rc, cr, ((u32 *)gpr)[ra]);
	#endif
}

void ii_sc(u32 opcode) {
	srr[0] = nia;
	srr[1] = 0;
	srr[1] = msr;
	/*idgaf*/

	#ifdef II_PRINT
	printf("Sc, ");
	#endif
}


void ii_slw(u32 opcode) {
	u32 swopcode = swaponle_u32(opcode);
	u8 rs = (swopcode >> 21) & 0x1F, ra = (swopcode >> 16) & 0x1F, rb = (swopcode >> 11) & 0x1f;

	if(gpr[rb] & 32) gpr[ra] = 0;
	else gpr[ra] = (gpr[rs] << (gpr[rb] & 31));

	ii_cr0thing(((s32 *)gpr)[ra]);

	#ifdef II_PRINT
	printf("Slw, rs 0x%x, ra 0x%x, rb 0x%x ", rs, ra, rb);
	#endif
}

void ii_srw(u32 opcode) {
	u32 swopcode = swaponle_u32(opcode);
	u8 rs = (swopcode >> 21) & 0x1F, ra = (swopcode >> 16) & 0x1F, rb = (swopcode >> 11) & 0x1f;

	if(gpr[rb] & 32) gpr[ra] = 0;
	else gpr[ra] = (gpr[rs] >> (gpr[rb] & 31));

	ii_cr0thing(((s32 *)gpr)[ra]);

	#ifdef II_PRINT
	printf("Srw, rs 0x%x, ra 0x%x, rb 0x%x ", rs, ra, rb);
	#endif
}

void ii_sraw(u32 opcode) {
	u32 swopcode = swaponle_u32(opcode);
	u8 rs = (swopcode >> 21) & 0x1F, ra = (swopcode >> 16) & 0x1F, rb = (swopcode >> 11) & 0x1f;

	if(gpr[rb] & 32) gpr[ra] = 0;
	else gpr[ra] = (gpr[rs] >> (gpr[rb] & 31));

	ii_cr0thing(((s32 *)gpr)[ra]);

	#ifdef II_PRINT
	printf("Sraw, rs 0x%x, ra 0x%x, rb 0x%x .. acting the same as srw for now", rs, ra, rb);
	#endif
}

void ii_srawi(u32 opcode) {
	u32 swopcode = swaponle_u32(opcode);
	u8 rs = (swopcode >> 21) & 0x1F, ra = (swopcode >> 16) & 0x1F, sh = (swopcode >> 11) & 0x1F;

	gpr[ra] = gpr[rs] >> sh;

	ii_cr0thing(((s32 *)gpr)[ra]);

	#ifdef II_PRINT
	printf("Srawi, rs 0x%x, ra 0x%x.. acting the same as srw for now", rs, ra);
	#endif
}

void ii_stb(u32 opcode) {
	u32 swopcode = swaponle_u32(opcode);
	u8 ra = (swopcode >> 16) & 0x1F, rs = (swopcode >> 21) & 0x1F;
	s16 d = swopcode & 0xFFFF;
	u32 b, ea;
	if(ra == 0) b = 0;
	else b = ((u32 *)gpr)[ra];
	ea = b + (s32)d;
	#ifdef II_PRINT
	printf("Stb, ra 0x%x, rs 0x%x, d 0x%04x, ea 0x%08x", ra, rs, d, ea);
	#endif
	memory_write8(ea, (u8)(gpr[rs]&0xFF));
}

void ii_stbu(u32 opcode) {
	u32 swopcode = swaponle_u32(opcode);
	u8 ra = (swopcode >> 16) & 0x1F, rs = (swopcode >> 21) & 0x1F;
	s16 d = swopcode & 0xFFFF;
	u32 ea = ((u32 *)gpr)[ra] + d;
	#ifdef II_PRINT
	printf("Stbu, ra 0x%x, rs 0x%x, d 0x%04x, ea 0x%08x", ra, rs, d, ea);
	printf(" wants to store contents of gpr %d (%08x) at %08x and update gpr", rs, gpr[rs], ea);
	#endif
	memory_write8(ea, gpr[rs]);
	((u32 *)gpr)[ra] = ea;
}

void ii_stbx(u32 opcode) {
	u32 swopcode = swaponle_u32(opcode);
	u8 ra = (swopcode >> 16) & 0x1F, rs = (swopcode >> 21) & 0x1F, rb = (swopcode >> 11) & 0x1f;
	u32 b, ea;
	if(ra == 0) b = 0;
	else b = ((u32 *)gpr)[ra];
	ea = b + gpr[rb];
	#ifdef II_PRINT
	printf("Stbx, ra 0x%x, rb 0x%x, rs 0x%x, ea 0x%08x", ra, rb, rs, ea);
	printf(" wants to store contents of gpr %d (%08x) at %08x", rs, gpr[rs], ea);
	#endif
	memory_write8(ea, gpr[rs]);
}

void ii_sth(u32 opcode) {
	u32 swopcode = swaponle_u32(opcode);
	u8 ra = (swopcode >> 16) & 0x1F, rs = (swopcode >> 21) & 0x1F;
	s16 d = swopcode & 0xFFFF;
	u32 b, ea;
	if(ra == 0) b = 0;
	else b = ((u32 *)gpr)[ra];
	ea = b + (s32)d;
	#ifdef II_PRINT
	printf("Sth, ra 0x%x, rs 0x%x, d 0x%04x, ea 0x%08x", ra, rs, d, ea);
	#endif
	memory_write16(ea, (u16)(gpr[rs]&0xFFFF));
}

void ii_sthx(u32 opcode) {
	u32 swopcode = swaponle_u32(opcode);
	u8 ra = (swopcode >> 16) & 0x1F, rs = (swopcode >> 21) & 0x1F, rb = (swopcode >> 11) & 0x1f;
	u32 b, ea;
	if(ra == 0) b = 0;
	else b = ((u32 *)gpr)[ra];
	ea = b + gpr[rb];
	#ifdef II_PRINT
	printf("Sthx, ra 0x%x, rb 0x%x, rs 0x%x, ea 0x%08x", ra, rb, rs, ea);
	printf(" wants to store contents of gpr %d (%08x) at %08x", rs, gpr[rs], ea);
	#endif
	memory_write16(ea, gpr[rs]);
}

void ii_sthu(u32 opcode) {
	u32 swopcode = swaponle_u32(opcode);
	u8 ra = (swopcode >> 16) & 0x1F, rs = (swopcode >> 21) & 0x1F;
	s16 d = swopcode & 0xFFFF;
	u32 b, ea;
	if(ra == 0) b = 0;
	else b = ((u32 *)gpr)[ra];
	ea = b + (s32)d;
	#ifdef II_PRINT
	printf("Sth, ra 0x%x, rs 0x%x, d 0x%04x, ea 0x%08x", ra, rs, d, ea);
	#endif
	memory_write16(ea, (u16)(gpr[rs]&0xFFFF));
	((u32 *)gpr)[ra] = ea;
}

void ii_stmw(u32 opcode) {
	u32 swopcode = swaponle_u32(opcode);
	u8 ra = (swopcode >> 16) & 0x1F, rs = (swopcode >> 21) & 0x1F, r;
	s16 d = swopcode & 0xFFFF;
	u32 b, ea;
	if(ra == 0) b = 0;
	else b = ((u32 *)gpr)[ra];
	ea = b + (s32)d;
	#ifdef II_PRINT
	printf("Stmw, ra 0x%x, rs 0x%x, d 0x%04x, ea 0x%08x", ra, rs, d, ea);
	#endif

	r = rs;
	while(1) {
		if(r > 31) break;
		memory_write32(ea, ((u32 *)gpr)[r]);
		r++;
		ea += 4;
	}
}

void ii_stw(u32 opcode) {
	u32 swopcode = swaponle_u32(opcode);
	u8 ra = (swopcode >> 16) & 0x1F, rs = (swopcode >> 21) & 0x1F;
	s16 d = swopcode & 0xFFFF;
	u32 b, ea;
	if(ra == 0) b = 0;
	else b = ((u32 *)gpr)[ra];
	ea = b + (s32)d;
	#ifdef II_PRINT
	printf("Stw, ra 0x%x, rs 0x%x, d 0x%04x, ea 0x%08x", ra, rs, d, ea);
	printf(" wants to store contents of gpr %d (%08x) at %08x", rs, gpr[rs], ea);
	#endif
	memory_write32(ea, gpr[rs]);
}

void ii_stwu(u32 opcode) {
	u32 swopcode = swaponle_u32(opcode);
	u8 ra = (swopcode >> 16) & 0x1F, rs = (swopcode >> 21) & 0x1F;
	s16 d = swopcode & 0xFFFF;
	u32 ea = ((u32 *)gpr)[ra] + d;
	#ifdef II_PRINT
	printf("Stwu, ra 0x%x, rs 0x%x, d 0x%04x, ea 0x%08x", ra, rs, d, ea);
	printf(" wants to store contents of gpr %d (%08x) at %08x and update gpr", rs, gpr[rs], ea);
	#endif
	memory_write32(ea, gpr[rs]);
	((u32 *)gpr)[ra] = ea;
}

void ii_stwx(u32 opcode) {
	u32 swopcode = swaponle_u32(opcode);
	u8 ra = (swopcode >> 16) & 0x1F, rs = (swopcode >> 21) & 0x1F, rb = (swopcode >> 11) & 0x1f;
	u32 b, ea;
	if(ra == 0) b = 0;
	else b = ((u32 *)gpr)[ra];
	ea = b + gpr[rb];
	#ifdef II_PRINT
	printf("Stwx, ra 0x%x, rb 0x%x, rs 0x%x, ea 0x%08x", ra, rb, rs, ea);
	printf(" wants to store contents of gpr %d (%08x) at %08x", rs, gpr[rs], ea);
	#endif
	memory_write32(ea, gpr[rs]);
}

void ii_subf(u32 opcode) {
	u32 swopcode = swaponle_u32(opcode);
	u8 rd = (swopcode >> 21) & 0x1F, ra = (swopcode >> 16) & 0x1F, rb = (swopcode >> 11) & 0x1f,
	oe = (swopcode >> 10) & 1, rc = swopcode & 1;
	
	gpr[rd] = (~gpr[ra]) + gpr[rb] + 1;

	if(rc) ii_cr0thing(((s32 *)gpr)[rd]);
	if(oe) {
		printf("Subf: Overflow bookkeeping not implemented\n"); exit(-500);
	}

	#ifdef II_PRINT
	printf("Subf, r%d %08x, r%d %08x, r%d %08x", rd, ((s32 *)gpr)[rd], ra, ((s32 *)gpr)[ra], rb, ((s32 *)gpr)[rb]);
	#endif
}

void ii_subfc(u32 opcode) {
	u32 swopcode = swaponle_u32(opcode);
	u8 rd = (swopcode >> 21) & 0x1F, ra = (swopcode >> 16) & 0x1F, rb = (swopcode >> 11) & 0x1f,
	oe = (swopcode >> 10) & 1, rc = swopcode & 1;
	
	gpr[rd] = (~gpr[ra]) + gpr[rb] + 1;

	if(rc) ii_cr0thing(((s32 *)gpr)[rd]);
	if(oe) {
		printf("Subfc: Overflow bookkeeping not implemented\n"); exit(-500);
	}

	#ifdef II_PRINT
	printf("Subfc, r%d %08x, r%d %08x, r%d %08x. We don't carry yet", rd, ((s32 *)gpr)[rd], ra, ((s32 *)gpr)[ra], rb, ((s32 *)gpr)[rb]);
	#endif
}

void ii_subfic(u32 opcode) {
	u32 swopcode = swaponle_u32(opcode);
	u8 rd = (swopcode >> 21) & 0x1F, ra = (swopcode >> 16) & 0x1F;
	s16 simm = swopcode & 0xFFFF;
	
	gpr[rd] = (~gpr[ra]) + simm + 1;

	#ifdef II_PRINT
	printf("Subfic, r%d %08x, r%d %08x. We don't carry yet", rd, ((s32 *)gpr)[rd], ra, ((s32 *)gpr)[ra]);
	#endif
}

void ii_sync(u32 opcode) {
	#ifdef II_PRINT
	printf("Sync, ");
	#endif
	/* We don't emulate cache */
}

void ii_xor(u32 opcode) {
	u32 swopcode = swaponle_u32(opcode);
	u8 ra = (swopcode >> 16) & 0x1F, rb = (swopcode >> 11) & 0x1f, 
	rs = (swopcode >> 21) & 0x1F, rc = swopcode & 1; 

	((u32 *)gpr)[ra] = ((u32 *)gpr)[rs] ^ ((u32 *)gpr)[rb];
	#ifdef II_PRINT
	printf("Xor, ra 0x%x, rb 0x%x rs 0x%x. r%d now %08x", ra, rb, rs, ra, ((u32 *)gpr)[ra]);
	#endif
	
	if(rc) ii_cr0thing(gpr[ra]);
}

void ii_xori(u32 opcode) {
	u32 swopcode = swaponle_u32(opcode);
	u8 ra = (swopcode >> 16) & 0x1F, rs = (swopcode >> 21) & 0x1F;
	u16 uimm = swopcode & 0xFFFF;
	
	((u32 *)gpr)[ra] = ((u32 *)gpr)[rs] ^ (u32)uimm;
	#ifdef II_PRINT
	printf("Xori, ra 0x%x, rs 0x%x, uimm 0x%08x. r%d now %08x", ra, rs, (u32)uimm, ra, ((u32 *)gpr)[ra]);
	#endif
}

void ii_xoris(u32 opcode) {
	u32 swopcode = swaponle_u32(opcode);
	u8 ra = (swopcode >> 16) & 0x1F, rs = (swopcode >> 21) & 0x1F;
	u16 uimm = swopcode & 0xFFFF;
	
	((u32 *)gpr)[ra] = ((u32 *)gpr)[rs] ^ (((u32)uimm) << 16);
	#ifdef II_PRINT
	printf("Xoris, ra 0x%x, rs 0x%x, uimm 0x%08x. r%d now %08x", ra, rs, (u32)uimm, ra, ((u32 *)gpr)[ra]);
	#endif
}


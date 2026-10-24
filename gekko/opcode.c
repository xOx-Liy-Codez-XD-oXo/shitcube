#include "../global/type.h"
#include "../global/endian.h"
#include "interpreter.h"
#include "interpreter_reg.h"
#include "instruction.h"
#include "memory.h"
#include "opcode.h"
#include <stdio.h>
#include <stdlib.h>

void opcode_executetableA(u32 opcode) {
	#ifdef INTERPRETER_PRINT
	printf("Execute table A ");
	printf("%04d ", (swaponle_u32(opcode) >> 1) & 0x03FF);
	#endif
	opcode_executetablea[(swaponle_u32(opcode) >> 1) & 0x03FF](opcode); /* Run opcode */
	return;
}

void opcode_executetableB(u32 opcode) {
	#ifdef INTERPRETER_PRINT
	printf("Execute table B ");
	printf("%04d ", (swaponle_u32(opcode) >> 1) & 0x03FF);
	#endif
	opcode_executetableb[(swaponle_u32(opcode) >> 1) & 0x03FF](opcode); /* Run opcode */
	return;
}

void opcode_executetableC(u32 opcode) {
	#ifdef INTERPRETER_PRINT
	printf("Execute table C ");
	#endif
	/*opcode_executetablec[(swaponle_u32(opcode) >> 1) & 0x03FF](opcode);*/ /* Run opcode */
	exit(0);
	return;
}

void opcode_executetableD(u32 opcode) {
	#ifdef INTERPRETER_PRINT
	printf("Execute table D ");
	#endif
	opcode_executetabled[(swaponle_u32(opcode) >> 1) & 0x03FF](opcode); /* Run opcode */
	return;
}

void opcode_execute(u32 opcode) {
	/* Opcode table for first 6 bit */
	static void (*table[64])(u32) = {
		ii_invalid, /* 000000 */
		ii_invalid, /* 000001 */
		ii_invalid, /* 000010 */
		ii_nop, /* 000011 twi */
		ii_ps_mr, /* 000100  */
		ii_invalid, /* 000101 */
		ii_invalid, /* 000110 */
		ii_mulli, /* 000111 mulli */
		ii_subfic, /* 001000 subfic */
		ii_invalid, /* 001001  */
		ii_cmpli, /* 001010 cmpli */
		ii_cmpi, /* 001011 cmpi */
		ii_addic, /* 001100 addic */
		ii_nop, /* 001101 addic. */
		ii_addi, /* 001110 addi */
		ii_addis, /* 001111 addis */
		ii_bc, /* 010000 bc */
		ii_sc, /* 010001 sc */
		ii_b, /* 010010 b */
		opcode_executetableA, /* 010011 mcrf, bclrx, crnor, rfi, crandc, isync,  */
		               /* crxor, crnand, crand, creqv, crorc, cror, bcctrx */
		ii_rlwimi, /* 010100 rlwimix */
		ii_rlwinm, /* 010101 rlwinm */
		ii_invalid, /* 010110  */
		ii_nop, /* 010111 rlwnmx */
		ii_ori, /* 011000 ori */
		ii_oris, /* 011001 oris */
		ii_xori, /* 011010 xori */
		ii_xoris, /* 011011 xoris */
		ii_andi, /* 011100 andi */
		ii_andis, /* 011101 andis */
		ii_invalid, /* 011110  */
		opcode_executetableB, /* 011111 cmp, tw, subfcx, addcx, mulhwux, mfcr, lwarx
                                  lwzx, slwx, cntlzwx, andx, cmpl, subfx, dcbst
                                  lwzux, andcx, mulhwx, mfmsr, dcbf, lbzx, negx
                                  lbuzx, norx, subfex, addex, mtcrf, mtmsr,
                                  stwcx, stwx, stwux, subfzex, addzex, mtsr,
                                  stbx, subfmex, addmex, mullwx, mtsrin, dcbtst
                                  stbux, addx, dcbt, lhzx, eqvx, tlbie, eciwx
                                  lhzux, xorx, mfspr, lhax, mftb, lhaux, sthx,
                                  orcx, ecowx, sthux, orx, divwux, mtspr, dcbi, 
                                  nandx, divwx, mcrxr, lswx, lwbrx, lfsx, srwx
                                  tlbsync, lfsux, mfsr, lswl, sync, lfdx, lfdux, 
                                  mfsrin, stswx, stwbrx, stfsx, stfsux, stswi, 
                                  stfdx, stfdux, lhbrx, srawx, srawix, eieio, 
                                  sthbrx, extshx, extsbx, icbi, stfiwx, dcbz */
		ii_lwz, /* 100000 lwz */
		ii_lwzu, /* 100001 lwzu */
		ii_lbz, /* 100010 lbz */
		ii_lbzu, /* 100011 lbzu */
		ii_stw, /* 100100 stw */
		ii_stwu, /* 100101 stwu */
		ii_stb, /* 100110 stb */
		ii_stbu, /* 100111 stbu */
		ii_lhz, /* 101000 lhz */
		ii_lhzu, /* 101001 lhzu */
		ii_lha, /* 101010 lha */
		ii_nop, /* 101011 lhau */
		ii_sth, /* 101100 sth */
		ii_sthu, /* 101101 sthu */
		ii_lmw, /* 101110 lmw */
		ii_stmw, /* 101111 stmw */
		ii_nop, /* 110000 lfs */
		ii_nop, /* 110001 lfsu */
		ii_lfd, /* 110010 lfd */
		ii_nop, /* 110011 lfdu */
		ii_nop, /* 110100 stfs */
		ii_nop, /* 110101 stfsu */
		ii_nop, /* 110110 stfd */
		ii_nop, /* 110111 stfdu */
		ii_psq_l, /* 111000 */ 
		ii_invalid, /* 111001 */
		ii_invalid, /* 111010 */
		opcode_executetableC, /* 111011 fdivsx, fsubsx, faddsx, fresx, fmulsx, 
                                                fmsubsx, fmaddsx, fnmsubsx, fnmaddsx */
		ii_invalid, /* 111100  */
		ii_invalid, /* 111101 */
		ii_invalid, /* 111110 */
		opcode_executetableD  /* 111111 fcmpu frspx fctiwx fctiwzx fdivx fsubx 
                                                faddx fselx fmulx fmsubx fmaddx fnmsubx 
                                                fnmaddx fcmpo mtfsb1x fnegx mcrfs mtfsb0x 
                                                fmrx mtfsfix fnabsx fabsx mffsx mtfsfx */
	};
	#ifdef BE
	table[(opcode >> 26) & 0x3f](opcode); /* Run opcode */
	#else
	table[(opcode >> 2) & 0x3f](opcode); /* Run opcode */
	#endif
}


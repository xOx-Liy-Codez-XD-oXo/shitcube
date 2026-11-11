#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include "../global/type.h"

void ii_invalid(u32 opcode, u32 swopcode);
void ii_nop(u32 opcode, u32 swopcode);

void ii_add(u32 opcode, u32 swopcode);
void ii_addc(u32 opcode, u32 swopcode);
void ii_adde(u32 opcode, u32 swopcode);
void ii_addi(u32 opcode, u32 swopcode);
void ii_addic(u32 opcode, u32 swopcode);
void ii_addis(u32 opcode, u32 swopcode);
void ii_addme(u32 opcode, u32 swopcode);
void ii_addze(u32 opcode, u32 swopcode);
void ii_and(u32 opcode, u32 swopcode);
void ii_andc(u32 opcode, u32 swopcode);
void ii_andi(u32 opcode, u32 swopcode);
void ii_andis(u32 opcode, u32 swopcode);
void ii_b(u32 opcode, u32 swopcode);
void ii_bc(u32 opcode, u32 swopcode);
void ii_bclr(u32 opcode, u32 swopcode);
void ii_bcctr(u32 opcode, u32 swopcode);
void ii_cmp(u32 opcode, u32 swopcode);
void ii_cmpi(u32 opcode, u32 swopcode);
void ii_cmpl(u32 opcode, u32 swopcode);
void ii_cmpli(u32 opcode, u32 swopcode);
void ii_cntlzw(u32 opcode, u32 swopcode);
void ii_dcbf(u32 opcode, u32 swopcode);
void ii_dcbi(u32 opcode, u32 swopcode);
void ii_divwu(u32 opcode, u32 swopcode);
void ii_eieio(u32 opcode, u32 swopcode);
void ii_extsb(u32 opcode, u32 swopcode);
void ii_extsh(u32 opcode, u32 swopcode);
void ii_isync(u32 opcode, u32 swopcode);
void ii_icbi(u32 opcode, u32 swopcode);
void ii_lbz(u32 opcode, u32 swopcode);
void ii_lbzu(u32 opcode, u32 swopcode);
void ii_lbzx(u32 opcode, u32 swopcode);
void ii_lha(u32 opcode, u32 swopcode);
void ii_lhz(u32 opcode, u32 swopcode);
void ii_lhzu(u32 opcode, u32 swopcode);
void ii_lhzx(u32 opcode, u32 swopcode);
void ii_lmw(u32 opcode, u32 swopcode);
void ii_lwz(u32 opcode, u32 swopcode);
void ii_lwzu(u32 opcode, u32 swopcode);
void ii_lwzx(u32 opcode, u32 swopcode);
void ii_mfcr(u32 opcode, u32 swopcode);
void ii_mfmsr(u32 opcode, u32 swopcode);
void ii_mfspr(u32 opcode, u32 swopcode);
void ii_mftb(u32 opcode, u32 swopcode);
void ii_mtcrf(u32 opcode, u32 swopcode);
void ii_mtmsr(u32 opcode, u32 swopcode);
void ii_mtsr(u32 opcode, u32 swopcode);
void ii_mtspr(u32 opcode, u32 swopcode);
void ii_mulli(u32 opcode, u32 swopcode);
void ii_mullw(u32 opcode, u32 swopcode);
void ii_mulhw(u32 opcode, u32 swopcode);
void ii_mulhwu(u32 opcode, u32 swopcode);
void ii_neg(u32 opcode, u32 swopcode);
void ii_nor(u32 opcode, u32 swopcode);
void ii_or(u32 opcode, u32 swopcode);
void ii_ori(u32 opcode, u32 swopcode);
void ii_oris(u32 opcode, u32 swopcode);
void ii_rfi(u32 opcode, u32 swopcode);
void ii_rlwimi(u32 opcode, u32 swopcode);
void ii_rlwinm(u32 opcode, u32 swopcode);
void ii_sc(u32 opcode, u32 swopcode);
void ii_slw(u32 opcode, u32 swopcode);
void ii_srw(u32 opcode, u32 swopcode);
void ii_sraw(u32 opcode, u32 swopcode);
void ii_srawi(u32 opcode, u32 swopcode);
void ii_sync(u32 opcode, u32 swopcode);
void ii_stb(u32 opcode, u32 swopcode);
void ii_stbu(u32 opcode, u32 swopcode);
void ii_stbx(u32 opcode, u32 swopcode);
void ii_sth(u32 opcode, u32 swopcode);
void ii_sthx(u32 opcode, u32 swopcode);
void ii_sthu(u32 opcode, u32 swopcode);
void ii_stmw(u32 opcode, u32 swopcode);
void ii_stw(u32 opcode, u32 swopcode);
void ii_stwu(u32 opcode, u32 swopcode);
void ii_stwx(u32 opcode, u32 swopcode);
void ii_subf(u32 opcode, u32 swopcode);
void ii_subfc(u32 opcode, u32 swopcode);
void ii_subfic(u32 opcode, u32 swopcode);
void ii_xor(u32 opcode, u32 swopcode);
void ii_xori(u32 opcode, u32 swopcode);
void ii_xoris(u32 opcode, u32 swopcode);

/* Float */

void ii_fneg(u32 opcode, u32 swopcode);
void ii_fnabs(u32 opcode, u32 swopcode);
void ii_fmr(u32 opcode, u32 swopcode);
void ii_lfd(u32 opcode, u32 swopcode);
void ii_mtfsb1(u32 opcode, u32 swopcode);
void ii_mtfsf(u32 opcode, u32 swopcode);

/* Ps float */

void ii_psq_l(u32 opcode, u32 swopcode);
void ii_ps_mr(u32 opcode, u32 swopcode);

#endif

/*
  Mace - http://www.macehq.cx
  Copyright 1999-2004
  See the file README for more information
*/


/***************************** 68000 SIMULATOR ****************************

File Name: PROTO.H
Version: 1.0

This file contains function prototype definitions for all functions
	in the program

***************************************************************************/



S32 MOVE(void );
S32 MOVEP(void );
S32 MOVEA(void );
S32 MOVE_FR_SR(void );
S32 MOVE_TO_CCR(void );
S32 MOVE_TO_SR(void );
S32 MOVEM(void );
S32 MOVE_USP(void );
S32 MOVEQ(void );
S32 EXG(void );
S32 LEA(void );
S32 PEA(void );
S32 LINK(void );
S32 UNLK(void );
S32 ADD(void );
S32 ADDA(void );
S32 ADDI(void );
S32 ADDQ(void );
S32 ADDX(void );
S32 SUB(void );
S32 SUBA(void );
S32 SUBI(void );
S32 SUBQ(void );
S32 SUBX(void );
S32 DIVS(void );
S32 DIVU(void );
S32 MULS(void );
S32 MULU(void );
S32 NEG(void );
S32 NEGX(void );
S32 CMP(void );
S32 CMPA(void );
S32 CMPI(void );
S32 CMPM(void );
S32 TST(void );
S32 CLR(void );
S32 EXT(void );
S32 ABCD(void );
S32 SBCD(void );
S32 NBCD(void );
S32 AND(void );
S32 ANDI(void );
S32 ANDI_TO_CCR(void );
S32 ANDI_TO_SR(void );
S32 OR(void );
S32 ORI(void );
S32 ORI_TO_CCR(void );
S32 ORI_TO_SR(void );
S32 EOR(void );
S32 EORI(void );
S32 EORI_TO_CCR(void );
S32 EORI_TO_SR(void );
S32 NOT(void );
S32 SHIFT_ROT(void );
S32 SWAP(void );
S32 BIT_OP(void );
S32 TAS(void );
S32 BCC(void );
S32 DBCC(void );
S32 SCC(void );
S32 BRA(void );
S32 BSR(void );
S32 JMP(void );
S32 JSR(void );
S32 RTE(void );
S32 RTR(void );
S32 RTS(void );
S32 NOP(void );
S32 CHK(void );
S32 ILLEGAL(void );
S32 RESET(void );
S32 STOP(void );
S32 TRAP(void );
S32 TRAPV(void );
S32 parse(char *str,char * *ptrbuf,S32 maxcnt);
S32 iswhite(char c,char *qflag);
S32 decode_size(S32 *result);
S32 eff_addr(S32 size,S32 mask,S32 add_times);
S32 runprog(void );
S32 exec_inst(void );
S32 exception(S32 class,S32 loc,S32 r_w);
S32 init(void );
S32 setdis(void );
S32 to_2s_comp(S32 number,S32 size,S32 *result);
S32 from_2s_comp(S32 number,S32 size,S32 *result);
S32 sign_extend(S32 number,S32 size_from,S32 *result);
S32 eff_addr_code(S32 inst,S32 start);
S32 a_reg(S32 reg_num);
S32 mem_put(S32 data,S32 loc,S32 size);
S32 mem_req(S32 loc,S32 size,S32 *result);
S32 mem_request(S32 *loc,S32 size,S32 *result);
S32 put(S32 *dest,S32 source,S32 size);
S32 value_of(S32 *EA,S32 *EV,S32 size);
S32 cc_update(S32 x,S32 n,S32 z,S32 v,S32 c,S32 source,S32 dest,S32 result,S32 size,S32 r);
S32 check_condition(S32 condition);


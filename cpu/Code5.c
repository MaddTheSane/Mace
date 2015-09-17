/*
  Mace - http://www.macehq.cx
  Copyright 1999-2004
  See the file README for more information
*/

/***************************** 68000 SIMULATOR ****************************

File Name: CODE5.C
Version: 1.0

The instructions implemented in this file are the binary coded decimal
   operations:

		ABCD, SBCD, NBCD


***************************************************************************/


#include <stdio.h>
#include "cpu.h"
#include "extern.h"         /* contains global "extern" declarations */


S32	ABCD()
{
S32	Rx, Ry, carry, temp_result;

Rx = (inst >> 9) & 0x0007;
Ry = inst & 0x0007;

/* perform the ABCD operation */
if (inst & 0x0008)  /* Rx & Ry are address registers used in predecrement */
	{	       		  /* mode */
	Rx = a_reg(Rx);
	Ry = a_reg(Ry);
	A[Rx]--;
	A[Ry]--;
	source = memory[A[Ry]];
	dest = memory[A[Rx]];
	}
else		/* Rx & Ry are data registers */
	{
	source = D[Ry] & BYTE;
	dest = D[Rx] & BYTE;
	}

/* perform the ABCD operation */
result = ((SR & xbit) >> 4) + (source & 0xf) + (dest & 0xf);
if (result > 9)
	{
	result = result - 10;
	carry = 1;
	}
else
	carry = 0;
temp_result = ((source >> 4) & 0xf) + ((dest >> 4) & 0xf) + carry;
if (temp_result > 9)
	{
	temp_result = temp_result - 10;
	carry = 1;
	}
else
	carry = 0;
result = result + (temp_result << 4);

if (inst & 0x0008)
	put ((S32 *)&memory[A[Rx]], result, (S32) BYTE);
else
	put (&D[Rx], result, (S32) BYTE);
if (carry)
	SR = SR | cbit;
else
	SR = SR & ~cbit;

cc_update (GEN, UND, CASE_1, UND, N_A, source, dest, result, (S32) BYTE, 0);

return SUCCESS;

}



S32	SBCD()
{
S32	Rx, Ry, borrow, temp_result;

Rx = (inst >> 9) & 0x0007;
Ry = inst & 0x0007;

/* perform the SUB operation */
if (inst & 0x0008)     /* Rx & Ry are address registers used in */
	{	       /* predecrement mode */
	Rx = a_reg(Rx);
	Ry = a_reg(Ry);
	A[Rx]--;
	A[Ry]--;
	source = memory[A[Ry]];
	dest = memory[A[Rx]];
	}
else
	{		/* Rx & Ry are data registers */
	source = D[Ry];
	dest = D[Rx];
	}

/* perform the SBCD operation */
result = (dest & 0xf) - (source & 0xf) - ((SR & xbit) >> 4);
if (result < 0)
	{
	result = result + 10;
	borrow = 1;
	}
else
	borrow = 0;
temp_result = ((dest >> 4) & 0xf) - ((source >> 4) & 0xf) - borrow;
if (temp_result < 0)
	{
	temp_result = temp_result + 10;
	borrow = 1;
	}
else
	borrow = 0;
result = result + (temp_result << 4);

if (inst & 0x0008)
	put ((S32 *)&memory[A[Rx]], result, (S32) BYTE);
else
	put (&D[Rx], result, (S32) BYTE);

if (borrow)
	SR = SR | cbit;
else
	SR = SR & ~cbit;

cc_update (GEN, UND, CASE_1, UND, N_A, source, dest, result, (S32) BYTE, 0);

return SUCCESS;

}



S32	NBCD()
{
S32	borrow, temp_result;

if (eff_addr ((S32) BYTE, DATA_ALT_ADDR, TRUE))
	return (BAD_INST);			/* illegal effective address */

dest = EV1 & BYTE;

/* perform the NBCD operation */
result = 10 - (dest & 0xf) - ((SR & xbit) >> 4);

if (result < 10)
	borrow = 1;
else
	borrow = 0;

temp_result = 10 - ((dest >> 4) & 0xf) - borrow;

if (temp_result < 10)
	borrow = 1;
else
	borrow = 0;

result = result + (temp_result << 4);

if (borrow)
	SR = SR | cbit;
else
	SR = SR & ~cbit;

put (EA1, result, (S32) BYTE);

cc_update (GEN, UND, CASE_1, UND, N_A, source, dest, result, (S32) BYTE, 0);

return SUCCESS;

}


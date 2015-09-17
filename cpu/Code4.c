/*
  Mace - http://www.macehq.cx
  Copyright 1999-2004
  See the file README for more information
*/

/***************************** 68000 SIMULATOR ****************************

File Name: CODE4.C
Version: 1.0

The instructions implemented in this file are the integer arithmetic
	operations:

		DIVS, DIVU, MULS, MULU, NEG, NEGX, CMP, CMPA, CMPI, CMPM,
		TST, CLR, EXT


***************************************************************************/


#include <stdio.h>
#include "cpu.h"
#include "extern.h"         /* contains global "extern" declarations */


S32	DIVS()
{
S32	reg, overflow;
S32	remainder;

if (eff_addr ((S32) WORD, DATA_ADDR, TRUE))
	return (BAD_INST);			/* bad instruction format */

reg = (inst >> 9) & 0x0007;

from_2s_comp (EV1 & WORD, (S32) WORD, &source);
from_2s_comp (D[reg], LONG, &dest);

if (source == 0)
	return (DIV_BY_ZERO);		/* initiate exception processing */

/* check for overflow */
if ( ( (source < 0) && (dest >= 0) ) || ( (source >= 0) && (dest < 0) ) )
	{
	if ((dest / -source) > WORD)
		overflow = TRUE;
	else
		overflow = FALSE;
	}
else
	{
	if ((dest / source) > WORD)
		overflow = TRUE;
	else
		overflow = FALSE;
	}

if (overflow)
	SR |= vbit;			/* if overflow then quit */
else
	{
	SR &= ~vbit;
	result = (dest / source) & 0xffff;
	remainder = (dest % source) & 0xffff;
	D[reg] = result = result | (remainder * 0x10000);
	cc_update (N_A, GEN, GEN, N_A, ZER, source, dest, result, (S32) WORD, 0);
	if ( ( (source < 0) && (dest >= 0) ) || ( (source >= 0) && (dest < 0) ) )
		SR |= nbit;
	else
		SR &= ~nbit;
	}

return SUCCESS;

}



S32	DIVU()
{
S32	reg;

if (eff_addr ((S32) WORD, DATA_ADDR, TRUE))
	return (BAD_INST);			/* bad instruction format */

reg = (inst >> 9) & 0x0007;

source = EV1 & WORD;
dest = D[reg];

if (source == 0)
	return (DIV_BY_ZERO);		/* initiate exception processing */

if ( (dest / source) > WORD)
	SR |= vbit; 		/* check for overflow */
else
	{
	SR &= ~vbit;
	D[reg] = result =((dest / source) & 0xffff) | ((dest % source) * 0x10000);

	cc_update (N_A, GEN, GEN, N_A, ZER, source, dest, result, (S32) WORD, 0);
	}

return SUCCESS;

}



S32	MULS()
{
S32	reg;

if (eff_addr ((S32) WORD, DATA_ADDR, TRUE))
	return (BAD_INST);			/* bad instruction format */

reg = (inst >> 9) & 0x0007;

from_2s_comp (EV1 & WORD, (S32) WORD, &source);
from_2s_comp (D[reg] & WORD, (S32) WORD, &dest);

D[reg] = result = source * dest;

cc_update (N_A, GEN, GEN, CASE_9, ZER, source, dest, result, (S32) WORD, 0);

return SUCCESS;

}



S32	MULU()
{
S32	reg;

if (eff_addr ((S32) WORD, DATA_ADDR, TRUE))
	return (BAD_INST);			/* bad instruction format */

reg = (inst >> 9) & 0x0007;

source = EV1 & WORD;
dest = D[reg] & WORD;

D[reg] = result = source * dest;

cc_update (N_A, GEN, GEN, ZER, ZER, source, dest, result, LONG, 0);

return SUCCESS;

}



S32	NEG()
{
S32	size;

if ((decode_size(&size)) ||
    (eff_addr (size, DATA_ALT_ADDR, TRUE)))
	return (BAD_INST);		/* bad instruction format */

source = dest = EV1 & size;

/* perform the NEG operation */
put (EA1, -dest, size);
value_of (EA1, &result, size);

cc_update (GEN, GEN, GEN, CASE_3, CASE_4, source, dest, result, size, 0);

return SUCCESS;

}



S32	NEGX()
{
S32	size;

if ((decode_size(&size)) ||
    (eff_addr (size, DATA_ALT_ADDR, TRUE)))
	return (BAD_INST);		/* bad instruction format */

dest = dest = EV1 & size;

/* perform the NEGX operation */
put (EA1, -dest - ((SR & xbit) >> 4), size);
value_of (EA1, &result, size);

cc_update (GEN, GEN, CASE_1, CASE_3, CASE_4, source, dest, result, size, 0);

return SUCCESS;

}



S32	CMP()
{
S32	size;
S32	reg;

if ((decode_size(&size)) ||
    (eff_addr (size, ALL_ADDR, TRUE)))
	return (BAD_INST);			/* bad instruction format */

reg = (inst >> 9) & 0x0007;

source = EV1 & size;
dest = D[reg] & size;

put (&result, dest - source, size);

/* now set the condition codes */
cc_update (N_A, GEN, GEN, CASE_2, CASE_6, source, dest, result, size, 0);

return SUCCESS;

}


S32	CMPA()
{
S32	size;
S32	reg;

if (inst & 0x0100)
	size = LONG;
else
	size = WORD;

if (eff_addr (size, ALL_ADDR, TRUE))
	return (BAD_INST);			/* bad instruction format */

reg = a_reg((inst >> 9) & 0x0007);

if (size == WORD)
	sign_extend ((S32)EV1, WORD, &EV1);

source = EV1 & size;
dest = A[reg] & size;

put (&result, dest - source, size);

/* now set the condition codes according to the result */
cc_update (N_A, GEN, GEN, GEN, GEN, source, dest, result, size, 0);

return SUCCESS;

}


S32	CMPI()
{
S32	size;

if (decode_size(&size))
	return (BAD_INST);			/* bad instruction format */

mem_request (&PC, size, &source);

if (eff_addr (size, DATA_ALT_ADDR, TRUE))
	return (BAD_INST);			/* bad instruction format */

dest = EV1 & size;

put (&result, dest - source, size);

cc_update (N_A, GEN, GEN, CASE_2, CASE_6, source, dest, result, size, 0);

return SUCCESS;

}



S32	CMPM()
{
S32	size;
S32	Rx, Ry;

if (decode_size(&size))
	return (BAD_INST);

Rx = a_reg((inst >> 9) & 0x07);
Ry = a_reg(inst & 0x07);

mem_req ( (S32) A[Ry], size, &source);
mem_req ( (S32) A[Rx], size, &dest);

put (&result, dest - source, size);

if (size == BYTE)  {
	A[Rx]++;
	A[Ry]++;
	};
if (size == WORD)  {
	A[Rx] += 2;
	A[Ry] += 2;
	};
if (size == LONG)  {
	A[Rx] += 4;
	A[Ry] += 4;
	};

/* now set the condition codes according to the result */
cc_update (N_A, GEN, GEN, CASE_2, CASE_6, source, dest, result, size, 0);

return SUCCESS;

}




S32	TST()
{
S32	size;

if ((decode_size(&size)) ||
    (eff_addr (size, DATA_ALT_ADDR, TRUE)))
	return (BAD_INST);		/* bad instruction format */

value_of (EA1, &dest, size);

/* test the dest operand and set the condition codes accordingly */
cc_update (N_A, GEN, GEN, ZER, ZER, source, dest, dest, size, 0);

return SUCCESS;

}



S32	CLR()
{
S32	size;

if ((decode_size(&size)) ||
    (eff_addr (size, DATA_ALT_ADDR, TRUE)))
	return (BAD_INST);		/* bad instruction format */

source = dest = EV1 & size;

/* perform the CLR operation */
put (EA1, (S32) 0, size);
value_of (EA1, &result, size);

cc_update (N_A, ZER, GEN, ZER, ZER, source, dest, result, size, 0);

return SUCCESS;

}




S32	EXT()
{
S32	size;
S32	reg;

reg = inst & 0x07;

if (inst & 0x0040)
	size = LONG;
else
	size = WORD;

source = dest = D[reg] & size;

if (size == WORD)
 D[reg] = (D[reg]&~WORD) | (D[reg] & BYTE) | (0xff00 * ((D[reg]>>7) & 0x01));
else
 D[reg] = (D[reg] & WORD) | (0xffff0000 * ((D[reg] >> 15) & 0x01));

cc_update (N_A, GEN, GEN, ZER, ZER, source, dest, D[reg], size, 0);

return SUCCESS;

}


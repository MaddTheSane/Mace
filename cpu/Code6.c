/*
  Mace - http://www.macehq.cx
  Copyright 1999-2004
  See the file README for more information
*/

/***************************** 68000 SIMULATOR ****************************

File Name: CODE6.C
Version: 1.0

The instructions implemented in this file are the logical arithmetic
   operations:

		AND, ANDI, ANDI_TO_CCR, ANDI_TO_SR, OR, ORI, ORI_TO_CCR,
		ORI_TO_SR, EOR, EORI, EORI_TO_CCR, EORI_TO_SR, NOT


***************************************************************************/


#include <stdio.h>
#include "cpu.h"
#include "extern.h"         /* contains global "extern" declarations */

S32	AND()
{
S32	addr_modes_mask, reg;
S32	size;

addr_modes_mask = (inst & 0x0100) ? MEM_ALT_ADDR : DATA_ADDR;

if ((decode_size(&size)) ||
    (eff_addr (size, addr_modes_mask, TRUE)))
	return (BAD_INST);			/* bad instruction format */

reg = (inst >> 9) & 0x0007;

if (inst & 0x0100)
	{
	source = D[reg] & size;
	dest = EV1 & size;
	put (EA1, dest & source, size);
	value_of (EA1, &result, size);
	}
else
	{
	source = EV1 & size;
	dest = D[reg] & size;
	put (&D[reg], dest & source, size);
	result = D[reg] & size;
	}

cc_update (N_A, GEN, GEN, ZER, ZER, source, dest, result, size, 0);

return SUCCESS;

}



S32	ANDI()
{
S32	size;

if (decode_size(&size))
	return (BAD_INST);			/* bad instruction format */

mem_request (&PC, size, &source);

if (eff_addr (size, DATA_ALT_ADDR, TRUE))
	return (BAD_INST);			/* bad instruction format */

dest = EV1 & size;

put (EA1, source & dest, size);
value_of (EA1, &result, size);

cc_update (N_A, GEN, GEN, ZER, ZER, source, dest, result, size, 0);

return SUCCESS;

}


S32	ANDI_TO_CCR()
{
S32	temp;

mem_request (&PC, (S32) WORD, &temp);

SR &= temp | 0xff00;

return SUCCESS;

}


S32	ANDI_TO_SR()
{
S32	temp;

if (!(SR & sbit))
	return (NO_PRIVILEGE);

mem_request (&PC, (S32) WORD, &temp);
SR &= temp;

return SUCCESS;

}




S32	OR()
{
S32	size;
S32	mask, reg;

mask = (inst & 0x0100) ? MEM_ALT_ADDR : DATA_ADDR;

if ((decode_size(&size)) ||
    (eff_addr(size, mask, TRUE)))
	return (BAD_INST);		/* bad instruction format */

reg = (inst >> 9) & 0x0007;

if (inst & 0x0100)
	{
	source = D[reg] & size;
	dest = EV1 & size;
	put (EA1, source | dest, size);
	value_of (EA1, &result, size);
	}
else
	{
	source = EV1 & size;
	dest = D[reg] & size;
	put (&D[reg], source | dest, size);
	result = D[reg] & size;
	}

cc_update (N_A, GEN, GEN, ZER, ZER, source, dest, result, size, 0);

return SUCCESS;

}




S32	ORI()
{
S32	size;

if (decode_size(&size))
	return (BAD_INST);			/* bad instruction format */

mem_request (&PC, size, &source);

if (eff_addr (size, DATA_ALT_ADDR, TRUE))
	return (BAD_INST);			/* bad instruction format */

dest = EV1 & size;

put (EA1, source | dest, size);
value_of (EA1, &result, size);

cc_update (N_A, GEN, GEN, ZER, ZER, source, dest, result, size, 0);

return SUCCESS;

}


S32	ORI_TO_CCR()
{
S32	temp;

mem_request (&PC, (S32) WORD, &temp);

SR |= temp;

return SUCCESS;

}


S32	ORI_TO_SR()
{
S32	temp;

if (!(SR & sbit))
	return (NO_PRIVILEGE);

mem_request (&PC, (S32) WORD, &temp);
SR |= temp;

return SUCCESS;

}



S32	EOR()
{
S32	size;
S32	reg;

if ((decode_size(&size)) ||
    (eff_addr (size, DATA_ALT_ADDR, TRUE)))
	return (BAD_INST);			/* bad instruction format */

reg = (inst >> 9) & 0x0007;

source = D[reg] & size;
dest = EV1 & size;

put (EA1, EV1 ^ D[reg], size);
value_of (EA1, &result, size);

cc_update (N_A, GEN, GEN, ZER, ZER, source, dest, result, size, 0);

return SUCCESS;

}



S32	EORI()
{
S32	size;
S32	data;

if (decode_size(&size))
	return (BAD_INST);			/* bad instruction format */

mem_request (&PC, size, &source);

if (eff_addr (size, DATA_ALT_ADDR, TRUE))
	return (BAD_INST);			/* bad instruction format */

dest = EV1 & size;

put (EA1, source ^ dest, size);
value_of (EA1, &result, size);

cc_update (N_A, GEN, GEN, ZER, ZER, source, dest, result, size, 0);

return SUCCESS;

}


S32	EORI_TO_CCR()
{
S32	temp;

mem_request (&PC, (S32) WORD, &temp);

SR ^= temp;

return SUCCESS;

}


S32	EORI_TO_SR()
{
S32	temp;

if (!(SR & sbit))
	return (NO_PRIVILEGE);

mem_request (&PC, (S32) WORD, &temp);
SR ^= temp;

return SUCCESS;

}


S32	NOT()
{
S32	size;

if ((decode_size(&size)) ||
    (eff_addr (size, DATA_ALT_ADDR, TRUE)))
	return (BAD_INST);		/* bad instruction format */

source = dest = EV1 & size;

/* perform the NOT operation */
put (EA1, ~dest, size);
value_of (EA1, &result, size);

cc_update (N_A, GEN, GEN, ZER, ZER, source, dest, result, size, 0);

return SUCCESS;

}


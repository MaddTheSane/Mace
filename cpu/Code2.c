/*
  Mace - http://www.macehq.cx
  Copyright 1999-2004
  See the file README for more information
*/

/***************************** 68000 SIMULATOR ****************************

File Name: CODE2.C
Version: 1.0

The instructions implemented in this file are the data movement instructions
   other than the MOVE instructions:

		EXG, LEA, PEA, LINK, UNLK


***************************************************************************/


#include <stdio.h>
#include "cpu.h"
#include "extern.h"         /* contains global "extern" declarations */


S32	EXG()
{
S32	temp_reg;
S32	Rx, Ry;

Rx = (inst >> 9) & 0x07;
Ry = inst & 0x07;
switch ((inst >> 3) & 0x1f)
	{
	case 0x08: temp_reg = D[Rx];
		D[Rx] = D[Ry];
		D[Ry] = temp_reg;
		break;
	case 0x09:	temp_reg = A[a_reg(Rx)];
		A[a_reg(Rx)] = A[a_reg(Ry)];
		A[a_reg(Ry)] = temp_reg;
		break;
	case 0x11: temp_reg = D[Rx];
		D[Rx] = A[a_reg(Ry)];
		A[a_reg(Ry)] = temp_reg;
		break;
	default  : return (BAD_INST);	/* bad op_mode field */
              break;
	}

return SUCCESS;

}




S32	LEA()
{
S32	reg;

reg = (inst >> 9) & 0x07;

if (eff_addr (LONG, CONTROL_ADDR, FALSE))
	return (BAD_INST);		/* bad instruction format */

/* perform the LEA operation */
A[a_reg(reg)] = (S32) ((S32)EA1 - (S32)&memory[0]);

return SUCCESS;

}




S32	PEA()
{

if (eff_addr (LONG, CONTROL_ADDR, FALSE))
	return (BAD_INST);		/* bad instruction format */

/* push the S32word address computed by the */
/* effective address routine onto the stack */

A[a_reg(7)] -= 4;
put ((S32 *)&memory[A[a_reg(7)]], (S32) ((S32)EA1 - (S32)&memory[0]), LONG);

return SUCCESS;

}




S32	LINK()
{
S32	reg;
S32	temp, displacement;

reg = inst & 0x07;

mem_request (&PC, (S32) WORD, &temp);
from_2s_comp(temp, (S32) WORD, &displacement);

/* perform the LINK operation */
A[a_reg(7)] -= 4;
put ((S32 *)&memory[A[a_reg(7)]], A[reg], LONG);
A[reg] = A[a_reg(7)];
A[a_reg(7)] = A[a_reg(7)] + displacement;

return SUCCESS;

}



S32	UNLK()
{
S32	reg;

reg = inst & 0x07;

A[a_reg(7)] = A[reg];
mem_req ( (S32) A[a_reg(7)], LONG, &A[reg]);
A[a_reg(7)] += 4;

return SUCCESS;

}


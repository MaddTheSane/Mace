/*
  Mace - http://www.macehq.cx
  Copyright 1999-2004
  See the file README for more information
*/

/***************************** 68000 SIMULATOR ****************************

File Name: CODE8.C
Version: 1.0

The instructions implemented in this file are the program control operations:

		BCC, DBCC, SCC, BRA, BSR, JMP, JSR, RTE, RTR, RTS, NOP


***************************************************************************/


#include <stdio.h>
#include "cpu.h"
#include "extern.h"         /* contains global "extern" declarations */


S32	BCC()
{
S32	displacement;
S32	condition;

displacement = inst & 0xff;
if (displacement == 0)
	{
	mem_request (&PC, (S32) WORD, &displacement);
	from_2s_comp (displacement, (S32) WORD, &displacement);
	}
else
	from_2s_comp (displacement, (S32) BYTE, &displacement);

condition = (inst >> 8) & 0x0f;

/* perform the BCC operation */
if (check_condition (condition))
	PC = OLD_PC + displacement + 2;
			/* displacement is relative to the end of the instructin word */

return SUCCESS;

}




S32	DBCC()
{
S32	displacement;
S32	reg;

mem_request(&PC, (S32) WORD, &displacement);
from_2s_comp (displacement, (S32) WORD, &displacement);
reg = inst & 0x07;

/* perform the DBCC operation */
if (check_condition ((inst >> 8) & 0x0f))
    ; //NOP
else
	{
	D[reg] = (D[reg] & ~WORD) | ((short)((D[reg] & 0xffff) - 1));
	if ((D[reg] & 0xffff) == -1)
		; //NOP
	else
		{
		PC = OLD_PC + displacement + 2;
			/* displacement is relative to the end of the instructin word */
		}
	}

return SUCCESS;

}


S32	SCC()
{
S32	condition;

if (eff_addr ((S32) BYTE, DATA_ALT_ADDR, TRUE))
	return (BAD_INST);		/* bad instruction format */

/* perform the SCC operation */
condition = (inst >> 8) & 0x0f;
if (check_condition (condition))
	put (EA1, (S32) BYTE, (S32) BYTE);
else
	put (EA1, (S32) 0, (S32) BYTE);

return SUCCESS;

}



S32	BRA()
{
S32	displacement;

displacement = inst & 0xff;
if (displacement == 0)
	{
	mem_request (&PC, (S32) WORD, &displacement);
	from_2s_comp (displacement, (S32) WORD, &displacement);
	}
else
	from_2s_comp (displacement, (S32) BYTE, &displacement);

/* perform the BRA operation */
PC = OLD_PC + displacement + 2;
			/* displacement is relative to the end of the instructin word */

return SUCCESS;

}



S32	BSR()
{
S32	displacement;

displacement = inst & 0xff;
if (displacement == 0)
	{
	mem_request (&PC, (S32) WORD, &displacement);
	from_2s_comp (displacement, (S32) WORD, &displacement);
	}
else
	from_2s_comp (displacement, (S32) BYTE, &displacement);

/* perform the BSR operation */
A[a_reg(7)] -= 4;
put ((S32 *)&memory[A[a_reg(7)]], PC, LONG);

PC = OLD_PC + displacement + 2;
			/* displacement is relative to the end of the instructin word */

return SUCCESS;

}



S32	JMP()
{

if (eff_addr ((S32) WORD, CONTROL_ADDR, FALSE))
	return (BAD_INST);		/* bad instruction format */

/* perform the JMP operation */
PC = (S32) ((S32)EA1 - (S32)&memory[0]);

//BUG: It looks like it's jumping 1 byte too far sometimes
if((PC % 2) != 0) //If PC is not even
    --PC; //Fix the bug

return SUCCESS;

}





S32	JSR()
{

if (eff_addr ((S32) WORD, CONTROL_ADDR, FALSE))
	return (BAD_INST);		/* bad instruction format */

/* push the S32word address immediately following PC on the system stack */
/* then change the PC */
A[a_reg(7)] -= 4;
put ((S32 *)&memory[A[a_reg(7)]], PC, LONG);
PC = (S32) ((S32)EA1 - (S32)&memory[0]);

//BUG: It looks like it's jumping 1 byte too far sometimes
if((PC % 2) != 0) //If PC is not even
    --PC; //Fix the bug

return SUCCESS;

}


S32	RTE()
{
S32	temp;

if (!(SR & sbit))
	return (NO_PRIVILEGE);

mem_request (&A[8], (S32) WORD, &temp);
SR = temp & WORD;
mem_request (&A[8], LONG, &PC);

return SUCCESS;

}



S32	RTR()
{
S32	temp;

mem_request (&A[a_reg(7)], (S32) BYTE, &temp);
SR = (SR & 0xff00) | (temp & 0xff);

mem_request (&A[a_reg(7)], LONG, &PC);

return SUCCESS;

}



S32	RTS()
{

mem_request (&A[a_reg(7)], LONG, &PC);

return SUCCESS;

}



S32	NOP()
{

return SUCCESS;

}


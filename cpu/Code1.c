/*
  Mace - http://www.macehq.cx
  Copyright 1999-2004
  See the file README for more information
*/

/***************************** 68000 SIMULATOR ****************************

File Name: CODE1.C
Version: 1.0

The instructions implemented in this file are the MOVE instructions:

   MOVE, MOVEP, MOVEA, MOVE_FR_SR, MOVE_TO_CCR, MOVE_TO_SR, MOVEM,
   MOVE_USP, MOVEQ


***************************************************************************/



#include <stdio.h>
#include "cpu.h"
#include "extern.h"         /* contains global "extern" declarations */


S32	MOVE()
{
S32	size;				/* 'size' holds the instruction size */
S32	src, dst;		/* 'src' and 'dst' hold the addressing mode codes */
							/* for instruction execution time computation */

///* shouldn't need this once GetResource() is fixed
    if(inst==0x2F10) //move.l (a0),-(sp)
    {
        //Cheap patch because otherwize this opcode crashes the emulator for some reason
        //Take a LONG from address pointed to by A0 and put it on the stack

        U32 Temp;

        mem_req(A[0],LONG,&Temp);
        A[8]-=4;
        mem_put(Temp,A[8],LONG);

        return SUCCESS;
    }
//*/

/* MOVE has a different format for size field than all other instructions */
/* so we can't use the 'decode_size' function */
switch ( (inst >> 12) & 0x03)
	{
	case 0x01 : size = BYTE;				/* bit pattern '01' */
		         break;
	case 0x03 : size = WORD;				/* bit pattern '10' */
		         break;
	case 0x02 : size = LONG;				/* bit pattern '11' */
		         break;
	default   : return (BAD_INST);		/* bad size field */
               break;
	}

/* the following gets the effective addresses for the source and destination
	operands */
if ( (eff_addr (size, ALL_ADDR, FALSE)) ||
     (eff_addr (size, DATA_ALT_ADDR, FALSE)) )
{
//DEBUG - Might have to fix something here
    Port_Puts(PUTS_DEBUG,"MOVE (cpu/code1.c)","Illegal instruction (Bad address format)");

	return (BAD_INST);   /* if bad address format then return */
								/* the code for 'bad instruction' */
}

dest = EV2;								/* set 'dest' for use in 'cc_update' */

put (EA2, EV1, size);				/* perform the move to '*EA2' */
value_of (EA2, &EV2, size);		/* set 'EV2' for use in 'cc_update' */

src = eff_addr_code(inst,0);		/* get the addressing mode codes */
dst = eff_addr_code(inst,6);

/* now update the condition codes */
cc_update (N_A, GEN, GEN, ZER, ZER, EV1, dest, EV2, size, 0);

/* return the value for 'success' */
return SUCCESS;

}



S32	MOVEP()
{
S32	address, Dx, disp, count, direction, reg;
S32	temp, size;

mem_request (&PC, (S32) WORD, &temp);
from_2s_comp (temp, (S32) WORD, &temp);

address = A[a_reg(inst&0x07)] + (temp & WORD);

direction = inst & 0x80;

if (inst & 0x40)
	{
	size = LONG;
	count = 4;
	}
else
	{
	size = WORD;
	count = 2;
	}

reg = (inst >> 9) & 0x07;
Dx = D[reg] & size;

for (;count > 0; count--)
	{
	disp = 8 * (count - 1);
	if (direction)
		mem_put ( (S32)((Dx >> disp) & BYTE) , address, (S32) BYTE);
	else
		{
		mem_req (address, (S32) BYTE, &temp);
		switch  (count) {
			case 4 :	D[reg] = (D[reg] & 0x00ffffff) | (temp * 0x1000000);
						break;
			case 3 :	D[reg] = (D[reg] & 0xff00ffff) | (temp * 0x10000);
						break;
			case 2 :	D[reg] = (D[reg] & 0xffff00ff) | (temp * 0x100);
						break;
			case 1 : D[reg] = (D[reg] & 0xffffff00) | (temp);
						break;
			}
		}
	address += 2;
	}

return SUCCESS;

}



S32	MOVEA()
{
S32	size;

if (inst & 0x1000)
	size = WORD;
else
	size = LONG;

if (eff_addr(size, ALL_ADDR, FALSE))
	return (BAD_INST);

if (size == WORD)
	sign_extend ((S32)EV1, (S32) WORD, &EV1);

A[a_reg((inst >> 9) & 0x07)] = EV1;

return SUCCESS;

}



S32	MOVE_FR_SR()
{

if (eff_addr ((S32) WORD, DATA_ALT_ADDR, TRUE))
	return (BAD_INST);

put (EA1, (S32) SR, (S32) WORD);

return SUCCESS;

}



S32	MOVE_TO_CCR()
{

if (eff_addr ((S32) WORD, DATA_ADDR, TRUE))
	return (BAD_INST);

put ((S32 *)&SR, EV1, (S32) BYTE);

return SUCCESS;

}



S32	MOVE_TO_SR()
{

if (! (SR & sbit))
	return (NO_PRIVILEGE);

if (eff_addr ((S32) WORD, DATA_ADDR, TRUE))
	return (BAD_INST);

put ((S32 *)&SR, EV1, (S32) WORD);

return SUCCESS;

}



S32	MOVEM()
{
S32	direction, addr_modes_mask, counter, addr_mode;
S32	displacement, address, total_displacement;
S32	size, mask_list, temp;

mem_request (&PC, (S32) WORD, &mask_list);

if (inst & 0x0040)
	size = LONG;
else
	size = WORD;

if (direction = (inst & 0x0400))
	addr_modes_mask = CONTROL_ADDR | bit_4;
else
	addr_modes_mask = CONT_ALT_ADDR | bit_5;

if (eff_addr (size, addr_modes_mask, FALSE))
	return (BAD_INST);			/* bad effective address */

address = (S32) ( (S32)EA1 - (S32)&memory[0]);
total_displacement = address;

if ((inst & 0x0038) != 0x20)
	{
	if (size == WORD)
		displacement = 2;
	else
		displacement = 4;
	}
else
	{
	if (size == WORD)
		displacement = -2;
	else
		displacement = -4;
	}

addr_mode = eff_addr_code (inst,0);

for (counter = 0; counter < 16; counter++)
	if (mask_list & (1 << counter))
		{
		if (direction)
			{
			if (size == WORD)		/* if size is WORD then sign-extend */
				{
				mem_req (address, (S32) WORD, &temp);
				sign_extend ((S32) temp, LONG, &temp);
				}
			if (counter < 8)
				D[counter] = temp;
			else
				A[a_reg(counter - 8)] = temp;
			}
		else
			{
			if ((inst & 0x38) == 0x20)
				{
				if (counter < 8)
					mem_put (A[a_reg(7 - counter)], address, size);
				else
					mem_put (D[15 - counter], address, size);
				}
			else
				{
				if (counter < 8)
					mem_put (D[counter], address, size);
				else
					mem_put (A[a_reg(counter - 8)], address, size);
				}
			}
		address += displacement;
		}
address -= displacement;
total_displacement = address - total_displacement;

/* if pre-decrement or post-increment modes then change the value */
/* of the address register appropriately */
if ( ((inst & 0x38) == 0x20) || ( (inst & 0x38) == 0x18) )
	A[a_reg(inst & 0x7)] += total_displacement;

return SUCCESS;

}



S32	MOVE_USP()
{
S32	reg;

if (!(SR & sbit))
	return (NO_PRIVILEGE);		/* supervisor state not on */

reg = inst & 0x7;
if (reg == 7)
	reg = 8;

if (inst & 0x8)
	A[reg] = A[7];
else
	A[7] = A[reg];

return SUCCESS;

}



S32	MOVEQ()
{
S32	reg;

reg = (inst >> 9) & 0x7;
source = inst & 0xff;
dest = D[reg];

/* the data register is sign extended to a longword */
sign_extend ((S32)source, (S32) BYTE, &D[reg]);
sign_extend ((S32)D[reg], (S32) WORD, &D[reg]);

cc_update (N_A, GEN, GEN, ZER, ZER, source, dest, D[reg], LONG, 0);
return SUCCESS;

}


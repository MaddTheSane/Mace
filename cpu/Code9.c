/*
  Mace - http://www.macehq.cx
  Copyright 1999-2004
  See the file README for more information
*/

/***************************** 68000 SIMULATOR ****************************

File Name: CODE9.C
Version: 1.0

The instructions implemented in this file are the exception processing
related operations:

        CHK, ILLEGAL, RESET, STOP, TRAP, TRAPV


***************************************************************************/


#include <stdio.h>
#include "cpu.h"
#include "extern.h"         /* contains global "extern" declarations */

S32	CHK()
{
S32	reg;
S32	temp;

reg = (inst >> 9) & 0x07;

if (eff_addr ((S32) WORD, DATA_ADDR, TRUE))
	return (BAD_INST);		/* bad instruction format */

from_2s_comp (EV1, (S32) WORD, &source);
dest = D[reg] & WORD;

cc_update (N_A, GEN, UND, UND, UND, source, D[reg], D[reg], WORD, 0);

/* perform the CHK operation */
if ((dest < 0) || (dest > source))
	return(CHK_EXCEPTION);

return SUCCESS;

}



S32	ILLEGAL()
{

return (ILLEGAL_TRAP);

}



S32	RESET()
{

if (!(SR & sbit))
	return (NO_PRIVILEGE);

/* assert the reset line to reset external devices */

return SUCCESS;

}



S32	STOP()
{
S32	temp;
S32	tr_on;

mem_request (&PC, (S32) WORD, &temp);

if (SR & tbit)
	tr_on = TRUE;
else
	tr_on = FALSE;

if (!(SR & sbit))
	return (NO_PRIVILEGE);

SR = temp & WORD;
if (tr_on)
	SR = SR | tbit;

if (!(SR & sbit))
	return (NO_PRIVILEGE);

if (!(SR & tbit))
	Port_Puts (PUTS_INFO,"STOP (cpu/code9.c)","Processor has entered stop mode.  Processing halted.");
else
{
	Port_Puts (PUTS_DEBUG,"STOP (cpu/code9.c)","Processor has entered stop mode.");
	Port_Puts (PUTS_DEBUG,"STOP (cpu/code9.c)","Processing halted, but resumed due to a trace exception.");
}

return (STOP_TRAP);

}



S32	TRAP()
{

return (TRAP_TRAP);

}




S32	TRAPV()
{

if (SR & vbit)
	return (TRAPV_TRAP);

return SUCCESS;

}


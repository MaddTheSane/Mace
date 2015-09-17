/*
  Mace - http://www.macehq.cx
  Copyright 1999-2004
  See the file README for more information
*/

/***************************** 68000 SIMULATOR ****************************

File Name: RUN.C
Version: 1.0

This file contains various routines to run a 68000 instruction.  These
routines are :

	decode_size(), eff_addr(), runprog(), exec_inst(), exception()

***************************************************************************/



#include <stdio.h>
#include "cpu.h"
#include "extern.h"         					/* global declarations */
#include "opcodes.h"        					/* opcode masks for decoding */
												/* the 68000 instructions */



/**************************** S32 decode_size() ****************************

   name       : S32 decode_size (result)
   parameters : S32 *result : the appropriate mask for the decoded size
   function   : decodes the size field in the instruction being processed
                  and returns a mask to be used in instruction execution.
                  For example, if the size field was "01" then the mask
                  returned is WORD.

****************************************************************************/


S32	decode_size (result)
S32	*result;
{
	S32	bits;

	/* the size field is always in bits 6 and 7 of the instruction word */
	bits = (inst >> 6) & 0x0003;

	switch (bits) {
		case 0 : *result = BYTE;
			 break;
		case 1 : *result = WORD;
			 break;
		case 2 : *result = LONG;
			 break;
		default : *result = 0;
		}

	if (result != 0)
	   return SUCCESS;
	else
	   return FAILURE;

}



/**************************** S32 eff_addr() *******************************

   name       : S32 eff_addr (size, mask, add_times)
   parameters : S32 size : the appropriate size mask
                S32 mask : the effective address modes mask to be used
                S32 add_times : tells whether to increment the cycle counter
                      (there are times when we don't want to)
   function   : eff_addr() decodes the effective address field in the current
                  instruction, returns a poS32er to the effective address
                  either in EA1 or EA2, and returns the value at that
                  location in EV1 or EV2.

****************************************************************************/


S32 eff_addr (size, mask, add_times)
S32	size;
S32	mask;
S32	add_times;
{
	S32	mode, reg, legal, addr, move_operation;
	S32	bwinc, linc;
	S32	ext, temp_ext, inc_size, ind_reg, *value, disp;

	if (
			(
				((inst & 0xf000) == 0x1000)
				|| ((inst & 0xf000) == 0x2000)
				||	((inst & 0xf000) == 0x3000)
			)
			&& (mask == DATA_ALT_ADDR)
		)
		move_operation = TRUE;
	else
		move_operation = FALSE;

	if (move_operation)
		addr = (inst >> 6) & 0x003f;
	else
		addr = inst & 0x003f;
	legal = FALSE;
	bwinc = linc = 0;
   if (move_operation)		/* reg and mode are reversed in MOVE dest EA */
		{
		reg = (addr & MODE_MASK) >> 3;
		mode = addr & REG_MASK;
		}
	else
		{
		mode = (addr & MODE_MASK) >> 3;
		reg = addr & REG_MASK;
		}
	switch (mode) {
	   case	0 : if (mask & bit_1)
			{
			value = &D[reg];
			bwinc = linc = 0;
			legal = TRUE;
			}
		    break;
	   case	1 : if (mask & bit_2)
			{
			reg = a_reg(reg);
			value = &A[reg];
			bwinc = linc = 0;
			legal = TRUE;
			}
		    break;
	   case	2 : if (mask & bit_3)
			{
			reg = a_reg(reg);
			value = (S32*) &memory[A[reg]];
			bwinc = 4;
			linc = 8;
			legal = TRUE;
			}
		    break;
	   case	3 : if (mask & bit_4)
			{
			reg = a_reg(reg);
			if (size == BYTE)
				inc_size = 1;
			else if (size == WORD)
				inc_size = 2;
			else inc_size = 4;
			value = (S32*) &memory[A[reg]];
			A[reg] = A[reg] + inc_size;
			bwinc = 4;
			linc = 8;
			legal = TRUE;
			}
		    break;
	   case	4 : if (mask & bit_5)
			{
			reg = a_reg(reg);
			if (size == BYTE)
				inc_size = 1;
			else if (size == WORD)
				inc_size = 2;
			else inc_size = 4;
			A[reg] = A[reg] - inc_size;
			value = (S32*) &memory[A[reg]];
			bwinc = 6;
			linc = 10;
			legal = TRUE;
			}
		    break;
	   case	5 : if (mask & bit_6)
			{
			reg = a_reg(reg);
			mem_request (&PC, (S32) WORD, &ext);
			from_2s_comp (ext, (S32) WORD, &ext);
			value = (S32*) &memory[A[reg] + (ext & WORD)];
			bwinc = 8;
			linc = 12;
			legal = TRUE;
			}
		    break;
	   case	6 : if (mask & bit_7)
			{
			reg = a_reg(reg);
			/* fetch extension word */
			mem_request (&PC, (S32) WORD, &ext);
			disp = ext & 0xff;
			sign_extend (disp, BYTE, &disp);
			from_2s_comp (disp, (S32) WORD, &disp);
			/* get index register value */
			if (ext & 0x8000)
				ind_reg = A[a_reg((ext & 0x7000) >> 12)];
			else
				ind_reg = D[(ext & 0x7000) >> 12];
			/* get correct length for index register */
			if (!(ext & 0x0800))
				{
				sign_extend (ind_reg, WORD, &ind_reg);
				from_2s_comp (ind_reg, (S32) LONG, &ind_reg);
				}
			value = (S32*) (&memory[A[reg] + (disp & WORD) + ind_reg]);
			bwinc = 10;
			linc = 14;
			legal = TRUE;
			}
		    break;
	   case	7 : switch (reg) {
			   case	0 : if (mask & bit_8)
	    			{
					mem_request (&PC, (S32) WORD, &ext);
					value = (S32*) &memory[ext];
					bwinc = 8;
					linc = 12;
					legal = TRUE;
					}
				    break;
			   case	1 : if (mask & bit_9)
					{
					mem_request (&PC, (S32) WORD, &ext);
					mem_request (&PC, (S32) WORD, &temp_ext);
					ext = ext * 0xffff + temp_ext;
					value = (S32*) &memory[ext & ADDRMASK];
					bwinc = 12;
					linc = 16;
					legal = TRUE;
					}
					 break;
			   case	2 : if (mask & bit_10)
					{
					mem_request (&PC, (S32) WORD, &ext);
					from_2s_comp (ext, (S32) WORD, &ext);
					value = (S32*) &memory[PC + (ext & WORD) - 1];
					bwinc = 8;
					linc = 12;
					legal = TRUE;
					}
					 break;
			   case	3 : if (mask & bit_11)
					{
					/* fetch extension word */
					mem_request (&PC, (S32) WORD, &ext);
					disp = ext & 0xff;
					sign_extend (disp, BYTE, &disp);
					from_2s_comp (disp, (S32) WORD, &disp);
					/* get index register value */
					if (ext & 0x8000)
					   ind_reg = A[a_reg((ext & 0x7000) >> 12)];
					else
					   ind_reg = D[(ext & 0x7000) >> 12];
					/* get correct length for index register */
					if (!(ext & 0x0800))
						{
						sign_extend (ind_reg, WORD, &ind_reg);
						from_2s_comp (ind_reg, (S32) LONG, &ind_reg);
						}
					ext = ext & 0x00ff;
					value = (S32*) (&memory[PC - 1 + (disp & WORD) + ind_reg]);
					bwinc = 10;
					linc = 14;
					legal = TRUE;
					}
				    break;
			   case	4 : if (mask & bit_12)
					{
					if ((size == BYTE) || (size == WORD))
						mem_request (&PC, (S32) WORD, &ext);
					else
						mem_request (&PC, LONG, &ext);
					global_temp = ext;
					value = &global_temp;
					bwinc = 4;
					linc = 8;
					legal = TRUE;
					}
				    break;
				}
			break;
	}   	  /* switch */

	if (legal)
		{
		if (move_operation)/* choose EA2 in case of MOVE dest effective address */
			{
	 		EA2 = value;
			value_of (EA2, &EV2, size);
			}
		else
			{
			EA1 = value;
			value_of (EA1, &EV1, size);
			}
		return SUCCESS;
		}
	else
		return FAILURE;       /* return FAILURE if illegal addressing mode */

}



/**************************** S32 runprog() *******************************

   name       : S32 runprog ()
   parameters : NONE
   function   : executes a program at PC specified or current PC if not
                  specified.  this function is the outer loop of the
                  running program.  it handles i/o S32errupts, user
                  (keyboard) S32errupts, and calls the error routine if an
                  illegal opcode is found.


****************************************************************************/

S32 runprog()
{
	S32 i;
	char ch;
	S32 ICount;

	errflg = 0;

	for (ICount=0;;++ICount)    /* execute instructions until error or break */
		{

		exec_inst();		/* execute an instruction */

        if(ICount>=10)
        {
            ICount=0;
            Mem_CheckStack();
        }

		if (errflg)   /* if illegal opcode in program initiate an exception */
		{
            char IllegalMsg[80];
            sprintf(IllegalMsg, "Illegal Opcode %X",inst);
			Port_Puts(PUTS_ERROR,"runprog (cpu/run.c)",IllegalMsg);

			exception (1, 0, READ);
			mem_req (0x10, LONG, &PC);
			setdis();
    		return (0);
		}

        VERY_OLD_PC = OLD_PC; // update VERY_OLD_PC
		OLD_PC = PC;	/* update the OLD_PC */

		if (sstep) return (0);  /* if single stepping is on then stop running */
		}

}




/**************************** S32 exec_inst() *****************************

   name       : S32 exec_inst ()
   parameters : NONE
   function   : executes a single instruction at the location poS32ed
                  to by PC.  it is called from runprog() and sets the
                  flag "errflg" if an illegal opcode is detected so
                  that runprog() terminates.  exec_inst() also takes
                  care of handling the different kinds of exceptions
                  that may occur.  If an instruction returns a different
                  return code than "SUCCESS" then the appropriate
                  exception is initiated, unless the "exceptions" flag
                  is turned off by the user in which case the exception
                  is not initiated and the program simply terminates and
                  informs the user that an exception condition has occurred.


****************************************************************************/


S32 exec_inst()
{
S32	start, finish, exec_result, i;

//PC is incremented by 2 by mem_request
if ( !(mem_request (&PC, (S32) WORD, (S32 *) &inst)) )
   {
	start = offsets[(inst & FIRST_FOUR) >> 12];
	finish = offsets[((inst & FIRST_FOUR) >> 12) + 1] - 1;
	errflg = TRUE;

	if ((inst & 0xF000) == 0xA000)	/* unimplemented instruction */
	{
        errflg = FALSE;

	    if(Toolbox_ALine(inst)==0)
	    {
	        char TrapMsg[80];

//        	mem_req (0x28, LONG, &PC);

            sprintf(TrapMsg, "Unimplemented A-Line %X",inst);
		    Port_Puts(PUTS_ERROR,"exec_inst (cpu/run.c)",TrapMsg);
	    }
	}
	else
	if ((inst & 0xF000) == 0xF000)	/* unimplemented instruction */
	{
        char TrapMsg[80];

//		errflg = FALSE;
//		mem_req (0x2C, LONG, &PC);

        sprintf(TrapMsg, "Unimplemented F-Line %X",inst);
        Port_Puts(PUTS_ERROR,"exec_inst (cpu/run.c)",TrapMsg);
	}
	else
	for (i = start; i <= finish; i++)
		{
		if ((inst & ~inst_arr[i].mask) == inst_arr[i].val)
			{
			//DEBUG: Output instruction name and the opcode (in hex)
			printf("Debug - exec_inst (cpu/run.c): %s (%Xh)\n",inst_arr[i].name, inst); //DEBUG

//            if(inst==0)
 //ORI (Have been seeing more of these than we should)
//                setdis();

			exec_result = (*(names[i]))();

	if (exceptions)
	{
			switch (exec_result)
				{
				case SUCCESS  : break;
				case BAD_INST : /* load the PC from the illegal */
									 exception (1, 0, READ);
									 mem_req (0x10, LONG, &PC);	    /* vector */
									 {
                                        char IllegalMsg[80];
                                        sprintf(IllegalMsg, "Illegal instruction %X",inst);
                            			Port_Puts(PUTS_WARNING,"exec_inst (cpu/run.c)",IllegalMsg);
	                                 }
						          break;
				case NO_PRIVILEGE :
									 exception (1, 0, READ);
									 mem_req (0x20, LONG, &PC);
	                                 Port_Puts (PUTS_WARNING,"exec_inst (cpu/run.c)","Supervisor privilege violation");
						          break;
				case CHK_EXCEPTION :
									 exception (2, 0, READ);
									 mem_req (0x18, LONG, &PC);
						          break;
				case ILLEGAL_TRAP :
									 exception (1, 0, READ);
									 mem_req (0x10, LONG, &PC);
						          break;
				case STOP_TRAP : sstep = TRUE;	/* break out of the program */
									 break;
				case TRAP_TRAP :
				                     exception (2, 0, READ);
									 mem_req (128 + (inst & 0x0f) * 4, LONG, &PC);
						          break;
				case TRAPV_TRAP :
									 exception (2, 0, READ);
									 mem_req (0x1c, LONG, &PC);
						          break;
				case DIV_BY_ZERO :
									 exception (2, 0, READ);
									 mem_req (0x14, LONG, &PC);
						          break;
				}

			if (SR & tbit)   /* initiate a trace exception */
				{
				exception (1, 0, READ);
				mem_req (0x24, LONG, &PC);
				}
	}
	else
	{
			switch (exec_result)
				{
				case SUCCESS  : break;
				case BAD_INST : sstep = TRUE;	/* break out of the program */
								  {
                                      char IllegalMsg[80];
                                      sprintf(IllegalMsg, "Illegal instruction %X. Execution halted",inst);
                            	      Port_Puts(PUTS_ERROR,"exec_inst (cpu/run.c)",IllegalMsg);
	                              }
						          break;
				case NO_PRIVILEGE : sstep = TRUE;	/* break out of the program */
	                              Port_Puts (PUTS_ERROR,"exec_inst (cpu/run.c)","Supervisor privilege violation. Execution halted");
						          break;
				case CHK_EXCEPTION : sstep = TRUE;	/* break out of the program */
	                              Port_Puts (PUTS_ERROR,"exec_inst (cpu/run.c)","CHK exception. Execution halted");
						          break;
				case ILLEGAL_TRAP : sstep = TRUE;	/* break out of the program */
								  {
                                      char IllegalMsg[80];
                                      sprintf(IllegalMsg, "Illegal TRAP %X. Execution halted",inst);
                            	      Port_Puts(PUTS_ERROR,"exec_inst (cpu/run.c)",IllegalMsg);
	                              }
						          break;
				case STOP_TRAP : sstep = TRUE;	/* break out of the program */
	                              Port_Puts (PUTS_INFO,"exec_inst (cpu/run.c)","STOP instruction. Execution halted");
								  break;
				case TRAP_TRAP : sstep = TRUE;	/* break out of the program */
                             	  Port_Puts (PUTS_INFO,"exec_inst (cpu/run.c)","TRAP exception. Execution halted");
						          break;
				case TRAPV_TRAP : sstep = TRUE;	/* break out of the program */
                            	  Port_Puts (PUTS_INFO,"exec_inst (cpu/run.c)","TRAPV exception. Execution halted");
						          break;
				case DIV_BY_ZERO : sstep = TRUE;	/* break out of the program */
                            	  Port_Puts (PUTS_ERROR,"exec_inst (cpu/run.c)","Divide by zero. Execution halted");
						          break;
				}

	if (SR & tbit)
		{
		sstep = TRUE;	/* break out of the program */
		Port_Puts (PUTS_INFO,"exec_inst (cpu/run.c)","TRACE exception. Execution halted");
		}
	}
			errflg = FALSE;
			break;
			}
		}

}

}





/**************************** S32 exception () *****************************

   name       : S32 exception (class, loc, r_w)
   parameters : S32 class : class of exception to be taken
                S32 loc : the address referenced in the case of an
                  address or bus error
                S32 r_w : in the case of an address or bus error, this
                  tells whether the reference was a read or write
   function   : initiates exception processing by pushing the appropriate
                  exception stack frame on the system stack and turning
                  supervisor mode on and trace mode off.


****************************************************************************/

S32	exception(class, loc, r_w)
S32	class;
S32	loc;
S32	r_w;
{
	S32	info_word;

	if ( (class == 1) || (class == 2))
		{
		A[8] -= 4;		/* create the stack frame for class 1 and 2 exceptions */
		put ((S32 *) &memory[A[8]], OLD_PC, LONG);
		A[8] -= 2;
		put ((S32 *) &memory[A[8]], (S32) SR, (S32) WORD);
		}
	else
		{						/* class 0 exception (address or bus error) */
		A[8] -= 4;						/* create the exception stack frame */
		put ((S32 *) &memory[A[8]], OLD_PC, LONG);
		A[8] -= 2;
		put ((S32 *) &memory[A[8]], (S32) SR, (S32) WORD);
		A[8] -= 2;
		put ((S32 *) &memory[A[8]], (S32) inst, (S32) WORD);
		A[8] -= 4;
		put ((S32 *) &memory[A[8]], loc, LONG);
		A[8] -= 2;
		info_word = 0x6;
		if (r_w == READ)
			info_word |= 0x10;
		put ((S32 *) &memory[A[8]], (S32)0x0016, (S32) WORD);/* push information word */
		}
	SR = SR | sbit;			/* force processor into supervisor state */
	SR = SR & ~tbit;			/* turn off trace mode */

}



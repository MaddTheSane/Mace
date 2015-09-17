/*
  Mace - http://www.macehq.cx
  Copyright 1999-2004
  See the file README for more information
*/


/***************************** 68000 SIMULATOR ****************************

File Name: VAR.H
Version: 1.0

This file contains all global variable definitions for the simulator
program.  It is included in the module "sim.c" which contains main().



        BE CAREFUL TO KEEP THESE DECLARATIONS IDENTICAL TO THE GLOBAL
        VARIABLE DECLARATIONS IN "EXTERN.H"


***************************************************************************/


#include "def.h"                   /* include file for constants & types */
#include "Proto.h"		/* function prototypes */


S32 	D[D_REGS], OLD_D[D_REGS], A[A_REGS], OLD_A[A_REGS];
S32 	PC, OLD_PC, VERY_OLD_PC;
S16	SR, OLD_SR;
S32 	trace, sstep, old_trace, old_sstep, exceptions;

U8 	memory[MEMSIZE];								/* array for memory */
S32 	errflg;
S32 	inst;

S32	*EA1, *EA2;
S32	EV1, EV2;

S32	source, dest, result;

S32	global_temp;		/* to hold an immediate data operand */

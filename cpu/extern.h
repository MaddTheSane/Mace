/*
  Mace - http://www.macehq.cx
  Copyright 1999-2004
  See the file README for more information
*/

#include "def.h"			/* constant declarations */
#include "Proto.h"		/* function prototypes */


extern S32			D[D_REGS], OLD_D[D_REGS], A[A_REGS], OLD_A[A_REGS];
extern S32			PC, OLD_PC, VERY_OLD_PC;
extern S16			SR, OLD_SR;

extern U8			 memory[MEMSIZE];
extern S32			errflg;
extern S32			trace, sstep, old_trace, old_sstep, exceptions;
extern S32			inst;
extern S32			*EA1, *EA2;
extern S32			EV1, EV2;

extern S32			source, dest, result;

extern S32			global_temp;		/* to hold an immediate data operand */

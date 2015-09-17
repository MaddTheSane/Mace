/*
  Mace - http://www.macehq.cx
  Copyright 1999-2004
  See the file README for more information
*/

//Glue between emulator and CPU core


#ifndef CPU_H
#define CPU_H

#define	D_REGS	8              	/* number of D registers */
#define	A_REGS	9              	/* number of A registers */

#include "../Globals.h"
#include "Var.h"


extern S32 D[D_REGS], A[A_REGS];
extern S32 PC;

S32 mem_put(S32 data,S32 loc,S32 size);
S32 mem_req(S32 loc,S32 size,S32 *result);

S32 setdis(void );


void Mem_CheckStack(void); //Checks for stack and heap collision

U8 Toolbox_ALine(U16 inst); //Returns 1 if it handled the trap

void CPU_Init(void);
void CPU_Start(void);

#include "../Port.h"

#endif //CPU_H

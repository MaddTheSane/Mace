/*
  Mace - http://www.macehq.cx
  Copyright 1999-2004
  See the file README for more information
*/

//Glue between emulator and CPU core

#include <stdio.h>
#include "cpu.h"
#include "Var.h"         /* include declarations for global variables */

void CPU_Init(void)
{
    init();
    return;
}

void CPU_Start(void)
{
    VERY_OLD_PC=OLD_PC=PC;

    setdis();

    sstep=1;

    while(1)//getchar())
    {
        runprog();

//        setdis();
    }

    return;
}



S32 init(void)                     /* initialization for the simulator */
{
	S32	i;
	long loop_counter;

	for(i=0; i<=7; i++)
	{
		A[i] = 0;
		D[i] = 0;
	}
    A[8]=0;

	for (loop_counter = 0; loop_counter < MEMSIZE; loop_counter++)
		memory[loop_counter] = 0;


	exceptions = FALSE;
	for (i = 0; i <= 7; i++)
		A[i] = D[i] = OLD_A[i] = OLD_D[i] = 0;
	OLD_A[8] = PC = 0;
	SR = 0x2000;
	A[8] = 0xf00;
	VERY_OLD_PC=OLD_PC = OLD_SR = -1; /* set different from 'PC' */
	trace = sstep = FALSE;
	old_trace = old_sstep = TRUE;	  /* set different from 'trace' and 'sstep' */

	return(0);
}

S32 setdis(void)														/* shows register display */
{
    U32 StackData;
    U32 AtPC, AtOldPC, AtVeryOldPC;
    char DebugMsg[256];

	printf("컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴�\n");
	printf("<D0>  = %08lx  <D4>  = %08lx  <A0>  = %08lx  <A4> = %08lx\n",D[0],
		D[4],A[0],A[4]);
	printf("<D1>  = %08lx  <D5>  = %08lx  <A1>  = %08lx  <A5> = %08lx\n",D[1],
		D[5],A[1],A[5]);
	printf("<D2>  = %08lx  <D6>  = %08lx  <A2>  = %08lx  <A6> = %08lx\n",D[2],
		D[6],A[2],A[6]);
	printf("<D3>  = %08lx  <D7>  = %08lx  <A3>  = %08lx  <A7> = %08lx\n",D[3],
		D[7],A[3],A[7]);
	printf("                                                      <A7'>= %08lx\n"
		, A[8]);
	printf("                                                      <PC> = %08lx\n"
		,PC);

    mem_req((long)PC,WORD,(long *)&AtPC);
    mem_req((long)OLD_PC,WORD,(long *)&AtOldPC);
    mem_req((long)VERY_OLD_PC,WORD,(long *)&AtVeryOldPC);
    printf("At VeryOldPC: %X, At OldPC: %X, At PC: %X\n",AtVeryOldPC,AtOldPC,AtPC);

    mem_req((long)A[8],LONG,(long *)&StackData);
    printf("%Xh on the stack\n",StackData);

	printf("컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴�\n");

	old_sstep = ~sstep;
	old_trace = ~trace;
	OLD_SR = ~SR;

	getchar();
	
    return(0);
}

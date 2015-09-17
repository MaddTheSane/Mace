/*
  Mace - http://www.macehq.cx
  Copyright 1999-2004
  See the file README for more information
*/

#ifndef MEM_H

#define MEM_H

#include "cpu/def.h"

U8 Mem_GetB (U8 * MemP, U32 Offset);
U16 Mem_GetW (U8 * MemP, U32 Offset);
U32 Mem_GetL (U8 * MemP, U32 Offset);

void Mem_Init(void);

void Mem_LoadApp (void);
U32 Mem_Alloc (U32 Size); //Returns address or 0 on error
U8 Mem_Free (U32 Addr); //Returns 0 on error

void Mem_CheckStack(void); //Checks for stack and heap collision

U8 StackWarn;

typedef struct
{
    U32 Addr;
    U32 Size;
} Mem_AllocSt;

//Memory layout

#define Mem_SysHeap_Start   0x1000 //Inside Macintosh says 2816 0xB00
#define Mem_SysHeap_End     (Mem_SysHeap_Start+0x7FFFF) //512KB System Heap
#define Mem_App_Start       (Mem_SysHeap_End+1)
#define Mem_App_End         (MEMSIZE-1)
U32 AppHeapLimit, Mem_A5, Mem_Stack, Mem_JumpSize;

#endif MEM_H

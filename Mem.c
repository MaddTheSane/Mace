/*
  Mace - http://www.macehq.cx
  Copyright 1999-2004
  See the file README for more information
*/

#include "Globals.h"
#include "Mem.h"
#include "Res.h"
#include "Port.h"
#include "File.h"
#include "cpu/cpu.h"

Mem_AllocSt Mem_Allocs[4096];
U16 Mem_AllocNum;

U8 Mem_GetB (U8 * MemP, U32 Offset)
{
    return(MemP[Offset]);
}

U16 Mem_GetW (U8 * MemP, U32 Offset)
{
    return(MemP[Offset]+(MemP[Offset+1]<<8));
}

U32 Mem_GetL (U8 * MemP, U32 Offset)
{
    return(MemP[Offset]+(MemP[Offset+1]<<8))+
      (MemP[Offset+2]<<16)+(MemP[Offset+3]<<24);
}

void Mem_Init(void)
{
    Mem_AllocNum=0;
    StackWarn=0;

    return;
}

U32 Mem_Alloc (U32 Size)
{
    if(Mem_AllocNum>=4096)
        return(0);

    if(Mem_AllocNum)
    {
        U32 Addr1, Addr2;
        U16 Count;

        Mem_Allocs[Mem_AllocNum].Addr=Mem_App_Start+AppHeapLimit;
        Mem_Allocs[Mem_AllocNum].Size=0;

        for(Count=0;Count<Mem_AllocNum;++Count) //Look at gaps between allocations
        {
            Addr1=Mem_Allocs[Count].Addr + Mem_Allocs[Count].Size;
            Addr2=Mem_Allocs[Count+1].Addr;

            if(Addr2-Addr1>=Size) //See if gap is big enough
            {
                //Insert this allocation into Mem_Allocs, and return the address.

                U16 Count2=Count+1;

                for(Count=Mem_AllocNum;Count>Count2;--Count)
                {
                    Mem_Allocs[Count].Addr=Mem_Allocs[Count-1].Addr;
                    Mem_Allocs[Count].Size=Mem_Allocs[Count-1].Size;
                }

                Mem_Allocs[Count2].Addr=Addr1;
                Mem_Allocs[Count2].Size=Size;
                ++Mem_AllocNum;

                return(Mem_Allocs[Count2].Addr);
            }
        }
    }
    else
    {
        Mem_Allocs[0].Addr=Mem_App_Start;
        Mem_Allocs[0].Size=Size;
        ++Mem_AllocNum;

        return(Mem_Allocs[0].Addr);
    }

    return(0);
}

U8 Mem_Free (U32 Addr)
{
    U16 Count;

    for(Count=0;Count<Mem_AllocNum;++Count) //Find allocation
    {
        if(Mem_Allocs[Count].Addr==Addr)
        {
            //Remove allocation

            --Mem_AllocNum;

            for(;Count<Mem_AllocNum;++Count)
            {
                Mem_Allocs[Count].Addr=Mem_Allocs[Count+1].Addr;
                Mem_Allocs[Count].Size=Mem_Allocs[Count+1].Size;
            }

            return(1);
        }
    }

    return(0);
}

void Mem_LoadApp (void)
{
    U32 DataOffset, MapOffset, DataLen, MapLen,
        CODE0, SIZE, AppHeapAvail, RequestSize,
        MinSize, Count;
    U8 Compat32=0;
    char Temp[256];

    CODE0=Res_GetOffset('CODE',0); //Find CODE 0
    CODE0+=4;

    Mem_JumpSize=FlipL(Mem_GetL(ResP,CODE0 - 4));

    if(FlipW(Mem_GetW(ResP,CODE0 + 0x10))==0x0004 || Res_GetOffset('rseg',0))
        Port_Puts(PUTS_DEBUG,"Mem_LoadApp (mem.c)","CFM-68K");
    else
        Port_Puts(PUTS_DEBUG,"Mem_LoadApp (mem.c)","Classic 68K");

    //Total App mem - jump table size - 66KB - 32
    //That gives the stack 32KB and a 2KB seperator from heap
    AppHeapAvail=(Mem_App_End - Mem_App_Start)
                 - Mem_JumpSize
                 - 0x10800
                 - 32;

    AppHeapLimit=0;

    SIZE=Res_GetOffset('SIZE',0); //Find SIZE 0
    if(SIZE)
    {
        SIZE+=4;

        if((ResP[SIZE+1] & 0x80) == 0x80) //Endianness (bit 0 = bit 7)
            Compat32=1;
        else
            Compat32=0;

        RequestSize=FlipL(Mem_GetL(ResP,SIZE+2));
        MinSize=FlipL(Mem_GetL(ResP,SIZE+6));
    }
    else
    {
        SIZE=Res_GetOffset('SIZE',0xFFFF); //Find SIZE -1
        if(SIZE)
        {
            SIZE+=4;

            if((ResP[SIZE+1] & 0x80) == 0x80) //Endianness (bit 0 = bit 7)
                Compat32=1;
            else
                Compat32=0;

            RequestSize=FlipL(Mem_GetL(ResP,SIZE+2));
            MinSize=FlipL(Mem_GetL(ResP,SIZE+6));
        }
        else
        {
            RequestSize=0x80000; //512KB
            MinSize=0;
        }
    }

/* For future use (when a 32-bit CPU is emulated)
    if(Compat32==0)
        Port_Puts(PUTS_WARNING,"Mem_LoadApp (mem.c)","Application may not be 32-bit compatible");
    if(Compat32==1)
        Port_Puts(PUTS_DEBUG,"Mem_LoadApp (mem.c)","Application is 32-bit compatible");
*/

    if(RequestSize>AppHeapAvail)
    {
        if(MinSize>AppHeapAvail)
        {
            Port_Puts(PUTS_ERROR,"Mem_LoadApp (mem.c)","Application requires more memory than is available");
            File_Close(ResP);
            App_Exit();
        }
        else
        {
            AppHeapLimit=AppHeapAvail; //Use all available app memory
            Port_Puts(PUTS_DEBUG,"Mem_LoadApp (mem.c)","Application requests more memory than is available, but\nthere is enough to satisfy minimum requirement. Using all available app memory");
        }
    }
    else
        AppHeapLimit=RequestSize; //Max out at requested memory size

    //Heap Size + 32KB (Stack) + 2KB (Buffer)
    Mem_Stack=Mem_App_Start+AppHeapLimit+0x8800;
    //Mem_Stack is end of stack, since it's FILO

    if((Mem_Stack % 2) == 1) //If it's odd
        ++Mem_Stack; //Make it even

    //Mem_Stack (Heap Size + 32KB (Stack) + 2KB) + 32KB (App Globals)
    Mem_A5=Mem_Stack+0x8000;

    if((Mem_A5 % 2) == 1) //If it's odd
        ++Mem_A5; //Make it even

    A[5]=Mem_A5;
    A[8]=Mem_Stack; //Stack Pointer
    A[6]=Mem_Stack; //Stack Frame

    //Copy jump table into memory
    for(Count=0;Count<Mem_JumpSize;++Count)
        mem_put(ResP[CODE0+Count],Mem_A5+32+Count,BYTE);

    mem_put(32,Mem_A5+32+0xC,LONG); //Jump table offset
    mem_put(32,0x934,WORD); //CurJTOffset

    //A5 + AppParams + JumpTable Header + First Offset WORD
    PC=Mem_A5+32+0x10+2; //Run code in jump table to load entry point

    return;
}

void Mem_CheckStack(void)
{
    //If stack has used up all stack space and is in the emergency 2KB buffer
    if(A[8]<=Mem_App_Start+AppHeapLimit+0x800)
    {
        if(A[8]<=Mem_App_Start+AppHeapLimit)
        {
            if(StackWarn<3)
            {
                StackWarn=3;
                Port_Puts(PUTS_WARNING,"Mem_CheckStack (mem.c)","Stack is in the heap zone; this is a serious problem! Data corruption may have already occured! You should save and exit immediately!");
            }
        }
        else
        {
            if(A[8]<=Mem_App_Start+AppHeapLimit+0x10)
            {
                if(StackWarn<2)
                {
                    StackWarn=2;
                    Port_Puts(PUTS_WARNING,"Mem_CheckStack (mem.c)","Stack is very near the heap zone. You should save and exit now! If the stack continues to expand, it will go into the heap zone, which is a serious problem.");
                }
            }
            else
            {
                if(StackWarn<1)
                {
                    StackWarn=1;
                    Port_Puts(PUTS_WARNING,"Mem_CheckStack (mem.c)","Stack is near the heap zone. You should save and exit. If the stack continues to expand, it may go into the heap zone, which is a serious problem.");
                }
            }
        }
    }
    else
    {
        if(StackWarn)
        {
            StackWarn=0;
            Port_Puts(PUTS_INFO,"Mem_CheckStack (mem.c)","Stack is ok");
        }
    }

    return;
}

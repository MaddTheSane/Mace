/*
  Mace - http://www.macehq.cx
  Copyright 1999-2004
  See the file README for more information
*/

//Emulate Toolbox (Traps and System Globals)

//There's actually more than the "Toolbox" here,
//parts of the OS are emulated too.

#include <stdio.h>

#include "Globals.h"
#include "Toolbox.h"
#include "Port.h"

#ifdef Win32
  #include <Windows.h>
#endif

#include "Mem.h"
#include "Res.h"
#include "cpu/cpu.h"

U8 Toolbox_ALine(U16 inst)
{
    //IMPORTANT: PC points to the next instruction, the current instruction is PC - 2

    char Temp[256];

//    setdis();

    sprintf(Temp,"A-Line %X",inst);
    Port_Puts(PUTS_DEBUG,"Toolbox_ALine (toolbox.c)",Temp);

    switch(inst)
    {
        //Should implement return values for ALL A-Line's that require them

        case 0xA03B: //Delay
            Toolbox_Delay();
            return(1);

        case 0xA992: //DetachResource
            //DO MORE HERE
            A[8]+=2; //Pop resource handle off stack
            return(1);

        case 0xA994: //CurResFile
            //DO MORE HERE
            A[0]=0xABCD; //Return dummy handle
            return(1);

        case 0xA036: //MoreMasters
            return(1);

        case 0xA9A0: //GetResource
            Toolbox_GetResource();
            return(1);

        case 0xA9A1: //GetNamedResource
            Toolbox_GetNamedResource();
            return(1);

        case 0xA9A3: //ReleaseResource
            Toolbox_ReleaseResource();
            return(1);

        case 0xA850: //InitCursor
            Port_NormalCursor();
            return(1);

        case 0xA86E: //InitGraf
            Toolbox_InitGraf();
            return(1);

        case 0xA8FE: //InitFonts
            return(1);

        case 0xA912: //InitWindows
            return(1);

        case 0xA930: //InitMenus
            return(1);

        case 0xA9CC: //TEInit
            return(1);

        case 0xA97B: //InitDialogs
            Toolbox_InitDialogs();
            return(1);

/* Need to get duration off the stack!
        case 0xA9C8: //SysBeep
            #ifdef Win32
                MessageBeep(MB_OK);
            #else
                printf("%c",7);
            #endif
            return(1);
*/

        case 0xA9EE: //Pack7 (Binary-Decimal Conversion Package)
            Toolbox_BDCP();
            return(1);

        case 0xA9F0: //LoadSeg
            Toolbox_LoadSeg();
            return(1);

        case 0xABFF: //DebugStr
            //Shouldn't have to do anything here
            return(1);

        default:
            return(0);
    }

    return(0);
}

#ifdef Win32
void Toolbox_Delay(void)
{
    //A Mac tick is 1/60th second

    U32 numTicks, finalTicks;

    //Convert Mac ticks to Windows ticks
    numTicks=A[0];
    numTicks=(U32)((double)numTicks*16.6666666666666666666666666666667);

    //Windows version of Delay
    Sleep(numTicks);

    //Convert Windows ticks to Mac ticks
    finalTicks=GetTickCount();
    finalTicks=(U32)((double)finalTicks/16.6666666666666666666666666666667);
    D[0]=finalTicks;


    //Update global variable Ticks
    mem_put(finalTicks,0x16A,LONG);

    //Need to implement global variable Ticks better
    //Have a timing thread that updates global Ticks

    return;
}
#else //Win32
void Toolbox_Delay(void)
{
    puts("WARNING: Need to port Toolbox_Delay() to this OS!!!");
    return;
}
#endif //Win32

void Toolbox_GetResource(void)
{
    U32 Type, Offset, Count, Length, Temp, ResPtr;
    U16 ID;

    mem_req((long) A[8],WORD,(long *) &ID);
    A[8]+=2;

    mem_req((long) A[8],LONG,(long *) &Type);
    A[8]+=4;

    Offset=Res_GetOffset(Type,ID);
    if(!Offset)
    {
        A[8]-=4;
        mem_put(0,A[8],LONG);

//        A[0]=0; //Should it return -1 (0xFFFFFFFF) ???
        mem_put((long)((U16)-192),0xA60,WORD); //Set global ResErr to -192 (Resource not found)
        return;
    }

    Offset+=4;

    Length=FlipL(Mem_GetL(ResP,Offset - 4));

    ResPtr=Mem_Alloc(Length);

    for(Count=0;Count<Length;++Count)
        mem_put(ResP[Offset+Count],ResPtr+Count,BYTE);


    A[8]-=4;
    mem_put(ResPtr,A[8],LONG);

    mem_put(0,0xA60,WORD); //Set global ResErr to 0 (No Error)

    {
        char TempS[5];

        TempS[0]=Type>>24;
        TempS[1]=Type>>16;
        TempS[2]=Type>>8;
        TempS[3]=Type;
        TempS[4]=0;

        printf("%s, %d, %d, %Xh\n",TempS, ID, Length,A[0]);

        setdis();
    }

    return;
}

void Toolbox_GetNamedResource(void)
{
    U32 Type, NamePtr, Count;
    char TempS[5], NameS[256];

//Warning message. This function really needs to be fixed
printf("WARNING: *INCOMPLETE* GetNamedResource()\n");

    mem_req((U32) A[8],LONG,(U32 *) &NamePtr);
    A[8]+=4;

    mem_req((U32) A[8],LONG,(U32 *) &Type);
    A[8]+=4;

    setdis();

    TempS[0]=Type>>24;
    TempS[1]=Type>>16;
    TempS[2]=Type>>8;
    TempS[3]=Type;
    TempS[4]=0;

    //255 or 256?
    for(Count=0;Count<255;++Count)
    {
        mem_req(NamePtr+Count,BYTE,(long *) &NameS[Count]);

//        printf("%c,",(U8)NameS[Count]); //DEBUG
    }
//    printf("\n");

    NameS[Count]=0;

    printf("%s, %Xh, %s\n",TempS, NamePtr, NameS);

    setdis();

/*
    {
        char TempS[256];
        int Count;
        CPTR NamePtr = get_long(m68k_areg(regs, 7));

        for(Count=0;Count<256;++Count)
            TempS[Count]=get_byte(NamePtr+Count);

        TempS[Count]=0;
    }
*/

    return;
}

void Toolbox_ReleaseResource(void)
{
    //DUMMY
    A[8]+=2; // Pop resource handle? off the stack

    return;
}

void Toolbox_InitDialogs(void)
{
    U32 StackData;
    char DebugMsg[256];

    mem_req(A[8],LONG,(long *) &StackData);
    A[8]+=4; //4 or 2 bytes?

    sprintf(DebugMsg,"%Xh on the stack",StackData);
    Port_Puts(PUTS_DEBUG,"Toolbox_InitDialogs (toolbox.c)",DebugMsg);

    return;
}

void Toolbox_InitGraf(void)
{
    U32 StackData;
    char DebugMsg[256];

    mem_req(A[8],LONG,(long *) &StackData);
    A[8]+=4; //4 or 2 bytes?

    sprintf(DebugMsg,"%Xh on the stack",StackData);
    Port_Puts(PUTS_DEBUG,"Toolbox_InitGraf (toolbox.c)",DebugMsg);

    return;
}

void Toolbox_LoadSeg(void)
{
    U32 CODE, Count;
    //Note: there was a bug here, when these were defined as U16
    U32 StackData, Seg_Offset;
    
    char DebugMsg[256];

    setdis();

    //Take segment number from the stack
    mem_req(A[8],WORD,(long *) &StackData);
    A[8]+=2;

    //This will probably cause problems if LoadSeg isn't called from the jump table
    mem_req(PC-8,WORD,(long *) &Seg_Offset);

    sprintf(DebugMsg,"%Xh on the stack, Seg_Offset = %Xh", StackData, Seg_Offset);
    Port_Puts(PUTS_DEBUG,"Toolbox_LoadSeg (toolbox.c)",DebugMsg);

    CODE=Res_GetOffset('CODE',StackData); //Find CODE 1

    if(!CODE)
        Port_Puts(PUTS_ERROR,"Toolbox_LoadSeg (toolbox.c)","Res_GetOffset failed");

    //Allocate memory for this CODE segment and set PC to the start of it
    PC=Mem_Alloc(FlipL(Mem_GetL(ResP,CODE)));

    if(!PC)
        Port_Puts(PUTS_ERROR,"Toolbox_LoadSeg (toolbox.c)","Mem_Alloc failed");

    //Copy this CODE segment into memory
    for(Count=0;Count<FlipL(Mem_GetL(ResP,CODE));++Count)
        mem_put(ResP[CODE+Count],PC+Count,BYTE);

    //Set PC to start of the desired function in this segment
    PC+=Seg_Offset+4; //4 byte Near segment header

    {
        U32 Temp1,Temp2;

        mem_req(PC,LONG,(long *) &Temp1);
        mem_req(PC+4,LONG,(long *) &Temp2);

        printf("%X %X\n",Temp1,Temp2);
    }

    //This is trap A9F0

    return;
}

void Toolbox_BDCP(void)
{
    U16 Selector;
    mem_req(A[8],WORD,(long *) &Selector); //Get routine selector from stack
    A[8]+=2; //Remove routine selector from stack

    if(Selector==0) //NumToString
    {
        char String[256];
        U8 Length, Count;

        mem_req(A[0],BYTE,(long *) &Length);

        Port_itoa(D[0],String,10);

        if(Length < strlen(String))
        {
            A[0]=0;
            return;
        }

        //return it with a length byte?

        for(Count=0;Count<strlen(String)+1;++Count)
            mem_put(String[Count],A[0]+Count,BYTE);
    }
    else if(Selector==1) //StringToNum
    {
        char String[256];
        U8 Length, Count;

        mem_req(A[0],BYTE,(long *) &Length);
        if(!Length)
        {
            D[0]=0;
            return;
        }

        for(Count=0;Count<Length;++Count)
            mem_req(A[0]+1+Count,String[Count],(long *) BYTE);

        String[Length]='\0';

        D[0]=atoi(String);
    }
/*
    else if(Selector==2) //PStr2Dec
    {
    }
    else if(Selector==3) //Dec2Str
    {
    }
    else if(Selector==4) //CStr2Dec
    {
    }
*/
    else
    {
        Port_Puts(PUTS_WARNING,"Toolbox_BDCP (toolbox.c)","Unimplemented routine");
        A[0]=0;
    }

    return;
}


/*
  Toolbox_Init() should be called before starting the CPU,
  but after initing the CPU and memory.
*/

void Toolbox_Init(void)
{
    //This assumes that mem_put does byte ordering conversion for us!!!

    mem_put(AppHeapLimit,0x130,LONG); //ApplLimit (App heap limit)
    mem_put(Mem_App_Start,0x2AA,LONG); //ApplZone (Address of app heap)
    mem_put(Mem_A5+32+Mem_JumpSize,0x10C,LONG); //BufPtr (End of jump table)
    mem_put(120,0x2F4,LONG); //CaretTime (Blink interval in ticks)
    mem_put(0,0x12F,WORD); //CPUFlag (CPU type [68000])

/*
    mem_put(strlen(MacAppName),0x910,BYTE); //CurApName
    for(Count=0;Count<strlen(MacAppName);++Count)
        mem_put(MacAppName[Count],0x911+Count,BYTE);
*/

    mem_put(32,0x934,WORD); //CurJTOffset (offset to jump table from A5)
//    mem_put( ,0xA5A,WORD); //CurMap (Ref num of current resource file)
    mem_put(Mem_A5,0x904,LONG); //CurrentA5
    mem_put(Mem_A5,0x908,LONG); //CurStackBase
    mem_put(0x8000,0x322,LONG); //DefltStack (Stack space [32KB])
    mem_put(120,0x2F0,LONG); //DoubleTime (Double click interval in ticks)
    mem_put(Mem_App_Start+AppHeapLimit,0x114,LONG); //HeapEnd (End of app heap)
    mem_put(0x00FFFFFF,0x31A,LONG); //Lo3Bytes
    mem_put(Mem_App_End,0x108,LONG); //MemTop (End of RAM)
    mem_put(0xFFFFFFFF,0xA06,LONG); //MinusOne
    mem_put(0x00010001,0xA02,LONG); //OneOne
    mem_put(0xFFFF,0x28E,WORD); //ROM85 (ROM version)
    mem_put(Mem_SysHeap_Start,0x2A6,LONG); //SysZone (System heap)
    mem_put(Mem_App_Start,0x118,LONG); //TheZone (Current heap zone)

    return;
}

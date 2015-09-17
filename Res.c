/*
  Mace - http://www.macehq.cx
  Copyright 1999-2004
  See the file README for more information
*/

#include <string.h>
#include <stdio.h>
#include "Globals.h"
#include "Res.h"
#include "Port.h"
#include "Mem.h"

void Res_DecodeHeader(U32 * DataOffset, U32 * MapOffset, U32 * DataLen, U32 * MapLen)
{
    if(DataOffset)
        *DataOffset=FlipL(Mem_GetL(ResP, 0));

    if(MapOffset)
        *MapOffset=FlipL(Mem_GetL(ResP, 4));

    if(DataLen)
        *DataLen=FlipL(Mem_GetL(ResP, 8));

    if(MapLen)
        *MapLen=FlipL(Mem_GetL(ResP, 12));

    return;
}

U32 Res_GetOffset(U32 Type, U16 ID)
{
    U32 DataOffset, MapOffset, DataLen, MapLen,
        ResOffset=0, Count;
    U16 TypeOffset, NameOffset, NumTypes, NumRes=0,
        RefOffset=0, ResNameOffset=0;
    U8 ResAttrib=0;

    if(!ResP) //Check for invalid ResP
    {
        Port_Puts(PUTS_DEBUG,"Res_GetOffset (res.c)","Invalid ResP");
        return(0);
    }

    Res_DecodeHeader(&DataOffset, &MapOffset, &DataLen, &MapLen);

    TypeOffset=FlipW(Mem_GetW(ResP,MapOffset+24));
    NameOffset=FlipW(Mem_GetW(ResP,MapOffset+26));

    NumTypes=FlipW(Mem_GetW(ResP,MapOffset+28))+1;

    for(Count=MapOffset+TypeOffset+2;
        Count<MapOffset+TypeOffset+(NumTypes*8);
        Count+=8)
    {
        if(FlipL(Mem_GetL(ResP,Count))==Type)
        {
            //Found the Type
            NumRes=FlipW(Mem_GetW(ResP,Count+4))+1;
            RefOffset=FlipW(Mem_GetW(ResP,Count+6));
            break;
        }
    }

    if(!NumRes && !RefOffset)
    {
        char DebugMsg[256], TypeS[5];

        TypeS[0]=FlipL(Type) & 0xFF;
        TypeS[1]=(FlipL(Type) & 0xFF00) >> 8;
        TypeS[2]=(FlipL(Type) & 0xFF0000) >> 16;
        TypeS[3]=(FlipL(Type) & 0xFF000000) >> 24;
        TypeS[4]='\0';

        strcpy(DebugMsg,"Resource type \'");
        strcat(DebugMsg,TypeS);
        strcat(DebugMsg,"\' not found");
        Port_Puts(PUTS_DEBUG,"Res_GetOffset (res.c)",DebugMsg);
        return(0);
    }

    for(Count=MapOffset+TypeOffset+RefOffset;
        Count<MapOffset+TypeOffset+RefOffset+(NumRes*12);
        Count+=12)
    {
        if(FlipW(Mem_GetW(ResP,Count))==ID)
        {
            //Found the ID
            ResNameOffset=FlipW(Mem_GetW(ResP,Count+2));
            ResAttrib=ResP[Count+4];
            ResOffset=(ResP[Count+5]<<16)+(ResP[Count+6]<<8)+ResP[Count+7];

            return(DataOffset+ResOffset);
        }
    }

    {
        char DebugMsg[256], TypeS[5];

        TypeS[0]=FlipL(Type) & 0xFF;
        TypeS[1]=(FlipL(Type) & 0xFF00) >> 8;
        TypeS[2]=(FlipL(Type) & 0xFF0000) >> 16;
        TypeS[3]=(FlipL(Type) & 0xFF000000) >> 24;
        TypeS[4]='\0';

        sprintf(DebugMsg,"Couldn't find resource ID %d of type \'%s\'",ID,TypeS);
        Port_Puts(PUTS_DEBUG,"Res_GetOffset (res.c)",DebugMsg);
    }

    return(0);
}

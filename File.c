/*
  Mace - http://www.macehq.cx
  Copyright 1999-2004
  See the file README for more information
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "Globals.h"
#include "Port.h"
#include "File.h"
#include "Mem.h"

//Opens an AppleDouble file
U8 * File_Open (char * FileName, U32 ID)
{
    FILE * FileH;
    U8 * FileP;
    AppleDoubleHeader FileHeader;
    AppleDoubleEntry Section;
    int c;

    FileH=fopen(FileName,"rb");

    if(!FileH)
    {
        char Temp[256];
        sprintf(Temp,"Could not open file \"%s\"",FileName);
        Port_Puts(PUTS_DEBUG,"File_Open (file.c)",Temp);
        return(0);
    }

    if(ID==FILE_RAW) //Raw open. No Apple Double stuff.
    {
        //Get file size: Seek to end and use ftell() to get the position.
        fseek(FileH,0,SEEK_END);
        Section.Length=ftell(FileH);
        fseek(FileH,0,SEEK_SET);  
    
        FileP=malloc(Section.Length);

        if(FileP==0)
        {
            fclose(FileH);
            Port_Puts(PUTS_ERROR,"File_Open (file.c)","Could not allocate memory");
            return(0);
        }

        fread(FileP,1,Section.Length,FileH);
         
        fclose(FileH);
        return(FileP);
    }

    fread(&FileHeader,26,1,FileH);

    FileHeader.Magic=FlipL(FileHeader.Magic);
    FileHeader.Version=FlipL(FileHeader.Version);
    FileHeader.Entries=FlipW(FileHeader.Entries);

    if((FileHeader.Magic == 0x00051600 || FileHeader.Magic == 0x00051607) && FileHeader.Version == 0x00020000)
    {
        for(c=0;c<FileHeader.Entries;++c)
        {
            fread(&Section,12,1,FileH);

            Section.ID=FlipL(Section.ID);
            Section.Offset=FlipL(Section.Offset);
            Section.Length=FlipL(Section.Length);

            if(Section.ID == ID)
            {
                fseek(FileH,Section.Offset,SEEK_SET);

                FileP=malloc(Section.Length);

                if(FileP==0)
                {
                    fclose(FileH);
                    Port_Puts(PUTS_ERROR,"File_Open (file.c)","Could not allocate memory");
                    return(0);
                }

                fread(FileP,1,Section.Length,FileH);
                fclose(FileH);
                return(FileP);
            }
        }
    }

    fclose(FileH);
    Port_Puts(PUTS_DEBUG,"File_Open (file.c)","Invalid AppleDouble file");
    return(0);
}

void File_Close (U8 * FileP)
{
    if(FileP)
    {
        free(FileP);
        FileP=0;
    }

    return;
}

//FindP must be a pointer to Finder info
void File_Info (U8 * FindP, char * Type, char * Creator)
{
    Type[0]=Mem_GetL(FindP,0) & 0xFF;
    Type[1]=(Mem_GetL(FindP,0) & 0xFF00) >> 8;
    Type[2]=(Mem_GetL(FindP,0) & 0xFF0000) >> 16;
    Type[3]=(Mem_GetL(FindP,0) & 0xFF000000) >> 24;
    Type[4]='\0';

    Creator[0]=Mem_GetL(FindP,4) & 0xFF;
    Creator[1]=(Mem_GetL(FindP,4) & 0xFF00) >> 8;
    Creator[2]=(Mem_GetL(FindP,4) & 0xFF0000) >> 16;
    Creator[3]=(Mem_GetL(FindP,4) & 0xFF000000) >> 24;
    Creator[4]='\0';

    return;
}

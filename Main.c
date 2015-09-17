/*
  Mace - http://www.macehq.cx
  Copyright 1999-2004
  See the file README for more information
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Globals.h"
#include "Res.h"
#include "Port.h"
#include "File.h"
#include "Mem.h"
#include "Toolbox.h"
#include "cpu/cpu.h"

//Set our version
char MaceVer[]="Mace PRE-RELEASE";

int main(int argc, char *argv[])
{
    U8 * FindP;
    char Type[5], Creator[5], FileName[1024], Temp[256];

    Port_Start();
    CPU_Init();
    Mem_Init();

    //Don't use Port_Puts here. These messages are intended for the console,
    //and the prefix that Port_Puts uses looks stupid for these.
    puts(MaceVer);
    puts("Copyright 1999-2000");
    puts("");

    /* SET FILENAME OF MAC APPLICATION TO RUN

      Put the Mac app's resource fork file name
      into FileName. ARDI's Executor preceeds the
      resource fork filename with a %

      You must specify the file name of the Mac
      application's resource fork in the command line.
    */

    if(argc < 2) {
      sprintf(Temp,"Usage: %s <mac_application>", argv[0]);
      Port_Puts(PUTS_ERROR_NOQUIT,"main (main.c)",Temp);
      App_Exit2();
    }

    strcpy(FileName, argv[1]);
    sprintf(Temp,"Loading '%s'...", FileName);
    Port_Puts(PUTS_DEBUG,"main (main.c)",Temp);

    //Set our name to include the Mac application name
    sprintf(argv[0],"%s - %s", argv[0], FileName);

    FindP=File_Open(FileName,FILE_FINDER);

    if(!FindP)
    {
        /* If it's not an AppleDouble resource fork, then try to use it as
           a raw resource fork. The emulation will crash if it's not a raw
           resource fork.
        */

        Port_Puts(PUTS_ERROR_NOQUIT,"main (main.c)","File_Open(FileName,FILE_FINDER) failed!");
        Port_Puts(PUTS_DEBUG,"main (main.c)","Attempting to load raw resource fork...");
        
        ResP=File_Open(FileName,FILE_RAW);
        if(!ResP)
        {
            //If we can't open the file at all
            Port_Puts(PUTS_ERROR_NOQUIT,"main (main.c)","File_Open(FileName,FILE_RAW) failed!");
            Port_Puts(PUTS_ERROR_NOQUIT,"main (main.c)","Raw resource fork open failed! File doesn't exist or you don't have read access to it.");
            App_Exit2();
        }
    }
    else
    {
        File_Info(FindP, Type, Creator);

        sprintf(Temp,"Type '%s', Creator '%s'", Type, Creator);
        Port_Puts(PUTS_DEBUG,"main (main.c)",Temp);

        File_Close(FindP);

        ResP=File_Open(FileName,FILE_RES);
        if(!ResP)
        {
            Port_Puts(PUTS_ERROR_NOQUIT,"main (main.c)","File_Open(FileName,FILE_RES) failed!");
            App_Exit2();
        }
    }

    //Uses global ResP
    Mem_LoadApp();

    Toolbox_Init(); //This needs to be AFTER Mem_LoadApp()

    CPU_Start();

    File_Close(ResP);

    App_Exit();

    return(0);
}

void App_Exit(void) //Same as App_Exit2(), except that this calls setdis()
{
    setdis();
    App_Exit2();
    return;
}

void App_Exit2(void)
{
    Port_End();
    exit(0);
}

//Flip a Word (U16)
U16 FlipW(U16 a)
{
    return ((U8)(a>>8)+((U8)a<<8));
}

//Flip a Long (U32)
U32 FlipL(U32 a)
{
    return ((a>>24)+((U8)(a >> 16)<<8)+((U8)(a >> 8)<<16)+((U8)a<<24));
}

void HexW (U16 n, char * String)
{
    char Temp[5];

    Port_itoa(n,Temp,16);
    String[0]='\0';

    switch(strlen(Temp)) //Add 0's (make 4A into 004A)
    {
        //Note: The lack of break's is intentional

        case 1:
            strcat(String,"0");
        case 2:
            strcat(String,"0");
        case 3:
            strcat(String,"0");
    }

    strcat(String,Temp);

    return;
}

void HexL (U32 n, char * String)
{
    char Temp[9];

    Port_itoa(n,Temp,16);
    String[0]='\0';

    switch(strlen(Temp)) //Add 0's (Make 4A into 0000004A)
    {
        //Note: The lack of break's is intentional

        case 1:
            strcat(String,"0");
        case 2:
            strcat(String,"0");
        case 3:
            strcat(String,"0");
        case 4:
            strcat(String,"0");
        case 5:
            strcat(String,"0");
        case 6:
            strcat(String,"0");
        case 7:
            strcat(String,"0");
    }

    strcat(String,Temp);

    return;
}

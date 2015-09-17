/*
  Mace - http://www.macehq.cx
  Copyright 1999-2004
  See the file README for more information
*/

#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "Globals.h"
#include "Port.h"

HWND MainWnd=NULL;

void Port_Start(void)
{
    MainWnd=NULL;

    SetCursor(LoadCursor(NULL,IDC_WAIT));

    return;
}

void Port_End(void)
{
    SetCursor(LoadCursor(NULL,IDC_ARROW));

    getchar();
    return;
}

void Port_itoa(int value, char * str, int radix)
{
    itoa(value,str,radix);
    return;
}

void Port_Puts(int Type, char * Location, char * Message)
{
    UINT MsgIcon;
    char Location2[256], Msg[256];

    switch(Type)
    {
        case PUTS_ERROR:
        case PUTS_ERROR_NOQUIT:
            MsgIcon=MB_ICONERROR;
            strcpy(Msg,"FATAL ERROR! ");
            break;

        case PUTS_WARNING:
            MsgIcon=MB_ICONWARNING;
            strcpy(Msg,"Warning: ");
            break;

        case PUTS_INFO:
            MsgIcon=MB_ICONINFORMATION;
            Msg[0]='\0';
            break;

        case PUTS_DEBUG:
            printf("Debug - %s: %s\n", Location, Message);
            return;
    }

    sprintf(Location2, "Mace: %s",Location);

    strcat(Msg,Message);

    MessageBox(MainWnd, Msg, Location2, MB_OK | MsgIcon);

    if(Type==PUTS_ERROR)
        App_Exit();

    return;
}

void Port_NormalCursor(void)
{
    SetCursor(LoadCursor(NULL,IDC_ARROW));
    return;
}

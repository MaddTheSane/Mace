/*
  Mace - http://www.macehq.cx
  Copyright 1999-2004
  See the file README for more information
*/

#include "Globals.h"
#include "Port.h"

void Port_Start(void)
{
    return;
}

void Port_End(void)
{
    return;
}

void Port_itoa(int value, char * str, int radix)
{
    //Convert the integer "value" to the ascii string "str"
    
    if(radix==10) //Decimal
    {
        sprintf(str,"%d",value);
    }
    else if(radix==16) //Hex
    {
        sprintf(str,"%X",value);
    }
    else
    {
        Port_Puts(PUTS_ERROR,"Port_itoa (Port.c)","Unsupported base! Fix this!");
    }
    
    return;
}

void Port_Puts(int Type, char * Location, char * Message)
{
    char Temp[32];
    
    switch(Type)
    {
        case PUTS_ERROR:
        case PUTS_ERROR_NOQUIT:
            strcpy(Temp,"FATAL ERROR!");
            break;
        
        case PUTS_WARNING:
            strcpy(Temp,"Warning:");
            break;
            
        case PUTS_INFO:
            strcpy(Temp,"I");
            break;
            
        case PUTS_DEBUG:
            printf("Debug - %s: %s\n", Location, Message);
            return;
        
    	default:
    	    Temp[0]=0;
    	    break;
    }

    printf("%s - [%s]: %s\n", Temp, Location, Message);

    if(Type==PUTS_ERROR)
    	App_Exit();

    return;
}

void Port_NormalCursor(void)
{
    return;
}

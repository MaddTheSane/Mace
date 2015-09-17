/*
  Mace - http://www.macehq.cx
  Copyright 1999-2004
  See the file README for more information
*/

#ifndef PORT_H

#define PORT_H

void Port_Start(void);
void Port_End(void);

void Port_itoa(int value, char * str, int radix);

void Port_Puts(int Type, char * Location, char * Message);

void Port_NormalCursor(void);

//Types for Port_Puts

//Always display these:
#define PUTS_ERROR   		0
#define PUTS_ERROR_NOQUIT	1 //same as PUTS_ERROR, except it doesn't call App_Exit()
#define PUTS_WARNING 		2
#define PUTS_INFO    		3

//Don't always display these:
#define PUTS_DEBUG   		4

//Define OS


//There are several different defines that various compilers use for Win32
//Mace only responds to Win32
#ifdef _WIN32
#define Win32
#endif

#ifdef WIN32
#define Win32
#endif

#ifdef _Win32
#define Win32
#endif

#endif //PORT_H

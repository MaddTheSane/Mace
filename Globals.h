/*
  Mace - http://www.macehq.cx
  Copyright 1999-2004
  See the file README for more information
*/

#ifndef GLOBALS_H
#define GLOBALS_H

#ifndef _MaceTypes
#define _MaceTypes
typedef unsigned char U8;
typedef char S8;
typedef unsigned short U16;
typedef signed short S16;

//Note: on 64-bit machines, replace "long" with "int"
typedef unsigned long U32;
typedef signed long S32;
#endif

void App_Exit(void);
 //Same as App_Exit2(), except that this calls setdis()
void App_Exit2(void);
U16 FlipW(U16 a);
U32 FlipL(U32 a);
void HexW (U16 n, char * String);
void HexL (U32 n, char * String);

#endif //GLOBALS_H

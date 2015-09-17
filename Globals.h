/*
  Mace - http://www.macehq.cx
  Copyright 1999-2004
  See the file README for more information
*/

#ifndef GLOBALS_H
#define GLOBALS_H

#ifndef _MaceTypes
#define _MaceTypes

#include <stdint.h>

typedef uint8_t U8;
typedef int8_t S8;
typedef uint16_t U16;
typedef int16_t S16;

typedef uint32_t U32;
typedef int32_t S32;
#endif

void App_Exit(void);
 //Same as App_Exit2(), except that this calls setdis()
void App_Exit2(void);
U16 FlipW(U16 a);
U32 FlipL(U32 a);
void HexW (U16 n, char * String);
void HexL (U32 n, char * String);

#endif //GLOBALS_H

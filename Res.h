/*
  Mace - http://www.macehq.cx
  Copyright 1999-2004
  See the file README for more information
*/

#ifndef RES_H

#define RES_H

void Res_DecodeHeader(U32 * DataOffset, U32 * MapOffset, U32 * DataLen, U32 * MapLen);
U32 Res_GetOffset(U32 Type, U16 ID);

U8 * ResP; //Current resource file

#endif //RES_H

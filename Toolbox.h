/*
  Mace - http://www.macehq.cx
  Copyright 1999-2004
  See the file README for more information
*/

#ifndef TOOLBOX_H

#define TOOLBOX_H

U8 Toolbox_ALine(U16 inst); //Returns 1 if it handled the trap

/*
  Toolbox_Init() should be called before starting the CPU,
  but after initing the CPU and memory.
*/

void Toolbox_Init(void);

void Toolbox_BDCP(void);
void Toolbox_LoadSeg(void);
void Toolbox_InitGraf(void);
void Toolbox_InitDialogs(void);
void Toolbox_GetResource(void);
void Toolbox_GetNamedResource(void);
void Toolbox_ReleaseResource(void);
void Toolbox_Delay(void);

#endif

/*
  Mace - http://www.macehq.cx
  Copyright 1999-2004
  See the file README for more information
*/

#ifndef FILE_H

#define FILE_H

//Opens an AppleDouble file and returns a pointer to the requested segment (ID)
U8 * File_Open (char * FileName, U32 ID);

void File_Close (U8 * FileP);

//FileP must be a pointer to Finder info
void File_Info (U8 * FindP, char * Type, char * Creator);

typedef struct
{
    U32 Magic;
    U32 Version;
    U8 Filler[16];
    U16 Entries;
} AppleDoubleHeader;

typedef struct
{
    U32 ID;
    U32 Offset;
    U32 Length;
} AppleDoubleEntry;

#define FILE_DATA 1
#define FILE_RES 2
#define FILE_REAL_NAME 3
#define FILE_COMMENT 4
#define FILE_ICON_BW 5
#define FILE_ICON_COLOR 6
#define FILE_DATE 8
#define FILE_FINDER 9
#define FILE_MAC_INFO 10
#define FILE_PRODOS_INFO 11
#define FILE_MSDOS_INFO 12
#define FILE_SHORT_NAME 13
#define FILE_AFP_INFO 14
#define FILE_DIR_ID 15

//Just for Mace. Instructs File_Open to ignore AppleDouble stuff and open raw.
#define FILE_RAW 255

#endif

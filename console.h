/**
 **  SuperJer Console
 **
 **/

#ifndef __SJCONSOLE_H__
#define __SJCONSOLE_H__

#include "SDL/SDL.h"
#include "font.h"

typedef struct
{
  char *buf[200];
  int size[200];
} SJC_t;

extern SJC_t SJC;

void SJC_Init();
void SJC_Put(char c);
void SJC_Write(const char *s,...);
void SJC_Rub();
void SJC_Submit();

#endif


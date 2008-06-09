/**
 **  SuperJer Console
 **
 **/

#include "console.h"


SJC_t SJC = {{0},{0}};


void SJC_Init()
{
  //int i;
  //for(i=0;i<200;i++)
  //{
  //  SJC.buf[i] = NULL;
  //  SJC.size[i] = 0;
  //}
}


void SJC_Put(char c)
{
  int n;
  if( SJC.buf[0]==NULL || strlen(SJC.buf[0])>=SJC.size[0]-1 )
  {
    fprintf(stderr,"Console: realloc'ing from %d [ADDR:%x]\n",SJC.size[0],SJC.buf[0]);
    SJC.buf[0] = realloc( SJC.buf[0], SJC.size[0]+32 );
    if( SJC.size[0]==0 )
      SJC.buf[0][0] = '\0';
    SJC.size[0] += 32;
  }
  n = strlen(SJC.buf[0]);
  SJC.buf[0][n] = c;
  SJC.buf[0][n+1] = '\0';
}


void SJC_Rub()
{
  int n;
  if( SJC.buf[0]!=NULL )
  {
    n = strlen(SJC.buf[0]);
    if( n )
      SJC.buf[0][n-1] = '\0';
  }
}


void SJC_Submit()
{
  fprintf(stderr,"Console: free'ing %d bytes [ADDR:%x]\n",SJC.size[199],SJC.buf[199]);
  free(SJC.buf[199]);
  memmove(SJC.buf+1,SJC.buf,sizeof(char*)*199);
  memmove(SJC.size+1,SJC.size,sizeof(int)*199);
  SJC.buf[0] = NULL;
  SJC.size[0] = 0;
}



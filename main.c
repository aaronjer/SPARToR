/**
 **  SPARToR [CORE]
 **  A.K.A SPARToR 4
 **
 **  100% SDL+OpenGL now
 **  Network-CORE-centered
 **
 **/


#include "SDL/SDL.h"
#include "SDL/SDL_net.h"


//macros
#define MIN(a,b) ((a)<(b)?(a):(b))
#define MAX(a,b) ((a)>(b)?(a):(b))
#define SWAP(t,a,b) {t SWAP_temp = a;a = b;b = SWAP_temp;}


//globals
SDL_Surface *screen;


//prototypes
void setvideo(int w,int h);
void cleanup();

int main(int argc,char **argv)
{
  SDL_Event event;
  const SDL_VideoInfo *vidinfo;

  if( SDL_Init(SDL_INIT_TIMER|SDL_INIT_AUDIO|SDL_INIT_VIDEO)<0 || !SDL_GetVideoInfo() )
  {
    fprintf(stderr,"SDL_Init: %s\n",SDL_GetError());
    exit(-1);
  }
  if( SDLNet_Init()<0 )
  {
    fprintf(stderr,"SDLNet_Init: %s\n",SDL_GetError());
    exit(-2);
  }
  setvideo(800,600);
  SDL_WM_SetCaption("[CORE]",NULL);
  vidinfo = SDL_GetVideoInfo();

  //main loop
  while(1)
  {
    while( SDL_PollEvent(&event) ) switch(event.type)
    {
      case SDL_VIDEORESIZE:
        setvideo(event.resize.w,event.resize.h);
        break;
      case SDL_QUIT:
        cleanup();
        break;
    }
    //chill for a bit
    SDL_Delay(10);
  }
  return 0;
}


void setvideo(int w,int h)
{
  screen = SDL_SetVideoMode(w,h,SDL_GetVideoInfo()->vfmt->BitsPerPixel,SDL_RESIZABLE|SDL_DOUBLEBUF);
}


void cleanup()
{
  SDL_Quit();
  exit(0);
}

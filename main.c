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
#include "main.h"
#include "font.h"
#include "console.h"
#include "net.h"
#include "host.h"
#include "client.h"


//globals
SDL_Surface *screen;
Uint32 ticks = 0;


int main(int argc,char **argv)
{
  SDL_Event event;
  const SDL_VideoInfo *vidinfo;
  int sym;

  printf("SDLNET_MAX_UDPCHANNELS=%d\n",SDLNET_MAX_UDPCHANNELS);

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
  setvideo(640,480);
  SDL_WM_SetCaption("SPARToR CORE",NULL);
  vidinfo = SDL_GetVideoInfo();

  SJF_Init();

  //main loop
  while(1)
  {
    ticks = SDL_GetTicks();
    while( SDL_PollEvent(&event) ) switch(event.type)
    {
      case SDL_VIDEOEXPOSE:
        break;
      case SDL_VIDEORESIZE:
        setvideo(event.resize.w,event.resize.h);
        break;
      case SDL_KEYDOWN:
        sym = event.key.keysym.sym;
        switch(sym)
        {
          case SDLK_ESCAPE:
            cleanup();
            break;
          default:
            if(sym>31 && sym<128)
              SJC_Put((char)sym);
            else if(sym==SDLK_RETURN)
            {
              SJC_Submit();
              command(SJC.buf[1]);
            }
            else if(sym==SDLK_BACKSPACE)
              SJC_Rub();
            break;
        }
        break;
      case SDL_QUIT:
        cleanup();
    }
    if(hostsock) host();
    if(clientsock) client();
    render();
    SDL_Delay(10);
  }
  return 0;
}


void render()
{
  const SDL_VideoInfo *vidinfo;
  SDL_Rect rect;
  Uint32 x,y,w,h;
  int i;

  vidinfo = SDL_GetVideoInfo();
  w = vidinfo->current_w;
  h = vidinfo->current_h;

  rect.x = 0;
  rect.y = 0;
  rect.w = w;
  rect.h = h;
  SDL_FillRect(screen,&rect,0x000088);

  //display console
  x = 10;
  y = 200;
  if( (ticks/200)%2 )
    SJF_DrawChar(screen, x+SJF_TextExtents(SJC.buf[0]), y, '_');
  for(i=0;i<20;i++)
  {
    if(SJC.buf[i])
      SJF_DrawText(screen,x,y,SJC.buf[i]);
    y -= 10;
  }

  SDL_Flip(screen);
}


void setvideo(int w,int h)
{
  screen = SDL_SetVideoMode(w,h,SDL_GetVideoInfo()->vfmt->BitsPerPixel,SDL_RESIZABLE|SDL_DOUBLEBUF);
}


void cleanup()
{
  SDLNet_Quit();
  SDL_Quit();
  exit(0);
}



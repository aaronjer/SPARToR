
#ifndef __MAIN_H__
#define __MAIN_H__

#define VERSION "0.1a"


//macros
#define MIN(a,b) ((a)<(b)?(a):(b))
#define MAX(a,b) ((a)>(b)?(a):(b))
#define SWAP(t,a,b) {t SWAP_temp = a;a = b;b = SWAP_temp;}
#define TRY do{
#define HARDER }while(0);

//externs
extern SDL_Surface *screen;
extern Uint32 ticks;

//prototypes
void render();
void setvideo(int w,int h);
void command(const char *s);
void cleanup();


#endif


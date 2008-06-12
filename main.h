
#ifndef __MAIN_H__
#define __MAIN_H__

#define VERSION "0.1a"


//macros
#define MIN(a,b) ((a)<(b)?(a):(b))
#define MAX(a,b) ((a)>(b)?(a):(b))
#define SWAP(t,a,b) {t SWAP_temp = a;a = b;b = SWAP_temp;}
#define TRY do{
#define HARDER }while(0);


//obj flags
#define OBJF_POS 0x00000001
#define OBJF_VIS 0x00000002


typedef struct{
  char cmd;
  char mousehi;
  char mousex;
  char mousey;
} FCMD_t;

typedef struct{
  short type;
  short flags;
  void *data;
} FOBJ_t;

typedef struct{
  FCMD_t *cmds;
  FOBJ_t *objs;
} FRAME_t;


//externs
extern size_t maxfr;
extern size_t maxobjs;
extern FRAME_t *fr;
extern SDL_Surface *screen;
extern Uint32 ticks;

//prototypes
void render();
void setvideo(int w,int h);
void command(const char *s);
void cleanup();


#endif



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
  size_t size;
  void *data;
} OBJ_t;

typedef struct{
  FCMD_t *cmds;
  OBJ_t *objs;
} FRAME_t;


typedef struct{
  float x,y,z;
} V;

//externs
extern size_t maxframes;
extern size_t maxobjs;

extern FRAME_t *fr;
extern Uint32 metafr;
extern Uint32 curfr;
extern Uint32 drawnfr;
extern Uint32 hotfr;
extern int creatables;

extern SDL_Surface *screen;
extern Uint32 ticks;

//prototypes
void advance();
void render();
void setvideo(int w,int h);
void command(const char *s);
void cleanup();
V *flexpos(OBJ_t *o);


#endif


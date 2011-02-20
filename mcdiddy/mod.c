/**
 **  McDiddy's Game
 **  Implementation example for the SPARToR Network Game Engine
 **  Copyright (c) 2010-2011  Jer Wilson
 **
 **  See COPYING for details.
 **
 **  http://www.superjer.com/
 **  http://www.spartor.com/
 **  http://github.com/superjer/SPARToR
 **/


#include "mod.h"


GLuint textures[TEX_COUNT];

INPUTNAME_t inputnames[] = {{"left" ,CMDT_1LEFT ,CMDT_0LEFT },
                            {"right",CMDT_1RIGHT,CMDT_0RIGHT},
                            {"up"   ,CMDT_1UP   ,CMDT_0UP   },
                            {"down" ,CMDT_1DOWN ,CMDT_0DOWN },
                            {"fire" ,CMDT_1FIRE ,CMDT_0FIRE },
                            {"jump" ,CMDT_1JUMP ,CMDT_0JUMP }};
int numinputnames = (sizeof inputnames) / (sizeof *inputnames);


static int    binds_size = 0;
static struct {
  short hash;
  char  cmd;
  char  _pad;
}            *binds;

static CB *hack_map; //FIXME remove hack
static CB *hack_dmap;


//FIXME REMOVE! change local player model
extern int    setmodel;
//
//FIXME REMOVE! force amigo to flykick
extern int    flykick;
//


void mod_setup(Uint32 setupfr)
{
  //default key bindings
  #define MAYBE_BIND(dev,sym,cmd)         \
    mod_keybind(dev,sym,0,CMDT_0 ## cmd); \
    mod_keybind(dev,sym,1,CMDT_1 ## cmd);
  MAYBE_BIND(INP_KEYB,SDLK_LEFT ,LEFT );
  MAYBE_BIND(INP_KEYB,SDLK_RIGHT,RIGHT);
  MAYBE_BIND(INP_KEYB,SDLK_UP   ,UP   );
  MAYBE_BIND(INP_KEYB,SDLK_DOWN ,DOWN );
  MAYBE_BIND(INP_KEYB,SDLK_z    ,JUMP );
  MAYBE_BIND(INP_KEYB,SDLK_x    ,FIRE );
  MAYBE_BIND(INP_JAXN,0         ,LEFT ); MAYBE_BIND(INP_JAXN,3         ,LEFT );
  MAYBE_BIND(INP_JAXP,0         ,RIGHT); MAYBE_BIND(INP_JAXP,3         ,RIGHT);
  MAYBE_BIND(INP_JAXN,1         ,UP   ); MAYBE_BIND(INP_JAXN,4         ,UP   );
  MAYBE_BIND(INP_JAXP,1         ,DOWN ); MAYBE_BIND(INP_JAXP,4         ,DOWN );
  MAYBE_BIND(INP_JBUT,1         ,JUMP );
  MAYBE_BIND(INP_JBUT,2         ,FIRE );
  #undef MAYBE_BIND

  //make the mother object
  fr[setupfr].objs[0] = (OBJ_t){ OBJT_MOTHER, 0, 0, sizeof(MOTHER_t), malloc(sizeof(MOTHER_t)) };
  *(MOTHER_t *)fr[setupfr].objs[0].data = (MOTHER_t){0};

  //make default context object (map)
  fr[setupfr].objs[1] = (OBJ_t){ OBJT_CONTEXT, 0, 0, sizeof(CONTEXT_t), malloc(sizeof(CONTEXT_t)) };
  CONTEXT_t *co = fr[setupfr].objs[1].data;
  co->blocksize = 16;
  co->x = 100;
  co->y =  15;
  co->z =   1;
  int volume = co->x * co->y * co->z;
  co->map  = hack_map  = malloc( (sizeof *co->map ) * volume ); //FIXME remove hack
  co->dmap = hack_dmap = malloc( (sizeof *co->dmap) * volume );
  memset( co->map,  0, (sizeof *co->map ) * volume );
  memset( co->dmap, 0, (sizeof *co->dmap) * volume );
  int i;
  for( i=0; i<volume; i++ ) {
    co->map[ i].data[0] = i%13;
    co->dmap[i].flags   = CBF_NULL;
  }

  //make some dummys
  #define MAYBE_A_DUMMY(i,x,y,w,h) {                                                                             \
    DUMMY_t *du;                                                                                                 \
    fr[setupfr].objs[i+20].type = OBJT_DUMMY;                                                                    \
    fr[setupfr].objs[i+20].flags = OBJF_POS|OBJF_VEL|OBJF_HULL|OBJF_VIS|OBJF_PLAT|OBJF_CLIP|OBJF_BNDX|OBJF_BNDY; \
    fr[setupfr].objs[i+20].size = sizeof *du;                                                                    \
    du = fr[setupfr].objs[i+20].data = malloc(sizeof *du);                                                       \
    du->pos = (V){x*8,y*8,0.0f};                                                                                 \
    du->vel = (V){0.0f,0.0f,0.0f};                                                                               \
    du->hull[0] = (V){-w*8,-h*8,0.0f};                                                                           \
    du->hull[1] = (V){ w*8, h*8,0.0f};                                                                           \
    du->model = 0;                                                                                               }
  MAYBE_A_DUMMY( 1,  1, 25,1,5);
  MAYBE_A_DUMMY( 2,  3, 25,1,5);
  MAYBE_A_DUMMY( 3,  5, 25,1,5);
  MAYBE_A_DUMMY( 4,  7, 25,1,5);
  MAYBE_A_DUMMY( 5,  9, 25,1,5);
  MAYBE_A_DUMMY( 6, 11, 25,1,5);
  MAYBE_A_DUMMY( 7, 13, 25,1,5);

  MAYBE_A_DUMMY( 8,  7, 15,7,1);
  MAYBE_A_DUMMY( 9, 22, 15,2,1);
  MAYBE_A_DUMMY(10, 32, 15,2,1);
  MAYBE_A_DUMMY(11, 44, 15,4,1);

  MAYBE_A_DUMMY(12, 21, 25,1,3);
  MAYBE_A_DUMMY(13, 23, 25,1,3);
  MAYBE_A_DUMMY(14, 31, 25,1,3);
  MAYBE_A_DUMMY(15, 33, 25,1,3);
  MAYBE_A_DUMMY(16, 41, 25,1,3);
  MAYBE_A_DUMMY(17, 43, 25,1,3);
  MAYBE_A_DUMMY(18, 45, 25,1,3);
  MAYBE_A_DUMMY(19, 47, 25,1,3);

  MAYBE_A_DUMMY(20,  3,-25,1,1);
  MAYBE_A_DUMMY(21,  3,-20,1,1);
  MAYBE_A_DUMMY(22,  3,-15,1,1);
  MAYBE_A_DUMMY(23,  3,-10,1,1);
  MAYBE_A_DUMMY(24,  5,-15,1,1);
  MAYBE_A_DUMMY(25,  9,-15,1,1);
  MAYBE_A_DUMMY(26, 43,-20,1,1);
  MAYBE_A_DUMMY(27, 43,-15,1,1);
  MAYBE_A_DUMMY(28, 45,-25,1,1);
  MAYBE_A_DUMMY(29, 45,-20,1,1);
  MAYBE_A_DUMMY(30, 45,-15,1,1);
  #undef MAYBE_A_DUMMY

  fr[setupfr+1].cmds[0].flags |= CMDF_NEW; //server is a client
}


void mod_recvobj(OBJ_t *o)
{
  CONTEXT_t *co;

  switch( o->type ) {
    case OBJT_CONTEXT:
      co = o->data;
      co->map  = hack_map; // FIXME: horrible hack FOR NOW!
      co->dmap = hack_dmap;//        when receiving map, use data we already have
      SJC_Write("mod_recvobj(): reusing map data before network connect (hack)");
      break;
  }
}

void mod_setvideo(int w,int h)
{
  mod_loadsurfs(0);
}


void mod_quit()
{
  mod_loadsurfs(1);
}


void mod_keybind(int device,int sym,int press,char cmd)
{
  int i;
  short hash = press<<15 | device<<8 | sym;

  for(i=0; i<binds_size; i++)
    if( binds[i].hash==hash || binds[i].hash==0 )
      break;
  if(i==binds_size) {
    binds = realloc(binds,sizeof(*binds)*(binds_size+32));
    memset(binds+binds_size,0,sizeof(*binds)*32);
    binds_size += 32;
  }
  binds[i].hash = hash;
  binds[i].cmd = cmd;
}


char mod_key2cmd(int device,int sym,int press)
{
  int i;
  short hash = press<<15 | device<<8 | sym;

  for(i=0; i<binds_size; i++)
    if( binds[i].hash==hash )
      return binds[i].cmd;
  return 0;
}


int mod_command(char *q)
{
  TRY
    if( q==NULL ){
      ;
    }else if( strcmp(q,"model")==0 ){
      setmodel = atoi(strtok(NULL," ")); // FIXME: lame hack
      return 0;
    }
//FIXME REMOVE: force amigo to flykick
else if( strcmp(q,"flykick")==0 ){ flykick = 1; return 0; }
//
  HARDER
  return 1;
}


void mod_loadsurfs(int quit)
{
  SDL_Surface *surf;

  // free existing textures
  glDeleteTextures(TEX_COUNT,textures);

  if( quit ) return;

  glPixelStorei(GL_UNPACK_ALIGNMENT,4);
  glGenTextures(TEX_COUNT,textures);

  #define LOADTEX(tex, file) {                                                                                            \
    glBindTexture(GL_TEXTURE_2D,textures[tex]);                                                                           \
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);                                                      \
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);                                                      \
    if( (surf = IMG_Load(file)) ) {                                                                                       \
     glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surf->w, surf->h, 0, SJDL_GLFormatOf(surf), GL_UNSIGNED_BYTE, surf->pixels); \
     SDL_FreeSurface(surf);                                                                                               \
    }                                                                                                                     }
  LOADTEX( TEX_PLAYER, MODNAME "/images/player.png" );
  LOADTEX( TEX_WORLD,  MODNAME "/images/world.png"  );
  LOADTEX( TEX_AMIGO,  MODNAME "/images/amigo.png"  );
  #undef LOADTEX
}


void mod_predraw(SDL_Surface *screen,Uint32 vidfr)
{
  //draw background
  int i=0,j;
  while(i<23) {
    int step = (i<6||i==10||i==15) ? 1 : 4;
    for(j=0;j<15;j++)
      SJGL_BlitScaled(textures[TEX_WORLD], &(SDL_Rect){80,16,step*16,16}, &(SDL_Rect){i*16,j*16,0,0}, scale, 0);
    i += step;
  }
  for(i=8;i<15;i++)
    SJGL_BlitScaled(textures[TEX_WORLD], &(SDL_Rect){ 80,64,16,16}, &(SDL_Rect){ 9*16, i*16,0,0}, scale, 0); //low tall
  SJGL_BlitScaled(  textures[TEX_WORLD], &(SDL_Rect){ 96,48,16,16}, &(SDL_Rect){ 9*16, 7*16,0,0}, scale, 0); //corner nw
  for(i=10;i<16;i++)
    SJGL_BlitScaled(textures[TEX_WORLD], &(SDL_Rect){112,32,16,16}, &(SDL_Rect){ i*16, 7*16,0,0}, scale, 0); //long horiz
  SJGL_BlitScaled(  textures[TEX_WORLD], &(SDL_Rect){112,80,16,16}, &(SDL_Rect){11*16, 7*16,0,0}, scale, 0); //t-piece
  SJGL_BlitScaled(  textures[TEX_WORLD], &(SDL_Rect){128,80,16,16}, &(SDL_Rect){16*16, 7*16,0,0}, scale, 0); //corner se
  for(i=0;i<7;i++)
    SJGL_BlitScaled(textures[TEX_WORLD], &(SDL_Rect){ 80,64,16,16}, &(SDL_Rect){16*16, i*16,0,0}, scale, 0); //high tall
  SJGL_BlitScaled(  textures[TEX_WORLD], &(SDL_Rect){ 80,64,16,16}, &(SDL_Rect){11*16, 6*16,0,0}, scale, 0); //little pipe
  SJGL_BlitScaled(  textures[TEX_WORLD], &(SDL_Rect){ 80,48,16,16}, &(SDL_Rect){11*16, 5*16,0,0}, scale, 0); //pipe end
  SJGL_BlitScaled(  textures[TEX_WORLD], &(SDL_Rect){144,16,48,16}, &(SDL_Rect){12*16, 8*16,0,0}, scale, 0); //shadow
  SJGL_BlitScaled(  textures[TEX_WORLD], &(SDL_Rect){ 80,32,16,16}, &(SDL_Rect){ 6*16, 7*16,0,0}, scale, 0); //end cap
}


void mod_draw(SDL_Surface *screen,int objid,OBJ_t *o)
{
  switch(o->type) {
    case OBJT_PLAYER:         obj_player_draw(     objid, o );     break;
    case OBJT_GHOST:          obj_ghost_draw(      objid, o );     break;
    case OBJT_BULLET:         obj_bullet_draw(     objid, o );     break;
    case OBJT_SLUG:           obj_slug_draw(       objid, o );     break;
    case OBJT_DUMMY:          obj_dummy_draw(      objid, o );     break;
    case OBJT_AMIGO:          obj_amigo_draw(      objid, o );     break;
    case OBJT_AMIGOSWORD:     obj_amigosword_draw( objid, o );     break;
  }
}


void mod_adv(int objid,Uint32 a,Uint32 b,OBJ_t *oa,OBJ_t *ob)
{
  switch( ob->type ) {
    case OBJT_MOTHER:
      assert(ob->size==sizeof(MOTHER_t));
      assert(objid==0);
      obj_mother_adv( objid, a, b, oa, ob );
      break;
    case OBJT_GHOST:
      assert(ob->size==sizeof(GHOST_t));
      obj_ghost_adv( objid, a, b, oa, ob );
      break;
    case OBJT_DUMMY:
      assert(ob->size==sizeof(DUMMY_t));
      obj_dummy_adv( objid, a, b, oa, ob );
      break;
    case OBJT_PLAYER:
      assert(ob->size==sizeof(PLAYER_t));
      obj_player_adv( objid, a, b, oa, ob );
      break;
    case OBJT_BULLET:
      assert(ob->size==sizeof(BULLET_t));
      obj_bullet_adv( objid, a, b, oa, ob );
      break;
    case OBJT_SLUG:
      assert(ob->size==sizeof(SLUG_t));
      obj_slug_adv( objid, a, b, oa, ob );
      break;
    case OBJT_AMIGO:
      assert(ob->size==sizeof(AMIGO_t));
      obj_amigo_adv( objid, a, b, oa, ob );
      break;
    case OBJT_AMIGOSWORD:
      assert(ob->size==sizeof(AMIGOSWORD_t));
      obj_amigosword_adv( objid, a, b, oa, ob );
      break;
  } //end switch ob->type
}


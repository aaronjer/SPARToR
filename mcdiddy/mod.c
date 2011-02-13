/**
 **  McDiddy's Game
 **  Implementation example for the SPARToR Network Game Engine
 **  Copyright (C) 2010-2011  Jer Wilson
 **
 **  See COPYING for details.
 **
 **  http://www.superjer.com/
 **  http://www.spartor.com/
 **  http://github.com/superjer/SPARToR
 **/


#include <GL/glew.h>
#include "SDL.h"
#include "main.h"
#include "console.h"
#include "net.h"
#include "video.h"
#include "input.h"
#include "patt.h"
#include "mod.h"
#include "mod_private.h"
#include <math.h>


INPUTNAME_t inputnames[] = {{"left" ,CMDT_1LEFT ,CMDT_0LEFT },
                            {"right",CMDT_1RIGHT,CMDT_0RIGHT},
                            {"up"   ,CMDT_1UP   ,CMDT_0UP   },
                            {"down" ,CMDT_1DOWN ,CMDT_0DOWN },
                            {"fire" ,CMDT_1FIRE ,CMDT_0FIRE },
                            {"jump" ,CMDT_1JUMP ,CMDT_0JUMP }};
int numinputnames = (sizeof inputnames) / (sizeof *inputnames);


static int    setmodel = -1;
static int    camx = 0;
static int    camy = 0;

static int    binds_size = 0;
static struct {
  short hash;
  char  cmd;
  char  _pad;
}            *binds;



enum { TEX_PLAYER = 0,
       TEX_WORLD,
       TEX_AMIGO,
       TEX_COUNT };
GLuint textures[TEX_COUNT];


void mod_setup(Uint32 setupfr) {

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
  fr[setupfr].objs[0] = (OBJ_t){OBJT_MOTHER,0,0,malloc(sizeof(MOTHER_t))};
  *(MOTHER_t *)fr[setupfr].objs[0].data = (MOTHER_t){0,0};

  //make some dummys
  #define MAYBE_A_DUMMY(i,x,y,w,h) {                                                         \
    DUMMY_t *du;                                                                             \
    fr[setupfr].objs[i+20].type = OBJT_DUMMY;                                                \
    fr[setupfr].objs[i+20].flags = OBJF_POS|OBJF_VEL|OBJF_HULL|OBJF_VIS|OBJF_PLAT|OBJF_CLIP; \
    fr[setupfr].objs[i+20].size = sizeof *du;                                                \
    du = fr[setupfr].objs[i+20].data = malloc(sizeof *du);                                   \
    du->pos = (V){x*8,y*8,0.0f};                                                             \
    du->vel = (V){0.0f,0.0f,0.0f};                                                           \
    du->hull[0] = (V){-w*8,-h*8,0.0f};                                                       \
    du->hull[1] = (V){ w*8, h*8,0.0f};                                                       \
    du->model = 0;                                                                           }
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

void mod_setvideo(int w,int h) {
  mod_loadsurfs(0);
}

void mod_quit() {
  mod_loadsurfs(1);
}


void mod_keybind(int device,int sym,int press,char cmd) {
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

char mod_key2cmd(int device,int sym,int press) {
  int i;
  short hash = press<<15 | device<<8 | sym;

  for(i=0; i<binds_size; i++)
    if( binds[i].hash==hash )
      return binds[i].cmd;
  return 0;
}


int mod_command(char *q) {
  TRY
    if( q==NULL ){
      ;
    }else if( strcmp(q,"model")==0 ){
      setmodel = atoi(strtok(NULL," ")); // FIXME: lame hack
      return 0;
    }
  HARDER
  return 1;
}

void mod_loadsurfs(int quit) {
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

void mod_predraw(SDL_Surface *screen,Uint32 vidfr) {
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

void mod_draw(SDL_Surface *screen,int objid,OBJ_t *o) {
  SDL_Rect drect;

  switch(o->type)
  {
    case OBJT_PLAYER: {
      PLAYER_t *pl = o->data;
      int gunshift;
      if( pl->goingu ) gunshift = 96;
      if( pl->goingd ) gunshift = 48;
      if( pl->goingu==pl->goingd ) gunshift = 0;
      drect = (SDL_Rect){(pl->pos.x-10),(pl->pos.y-15),0,0};
      int z = drect.y + pl->hull[1].y;
      if( pl->facingr ) {
        if( pl->model==4 ) //girl hair
          SJGL_BlitScaled(textures[TEX_PLAYER], &(SDL_Rect){ 80,120,20,15},
                                                &(SDL_Rect){drect.x-4,drect.y+pl->gundown/7,0,0}, scale, z);
        SJGL_BlitScaled(textures[TEX_PLAYER], &(SDL_Rect){ 0, 0+pl->model*30,20,30}, &drect, scale, z);
        drect = (SDL_Rect){(pl->pos.x- 5-pl->gunback),(pl->pos.y-10+pl->gundown/5),0,0};
        if( !pl->stabbing ) //gun
          SJGL_BlitScaled(textures[TEX_PLAYER], &(SDL_Rect){ 0+gunshift,150,24,21}, &drect, scale, z);
      } else {
        if( pl->model==4 ) //girl hair
          SJGL_BlitScaled(textures[TEX_PLAYER], &(SDL_Rect){100,120,20,15},
                                                &(SDL_Rect){drect.x+4,drect.y+pl->gundown/7,0,0}, scale, z);
        SJGL_BlitScaled(textures[TEX_PLAYER], &(SDL_Rect){20, 0+pl->model*30,20,30}, &drect, scale, z);
        drect = (SDL_Rect){(pl->pos.x-19+pl->gunback),(pl->pos.y-10+pl->gundown/5),0,0};
        if( !pl->stabbing ) //gun
          SJGL_BlitScaled(textures[TEX_PLAYER], &(SDL_Rect){24+gunshift,150,24,21}, &drect, scale, z);
      }
      if( pl->stabbing ) { //up/down stabbing
        drect = (SDL_Rect){pl->pos.x-2,pl->pos.y-(pl->stabbing<0?30:0),0,0};
        SJGL_BlitScaled(textures[TEX_PLAYER], &(SDL_Rect){148+(pl->stabbing<0?5:0),150,5,27}, &drect, scale, z);
      }
      break;
    }
    case OBJT_BULLET: {
      BULLET_t *bu = o->data;
      SJGL_BlitScaled(textures[TEX_PLAYER], &(SDL_Rect){144,150,4,4},
                                            &(SDL_Rect){bu->pos.x-2, bu->pos.y-2, 4, 4}, scale,NATIVEH);
      break;
    }
    case OBJT_SLUG: {
      SLUG_t *sl = o->data;
      SJGL_BlitScaled(textures[TEX_PLAYER], &(SDL_Rect){(sl->vel.x>0?20:0)+(sl->dead?40:0),177,20,16},
                                            &(SDL_Rect){sl->pos.x-10,sl->pos.y-8,0,0}, scale, sl->pos.y);
      break;
    }
    case OBJT_DUMMY: {
      DUMMY_t *du = o->data;
      drect = (SDL_Rect){du->pos.x    +du->hull[0].x, du->pos.y    +du->hull[0].y,
                         du->hull[1].x-du->hull[0].x, du->hull[1].y-du->hull[0].y};
      Sint16 offs = drect.w==drect.h ? 48 : 0;
      if( drect.w > drect.h ) while( drect.w>0 && drect.w<400 ) {
        SJGL_BlitScaled(textures[TEX_WORLD], &(SDL_Rect){0+offs,16,16,16},
                                             &(SDL_Rect){drect.x,drect.y,drect.w,drect.h}, scale, 0);
        drect.x += 16;
        drect.w -= 16;
        offs = drect.w==16 ? 32 : 16;
      } else                  while( drect.h>0 && drect.h<400 ) {
        SJGL_BlitScaled(textures[TEX_WORLD], &(SDL_Rect){48,0+offs,16,16},
                                             &(SDL_Rect){drect.x,drect.y,drect.w,drect.h}, scale, 0);
        drect.y += 16;
        drect.h -= 16;
        offs = drect.h==16 ? 32 : 16;
      }
      break;
    }
    case OBJT_AMIGO: {
      typedef struct { int x, y, w, h, dx, dy; } XSPR;
      XSPR tip = {0}; // extra sprite for tip of sword
      AMIGO_t *am = o->data;
      int w = 50;
      int h = 50;
      int x = 0,y = 0;
      int z = am->pos.y-32 + am->hull[1].y;
      switch( am->state ) {
        case AMIGO_HELLO:
          x = (am->statetime/30) * 50; //                       x   y  w  h  dx  dy
          if(      am->statetime<30 ) { x =   0; tip = (XSPR){0}; }
          else if( am->statetime<60 ) { x =  50; tip = (XSPR){0}; }
          else if( am->statetime<90 ) { x = 100; tip = (XSPR){0}; }
          else                        { x = 150; tip = (XSPR){220,230,10,10,  8,-10}; }
          y = 150;
          break;
        case AMIGO_COOLDOWN:
          tip = (XSPR){210,230,10,10, 20,-10};
          break;
        case AMIGO_JUMP:
          tip = (XSPR){240,230,10,10, 33,-10};
          x = 0;
          y = 50;
          break;
        case AMIGO_SLASH: //                                    x   y  w  h  dx  dy
          if(      am->statetime<20 ) { x =  50; tip = (XSPR){220,240,30,10,-30, 42}; }
          else if( am->statetime<25 ) { x = 100; tip = (XSPR){220,230,10,10,  6,-10}; }
          else                        { x = 150; tip = (XSPR){230,230,10,10, 32,-10}; }
          break;
        case AMIGO_FLYKICK:
          break;
        case AMIGO_DASH:
          tip = (XSPR){210,250,40, 6,-40, 40};
          x = 100;
          y = 50;
          break;
      }
      SJGL_BlitScaled(textures[TEX_AMIGO], &(SDL_Rect){     x,     y,     w,     h },
                                           &(SDL_Rect){ am->pos.x-35,        am->pos.y-32,        0, 0 }, scale, z);
      SJGL_BlitScaled(textures[TEX_AMIGO], &(SDL_Rect){ tip.x, tip.y, tip.w, tip.h },
                                           &(SDL_Rect){ am->pos.x-35+tip.dx, am->pos.y-32+tip.dy, 0, 0 }, scale, z);
      break;
    }
  }
}

void mod_adv(Uint32 objid,Uint32 a,Uint32 b,OBJ_t *oa,OBJ_t *ob) {
  #define MKOBJ( ptr, t, f )                            \
    t ## _t *ptr;                                       \
    slot0 = findfreeslot(b);                            \
    fr[b].objs[slot0].type = OBJT_ ## t;                \
    fr[b].objs[slot0].flags = f;                        \
    fr[b].objs[slot0].size = sizeof *ptr;               \
    ptr = fr[b].objs[slot0].data = malloc(sizeof *ptr);
  int i, j;
  int slot0;
  switch( ob->type ) {
    case OBJT_MOTHER:
      //the mother object is sort of the root of the object tree... only not... and there's no object tree
      for(i=0;i<maxclients;i++) {
        if( fr[b].cmds[i].flags & CMDF_NEW ) {
          for(j=0;j<maxobjs;j++)
            if( fr[b].objs[j].type==OBJT_GHOST && ((GHOST_t *)fr[b].objs[j].data)->client==i )
              SJC_Write( "%d: Client %i already has a ghost at obj#%d!", hotfr, i, j );

          MKOBJ( gh, GHOST, OBJF_POS );
          int ghostslot = slot0;
          MKOBJ( pl, PLAYER, OBJF_POS|OBJF_VEL|OBJF_HULL|OBJF_PVEL|OBJF_VIS|OBJF_PLAT|OBJF_CLIP );

          SJC_Write( "%d: New client %i created ghost is obj#%d player is obj#%d", hotfr, i, ghostslot, slot0 );

          gh->pos = (V){0.0f,0.0f,0.0f};
          gh->client = i;
          gh->avatar = slot0;

          pl->pos  = (V){(i+1)*64,-50.0f,0.0f};
          pl->vel  = (V){0.0f,0.0f,0.0f};
          pl->hull[0] = (V){-6.0f,-15.0f,0.0f};
          pl->hull[1] = (V){ 6.0f, 15.0f,0.0f};
          pl->pvel = (V){0.0f,0.0f,0.0f};
          pl->model = i%5;
          pl->ghost = ghostslot;
          pl->goingl = 0;
          pl->goingr = 0;
          pl->goingu = 0;
          pl->goingd = 0;
          pl->jumping = 0;
          pl->firing = 0;
          pl->cooldown = 0;
          pl->projectiles = 0;
          pl->grounded = 0;
          pl->facingr = 1;
          pl->stabbing = 0;
        }
      }
      if(hotfr%77==0) {
        MKOBJ( sl, SLUG, OBJF_POS|OBJF_VEL|OBJF_HULL|OBJF_VIS|OBJF_PLAT|OBJF_CLIP );
        sl->pos  = (V){(hotfr%2)*368.0f+8.0f,0.0f,0.0f};
        sl->vel  = (V){(hotfr%2)?-0.5f:0.5f,0.0f,0.0f};
        sl->hull[0] = (V){-8.0f,-4.0f,0.0f};
        sl->hull[1] = (V){ 8.0f, 8.0f,0.0f};
        sl->model = 0;
        sl->dead = 0;
      }
      if(hotfr==200) {
        MKOBJ( am, AMIGO, OBJF_POS|OBJF_VEL|OBJF_HULL|OBJF_VIS|OBJF_PLAT|OBJF_CLIP );
        am->pos  = (V){300,0,0};
        am->vel  = (V){0,0,0};
        am->hull[0] = (V){-13,-18,0};
        am->hull[1] = (V){ 13, 18,0};
        am->model = 0;
        am->state = AMIGO_HELLO;
        am->statetime = 0;
      }
      break;
    case OBJT_GHOST:
      assert(ob->size==sizeof(GHOST_t));
      break;
    case OBJT_DUMMY:
      assert(ob->size==sizeof(DUMMY_t));
      DUMMY_t *du = ob->data;

      // friction
      if(      du->vel.x> 0.1f ) du->vel.x -= 0.1f;
      else if( du->vel.x>-0.1f ) du->vel.x  = 0.0f;
      else                       du->vel.x += 0.1f;

      if( objid==(hotfr+100)%2000 ) //tee-hee
        du->vel.x += (float)(b%4)-1.5;

      du->vel.y += 0.01f;        //gravity
      break;
    case OBJT_PLAYER:
      assert(ob->size==sizeof(PLAYER_t));
      PLAYER_t *oldme = oa->data;
      PLAYER_t *newme = ob->data;
      GHOST_t *gh = fr[b].objs[newme->ghost].data;
      switch( fr[b].cmds[gh->client].cmd ) {
        case CMDT_1LEFT:  newme->goingl  = 1; newme->facingr = 0;  break;
        case CMDT_0LEFT:  newme->goingl  = 0;                      break;
        case CMDT_1RIGHT: newme->goingr  = 1; newme->facingr = 1;  break;
        case CMDT_0RIGHT: newme->goingr  = 0;                      break;
        case CMDT_1UP:    newme->goingu  = 1;                      break;
        case CMDT_0UP:    newme->goingu  = 0;                      break;
        case CMDT_1DOWN:  newme->goingd  = 1;                      break;
        case CMDT_0DOWN:  newme->goingd  = 0;                      break;
        case CMDT_1JUMP:  newme->jumping = 1;                      break;
        case CMDT_0JUMP:  newme->jumping = 0;                      break;
        case CMDT_1FIRE:  newme->firing  = 1;                      break;
        case CMDT_0FIRE:  newme->firing  = 0; newme->cooldown = 0; break;
      }

      if( !oldme ) //FIXME why's this null?
        break;

      if( ((GHOST_t *)fr[b].objs[newme->ghost].data)->client==me ) //local client match
      {
        camx = newme->pos.x - NATIVEW/2;
        camy = newme->pos.y - NATIVEH/2;
        if( setmodel>-1 ) { //FIXME -- just for fun, will not sync!
          newme->model = setmodel;
          setmodel = -1;
        }
      }

      if( newme->firing || (newme->stabbing<0 && !newme->goingu)
                        || (newme->stabbing>0 && !newme->goingd) ) //firing or stopping pressing up/down
        newme->stabbing = 0;
      else if( !newme->stabbing && newme->vel.y!=0.0 ) {       //freefalling, not stabbing
        if( newme->goingu && !oldme->goingu )                  //just pressed up
          newme->stabbing = -4;
        if( newme->goingd && !oldme->goingd )                  //just pressed down
          newme->stabbing = 4;
      }
      if( newme->stabbing && newme->vel.y==0.0 ) {             //tink tink tink!
        newme->stabbing += (newme->stabbing>0 ? -1 : 1);
        if( newme->stabbing>0 )
          newme->vel.y -= 0.8f*oldme->vel.y;
      }
      if( newme->stabbing && newme->goingu ) {                 //expand hull for stabbing
        newme->hull[0] = (V){-6.0f,-30.0f,0.0f};
        newme->hull[1] = (V){ 6.0f, 15.0f,0.0f};
      } else if( newme->stabbing && newme->goingd ) {
        newme->hull[0] = (V){-6.0f,-15.0f,0.0f};
        newme->hull[1] = (V){ 6.0f, 25.0f,0.0f};
      } else {
        newme->hull[0] = (V){-6.0f,-15.0f,0.0f};
        newme->hull[1] = (V){ 6.0f, 15.0f,0.0f};
      }

      newme->gunback = 0; //reset gun position
      if(newme->goingr||newme->goingl)
        newme->gundown = (newme->gundown+1)%10;
      else
        newme->gundown = 0;

      // friction
      if(      newme->vel.x> 0.2f ) newme->vel.x -= 0.2f;
      else if( newme->vel.x>-0.2f ) newme->vel.x  = 0.0f;
      else                          newme->vel.x += 0.2f;
      if(      newme->pvel.x> 0.5f ) newme->pvel.x -= 0.5f;
      else if( newme->pvel.x>-0.5f ) newme->pvel.x  = 0.0f;
      else                           newme->pvel.x += 0.5f;

      // -- WALK --
      if( newme->goingl ) {
        if(      newme->pvel.x>-2.0f ) newme->pvel.x += -1.0f;
        else if( newme->pvel.x>-3.0f ) newme->pvel.x  = -3.0f;
      }
      if( newme->goingr ) {
        if(      newme->pvel.x< 2.0f ) newme->pvel.x +=  1.0f;
        else if( newme->pvel.x< 3.0f ) newme->pvel.x  =  3.0f;
      }

      // -- JUMP --
      if( newme->pvel.y <= -2.0f ) {     //jumping in progress
        newme->pvel.y   +=  2.0f;        //jumpvel fades into real velocity
        newme->vel.y    += -2.0f;
      }
      else if( newme->pvel.y < 0.0f ) {  //jumping ending
        newme->vel.y    += newme->pvel.y;
        newme->pvel.y   = 0.0f;
        newme->jumping  = 0;             //must press jump again now
      }
      if( !newme->jumping )              //low-jump, cancel jump velocity early
        newme->pvel.y   = 0.0f;
      if( (newme->vel.y==0.0f || oldme->vel.y==0.0f) && newme->jumping ) //FIXME 0 velocity means grounded? not really
        newme->pvel.y  = -9.1f;         //initiate jump!

      // -- FIRE --
      if( newme->cooldown>0 )
        newme->cooldown--;
      if( newme->firing && newme->cooldown==0 && newme->projectiles<5 ) { // create bullet
        MKOBJ( bu, BULLET, OBJF_POS|OBJF_VEL|OBJF_VIS|OBJF_CLIP );
        if( newme->facingr ) {
          bu->pos = (V){newme->pos.x+19.0f,newme->pos.y-3.0f,0.0f};
          bu->vel = (V){ 8.0f,0.0f,0.0f};
        } else {
          bu->pos = (V){newme->pos.x-19.0f,newme->pos.y-3.0f,0.0f};
          bu->vel = (V){-8.0f,0.0f,0.0f};
        }
        if( newme->goingu ) { // aiming
          bu->vel.y += -8.0f;
          bu->pos.y +=-10.0f;
        }
        if( newme->goingd ) {
          bu->vel.y +=  8.0f;
          bu->pos.y += 12.0f;
        }
        bu->model = 1;
        bu->owner = objid;
        bu->ttl = 50;
        newme->cooldown = 5;
        newme->projectiles++;
        newme->gunback = 2;
      }

      for(i=0;i<objid;i++)  //find other players to interact with -- who've already MOVED
        if(fr[b].objs[i].type==OBJT_PLAYER) {
          PLAYER_t *oldyou = fr[a].objs[i].data;
          PLAYER_t *newyou = fr[b].objs[i].data;
          if( !oldyou                                  ||
              fabsf(newme->pos.x - newyou->pos.x)>5.0f || //we're not on top of each other
              fabsf(newme->pos.y - newyou->pos.y)>2.0f || 
              newme->goingr ||  newme->goingl         || //or we're moving
              newyou->goingr || newyou->goingl            )
            continue;
          if(newme->pos.x < newyou->pos.x) {
            newme->pvel.x  -= 0.2f;
            newyou->pvel.x += 0.2f;
          } else {
            newme->pvel.x  += 0.2f;
            newyou->pvel.x -= 0.2f;
          }
        }

      newme->vel.y += 0.7f;        //gravity
      break;
    case OBJT_BULLET:
      assert(ob->size==sizeof(BULLET_t));
      BULLET_t *bu = ob->data;
      if( bu->ttl ) bu->ttl--;
      for(i=0;i<maxobjs;i++)  //find players to hit
        if(fr[b].objs[i].type==OBJT_PLAYER) {
          PLAYER_t *pl = fr[b].objs[i].data;
          if( i==bu->owner                       || //player owns bullet
              fabsf(bu->pos.x - pl->pos.x)>10.0f || //not touching
              fabsf(bu->pos.y - pl->pos.y)>15.0f    )
            continue;
          pl->vel.y += -5.0f;
          pl->vel.x += (bu->vel.x>0.0f?5.0f:-5.0f);
          bu->ttl = 0; //delete bullet
        }
      if(bu->pos.x<=0.0f || bu->pos.x>=NATIVEW || bu->ttl==0) {
        if( fr[b].objs[bu->owner].type==OBJT_PLAYER )
          ((PLAYER_t *)fr[b].objs[bu->owner].data)->projectiles--;
        ob->flags |= OBJF_DEL;
      }
      break;
    case OBJT_SLUG:
      assert(ob->size==sizeof(SLUG_t));
      SLUG_t *sl = ob->data;
      sl->vel.y += 0.6f;      //gravity
      if( sl->dead )          //decay
        sl->dead++;
      else for(i=0;i<maxobjs;i++)  //find players, bullets to hit
        if(fr[b].objs[i].type==OBJT_PLAYER) {
          PLAYER_t *pl = fr[b].objs[i].data;
          if( pl->stabbing<0 &&
              fabsf(sl->pos.x -  pl->pos.x       )<8.0f && //up stab contact
              fabsf(sl->pos.y - (pl->pos.y-20.0f))<12.0f   )
          {
            pl->vel.y = sl->vel.y;
            sl->vel.x /= 100.0f;
            sl->vel.y = -5.0f;
            sl->dead = 1;
            ob->flags &= ~OBJF_PLAT;
          } else if( pl->stabbing>0 &&
              fabsf(sl->pos.x -  pl->pos.x       )<8.0f && //down stab contact
              fabsf(sl->pos.y - (pl->pos.y+20.0f))<12.0f   )
          {
            pl->vel.y = sl->vel.y - 4.5f;
            sl->vel.x /= 100.0f;
            sl->vel.y = 0.0f;
            sl->dead = 1;
            ob->flags &= ~OBJF_PLAT;
          }
        } else if(fr[b].objs[i].type==OBJT_BULLET) {
          BULLET_t *bu = fr[b].objs[i].data;
          if( fabsf(sl->pos.x - bu->pos.x)>8.0f || //no hit
              fabsf(sl->pos.y - bu->pos.y)>8.0f )
            continue;
          bu->ttl = 0;
          sl->vel.x /= 100.0f;
          sl->vel.y = -3.0f;
          sl->dead = 1;
          ob->flags &= ~OBJF_PLAT;
        }
      if( sl->dead==5 )
        ob->flags &= ~OBJF_CLIP;
      if(sl->vel.x==0 || sl->dead>100)
        ob->flags |= OBJF_DEL;
      break;
    case OBJT_AMIGO:
      assert(ob->size==sizeof(AMIGO_t));
      AMIGO_t *am = ob->data;
      float amigo_gravity = 0.6f;
//TEMP! Wrap amigo since he can mostly only go left
if( am->pos.x <         20.0f ) am->pos.x += NATIVEW-41.0f;
if( am->pos.x > NATIVEW-20.0f ) am->pos.x -= NATIVEW-41.0f;
      spatt(hotfr);
      switch( am->state ) {
        case AMIGO_HELLO:
          if( am->statetime>120 ) {
            am->state = AMIGO_COOLDOWN;
            am->statetime = 0;
          }
          break;
        case AMIGO_COOLDOWN:
          if( am->vel.y==0 ) {
            am->vel.x = 0.0f;
            if( am->statetime>30 ) {
              am->statetime = 0;
              switch( patt()%8 ) {
                case 0: am->state = AMIGO_JUMP; am->vel.y -= 10.0f;                    break;
                case 1: am->state = AMIGO_JUMP; am->vel.y -= 10.0f; am->vel.x =  4.0f; break;
                case 2: am->state = AMIGO_JUMP; am->vel.y -= 10.0f; am->vel.x = -4.0f; break;
                case 3: am->state = AMIGO_JUMP; am->vel.y -=  8.0f; am->vel.x =  4.0f; break;
                case 4: am->state = AMIGO_JUMP; am->vel.y -=  8.0f; am->vel.x = -4.0f; break;
                case 5: am->state = AMIGO_SLASH;                    am->vel.x = -0.1f; break;
                case 6: am->state = AMIGO_FLYKICK;                                     break;
                case 7: am->state = AMIGO_DASH;                     am->vel.x =-10.0f; break;
              }
            }
          }
          break;
        case AMIGO_JUMP:
          if( am->statetime>20 ) {
            am->state = AMIGO_COOLDOWN;
            am->statetime = 0;
          }
          break;
        case AMIGO_SLASH:
          am->vel.x += 0.05f;
          if( am->vel.x > 0.0f )
            am->vel.x = 0.0f;
          if( am->statetime>30 ) {
            am->state = AMIGO_COOLDOWN;
            am->statetime = 0;
          }
          break;
        case AMIGO_FLYKICK:
          if( am->statetime>1 ) {
            am->state = AMIGO_COOLDOWN;
            am->statetime = 0;
          }
          break;
        case AMIGO_DASH:
          if( am->statetime>50 ) {
            am->state = AMIGO_COOLDOWN;
            am->statetime = 0;
          }
          break;
      }
      am->statetime++;
      am->vel.y += amigo_gravity; //gravity
      break;
  } //end switch ob->type
  #undef MKOBJ
}


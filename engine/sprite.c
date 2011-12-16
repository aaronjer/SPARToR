/**
 **  SPARToR
 **  Network Game Engine
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
#include "SDL_net.h"
#include "mod.h"
#include "video.h"
#include "main.h"
#include "console.h"
#include "sprite.h"


SPRITE_T *sprites;
size_t    spr_count;

static size_t spr_alloc;
static char   filename[100];
static int    line_num;
FILE         *f;


static void more_sprites();
static int fail(const char *msg);


int load_sprites(int texnum)
{
  if( strlen(textures[texnum].filename) > 95 ) {
    SJC_Write("load_sprites: filename too long: %s", textures[texnum].filename);
    return -1;
  }

  sprintf(filename, "%s.txt", textures[texnum].filename);
  f = fopen(filename, "r");

  if( !f ) {
    SJC_Write("load_sprites: failed opening: %s", filename);
    return -1;
  }

  char buf[100];
  char line[1000];
  enum { READY, DEFAULT, GRID, GRIDITEM, NOMORE } mode = READY;
  int gridcols;
  SPRITE_T *spr;

  line_num = 0;

  while( 1 ) {
    if( mode==DEFAULT || mode==NOMORE )
      mode = READY;

    if( !fgets(line,1000,f) )
      break;

    if( line[998] )
      return fail("load_sprites: Line too long");

    spr = NULL;

    if( 1 != sscanf(line,"%99s",buf) )
      return fail("load_sprites: Expecting keyword or identifier");

    SJC_Write("load_sprites: %s", buf);

    if( buf[0] != '.' ) { // sprite name found
      //spr = new_sprite(texnum,buf);
    } else if( 0 == strcmp(buf,".default") ) {
      mode = DEFAULT;
    } else if( 0 == strcmp(buf,".grid") ) {
      mode = GRID;
    } else if( 0 == strcmp(buf,".end") ) {
      mode = NOMORE;
    } else if( 0 == strcmp(buf,".") ) {
      ;
    }

    break;
  }

  return 0;
}

void unload_sprites()
{
}

static void more_sprites()
{
  if( spr_count != spr_alloc )
    return; // don't need more!

  size_t new_count = spr_alloc < 8 ? 8 : spr_alloc*2;
  sprites = realloc( sprites, new_count * sizeof *sprites );
  memset( sprites + spr_alloc, 0, (new_count - spr_alloc) * sizeof *sprites );
  spr_alloc = new_count;
}

static int fail(const char *msg)
{
  SJC_Write("%s(%d) %s",filename,line_num,msg);
  fclose(f);
  return -1;
}


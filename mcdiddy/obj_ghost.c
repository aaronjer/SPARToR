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

#include "obj_.h"

void obj_ghost_draw( int objid, OBJ_t *o )
{
  GHOST_t *gh = o->data;

  if( !v_drawhulls ) return;

  SJGL_SetTex( TEX_PLAYER );
  SJGL_Blit( &(SDL_Rect){80,177,16,16}, gh->pos.x, gh->pos.y, NATIVEH );
}

void obj_ghost_adv( int objid, Uint32 a, Uint32 b, OBJ_t *oa, OBJ_t *ob )
{
  GHOST_t  *gh = ob->data;
  PLAYER_t *pl = fr[b].objs[gh->avatar].data;

  if( gh->client==me )
  {
    myghostleft = gh->pos.x + gh->hull[0].x;
    myghosttop  = gh->pos.y + gh->hull[0].y;
    mycontext   = ob->context;
  }

  FCMD_t *c = fr[b].cmds + gh->client;
  switch( c->cmd ) {
    case CMDT_0EPANT: { //FIXME: UNSAFE check for edit rights, data values
      size_t n = 0;
      int  i, j;
      char letter = (char)unpackbytes(c->data,MAXCMDDATA,&n,1);
      int  dnx    = (int) unpackbytes(c->data,MAXCMDDATA,&n,4);
      int  dny    = (int) unpackbytes(c->data,MAXCMDDATA,&n,4);
      int  upx    = (int) unpackbytes(c->data,MAXCMDDATA,&n,4);
      int  upy    = (int) unpackbytes(c->data,MAXCMDDATA,&n,4);
      int  value  = (int) unpackbytes(c->data,MAXCMDDATA,&n,4);

      if( letter!='p' ) { SJC_Write("Unknown edit command!"); break; }

      CONTEXT_t *co = fr[b].objs[ob->context].data;

      if( dnx > upx )  { int tmp = upx; upx = dnx; dnx = tmp; } //make so dn is less than up
      if( dny > upy )  { int tmp = upy; upy = dny; dny = tmp; }

      if( dnx<0 || dny<0 || upx>=co->x || upy>=co->y ) { SJC_Write("Edit command out of bounds!"); break; }

      for( j=dny; j<=upy; j++ )
        for( i=dnx; i<=upx; i++ ) {
          int pos = 0*co->y*co->x + j*co->x + i;

          // FIXME: WARNING: ALERT: LAME HACK for tool textures
          switch( value ) {
            case 15: //NUL
              co->dmap[pos].flags |=  CBF_NULL;
              break;

            case 31: //SOL
              if( co->dmap[pos].flags & CBF_NULL ) co->dmap[pos].data[0] = co->map[pos].data[0];
              co->dmap[pos].flags &= ~(CBF_NULL|CBF_PLAT);
              co->dmap[pos].flags |=  CBF_SOLID;
              break;

            case 47: //PLAT
              if( co->dmap[pos].flags & CBF_NULL ) co->dmap[pos].data[0] = co->map[pos].data[0];
              co->dmap[pos].flags &= ~(CBF_NULL|CBF_SOLID);
              co->dmap[pos].flags |=  CBF_PLAT;
              break;

            case 63: //OPN
              if( co->dmap[pos].flags & CBF_NULL ) co->dmap[pos].data[0] = co->map[pos].data[0];
              co->dmap[pos].flags &= ~(CBF_NULL|CBF_SOLID|CBF_PLAT);
              break;

            case 79: //COPY
              break;

            case 95: //PSTE
              break;

            case 111: //OBJ
              pl->pos.x = i*co->blocksize;
              pl->pos.y = j*co->blocksize;
              break;

            default:
              co->dmap[pos].flags   &= ~CBF_NULL;
              co->dmap[pos].data[0]  = (char)value;
          }
          // END LAME HACK
        }

      break;
    }
  }
}


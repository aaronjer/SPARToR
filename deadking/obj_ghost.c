/**
 **  Dead Kings' Quest
 **  A special game for the SPARToR Network Game Engine
 **  Copyright (c) 2010-2011  Jer Wilson
 **
 **  See COPYING for details.
 **
 **  http://www.superjer.com/
 **  http://www.spartor.com/
 **  http://github.com/superjer/SPARToR
 **/

#include "obj_.h"

void obj_ghost_draw( int objid, Uint32 vidfr, OBJ_t *o, CONTEXT_t *co )
{
  GHOST_t *gh = o->data;

  if( !v_drawhulls ) return;

  int g = TILE2NATIVE_X(co, gh->pos.x, gh->pos.y, gh->pos.z);
  int h = TILE2NATIVE_Y(co, gh->pos.x, gh->pos.y, gh->pos.z);

  SJGL_SetTex( sys_tex[TEX_PLAYER].num );
  SJGL_Blit( &(SDL_Rect){80,177,16,16}, g, h, NATIVEH );
}

void obj_ghost_adv( int objid, Uint32 a, Uint32 b, OBJ_t *oa, OBJ_t *ob )
{
  GHOST_t  *gh = ob->data;
  PLAYER_t *pl = fr[b].objs[gh->avatar].data;

  if( gh->client==me )
  {
    myghost     = objid;
    myghostleft = gh->pos.x + gh->hull[0].x;
    myghosttop  = gh->pos.y + gh->hull[0].y;
    mycontext   = ob->context;
  }

  FCMD_t *c = fr[b].cmds + gh->client;
  switch( c->cmd ) {
    case CMDT_0EPANT: { //FIXME: UNSAFE check for edit rights, data values
      size_t n = 0;
      int  i, j, k;
      char letter = (char)unpackbytes(c->data,MAXCMDDATA,&n,1);
      int  dnx    = (int) unpackbytes(c->data,MAXCMDDATA,&n,4);
      int  dny    = (int) unpackbytes(c->data,MAXCMDDATA,&n,4);
      int  dnz    = (int) unpackbytes(c->data,MAXCMDDATA,&n,4);
      int  upx    = (int) unpackbytes(c->data,MAXCMDDATA,&n,4);
      int  upy    = (int) unpackbytes(c->data,MAXCMDDATA,&n,4);
      int  upz    = (int) unpackbytes(c->data,MAXCMDDATA,&n,4);
      int  value  = (int) unpackbytes(c->data,MAXCMDDATA,&n,1);
      int  ntex   = (int) unpackbytes(c->data,MAXCMDDATA,&n,1);

      if( letter!='p' ) { SJC_Write("Unknown edit command!"); break; }

      CONTEXT_t *co = fr[b].objs[ob->context].data;

      if( dnx > upx )  { int tmp = upx; upx = dnx; dnx = tmp; } //make so dn is less than up
      if( dny > upy )  { int tmp = upy; upy = dny; dny = tmp; }
      if( dnz > upz )  { int tmp = upz; upz = dnz; dnz = tmp; }

      if( dnx<0 || dny<0 || dnz<0 || upx>=co->x || upy>=co->y || upz>=co->z )
      {
        SJC_Write("Edit command out of bounds!");
        break;
      }

      if( value == 0x4F ) { //COPY tool texture
        gh->clipboard_x = upx - dnx + 1;
        gh->clipboard_y = upy - dny + 1;
        gh->clipboard_z = upz - dnz + 1;
        gh->clipboard_data = malloc( gh->clipboard_x*gh->clipboard_y*gh->clipboard_z*(sizeof *gh->clipboard_data) ); //FIXME: mem leak
      }

      if( value == 0x5F && !gh->clipboard_data ) { SJC_Write("Clipboard is empty"); break; }

      for( k=dnz; k<=upz; k++ ) for( j=dny; j<=upy; j++ ) for( i=dnx; i<=upx; i++ ) {
        int pos = k*co->y*co->x + j*co->x + i;

        // FIXME: WARNING: ALERT: LAME HACK for tool textures
        switch( value ) {
          case 0x0F: //NUL
            co->dmap[pos].flags |=  CBF_NULL;
            break;

          case 0x1F: //SOL
            if( co->dmap[pos].flags & CBF_NULL ) memcpy( co->dmap[pos].data, co->map[pos].data, 2 );
            co->dmap[pos].flags &= ~(CBF_NULL|CBF_PLAT);
            co->dmap[pos].flags |=  CBF_SOLID;
            break;

          case 0x2F: //PLAT
            if( co->dmap[pos].flags & CBF_NULL ) memcpy( co->dmap[pos].data, co->map[pos].data, 2 );
            co->dmap[pos].flags &= ~(CBF_NULL|CBF_SOLID);
            co->dmap[pos].flags |=  CBF_PLAT;
            break;

          case 0x3F: //OPN
            if( co->dmap[pos].flags & CBF_NULL ) memcpy( co->dmap[pos].data, co->map[pos].data, 2 );
            co->dmap[pos].flags &= ~(CBF_NULL|CBF_SOLID|CBF_PLAT);
            break;

          case 0x4F: { //COPY
            CB *cb = ( (co->dmap[pos].flags & CBF_NULL) ? co->map : co->dmap ) + pos;
            gh->clipboard_data[  (k-dnz)*gh->clipboard_y*gh->clipboard_x
                               + (j-dny)*gh->clipboard_x
                               + (i-dnx)
                              ] = *cb;
            break; }

          case 0x5F: //PSTE
            co->dmap[pos] = gh->clipboard_data[  ((k-dnz)%gh->clipboard_z)*gh->clipboard_y*gh->clipboard_x
                                               + ((j-dny)%gh->clipboard_y)*gh->clipboard_x
                                               + ((i-dnx)%gh->clipboard_x)
                                              ];
            break;

          case 0x6F: //OBJ
            pl->pos.x = i*co->bsx;
            pl->pos.y = j*co->bsy;
            pl->pos.z = k*co->bsz;
            break;

          default:
            if( co->dmap[pos].flags & CBF_NULL ) co->dmap[pos].flags = co->map[pos].flags;
            co->dmap[pos].flags   &= ~CBF_NULL;
            co->dmap[pos].data[0]  = (char)value;
            co->dmap[pos].data[1]  = (char)ntex;
        }
        // END LAME HACK
      }

      break;
    }
  }
}


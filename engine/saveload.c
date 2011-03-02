

#include "mod.h"
#include "main.h"
#include "saveload.h"
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>


char *sjtempnam(const char *dir, const char *pfx, const char *ext)
{
  FILE *f;
  char *buf = malloc(256);
  int i;

  for( i=0; i<9999; i++ ) {
    snprintf( buf, 256, "%s/%s%d%s", dir, pfx, i, ext);
    f = fopen( buf, "r" );
    if( !f )
      return buf;
    fclose( f );
  }

  return NULL;
}


int save_context(const char *name,int context,int savefr)
{
  char  path[256];
  char  bakdir[256];
  char *bakfile;

  snprintf( path,   256, "%s/maps/%s.txt", MODNAME, name );
  snprintf( bakdir, 256, "%s/maps/backup", MODNAME       );

  // attempt to backup existing file by moving it to backup directory
  if( !(bakfile = sjtempnam(bakdir,name,".txt")) ) {
    SJC_Write("Failed to create temporary name");
    return -1;
  }

  // fail if any error occurs other than that the file does not already exist
  if( -1 == rename(path,bakfile) ) {
    if( errno != ENOENT ) {
      SJC_Write("Failed to backup old map file");
      free( bakfile );
      return -1;
    }
  } else
    SJC_Write("Old file backed up to %s",bakfile);

  free( bakfile );

  // now that the existing file, if any, is safe, open file to write to
  FILE *f = fopen( path, "w" );
  if( !f ) {
    SJC_Write("Failed to open file for writing: %s",path);
    return -1;
  }

  savefr = savefr%maxframes;
  CONTEXT_t *co = fr[savefr].objs[context].data;
  int x,y,z;

  //                      v--- number of texture file names
  if(0>fprintf( f, "%s %i 1\n", MODNAME, MAPVERSION )) goto fail;

  if(0>fprintf( f, "faketexture.png\n" )) goto fail;

  if(0>fprintf( f, "%i %i %i %i\n", co->blocksize, co->x, co->y, co->z )) goto fail;

  for( z=0; z<co->z; z++ ) {
    for( y=0; y<co->y; y++ ) {
      for( x=0; x<co->x; x++ ) {
        int    pos = co->x*co->y*z + co->x*y + x;
        Uint8 *data;
        int    flags;

        if( co->dmap[ pos ].flags & CBF_NULL ) {
          data  = co->map[  pos ].data;
          flags = co->map[  pos ].flags;
        } else {
          data  = co->dmap[ pos ].data;
          flags = co->dmap[ pos ].flags;
        }

        if( flags ) {
          if(0>fprintf( f, " %.2x,%.2x", data[0], flags )) goto fail;
        } else {
          if(0>fprintf( f, " %.2x   "  , data[0]        )) goto fail;
        }
      }
      if(0>fprintf( f, "\n" )) goto fail;
    }
    if(0>fprintf( f, "\n" )) goto fail;
  }

  fclose(f);
  SJC_Write("Current context saved to %s",path);
  return 0;

  fail:
  fclose(f);
  SJC_Write("There was an error while writing the file!");
  return -1;
}


int fail(FILE *f,const char *msg)
{
  SJC_Write("Context loading error: %s",msg);
  fclose(f);
  return -1;
}


int load_context(const char *name,int context,int loadfr)
{
  char path[256];
  int i;

  snprintf( path, 256, "%s/maps/%s.txt", MODNAME, name );

  FILE *f = fopen( path, "r" );
  if( !f ) {
    SJC_Write("Failed to open file for reading: %s",path);
    return -1;
  }

  char modname[256];
  int version = 0;
  int ntex = 0;
  if( 3 != fscanf(f,"%100s %d %d\n",modname,&version,&ntex) )     return fail(f,"failed to read line 1");
  if( 0 != strcmp(modname,MODNAME) )                              return fail(f,"MODNAME mismatch");
  if( version != MAPVERSION )                                     return fail(f,"MAPVERSION mismatch");
  if( ntex<1 || ntex>255 )                                        return fail(f,"invalid texture count");

  char texnames[ntex][100];
  for( i=0; i<ntex; i++ ) {
    if( 1 != fscanf(f,"%100s\n",texnames[i]) )                    return fail(f,"error reading texture name");
  }

  int blocksize,x,y,z;
  if( 4 != fscanf(f,"%d %d %d %d\n",&blocksize,&x,&y,&z) )        return fail(f,"failed to read line 2");
  if( blocksize<1 || blocksize>1024 )                             return fail(f,"blocksize is out of range");
  if( x<1 || x>9000 )                                             return fail(f,"x is out of range");
  if( y<1 || y>9000 )                                             return fail(f,"y is out of range");
  if( z<1 || z>9000 )                                             return fail(f,"z is out of range");

  int volume = x * y * z;
  CB *map  = malloc( (sizeof *map  ) * volume );
  CB *dmap = malloc( (sizeof *dmap ) * volume );

  for( i=0; i<volume; i++ ) {
    unsigned int tile, flags = 0;
    int numread;

    if( 1 > (numread=fscanf(f,"%x,%x",&tile,&flags)) )  return fail(f,"failed to read block data");

    map[ i].data[0] = (Uint8)tile;
    map[ i].flags   = flags;
    dmap[i].flags   = CBF_NULL;
  }

  // everything ok? swap it in
  loadfr = loadfr%maxframes;
  CONTEXT_t *co = fr[loadfr].objs[context].data;
  co->blocksize = blocksize;
  co->x = x;
  co->y = y;
  co->z = z;
  if( co->map  ) free(co->map ); //FIXME remove hack_map someday and don't free this stuff here
  if( co->dmap ) free(co->dmap);
  co->map  = hack_map  = map;
  co->dmap = hack_dmap = dmap;

  fclose(f);

  return 0;
}



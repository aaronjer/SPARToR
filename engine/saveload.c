

#include "mod.h"
#include "main.h"
#include "saveload.h"


int save_context(const char *name,int context,int savefr)
{
  char path[256];
  snprintf( path, 256, "%s/maps/%s.txt", MODNAME, name );

  FILE *f = fopen( path, "w" );
  if( !f ) {
    SJC_Write("Failed to open file for writing: %s",path);
    return -1;
  }

  savefr = savefr%maxframes;
  CONTEXT_t *co = fr[savefr].objs[context].data;
  int x,y,z;

  if(0>fprintf( f, "%s %i\n", MODNAME, MAPVERSION )) goto fail;

  if(0>fprintf( f, "%i %i %i %i\n", co->blocksize, co->x, co->y, co->z )) goto fail;

  for( z=0; z<co->z; z++ ) {
    for( y=0; y<co->y; y++ ) {
      for( x=0; x<co->x; x++ ) {
        int   pos = co->x*co->y*z + co->x*y + x;
        char *data;
        int   flags;

        if( co->dmap[ pos ].flags & CBF_NULL ) {
          data  = co->map[  pos ].data;
          flags = co->map[  pos ].flags;
        } else {
          data  = co->dmap[ pos ].data;
          flags = co->dmap[ pos ].flags;
        }

        if(0>fprintf( f, " %.2x,%.2x", data[0], flags )) goto fail;
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


int load_fail(FILE *f,const char *msg)
{
  SJC_Write(msg);
  fclose(f);
  return -1;
}


int load_context(const char *name,int context,int loadfr)
{
  char path[256];
  snprintf( path, 256, "%s/maps/%s.txt", MODNAME, name );

  FILE *f = fopen( path, "r" );
  if( !f ) {
    SJC_Write("Failed to open file for reading: %s",path);
    return -1;
  }
  
  //if( !fgets(path,256,f) ) return load_fail(f,"Failed to read line 1");

  char modname[256];
  int version = 0;
  if( 2 != fscanf(f,"%100s %d\n",modname,&version) )
    return load_fail(f,"Failed to read line 1");

  SJC_Write("modname='%s' version='%s'",modname,version);
/*
  int volume = x * y * z;

  // everything ok? swap it in
  CONTEXT_t *co = fr[loadfr].objs[context].data;
  co->blocksize = 16;
  co->x = 100;
  co->y =  15;
  co->z =   1;
  co->map  = hack_map  = malloc( (sizeof *co->map ) * volume ); //FIXME remove hack
  co->dmap = hack_dmap = malloc( (sizeof *co->dmap) * volume );
  memset( co->map,  0, (sizeof *co->map ) * volume );
  memset( co->dmap, 0, (sizeof *co->dmap) * volume );
  int i;
  for( i=0; i<volume; i++ ) {
    co->map[ i].data[0] = 255;
    co->dmap[i].flags   = CBF_NULL;
  }
*/

  return 0;
}



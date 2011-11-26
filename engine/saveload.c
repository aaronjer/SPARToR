//
// map header format:
//  modname mapversion texturefilecount
//  texturefilepath
//  texturefilepath
//  ...
//  blocksizeX blocksizeY blocksizeZ
//  dimensionX dimensionY dimensionZ
//  [blockdata...]

#include "mod.h"
#include "main.h"
#include "saveload.h"
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>


#define B85_1(x) (b85alphabet[((x)              )%85])
#define B85_2(x) (b85alphabet[((x)/(         85))%85])
#define B85_3(x) (b85alphabet[((x)/(      85*85))%85])
#define B85_4(x) (b85alphabet[((x)/(   85*85*85))%85])
#define B85_5(x) (b85alphabet[((x)/(85*85*85*85))%85])


static char b85alphabet[] = "!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstu";

Uint32 from_b85( const char *s )
{
  int i;
  char *p;
  Uint32 sum = 0;

  for( i=0; i<5; i++ ) {
    if( !s[i] || !( p = strchr(b85alphabet,s[i]) ) )
      break;
    sum = sum*85 + (p-b85alphabet);
  }
  return sum;
}


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
      SJC_Write("Failed to backup old file %s",path);
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

  // find textures in use
  int i;
  int in_use[ tex_count ];
  memset( in_use, 0, tex_count * sizeof *in_use );
  for( i=0; i<co->x*co->y*co->z; i++ ) {
    size_t texn = (co->dmap[i].flags & CBF_NULL) ? co->map[i].data[1] : co->dmap[i].data[1];

    if( texn < tex_count )
      in_use[texn] = 1;
    else
      SJC_Write("Warning: texture %d is in use but is out of bounds (tex_count=%d)",texn,tex_count);
  }

  // get count of used textures, and number each one sequentially in in_use[]
  int use_count = 0;
  for( i=0; i<(int)tex_count; i++ )
    if( in_use[i] )
      in_use[i] = ++use_count; // inflates in_use values by 1

  // start writing the file
  if(0>fprintf( f, "%s %i %i\n", MODNAME, MAPVERSION, use_count )) goto fail;

  for( i=0; i<(int)tex_count; i++ )
    if( in_use[i] && 0>fprintf( f, "%s\n", textures[i].filename ) ) goto fail;

  if(0>fprintf( f, "%i %i %i\n", co->bsx, co->bsy, co->bsz )) goto fail;

  if(0>fprintf( f, "%i %i %i\n", co->x,   co->y,   co->z   )) goto fail;

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
        //                                         v--- in_use values were inflated by 1
        size_t val = data[0] + ((in_use[data[1]] - 1)<<8);

        if( flags ) {
          if(0>fprintf( f, " %c%c%c%c~%.2x", B85_4(val), B85_3(val), B85_2(val), B85_1(val), flags )) goto fail;
        } else {
          if(0>fprintf( f, " %c%c%c%c   "  , B85_4(val), B85_3(val), B85_2(val), B85_1(val)        )) goto fail;
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
  int texnumbers[ntex];
  memset( texnumbers, 0, ntex * sizeof *texnumbers );
  for( i=0; i<ntex; i++ ) {
    if( 1 != fscanf(f,"%100s\n",texnames[i]) )                    return fail(f,"error reading texture name");
    texnumbers[i] = make_sure_texture_is_loaded(texnames[i]);
  }

  int bsx,bsy,bsz;
  if( 3 != fscanf(f,"%d %d %d\n",&bsx,&bsy,&bsz) )                return fail(f,"failed to read context size"); 
  if( bsx<1 || bsx>1024 )                                         return fail(f,"bsx is out of range");
  if( bsy<1 || bsy>1024 )                                         return fail(f,"bsy is out of range");
  if( bsz<1 || bsz>1024 )                                         return fail(f,"bsz is out of range");

  int x,y,z;
  if( 3 != fscanf(f,"%d %d %d\n",&x,&y,&z) )                      return fail(f,"failed to read dimensions");
  if( x<1 || x>9000 )                                             return fail(f,"x is out of range");
  if( y<1 || y>9000 )                                             return fail(f,"y is out of range");
  if( z<1 || z>9000 )                                             return fail(f,"z is out of range");

  int volume = x * y * z;
  CB *map  = malloc( (sizeof *map ) * volume );
  CB *dmap = malloc( (sizeof *dmap) * volume );

  for( i=0; i<volume; i++ ) {
    Uint32  tile, flags = 0;
    int     tex;
    Uint32  b85num;
    char    b85str[6] = {0};

    if( 1 > fscanf(f," %5[^ vwxyz{|}~]~%x ",b85str,&flags) ) //   return fail(f,"failed to read block data");
      b85num = 0;
    else
      b85num = from_b85(b85str);

    tile = (Uint8)b85num;
    tex  = (Uint8)(b85num>>8);
    if( tex >= ntex ) SJC_Write("tex is too high! (%d/%d)",tex,ntex);

    map[ i].data[0] = tile;
    map[ i].data[1] = (tex < ntex ? texnumbers[tex] : 0); // avoid overread if tex is too high
    map[ i].flags   = flags;
    dmap[i].flags   = CBF_NULL;
  }

  // everything ok? swap it in
  loadfr = loadfr%maxframes;
  CONTEXT_t *co = fr[loadfr].objs[context].data;
  co->bsx = bsx;
  co->bsy = bsy;
  co->bsz = bsz;
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


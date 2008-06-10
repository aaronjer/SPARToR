/**
 **  SuperJer Strings
 **/

#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include "sjstring.h"


//create a new superjer string
SJS_t *sjsnew(const char *src)
{
  SJS_t *dst = malloc(sizeof(SJS_t));
  dst->len = (src?strlen(src):0);
  dst->data = malloc(dst->len+1);
  if(src)
    memcpy(dst->data,src,dst->len+1);
  return dst;
}


//destroy a superjer string
void sjsfree(SJS_t *s)
{
  free(s->data);
  free(s);
}


//return a superjer string as a const C string
const char *sjscstr(const SJS_t *s)
{
  return (const char *)s->data;
}


//gets the char at position n in a superjer string
int sjsat(const SJS_t *s,size_t n)
{
  if( n>s->len )
    return 0;
  return s->data[n];
}


//gets a substring of a superjer string, works like PHP's substr()
SJS_t *sjssub(SJS_t *dst,const SJS_t *src,int start,int n)
{
  int stop;
  if( dst==NULL )
    dst = malloc(sizeof(SJS_t *));
  else
    free(dst->data);
  if(start>(int)src->len)
    start = (int)src->len;
  else if(start<0)
  {
    start = (int)src->len+start;
    if( start<0 ) start = 0;
  }
  if(n>0)
  {
    stop = start+n;
    if( stop>(int)src->len ) stop = (int)src->len;
  }
  else
  {
    stop = (int)src->len+n;
    if( stop<start ) stop = start;
  }
  dst->len = stop-start;
  dst->data = malloc(dst->len+1);
  memcpy(dst->data,src->data+start,dst->len);
  dst->data[dst->len] = '\0';
  return dst;
}


//fixes the length and null-term of a superjer string if it's been mucked with
void sjsfix(SJS_t *s)
{
  s->data[s->len] = '\0';
  s->len = strlen(s->data);
}


//copy a superjer string to another superjer string
SJS_t *sjscpy(SJS_t *dst,const SJS_t *src)
{
  free(dst->data);
  dst->len = src->len;
  dst->data = malloc(dst->len+1);
  memcpy(dst->data,src->data,dst->len+1);
  return dst;
}


//copy a C string to a superjer string
SJS_t *sjscpy_im(SJS_t *dst,const char *src)
{
  free(dst->data);
  dst->len = strlen(src);
  dst->data = malloc(dst->len+1);
  memcpy(dst->data,src,dst->len+1);
  return dst;
}


//copy a superjer string to another superjer string (bounded)
SJS_t *sjsncpy(SJS_t *dst,const SJS_t *src,size_t n)
{
  free(dst->data);
  if( n>src->len+1 )
    dst->len = src->len;
  else
    dst->len = n;
  dst->data = malloc(dst->len+1);
  memcpy(dst->data,src->data,dst->len+1);
  return dst;
}


//copy a C string to a superjer string (bounded)
SJS_t *sjsncpy_im(SJS_t *dst,const char *src,size_t n)
{
  free(dst->data);
  dst->len = strlen(src);
  if( n<dst->len )
    dst->len = n;
  dst->data = malloc(dst->len+1);
  memcpy(dst->data,src,dst->len+1);
  return dst;
}


//copy a superjer string to a C string (bounded)
char *sjsncpy_ex(char *dst,const SJS_t *src,size_t n)
{
  if( n>src->len+1 )
    memmove(dst,src->data,src->len+1);
  else
  {
    memmove(dst,src->data,n-1);
    dst[n-1] = '\0';
  }
  return dst;
}


//concatenate superjer strings
SJS_t *sjscat(SJS_t *dst,const SJS_t *src)
{
  dst->data = realloc(dst->data,dst->len+src->len+1);
  memcpy(dst->data+dst->len,src->data,src->len+1);
  dst->len += src->len;
  return dst;
}


//concatenate a C string onto a superjer string
SJS_t *sjscat_im(SJS_t *dst,const char *src)
{
  size_t sl = strlen(src);
  dst->data = realloc(dst->data,dst->len+sl+1);
  memcpy(dst->data+dst->len,src,sl+1);
  dst->len += sl;
  return dst;
}


//concatenate superjer strings (bounded)
SJS_t *sjsncat(SJS_t *dst,const SJS_t *src,size_t n)
{
  size_t sl = (n>src->len?src->len:n);
  dst->data = realloc(dst->data,dst->len+sl+1);
  memcpy(dst->data+dst->len,src->data,sl);
  dst->len += sl;
  dst->data[dst->len] = '\0';
  return dst;
}


//concatenate a C string onto a superjer string (bounded)
SJS_t *sjsncat_im(SJS_t *dst,const char *src,size_t n)
{
  size_t sl = strlen(src);
  sl = (n>sl?sl:n);
  dst->data = realloc(dst->data,dst->len+sl+1);
  memcpy(dst->data+dst->len,src,sl);
  dst->len += sl;
  dst->data[dst->len] = '\0';
  return dst;
}


//find first char c in a superjer string
size_t sjschr(const SJS_t *s,int c,size_t offset)
{
  if(offset>s->len)
    return -1;
  char *p = strchr(s->data+offset,c);
  return p ? (p-s->data) : -1;
}


//find first char c in a superjer string in reverse
size_t sjsrchr(const SJS_t *s,int c,size_t offset)
{
  if( offset>s->len)
    return -1;
  char *p = s->data + s->len - 1 - offset;
  while(*p)
  {
    if(*p==c) return p-s->data;
    p--;
  }
  return -1;
}


//compare superjer strings
int sjscmp(const SJS_t *lhs,const SJS_t *rhs)
{
  return strcmp(lhs->data,rhs->data);
}


//compare first n chars of superjer strings
int sjsncmp(const SJS_t *lhs,const SJS_t *rhs,size_t n)
{
  return strncmp(lhs->data,rhs->data,n);
}


//get length of a superjer string
size_t sjslen(const SJS_t *s)
{
  return s->len;
}


//find length of substring containing ONLY certain chars in a superjer string
size_t sjsspn(const SJS_t *s,const char *chars,size_t offset)
{
  if( offset>s->len )
    return -1;
  return strspn(s->data+offset,chars);
}


//find length of substring NOT containing certain chars in a superjer string
size_t sjscspn(const SJS_t *s,const char *chars,size_t offset)
{
  if( offset>s->len )
    return -1;
  return strcspn(s->data+offset,chars);
}


//find a superjer string in a superjer string
size_t sjsstr(const SJS_t *s,const SJS_t *find,size_t offset)
{
  if( offset>s->len )
    return -1;
  char *p = strstr(s->data+offset,find->data);
  return p ? (p-s->data) : -1;
}


//find a C string in  a superjer string
size_t sjsstr_im(const SJS_t *s,const char *find,size_t offset)
{
  if( offset>s->len )
    return -1;
  char *p = strstr(s->data+offset,find);
  return p ? (p-s->data) : -1;
}



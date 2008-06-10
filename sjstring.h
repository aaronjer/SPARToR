/**
 **  SuperJer Strings
 **
 **  faster and "safer" than regular C strings
 **  use sjscstr() to get the C string representation of a
 **  superjer string for use with most standard calls
 **
 **  NOT SELF-REFERENTIALLY SAFE!!
 **/

#ifndef __SJSTRING_H__
#define __SJSTRING_H__

#include <string.h>


typedef struct
{
  size_t len;
  char *data;
} SJS_t;

typedef SJS_t *sjs;


SJS_t *     sjsnew(const char *src);
void        sjsfree(SJS_t *s);
const char *sjscstr(const SJS_t *s);
int         sjsat(const SJS_t *s,size_t n);
SJS_t *     sjssub(SJS_t *dst,const SJS_t *src,int start,int n);

void        sjsfix(SJS_t *s); //questionable

SJS_t *     sjscat(SJS_t *dst,const SJS_t *src);
SJS_t *     sjscat_im(SJS_t *dst,const char *src);
SJS_t *     sjsncat(SJS_t *dst,const SJS_t *src,size_t n);
SJS_t *     sjsncat_im(SJS_t *dst,const char *src,size_t n);
SJS_t *     sjscpy(SJS_t *dst,const SJS_t *src);
SJS_t *     sjscpy_im(SJS_t *dst,const char *src);
SJS_t *     sjsncpy(SJS_t *dst,const SJS_t *src,size_t n);
SJS_t *     sjsncpy_im(SJS_t *dst,const char *src,size_t n);
char *      sjsncpy_ex(char *dst,const SJS_t *src,size_t n);
size_t      sjschr(const SJS_t *s,int c,size_t offset);
size_t      sjsrchr(const SJS_t *s,int c,size_t offset);
int         sjscmp(const SJS_t *lhs,const SJS_t *rhs);
int         sjsncmp(const SJS_t *lhs,const SJS_t *rhs,size_t n);
int         sjscasecmp(const SJS_t *lhs,const SJS_t *rhs);
int         sjsncasecmp(const SJS_t *lhs,const SJS_t *rhs,size_t n);
size_t      sjslen(const SJS_t *s);
size_t      sjsspn(const SJS_t *s,const char *chars,size_t offset);
size_t      sjscspn(const SJS_t *s,const char *chars,size_t offset);
size_t      sjsstr(const SJS_t *s,const SJS_t *find,size_t offset);
size_t      sjsstr_im(const SJS_t *s,const char *find,size_t offset);


#endif


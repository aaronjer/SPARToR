#ifndef SPARTOR_SAVELOAD_H_
#define SPARTOR_SAVELOAD_H_

#include "main.h"

#define MAPVERSION 1


int save_context(const char *name,int context,int savefr);
int load_context(const char *file,int context,int loadfr);

#endif

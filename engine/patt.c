

#include "mod.h"
#include "patt.h"


uint32_t patt()
{
  return genrand_int32();
}

void spatt(uint32_t seed)
{
  init_genrand(seed);
}


#ifndef PATT_H_
#define PATT_H_

#include "mt19937ar.h"

inline uint32_t patt()
{
  return genrand_int32();
}

inline void spatt(uint32_t seed)
{
  init_genrand(seed);
}

#endif

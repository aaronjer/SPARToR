#ifndef PATT_H_
#define PATT_H_

static uint32_t a = 0x8675309;

inline uint32_t patt()
{
  a = ((uint64_t)a * 0x10A860C1) % 0xFFFFFFFF;
  return a;
}

inline void spatt(uint32_t seed)
{
  a = seed % 0xFFFFFFFF;
}

#endif

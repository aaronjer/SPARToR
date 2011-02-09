#ifndef PATT_H_
#define PATT_H_

static unsigned long long a;

inline void spatt(unsigned long long seed)
{
  a = seed;
}

inline unsigned int patt()
{
  a = a * 7212336353ULL + 8963315421273233617ULL;
  return (unsigned int)a;
}

#endif

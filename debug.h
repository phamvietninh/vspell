#ifndef __DEBUG_H__ // -*- mode: c++ -*-
#define __DEBUG_H__
#include <stdio.h>
#define ASSERT(i) if (!(i)) {fprintf(stderr,"Error at %d",__LINE__); exit(0);}
#endif

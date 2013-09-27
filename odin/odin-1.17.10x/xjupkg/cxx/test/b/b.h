#ifndef _B_H_
#define _B_H_

#include "c.h"
#include <b/d.h>
#include <string>

inline std::string b() throw()
{
  return "b"+c()+d();
}

#endif

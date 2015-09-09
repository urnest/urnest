#include <iostream>
#include <stdint.h>
#include <limits.h>

int main(int argc, char* argv[])
{
  std::cout 
    << UINT64_MAX - ULONG_MAX
    << std::endl;
  return 0;
}

    

#include "x.hh"
#include <iostream>

int main(int argc, char* argv[])
{
  std::cout << F::x << F::y[1] << F::z[0] << F::a
            << N::G::q 
            << std::endl;
  return 0;
}

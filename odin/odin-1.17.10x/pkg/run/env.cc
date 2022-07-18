#include <iostream>
#include <unistd.h>

void assert_not_eof(int c)
{
  if (c==EOF) {
    abort();
  }
}
void assert_equal(int c, int expect)
{
  if (c != expect) {
    abort();
  }
}

int main(int argc, char* argv[])
{
  int c;
  for(c=std::cin.get(); c!=EOF && c!='\n'; c=std::cin.get());
  assert_equal(c,'\n');
  for(c=std::cin.get(); c!=EOF; c=std::cin.get()) {
    switch(c) {
    case '=':
    {
      std::cout << "=\"";
      for(c=std::cin.get(); c!=EOF && c!=1; c=std::cin.get()) {
        std::cout << (char)c;
      }
      assert_equal(c,1);
      std::cout << "\"\n";
      assert_equal(std::cin.get(),'\n');
    }
    break;
    default:
      std::cout << (char)c;
    }
  }
  
  return 0;
}
         

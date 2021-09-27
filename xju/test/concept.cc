#include <string>
#include <memory>

template<class T,class U,class P1>
class CallToV1
{
public:
  explicit CallToV1(
    T& x,void 
    (U::*f)(P1),
    P1 p1)
      : x_(x),
        f_(f),
        p1_(p1)
  {
  }
  T& x_;
  void (U::* const f_)(P1);
  P1 p1_;
};


template<class T,class U,class P1>
class CallToV1_
{
public:
  explicit CallToV1_(
    T& x,void 
    (U::*f)(P1))
      : x_(x),
        f_(f)
  {
  }
  T& x_;
  void (U::* const f_)(P1);

  std::shared_ptr<CallToV1<T,U,P1> > operator()(P1 p1){
    return std::shared_ptr<CallToV1<T,U,P1> >(
      new CallToV1<T,U,P1>(x_, f_, p1));
  }
};


template<class T,class U,class P1>
CallToV1_<T,U,P1> f(
  T& x, 
  void (U::*f)(P1)) noexcept
{
  return CallToV1_(x,f);
}


class X
{
public:
  void f(std::string& x){}
  void g(int x){}
  void h(int const& x){}
};

int main(int argc, char* argv[]){
  X x;
  std::string fred("fred");
  f(x, &X::f)(fred);
  f(x, &X::g)(3);
  f(x, &X::h)(3);
  return 0;
}



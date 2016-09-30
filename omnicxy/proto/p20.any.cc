#include <omnicxy/proto/p20.any.hh>
#line 1 "/home/xju/urnest/omnicxy/proto/p20.any.hcp"
#line 6
#include <cxy/any_.hh> //impl

namespace cxy
{
#line 14
std::shared_ptr<cxy::TypeCode> typeCodeOf(
  cxy::TypeTag< ::p20::A > const&) throw(std::bad_alloc)
{
  return std::shared_ptr<cxy::TypeCode>(
    new cxy::StructTypeCode(
      cxy::cdr< ::p20::A >::repoId,
      "p20::A",
      {
        { "a_", cxy::typeCodeOf(cxy::TypeTag< int16_t >()) }
      }));
}

}

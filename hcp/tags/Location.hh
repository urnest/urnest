#ifndef _HCP_TAGS_LOCATION_HH
#define _HCP_TAGS_LOCATION_HH
// generated from Location.idl by omni cxy idl backend specifying 
// xju::Exception from <xju/Exception.hh> as base class for all ipc exceptions

#include <xju/Exception.hh>


// included idl
#include <hcp/tags/AbsolutePath.hh>
#include <hcp/tags/FileName.hh>
#include <hcp/tags/DirName.hh>
#include <hcp/tags/LineNumber.hh>

namespace hcp
{
namespace tags
{
struct Location
{
  
  ::hcp::tags::AbsolutePath directory;
  ::hcp::tags::FileName file;
  ::hcp::tags::LineNumber line;

  Location(
    ::hcp::tags::AbsolutePath const& p1,
    ::hcp::tags::FileName const& p2,
    ::hcp::tags::LineNumber const& p3) throw();

  friend bool operator<(
    Location const& x, 
    Location const& y) throw() {
    if (x.directory<y.directory) return true;
    if (y.directory<x.directory) return false;
    if (x.file<y.file) return true;
    if (y.file<x.file) return false;
    if (x.line<y.line) return true;
    if (y.line<x.line) return false;
    return false;
  }
  friend bool operator>(
    Location const& x, 
    Location const& y) throw() {
    return y<x;
  }
  friend bool operator!=(
    Location const& x, 
    Location const& y) throw() {
    return (x<y)||(y<x);
  }
  friend bool operator==(
    Location const& x, 
    Location const& y) throw() {
    return !(x!=y);
  }
  friend bool operator<=(
    Location const& x, 
    Location const& y) throw() {
    return (x<y)||(x==y);
  }
  friend bool operator>=(
    Location const& x, 
    Location const& y) throw() {
    return (x>y)||(x==y);
  }
};

}
}

#endif

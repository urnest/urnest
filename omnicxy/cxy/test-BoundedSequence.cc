// Copyright (c) 2015 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <cxy/BoundedSequence.hh>

#include <iostream>
#include <xju/assert.hh>
#include <xju/next.hh>
#include <xju/prev.hh>

namespace cxy
{

void test1() {
  {
    BoundedSequence<int, 2> x;
    xju::assert_equal(x.size(),0);
    xju::assert_equal(x.begin(),x.end());
    xju::assert_equal(x.empty(),true);
    
    x.push_back(1);
    x.push_back(2);
    xju::assert_equal(x[0],1);
    xju::assert_equal(x[1],2);
    xju::assert_equal(x.size(),2);
      
    BoundedSequence<int, 2> y(x);
    xju::assert_equal(x,y);
    try {
      x.push_back(3);
      xju::assert_never_reached();
    }
    catch(cxy::Exception const& e) {
      xju::assert_equal(readableRepr(e),"BoundedSequence of max size 2 is full.");
    }
    xju::assert_equal(x,y);
    BoundedSequence<int, 2> z;
    z=std::move(y);
    xju::assert_equal(x,z);
    y=z;
    xju::assert_equal(x,y);
    xju::assert_equal(x,z);

    {
      BoundedSequence<int,2> z{9};
      z=std::move(y);
      xju::assert_equal(x,z);
    }
    {
      BoundedSequence<int,2> z{9};
      z={1,2};
      xju::assert_equal(x,z);
      try {
        z={1,2,3};
        xju::assert_never_reached();
      }
      catch(cxy::Exception const& e) {
        xju::assert_equal(readableRepr(e),"BoundedSequence of max size 2 cannot accept 3 elements.");
      }
      xju::assert_equal(x,z);
    }
    
    BoundedSequence<int,2> a{1,2};
    xju::assert_equal(x,a);

    try {
      BoundedSequence<int,2> b{1,2,3};
      xju::assert_never_reached();
    }
    catch(cxy::Exception const& e) {
      xju::assert_equal(readableRepr(e),"BoundedSequence of max size 2 cannot accept 3 elements.");
    }
    
    std::vector<int> c{1,2,3};
    {
      BoundedSequence<int,3> d(c.begin(),c.end());
      xju::assert_equal(d, BoundedSequence<int,3>{1,2,3});
    }
    {
      BoundedSequence<int,3> d;
      d.assign(c.begin(),c.end());
      xju::assert_equal(d, BoundedSequence<int,3>{1,2,3});
    }
          
    try {
      BoundedSequence<int,2> d(c.begin(),c.end());
      xju::assert_never_reached();
    }
    catch(cxy::Exception const& e) {
      xju::assert_equal(readableRepr(e),"BoundedSequence of max size 2 cannot accept 3 elements.");
    }
    try {
      BoundedSequence<int,2> d;
      d.assign(c.begin(),c.end());
      xju::assert_never_reached();
    }
    catch(cxy::Exception const& e) {
      xju::assert_equal(readableRepr(e),"BoundedSequence of max size 2 cannot accept 3 elements.");
    }
    {
      BoundedSequence<int,3> d;
      d.assign({1,2,3});
      xju::assert_equal(d, BoundedSequence<int,3>{1,2,3});
    }
    try {
      BoundedSequence<int,2> d;
      d.assign({1,2,3});
      xju::assert_never_reached();
    }
    catch(cxy::Exception const& e) {
      xju::assert_equal(readableRepr(e),"BoundedSequence of max size 2 cannot accept 3 elements.");
    }

    xju::assert_equal(x.begin(),x.cbegin());
    xju::assert_equal(x.end(),x.cend());
    xju::assert_equal(*x.rbegin(),2);
    xju::assert_equal(xju::next(xju::next(x.rbegin())),x.rend());
    xju::assert_equal(xju::next(xju::next(x.crbegin())),x.crend());
    xju::assert_equal(x.max_size(),2);

    //resize
    {
      BoundedSequence<int,2> d;
      d.resize(2,9);
      xju::assert_equal(d,BoundedSequence<int,2>{9,9});
    }
    try {
      BoundedSequence<int,2> d;
      d.resize(3,9);
      xju::assert_never_reached();
    }
    catch(cxy::Exception const& e) {
      xju::assert_equal(readableRepr(e),"BoundedSequence of max size 2 cannot be resized to 3.");
    }
    
    //capacity
    xju::assert_equal(x.capacity(),2);

    //empty
    xju::assert_equal(BoundedSequence<int,2>().empty(),true);

    //reserve
    {
      BoundedSequence<int,2> d{1};
      d.reserve(2);
      xju::assert_equal(d,BoundedSequence<int,2>{1});
    }
    try {
      BoundedSequence<int,2> d;
      d.reserve(3);
      xju::assert_never_reached();
    }
    catch(cxy::Exception const& e) {
      xju::assert_equal(readableRepr(e),"BoundedSequence of max size 2 cannot reserve room for 3 elements.");
    }

    //operator[] const
    {
      BoundedSequence<int,2> const d{2,1};
      xju::assert_equal(d[0],2);
      xju::assert_equal(d[1],1);
    }
    //at
    {
      BoundedSequence<int,2> d{2,1};
      xju::assert_equal(d.at(0),2);
      xju::assert_equal(d.at(1),1);
    }
    try {
      BoundedSequence<int,2> d{2,1};
      d.at(3);
      xju::assert_never_reached();
    }
    catch(cxy::Exception const& e) {
      xju::assert_equal(readableRepr(e),"BoundedSequence has only 2 elements, not 4.");
    }
    //at const
    {
      BoundedSequence<int,2> const d{2,1};
      xju::assert_equal(d.at(0),2);
      xju::assert_equal(d.at(1),1);
    }
    try {
      BoundedSequence<int,2> const d{2,1};
      d.at(3);
      xju::assert_never_reached();
    }
    catch(cxy::Exception const& e) {
      xju::assert_equal(readableRepr(e),"BoundedSequence has only 2 elements, not 4.");
    }
    //front,back,data
    xju::assert_equal(x.front(),1);
    xju::assert_equal(x.back(),2);
    xju::assert_not_equal(x.data(),(int*)0);
    xju::assert_equal(x.data()[0],1);
    xju::assert_equal(x.data()[1],2);
    {
      BoundedSequence<int,2> const d{2,1};
      xju::assert_equal(d.front(),2);
      xju::assert_equal(d.back(),1);
      xju::assert_equal(d.data()[0],2);
      xju::assert_equal(d.data()[1],1);
    }

    //push_back
    {
      BoundedSequence<int,2> d;
      d.push_back(std::move(2));
      xju::assert_equal(d,BoundedSequence<int,2>{2});
      d.push_back(3);
      xju::assert_equal(d,BoundedSequence<int,2>{2,3});
      try {
        d.push_back(4);
        xju::assert_never_reached();
      }
      catch(cxy::Exception const& e) {
        xju::assert_equal(readableRepr(e),"BoundedSequence of max size 2 is full.");
      }
      xju::assert_equal(d,BoundedSequence<int,2>{2,3});
    }
      
    //emplace_back
    {
      BoundedSequence<int,1> d;
      d.emplace_back(2);
      xju::assert_equal(d,BoundedSequence<int,1>{2});
      try {
        d.emplace_back(1);
        xju::assert_never_reached();
      }
      catch(cxy::Exception const& e) {
        xju::assert_equal(readableRepr(e),"BoundedSequence of max size 1 is full.");
      }
      xju::assert_equal(d,BoundedSequence<int,1>{2});
    }

    //insert
    {
      BoundedSequence<int,2> d;
      d.insert(d.end(),3);
      xju::assert_equal(d,BoundedSequence<int,2>{3});
      d.insert(d.begin(),std::move(2));
      xju::assert_equal(d,BoundedSequence<int,2>{2,3});
      try {
        d.insert(d.end(),4);
        xju::assert_never_reached();
      }
      catch(cxy::Exception const& e) {
        xju::assert_equal(readableRepr(e),"BoundedSequence of max size 2 is full.");
      }
      try {
        d.insert(d.end(),std::move(4));
        xju::assert_never_reached();
      }
      catch(cxy::Exception const& e) {
        xju::assert_equal(readableRepr(e),"BoundedSequence of max size 2 is full.");
      }
    }
    {
      BoundedSequence<int,4> d;
      d.insert(d.end(),{2,4});
      xju::assert_equal(d,BoundedSequence<int,4>{2,4});
      d.insert(xju::next(d.begin()),{3});
      xju::assert_equal(d,BoundedSequence<int,4>{2,3,4});
      try {
        d.insert(xju::next(d.begin()),{6,7});
        xju::assert_never_reached();
      }
      catch(cxy::Exception const& e) {
        xju::assert_equal(readableRepr(e),"Failed to insert range of elements into BoundedSequence of max size 4 at offset 1 because\nBoundedSequence of max size 4 is full.");
      }
      xju::assert_equal(d,BoundedSequence<int,4>{2,3,4});
      d.insert(d.end(),{5});
      xju::assert_equal(d,BoundedSequence<int,4>{2,3,4,5});
    }
    {
      BoundedSequence<int,3> d;
      d.insert(d.end(),2,3);
      xju::assert_equal(d,BoundedSequence<int,3>{3,3});
      try {
        d.insert(d.end(),2,4);
        xju::assert_never_reached();
      }
      catch(cxy::Exception const& e) {
        xju::assert_equal(readableRepr(e),"BoundedSequence of max size 3 has 2 elements and cannot accept 2 more.");
      }
      xju::assert_equal(d,BoundedSequence<int,3>{3,3});
    }
    {
      BoundedSequence<int,4> d;
      std::vector<int> dd{2,4};
      d.insert(d.end(),dd.begin(),dd.end());
      xju::assert_equal(d,BoundedSequence<int,4>{2,4});
      std::vector<int> ee{3,5};
      d.insert(xju::next(d.begin()),ee.begin(),xju::next(ee.begin()));
      xju::assert_equal(d,BoundedSequence<int,4>{2,3,4});
      try {
        d.insert(xju::next(d.begin()),dd.begin(),dd.end());
        xju::assert_never_reached();
      }
      catch(cxy::Exception const& e) {
        xju::assert_equal(readableRepr(e),"Failed to insert range of elements into BoundedSequence of max size 4 at offset 1 because\nBoundedSequence of max size 4 is full.");
      }
      xju::assert_equal(d,BoundedSequence<int,4>{2,3,4});
      d.insert(xju::prev(d.end()),dd.begin(),xju::next(dd.begin()));
      xju::assert_equal(d,BoundedSequence<int,4>{2,3,2,4});
    }
    {
      BoundedSequence<int,4> d;
      std::vector<int> dd{2,5};
      d.insert(d.end(),dd.begin(),dd.end());
      xju::assert_equal(d,BoundedSequence<int,4>{2,5});
      std::vector<int> ee{3,4,7};
      try {
        d.insert(d.begin(),ee.begin(),ee.end());
        xju::assert_never_reached();
      }
      catch(cxy::Exception const& e) {
        xju::assert_equal(readableRepr(e),"Failed to insert range of elements into BoundedSequence of max size 4 at offset 0 because\nBoundedSequence of max size 4 is full.");
      }
      xju::assert_equal(d,BoundedSequence<int,4>{2,5});
      d.insert(xju::next(d.begin()),ee.begin(),xju::prev(ee.end()));
      xju::assert_equal(d,BoundedSequence<int,4>{2,3,4,5});
    }
    // erase
    {
      BoundedSequence<int,2> x{1,2};
      auto const i(x.erase(x.begin()));
      xju::assert_equal(x,BoundedSequence<int,2>{2});
      xju::assert_equal(i,x.begin());
    }
    {
      BoundedSequence<int,2> x{1,2};
      auto const i(x.erase(x.begin(),x.end()));
      xju::assert_equal(x,BoundedSequence<int,2>{});
      xju::assert_equal(i,x.begin());
    }
    //swap
    {
      BoundedSequence<int,2> x{1,2};
      BoundedSequence<int,2> y{3,4};
      x.swap(y);
      xju::assert_equal(x,BoundedSequence<int,2>{3,4});
      xju::assert_equal(y,BoundedSequence<int,2>{1,2});
    }
    //clear
    {
      BoundedSequence<int,2> x{1,2};
      x.clear();
      xju::assert_equal(x,BoundedSequence<int,2>{});
    }
  }
}

}

using namespace cxy;

int main(int argc, char* argv[])
{
  unsigned int n(0);
  test1(), ++n;
  std::cout << "PASS - " << n << " steps" << std::endl;
  return 0;
}

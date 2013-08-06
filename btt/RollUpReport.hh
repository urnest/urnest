//     -*- mode: c++ ; c-file-style: "osse" ; -*-
//
// Copyright (c) 2008
// Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#ifndef BTT_ROLLUPREPORT_H_
#define BTT_ROLLUPREPORT_H_

#include "xju/Seconds.hh"
#include <iomanip>
#include "xju/format.hh"
#include <map>
#include <string>
#include <algorithm>
#include "xju/assert.hh"
#include "btt/Tasks.hh"

namespace btt
{
class RollUpReport
{
public:
  btt::Tasks const tasks_;
  
  explicit RollUpReport(const btt::Tasks& tasks,
                        std::map<std::string, xju::Seconds>& totals) throw():
    tasks_(tasks),
    totals_(totals) {
  }
  
  // Accumlate time against task "group", as defined by the task
  // name up to the first colon, e.g. "spr: 1123" is accounted to
  // "spr".
  // - use whole name if no colon
  void operator()(std::pair<const btt::TaskId, unsigned int> const& x) throw() {
    const btt::Tasks::const_iterator t(
      std::find_if(tasks_.begin(),
                   tasks_.end(),
                   btt::Task::HasId(x.first)));
    xju::assert_not_equal(t, tasks_.end());
    std::string const description((*t).description_.value());
    std::string const key(
      description.begin(),
      std::find(description.begin(), description.end(), ':'));
    (*totals_.insert(std::make_pair(key, xju::Seconds(0))).first).second+=
        xju::Seconds(x.second);
  }
  
  std::map<std::string, xju::Seconds>& totals_;

    friend std::ostream& operator<<(std::ostream& s, 
                                RollUpReport const& x) throw() {
    for(std::map<std::string, xju::Seconds>::const_iterator i=x.totals_.begin();
        i!=x.totals_.end();
        ++i) {
        s << "    ";
        s.setf(std::ios_base::left);
      s << std::setw(40) << (*i).first
        << " "
        << xju::format::float_(((double)(*i).second.value())/60.0/60.0, 
                               std::ios_base::fixed, 1)
        << " hours"
        << std::endl;
    }
    return s;
  }
};

}

#endif

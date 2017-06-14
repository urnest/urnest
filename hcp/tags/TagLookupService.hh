#ifndef _TAGLOOKUPSERVICE_HH
#define _TAGLOOKUPSERVICE_HH
// Copyright (c) 2017 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <hcp/tags/Lookup.hh>

#include <xju/io.hh>
#include <memory>
#include <vector>
#include <map>
#include <utility>
#include <xju/path.hh>
#include <atomic>
#include "xju/io/FileObserver.hh"
#include <set>
#include <hcp/tags/Namespace.hh>
#include "xju/Mutex.hh"
#include "xju/Lock.hh"
namespace hcp
{
namespace tags
{
class TagLookupService : public Lookup
{
public:
  // create tags lookup service covering tagsFiles
  // - tagsFiles need not exist yet, but their parent directories must exist
  explicit TagLookupService(
    std::vector<std::pair<xju::path::AbsolutePath,xju::path::FileName> > const& tagsFiles) throw(
      // eg a parent directory does not exist
      xju::Exception);


  // track tags file changes, until stop() called
  // - note that files should be updated by writing to a separate file
  //   then renaming over the tagsFile
  //
  void run() throw();


  // make current and future calls to run() return immediately
  void stop() throw();


  // lookup symbol amongst tags files
  // - returns results from a tags file that knows about symbol
  //
  Lookup::Locations lookupSymbol(NamespaceNames const& fromScope,
                                 NamespaceNames const& symbolScope,
                                 UnqualifiedSymbol const& symbol) throw();


  // update ie reload any files that have changed
  // pre: l.holds(guard_)
  void updateFiles(xju::Lock const& l) throw();

  
private:
  typedef std::pair<xju::path::AbsolutePath,xju::path::FileName> AbsFile;

  std::vector<AbsFile> const files_;

  std::atomic<bool> stop_;
  std::pair<std::unique_ptr<xju::io::IStream>,
            std::unique_ptr<xju::io::OStream> > const stopper_;

  mutable xju::Mutex guard_;
  xju::io::FileObserver filesWatcher_;
  std::map<AbsFile, std::shared_ptr<Namespace> > const filesMap_;
  
};
  
}
}
#endif

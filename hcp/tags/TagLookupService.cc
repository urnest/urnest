#include "TagLookupService.hh"
#include <hcp/tags/str.hh> //impl
#include <iostream> //impl
#include <xju/pipe.hh> //impl
#include <xju/now.hh> //impl
#include "hcp/tags/augmentRootNamespace.hh" //impl

namespace hcp
{
namespace tags
{
namespace
{
typedef std::pair<xju::path::AbsolutePath,xju::path::FileName> AbsFile;

std::map<AbsFile, std::shared_ptr<Namespace> > makeFilesMap(
  std::vector<AbsFile> const& tagsFiles) throw()
{
  std::map<AbsFile, std::shared_ptr<Namespace> > result;
  std::transform(tagsFiles.begin(),tagsFiles.end(),
                 std::inserter(result,result.end()),
                 [](AbsFile const& x) {
                   return std::make_pair(
                     x,
                     std::shared_ptr<Namespace>(new Namespace));
                 }
    );
  return result;
}

}

TagLookupService::TagLookupService(
    std::vector<std::pair<xju::path::AbsolutePath,xju::path::FileName> > const& tagsFiles) throw(
      // eg a parent directory does not exist
      xju::Exception):
      files_(tagsFiles),
      stop_(false),
      stopper_(xju::pipe(true,true)),
      filesWatcher_(std::set<AbsFile>(tagsFiles.begin(),tagsFiles.end())),
      filesMap_(makeFilesMap(files_))
  {
  }

  // track tags file changes, until stop() called
  // - note that files should be updated by writing to a separate file
  //   then renaming over the tagsFile
  //
  
void TagLookupService::run() throw()
  {
    std::set<xju::io::Input const*> const inputs
      {&filesWatcher_,&*stopper_.first};
    while(!stop_.load()) {
      if (xju::io::select(inputs,xju::now()+std::chrono::seconds(10))
          .find(&filesWatcher_)!=inputs.end()) {
        xju::Lock l(guard_);
        updateFiles(l);
      }
    }
  }

  // make current and future calls to run() return immediately
  
void TagLookupService::stop() throw()
  {
    stop_.store(true);
    stopper_.second->write("x",1U,std::chrono::system_clock::now());
  }

  // lookup symbol amongst tags files
  // - returns results from a tags file that knows about symbol
  //
  
Lookup::Locations TagLookupService::lookupSymbol(NamespaceNames const& fromScope,
                                 NamespaceNames const& symbolScope,
                                 UnqualifiedSymbol const& symbol) throw() {
    xju::Lock l(guard_);
    updateFiles(l);
    for(auto const x:files_) {
      Namespace const& root(*(*filesMap_.find(x)).second);
      try {
        auto const l(root.lookup(fromScope,symbolScope,symbol));
        if (l.size()) {
          std::cerr << "found " << str(symbolScope) << str(symbol)
                    << " (referenced from ::"
                    << str(fromScope)
                    << ") in tags file "
                    << xju::path::str(x);
          return l;
        }
      }
      catch(xju::Exception& e) {
        std::ostringstream s;
        s << "lookup symbol in tags file " << xju::path::str(x);
        e.addContext(s.str(),XJU_TRACED);
        std::cerr << readableRepr(e) << std::endl;
      }
    }
    return Lookup::Locations();
  }

  // update ie reload any files that have changed
  // pre: l.holds(guard_)
  
void TagLookupService::updateFiles(xju::Lock const& l) throw()
  {
    xju::assert_equal(l.holds(guard_),true);
    for(auto x: filesWatcher_.read(xju::now())) {
      std::ostringstream s;
      s << "reload tags file " << xju::path::str(x);
      try {
        std::cerr << s.str() << std::endl;
        auto const i(filesMap_.find(x));
        xju::assert_not_equal(i,filesMap_.end());
        Namespace& root(*(*i).second);
        root=Namespace();
        try {
          augmentRootNamespace(root,x,false);
        }
        catch(xju::Exception& e) {
          std::ostringstream s;
          s << "reload tags file " << xju::path::str(x);
          e.addContext(s.str(),XJU_TRACED);
          throw;
        }
      }
      catch(xju::Exception& e) {
        e.addContext(s.str(),XJU_TRACED);
        std::cerr << "ERROR: " << readableRepr(e) << std::endl;
      }
    }
  }
  

}
}

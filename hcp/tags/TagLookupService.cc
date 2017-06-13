#include "TagLookupService.hh"
#include <hcp/tags/str.hh> //impl
#include <iostream> //impl
#include <xju/pipe.hh> //impl
#include <xju/now.hh> //impl
namespace hcp
{
namespace tags
{
namespace
{
typedef std::pair<xju::path::AbsolutePath,xju::path::FileName> AbsFile;

std::vector<std::shared_ptr<TagsFile>> makeOrderedFiles(
  std::vector<AbsFile> const& files) throw()
{
  std::vector<std::shared_ptr<TagsFile>> result;
  std::transform(files.begin(), files.end(),
                 std::back_inserter(result),
                 [](AbsFile const& f) {
                   return std::shared_ptr<TagsFile>(
                     new TagsFile(f.first,f.second));
                 }
    );
  return result;
}

std::map<AbsFile, std::shared_ptr<TagsFile> > makeFilesMap(
  std::vector<std::shared_ptr<TagsFile>> const& tagsFiles) throw()
{
  std::map<AbsFile, std::shared_ptr<TagsFile> > result;
  std::transform(tagsFiles.begin(),tagsFiles.end(),
                 std::inserter(result,result.end()),
                 [](std::shared_ptr<TagsFile> const& x) {
                   return std::make_pair(
                     AbsFile(x->d_,x->f_),
                     x);
                 }
    );
  return result;
}

}

TagLookupService::TagLookupService(
    std::vector<std::pair<xju::path::AbsolutePath,xju::path::FileName> > const& tagsFiles) throw(
      // eg a parent directory does not exist
      xju::Exception):
      stop_(false),
      filesWatcher_(std::set<AbsFile>(tagsFiles.begin(),tagsFiles.end())),
      files_(makeOrderedFiles(tagsFiles)),
      filesMap_(makeFilesMap(files_)),
      stopper_(xju::pipe(true,true))
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
        updateFiles();
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
    updateFiles();
    for(auto const x:files_) {
      auto const l(x->lookupSymbol(fromScope,symbolScope,symbol));
      if (l.size()) {
        std::cerr << "found " << str(symbolScope) << str(symbol)
                  << " (referenced from ::"
                  << str(fromScope)
                  << ") in tags file "
                  << xju::path::str(std::make_pair(x->d_,x->f_));
        return l;
      }
    }
    return Lookup::Locations();
  }

  // update ie reload any files that have changed
  
void TagLookupService::updateFiles() throw()
  {
    for(auto x: filesWatcher_.read(xju::now())) {
      std::ostringstream s;
      s << "reload tags file " << xju::path::str(x);
      try {
        std::cerr << s.str() << std::endl;
        auto const i(filesMap_.find(x));
        xju::assert_not_equal(i,filesMap_.end());
        (*i).second->reload();
      }
      catch(xju::Exception& e) {
        e.addContext(s.str(),XJU_TRACED);
        std::cerr << "ERROR: " << readableRepr(e) << std::endl;
      }
    }
  }
  

}
}

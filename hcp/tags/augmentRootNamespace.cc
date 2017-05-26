#include <hcp/tags/augmentRootNamespace.hh>
#line 1 "/home/xju/urnest/hcp/tags/augmentRootNamespace.hcp"
#line 12
#include <hcp/parser.hh> //impl
#line 15
#include <xju/stringToUInt.hh> //impl
#include <hcp/ast.hh> //impl
#include "xju/assert.hh" //impl
#line 19
#include <xju/readFile.hh> //impl
#line 21
#include <hcp/tags/splitSymbol.hh> //impl

namespace hcp
{
namespace tags
{
#line 28
namespace
{
using namespace hcp_parser;

PR eatWhite(zeroOrMore*whitespaceChar);
hcp_parser::PR stringValue(parseUntil(doubleQuote));
struct SymbolTag{};
typedef hcp_ast::TaggedCompositeItem<SymbolTag> Symbol;
PR symbol(new NamedParser<Symbol>(
            "symbol",
            stringValue));
struct FileNameTag{};
typedef hcp_ast::TaggedCompositeItem<FileNameTag> ParsedFileName;
PR fileName(new NamedParser<ParsedFileName>(
              "file name",
              stringValue));

class ParsedLineNumber : public hcp_ast::CompositeItem
{
public:
  ParsedLineNumber(std::vector<IR> const& items) throw(xju::Exception):
      CompositeItem(items),
      lineNumber_(xju::stringToUInt(str()))
  {
  }
  hcp::tags::LineNumber const lineNumber_;
  operator hcp::tags::LineNumber() const throw()
  {
    return lineNumber_;
  }
};
  
PR digit(charInRange('0','9'));
PR lineNumber(new NamedParser<ParsedLineNumber>(
                "line number",
                // we do not use parse::atLeastOne as it uses
                // CompositeItem, which is redundent
                digit+zeroOrMore*digit));

PR openBrace(parseLiteral("{"));
PR closeBrace(parseLiteral("}"));
PR f(parseLiteral("f"));
PR l(parseLiteral("l"));
PR colon(parseLiteral(":"));
PR comma(parseLiteral(","));

struct LocationTag{};
typedef hcp_ast::TaggedCompositeItem<LocationTag> ParsedLocation;
PR location(new NamedParser<ParsedLocation>(
              "location",
              openBrace+eatWhite+doubleQuote+f+doubleQuote+eatWhite+
              colon+eatWhite+
              doubleQuote+fileName+doubleQuote+eatWhite+comma+eatWhite+
              doubleQuote+l+doubleQuote+eatWhite+colon+eatWhite+
              lineNumber+eatWhite+closeBrace+eatWhite));

PR openSquare(parseLiteral("["));
PR closeSquare(parseLiteral("]"));

PR locations=openSquare+eatWhite+location+eatWhite+
  zeroOrMore*(comma+eatWhite+location+eatWhite)+
  closeSquare+eatWhite;

struct EntryTag{};
typedef hcp_ast::TaggedCompositeItem<EntryTag> Entry;
PR entry(new NamedParser<Entry>(
           "entry",
           doubleQuote+symbol+doubleQuote+eatWhite+
           colon+eatWhite+locations));


PR tagsFile=openBrace+eatWhite+
  ((entry+eatWhite+zeroOrMore*(comma+eatWhite+entry)+closeBrace+eatWhite)|
    (closeBrace+eatWhite))+
  endOfFile;

template<class T>
T const& find(hcp_ast::IRs::const_iterator begin,
              hcp_ast::IRs::const_iterator end)
  throw() {
  hcp_ast::IRs::const_iterator const i(
    hcp_ast::find1stInTree(begin,end,
                           [](IR x) { return x->isA<T>(); }));
  xju::assert_not_equal(i,end);
  return (*i)->asA<T>();
}
}


// augment rootNamespace with symbols from file
void augmentRootNamespace(Namespace& rootNamespace,
                          std::pair<hcp::tags::AbsolutePath, hcp::tags::FileName> const& tagsFileName,
                          bool const traceParser)
  throw(
    // pre: rootNamespace == rootNamespace@pre
    xju::Exception) {
  try {
    std::string const x(xju::readFile(xju::path::str(tagsFileName)));
    hcp_parser::I i(x.begin(), x.end());
    hcp_ast::CompositeItem root;
    
    hcp_parser::parse(root, i, tagsFile, traceParser);

    for(auto x:root.items_) {
      if (x->isA<Entry>()) {
        Entry const& entry(x->asA<Entry>());
        Symbol const& symbol(
          find<Symbol>(entry.items_.begin(),entry.items_.end()));
        std::vector<hcp::tags::Location> locations;
        for(auto x:entry.items_) {
          if (hcp_ast::isA_<ParsedLocation>(x)) {
            ParsedLocation const& location(
              x->asA<ParsedLocation>());
            auto f(find<ParsedFileName>(location.items_.begin(),
                                        location.items_.end()));
            auto l(find<ParsedLineNumber>(location.items_.begin(),
                                          location.items_.end()));
            auto fileName(xju::path::split(f.str()));
            locations.push_back(hcp::tags::Location(
                                  fileName.first,
                                  fileName.second,
                                  l));
          }
        }
        auto const ns(splitSymbol(symbol.str()));
        rootNamespace.addSymbol(ns.first,ns.second,locations);
      }
    }
  }
  catch(xju::Exception& e) {
    std::ostringstream s;
    s << "augment namespace with symbols from tags file "
      << xju::path::str(tagsFileName);
    e.addContext(s.str(),XJU_TRACED);
    throw;
  }
}

}
}

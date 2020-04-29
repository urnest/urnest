//     -*- mode: c++ ; c-file-style: "osse" ; -*-
//
// Copyright (c) 2005
// Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
//    XML (parsing) utilities
//
//    REVISIT: unicode support
//    REVISIT: pass tag names to parseStart/End tag (but still provide
//             current varients)
//    REVISIT: dynamic entities, character references in entities
#ifndef _XJU_XML_HH_
#define _XJU_XML_HH_

#include <map>
#include <utility>
#include <string>
#include <xju/Exception.hh>
#include <sstream>
#include <iterator>
#include <string>
#include <xju/format.hh>
#include <xju/stringToUInt.hh>
#include <xju/Optional.hh>
#include <xju/parse.hh>

namespace xju
{
    namespace xml
    {
        // REVISIT: use wstring/wchar, with auto-conversion from char in
        // IteratorAdaptor if necessary

        template<class T, class Tag> class Tagged;

        typedef std::string NCName;

        class NameLocalPartTag{};
        class NamePrefixTag{};
        class URITag{};
        class AttributeValueTag{};
        class CDataTag{};
        class CommentTag{};
        class WhiteSpaceTag{};
        
        typedef Tagged<NCName, NameLocalPartTag> NameLocalPart;
        typedef Tagged<NCName, NamePrefixTag> NamePrefix;
        typedef Tagged<std::string, URITag> URI;
        typedef Tagged<std::string, AttributeValueTag> AttributeValue;
        typedef Tagged<std::string, CDataTag> CData;
        typedef Tagged<std::string, CommentTag> Comment;
        typedef Tagged<std::string, WhiteSpaceTag> WhiteSpace;
        
        typedef std::pair<NamePrefix, NameLocalPart> UnexpandedName;
        typedef std::pair<URI, NameLocalPart> ExpandedName;

        typedef std::map<NamePrefix, URI> NamespaceDefinitions;
        
        typedef std::map<ExpandedName, AttributeValue> Attributes;
        
        typedef std::pair<ExpandedName, Attributes> Tag;
        // TagOpening.second true iff tag is empty (i.e, ends with />)
        typedef std::pair<Tag, bool> TagOpening;

        class XmlDecl
        {
        public:
            explicit XmlDecl(
                std::string const& version,
                xju::Optional<std::string> const& encoding) throw():
                version_(version),
                encoding_(encoding)
            {
            }
            std::string version_;
            xju::Optional<std::string> encoding_;
        };
        inline bool operator<(XmlDecl const& a, XmlDecl const& b) throw()
        {
            return (a.version_ < b.version_) ||
                (a.version_ == b.version_ && a.encoding_ < b.encoding_);
        }
        inline bool operator==(XmlDecl const& a, XmlDecl const& b) throw()
        {
            return !(a < b) && !(b < a);
        }

        // Below: see parse.hh IteratorAdaptor and RewindableIterator concept

        //
        // Skip over all whitespace and comments at currentLocation.
        // 
        template<class RewindableIterator>
        RewindableIterator skipWhiteSpaceAndComments(
            RewindableIterator const at) /*throw(
                xju::Exception)*/;

        // Parse tag opening assumed to commence at "at", augmenting
        // namespaces and applying attributeDefaults.
        // - apply skip at *end* to e.g. skip trailing whitespace
        // e.g. parses '<T a="fred"/>'
        // e.g. parses '<x:T xmlns:x="X">'
        // pre: default namespace if present in namespaces has NamePrefix ""
        // post: result.second > at
        //       result.first.first.second has no namespace definitions
        //       result.first.first.second has at least attributeDefaults
        // post: namespaces augmented with new/changed definitions
        template<class RewindableIterator>
        std::pair<TagOpening, RewindableIterator> parseTagOpening(
            RewindableIterator const at,
            NamespaceDefinitions& namespaces,
            const Attributes& attributeDefaults = Attributes(),
            RewindableIterator (*skip)(RewindableIterator) = 
              skipWhiteSpaceAndComments) /*throw(
                  // post: namespaces have not been modified
                  xju::Exception)*/;

        // - apply skip at *end* to e.g. skip trailing whitespace
        // post: result.second > at
        template<class RewindableIterator>
        std::pair<ExpandedName, RewindableIterator> parseTagEnd(
            RewindableIterator const at,
            NamespaceDefinitions const& namespaces,
            RewindableIterator (*skip)(RewindableIterator) = 
              skipWhiteSpaceAndComments) /*throw(
                xju::Exception)*/;

        // - apply skip at *end* to e.g. skip trailing whitespace
        template<class RewindableIterator>
        std::pair<XmlDecl, RewindableIterator> parseXmlDecl(
            RewindableIterator at,
            RewindableIterator (*skip)(RewindableIterator) =
            skipWhiteSpaceAndComments) /*throw(
                xju::Exception)*/;
        
        template<class RewindableIterator>
        std::pair<CData, RewindableIterator> parseCData(
            RewindableIterator at) throw();

        template<class RewindableIterator>
        std::pair<Comment, RewindableIterator> parseComment(
            RewindableIterator at) /*throw(xju::Exception)*/;

        template<class RewindableIterator>
        std::pair<WhiteSpace, RewindableIterator> parseWhiteSpace(
            RewindableIterator at) /*throw(xju::Exception)*/;

        // post: result.second > at
        template<class RewindableIterator>
        std::pair<std::string, RewindableIterator> parseEntityReference(
            RewindableIterator const at) /*throw(
                xju::Exception)*/;

        // post: result.second > at
        template<class RewindableIterator>
        std::pair<char, RewindableIterator> parseDecimalCharacterReference(
            RewindableIterator at) /*throw(
                xju::Exception)*/;
        
        // post: result.second > at
        template<class RewindableIterator>
        std::pair<char, RewindableIterator> parseHexCharacterReference(
            RewindableIterator at) /*throw(
                xju::Exception)*/;
        
        // post: result.second > at
        template<class RewindableIterator>
        std::pair<char, RewindableIterator> parseCharacterReference(
            RewindableIterator at) /*throw(
                xju::Exception)*/;

        // post: result.second > at
        template<class RewindableIterator>
        std::pair<NameLocalPart, RewindableIterator> parseNameLocalPart(
            RewindableIterator at) /*throw(
                xju::Exception)*/;
        
        // post: result.second > at
        template<class RewindableIterator>
        std::pair<NamePrefix, RewindableIterator> parseNamePrefix(
            RewindableIterator at) /*throw(
                xju::Exception)*/;

        // post: result.second > at
        template<class RewindableIterator>
        std::pair<UnexpandedName, RewindableIterator> parseUnexpandedName(
            RewindableIterator const at) /*throw(
                xju::Exception)*/;
        
        // post: result.second > at
        template<class RewindableIterator>
        std::pair<AttributeValue, RewindableIterator> parseAttributeValue(
            RewindableIterator at) /*throw(
                xju::Exception)*/;
        
        // post: result.second > at
        template<class RewindableIterator>
        std::pair<std::pair<UnexpandedName, AttributeValue>, 
                  RewindableIterator> parseAttribute(
                      RewindableIterator const at) /*throw(
                          xju::Exception)*/;

        // Parse attribute definitions, augmenting namespace definitions.
        template<class RewindableIterator>
        std::pair<Attributes, RewindableIterator> parseAttributes(
            RewindableIterator const at,
            NamespaceDefinitions& namespaceDefinitions,
            const Attributes& attributeDefaults) /*throw(
                // post: namespaceDefinitions has not been modified
                xju::Exception)*/;
        
        template<class T, class Tag>
        class Tagged
        {
        public:
            explicit Tagged(const T& x):
                m_value(x)
            {
            }
            const T& value() const throw()
            {
                return m_value;
            }
        private:
            T m_value;
        };
        template<class T, class Tag>
        bool operator<(const Tagged<T, Tag>& x,
                       const Tagged<T, Tag>& y)
        {
            return x.value() < y.value();
        }
        template<class T, class Tag>
        bool operator==(const Tagged<T, Tag>& x,
                        const Tagged<T, Tag>& y)
        {
            return x.value() == y.value();
        }
        template<class T, class Tag>
        bool operator!=(const Tagged<T, Tag>& x,
                        const Tagged<T, Tag>& y)
        {
            return x.value() != y.value();
        }
        template<class T, class Tag>
        bool operator>(const Tagged<T, Tag>& x,
                       const Tagged<T, Tag>& y)
        {
            return x.value() > y.value();
        }
        inline std::string str(const NameLocalPart& x) throw()
        {
            return x.value();
        }
        inline std::string str(const URI& x) throw()
        {
            return x.value();
        }
        inline std::string str(const ExpandedName& x) throw()
        {
            return str(x.first)+"/"+str(x.second);
        }

    }
}

// implementation
namespace xju
{
    namespace xml
    {
        inline std::ostream& operator<<(std::ostream& s,
                                        NameLocalPart const& x) throw()
        {
            return s << "Name (Local Part) " << xju::format::quote(x.value());
        }
        
        inline std::ostream& operator<<(std::ostream& s,
                                        NamePrefix const& x) throw()
        {
            return s << "Name Prefix " << xju::format::quote(x.value());
        }
        inline std::ostream& operator<<(std::ostream& s,
                                        URI const& x) throw()
        {
            return s << "URI " << xju::format::quote(x.value());
        }
        inline std::ostream& operator<<(std::ostream& s,
                                        AttributeValue const& x) throw()
        {
            return s << "Attribute Value " << xju::format::quote(x.value());
        }
        inline std::ostream& operator<<(std::ostream& s,
                                        CData const& x) throw()
        {
            return s << "CData " << xju::format::quote(x.value());
        }
        inline std::ostream& operator<<(std::ostream& s,
                                        Comment const& x) throw()
        {
            return s << "Comment " << xju::format::quote(x.value());
        }
        inline std::ostream& operator<<(std::ostream& s,
                                        WhiteSpace const& x) throw()
        {
            return s << "Whitespace " << xju::format::quote(x.value());
        }
        
        inline std::ostream& operator<<(std::ostream& s,
                                        UnexpandedName const& x) throw()
        {
            return s << "Unexpanded Name " << xju::format::quote(
                x.first.value()+":"+x.second.value());
        }
        inline std::ostream& operator<<(std::ostream& s,
                                        ExpandedName const& x) throw()
        {
            return s << "Expanded Name " << xju::format::quote(
                x.first.value()+":"+x.second.value());
        }

        ExpandedName expandName(UnexpandedName const& name,
                                NamespaceDefinitions const& namespaces) /*throw(
                                    // name not found
                                    xju::Exception)*/;

        
        template<class RewindableIterator>
        std::pair<std::string, RewindableIterator> parseLiteral(
            RewindableIterator const at,
            std::string literal) /*throw(
                xju::Exception)*/
        {
            try
            {
                std::pair<std::string::const_iterator, RewindableIterator> x =
                    std::mismatch(literal.begin(), literal.end(), at);
                if (x.first != literal.end())
                {
                    // REVISIT: say what is at the location
                    std::ostringstream s;
                    s << xju::format::quote(literal)+" not at " << at;
                    throw xju::Exception(s, XJU_TRACED);
                }
                return std::make_pair(literal, x.second);
            }
            catch(xju::Exception& e)
            {
                std::ostringstream s;
                s << "parse " + xju::format::quote(literal) 
                  << " assumed to be at "
                  << at;
                e.addContext(s, XJU_TRACED);
                throw;
            }
        }
        
        // - result.first does not include literal
        // - result.second points just after literal
        template<class RewindableIterator>
        std::pair<std::string, RewindableIterator> parseUntilLiteral(
            RewindableIterator const at,
            std::string literal) /*throw(
                xju::Exception)*/
        {
            try
            {
                std::ostringstream s;
                RewindableIterator i(at);
                while(1)
                {
                    try
                    {
                        std::pair<std::string, RewindableIterator> x(
                            parseLiteral(i, literal));
                        return std::make_pair(s.str(), x.second);
                    }
                    catch(xju::Exception const&)
                    {
                        if (i.atEnd())
                        {
                            throw;
                        }
                        s << *i++;
                    }
                }
            }
            catch(xju::Exception& e)
            {
                std::ostringstream s;
                s << "parse from " << at << " until first occurance of "
                  << xju::format::quote(literal) ;
                e.addContext(s, XJU_TRACED);
                throw;
            }
        }
        
        template<class RewindableIterator>
        std::pair<Comment, RewindableIterator> parseComment(
            RewindableIterator const at) /*throw(xju::Exception)*/
        {
            try
            {
                std::ostringstream c;
                for(RewindableIterator i(parseLiteral(at, "<--").second);
                    !i.atEnd();
                    ++i)
                {
                    try
                    {
                        std::pair<std::string, RewindableIterator> const y(
                            parseLiteral(i, "-->"));
                        return std::make_pair(Comment(c.str()), y.second);
                    }
                    catch(xju::Exception)
                    {
                        c << *i;
                    }
                }
                throw xju::Exception("comment is not terminated", XJU_TRACED);
            }
            catch(xju::Exception& e)
            {
                std::ostringstream s;
                s << "parse comment assumed to commence at " << at;
                e.addContext(s, XJU_TRACED);
                throw;
            }
        }
        
        template<class RewindableIterator>
        std::pair<WhiteSpace, RewindableIterator> parseWhiteSpace(
            RewindableIterator at) /*throw(xju::Exception)*/
        {
            try
            {
                RewindableIterator i = at;
                if (!std::isspace(*i))
                {
                    throw xju::Exception("there is no whitespace here", XJU_TRACED);
                }
                std::ostringstream s;
                s << *i;
                ++i;
                while(!i.atEnd() && std::isspace(*i))
                {
                    s << *i;
                    ++i;
                }
                return std::make_pair(WhiteSpace(s.str()), i);
            }
            catch(xju::Exception& e)
            {
                std::ostringstream s;
                s << "parse whitespace assumed to be at " << at;
                e.addContext(s, XJU_TRACED);
                throw;
            }
        }

        template<class RewindableIterator>
        RewindableIterator skipWhiteSpaceAndComments(
            RewindableIterator const at) /*throw(
                xju::Exception)*/
        {
            try
            {
                RewindableIterator i(at);
                
                while(!i.atEnd())
                {
                    try
                    {
                        i = parseWhiteSpace(i).second;
                    }
                    catch(xju::Exception)
                    {
                        try
                        {
                            i = parseComment(i).second;
                        }
                        catch(xju::Exception)
                        {
                            return i;
                        }
                    }
                }
                return i;
            }
            catch(xju::Exception& e)
            {
                std::ostringstream s;
                s << "skip whitespace and comments at " << at;
                e.addContext(s, XJU_TRACED);
                throw;
            }
        }

        inline bool isLetter(char x) throw()
        {
            // REVISIT: not correct: see Letter definition in xml spec.
            return ((x >= 'A' && x <= 'Z') ||
                    (x >= 'a' && x <= 'z'));
        }
        inline bool isDigit(char x) throw()
        {
            return std::isdigit(x);
        }
        
        inline bool isNameChar(char c) throw()
        {
            return (isLetter(c) ||
                    isDigit(c) ||
                    (c) == '.' ||
                    (c) == '-' || 
                    (c) == ':' ||
                    // REVISIT: or Combining Char or Extender from xml spec
                    (c) == '_');
        }
        
        inline bool isNCNameChar(char c) throw()
        {
            return (isLetter(c) ||
                    isDigit(c) ||
                    (c) == '.' ||
                    (c) == '-' || 
                    // REVISIT: or Combining Char or Extender from xml spec
                    (c) == '_');
        }
        
        template<class RewindableIterator>
        std::pair<std::string, RewindableIterator> parseName(
            RewindableIterator const at) /*throw(
                xju::Exception)*/
        {
            try
            {
                RewindableIterator i(at);
                if (!isLetter(*i) && 
                    !((*i) == '_') &&
                    !((*i) == ':'))
                {
                    std::ostringstream s;
                    s << xju::format::quote("'", std::string(1, *i))
                      << " is not a Letter "
                      << "or '_' or ':' at " << i;
                    throw xju::Exception(s, XJU_TRACED);
                }
                std::ostringstream n;
                n << (*i++);
                for(; !i.atEnd() && isNameChar(*i); ++i)
                {
                    n << *i;
                }
                return std::make_pair(n.str(), i);
            }
            catch(xju::Exception& e)
            {
                std::ostringstream s;
                s << "parse the Name assumed to commence at " << at;
                e.addContext(s, XJU_TRACED);
                throw;
            }
        }

        std::string lookupEntity(std::string name) /*throw(
            xju::Exception)*/;
        
        template<class RewindableIterator>
        std::pair<std::string, RewindableIterator> parseEntityReference(
            RewindableIterator const at) /*throw(
                xju::Exception)*/
        {
            try
            {
                std::ostringstream s;
                RewindableIterator i(parseLiteral(at, "&").second);
                std::pair<std::string, RewindableIterator> x(parseName(i));
                if (*x.second != ';')
                {
                    std::ostringstream s;
                    s << xju::format::quote("'", std::string(1, *x.second))
                      << " is not ';' at " << x.second;
                    throw xju::Exception(s, XJU_TRACED);
                }
                ++x.second;
                return std::make_pair(lookupEntity(x.first), x.second);
            }
            catch(xju::Exception& e)
            {
                std::ostringstream s;
                s << "parse entity reference assumed to commence at " << at;
                e.addContext(s, XJU_TRACED);
                throw;
            }
        }
        
        template<class RewindableIterator>
        std::pair<char, RewindableIterator> parseDecimalCharacterReference(
            RewindableIterator at) /*throw(
                xju::Exception)*/
        {
            try
            {
                std::ostringstream s;
                RewindableIterator i(parseLiteral(at, "&#").second);
                for(; !i.atEnd() && (*i != ';'); ++i)
                {
                    s << *i;
                }
                if (i.atEnd())
                {
                    throw xju::Exception("character reference is not terminated - i.e. no terminating ';' found", XJU_TRACED);
                }
                unsigned int c = xju::stringToUInt(s.str());
                if (c > 255)
                {
                    std::ostringstream s;
                    s << "character " << c << " is not an 8-bit character";
                    throw xju::Exception(s, XJU_TRACED);
                }
                return std::make_pair(c, ++i);
            }
            catch(xju::Exception& e)
            {
                std::ostringstream s;
                s << "parse character reference (decimal form) assumed to "
                  << "commence at " << at;
                e.addContext(s, XJU_TRACED);
                throw;
            }
        }
        
        template<class RewindableIterator>
        std::pair<char, RewindableIterator> parseHexCharacterReference(
            RewindableIterator at) /*throw(
                xju::Exception)*/
        {
            try
            {
                std::ostringstream s;
                RewindableIterator i(parseLiteral(at, "&#x").second);
                for(; !i.atEnd() && (*i != ';'); ++i)
                {
                    s << *i;
                }
                if (i.atEnd())
                {
                    throw xju::Exception("character reference is not terminated - i.e. no terminating ';' found", XJU_TRACED);
                }
                unsigned int c = xju::stringToUInt(s.str(), 16);
                if (c > 255)
                {
                    std::ostringstream s;
                    s << "character " << c << " is not an 8-bit character";
                    throw xju::Exception(s, XJU_TRACED);
                }
                return std::make_pair(c, ++i);
            }
            catch(xju::Exception& e)
            {
                std::ostringstream s;
                s << "parse character reference (hex form) assumed to "
                  << "commence at " << at;
                e.addContext(s, XJU_TRACED);
                throw;
            }
        }
        
        template<class RewindableIterator>
        std::pair<char, RewindableIterator> parseCharacterReference(
            RewindableIterator at) /*throw(
                xju::Exception)*/
        {
            try
            {
                try
                {
                    return parseHexCharacterReference(at);
                }
                catch(xju::Exception const& e1)
                {
                    try 
                    {
                        return parseDecimalCharacterReference(at);
                    }
                    catch(xju::Exception const& e2)
                    {
                        throw xju::Exception(
                            // REVISIT: do better join
                            readableRepr(e1)+" and "+readableRepr(e2),
                            XJU_TRACED);
                    }
                }
            }
            catch(xju::Exception& e)
            {
                std::ostringstream s;
                s << "parse character reference assumed to commence at " << at;
                e.addContext(s, XJU_TRACED);
                throw;
            }
        }

        template<class RewindableIterator>
        std::pair<NCName, RewindableIterator> parseNCName(
            RewindableIterator const at) /*throw(
                xju::Exception)*/
        {
            try
            {
                RewindableIterator i(at);
                if (!isLetter(*i) && 
                    !((*i) == '_'))
                {
                    std::ostringstream s;
                    s << xju::format::quote("'", std::string(1, *i))
                      << " is not a Letter "
                      << "or '_'";
                    throw xju::Exception(s, XJU_TRACED);
                }
                std::ostringstream n;
                n << (*i++);
                for(; !i.atEnd() && isNCNameChar(*i); ++i)
                {
                    n << *i;
                }
                return std::make_pair(n.str(), i);
            }
            catch(xju::Exception& e)
            {
                std::ostringstream s;
                s << "parse the NCName assumed to commence at " << at;
                e.addContext(s, XJU_TRACED);
                throw;
            }
        }
        
        template<class RewindableIterator>
        std::pair<NameLocalPart, RewindableIterator> parseNameLocalPart(
            RewindableIterator const at) /*throw(
                xju::Exception)*/
        {
            try
            {
                std::pair<NCName, RewindableIterator> const n(
                    parseNCName(at));
                if (!n.second.atEnd() && ((*n.second) == ':'))
                {
                    std::ostringstream s;
                    s << "name terminated by ':' at " << n.second;
                    throw xju::Exception(s, XJU_TRACED);
                }
                return std::make_pair(NameLocalPart(n.first), n.second);
            }
            catch(xju::Exception& e)
            {
                std::ostringstream s;
                s << "parse local part of name assumed to commence at " << at;
                e.addContext(s, XJU_TRACED);
                throw;
            }
        }
        
        template<class RewindableIterator>
        std::pair<NamePrefix, RewindableIterator> parseNamePrefix(
            RewindableIterator at) /*throw(
                xju::Exception)*/
        {
            try
            {
                std::pair<NCName, RewindableIterator> n(
                    parseNCName(at));
                if (n.second.atEnd() || ((*n.second) != ':'))
                {
                    std::ostringstream s;
                    s << "name terminated by non-':' at " << n.second;
                    throw xju::Exception(s, XJU_TRACED);
                }
                ++n.second;
                return std::make_pair(NamePrefix(n.first), n.second);
            }
            catch(xju::Exception& e)
            {
                std::ostringstream s;
                s << "parse name prefix assumed to commence at " << at;
                e.addContext(s, XJU_TRACED);
                throw;
            }
        }
        
        template<class RewindableIterator>
        std::pair<UnexpandedName, RewindableIterator> parseUnexpandedName(
            RewindableIterator const at) /*throw(
                xju::Exception)*/
        {
            try
            {
                NamePrefix prefix("");
                RewindableIterator i(at);
                try
                {
                    std::pair<NamePrefix, RewindableIterator> const x(
                        parseNamePrefix(i));
                    prefix = x.first;
                    i = x.second;
                }
                catch(xju::Exception)
                {
                }
                std::pair<NameLocalPart, RewindableIterator> const x(
                    parseNameLocalPart(i));
                return std::make_pair(std::make_pair(prefix, x.first),
                                      x.second);
            }
            catch(xju::Exception& e)
            {
                std::ostringstream s;
                s << "parse the qualified but unexpanded name assumed to "
                  << "commence at " << at;
                e.addContext(s, XJU_TRACED);
                throw;
            }
        }
        
        template<class RewindableIterator>
        std::pair<AttributeValue, RewindableIterator> parseAttributeValue(
            RewindableIterator const at) /*throw(
                xju::Exception)*/
        {
            try
            {
                RewindableIterator i(at);
                if ((*i) != '"')
                {
                    std::ostringstream s;
                    s << xju::format::quote("'", std::string(1, *i))
                      << " is not '\"' at " << i;
                    throw xju::Exception(s, XJU_TRACED);
                }
                std::ostringstream v;
                ++i;
                try
                {
                    while(*i != '"')
                    {
                        try
                        {
                            std::pair<char, RewindableIterator> const x(
                                parseCharacterReference(i));
                            v << x.first;
                            i = x.second;
                        }
                        catch(xju::Exception)
                        {
                            try
                            {
                                std::pair<std::string, RewindableIterator> 
                                    const x(parseEntityReference(i));
                                v << x.first;
                                i = x.second;
                            }
                            catch(xju::Exception)
                            {
                                if ((*i) == '&')
                                {
                                    std::ostringstream s;
                                    s << "'&' (at " << i << ") is not valid "
                                      << "in attribute values unless it "
                                      << "commences a character or entity "
                                      << "reference";
                                    throw xju::Exception(s, XJU_TRACED);
                                }
                                v << *i++;
                            }
                        }
                    }
                }
                catch(xju::Exception& e)
                {
                    e.addContext("read value until next '\"'", XJU_TRACED);
                    throw;
                }
                ++i;
                return std::make_pair(AttributeValue(v.str()), i);
            }
            catch(xju::Exception& e)
            {
                std::ostringstream s;
                s << "parse the attribute value assumed to commence at " << at;
                e.addContext(s, XJU_TRACED);
                throw;
            }
        }
        
        template<class RewindableIterator>
        std::pair<std::pair<UnexpandedName, AttributeValue>, 
                  RewindableIterator> parseAttribute(
                      RewindableIterator const at) /*throw(
                          xju::Exception)*/
        {
            try
            {
                std::pair<UnexpandedName, RewindableIterator> const x(
                    parseUnexpandedName(at));
                std::pair<std::string, RewindableIterator> const y(
                    parseLiteral(skipWhiteSpaceAndComments(x.second), "="));
                std::pair<AttributeValue, RewindableIterator> const z(
                    parseAttributeValue(skipWhiteSpaceAndComments(y.second)));
                return std::make_pair(
                    std::make_pair(x.first, z.first),
                    z.second);
            }
            catch(xju::Exception& e)
            {
                std::ostringstream s;
                s << "parse attribute definition assumed to commence at " << at;
                e.addContext(s, XJU_TRACED);
                throw;
            }
        }
        
        inline std::ostream& operator<<(
            std::ostream& s,
            std::pair<const NamePrefix, URI> const& x) throw()
        {
            return s << x.first << " = " << x.second;
        }
        
        inline std::ostream& operator<<(std::ostream& s,
                                        NamespaceDefinitions const& x) throw()
        {
            s << "{ ";
            std::copy(x.begin(),
                      x.end(),
                      xju::JoiningIterator<std::pair<const NamePrefix, URI>, 
                                           std::string>(s, ", "));
            s << " }";
            return s;
        }

        inline std::ostream& operator<<(
            std::ostream& s,
            std::pair<ExpandedName const, AttributeValue> const& x) throw()
        {
            return s << x.first << ": " << x.second;
        }
        
        inline std::ostream& operator<<(std::ostream& s,
                                        Attributes const& x) throw()
        {
            s << "{ ";
            std::copy(
                x.begin(),
                x.end(),
                xju::JoiningIterator<
                    std::pair<ExpandedName const, AttributeValue>,
                    std::string>(s, ", "));
            s << " }";
            return s;
        }
        
        template<class RewindableIterator>
        std::pair<Attributes, RewindableIterator> parseAttributes(
            RewindableIterator const at,
            NamespaceDefinitions& namespaceDefinitions,
            const Attributes& attributeDefaults) /*throw(
                xju::Exception)*/
        {
            try
            {
                NamespaceDefinitions ns(namespaceDefinitions);
                std::map<UnexpandedName, AttributeValue> atts;
                RewindableIterator i(at);
                for(;
                    (*i) != '>' && (*i) != '/';
                    i = skipWhiteSpaceAndComments(i))
                {
                    std::pair<std::pair<UnexpandedName, AttributeValue>, 
                        RewindableIterator> const x(
                            parseAttribute(i));
                    i = x.second;
                    UnexpandedName const& n(x.first.first);
                    AttributeValue const& v(x.first.second);
                    
                    if (n.first == NamePrefix("") &&
                        n.second == NameLocalPart("xmlns"))
                    {
                        std::pair<NamespaceDefinitions::iterator, bool> 
                            const y(ns.insert(
                                std::make_pair(NamePrefix(""),
                                               URI(v.value()))));
                        if (!y.second)
                        {
                            (*y.first).second = URI(v.value());
                        }
                    }
                    else if (n.first == NamePrefix("xmlns"))
                    {
                        std::pair<NamespaceDefinitions::iterator, bool>
                            const y(ns.insert(
                                std::make_pair(
                                    NamePrefix(n.second.value()),
                                    URI(v.value()))));
                        if (!y.second)
                        {
                            (*y.first).second = URI(v.value());
                        }
                    }
                    else
                    {
                        std::pair<std::map<UnexpandedName, 
                            AttributeValue>::const_iterator, bool> const y(
                                atts.insert(x.first));
                        if (!y.second)
                        {
                            std::ostringstream s;
                            s << "duplicate value for attribute "
                              << n << " at " << i;
                            throw xju::Exception(s, XJU_TRACED);
                        }
                    }
                    i = x.second;
                }
                NamespaceDefinitions ans(ns);
                (*(ans.insert(
                    std::make_pair(
                        NamePrefix(""), URI(""))).first)).second = URI("");
                Attributes result;
                for(std::map<UnexpandedName, AttributeValue>::const_iterator j(
                    atts.begin());
                    j != atts.end();
                    ++j)
                {
                    ExpandedName const n(expandName((*j).first, ans));
                    std::pair<Attributes::const_iterator, bool> const y(
                        result.insert(std::make_pair(n, (*j).second)));
                    if (!y.second)
                    {
                        std::ostringstream s;
                        s << "duplicate value for attribute " << n;
                        throw xju::Exception(s, XJU_TRACED);
                    }
                }
                std::copy(attributeDefaults.begin(),
                          attributeDefaults.end(),
                          std::inserter(result, result.end()));
                
                std::swap(ns, namespaceDefinitions);
                return std::make_pair(result, i);
            }
            catch(xju::Exception& e)
            {
                std::ostringstream s;
                s << "parse attributes assumed to commence at " << at
                  << ", augmenting namespace definitions " 
                  << namespaceDefinitions
                  << " and attribute defaults " << attributeDefaults;
                e.addContext(s, XJU_TRACED);
                throw;
            }
        }
        
        template<class RewindableIterator>
        std::pair<TagOpening, RewindableIterator> parseTagOpening(
            RewindableIterator const at,
            NamespaceDefinitions& namespaces,
            const Attributes& attributeDefaults,
            RewindableIterator (*skip)(RewindableIterator)) /*throw(
                  xju::Exception)*/
        {
            try
            {
                RewindableIterator i(parseLiteral(at, "<").second);
                std::pair<UnexpandedName, RewindableIterator> const x(
                    parseUnexpandedName(i));
                std::pair<xju::xml::Attributes, RewindableIterator> const y(
                    xju::xml::parseAttributes((*skip)(x.second),
                                              namespaces,
                                              attributeDefaults));
                ExpandedName const tagName(expandName(x.first, namespaces));
                bool simpleTag = false;
                i=y.second;
                if (*i == '/')
                {
                    simpleTag = true;
                    ++i;
                }
                i=parseLiteral(skipWhiteSpaceAndComments(i), ">").second;

                Tag const tag(tagName, y.first);
                return std::make_pair(std::make_pair(tag, simpleTag), 
                                      (*skip)(i));
            }
            catch(xju::Exception& e)
            {
                std::ostringstream s;
                s << "parse tag opening assumed to commence at " << at
                  << ", augmenting namespace definitions "
                  << namespaces
                  << " and applying attribute defaults " << attributeDefaults;
                e.addContext(s, XJU_TRACED);
                throw;
            }
            
        }
        
        template<class RewindableIterator>
        std::pair<ExpandedName, RewindableIterator> parseTagEnd(
            RewindableIterator const at,
            NamespaceDefinitions const& namespaces,
            RewindableIterator (*skip)(RewindableIterator)) /*throw(
                  xju::Exception)*/
        {
            try
            {
                RewindableIterator i(at);
                i = parseLiteral(i, "<").second;
                i = parseLiteral(skipWhiteSpaceAndComments(i), "/").second;
                std::pair<UnexpandedName, RewindableIterator> const x(
                    parseUnexpandedName(skipWhiteSpaceAndComments(i)));
                i = parseLiteral(
                    skipWhiteSpaceAndComments(x.second), ">").second;
                return std::make_pair(expandName(x.first, namespaces),
                                      (*skip)(i));
            }
            catch(xju::Exception& e)
            {
                std::ostringstream s;
                s << "parse end tag assumed to commence at " << at
                  << ", using namespace definitions "
                  << namespaces;
                e.addContext(s, XJU_TRACED);
                throw;
            }
            
        }

        template<class RewindableIterator>
        std::pair<std::string, RewindableIterator> parseEitherQuotedLiteral(
            RewindableIterator at) /*throw(
                xju::Exception)*/
        {
            try
            {
                RewindableIterator i(at);
                try
                {
                    i = parseLiteral(i, "\"").second;
                    return parseUntilLiteral(i, "\"");
                }
                catch(xju::Exception const& e)
                {
                    try
                    {
                        i = parseLiteral(i, "'").second;
                        return parseUntilLiteral(i, "'");
                    }
                    catch(xju::Exception const& f)
                    {
                        std::ostringstream s;
                        s << readableRepr(e) << " and " << readableRepr(f);
                        throw xju::Exception(s, XJU_TRACED);
                    }
                }
            }
            catch(xju::Exception& e)
            {
                std::ostringstream s;
                s << "parse 'x' or \"x\" assumed to commence at " << at;
                e.addContext(s, XJU_TRACED);
                throw;
            }
        }
        

        template<class RewindableIterator>
        std::pair<xju::Optional<std::string>, RewindableIterator> 
        parseOptionalEncoding(RewindableIterator at) /*throw(
            xju::Exception)*/
        {
            try
            {
                RewindableIterator i(at);
                try
                {
                    i=parseLiteral(i, "encoding").second;
                }
                catch(xju::Exception const& e)
                {
                    return std::make_pair(xju::Optional<std::string>(), at);
                }
                i=parseLiteral(skipWhiteSpaceAndComments(i), "=").second;
                std::pair<std::string, RewindableIterator> const x(
                    parseEitherQuotedLiteral(skipWhiteSpaceAndComments(i)));
                return std::make_pair(xju::Optional<std::string>(x.first),
                                      x.second);
            }
            catch(xju::Exception& e)
            {
                std::ostringstream s;
                s << "parse optional \"encoding\" assumed to commence at " 
                  << at;
                e.addContext(s, XJU_TRACED);
                throw;
            }
        }
        

        template<class RewindableIterator>
        std::pair<XmlDecl, RewindableIterator> parseXmlDecl(
            RewindableIterator at,
            RewindableIterator (*skip)(RewindableIterator)) /*throw(
                xju::Exception)*/
        {
            try
            {
                RewindableIterator i(at);
                i=parseLiteral(i, "<").second;
                i=parseLiteral(skipWhiteSpaceAndComments(i), "?").second;
                i=parseLiteral(skipWhiteSpaceAndComments(i), "xml").second;
                i=parseLiteral(skipWhiteSpaceAndComments(i), "version").second;
                i=parseLiteral(skipWhiteSpaceAndComments(i), "=").second;
                std::pair<std::string, RewindableIterator> const version(
                    parseEitherQuotedLiteral(skipWhiteSpaceAndComments(i)));
                std::pair<xju::Optional<std::string>, RewindableIterator> const
                    encoding(parseOptionalEncoding(
                        skipWhiteSpaceAndComments(version.second)));
                i=parseLiteral(skipWhiteSpaceAndComments(encoding.second),
                               "?").second;
                i=parseLiteral(skipWhiteSpaceAndComments(i), ">").second;
                return std::make_pair(XmlDecl(version.first, 
                                              encoding.first),
                                      (*skip)(i));
            }
            catch(xju::Exception& e)
            {
                std::ostringstream s;
                s << "parse XML Declaration assumed to commence at " << at;
                e.addContext(s, XJU_TRACED);
                throw;
            }
        }
    }
}

#endif

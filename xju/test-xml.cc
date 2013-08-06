//     -*- mode: c++ ; c-file-style: "osse" ; -*-
//
// Copyright (c) 2007
// Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/xml.hh>

#include <utility>
#include <string>
#include <xju/assert.hh>
#include <iostream>
#include <xju/parse.hh>

typedef xju::parse::IteratorAdaptor<std::string::const_iterator> I;

template<class T>
class V : public std::pair<T, I>
{
public:
    V(std::pair<T, I> x) throw():
        std::pair<T, I>(x)
    {
    }
};


void test1()
{
    {
        std::string const c("<-- abc -->def");
        I i(c.begin(), c.end());
        
        V<xju::xml::Comment> const x(xju::xml::parseComment(i));
        xju::assert_equal(x.first.value(), " abc ");
        xju::assert_equal(*x.second, 'd');
    }
    {
        std::string const c("<---->");
        I i(c.begin(), c.end());
        
        V<xju::xml::Comment> const x(xju::xml::parseComment(i));
        xju::assert_equal(x.second, I(c.end(), c.end()));
    }
}

void test2()
{
    std::string const c("<-- abc");
    I i(c.begin(), c.end());
    try
    {
        V<xju::xml::Comment> const x(xju::xml::parseComment(i));
        xju::assert_abort();
    }
    catch(xju::Exception const& e)
    {
        xju::assert_equal(readableRepr(e), "Failed to parse comment assumed to commence at line 1 column 1 because\ncomment is not terminated.");
    }
}

void test3()
{
    std::string const c("   x");
    I i(c.begin(), c.end());
    V<xju::xml::WhiteSpace> const x(xju::xml::parseWhiteSpace(i));
    xju::assert_equal(x.first.value(), "   ");
    xju::assert_equal(*x.second, 'x');
    try
    {
        V<xju::xml::WhiteSpace> const y = xju::xml::parseWhiteSpace(x.second);
        xju::assert_abort();
    }
    catch(xju::Exception const& e)
    {
        xju::assert_equal(readableRepr(e), "Failed to parse whitespace assumed to be at line 1 column 4 because\nthere is no whitespace here.");
    }
}

void test4()
{
    std::string const c("   <-- fred --> <--fred--> x");
    I i(c.begin(), c.end());
    I i2 = xju::xml::skipWhiteSpaceAndComments(i);
    xju::assert_equal(*i2, 'x');
}

void test5()
{
    std::string const c("abc");
    I i(c.begin(), c.end());
    xju::assert_equal(i.line_, 1);
    xju::assert_equal(i.column_, 1);
    xju::assert_equal(*i++, 'a');
    xju::assert_equal(i.line_, 1);
    xju::assert_equal(i.column_, 2);
    xju::assert_equal(*++i, 'c');
    xju::assert_equal(i.line_, 1);
    xju::assert_equal(i.column_, 3);
    xju::assert_equal(*i++, 'c');
    xju::assert_equal(i.line_, 1);
    xju::assert_equal(i.column_, 4);

    std::string const d("\n\na\nb");
    I j(d.begin(), d.end());
    xju::assert_equal(j.line_, 1);
    xju::assert_equal(j.column_, 1);
    ++j;
    xju::assert_equal(j.line_, 2);
    xju::assert_equal(j.column_, 1);
    ++j;
    xju::assert_equal(j.line_, 3);
    xju::assert_equal(j.column_, 1);
    ++j;
    xju::assert_equal(j.line_, 3);
    xju::assert_equal(j.column_, 2);
    ++j;
    xju::assert_equal(j.line_, 4);
    xju::assert_equal(j.column_, 1);
    ++j;
    xju::assert_equal(j.line_, 4);
    xju::assert_equal(j.column_, 2);
    xju::assert_equal(j.atEnd(), true);
}

void test6()
{
    std::string const c("&#46;");
    I i(c.begin(), c.end());
    
    std::pair<char, I> x = xju::xml::parseDecimalCharacterReference(i);
    xju::assert_equal(x.first, 46);
    xju::assert_equal(x.second.atEnd(), true);

    try
    {
        std::string const c("#46;");
        I i(c.begin(), c.end());
        std::pair<char, I> x = xju::xml::parseDecimalCharacterReference(i);
        xju::assert_abort();
    }
    catch(xju::Exception& e)
    {
        xju::assert_equal(readableRepr(e), "Failed to parse character reference (decimal form) assumed to commence at line 1 column 1 because\nfailed to parse \"&#\" assumed to be at line 1 column 1 because\n\"&#\" not at line 1 column 1.");
    }
    try
    {
        std::string const c("&#46");
        I i(c.begin(), c.end());
        std::pair<char, I> x = xju::xml::parseDecimalCharacterReference(i);
        xju::assert_abort();
    }
    catch(xju::Exception& e)
    {
        xju::assert_equal(readableRepr(e), "Failed to parse character reference (decimal form) assumed to commence at line 1 column 1 because\ncharacter reference is not terminated - i.e. no terminating ';' found.");
    }
    try
    {
        std::string const c("&#1000;");
        I i(c.begin(), c.end());
        std::pair<char, I> x = xju::xml::parseDecimalCharacterReference(i);
        xju::assert_abort();
    }
    catch(xju::Exception& e)
    {
        xju::assert_equal(readableRepr(e), "Failed to parse character reference (decimal form) assumed to commence at line 1 column 1 because\ncharacter 1000 is not an 8-bit character.");
    }
    try
    {
        std::string const c("&#t42;");
        I i(c.begin(), c.end());
        std::pair<char, I> x = xju::xml::parseDecimalCharacterReference(i);
        xju::assert_abort();
    }
    catch(xju::Exception& e)
    {
        xju::assert_equal(readableRepr(e), "Failed to parse character reference (decimal form) assumed to commence at line 1 column 1 because\nfailed to convert \"t42\" to a base-10 unsigned integer because\ncharacter 1 ('t') of \"t42\" unexpected.");
    }
}

void test7()
{
    std::string const c("&#x2a;");
    I i(c.begin(), c.end());
    
    std::pair<char, I> x = xju::xml::parseHexCharacterReference(i);
    xju::assert_equal(x.first, 0x2a);
    xju::assert_equal(x.second.atEnd(), true);

    try
    {
        std::string const c("#x46;");
        I i(c.begin(), c.end());
        std::pair<char, I> x = xju::xml::parseHexCharacterReference(i);
        xju::assert_abort();
    }
    catch(xju::Exception& e)
    {
        xju::assert_equal(readableRepr(e), "Failed to parse character reference (hex form) assumed to commence at line 1 column 1 because\nfailed to parse \"&#x\" assumed to be at line 1 column 1 because\n\"&#x\" not at line 1 column 1.");
    }
    try
    {
        std::string const c("&#x46");
        I i(c.begin(), c.end());
        std::pair<char, I> x = xju::xml::parseHexCharacterReference(i);
        xju::assert_abort();
    }
    catch(xju::Exception& e)
    {
        xju::assert_equal(readableRepr(e), "Failed to parse character reference (hex form) assumed to commence at line 1 column 1 because\ncharacter reference is not terminated - i.e. no terminating ';' found.");
    }
    try
    {
        std::string const c("&#x1000;");
        I i(c.begin(), c.end());
        std::pair<char, I> x = xju::xml::parseHexCharacterReference(i);
        xju::assert_abort();
    }
    catch(xju::Exception& e)
    {
        xju::assert_equal(readableRepr(e), "Failed to parse character reference (hex form) assumed to commence at line 1 column 1 because\ncharacter 4096 is not an 8-bit character.");
    }
    try
    {
        std::string const c("&#xt42;");
        I i(c.begin(), c.end());
        std::pair<char, I> x = xju::xml::parseHexCharacterReference(i);
        xju::assert_abort();
    }
    catch(xju::Exception& e)
    {
        xju::assert_equal(readableRepr(e), "Failed to parse character reference (hex form) assumed to commence at line 1 column 1 because\nfailed to convert \"t42\" to a base-16 unsigned integer because\ncharacter 1 ('t') of \"t42\" unexpected.");
    }
}

void test8()
{
    {
        std::string const c("&#x2a;");
        I i(c.begin(), c.end());
        
        std::pair<char, I> x = xju::xml::parseCharacterReference(i);
        xju::assert_equal(x.first, 0x2a);
        xju::assert_equal(x.second.atEnd(), true);
    }
    {
        std::string const c("&#22;");
        I i(c.begin(), c.end());
        
        std::pair<char, I> x = xju::xml::parseCharacterReference(i);
        xju::assert_equal(x.first, 22);
        xju::assert_equal(x.second.atEnd(), true);
    }
    
    try
    {
        std::string const c("#xxx;");
        I i(c.begin(), c.end());
        std::pair<char, I> x = xju::xml::parseCharacterReference(i);
        xju::assert_abort();
    }
    catch(xju::Exception& e)
    {
        xju::assert_equal(readableRepr(e), "Failed to parse character reference assumed to commence at line 1 column 1 because\nFailed to parse character reference (hex form) assumed to commence at line 1 column 1 because\nfailed to parse \"&#x\" assumed to be at line 1 column 1 because\n\"&#x\" not at line 1 column 1. and Failed to parse character reference (decimal form) assumed to commence at line 1 column 1 because\nfailed to parse \"&#\" assumed to be at line 1 column 1 because\n\"&#\" not at line 1 column 1..");
    }
}

void test9()
{
    {
        std::string const c("fred");
        I i(c.begin(), c.end());
        
        std::pair<xju::xml::NCName, I> x = xju::xml::parseNCName(i);
        xju::assert_equal(x.first, xju::xml::NCName("fred"));
        xju::assert_equal(x.second, I(c.end(), c.end()));
    }
    {
        std::string const c("_Fred97 ");
        I i(c.begin(), c.end());
        
        std::pair<xju::xml::NCName, I> x = xju::xml::parseNCName(i);
        xju::assert_equal(x.first, xju::xml::NCName("_Fred97"));
        xju::assert_equal(++x.second, I(c.end(), c.end()));
    }
    {
        std::string const c("76er");
        I i(c.begin(), c.end());
        try
        {
            std::pair<xju::xml::NCName, I> x = xju::xml::parseNCName(i);
            xju::assert_abort();
        }
        catch(xju::Exception const& e)
        {
            xju::assert_equal(readableRepr(e), "Failed to parse the NCName assumed to commence at line 1 column 1 because\n'7' is not a Letter or '_'.");
        }
    }
}

void test10()
{
    {
        std::string const c("fred");
        I i(c.begin(), c.end());
        
        std::pair<xju::xml::NameLocalPart, I> const x = 
            xju::xml::parseNameLocalPart(i);
        xju::assert_equal(x.first, xju::xml::NameLocalPart("fred"));
        xju::assert_equal(x.second, I(c.end(), c.end()));
    }
    
    {
        std::string const c("fred:jock");
        I i(c.begin(), c.end());
        try
        {
            std::pair<xju::xml::NameLocalPart, I> const x = 
                xju::xml::parseNameLocalPart(i);
            xju::assert_abort();
        }
        catch(xju::Exception const& e)
        {
            xju::assert_equal(readableRepr(e), "Failed to parse local part of name assumed to commence at line 1 column 1 because\nname terminated by ':' at line 1 column 5.");
        }
    }
}

void test11()
{
    {
        std::string const c("fred:");
        I i(c.begin(), c.end());
        
        std::pair<xju::xml::NamePrefix, I> const x = 
            xju::xml::parseNamePrefix(i);
        xju::assert_equal(x.first, xju::xml::NamePrefix("fred"));
        xju::assert_equal(x.second, I(c.end(), c.end()));
    }
    
    {
        std::string const c("fred ");
        I i(c.begin(), c.end());
        try
        {
            std::pair<xju::xml::NamePrefix, I> const x = 
                xju::xml::parseNamePrefix(i);
            xju::assert_abort();
        }
        catch(xju::Exception const& e)
        {
            xju::assert_equal(readableRepr(e), "Failed to parse name prefix assumed to commence at line 1 column 1 because\nname terminated by non-':' at line 1 column 5.");
        }
    }
    {
        std::string const c("fred");
        I i(c.begin(), c.end());
        try
        {
            std::pair<xju::xml::NamePrefix, I> const x = 
                xju::xml::parseNamePrefix(i);
            xju::assert_abort();
        }
        catch(xju::Exception const& e)
        {
            xju::assert_equal(readableRepr(e), "Failed to parse name prefix assumed to commence at line 1 column 1 because\nname terminated by non-':' at line 1 column 5.");
        }
    }
}

void test12()
{
    {
        std::string const c("jock");
        I i(c.begin(), c.end());
        
        std::pair<xju::xml::UnexpandedName, I> const x = 
            xju::xml::parseUnexpandedName(i);
        xju::assert_equal(x.first, xju::xml::UnexpandedName(
            xju::xml::NamePrefix(""),
            xju::xml::NameLocalPart("jock")));
        xju::assert_equal(x.second, I(c.end(), c.end()));
    }
    {
        std::string const c("fred:jock");
        I i(c.begin(), c.end());
        
        std::pair<xju::xml::UnexpandedName, I> const x = 
            xju::xml::parseUnexpandedName(i);
        xju::assert_equal(x.first, xju::xml::UnexpandedName(
            xju::xml::NamePrefix("fred"),
            xju::xml::NameLocalPart("jock")));
        xju::assert_equal(x.second, I(c.end(), c.end()));
    }
    {
        std::string const c("fred:jock:");
        I i(c.begin(), c.end());
        
        try
        {
            std::pair<xju::xml::UnexpandedName, I> const x = 
                xju::xml::parseUnexpandedName(i);
            xju::assert_abort();
        }
        catch(xju::Exception const& e)
        {
            xju::assert_equal(readableRepr(e), "Failed to parse the qualified but unexpanded name assumed to commence at line 1 column 1 because\nfailed to parse local part of name assumed to commence at line 1 column 6 because\nname terminated by ':' at line 1 column 10.");
        }
    }
    {
        std::string const c("fred:54");
        I i(c.begin(), c.end());
        
        try
        {
            std::pair<xju::xml::UnexpandedName, I> const x = 
                xju::xml::parseUnexpandedName(i);
            xju::assert_abort();
        }
        catch(xju::Exception const& e)
        {
            xju::assert_equal(readableRepr(e), "Failed to parse the qualified but unexpanded name assumed to commence at line 1 column 1 because\nfailed to parse local part of name assumed to commence at line 1 column 6 because\nfailed to parse the NCName assumed to commence at line 1 column 6 because\n'5' is not a Letter or '_'.");
        }
    }
    {
        std::string const c(":jock");
        I i(c.begin(), c.end());
        
        try
        {
            std::pair<xju::xml::UnexpandedName, I> const x = 
                xju::xml::parseUnexpandedName(i);
            xju::assert_abort();
        }
        catch(xju::Exception const& e)
        {
            xju::assert_equal(readableRepr(e), "Failed to parse the qualified but unexpanded name assumed to commence at line 1 column 1 because\nfailed to parse local part of name assumed to commence at line 1 column 1 because\nfailed to parse the NCName assumed to commence at line 1 column 1 because\n':' is not a Letter or '_'.");
        }
    }
    
}

void test13()
{
    {
        std::string const c("jock");
        I i(c.begin(), c.end());
        
        std::pair<std::string, I> const x = 
            xju::xml::parseName(i);
        xju::assert_equal(x.first, "jock");
        xju::assert_equal(x.second, I(c.end(), c.end()));
    }
    {
        std::string const c("Jock ");
        I i(c.begin(), c.end());
        
        std::pair<std::string, I> x = 
            xju::xml::parseName(i);
        xju::assert_equal(x.first, "Jock");
        ++x.second;
        xju::assert_equal(x.second, I(c.end(), c.end()));
    }
    {
        std::string const c("_Jock:");
        I i(c.begin(), c.end());
        
        std::pair<std::string, I> x = 
            xju::xml::parseName(i);
        xju::assert_equal(x.first, "_Jock:");
        xju::assert_equal(x.second, I(c.end(), c.end()));
    }
    
}

void test14()
{
    {
        std::string const c("&amp;");
        I i(c.begin(), c.end());
        
        std::pair<std::string, I> const x = 
            xju::xml::parseEntityReference(i);
        xju::assert_equal(x.first, "&");
        xju::assert_equal(x.second, I(c.end(), c.end()));
    }
    {
        std::string const c("&jock;");
        I i(c.begin(), c.end());
        try
        {
            std::pair<std::string, I> const x = 
                xju::xml::parseEntityReference(i);
            xju::assert_abort();
        }
        catch(xju::Exception& e)
        {
            xju::assert_equal(readableRepr(e), "Failed to parse entity reference assumed to commence at line 1 column 1 because\nentity jock is not defined.");
        }
    }
    {
        std::string const c("&jock ;");
        I i(c.begin(), c.end());
        try
        {
            std::pair<std::string, I> const x = 
                xju::xml::parseEntityReference(i);
            xju::assert_abort();
        }
        catch(xju::Exception& e)
        {
            xju::assert_equal(readableRepr(e), "Failed to parse entity reference assumed to commence at line 1 column 1 because\n' ' is not ';' at line 1 column 6.");
        }
    }
    {
        std::string const c("& jock;");
        I i(c.begin(), c.end());
        try
        {
            std::pair<std::string, I> const x = 
                xju::xml::parseEntityReference(i);
            xju::assert_abort();
        }
        catch(xju::Exception& e)
        {
            xju::assert_equal(readableRepr(e), "Failed to parse entity reference assumed to commence at line 1 column 1 because\nfailed to parse the Name assumed to commence at line 1 column 2 because\n' ' is not a Letter or '_' or ':' at line 1 column 2.");
        }
    }
    {
        std::string const c("&;");
        I i(c.begin(), c.end());
        try
        {
            std::pair<std::string, I> const x = 
                xju::xml::parseEntityReference(i);
            xju::assert_abort();
        }
        catch(xju::Exception& e)
        {
            xju::assert_equal(readableRepr(e), "Failed to parse entity reference assumed to commence at line 1 column 1 because\nfailed to parse the Name assumed to commence at line 1 column 2 because\n';' is not a Letter or '_' or ':' at line 1 column 2.");
        }
    }
}

void test15()
{
    {
        std::string const c("\" 1c;&#36;=\"");
        I i(c.begin(), c.end());
        
        std::pair<xju::xml::AttributeValue, I> const x = 
            xju::xml::parseAttributeValue(i);
        xju::assert_equal(
            x.first, xju::xml::AttributeValue(" 1c;"+std::string(1, 36)+"="));
        xju::assert_equal(x.second, I(c.end(), c.end()));
    }
    {
        std::string const c("\" 1c;&#36;=");
        I i(c.begin(), c.end());
        try
        {
            std::pair<xju::xml::AttributeValue, I> const x = 
                xju::xml::parseAttributeValue(i);
            xju::assert_abort();
        }
        catch(xju::Exception const& e)
        {
            xju::assert_equal(readableRepr(e), "Failed to parse the attribute value assumed to commence at line 1 column 1 because\nfailed to read value until next '\"' because\nend of input at line 1 column 12.");
        }
    }
    {
        std::string const c("\" 1c;&fred\"");
        I i(c.begin(), c.end());
        try
        {
            std::pair<xju::xml::AttributeValue, I> const x = 
                xju::xml::parseAttributeValue(i);
            xju::assert_abort();
        }
        catch(xju::Exception const& e)
        {
            xju::assert_equal(readableRepr(e), "Failed to parse the attribute value assumed to commence at line 1 column 1 because\nfailed to read value until next '\"' because\n'&' (at line 1 column 6) is not valid in attribute values unless it commences a character or entity reference.");
        }
    }
}

void test16()
{
    {
        std::string const c("x =  \"fred\"");
        I i(c.begin(), c.end());
        
        std::pair<std::pair<xju::xml::UnexpandedName, xju::xml::AttributeValue>, I> const x = 
            xju::xml::parseAttribute(i);
        xju::assert_equal(
            x.first.first, 
            xju::xml::UnexpandedName(xju::xml::NamePrefix(""),
                                     xju::xml::NameLocalPart("x")));
        xju::assert_equal(
            x.first.second, xju::xml::AttributeValue("fred"));
        xju::assert_equal(x.second, I(c.end(), c.end()));
    }
    {
        std::string const c("x=7");
        I i(c.begin(), c.end());
        try
        {
            std::pair<std::pair<xju::xml::UnexpandedName, 
                xju::xml::AttributeValue>, I> const x = 
                xju::xml::parseAttribute(i);
            xju::assert_abort();
        }
        catch(xju::Exception const& e)
        {
            xju::assert_equal(readableRepr(e), "Failed to parse attribute definition assumed to commence at line 1 column 1 because\nfailed to parse the attribute value assumed to commence at line 1 column 3 because\n'7' is not '\"' at line 1 column 3.");
        }
    }
}

void test17()
{
    {
        std::string const c("x=\"fred\" z:y=\"jock\" a=\"A\">");
        xju::xml::NamespaceDefinitions ns;
        ns.insert(std::make_pair(xju::xml::NamePrefix("z"),
                                 xju::xml::URI("/z.org")));
        ns.insert(std::make_pair(xju::xml::NamePrefix(""),
                                 xju::xml::URI("/d.org")));
        xju::xml::Attributes d;
        d.insert(std::make_pair(
            std::make_pair(xju::xml::URI(""), 
                           xju::xml::NameLocalPart("a")),
            xju::xml::AttributeValue("-A")));
        d.insert(std::make_pair(
            std::make_pair(xju::xml::URI(""), 
                           xju::xml::NameLocalPart("b")),
            xju::xml::AttributeValue("-B")));

        std::pair<xju::xml::Attributes, I> const x(
            xju::xml::parseAttributes(I(c.begin(), c.end()),
                                      ns,
                                      d));

        xju::assert_equal(x.first.size(), 4U);
        xju::assert_not_equal(
            x.first.find(std::make_pair(xju::xml::URI(""),
                                        xju::xml::NameLocalPart("x"))),
            x.first.end());
        xju::assert_equal(
            (*x.first.find(xju::xml::ExpandedName(
                xju::xml::URI(""),
                xju::xml::NameLocalPart("x")))).second,
            xju::xml::AttributeValue("fred"));
        xju::assert_not_equal(
            x.first.find(std::make_pair(xju::xml::URI("/z.org"),
                                        xju::xml::NameLocalPart("y"))),
            x.first.end());
        xju::assert_equal(
            (*x.first.find(xju::xml::ExpandedName(
                xju::xml::URI("/z.org"),
                xju::xml::NameLocalPart("y")))).second,
            xju::xml::AttributeValue("jock"));
        xju::assert_not_equal(
            x.first.find(std::make_pair(xju::xml::URI(""),
                                        xju::xml::NameLocalPart("a"))),
            x.first.end());
        xju::assert_equal(
            (*x.first.find(xju::xml::ExpandedName(
                xju::xml::URI(""),
                xju::xml::NameLocalPart("a")))).second,
            xju::xml::AttributeValue("A"));
        xju::assert_not_equal(
            x.first.find(std::make_pair(xju::xml::URI(""),
                                        xju::xml::NameLocalPart("b"))),
            x.first.end());
        xju::assert_equal(
            (*x.first.find(xju::xml::ExpandedName(
                xju::xml::URI(""),
                xju::xml::NameLocalPart("b")))).second,
            xju::xml::AttributeValue("-B"));
        xju::assert_equal(*x.second, '>');
    }
    {
        std::string const c("x=\"fred\" xmlns=\"/e.org\" z:y=\"jock\" xmlns:z=\"/zz.org\" a=\"A\"/>");
        xju::xml::NamespaceDefinitions ns;
        ns.insert(std::make_pair(xju::xml::NamePrefix("z"),
                                 xju::xml::URI("/z.org")));
        ns.insert(std::make_pair(xju::xml::NamePrefix(""),
                                 xju::xml::URI("/d.org")));
        xju::xml::Attributes d;
        d.insert(std::make_pair(
            std::make_pair(xju::xml::URI(""), 
                           xju::xml::NameLocalPart("a")),
            xju::xml::AttributeValue("-A")));
        std::pair<xju::xml::Attributes, I> const x(
            xju::xml::parseAttributes(I(c.begin(), c.end()),
                                      ns,
                                      d));

        xju::assert_equal(
            (*ns.find(xju::xml::NamePrefix(""))).second,
            xju::xml::URI("/e.org"));
        
        xju::assert_not_equal(
            x.first.find(std::make_pair(xju::xml::URI(""),
                                        xju::xml::NameLocalPart("x"))),
            x.first.end());
        xju::assert_equal(
            (*x.first.find(xju::xml::ExpandedName(
                xju::xml::URI(""),
                xju::xml::NameLocalPart("x")))).second,
            xju::xml::AttributeValue("fred"));
        xju::assert_not_equal(
            x.first.find(std::make_pair(xju::xml::URI("/zz.org"),
                                        xju::xml::NameLocalPart("y"))),
            x.first.end());
        xju::assert_equal(
            (*x.first.find(xju::xml::ExpandedName(
                xju::xml::URI("/zz.org"),
                xju::xml::NameLocalPart("y")))).second,
            xju::xml::AttributeValue("jock"));
        xju::assert_not_equal(
            x.first.find(std::make_pair(xju::xml::URI(""),
                                        xju::xml::NameLocalPart("a"))),
            x.first.end());
        xju::assert_equal(
            (*x.first.find(xju::xml::ExpandedName(
                xju::xml::URI(""),
                xju::xml::NameLocalPart("a")))).second,
            xju::xml::AttributeValue("A"));

        xju::assert_equal(x.first.size(), 3U);

        xju::assert_equal(*x.second, '/');
    }
    {
        std::string const c("x=\"fred\" xmlns=\"/e.org\" z:y=\"jock\" xmlns:Z=\"/zz.org\" a=\"A\"/>");
        xju::xml::NamespaceDefinitions ns;
        ns.insert(std::make_pair(xju::xml::NamePrefix(""),
                                 xju::xml::URI("/d.org")));
        xju::xml::Attributes d;
        d.insert(std::make_pair(
            std::make_pair(xju::xml::URI("/e.org"), 
                           xju::xml::NameLocalPart("a")),
            xju::xml::AttributeValue("-A")));
        try
        {
            std::pair<xju::xml::Attributes, I> const x(
                xju::xml::parseAttributes(I(c.begin(), c.end()),
                                          ns,
                                          d));
            xju::assert_abort();
        }
        catch(xju::Exception const& e)
        {
            xju::assert_equal(readableRepr(e), "Failed to parse attributes assumed to commence at line 1 column 1, augmenting namespace definitions { Name Prefix \"\" = URI \"/d.org\" } and attribute defaults { Expanded Name \"/e.org:a\": Attribute Value \"-A\" } because\nfailed to expand Unexpanded Name \"z:y\" using namespaces { Name Prefix \"\" = URI \"\", Name Prefix \"Z\" = URI \"/zz.org\" } because\nunknown namespace z in Unexpanded Name \"z:y\".");
        }
    }
    {
        std::string const c("x=\"fred\" xmlns=\"/e.org\" z:y=\"jock\"");
        xju::xml::NamespaceDefinitions ns;
        xju::xml::Attributes d;
        try
        {
            std::pair<xju::xml::Attributes, I> const x(
                xju::xml::parseAttributes(I(c.begin(), c.end()),
                                          ns,
                                          d));
            xju::assert_abort();
        }
        catch(xju::Exception const& e)
        {
            xju::assert_equal(readableRepr(e), "Failed to parse attributes assumed to commence at line 1 column 1, augmenting namespace definitions {  } and attribute defaults {  } because\nend of input at line 1 column 35.");
        }
    }
    // REVISIT: do examples from end of spec section 6
}

void test18()
{
    {
        std::string const c("<T x=\"fred\" z:y=\"jock\" xmlns:z=\"/z.org\"/>");
        xju::xml::NamespaceDefinitions ns;
        ns.insert(std::make_pair(xju::xml::NamePrefix(""),
                                 xju::xml::URI("/d.org")));
        xju::xml::Attributes d;
        d.insert(std::make_pair(
            std::make_pair(xju::xml::URI("/e.org"), 
                           xju::xml::NameLocalPart("a")),
            xju::xml::AttributeValue("-A")));

        std::pair<xju::xml::TagOpening, I> const x(
            xju::xml::parseTagOpening(I(c.begin(), c.end()), ns, d));
        
        xju::assert_equal(x.first.second, true);
        xju::assert_equal(x.first.first.first,
                          xju::xml::ExpandedName(
                              xju::xml::URI("/d.org"),
                              xju::xml::NameLocalPart("T")));
        xju::xml::Attributes const& atts(x.first.first.second);
        xju::assert_not_equal(
            atts.find(std::make_pair(xju::xml::URI(""),
                                        xju::xml::NameLocalPart("x"))),
            atts.end());
        xju::assert_equal(
            (*atts.find(xju::xml::ExpandedName(
                xju::xml::URI(""),
                xju::xml::NameLocalPart("x")))).second,
            xju::xml::AttributeValue("fred"));
        xju::assert_not_equal(
            atts.find(std::make_pair(xju::xml::URI("/z.org"),
                                        xju::xml::NameLocalPart("y"))),
            atts.end());
        xju::assert_equal(
            (*atts.find(xju::xml::ExpandedName(
                xju::xml::URI("/z.org"),
                xju::xml::NameLocalPart("y")))).second,
            xju::xml::AttributeValue("jock"));
        xju::assert_not_equal(
            atts.find(std::make_pair(xju::xml::URI("/e.org"),
                                        xju::xml::NameLocalPart("a"))),
            atts.end());
        xju::assert_equal(
            (*atts.find(xju::xml::ExpandedName(
                xju::xml::URI("/e.org"),
                xju::xml::NameLocalPart("a")))).second,
            xju::xml::AttributeValue("-A"));
        xju::assert_equal(atts.size(), 3U);
        xju::assert_equal(x.second, I(c.end(), c.end()));
    }
    
    {
        std::string const c("<T>");
        xju::xml::NamespaceDefinitions ns;
        ns.insert(std::make_pair(xju::xml::NamePrefix(""),
                                 xju::xml::URI("/d.org")));
        xju::xml::Attributes d;

        std::pair<xju::xml::TagOpening, I> const x(
            xju::xml::parseTagOpening(I(c.begin(), c.end()), ns, d));
        
        xju::assert_equal(x.first.second, false);
        xju::assert_equal(x.first.first.first,
                          xju::xml::ExpandedName(
                              xju::xml::URI("/d.org"),
                              xju::xml::NameLocalPart("T")));
        xju::xml::Attributes const& atts(x.first.first.second);
        xju::assert_equal(atts.size(), 0U);
        xju::assert_equal(x.second, I(c.end(), c.end()));
    }
    {
        std::string const c("<x:T xmlns:x=\"/x.org\">");
        xju::xml::NamespaceDefinitions ns;
        xju::xml::Attributes d;

        std::pair<xju::xml::TagOpening, I> const x(
            xju::xml::parseTagOpening(I(c.begin(), c.end()), ns, d));
        
        xju::assert_equal(x.first.second, false);
        xju::assert_equal(x.first.first.first,
                          xju::xml::ExpandedName(
                              xju::xml::URI("/x.org"),
                              xju::xml::NameLocalPart("T")));
        xju::xml::Attributes const& atts(x.first.first.second);
        xju::assert_equal(atts.size(), 0U);
        xju::assert_equal(x.second, I(c.end(), c.end()));

        xju::assert_equal(
            (*ns.find(xju::xml::NamePrefix("x"))).second,
            xju::xml::URI("/x.org"));
    }
    {
        std::string const c("<x:T>");
        xju::xml::NamespaceDefinitions ns;
        ns.insert(std::make_pair(xju::xml::NamePrefix(""),
                                 xju::xml::URI("/d.org")));
        xju::xml::Attributes d;

        try
        {
            std::pair<xju::xml::TagOpening, I> const x(
                xju::xml::parseTagOpening(I(c.begin(), c.end()), ns, d));
            xju::assert_abort();
        }
        catch(xju::Exception const& e)
        {
            xju::assert_equal(readableRepr(e), "Failed to parse tag opening assumed to commence at line 1 column 1, augmenting namespace definitions { Name Prefix \"\" = URI \"/d.org\" } and applying attribute defaults {  } because\nfailed to expand Unexpanded Name \"x:T\" using namespaces { Name Prefix \"\" = URI \"/d.org\" } because\nunknown namespace x in Unexpanded Name \"x:T\".");
        }
    }
    {
        std::string const c("<T>");
        xju::xml::NamespaceDefinitions ns;
        xju::xml::Attributes d;

        try
        {
            std::pair<xju::xml::TagOpening, I> const x(
                xju::xml::parseTagOpening(I(c.begin(), c.end()), ns, d));
            xju::assert_abort();
        }
        catch(xju::Exception const& e)
        {
            xju::assert_equal(readableRepr(e), "Failed to parse tag opening assumed to commence at line 1 column 1, augmenting namespace definitions {  } and applying attribute defaults {  } because\nfailed to expand Unexpanded Name \":T\" using namespaces {  } because\nunknown namespace  in Unexpanded Name \":T\".");
        }
    }
}

void test19()
{
    {
        std::string const c("</T>");
        xju::xml::NamespaceDefinitions ns;
        ns.insert(std::make_pair(xju::xml::NamePrefix(""),
                                 xju::xml::URI("/d.org")));
        std::pair<xju::xml::ExpandedName, I> const x(
            xju::xml::parseTagEnd(I(c.begin(), c.end()), ns));
        xju::assert_equal(x.first, 
                          xju::xml::ExpandedName(
                              xju::xml::URI("/d.org"),
                              xju::xml::NameLocalPart("T")));
        xju::assert_equal(x.second, I(c.end(), c.end()));
    }
    {
        std::string const c("< / x:T >");
        xju::xml::NamespaceDefinitions ns;
        ns.insert(std::make_pair(xju::xml::NamePrefix("x"),
                                 xju::xml::URI("/d.org")));
        std::pair<xju::xml::ExpandedName, I> const x(
            xju::xml::parseTagEnd(I(c.begin(), c.end()), ns));
        xju::assert_equal(x.first, 
                          xju::xml::ExpandedName(
                              xju::xml::URI("/d.org"),
                              xju::xml::NameLocalPart("T")));
        xju::assert_equal(x.second, I(c.end(), c.end()));
    }
    {
        std::string const c("<x:T>");
        xju::xml::NamespaceDefinitions ns;
        ns.insert(std::make_pair(xju::xml::NamePrefix("x"),
                                 xju::xml::URI("/d.org")));
        try
        {
            std::pair<xju::xml::ExpandedName, I> const x(
                xju::xml::parseTagEnd(I(c.begin(), c.end()), ns));
            xju::assert_abort();
        }
        catch(xju::Exception const& e)
        {
            xju::assert_equal(readableRepr(e), "Failed to parse end tag assumed to commence at line 1 column 1, using namespace definitions { Name Prefix \"x\" = URI \"/d.org\" } because\nfailed to parse \"/\" assumed to be at line 1 column 2 because\n\"/\" not at line 1 column 2.");
        }
    }
    {
        std::string const c("</x:T a=\"1\">");
        xju::xml::NamespaceDefinitions ns;
        ns.insert(std::make_pair(xju::xml::NamePrefix("x"),
                                 xju::xml::URI("/d.org")));
        try
        {
            std::pair<xju::xml::ExpandedName, I> const x(
                xju::xml::parseTagEnd(I(c.begin(), c.end()), ns));
            xju::assert_abort();
        }
        catch(xju::Exception const& e)
        {
            xju::assert_equal(readableRepr(e), "Failed to parse end tag assumed to commence at line 1 column 1, using namespace definitions { Name Prefix \"x\" = URI \"/d.org\" } because\nfailed to parse \">\" assumed to be at line 1 column 7 because\n\">\" not at line 1 column 7.");
        }
    }
    {
        std::string const c("</y:T>");
        xju::xml::NamespaceDefinitions ns;
        ns.insert(std::make_pair(xju::xml::NamePrefix("x"),
                                 xju::xml::URI("/d.org")));
        try
        {
            std::pair<xju::xml::ExpandedName, I> const x(
                xju::xml::parseTagEnd(I(c.begin(), c.end()), ns));
            xju::assert_abort();
        }
        catch(xju::Exception const& e)
        {
            xju::assert_equal(readableRepr(e), "Failed to parse end tag assumed to commence at line 1 column 1, using namespace definitions { Name Prefix \"x\" = URI \"/d.org\" } because\nfailed to expand Unexpanded Name \"y:T\" using namespaces { Name Prefix \"x\" = URI \"/d.org\" } because\nunknown namespace y in Unexpanded Name \"y:T\".");
        }
    }
}


void test20()
{
    {
        std::string const c("fred");
        std::pair<std::string, I> const x(xju::xml::parseUntilLiteral(
            I(c.begin(), c.end()), "fred"));
        xju::assert_equal(x.first, "");
        xju::assert_equal(x.second, I(c.end(), c.end()));
    }
    {
        std::string const c("mr fred");
        std::pair<std::string, I> const x(xju::xml::parseUntilLiteral(
            I(c.begin(), c.end()), "fred"));
        xju::assert_equal(x.first, "mr ");
        xju::assert_equal(x.second, I(c.end(), c.end()));
    }
    {
        std::string const c("mr fre");
        try
        {
            std::pair<std::string, I> const x(xju::xml::parseUntilLiteral(
                I(c.begin(), c.end()), "fred"));
            xju::assert_abort();
        }
        catch(xju::Exception const& e)
        {
            xju::assert_equal(readableRepr(e), "Failed to parse from line 1 column 1 until first occurance of \"fred\" because\nfailed to parse \"fred\" assumed to be at line 1 column 7 because\nend of input at line 1 column 7.");
        }
    }
}

void test21()
{
    {
        std::string const c("'fred'");
        std::pair<std::string, I> const x(xju::xml::parseEitherQuotedLiteral(
            I(c.begin(), c.end())));
        xju::assert_equal(x.first, "fred");
        xju::assert_equal(x.second, I(c.end(), c.end()));
    }
    {
        std::string const c("\"fred\"");
        std::pair<std::string, I> const x(xju::xml::parseEitherQuotedLiteral(
            I(c.begin(), c.end())));
        xju::assert_equal(x.first, "fred");
        xju::assert_equal(x.second, I(c.end(), c.end()));
    }
    {
        std::string const c("\"fred'");
        try
        {
            std::pair<std::string, I> const x(
                xju::xml::parseEitherQuotedLiteral(
                    I(c.begin(), c.end())));
            xju::assert_abort();
        }
        catch(xju::Exception const& e)
        {
            xju::assert_equal(readableRepr(e), "Failed to parse 'x' or \"x\" assumed to commence at line 1 column 1 because\nFailed to parse from line 1 column 2 until first occurance of \"\"\" because\nfailed to parse \"\"\" assumed to be at line 1 column 7 because\nend of input at line 1 column 7. and Failed to parse \"'\" assumed to be at line 1 column 2 because\n\"'\" not at line 1 column 2..");
        }
    }
    {
        std::string const c("'fred\"");
        try
        {
            std::pair<std::string, I> const x(
                xju::xml::parseEitherQuotedLiteral(
                    I(c.begin(), c.end())));
            xju::assert_abort();
        }
        catch(xju::Exception const& e)
        {
            xju::assert_equal(readableRepr(e), "Failed to parse 'x' or \"x\" assumed to commence at line 1 column 1 because\nFailed to parse \"\"\" assumed to be at line 1 column 1 because\n\"\"\" not at line 1 column 1. and Failed to parse from line 1 column 2 until first occurance of \"'\" because\nfailed to parse \"'\" assumed to be at line 1 column 7 because\nend of input at line 1 column 7..");
        }
    }
}
void test22()
{
    {
        std::string const c("encoding='iso-8859-2'");
        std::pair<xju::Optional<std::string>, I> const x(
            xju::xml::parseOptionalEncoding(
                I(c.begin(), c.end())));
        xju::assert_equal(x.first.valid(), true);
        xju::assert_equal(x.first.value(), "iso-8859-2");
        xju::assert_equal(x.second, I(c.end(), c.end()));
    }
    {
        std::string const c("encoding = \"iso-8859-2\"");
        std::pair<xju::Optional<std::string>, I> const x(
            xju::xml::parseOptionalEncoding(
                I(c.begin(), c.end())));
        xju::assert_equal(x.first.valid(), true);
        xju::assert_equal(x.first.value(), "iso-8859-2");
        xju::assert_equal(x.second, I(c.end(), c.end()));
    }
    {
        std::string const c("fred");
        std::pair<xju::Optional<std::string>, I> const x(
            xju::xml::parseOptionalEncoding(
                I(c.begin(), c.end())));
        xju::assert_equal(x.first.valid(), false);
        xju::assert_equal(x.second, I(c.begin(), c.end()));
    }
    {
        std::string const c("encoding=fred");
        try
        {
            std::pair<xju::Optional<std::string>, I> const x(
                xju::xml::parseOptionalEncoding(
                    I(c.begin(), c.end())));
            xju::assert_abort();
        }
        catch(xju::Exception const& e)
        {
            xju::assert_equal(readableRepr(e), "Failed to parse optional \"encoding\" assumed to commence at line 1 column 1 because\nfailed to parse 'x' or \"x\" assumed to commence at line 1 column 10 because\nFailed to parse \"\"\" assumed to be at line 1 column 10 because\n\"\"\" not at line 1 column 10. and Failed to parse \"'\" assumed to be at line 1 column 10 because\n\"'\" not at line 1 column 10..");
        }
    }
    {
        std::string const c("encoding:\"fred\"");
        try
        {
            std::pair<xju::Optional<std::string>, I> const x(
                xju::xml::parseOptionalEncoding(
                    I(c.begin(), c.end())));
            xju::assert_abort();
        }
        catch(xju::Exception const& e)
        {
            xju::assert_equal(readableRepr(e), "Failed to parse optional \"encoding\" assumed to commence at line 1 column 1 because\nfailed to parse \"=\" assumed to be at line 1 column 9 because\n\"=\" not at line 1 column 9.");
        }
    }
}

void test23()
{
    {
        std::string const c("<?xml version='1.0' encoding='iso-8859-2'?>");
        std::pair<xju::xml::XmlDecl, I> const x(xju::xml::parseXmlDecl(
            I(c.begin(), c.end())));
        xju::assert_equal(
            x.first, 
            xju::xml::XmlDecl("1.0", xju::Optional<std::string>(
                "iso-8859-2")));
        xju::assert_equal(x.second, I(c.end(), c.end()));
    }
    {
        std::string const c("< ? xml version= '1.1' ? >");
        std::pair<xju::xml::XmlDecl, I> const x(xju::xml::parseXmlDecl(
            I(c.begin(), c.end())));
        xju::assert_equal(
            x.first, 
            xju::xml::XmlDecl("1.1", xju::Optional<std::string>()));
        xju::assert_equal(x.second, I(c.end(), c.end()));
    }
    {
        std::string const c("< ? XML version= '1.1' ? >");
        try
        {
            std::pair<xju::xml::XmlDecl, I> const x(xju::xml::parseXmlDecl(
                I(c.begin(), c.end())));
            xju::assert_abort();
        }
        catch(xju::Exception const& e)
        {
            xju::assert_equal(readableRepr(e), "Failed to parse XML Declaration assumed to commence at line 1 column 1 because\nfailed to parse \"xml\" assumed to be at line 1 column 5 because\n\"xml\" not at line 1 column 5.");
        }
    }
    {
        std::string const c("< ? xml version='1.1'/>");
        try
        {
            std::pair<xju::xml::XmlDecl, I> const x(xju::xml::parseXmlDecl(
                I(c.begin(), c.end())));
            xju::assert_abort();
        }
        catch(xju::Exception const& e)
        {
            xju::assert_equal(readableRepr(e), "Failed to parse XML Declaration assumed to commence at line 1 column 1 because\nfailed to parse \"?\" assumed to be at line 1 column 22 because\n\"?\" not at line 1 column 22.");
        }
    }
    {
        std::string const c("</xml version='1.1'/>");
        try
        {
            std::pair<xju::xml::XmlDecl, I> const x(xju::xml::parseXmlDecl(
                I(c.begin(), c.end())));
            xju::assert_abort();
        }
        catch(xju::Exception const& e)
        {
            xju::assert_equal(readableRepr(e), "Failed to parse XML Declaration assumed to commence at line 1 column 1 because\nfailed to parse \"?\" assumed to be at line 1 column 2 because\n\"?\" not at line 1 column 2.");
        }
    }
}

int main(int argc, char* argv[])
{
    unsigned int n(0);
    test1(); ++n;
    test2(); ++n;
    test3(); ++n;
    test4(); ++n;
    test5(); ++n;
    test6(); ++n;
    test7(); ++n;
    test8(); ++n;
    test9(); ++n;
    test10(); ++n;
    test11(); ++n;
    test12(); ++n;
    test13(); ++n;
    test14(); ++n;
    test15(); ++n;
    test16(); ++n;
    test17(); ++n;
    test18(); ++n;
    test19(); ++n;
    test20(); ++n;
    test21(); ++n;
    test22(); ++n;
    test23(); ++n;
    
    std::cout << "PASS - " << n << " steps" << std::endl;
    return 0;
}

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

#include <map>

namespace xju
{
    namespace xml
    {
        ExpandedName expandName(UnexpandedName const& name,
                                NamespaceDefinitions const& ns) /*throw(
                                    xju::Exception)*/
        {
            try
            {
                NamespaceDefinitions::const_iterator x(
                    ns.find(name.first));
                if (x == ns.end())
                {
                    std::ostringstream s;
                    s << "unknown namespace " << name.first.value()
                      << " in " << name;
                    throw xju::Exception(s, XJU_TRACED);
                }
                return ExpandedName((*x).second, name.second);
            }
            catch(xju::Exception& e)
            {
                std::ostringstream s;
                s << "expand " << name << " using namespaces " << ns;
                e.addContext(s, XJU_TRACED);
                throw;
            }
        }
        
        std::map<std::string, std::string> makePredefinedEntities() throw()
        {
            std::map<std::string, std::string> result;
            result.insert(std::make_pair("lt", "<"));
            result.insert(std::make_pair("gt", ">"));
            result.insert(std::make_pair("amp", "&"));
            result.insert(std::make_pair("apos", "'"));
            result.insert(std::make_pair("quot", "\""));
            return result;
        }
        const std::map<std::string, std::string> predefinedEntities(
            makePredefinedEntities());
        
        std::string lookupEntity(std::string name) /*throw(
            xju::Exception)*/
        {
            std::map<std::string, std::string>::const_iterator i(
                predefinedEntities.find(name));
            if (i == predefinedEntities.end())
            {
                // REVISIT: list them all
                std::ostringstream s;
                s << "entity " << name << " is not defined";
                throw xju::Exception(s, XJU_TRACED);
            }
            return (*i).second;
        }
    }
}

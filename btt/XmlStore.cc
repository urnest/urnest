//     -*- mode: c++ ; c-file-style: "osse" ; -*-
//
// Copyright (c) 2004
// Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <btt/XmlStore.hh>

#include <xju/xml.hh>
#include <string>
#include <sstream>
#include <xju/stringToInt.hh>
#include <xju/readFile.hh>
#include <unistd.h>
#include <sys/param.h>
#include <xju/AutoFd.hh>
#include <xju/stdio.hh>
#include <xju/fcntl.hh>
#include <xju/unistd.hh>
#include <fstream>

namespace btt
{
    typedef std::string::const_iterator SI;
        
    namespace
    {
        static const std::string bttns(
            "http://v1.timetracker.hfmpmobiles.australia.boeing.com");
        const xju::xml::URI bttNamespace(bttns);
        const xju::xml::ExpandedName tasksElementName(
            bttNamespace,
            xju::xml::NameLocalPart("tasks"));
        const xju::xml::ExpandedName taskElementName(
            bttNamespace,
            xju::xml::NameLocalPart("task"));
        const xju::xml::ExpandedName ID(
            bttNamespace,
            xju::xml::NameLocalPart("id"));
        const xju::xml::ExpandedName DESCRIPTION(
            bttNamespace,
            xju::xml::NameLocalPart("description"));
        const xju::xml::ExpandedName SECONDS_SPENT_ON_TASK(
            bttNamespace,
            xju::xml::NameLocalPart("secondsSpentOnThisTask"));
        const xju::xml::ExpandedName STARTED(
            bttNamespace,
            xju::xml::NameLocalPart("started"));
        
        typedef xju::parse::IteratorAdaptor<SI> I;

        template<class V>
        class PV : public std::pair<V, I>
        {
        public:
            template<class X, class Y>
            PV(X x, Y y) : std::pair<V, I>(x, y) {}
            template<class X>
            PV(X x) : std::pair<V, I>(x) {}
        };

        const xju::xml::AttributeValue& att(
            const xju::xml::Attributes a,
            const xju::xml::ExpandedName x) throw(
                xju::Exception)
        {
            const xju::xml::Attributes::const_iterator i(a.find(x));
            if (i == a.end())
            {
                throw xju::Exception(str(x) + " not found", XJU_TRACED);
            }
            return (*i).second;
        }
        
        TaskId id(const xju::xml::Attributes& a) throw(
            xju::Exception)
        {
            try
            {
                return TaskId(xju::stringToInt((att(a, ID)).value()));
            }
            catch(xju::Exception& e)
            {
                std::ostringstream s;
                s << "get id attribute";
                e.addContext(s.str(), XJU_TRACED);
                throw;
            }
        }

        std::string description(const xju::xml::Attributes& a) throw(
            xju::Exception)
        {
            try
            {
                return att(a, DESCRIPTION).value();
            }
            catch(xju::Exception& e)
            {
                std::ostringstream s;
                s << "get description attribute";
                e.addContext(s.str(), XJU_TRACED);
                throw;
            }
        }
        
        unsigned int secondsSpentOnTask(const xju::xml::Attributes& a) throw(
            xju::Exception)
        {
            try
            {
                return xju::stringToInt(att(a, SECONDS_SPENT_ON_TASK).value());
            }
            catch(xju::Exception& e)
            {
                std::ostringstream s;
                s << "get seconds-spent-on-task attribute";
                e.addContext(s.str(), XJU_TRACED);
                throw;
            }
        }
        
        boost::optional<xju::Time> started(
            const xju::xml::Attributes& a) throw(
                xju::Exception)
        {
            try
            {
                return xju::Time(xju::stringToInt(att(a, STARTED).value()), 0);
            }
            catch(xju::Exception& e)
            {
                return boost::optional<xju::Time>();
            }
        }
        
        PV<Task> parseTask(
            const I at,
            xju::xml::NamespaceDefinitions namespaces) throw(
                xju::Exception)
        {
            try
            {
                I i(at);
                const PV<xju::xml::TagOpening> task(
                    xju::xml::parseTagOpening(
                        i, namespaces, xju::xml::Attributes()));
                if (task.first.first.first != taskElementName)
                {
                    std::ostringstream s;
                    s << "expected \"task\" tag, got "
                      << str(task.first.first.first);
                    throw xju::Exception(s.str(), XJU_TRACED);
                }
                if (!task.first.second)
                {
                    std::ostringstream s;
                    s << "task tag must be simple (i.e. no separate end tag)";
                    throw xju::Exception(s.str(), XJU_TRACED);
                }
                const xju::xml::Attributes& a(task.first.first.second);
                return PV<Task>(Task(id(a),
                                     description(a),
                                     secondsSpentOnTask(a),
                                     started(a)),
                                task.second);
            }
            catch(xju::Exception& e)
            {
                std::ostringstream s;
                s << "parse Task at " << at;
                e.addContext(s, XJU_TRACED);
                throw;
            }
        }
        
        PV<Tasks> parseTasks(
            const I at,
            xju::xml::NamespaceDefinitions namespaces) throw(
                xju::Exception)
        {
            try
            {
                const PV<xju::xml::TagOpening> tasks(
                    xju::xml::parseTagOpening(at, namespaces));
                if (tasks.first.first.first != tasksElementName)
                {
                    std::ostringstream s;
                    s << "expected " << tasksElementName << " tag, got "
                      << tasks.first.first.first;
                    throw xju::Exception(s.str(), XJU_TRACED);
                }
                if (tasks.first.second)
                {
                    return PV<Tasks>(Tasks(), tasks.second);
                }
                xju::NotifyingList<Task> items;
                for(I i=tasks.second; true; true)
                {
                    try
                    {
                        PV<Task> x(parseTask(i, namespaces));
                        items.insert(items.end(), x.first);
                        i = x.second;
                    }
                    catch(xju::Exception const& e)
                    {
                        try
                        {
                            const PV<xju::xml::ExpandedName> y(
                                xju::xml::parseTagEnd(i, namespaces));
                            if (y.first != tasksElementName)
                            {
                                std::ostringstream s;
                                s << "expected task tag or end of tasks "
                                  << "tag at " << i;
                                throw xju::Exception(s, XJU_TRACED);
                            }
                            return PV<Tasks>(Tasks(items), y.second);
                        }
                        catch(xju::Exception const& f)
                        {
                            throw xju::Exception(
                                readableRepr(e) + " and " + readableRepr(f),
                                XJU_TRACED);
                        }
                    }
                }
            }
            catch(xju::Exception& e)
            {
                std::ostringstream s;
                s << "parse tasks at " << at;
                e.addContext(s, XJU_TRACED);
                throw;
            }
        }
    }
    
    std::auto_ptr<Tasks> XmlStore::loadTasks(
        const std::string& fileName,
        const std::string& tasksSchemaFileName) throw(
            xju::Exception)
    {
        try
        {
            const std::string fileContent(xju::readFile(fileName));
            const I i(fileContent.begin(), fileContent.end());
            xju::xml::NamespaceDefinitions namespaces;
            namespaces.insert(std::make_pair(
                xju::xml::NamePrefix(""),
                xju::xml::URI(""))); // REVISIT: xml namespace spec default?
            return std::auto_ptr<Tasks>(new Tasks(
                parseTasks(xju::xml::parseXmlDecl(
                    xju::xml::skipWhiteSpaceAndComments(i)).second,
                           namespaces).first));
        }
        catch(xju::Exception& e)
        {
            char buf[PATH_MAX];
            
            std::ostringstream s;
            s << "read tasks.xml file " << fileName
              << " (current directory is "
              << getcwd(buf, sizeof(buf)) << ")";
            e.addContext(s.str(), XJU_TRACED);
            throw;
        }
    }

    namespace
    {
        struct StringContains
        {
            std::string chars_;
            
            explicit StringContains(std::string const chars) throw():
                chars_(chars)
            {
            }
            bool operator()(char const c) const throw()
            {
                return std::find(chars_.begin(), chars_.end(), c) != 
                    chars_.end();
            }
        };
        
        std::string replaceCharWithCharacterRef(
            const std::string& x, 
            std::string const& chars) throw()
        {
            StringContains const replace(chars);
            
            std::string result;
            
            std::string::const_iterator i(x.begin());
            while(i != x.end())
            {
                std::string::const_iterator j(
                    std::find_if(i, x.end(), replace));
                result += std::string(i, j);
                if (j != x.end())
                {
                    std::ostringstream s;
                    s << "&#" << (int)(*j) << ";";
                    result += std::string(s.str());
                    ++j;
                }
                i = j;
            }
            return result;
        }
        std::string escapeAttributeValue(
            const std::string& x) throw()
        {
            std::string result(x);
            result = replaceCharWithCharacterRef(result, "\"&");
            return result;
        }
        
        struct WriteTask
        {
            std::ostream& o_;
            
            WriteTask(std::ostream& o) throw():
                o_(o)
            {
            }
            
            void operator()(const btt::Task& task) throw(
                xju::Exception)
            {
                try
                {
                    o_ << " <btt:task btt:id=\"" << task.id_.str()
                       << "\" btt:description=\"" 
                       << escapeAttributeValue(task.description_.value())
                       << "\" btt:secondsSpentOnThisTask=\""
                       << xju::format::int_(task.secondsSpentOnThisTask_.value())
                       << "\"";
                    
                    if (task.started_.value())
                    {
                        o_ << " btt:started=\""
                           << xju::format::int_(
                               (*task.started_.value()).getSecs())
                           << "\"";
                    }
                    o_ << "/>" << std::endl;
                }
                catch(xju::Exception& e)
                {
                    std::ostringstream s;
                    s << "failed to write task " << task.id_
                      << " to output stream";
                    e.addContext(s, XJU_TRACED);
                    throw;
                }
            }
        };
    }
    
    void XmlStore::saveTasks(const Tasks& tasks, 
                                const std::string fileName) throw(
                                    xju::Exception)
    {
	try
	{
	    const std::string tempFile(fileName+".new");
	    {
		std::fstream o(tempFile.c_str(), std::ios_base::out);
		if (!o)
		{
		    throw xju::Exception(
			std::string("failed to open ") + tempFile +
			" for writing",
			XJU_TRACED);
		}
		o << "<?xml version=\"1.0\" encoding=\"iso-8859-1\"?>"
		  << std::endl
		  << "<btt:tasks xmlns:btt=\""
		  << bttns
		  << "\">"
		  << std::endl;
		
		std::for_each(tasks.begin(),
			      tasks.end(),
			      WriteTask(o));
		
		o << "</btt:tasks>" << std::endl;
		if (!o)
		{
		    throw xju::Exception(
			"failed to write all tasks to file " +
			tempFile,
			XJU_TRACED);
		}
	    }
	    
	    xju::syscall(xju::rename, XJU_TRACED)(
		tempFile.c_str(),
		fileName.c_str());
	}
	catch(xju::Exception& e)
	{
	    std::ostringstream s;
	    s << "save " << tasks.size() << "tasks to file " << fileName;
	    e.addContext(s, XJU_TRACED);
	    throw;
	}
    }
}

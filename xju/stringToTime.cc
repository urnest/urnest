//     -*- mode: c++ ; c-file-style: "osse" ; -*-
//
// Copyright (c) 2003
// Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/stringToTime.hh>


#include <sstream>
#include <xju/format.hh>
#include <xju/assert.hh>
#include <xju/stringToInt.hh>
#include <xju/parse.hh>

namespace xju
{
    namespace
    {
        using namespace xju::parse;
        
        class NoMatch : public xju::Exception
        {
        public:
            ~NoMatch() throw()
            {
            }
            NoMatch(const std::string& pattern,
                    xju::Exception e) throw():
                xju::Exception(e),
                _pattern(pattern)
            {
            }
            const std::string _pattern;
        };
        typedef xju::Time (*Format)(const std::string&);

        // assumes leading whitespace removed
        // does not remove trailing whitespace
        // returns HH 0-23, MM 0-59 as a pair
        PV<std::pair<unsigned int, unsigned int> > parseHHMM(
            const std::string& x) throw(xju::Exception)
        {
            try
            {
                const PV<std::string> hs(parseToChar(x, ':'));
                if (hs.first.size() < 1)
                {
                    std::ostringstream s;
                    s << "need at least one hours digit got none";
                    throw xju::Exception(s, XJU_TRACED);
                }
                if (hs.first.size() > 2)
                {
                    std::ostringstream s;
                    s << "need at most two hours digits, got "
                      << hs.first.size();
                    throw xju::Exception(s, XJU_TRACED);
                }
                if (hs.second.size() < 3)
                {
                    std::ostringstream s;
                    s << "need at least two minutes digits, only got "
                      << std::string(hs.second.begin()+1, hs.second.end());
                }
                const std::string ms(hs.second.begin()+1,
                                     hs.second.begin()+3);
                const std::string rest(hs.second.begin()+3, hs.second.end());
                if (rest.size() < 2)
                {
                    std::ostringstream s;
                    s << "must have trailing am/pm/AM/PM - only have "
                      << rest;
                }
                const std::string ampm(rest.begin(), rest.begin()+2);
                if (ampm != "am" && ampm != "pm" &&
                    ampm != "am" && ampm != "pm")
                {
                    std::ostringstream s;
                    s << "am/pm indicator must be one of am, pm, AM or PM, "
                      << "not " << xju::format::quote(ampm);
                    throw xju::Exception(s, XJU_TRACED);
                }
                unsigned int hours(xju::stringToInt(hs.first));
                if (hours < 1 || hours > 12)
                {
                    std::ostringstream s;
                    s << "hours must be 1..12";
                    throw xju::Exception(s, XJU_TRACED);
                }
                const unsigned int minutes(xju::stringToInt(ms));
                if (ampm[0] == 'p' || ampm[0]=='P')
                {
                    if (hours != 12)
                    {
                        hours += 12;
                    }
                }
                else
                {
                    if (hours == 12)
                    {
                        hours = 0;
                    }
                }
                return PV<std::pair<unsigned int, unsigned int> >(
                    std::make_pair(hours, minutes),
                    std::string(rest.begin()+2, rest.end()));
            }
            catch(xju::Exception& e)
            {
                std::ostringstream s;
                s << "parse " << xju::format::quote(x) 
                  << " assuming it looks like e.g. 5:30pm";
                e.addContext(s, XJU_TRACED);
                throw;
            }
        }
        xju::Time format1(const std::string& x) throw(
            NoMatch)
        {
            const std::string pattern("YYYYMMDD HH:MMam");
            try
            {
                const PV<std::string> date(parseToChar(skipWhite(x), ' '));
                if (date.first.size() != 8)
                {
                    std::ostringstream s;
                    s << "date part must have 8 digits, e.g. 20050923";
                    throw xju::Exception(s, XJU_TRACED);
                }
                const unsigned int year(xju::stringToInt(
                    std::string(date.first.begin(), date.first.begin()+4)));
                const unsigned int month(xju::stringToInt(
                    std::string(date.first.begin()+4, date.first.begin()+6)));
                const unsigned int day(xju::stringToInt(
                    std::string(date.first.begin()+6, date.first.begin()+8)));
                const PV<std::pair<unsigned int, unsigned int> > hhmm(
                    parseHHMM(skipWhite(date.second)));
                struct tm x;
                x.tm_year = year - 1900;
                x.tm_mon = month - 1;
                x.tm_mday = day;
	    
                x.tm_hour = hhmm.first.first;
                x.tm_min = hhmm.first.second;
                x.tm_sec = 0;
                x.tm_yday = 0;
                x.tm_isdst = 0;
            
                const time_t y(mktime(&x));
                return xju::Time(y, 0);
            }
            catch(xju::Exception& e)
            {
                std::ostringstream s;
                s << "parse " << xju::format::quote(x) 
                  << " assuming it looks like "
                  << xju::format::quote(pattern);
                e.addContext(s, XJU_TRACED);
                throw NoMatch(pattern, e);
            }
        }
        // parses e.g. "4:52pm"
        xju::Time format2(const std::string& s) throw(
            NoMatch)
        {
            const std::string pattern("HH:MMam");
            try
            {
                struct tm x;
                time_t now(time(0));
#ifdef __MINGW32__
                x = *localtime(&now);
#else
                localtime_r(&now, &x);
#endif
                const PV<std::pair<unsigned int, unsigned int> > hhmm(
                    parseHHMM(skipWhite(s)));

                x.tm_hour = hhmm.first.first;
                x.tm_min = hhmm.first.second;

                x.tm_sec = 0;
                x.tm_yday = 0;
                x.tm_isdst = 0;
                
                const time_t y(mktime(&x));
                return xju::Time(y, 0);
            }
            catch(xju::Exception& e)
            {
                std::ostringstream ss;
                ss << "parse " << xju::format::quote(s) 
                   << " assuming it looks like "
                   << xju::format::quote(pattern);
                e.addContext(ss, XJU_TRACED);
                throw NoMatch(pattern, e);
            }
        }
    }
    
    xju::Time stringToTime(const std::string& s) throw(
	xju::Exception)
    {
	try
	{
            Format formats[] = 
            {
                format1,
                format2
            };
            Format* formatsEnd(
                formats+sizeof(formats)/sizeof(Format));
            std::vector<std::string> failedPatterns;
            for(Format* f = formats; f != formatsEnd; ++f)
            {
                try
                {
                    return (*f)(s);
                }
                catch(const NoMatch& e)
                {
                    failedPatterns.push_back(e._pattern);
                }
            }
            std::ostringstream cause;
            cause << xju::format::quote(s) << " does not look like "
                  << xju::format::join(failedPatterns.begin(),
                                       failedPatterns.end(),
                                       " or ");
            throw xju::Exception(cause, XJU_TRACED);
	}
	catch(xju::Exception& e)
	{
	    std::ostringstream ss;
	    ss << "parse " << xju::format::quote(s) << " as a time";
	    e.addContext(ss, XJU_TRACED);
	    throw;
	}
    }
}

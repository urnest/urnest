//     -*- mode: c++ ; c-file-style: "osse" ; -*-
//
// Copyright (c) 1997-2007 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
//    This class acts as a "client" of twt_Event object.  It's trigger
//    method is called whenever the twt_Event occurs.
//
#ifndef _XJU_EVENTCLIENT_HH_
#define _XJU_EVENTCLIENT_HH_


#include <xju/Event.hh>
#include <xju/SafeTriggerIf.hh>
#include <xju/Ring.hh>


namespace xju
{
    class EventClient : public SafeTriggerIf
    {
    public:
	//
	// Postconditions:
	//
	//    this is detached from the event it was attached to
	//    (if any), and attached to x.
	//
	const EventClient& operator=(const Event& x) const throw();
	
	
	//
	// Preconditions:
	//
	//    event is the event to watch for
	//
	explicit EventClient(const Event& event) throw();
	
	//
	// This function is called whenever the event occurs
	//
	virtual void trigger() throw() = 0;
	
	
    private:
	mutable Ring<SafeTriggerIf*> m_link;
	Ring<SafeTriggerIf*>& link() const
	{
	    return m_link;
	}
	
	
	//
	// not implemented
	//
	// (Reasoning: there two reasonable but conflicting
	// possible copy behaviours, so it is better to
	// force the user to explicitly implement one than
	// to risk them assuming the wrong behaviour.)
	//
	EventClient(const EventClient& b) throw();
	const EventClient& operator=(const EventClient&) throw();
    };
    
    
    inline EventClient::EventClient(const Event& event) throw():
	m_link(this)
    {
	event.observers().splice(m_link);
    }
    
    
    inline const EventClient& EventClient::operator=(
	const Event& x) const throw()
    {
	link().cut();
	x.observers().splice(link());
	return *this;
    }
    
}

#endif
    

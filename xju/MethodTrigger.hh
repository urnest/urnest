//     -*- mode: c++ ; c-file-style: "osse" ; -*-
//
// From code Copyright (c) 1998 Trevor Taylor
// Copyright (c) 2003 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
//    Class that implements a trigger interface, calling
//    a specified member function on a specified object when
//    it is triggered.
//
#ifndef _XJU_METHODTRIGGER_HH_
#define _XJU_METHODTRIGGER_HH_


namespace xju
{
    
    //
    // Preconditions:
    //
    //    TriggerIf has method void trigger();
    //    
    template<class T, class TriggerIf>
    class MethodTrigger : public TriggerIf
    {
	
    public:
	MethodTrigger(T& client, void (T::*f)()):
	    m_client(client),
	    m_f(f)
	{
	}
	
	
	void trigger() throw()
	{
	    (m_client.*m_f)();
	}
	
	
    private:
	T& m_client;
	void (T::*m_f)();
    };
    
}

#endif
    

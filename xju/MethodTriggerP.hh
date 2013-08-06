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
//    A parameter of arbitrary type is carried in the trigger.
//
#ifndef _XJU_METHODTRIGGERP_HH_
#define _XJU_METHODTRIGGERP_HH_


namespace xju
{
    
    //
    // Preconditions:
    //
    //    TriggerIf has method void trigger(P);
    //    
    template<class T, class TriggerIf, class P>
    class MethodTriggerP : public TriggerIf
    {
	
    public:
	MethodTriggerP(T& client, void (T::*f)(P)):
	    m_client(client),
	    m_f(f)
	{
	}
	
	
	void trigger(P p) throw()
	{
	    (m_client.*m_f)(p);
	}
	
	
    private:
	T& m_client;
	void (T::*m_f)(P);
    };
}

    
#endif
    

//     -*- mode: c++ ; c-file-style: "osse" ; -*-
//
// From code Copyright (c) 1997 Trevor Taylor with permission
// Copyright (c) 2003 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
//    Template for a "safe" trigger interface. This interface allows some
//    capability of the implementor to be invoked, with a type-safe
//    parameter.  The invoked
//    capability cannot throw any exceptions.
//
//    (See also TriggerIf.hh)
//
#ifndef _XJU_SAFETRIGGERPIF_HH_
#define _XJU_SAFETRIGGERPIF_HH_


namespace xju
{
    template<class T>
    class SafeTriggerPIf
    {
    public:
	virtual void trigger(T) throw() = 0;
	
	virtual ~SafeTriggerPIf() {}
        
    };
    template<class T1, class T2>
    class SafeTriggerP2If
    {
    public:
	virtual void trigger(T1, T2) throw() = 0;
	
	virtual ~SafeTriggerP2If() {}
        
    };
    template<class T1, class T2, class T3>
    class SafeTriggerP3If
    {
    public:
	virtual void trigger(T1, T2, T3) throw() = 0;
	
	virtual ~SafeTriggerP3If() {}
        
    };
    template<class T1, class T2, class T3, class T4>
    class SafeTriggerP4If
    {
    public:
	virtual void trigger(T1, T2, T3, T4) throw() = 0;
	
	virtual ~SafeTriggerP4If() {}
        
    };
    template<class T1, class T2, class T3, class T4, class T5>
    class SafeTriggerP5If
    {
    public:
	virtual void trigger(T1, T2, T3, T4, T5) throw() = 0;
	
	virtual ~SafeTriggerP5If() {}
        
    };
}


#endif

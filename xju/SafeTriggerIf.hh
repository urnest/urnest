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
//    "Safe" trigger interface.  A basic interface that allows some
//    capability of the implementor to be invoked.  The invoked
//    capability cannot throw any exceptions.
//
//    (See also Trigger.hh)
//
#ifndef _XJU_SAFETRIGGERIF_H_
#define _XJU_SAFETRIGGERIF_H_

namespace xju
{
    class SafeTriggerIf
    {
    public:
	virtual void trigger() throw() = 0;
	
	virtual ~SafeTriggerIf() {}
        
    };
}


#endif

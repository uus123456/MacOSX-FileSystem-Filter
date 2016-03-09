/*
 * Copyright (c) 2015 Slava Imameev. All rights reserved.
 */

#ifndef __VFSFilter0__VFSFilter0__
#define __VFSFilter0__VFSFilter0__

#include <IOKit/IOService.h>
#include <IOKit/IOUserClient.h>

#include "Common.h"

//--------------------------------------------------------------------

//
// the I/O Kit driver class
//
class com_FsdFilter : public IOService
{
    OSDeclareDefaultStructors(com_FsdFilter)
    
public:
    virtual bool start(IOService *provider);
    virtual void stop( IOService * provider );
    
    static com_FsdFilter*  getInstance(){ return com_FsdFilter::Instance; };
    
protected:
    
    virtual bool init();
    virtual void free();
    
private:
    
    static com_FsdFilter* Instance;
    
};

//--------------------------------------------------------------------

#endif//__VFSFilter0__VFSFilter0__


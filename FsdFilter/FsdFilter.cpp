/*
 * Copyright (c) 2015 Slava Imameev. All rights reserved.
 */

#include <IOKit/IOLib.h>
#include <IOKit/IODeviceTreeSupport.h>
#include <libkern/c++/OSContainers.h>
#include <IOKit/assert.h>
#include <IOKit/IOCatalogue.h>
#include "Common.h"
#include "FsdFilter.h"
#include "VFSHooks.h"
#include "Kauth.h"
#include "VNodeHook.h"

#ifdef USE_FAKE_FSD
    #include "FltFakeFSD.h"
#else
    #include "VersionDependent.h"
#endif

//--------------------------------------------------------------------

com_FsdFilter* com_FsdFilter::Instance;

//--------------------------------------------------------------------

//
// the standard IOKit declarations
//
#undef super
#define super IOService

OSDefineMetaClassAndStructors(com_FsdFilter, IOService)

//--------------------------------------------------------------------

bool
com_FsdFilter::start(
    __in IOService *provider
    )
{
    if( kIOReturnSuccess != FltGetVnodeLayout() ){
        
        DBG_PRINT_ERROR( ( "FltGetVnodeLayout() failed\n" ) );
        goto __exit_on_error;
    }
    
    if( kIOReturnSuccess != VFSHookInit() ){
        
        DBG_PRINT_ERROR( ( "VFSHookInit() failed\n" ) );
        goto __exit_on_error;
    }
    
    if( ! FltVnodeHooksHashTable::CreateStaticTableWithSize( 8, true ) ){
        
        DBG_PRINT_ERROR( ( "FltVnodeHooksHashTable::CreateStaticTableWithSize() failed\n" ) );
        goto __exit_on_error;
    }
    
    //
    // create an object for the vnodes KAuth callback and register the callback,
    // the callback might be called immediatelly just after registration!
    //
    gVnodeGate = FltIOKitKAuthVnodeGate::withCallbackRegistration( this );
    assert( NULL != gVnodeGate );
    if( NULL == gVnodeGate ){
        
        DBG_PRINT_ERROR( ( "FltIOKitKAuthVnodeGate::withDefaultSettings() failed\n" ) );
        goto __exit_on_error;
    }
    
    Instance = this;
    
    //
    // register with IOKit to allow the class matching
    //
    registerService();
    
    //
    // make the driver unloadable
    //
    this->retain();

    return true;
    
__exit_on_error:
    
    //
    // all cleanup will be done in stop() and free()
    //
    this->release();
    return false;
}

//--------------------------------------------------------------------

void
com_FsdFilter::stop(
    __in IOService * provider
    )
{
    super::stop( provider );
}

//--------------------------------------------------------------------

bool com_FsdFilter::init()
{
    if(! super::init() )
        return false;
    
    return true;
}

//--------------------------------------------------------------------

//
// actually this will not be called as the module should be unloadable in release build
//
void com_FsdFilter::free()
{
    if( gVnodeGate ){
        
        gVnodeGate->release();
        gVnodeGate = NULL;
    }
    
    FltVnodeHooksHashTable::DeleteStaticTable();
    
    VFSHookRelease();
    
    super::free();
}

//--------------------------------------------------------------------



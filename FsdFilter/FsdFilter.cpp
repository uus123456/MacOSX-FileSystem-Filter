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
    
    if( kIOReturnSuccess != VFSHookInit() ){
        
        DBG_PRINT_ERROR( ( "VFSHookInit() failed\n" ) );
        goto __exit_on_error;
    }
    
    if( ! FltVnodeHooksHashTable::CreateStaticTableWithSize( 8, true ) ){
        
        DBG_PRINT_ERROR( ( "FltVnodeHooksHashTable::CreateStaticTableWithSize() failed\n" ) );
        goto __exit_on_error;
    }

    
    //
    // gSuperUserContext must have a valid thread and process pointer
    // TO DO redesign this! Indefinit holding a thread or task object is a bad behaviour.
    //
    thread_reference( current_thread() );
    task_reference( current_task() );
    
    gSuperUserContext = vfs_context_create(NULL); // vfs_context_kernel()
    
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
    
    if( kIOReturnSuccess != FltGetVnodeLayout() ){
        
        super::free();
        return false;
    }
    
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



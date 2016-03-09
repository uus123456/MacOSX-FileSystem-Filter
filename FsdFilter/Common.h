//
//  Common.h
//
//  Copyright (c) 2015 Slava Imameev. All rights reserved.
//

#ifndef VFSFilter0_Common_h
#define VFSFilter0_Common_h

#include <IOKit/IOLib.h>
#include <libkern/OSAtomic.h>

#ifdef __cplusplus
extern "C" {
#endif
    
#include <stdint.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/kauth.h>
#include <sys/vnode.h>
#include <mach/vm_types.h>
#include <kern/sched_prim.h>
#include <sys/lock.h>
#include <sys/proc.h>

#ifdef __cplusplus
}
#endif

//--------------------------------------------------------------------

#define __in
#define __out
#define __inout
#define __in_opt
#define __out_opt
#define __opt

//--------------------------------------------------------------------

//
// ASL - Apple System Logger,
// the macro requires a boolean variable isError being defined in the outer scope,
// the macro uses only ASL, in case of stampede the ASL silently drops data on the floor
//
//
// TO DO - IOSleep called after IOLog allows the system to replenish the log buffer
// by retrieving the existing entries usin syslogd
//
#define DLD_COMM_LOG_EXT_TO_ASL( _S_ ) do{\
    IOLog(" [%-7d] FltKrnLog:" ); \
    IOLog("%s %s(%u):%s: ", isError?"ERROR!!":"", __FILE__ , __LINE__, __PRETTY_FUNCTION__ );\
    IOLog _S_ ; \
}while(0);

//
// a common log
//
#if !defined(_DLD_LOG)

    #define DBG_PRINT( _S_ )   do{ void(0); }while(0);// { kprintf _S_ ; }

#else

    #define DBG_PRINT( _S_ )  do{ bool  isError = false; DLD_COMM_LOG_EXT_TO_ASL( _S_ ); }while(0);

#endif


//
// an errors log
//
#if !defined(_DLD_LOG_ERRORS)

    #define DBG_PRINT_ERROR( _S_ )   do{ void(0); }while(0);//DBG_PRINT( _S_ )

#else

    #define DBG_PRINT_ERROR( _S_ )   do{ bool  isError = true; DLD_COMM_LOG_EXT_TO_ASL( _S_ ); }while(0);

#endif


#if defined(DBG)
    #define DLD_INVALID_POINTER_VALUE ((long)(-1))
    #define DLD_DBG_MAKE_POINTER_INVALID( _ptr ) do{ (*(long*)&_ptr) = DLD_INVALID_POINTER_VALUE; }while(0);
    #define DLD_IS_POINTER_VALID( _ptr ) ( NULL != _ptr && DLD_INVALID_POINTER_VALUE != (long)_ptr )
#else//DBG
    #define DLD_DBG_MAKE_POINTER_INVALID( _ptr )  do{void(0);}while(0);
    #define DLD_IS_POINTER_VALID( _ptr ) ( NULL != _ptr )
#endif//DBG

//---------------------------------------------------------------------

//
// Calculate the address of the base of the structure given its type, and an
// address of a field within the structure.
//

#define CONTAINING_RECORD(address, type, field) ((type *)( \
    (char*)(address) - \
    reinterpret_cast<vm_address_t>(&((type *)0)->field)))


#define __countof( X )  ( sizeof( X ) / sizeof( X[0] ) )


//--------------------------------------------------------------------

//
// a type for the vnode operations
//
typedef int (*VOPFUNC)(void *) ;
typedef int (*VFSFUNC)(void *) ;

//--------------------------------------------------------------------


#endif // VFSFilter0_Common_h

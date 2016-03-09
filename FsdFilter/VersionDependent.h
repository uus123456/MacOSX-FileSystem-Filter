//
//  VersionDependent.h
//
//  Copyright (c) 2015 Slava Imameev.. All rights reserved.
//

#ifndef USE_FAKE_FSD

#ifndef __VFSFilter0__VersionDependent__
#define __VFSFilter0__VersionDependent__

#include "Common.h"

//--------------------------------------------------------------------

//
// the structure defines an offset from the start of a v_op vector for a function
// implementing a corresponding vnode operation
//
typedef struct _FltVnodeOpvOffsetDesc {
	struct vnodeop_desc *opve_op;   /* which operation this is, NULL for the terminating entry */
	vm_offset_t offset;		/* offset in bytes from the start of v_op, (-1) means "unknown" */
} FltVnodeOpvOffsetDesc;

//--------------------------------------------------------------------

errno_t
FltVnodeGetSize(vnode_t vp, off_t *sizep, vfs_context_t ctx);

errno_t
FltVnodeSetsize(vnode_t vp, off_t size, int ioflag, vfs_context_t ctx);

VOPFUNC*
FltGetVnodeOpVector(
                    __in vnode_t vn
                    );

FltVnodeOpvOffsetDesc*
FltRetriveVnodeOpvOffsetDescByVnodeOpDesc(
                                          __in struct vnodeop_desc *opve_op
                                          );

VOPFUNC
FltGetVnop(
           __in vnode_t   vn,
           __in struct vnodeop_desc *opve_op
           );

const char*
GetVnodeNamePtr(
    __in vnode_t vn
    );

int
FltGetVnodeVfsFlags(
                    __in vnode_t vnode
                    );

IOReturn
FltGetVnodeLayout();

//--------------------------------------------------------------------

#endif /* defined(__VFSFilter0__VersionDependent__) */
#endif // USE_FAKE_FSD

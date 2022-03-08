/******************************************************************************
 * Name         : sglmem.h
 * Title        : SGL Memory Management
 * Author       : Michael Green
 * Created      : 13th May 1997
 *
 * Copyright	: 1995-2022 Imagination Technologies (c)
 * License		: MIT
 *
 * Description  : Single threaded memory management for SGL and PVRD
 * 
 * Platform     : ANSI
 *
 * Modifications:
 * $Log: sglmem.h,v $
 * Revision 1.3  1997/06/02  02:48:44  jop
 * Converted to use glbal heap stuff in heap.c/h
 *
 * Revision 1.2  1997/05/13  19:09:39  mjg
 * Removed redundant pragma.
 *
 * Revision 1.1  1997/05/13  17:56:50  mjg
 * Initial revision
 *
 *
 *****************************************************************************/


#ifndef __SGLMEM_H__

#define __SGLMEM_H__

#include "heap.h"

#define	SGLMalloc(X)	MemAlloc (X)
#define	SGLCalloc(X,Y)	MemAlloc (X*Y)
#define	SGLRealloc(X,Y)	MemReAlloc (X,Y)
#define	SGLFree(X)		MemFree (X)

#define NEW(struct_type) ((struct_type *)(SGLMalloc(sizeof(struct_type))))


#endif /* __SGLMEM_H__ */
													
/* --------=======+++++#### END of FILE ####+++++=======-------- */


/******************************************************************************
 * Name         : tmalloc.h
 * Title        : Controls the allocation of texture memory.
 * Author       : Peter Leaback
 * Created      : 23/06/1995
 *
 * Copyright	: 1995-2022 Imagination Technologies (c)
 * License		: MIT
 *
 * Description  : These routines don't actually touch the texture memory.
 * 
 * Platform     : ANSI
 *
 * Modifications:
 * $Log: tmalloc.h,v $
 * Revision 1.9  1997/06/23  22:00:27  jop
 * Added vlock counter and low water mark to mnode
 *
 * Revision 1.8  1997/06/22  17:38:52  jop
 * Added adaptive TSP parameter stuff and backwards allocation
 *
 * Revision 1.7  1997/06/04  15:33:45  gdc
 * added prototypes for TMAlloc etc as they have no prototypes
 * elsewhere
 *
 * Revision 1.6  1997/04/30  19:29:27  mjg
 * Removed Texheap and prototypes for TMalloc, TFree and
 * InitTextureMemory as they are now in pvrosapi.h
 *
 * Revision 1.5  1997/04/29  15:04:45  erf
 * 1. Changed prototype definitions of TMalloc() and TFree().
 * 2. Changed UINT's to sgl_uint's etc...
 *
 * Revision 1.4  1997/04/28  22:11:45  erf
 * Added #ifndef __TMALLOC_H__ etc etc...
 *
 * Revision 1.3  1997/04/25  16:06:01  mjg
 * Added TEXTUREHEAP structure and altered the prototypes for
 * TFree and TMalloc.
 *
 * Revision 1.2  1997/03/13  11:46:46  sxy
 * Changed MemoryRoot to a parameter rather than static.
 *
 * Revision 1.1  1995/07/17  16:04:05  pdl
 * Initial revision
 *
 *
 *****************************************************************************/

#ifndef __TMALLOC_H__
#define __TMALLOC_H__

#define TMALLOC_BACKWARDS	1

#define A_BANK	1  /*these numbers are assumed in the code*/
#define B_BANK	2

#define PROBABLE_CYCLE	10000 /*to test for cyclic lists*/

typedef enum
{
	NORMAL_BLOCK,
	TSP_PARAM_BLOCK,
	ISP_OUT_OF_CACHE_BLOCK

} BLOCK_TYPE;

typedef struct _MNODE {
	struct _MNODE *Next;  /*required for the coalescing */
	struct _MNODE *Prev;  /*increasing address */
	sgl_uint32	MemoryAddress;
	sgl_uint32	BlockSize;		/* Does not have to be texture size	*/
	sgl_uint8	UsedStatus;			/* A_BANK | B_BANK */
	sgl_uint32  MemoryLeft;    /*memory left  */

	struct _MNODE *NextFree;	/* this list does not contain free budies*/
	struct _MNODE *PrevFree;	/* random order */

	struct _MNODE *NextBuddy; /* block size of these has to be texture size*/
	struct _MNODE *PrevBuddy; 

	#if TMALLOC_BACKWARDS
	sgl_uint32	MemoryOffset;
	sgl_uint32	MemorySize;
	#endif

	BLOCK_TYPE	BlockType;

	sgl_int32	AllocatedBlocks;

	sgl_int32	LowWaterMark;

} MNODE;

typedef struct {
	MNODE *MNode;
	int Status;	/* A_BANK ^ B_BANK ^ -1 */
} MNODE_BLOCK;

#include "texapi.h"

void InitTextureMemory(sgl_uint32 TextureMemorySize, HTEXHEAP hTexHeap);
void TMalloc(sgl_uint32 RequestSize, HTEXHEAP hTexHeap, MNODE_BLOCK *FreeNode);
void TFree(MNODE_BLOCK *FreeNode, HTEXHEAP hTexHeap);
sgl_int32 TSetHighWaterMark (HTEXHEAP hTexHeap, sgl_int32 nRequested);


#endif	/* __TMALLOC_H__	*/

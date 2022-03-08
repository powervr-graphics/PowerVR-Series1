/******************************************************************************
 * Name         : tmalloc.c
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
 * $Log: tmalloc.c,v $
 * Revision 1.24  1997/11/06  19:26:17  mjg
 * Removed some compiler warnings.
 *
 * Revision 1.23  1997/07/24  19:16:39  mjg
 * Removed the hard-coded LowWaterMark - now uses the value in the
 * TexHeap (filled in by the ini settings).
 *
 * Revision 1.22  1997/07/18  14:24:07  gdc
 * chnaged printf's to DPF's
 *
 * Revision 1.21  1997/07/14  13:38:47  gdc
 * removed ^M's
 *
 * Revision 1.20  1997/06/24  18:28:47  gdc
 * extended debug message a bit
 *
 * Revision 1.19  1997/06/23  22:00:12  jop
 * Added GetHighWaterMark fn
 *
 * Revision 1.18  1997/06/22  17:38:30  jop
 * Added adaptive TSP parameter stuff, and backwards allocation
 *
 * Revision 1.17  1997/05/02  07:42:44  erf
 * Removed UINT32's etc and replaced with sgl_uint32's etc.
 *
 * Revision 1.16  1997/05/01  14:47:05  mjg
 * Fixed a dodgy cast.
 *
 * Revision 1.15  1997/04/30  19:27:53  mjg
 * Rolled in new Texture API.
 *
 * Revision 1.14  1997/04/29  15:03:57  erf
 * 1. Changed TMalloc() and TFree().
 * 2. Removed compiler warnings on type checking.
 *
 * Revision 1.13  1997/04/28  20:56:33  erf
 * Removed extern from InitTextureMemory() function header.
 *
 * Revision 1.12  1997/04/27  22:42:26  jop
 * Added USE_HEAPMAN define in order to get all texturemap nodes
 *
 * Revision 1.11  1997/04/25  16:35:14  mjg
 * Updated TFree and TMalloc.
 *
 * Revision 1.10  1997/04/21  18:31:15  gdc
 * removed unused Talloccheck function as is called an sgl_ api function
 *
 * Revision 1.9  1997/03/27  12:36:08  erf
 * Operating system independent memory allocation/deallocation macros.
 * 1. Added #include "pvrosapi.h" to include new macros.
 * 2. Changed malloc, calloc, realloc and free to PVROSMalloc, PVROSCalloc,
 *    PVROSRealloc and PVROSFree.
 *										  
 * Revision 1.8  1997/03/15  13:25:16  jop
 * removed windows.h include (!)
 *
 * Revision 1.7  1997/03/13  11:45:42  sxy
 * Changed MemoryRoot to a parameter rather than static.
 *
 * Revision 1.6  1996/10/01  16:47:40  jrc
 * Added a comment about checking u32StructSize if we add new members to
 * the sgl_texture_mem_info struct.
 *
 * Revision 1.5  1996/09/27  18:42:00  jrc
 * Gave the FreeMem members of the sgl_texture_mem_info struct clearer names.
 *
 * Revision 1.4  1996/09/26  18:04:12  jrc
 * Now doing best-fit texture memory allocation (instead of first-fit).
 *
 * Revision 1.3  1996/09/25  15:02:00  jrc
 * Changed sgl_get_largest_free_texture_mem to sgl_get_free_texture_mem_info.
 *
 * Revision 1.2  1996/09/24  09:33:21  jrc
 * Added sgl_get_largest_free_texture_mem.
 *
 * Revision 1.1  1995/07/17  16:03:41  pdl
 * Initial revision
 *
 *
 *****************************************************************************/

#define MODULE_ID MODID_TMALLOC

#include <stdio.h>
#include "sgl_defs.h"
#include "sgl_init.h"

#define USE_HEAPMAN

#include "pvrosapi.h"
#include "texapi.h"
#include "tmalloc.h"

#define BEST_FIT  TRUE /* Set FALSE for previous 'first fit' operation. */

/******************************************************************************
 * Function Name: InitTextureMemory
 *
 * Inputs       : TextureMemorySize,the number of bytes in the texture memory.
 * Outputs      : -
 * Returns      : -
 * Globals Used : MemoryRoot
 *				  MemoryLeft
 * Description  : This should only be called once. If it is called more than
 *				  once it will unlink any list at the root, so main memory
 *				  will be lost. 
 *****************************************************************************/
void InitTextureMemory(sgl_uint32 TextureMemorySize, HTEXHEAP hTexHeap)
{	
	((MNODE*)hTexHeap->pMemoryRoot)->Prev			= NULL;
	((MNODE*)hTexHeap->pMemoryRoot)->Next			= NULL;
	((MNODE*)hTexHeap->pMemoryRoot)->MemoryAddress	= 0;
	((MNODE*)hTexHeap->pMemoryRoot)->BlockSize		= TextureMemorySize >> 1; 
											/*the size of a bank in bytes*/
	((MNODE*)hTexHeap->pMemoryRoot)->UsedStatus		= 0;

	((MNODE*)hTexHeap->pMemoryRoot)->NextFree		= NULL;
	((MNODE*)hTexHeap->pMemoryRoot)->PrevFree		= NULL;

	((MNODE*)hTexHeap->pMemoryRoot)->NextBuddy		= NULL;
	((MNODE*)hTexHeap->pMemoryRoot)->PrevBuddy		= NULL;

	((MNODE*)hTexHeap->pMemoryRoot)->MemoryLeft		= TextureMemorySize;

#if TMALLOC_BACKWARDS
	((MNODE*)hTexHeap->pMemoryRoot)->MemoryOffset	= 0;
	/* 1 bank's worth - kept as a reference */
	((MNODE*)hTexHeap->pMemoryRoot)->MemorySize		= TextureMemorySize >> 1;
#endif

	((MNODE*)hTexHeap->pMemoryRoot)->BlockType		= NORMAL_BLOCK;

	((MNODE*)hTexHeap->pMemoryRoot)->AllocatedBlocks	= 0;
	((MNODE*)hTexHeap->pMemoryRoot)->LowWaterMark		= hTexHeap->TexParamSize;
}

/******************************************************************************
 * Function Name: TMalloc
 *
 * Inputs       : RequestSize,the number of bytes required to be allocated.
 * Outputs      : -
 * Returns      : -
 * Globals Used : MemoryRoot
 *				  MemoryLeft
 * Description  : This routine allocates memory from the texture memory.
 *				  The design document gives a brief explaination of the algorthm 
 *		   		  implemented.
 * Pre-condition: InitTextureMemory has been called.
 *****************************************************************************/
void TMalloc(sgl_uint32 RequestSize, HTEXHEAP hTexHeap, MNODE_BLOCK *FreeNode)
{
	MNODE *MemWalk;
	MNODE *NewNode;

	/*
	** search the buddy list for a free buddy of the exact size 
	*/

	MemWalk = hTexHeap->pMemoryRoot;

	while(MemWalk->NextBuddy != NULL && 
	(MemWalk->BlockSize != RequestSize || MemWalk->UsedStatus==0 || MemWalk->UsedStatus==3))
		MemWalk=MemWalk->NextBuddy;

	if(MemWalk->UsedStatus==A_BANK || MemWalk->UsedStatus==B_BANK)
	{	
		if(MemWalk->BlockSize == RequestSize)
		{

			/* remove this block from the buddy list */

			if(MemWalk != (MNODE*) hTexHeap->pMemoryRoot)
 			{

				MemWalk->PrevBuddy->NextBuddy=MemWalk->NextBuddy;
				if(MemWalk->NextBuddy != NULL)
					MemWalk->NextBuddy->PrevBuddy=MemWalk->PrevBuddy;

			}
			/* return the free buddy */
	  
		#if TMALLOC_BACKWARDS
			MemWalk->MemoryAddress=((MNODE*)hTexHeap->pMemoryRoot)->MemorySize-MemWalk->MemoryOffset-RequestSize;
		#endif

			FreeNode->Status=MemWalk->UsedStatus ^ 3;
			FreeNode->MNode=MemWalk;

			/* mark this block as full */

			MemWalk->UsedStatus=A_BANK | B_BANK;
			
			((MNODE*)hTexHeap->pMemoryRoot)->MemoryLeft-=RequestSize;
			((MNODE*)hTexHeap->pMemoryRoot)->AllocatedBlocks ++;

			return ;

		}
	}

	/*
	**
	** There are NO correct Buddies,so do a first or best fit in the free list.
	**
	*/

	MemWalk=hTexHeap->pMemoryRoot;

#if BEST_FIT
	{
		sgl_uint32  u32BestSize = 0xffffffff;
		MNODE  *pBestNode = MemWalk;

		ASSERT(MemWalk != NULL);
		do
		{
			if (MemWalk->UsedStatus == 0 && MemWalk->BlockSize >= RequestSize
			  && MemWalk->BlockSize < u32BestSize)
			{
				u32BestSize = MemWalk->BlockSize;
				pBestNode = MemWalk;
			}
			MemWalk=MemWalk->NextFree;

		} while (MemWalk != NULL);

		ASSERT(pBestNode != NULL);
		MemWalk = pBestNode;
	}
#else
	while (MemWalk->NextFree != NULL &&
	  (MemWalk->BlockSize < RequestSize || MemWalk->UsedStatus!=0))
	{
		MemWalk=MemWalk->NextFree;
	}
#endif

	if(MemWalk->UsedStatus==0 && MemWalk->BlockSize >= RequestSize) 
	{
		/* allocate from this space */

		/* unlink the node from the free list */

		if(MemWalk != (MNODE*)hTexHeap->pMemoryRoot)
 		{
			MemWalk->PrevFree->NextFree=MemWalk->NextFree;
			if(MemWalk->NextFree != NULL)
				MemWalk->NextFree->PrevFree=MemWalk->PrevFree;
		}

		/* put the node on the buddy list*/

		if(MemWalk != (MNODE*)hTexHeap->pMemoryRoot)
		{
			MemWalk->PrevBuddy = hTexHeap->pMemoryRoot;
			MemWalk->NextBuddy = ((MNODE*)hTexHeap->pMemoryRoot)->NextBuddy;

			if(((MNODE*)hTexHeap->pMemoryRoot)->NextBuddy !=NULL)
				((MNODE*)hTexHeap->pMemoryRoot)->NextBuddy->PrevBuddy=MemWalk;
			((MNODE*)hTexHeap->pMemoryRoot)->NextBuddy=MemWalk;
		}

		if(MemWalk->BlockSize > RequestSize)
		{
			/*
			**create a new free block for the left overs
			*/

			NewNode=(MNODE*)PVROSMalloc(sizeof(MNODE));

			/* link new node into the master list */

			NewNode->Prev=MemWalk;
			NewNode->Next=MemWalk->Next;

			if(MemWalk->Next !=NULL)
				MemWalk->Next->Prev=NewNode;
			MemWalk->Next=NewNode;

			/* fill out the other fields */

		#if TMALLOC_BACKWARDS
			NewNode->MemoryOffset=MemWalk->MemoryOffset + RequestSize;
		#else
			NewNode->MemoryAddress=MemWalk->MemoryAddress + RequestSize;
		#endif
			NewNode->BlockSize=MemWalk->BlockSize - RequestSize;
			NewNode->UsedStatus=0;

			/*link free node after root node*/

			NewNode->PrevFree=hTexHeap->pMemoryRoot;
			NewNode->NextFree=((MNODE*)hTexHeap->pMemoryRoot)->NextFree;

			if(((MNODE*)hTexHeap->pMemoryRoot)->NextFree !=NULL)
				((MNODE*)hTexHeap->pMemoryRoot)->NextFree->PrevFree=NewNode;
			((MNODE*)hTexHeap->pMemoryRoot)->NextFree=NewNode;

			NewNode->NextBuddy=NULL; /*not strictly required */
			NewNode->PrevBuddy=NULL;

		}

		MemWalk->BlockSize=RequestSize;
		MemWalk->UsedStatus=A_BANK;

		#if TMALLOC_BACKWARDS
			MemWalk->MemoryAddress=((MNODE*)hTexHeap->pMemoryRoot)->MemorySize-MemWalk->MemoryOffset-RequestSize;
		#endif

		FreeNode->Status=A_BANK;
		FreeNode->MNode=MemWalk;

		((MNODE*)hTexHeap->pMemoryRoot)->MemoryLeft-=RequestSize;
		((MNODE*)hTexHeap->pMemoryRoot)->AllocatedBlocks ++;

		return ;
	}
	
	/* there is no free space, so return with an error */

	FreeNode->Status=-1;
	FreeNode->MNode=NULL;

	#if DEBUG
	
		{
			sgl_uint32 Start, Size;
			DPF((DBG_WARNING, "TMalloc failed!: Requested size was %d", RequestSize));
			DPF((DBG_WARNING, "Set module debug level to 4 to dump texture heap"));
		
			MemWalk = hTexHeap->pMemoryRoot;

			while (MemWalk)
			{
				Start = MemWalk->MemoryOffset;
				Size = MemWalk->BlockSize;
				
				switch (MemWalk->UsedStatus)
				{
					case 0:
					{
						DPF ((DBG_MESSAGE, "\t%08lx (%d): Unused", Start, Size));
						break;
					}
					case A_BANK:
					{
						DPF ((DBG_MESSAGE, "\t%08lx (%d): Bank A used", Start, Size));
						break;
					}
					case B_BANK:
					{
						DPF ((DBG_MESSAGE, "\t%08lx (%d): Bank B used", Start, Size));
						break;
					}
					case A_BANK | B_BANK:
					{
						DPF ((DBG_MESSAGE, "\t%08lx (%d): Banks A&B used", Start, Size));
						break;
					}
				}
				
				MemWalk = MemWalk->Next;
			}
			
			DPF ((DBG_MESSAGE, "Free list:"));

			MemWalk = hTexHeap->pMemoryRoot;

			while (MemWalk)
			{
				Start = MemWalk->MemoryOffset;
				Size = MemWalk->BlockSize;
				
				switch (MemWalk->UsedStatus)
				{
					case 0:
					{
					    DPF ((DBG_MESSAGE, "\t%08lx (%d): Unused", Start, Size));
						break;
					}
					case A_BANK:
					{
						DPF ((DBG_MESSAGE, "\t%08lx (%d): Bank A used", Start, Size));
						break;
					}
					case B_BANK:
					{
						DPF ((DBG_MESSAGE, "\t%08lx (%d): Bank B used", Start, Size));
						break;
					}
					case A_BANK | B_BANK:
					{
						DPF ((DBG_MESSAGE, "\t%08lx (%d): Banks A&B used", Start, Size));
						break;
					}
				}
				
				MemWalk = MemWalk->NextFree;
			}
			
			DPF ((DBG_MESSAGE, "End of heap dump"));
		}
		
	#endif
}


/******************************************************************************
 * Function Name: TFree
 *
 * Inputs       : FreeNode,the MNODE_BLOCK that TMalloc returned for that
 *					       texture memory.
 * Outputs      : -
 * Returns      : -
 * Globals Used : MemoryRoot
 *				  MemoryLeft
 * Description  : This routine de-allocated memory from the texture memory.
 *				  This routine has to be called with a valid MNODE_BLOCK. So,
 *				  if the same block is deleted twise, the routine many not 
 *				  function correctly.
 * Pre-condition: InitTextureMemory has been called.
 *				: FreeNode is an allocated MNODE_BLOCK.
 *****************************************************************************/
void TFree(MNODE_BLOCK *FreeNode, HTEXHEAP hTexHeap) 
{

	MNODE *TempNode;

	((MNODE*)hTexHeap->pMemoryRoot)->MemoryLeft += FreeNode->MNode->BlockSize;

	/*
	** do we need to create an empty buddy ?
	*/


	if(FreeNode->MNode->UsedStatus == (A_BANK | B_BANK))
	{
	 	/*
		** insert the buddy after the root node.
		*/

		if(FreeNode->MNode != (MNODE*)hTexHeap->pMemoryRoot)
		{
			FreeNode->MNode->PrevBuddy=hTexHeap->pMemoryRoot;
			FreeNode->MNode->NextBuddy=((MNODE*)hTexHeap->pMemoryRoot)->NextBuddy;

			if(((MNODE*)hTexHeap->pMemoryRoot)->NextBuddy !=NULL)
				((MNODE*)hTexHeap->pMemoryRoot)->NextBuddy->PrevBuddy=FreeNode->MNode;
			((MNODE*)hTexHeap->pMemoryRoot)->NextBuddy=FreeNode->MNode;
		}

		/* clear the used status of the buddy */

		FreeNode->MNode->UsedStatus = FreeNode->Status ^ 3;

	}
	else
	{
		FreeNode->MNode->UsedStatus=0;

		/*
		**
		** free up the memory and try and coalesce with adjacent memory.
		**
		*/

		/* unlink the free buddy */


		if(FreeNode->MNode != (MNODE*)hTexHeap->pMemoryRoot)
		{
			FreeNode->MNode->PrevBuddy->NextBuddy=FreeNode->MNode->NextBuddy;
			if(FreeNode->MNode->NextBuddy != NULL)
				FreeNode->MNode->NextBuddy->PrevBuddy=FreeNode->MNode->PrevBuddy;
		}

		/* insert the free block after the root node */

		if(FreeNode->MNode != (MNODE*)hTexHeap->pMemoryRoot)
		{
			FreeNode->MNode->PrevFree=hTexHeap->pMemoryRoot;
			FreeNode->MNode->NextFree=((MNODE*)hTexHeap->pMemoryRoot)->NextFree;

			if(((MNODE*)hTexHeap->pMemoryRoot)->NextFree !=NULL)
				((MNODE*)hTexHeap->pMemoryRoot)->NextFree->PrevFree=FreeNode->MNode;
			((MNODE*)hTexHeap->pMemoryRoot)->NextFree=FreeNode->MNode;
		}

		/*
		** try and coalesce the free block 
		*/

		/* first try coelescing the next block */

		if(FreeNode->MNode->Next != NULL && FreeNode->MNode->Next->UsedStatus==0)
		{
			TempNode=FreeNode->MNode->Next;

			/* add the space togeather. */
 
			FreeNode->MNode->BlockSize += TempNode->BlockSize;

			/* unlink the node from the free list */

			TempNode->PrevFree->NextFree=TempNode->NextFree;
			if(TempNode->NextFree != NULL)
				TempNode->NextFree->PrevFree=TempNode->PrevFree;


			/* unlink the node from the master list*/

			FreeNode->MNode->Next=TempNode->Next;
			if(TempNode->Next != NULL)
				TempNode->Next->Prev=FreeNode->MNode;

			/* deallocate the MNode space */

			PVROSFree(TempNode);

		}

		/* secondly, try coelescing FreeNode with the previous block */

		if(FreeNode->MNode->Prev != NULL && FreeNode->MNode->Prev->UsedStatus==0)
		{
			/* add the space togeather. */

			FreeNode->MNode->Prev->BlockSize += FreeNode->MNode->BlockSize;

			/* unlink FreeNode from the free list */

			FreeNode->MNode->PrevFree->NextFree=FreeNode->MNode->NextFree;
			if(FreeNode->MNode->NextFree != NULL)
				FreeNode->MNode->NextFree->PrevFree=FreeNode->MNode->PrevFree;

			/* unlink the node from the master list*/

			FreeNode->MNode->Prev->Next=FreeNode->MNode->Next;
			if(FreeNode->MNode->Next != NULL)
				FreeNode->MNode->Next->Prev=FreeNode->MNode->Prev;

			/* deallocate the MNode space */

			PVROSFree(FreeNode->MNode);

		}
	}

	/* are we empty, and do we therefore need to reset the high water mark? */
		
	if (--((MNODE*)hTexHeap->pMemoryRoot)->AllocatedBlocks == 5)
	{
		DPF ((DBG_MESSAGE, "Setting high water mark: %d", ((MNODE*)hTexHeap->pMemoryRoot)->LowWaterMark));
		
		PVROSSetTSPHighWaterMark (hTexHeap, ((MNODE*)hTexHeap->pMemoryRoot)->LowWaterMark, TRUE);
	}
}

/******************************************************************************
 * Function Name: PVROSSetTSPHighWaterMark
 *
 * Inputs       : Sets the high water mark for textures in BANKA/B
 * Outputs      : -
 * Returns      : -
 * Globals Used : MemoryRoot
 * Description  : 
 * Pre-condition: 
 *****************************************************************************/
sgl_int32 CALL_CONV PVROSSetTSPHighWaterMark (PVRHANDLE TexHeap, sgl_int32 nRequested, sgl_bool bForceIt)
{
	MNODE 		*MemWalk;
	sgl_int32	nCurrentPos;
	HTEXHEAP	hTexHeap = TexHeap;

	MemWalk = hTexHeap->pMemoryRoot;

	if (MemWalk->LowWaterMark > nRequested)
	{
		MemWalk->LowWaterMark = nRequested;
	}

	while (MemWalk)
	{
		/*  we shouldn't ever get to the end, we should always have found the
			overflow/TSP blocks by now!
		*/
		
		ASSERT (MemWalk->Next);
		
		if (MemWalk->Next->BlockType != NORMAL_BLOCK)
		{
			nCurrentPos = (sgl_int32) ((MNODE*)hTexHeap->pMemoryRoot)->MemorySize - MemWalk->Next->MemoryOffset;

			if (MemWalk->Next->UsedStatus != A_BANK)
			{
				DPF((DBG_WARNING,"Can't move HWM - TSP Param block's buddy is in use"));
			}
			else if (MemWalk->UsedStatus != 0)
			{
				DPF((DBG_WARNING,"Can't move HWM - Top end of texture memory is in use"));
			}
			else if (!bForceIt && (nCurrentPos >= nRequested))
			{
				DPF((DBG_WARNING,"Requested HWM inside current limit (Req: %d, %d)", nRequested, nCurrentPos));
				
				nCurrentPos = nRequested;
			}
			else
			{
				sgl_int32 nOffset = nRequested - nCurrentPos;

				if (nOffset > (sgl_int32) MemWalk->BlockSize)
				{
					DPF((DBG_WARNING,"Can't move HWM - Top end Memory block too small"));
				}
				else
				{
					DPF((DBG_MESSAGE,"Extending HWM: %d -> %d\n", nCurrentPos, nRequested));

					MemWalk->Next->BlockSize += nOffset;
					MemWalk->Next->MemoryOffset -= nOffset;

					MemWalk->BlockSize -= nOffset;
					
					nCurrentPos = nRequested;
				}
			}

			break;
		}

		MemWalk = MemWalk->Next;
	}

	return (nCurrentPos);
}

/******************************************************************************
 * Function Name: PVROSGetTSPHighWaterMark
 *
 * Inputs       : Gets the high water mark for textures in BANKA/B
 * Outputs      : -
 * Returns      : -
 * Globals Used : MemoryRoot
 * Description  : 
 * Pre-condition: 
 *****************************************************************************/
sgl_int32 CALL_CONV PVROSGetTSPHighWaterMark (PVRHANDLE TexHeap)
{
	MNODE 		*MemWalk;
	HTEXHEAP	hTexHeap = TexHeap;

	MemWalk = hTexHeap->pMemoryRoot;

	while (MemWalk)
	{
		/*  we shouldn't ever get to the end, we should always have found the
			overflow/TSP blocks by now!
		*/
		
		ASSERT (MemWalk->Next);
		
		if (MemWalk->Next->BlockType != NORMAL_BLOCK)
		{
			return ((sgl_int32) ((MNODE*)hTexHeap->pMemoryRoot)->MemorySize - MemWalk->Next->MemoryOffset);
		}

		MemWalk = MemWalk->Next;
	}

	ASSERT (FALSE);
	
	return (0);
}

/*
// END OF FILE
*/


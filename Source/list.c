/*****************************************************************************
;++
Name           	:   $RCSfile: list.c,v $
Title           :   LIST.C
C Author       	:   Jim Page
Created         :   20/04/94

Copyright		: 1995-2022 Imagination Technologies (c)
License			: MIT

Description     :   List handling services. Modules that wish to take advantage
					of these services must define structures that have a LISTHDR
					see (list.h) as the FIRST member. And the list itself should be
					declared as type LIST. These services then form	a singly linked 
					list from the client's structures which can be searched, enummed 
					and destroyed. These are high level services and the workings of 
					the list are purposely hidden from the client. Each list has a 
					cache which can be varied in size according to your wishes by 
					defining LIST_ITEM_CACHE_SIZE (list.h) to be anything you like. 
					Callbacks for searching, enumming and destroying should be NEAR 
					and should be placed in the LIST_SEGMENT segment.  This significantly 
					improves speed. Debug builds output cache performance. 
					Happy listing!
                    
Program Type    :   C module (.dll)

RCS info:

  $Date: 1997/06/06 14:39:35 $
  $Revision: 1.36 $
  $Locker:  $
  $Log: list.c,v $
 * Revision 1.36  1997/06/06  14:39:35  mjg
 * Added a cast,
 * ,
 *
 * Revision 1.35  1997/05/13  17:54:25  mjg
 * Replaced PVROS memory management calls with SGL.
 *
 * Revision 1.34  1997/05/02  07:42:44  erf
 * Removed UINT32's etc and replaced with sgl_uint32's etc.
 *
 * Revision 1.33  1997/04/30  19:19:35  jop
 * Put Ed's changes back to how they were
 *
 * Revision 1.32  1997/04/29  14:47:09  erf
 * Added new PVROS API function call mechanism.
 *
 * Revision 1.31  1997/03/31  23:40:06  mlj
 * Fixes to get DOS32 driver to build
 *
 * Revision 1.30  1997/03/31  22:08:19  jop
 * Sorted out WIN32/DOS32 issues
 *
 * Revision 1.29  1997/03/27  12:36:08  erf
 * Operating system independent memory allocation/deallocation macros.
 * 1. Added #include "pvrosapi.h" to include new macros.
 * 2. Changed malloc, calloc, realloc and free to PVROSMalloc, PVROSCalloc,
 *    PVROSRealloc and PVROSFree.
 *
 * Revision 1.28  1996/11/14  15:10:23  erf
 * Added ) to 415 !!!
 *
 * Revision 1.27  1996/11/08  11:12:18  gdc
 * sorted a potential memcpy on overlapping memory in ListRemoveItem
 *
 * Revision 1.26  1996/07/11  16:39:53  sjf
 * Needed to swap order of sgl.h and sgl_defs.h includes.
 *
 * Revision 1.25  1996/07/05  07:56:18  sjf
 * Added asserts.
 *
 * Revision 1.24  1996/05/07  08:25:32  sjf
 * Tidied some code that was generating warnings with ICL compiler.
 *
 * Revision 1.23  1996/03/01  11:27:43  jop
 * Added a switch for VirtualAlloc in a windows build
 *
 * Revision 1.22  1996/02/16  12:24:37  jrc
 * Added missing semicolon for SUN version of code.
 *
 * Revision 1.21  1996/02/09  16:26:03  jop
 * Went to array method
 *
 * Revision 1.20  1995/10/30  16:56:31  jop
 * Virtual memory now unlocked
 *
 * Revision 1.19  1995/10/29  14:18:16  jop
 * Added more sensible virtual memory allocation scheme.
 *
 * Revision 1.18  1995/09/12  18:38:21  jop
 * Removed some functions to list.h - now inlined
 *
 * Revision 1.17  1995/08/03  19:01:00  jop
 * sorted out SUN realloc bug
 *
 * Revision 1.16  1995/08/01  17:01:31  jop
 * Made searches go forwards rather than backwards
 *
 * Revision 1.15  1995/07/23  17:16:58  jop
 * Added find first and find next structures
 *
 * Revision 1.14  1995/07/16  14:11:22  jop
 * Added GetFirst and GetNext
 *
 * Revision 1.13  1995/07/06  17:20:24  jop
 * removed warning.
 *
 * Revision 1.12  1995/06/06  15:14:57  jop
 * ListGetItemsInList now always returns a number
 *
 * Revision 1.11  1995/06/05  15:51:15  jop
 * removed bug from extension code
 *
 * Revision 1.10  1995/06/02  20:09:35  jop
 * Added ListGetItemsInList
 *
 * Revision 1.9  1995/06/02  15:41:21  jop
 * Made types compatible with sgl.h/sgl_defs.h
 *
 * Revision 1.8  1995/06/01  09:30:40  jop
 * Removed encryption
 *
 * Revision 1.7  1995/05/31  15:40:41  jop
 * moved NULL_ITEM def to list.h
 *
 * Revision 1.6  1995/05/18  12:53:41  jop
 * Made encryption easier to disable
 *
 * Revision 1.5  1995/05/13  18:55:44  jop
 * Rmoved some bugs preventing ListRemoveItem from working properly
 *
 * Revision 1.4  1995/05/04  17:54:01  jop
 * Fixed NULL ptr bug
 *
 * Revision 1.3  1995/05/02  10:54:43  jop
 * Changed over to pseudo hash-table format
 *
 * Revision 1.2  1995/04/27  13:53:08  jop
 * New text format
 *
 * Revision 1.1  1995/04/27  13:32:32  jop
 * Initial revision
 *
 
;--
*****************************************************************************/
/*
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * ++	                				Includes		                       	 ++
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */	

#include <stdlib.h>
#include <string.h>

#define __LIST_C__
#define MODULE_ID MODID_LIST

#include "sgl_defs.h"
#include "sgl.h"
#include "list.h"
#include "pvrosapi.h"
#include "sglmem.h"

/*
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * ++	                		static functions		                       	 ++
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */	

void *InternalRealloc (void *OldPtr, int NewSize)
{
	void *Ret;
	
	/* PVROSRealloc should take zero pointers and act like malloc - but doesn't on the SUN!! */

	if (OldPtr)
	{
		Ret = SGLRealloc (OldPtr, NewSize);
	}
	else
	{
		Ret = SGLMalloc (NewSize);
	}

	return (Ret);
}

/*===========================================
 * Function:	ExpandList
 *===========================================
 *
 * Scope:		Static to idThis module
 *
 * Purpose:		Extend the List array by a block's worth and initialise it.
 *
 * Params:		PLIST pvListData: list to which item belongs
 *				void *pItem: ptr to item
 *
 * Return:		void
 *========================================================================================*/
static sgl_bool ExpandList (PLIST pvListData)
{
	LIST *pList = pvListData;
	void *pNewItemList;

	pNewItemList = InternalRealloc (pList->pList, (pList->uBlockSize + pList->uMaxItems) * pList->uItemSize);
		
	if (!pNewItemList)
	{
		DPF ((DBG_WARNING, "Error expanding list!"));
	}
	else
	{
		pList->pList = pNewItemList;
		pList->uMaxItems += pList->uBlockSize;
	}

	return (pList->pList != NULL);
}

/*
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * ++	                		entry point functions	                       	 ++
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */	

/*===========================================
 * Function:	ListAddItem
 *===========================================
 *
 * Scope:		Project
 *
 * Purpose:		Allocate an item of size wSize and add item to list
 *
 * Params:		PLIST pvListData:list to add item to
 *
 * Return:		void *: ptr new item if succesful
 *						NULL if not
 *========================================================================================*/
void * ListAddItem (PLIST pvListData)
{
	void *pNewItem;
	LIST *pList;

	pList = pvListData;

	ASSERT (pList);

	if (pList->nItemsInList == (sgl_int32) pList->uMaxItems)
	{
		if (!ExpandList (pList))
		{
			return (NULL);
		}
	}

	ASSERT (pList->pList != NULL);

	pNewItem = (void *) ((sgl_uint32) pList->pList + (pList->uItemSize * pList->nItemsInList));
	pList->nItemsInList ++;

	return (pNewItem);
}

/*===========================================
 * Function:	ListFindItem
 *===========================================
 *
 * Scope:		Project
 *
 * Purpose:		Search list for item using passed callback.
 *
 * Params:		PLIST pvListData: List to search
 *				FINDITEMFN pfnFindItem: callback function tha returns TRUE if item matches
 *				sgl_uint32 wFindItemData: data to pass to callback
 *
 * Return:		void *: ptr to item if successful
 *						NULL if not
 *========================================================================================*/
void * ListFindItem (PLIST pvListData, FINDITEMFN pfnFindItem, sgl_uint32 wFindItemData)
{
	void *pItem = NULL;
	LIST *pList = pvListData;
	
	ASSERT (pList);

	if (!pList->pList)
	{
		DPF ((DBG_WARNING, "ListFindItem: NULL list!"));
	}
	else
	{
		if (pfnFindItem)
		{
			int k;
			void *pTempItem;
			
			pTempItem = pList->pList;
			
			for (k = 0;  k < pList->nItemsInList; ++k, 
									pTempItem = (void *) ((sgl_uint32) pTempItem + pList->uItemSize))
			{
				if (pfnFindItem (pTempItem, wFindItemData))
				{
					pItem = pTempItem;
					break;
				}
			}
		}
		else
		{
			pItem = (void *) ((sgl_uint32) pList->pList + (wFindItemData * pList->uItemSize));
		}
	}

	return (pItem);
}

/*===========================================
 * Function:	ListRemoveItem
 *===========================================
 *
 * Scope:		Project
 *
 * Purpose:		Find and remove item from list
 *
 * Params:		PLIST pvListData: list to search
 *				FINDITEMFN pfnFindItem: callback function that returns TRUE if item matches
 *				sgl_uint32 wFindItemData: data to pass to callback
 *
 * Return:		TRUE if successful
 *				FALSE if not
 *========================================================================================*/
sgl_bool ListRemoveItem (PLIST pvListData, FINDITEMFN pfnFindItem, sgl_uint32 wFindItemData)
{
	void *pItem = NULL;
	LIST *pList = pvListData;
	
	ASSERT (pList);

	if (!pList->pList)
	{
		DPF ((DBG_WARNING, "ListRemoveItem: NULL List"));
	}
	else
	{
		int which;
										
		if (pfnFindItem)
		{
			int k;
			void *pTempItem;
			
			pTempItem = pList->pList;
			
			for (k = 0;  k < pList->nItemsInList; ++k, 
										pTempItem = (void *) ((sgl_uint32) pTempItem + pList->uItemSize))
			{
				if (pfnFindItem (pTempItem, wFindItemData))
				{
					which = k;
					pItem = pTempItem;
					break;
				}
			}
		}
		else
		{
			which = wFindItemData;
			pItem = (void *) ((sgl_uint32) pList->pList + (wFindItemData * pList->uItemSize));

			/*
			// Check thawe haven't been asked to remove total crap
			*/
			ASSERT(which >=0);
			ASSERT(which < pList->nItemsInList);
		}

		if (pItem != NULL)
		{
			#if defined sun || defined sparc

				int i;
				/* for machines that I know dont have a memmove 
				** add yours to the list if it doesnt either
				*/
				for(i=which+1; i<pList->nItemsInList; i++)
					memcpy ((void *)((sgl_uint32)(pItem + pList->uItemSize*(i-1))), 
							(void *)((sgl_uint32) (pItem + pList->uItemSize*i)), 
							pList->uItemSize);
			#else

				/* machines with a memmove */
				memmove (pItem, (void *) ((sgl_uint32) pItem + pList->uItemSize), 
						 (pList->nItemsInList - (which + 1)) * pList->uItemSize);


			#endif
			
			--pList->nItemsInList;
		}
	}

	return (pItem != NULL);
}

/*===========================================
 * Function:	ListEnumItems
 *===========================================
 *
 * Scope:		Project
 *
 * Purpose:		Enumerate each item in the list and call the specified callback
 *
 * Params:		PLIST pvListData: List to search
 *				ENUMITEMFN pfnEnumItems: callback function to call on enumeration of members
 *				sgl_uint32 wEnumItemData: data to pass to callback
 *
 * Return:		Always TRUE
 *========================================================================================*/
sgl_bool ListEnumItems (PLIST pvListData, ENUMITEMFN pfnEnumItems, sgl_uint32 wEnumItemData)
{
	LIST *pList = pvListData;

	ASSERT (pList);

	if (!pList->pList)
	{
		DPF ((DBG_WARNING, "ListEnumItems: NULL List"));
	}
	else
	{
		int k;
		void *pTempItem = pList->pList;

		ASSERT (pfnEnumItems)
			
		for (k = 0;  k < pList->nItemsInList; ++k, 
						 pTempItem = (void *) ((sgl_uint32) pTempItem + pList->uItemSize))
		{
			pfnEnumItems (pTempItem, wEnumItemData);
		}
	}

	return (TRUE);
}

/*===========================================
 * Function:	ListInitialiseList
 *===========================================
 *
 * Scope:		VDI
 *
 * Purpose:		Initialise list (oddly enough)
 *
 * Params:		PLIST pList: list to initialise
 *				sgl_uint32 uItemSize: size of list item
 *				sgl_uint32 uBlockSize: rough likely size of list
 *				DESTROYITEMFN pfnOnDestroyItem: callback to call on member destruction
 *												 may be NULL.
 *
 * Return:		Always TRUE
 *========================================================================================*/
sgl_bool ListInitialiseList (PLIST *ppvListData, sgl_uint32 uItemSize, sgl_uint32 uBlockSize, DESTROYITEMFN pfnOnDestroyItem)
{
	LIST *pList = SGLMalloc (sizeof (LIST));

	ASSERT (ppvListData != NULL);

	if (pList)
	{
		pList->pList = NULL;
		pList->pfnOnDestroyItem = pfnOnDestroyItem;	

		if (uBlockSize)
		{
			pList->uBlockSize = uBlockSize;
		}
		else
		{
			ASSERT (uItemSize <= 4096);

			pList->uBlockSize = 4096 / uItemSize;
		}

		pList->uMaxItems = 0;
		pList->uItemSize = uItemSize;
		pList->nItemsInList = 0;
				
		*ppvListData = (PLIST) pList;

		return (ExpandList (pList));
	}
	else
	{
		return (FALSE);
	}
}

/*===========================================
 * Function:	ListDeleteList
 *===========================================
 *
 * Scope:		Project
 *
 * Purpose:		Delete members of the list and re-initialise it.
 *
 * Params:		PLIST pvListData: list to delete
 *
 * Return:		Always TRUE
 *========================================================================================*/
sgl_bool ListDeleteList (PLIST *ppvListData)
{
	LIST *pList = *ppvListData;

	ASSERT (ppvListData != NULL);

	if (pList)
	{
		if (pList->pList)
		{
			if (pList->pfnOnDestroyItem)
			{
				int k;
				void *pTempItem = pList->pList;

				for (k = 0;  k < pList->nItemsInList; ++k, 
						 pTempItem = (void *) ((sgl_uint32) pTempItem + pList->uItemSize))
				{
					pList->pfnOnDestroyItem (pTempItem);
				}
			}

			SGLFree (pList->pList);
		}

		SGLFree (pList);
		*ppvListData = NULL;
	}

	return (TRUE);
}

/* list.c */

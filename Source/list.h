/*****************************************************************************
;++
Name           	:   $RCSFile$
Title           :   LIST.H
C Author       	:   Jim Page
Created         :   20/04/94
Copyright		: 1995-2022 Imagination Technologies (c)
License			: MIT

Description     :   pList handling services. Modules that wish to take advantage
					of these services must define structures that have a LISTHDR
					see as the FIRST member. And the list itself should be
					declared as type LIST. These services then form	a singly linked 
					list from the client's structures which can be searched, enummed 
					and destroyed. These are high level services and the workings of 
					the list are purposely hidden from the client. Each list has a 
					cache which can be varied in size according to your wishes by 
					defining LIST_ITEM_CACHE_SIZE to be anything you like. 
					Callbacks for searching, enumming and destroying should be NEAR 
					and should be placed in the LIST_SEGMENT segment.  This significantly 
					improves speed. Debug builds output cache performance. 
					Happy listing!
                    
Program Type    :   C module (.dll)

RCS info:

  $Date: 1997/05/02 07:45:14 $
  $Revision: 1.14 $
  $Locker:  $
  $Log: list.h,v $
 * Revision 1.14  1997/05/02  07:45:14  erf
 * Removed UINT32's etc and replaced with sgl_uint32's etc.
 *
 * Revision 1.13  1996/02/09  16:31:30  jop
 * Went to array method
 *
 * Revision 1.12  1995/10/04  10:36:41  jop
 * Made GCC/ICC compatible inline definition
 *
 * Revision 1.11  1995/09/23  15:07:25  sjf
 * Changed static inline to extern inline for correct behaviour
 * of gcc 2.7.0
 *
 * Revision 1.10  1995/09/13  18:23:07  jop
 * ListGetFirst/NextItem removed, prototypes tidied up
 *
 * Revision 1.9  1995/09/12  18:38:51  jop
 * Several functions now inlined + watcom asm pragmas
 *
 * Revision 1.8  1995/07/16  14:11:48  jop
 * Added GetFirst and GetNext
 *
 * Revision 1.7  1995/06/06  15:10:39  sjf
 * Fixed param in ListDeleteList.
 *
 * Revision 1.6  1995/06/02  20:09:55  jop
 * Added ListGetItemsInList
 *
 * Revision 1.5  1995/06/02  15:41:48  jop
 * Made types compatible with sgl.h/sgl_types.h
 *
 * Revision 1.4  1995/05/31  15:39:12  jop
 * moved NULL_ITEM define to list.h
 *
 * Revision 1.3  1995/05/02  10:54:43  jop
 * Changed over to pseudo hash-table format
 *
 * Revision 1.2  1995/04/27  13:53:36  jop
 * Changed text format
 *
 * Revision 1.1  1995/04/27  13:33:09  jop
 * Initial revision
 *
;--
*****************************************************************************/

#ifndef __LIST__
#define __LIST__

/*
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * ++	              			Callback function typedefs 		         		 ++
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */	
/*===========================================
 * Typedef:		ENUMITEMFN
 *===========================================
 *
 * Scope:		physical: NEAR in LIST_SEGMENT segment
 *
 * Purpose:		Enumerator function called by ListEnumItems for each member. A sgl_uint32 data
 *				variable is passed from the client to the callback.
 *
 * Params:		void * Item: far ptr to Item
 *				sgl_uint32 wData: data variable passed to ListEnumItems by client
 *
 * Return:		void
 *========================================================================================*/
typedef void (* ENUMITEMFN)(void *, sgl_uint32);

/*===========================================
 * Typedef:		FINDITEMFN
 *===========================================
 *
 * Scope:		physical: NEAR in LIST_SEGMENT segment
 *
 * Purpose:		Callback function called by ListFindItem for each member. A sgl_uint32 data
 *				variable is passed from the client to the callback, usually referring
 *				to the target Item to find. Returns TRUE if the item matches, FALSE if
 *				not.  If the function returns TRUE, the search is aborted. Also used by
 *				ListRemoveItem.
 *
 * Params:		void * Item: far ptr to Item
 *				sgl_uint32 wData: data variable passed to ListFindItem by client
 *
 * Return:		void
 *========================================================================================*/
typedef sgl_bool (* FINDITEMFN)(void *, sgl_uint32);

/*===========================================
 * Typedef:		DESTROYITEMFN
 *===========================================
 *
 * Scope:		physical: NEAR in LIST_SEGMENT segment
 *
 * Purpose:		Callback function called by ListRemoveItem and called whenever a data item
 *				is about to be deallocated. It is passed in to ListInitialiseList
 *
 * Params:		void * Item: far ptr to Item
 *
 * Return:		void
 *========================================================================================*/
typedef const void *PCVOID;
typedef void (* DESTROYITEMFN)(PCVOID);

/*
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * ++	              			Structure definitions	 		         		 ++
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */	

typedef int ITEM_ID;

#define NULL_ITEM ((ITEM_ID)-1)

typedef struct tagLIST
{
	void			*pList;
	sgl_int32			nItemsInList;
	sgl_uint32			uMaxItems;
	sgl_uint32			uBlockSize;
	sgl_uint32			uItemSize;
	DESTROYITEMFN	pfnOnDestroyItem;
} LIST, *PLIST;

/*
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * ++	                		entry point prototypes	                       	 ++
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */	
/*===========================================
 * Function:	ListAddItem
 *===========================================
 *
 * Scope:		VDI
 *
 * Purpose:		Allocate and item of size wSize and add item to list
 *
 * Params:		PLIST pList:list to add item to
 *
 * Return:		void *: ptr new item if succesful
 *						NULL if not
 *========================================================================================*/
void * 	ListAddItem (PLIST pList);

/*===========================================
 * Function:	ListFindItem
 *===========================================
 *
 * Scope:		VDI
 *
 * Purpose:		Search list for item using passed callback. If item is not in cache
 *				add item to cache
 *
 * Params:		PLIST pList: pList to search
 *				FINDITEMFN pfnFindItem: callback function tha returns TRUE if item matches
 *				sgl_uint32 wFindItemData: data to pass to callback
 *
 * Return:		void *: far ptr to item if successful
 *						NULL if not
 *========================================================================================*/
void * 	ListFindItem (PLIST pList, FINDITEMFN pfnFindItem, sgl_uint32 wFindItemData);

/*===========================================
 * Function:	ListRemoveItem
 *===========================================
 *
 * Scope:		VDI
 *
 * Purpose:		Find and remove item from list
 *
 * Params:		PLIST pList: list to search
 *				FINDITEMFN pfnFindItem: callback function tha returns TRUE if item matches
 *				sgl_uint32 wFindItemData: data to pass to callback
 *
 * Return:		TRUE if successful
 *				FALSE if not
 *========================================================================================*/
sgl_bool 	ListRemoveItem (PLIST pList, FINDITEMFN pfnFindItem, sgl_uint32 wFindItemData);

/*===========================================
 * Function:	ListEnumItems
 *===========================================
 *
 * Scope:		VDI
 *
 * Purpose:		Enumerate each item in the list and call the specified callback
 *
 * Params:		PLIST pList: pList to search
 *				ENUMITEMFN pfnEnumItems: callback function to call on enumeration of members
 *				sgl_uint32 wEnumItemData: data to pass to callback
 *
 * Return:		Always TRUE
 *========================================================================================*/
sgl_bool 	ListEnumItems (PLIST pList, ENUMITEMFN pfnEnumItems, sgl_uint32 wEnumItemData);

/*===========================================
 * Function:	ListInitialiseList
 *===========================================
 *
 * Scope:		VDI
 *
 * Purpose:		Initialise list (oddly enough)
 *
 * Params:		PLIST pList: list to initialise
 *				sgl_uint32 uItemSize: size of items in list
 *				sgl_uint32 uBlockSize: rough likely size of list
 *				DESTROYITEMFN pfnOnDestroyItem: callback to call on member destruction
 *												 may be NULL.
 *
 * Return:		Always TRUE
 *========================================================================================*/
sgl_bool ListInitialiseList (PLIST *ppvListData, sgl_uint32 uItemSize, sgl_uint32 uBlockSize, DESTROYITEMFN pfnOnDestroyItem);

/*===========================================
 * Function:	ListDeleteList
 *===========================================
 *
 * Scope:		VDI
 *
 * Purpose:		Delete members of the list and re-initialise it.
 *
 * Params:		PLIST pList: list to delete
 *
 * Return:		Always TRUE
 *========================================================================================*/
sgl_bool 	ListDeleteList (PLIST * pList);

/*
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * ++	              			Inline functions		 		         		 ++
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */	

#if (INLINING || defined (__LIST_C__))

/*
	make sure there are functions in list.c even if we are inlining - external
	modules can call them.
*/

#if (INLINING && !defined (__LIST_C__))
	#if defined (GCC)
		#define STATIC_IN INLINE extern
	#else
		#define STATIC_IN INLINE static
	#endif
#else
	#define STATIC_IN
#endif

/*===========================================
 * Function:	ListGetItemID
 *===========================================
 *
 * Scope:		Project
 *
 * Purpose:		Get the ID from a data block
 *
 * Params:		PLIST pList: list to delete
 *				void *pData: item
 *
 * Return:		ITEM_ID ID of item
 *========================================================================================*/
STATIC_IN ITEM_ID ListGetItemID (PLIST pList, void *pData)
{
	ITEM_ID ID;

	ASSERT (pList);

	if (pData)
	{
		ID = ((sgl_uint32) pData - (sgl_uint32) pList->pList) / pList->uItemSize;	
	}
	else
	{
		DPF ((DBG_WARNING, "ListGetItemID: Null pData parameter"));
		return (NULL_ITEM);
	}

	return (ID);
}

/*===========================================
 * Function:	ListFindItemFast
 *===========================================
 *
 * Scope:		Project
 *
 * Purpose:		Search list for item using passed callback.
 *
 * Params:		PLIST pvListData: List to search
 *				sgl_uint32 wFindItemData: data to pass to callback
 *
 * Return:		void *: ptr to item if successful
 *						NULL if not
 *========================================================================================*/
STATIC_IN void * ListFindItemFast (PLIST pList, sgl_uint32 wFindItemData)
{
	ASSERT (pList);
	ASSERT (pList->pList);

	return ((void *) ((sgl_uint32) pList->pList + (wFindItemData * pList->uItemSize)));
}

/*===========================================
 * Function:	ListGetItemsInList
 *===========================================
 *
 * Scope:		Project
 *
 * Purpose:		Get the item count
 *
 * Params:		PLIST pvListData: List to search
 *
 * Return:		int: number of items added to list
 *========================================================================================*/
STATIC_IN int ListGetItemsInList (PLIST pList)
{
	ASSERT (pList);

	if (pList)
	{
		return (pList->nItemsInList);
	}
	return (0);
}

#else

ITEM_ID ListGetItemID (PLIST pList, void *pData);
void * ListFindItemFast (PLIST pvListData, sgl_uint32 wFindItemData);
int ListGetItemsInList (PLIST pvListData);
void * ListGetFirstItem (PLIST pvListData);
void * ListGetNextItem (PLIST pvListData);

#endif	/* INLINING || defined (__LIST_C__) */


/*
// remove the local STATIC_IN definition
*/
#undef STATIC_IN


#endif	/* __LIST__ */

/* list.h */


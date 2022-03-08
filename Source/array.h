/*****************************************************************************
 *;++
 * Name           : $RCSfile: array.h,v $
 * Title          : ARRAY.H
 * C Author       : Jim Page
 * Created        : 21/04/95
 *
 * Copyright	  : 1995-2022 Imagination Technologies (c)
 * License		  : MIT
 *
 * Description    :	Header for extendable int array
 *                   
 * Program Type   :	C (SGL cross-platform)
 *
 * RCS info:
 *
 * $Date: 1995/07/17 11:08:32 $
 * $Revision: 1.1 $
 * $Locker:  $
 * $Log: array.h,v $
 * Revision 1.1  1995/07/17  11:08:32  jop
 * Initial revision
 *
 * Revision 1.1  1995/07/16  14:11:01  jop
 * Initial revision
 *
 *
 *;--
 *****************************************************************************/

#ifndef __ARRAY__
#define __ARRAY__

typedef struct tagINTARRAY
{
	char *pArray;
	int nItemSize;
	int nBlockSize;
	int nMaxItems;
	int nItems;

} ARRAY, *PARRAY;

int  ArrayInitialiseArray (PARRAY pArray, int nItemSize, int nBlockSize);
void ArrayDeleteArray (PARRAY pArray);
void *ArrayAddItem (PARRAY pArray);
void *ArrayGetItem (PARRAY pArray, int nID);
int  ArrayRemoveItem (PARRAY pArray, int nID);
int  ArrayGetNumItems (PARRAY pArray);
void ArrayPrintStats (void);

#endif

/* end of $RCSfile: array.h,v $ */

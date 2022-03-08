/*****************************************************************************
 *;++
 * Name           : $RCSfile: array.c,v $
 * Title          : ARRAY.C
 * C Author       : Jim Page
 * Created        : 21/04/95
 *
 * Copyright	  : 1995-2022 Imagination Technologies (c)
 * License		  : MIT
 *
 * Description    :	Extendable array services
 *                   
 * Program Type   :	C (SGL cross-platform)
 *
 * RCS info:
 *
 * $Date: 1997/05/13 17:50:33 $
 * $Revision: 1.7 $
 * $Locker:  $
 * $Log: array.c,v $
 * Revision 1.7  1997/05/13  17:50:33  mjg
 * Replaced PVROS memory management calls with SGL.
 *
 * Revision 1.6  1997/04/30  19:17:33  jop
 * Put Ed's changes back to how they were
 *
 * Revision 1.5  1997/04/29  14:38:16  erf
 * Added new PVROS API function calls.
 *
 * Revision 1.4  1997/03/27  12:36:08  erf
 * Operating system independent memory allocation/deallocation macros.
 * 1. Added #include "pvrosapi.h" to include new macros.
 * 2. Changed malloc, calloc, realloc and free to PVROSMalloc, PVROSCalloc,
 *    PVROSRealloc and PVROSFree.
 *
 * Revision 1.3  1996/07/11  16:38:35  sjf
 * Needed to swap order of sgl.h and sgl_defs.h includes.
 *
 * Revision 1.2  1995/07/26  15:29:40  jop
 * removed memmove reference - not available on Sun
 *
 * Revision 1.1  1995/07/17  11:08:32  jop
 * Initial revision
 *
 * Revision 1.1  1995/07/16  14:10:32  jop
 * Initial revision
 *
 *
 *;--
 *****************************************************************************/

#include <string.h>

#define MODULE_ID MODID_LIST

#include "sgl_defs.h"
#include "sgl.h"
#include "array.h"
#include "pvrosapi.h"
#include "sglmem.h"

#define ARRAY_STATS
#ifdef ARRAY_STATS
static int gnInits = 0;
static int gnDeletes = 0;
static int gnAdds = 0;
static int gnExtends = 0;
static int gnGets = 0;
static int gnRemoves = 0;
static int gnNum = 0;
#endif

int ArrayInitialiseArray (PARRAY pArray, int nItemSize, int nBlockSize)
{
	pArray->pArray = SGLMalloc (nBlockSize * nItemSize);
	pArray->nItemSize = nItemSize;
	pArray->nBlockSize = nBlockSize;
	pArray->nMaxItems = nBlockSize;
	pArray->nItems = 0;

	gnInits++;

	return (pArray->pArray != NULL);
}

void ArrayDeleteArray (PARRAY pArray)
{
	gnDeletes++;

	if (pArray->pArray)
	{
		SGLFree (pArray->pArray);
		pArray->pArray = NULL;
	}

	if (pArray->nItems)
	{
		DPF ((DBG_VERBOSE, "Array being deleted contained %d items!", pArray->nItems));
	}

	pArray->nItemSize = 0;
	pArray->nBlockSize = 0;
	pArray->nItems = 0;
}

void *ArrayAddItem (PARRAY pArray)
{
	int nID;

	ASSERT (pArray);
	ASSERT (pArray->pArray);

	gnAdds++;

	nID = pArray->nItems++;

	if (pArray->nItems == pArray->nMaxItems)
	{
		char *pNewArray = SGLRealloc (pArray->pArray, (pArray->nMaxItems + pArray->nBlockSize) * pArray->nItemSize);

		++gnExtends;

		if (pNewArray)
		{
			pArray->pArray = pNewArray;
			pArray->nMaxItems += pArray->nBlockSize;
		}
		else
		{
			nID = -1;
		}
	}

	return (pArray->pArray + (nID * pArray->nItemSize));
}

void *ArrayGetItem (PARRAY pArray, int nID)
{
	ASSERT (pArray);
	ASSERT (pArray->pArray);

	gnGets++;

	if ((nID >= 0) && (nID < pArray->nItems))
	{
		return (pArray->pArray + (nID * pArray->nItemSize));
	}
	else
	{
		DPF ((DBG_ERROR, "ArrayGetItem - attempt to get item outside array bounds (%d)", nID));
		return (0);
	}
}

int ArrayRemoveItem (PARRAY pArray, int nID)
{
	ASSERT (pArray);
	ASSERT (pArray->pArray);

	++gnRemoves;

	if ((nID >= 0) && (nID < pArray->nItems))
	{
		char *p1 = pArray->pArray + (nID * pArray->nItemSize);
		char *p2 = p1 + pArray->nItemSize;
		int k, nStuffToMove;
		
		/* memcpy is undefined for overlapping buffers */
		
		nStuffToMove = (pArray->nItems - (nID + 1)) * pArray->nItemSize;
		
		for (k = 0; k < nStuffToMove; ++k)
		{
			p1[k] = p2[k];
		}
		
		/* one less item in the array */
		
		pArray->nItems--;

		return (1);
	}
	else
	{
		DPF ((DBG_ERROR, "ArrayRemoveItem - attempt to remove item outside array bounds (%d)", nID));
		return (0);
	}
}

int ArrayGetNumItems (PARRAY pArray)
{
	ASSERT (pArray);
	ASSERT (pArray->pArray);

	gnNum++;

	return (pArray->nItems);
}

void ArrayPrintStats (void)
{
	DPF ((DBG_MESSAGE, "Array stats: Inits %d, Deletes %d, Adds %d, Gets %d, Removes %d, Num %d, Extends %d",
								gnInits,
								gnDeletes,
								gnAdds,
								gnGets,
								gnRemoves,
								gnNum,
								gnExtends));
}

/* end of $RCSfile: array.c,v $ */


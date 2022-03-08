/*****************************************************************************
 *;++
 * Name           : $RCSfile: callback.c,v $
 * Title          : callback.C
 * C Author       : Jim Page
 * Created        : 21/04/95
 *
 * Copyright	  : 1995-2022 Imagination Technologies (c)
 * License		  : MIT
 *
 * Description    :	generic callback API
 *                   
 * Program Type   :	ansi
 *
 * RCS info:
 *
 * $Date: 1997/11/06 14:41:15 $
 * $Revision: 1.14 $
 * $Locker:  $
 * $Log: callback.c,v $
 * Revision 1.14  1997/11/06  14:41:15  sxy
 * added extra check in PVROSCallbackRegister() to avoid the same callback
 * be added again, fixed the flip buffer bug in Actua Soccer2.
 *
 * Revision 1.13  1997/10/16  11:41:57  sxy
 * removed DPFDEVFs, its all in DEPDEV.
 *
 * Revision 1.12  1997/10/15  09:36:15  sxy
 * added some DPFDEVF messages.
 *
 * Revision 1.11  1997/09/24  14:05:29  sxy
 * changed some DPF to DPFDEV.
 *
 * Revision 1.10  1997/07/24  15:55:51  gdc
 * removed un-needed windows.h include
 *
 * Revision 1.9  1997/07/14  10:00:38  gdc
 * tried to turn off windows include file warnings and failed
 *
 * Revision 1.8  1997/05/30  16:28:49  mjg
 * 1.  Callback datatype now stores the logical device (ID)
 *     rather than the process ID.
 * 2.  PVROSCallbackRegister will check to see if a specified
 *     callback already exists (i.e. same Context, Logical
 *     Device, Function and Type)
 *
 * Revision 1.7  1997/05/22  20:22:43  mjg
 * Added ResetCallbacks to ensure all old callbacks are removed.
 *
 * Revision 1.6  1997/05/19  18:37:02  jop
 * Added processID to callback functionality
 *
 * Revision 1.5  1997/04/29  14:39:08  erf
 * Removed #include "pvrosP.h".
 *
 * Revision 1.4  1997/04/25  13:56:18  gdc
 * added private pvros header file
 *
 * Revision 1.3  1997/04/03  13:12:10  mjg
 * Updated the calling convention.
 *
 * Revision 1.2  1997/03/27  12:36:08  erf
 * Operating system independent memory allocation/deallocation macros.
 * 1. Added #include "pvrosapi.h" to include new macros.
 * 2. Changed malloc, calloc, realloc and free to PVROSMalloc, PVROSCalloc,
 *    PVROSRealloc and PVROSFree.
 *
 * Revision 1.1  1997/03/25  10:13:50  jop
 * Initial revision
 *
 *
 *;--
 *****************************************************************************/
#define MODULE_ID MODID_CALLBACK

#include "sgl.h"
#include "sgl_defs.h"
#include "pvrosapi.h"
#include "debug.h"

typedef struct PVRCALLBACK
{
	struct PVRCALLBACK *pNext;
	PVROSCB			fn;
	void			*pContext;
	void			*pLogicalDev;
} PVRCALLBACK, *PPVRCALLBACK;

static PPVRCALLBACK	pCallback[CB_MAX];

static char *pszCB[] = 
{
	"pre render",
	"post render",
	"end of render",
	"2d"
};

PVROSERR CALL_CONV PVROSCallbackRegister (void *pLogDevice, CALLBACK_TYPE cType, PVROSCB fn, void *pContext)
{
	if (!fn)
	{
		DPFDEV ((DBG_ERROR, "PVROSCallbackRegister: null fn ptr"));
	}
	else
	{
		PPVRCALLBACK pCB = PVROSMalloc (sizeof (PVRCALLBACK));
		
		if (!pCB)
		{
			DPF ((DBG_WARNING, "PVROSCallbackRegister: PVROSMalloc failed"));
		}
		else
		{
			if ((cType >= 0) && (cType < CB_MAX))
			{
				PPVRCALLBACK	*pCBRoot = pCallback + cType;

				DPF ((DBG_MESSAGE, "PVROSCallbackRegister: %s callback registered", pszCB[cType]));

				if (*pCBRoot)
				{
					PPVRCALLBACK pCBThis = *pCBRoot;
					
					/* Search through the list of callbacks, new ones get
					   added onto the tail. */

					do
					{
						if ( (pCBThis->fn == fn) &&
							 (pCBThis->pContext == pContext) &&
							 (pCBThis->pLogicalDev == pLogDevice) )
						{
							/* Duplicate so don't replace*/
							PVROSFree (pCB);					
							return (PVROS_GROOVY);										
						}

						if (pCBThis->pNext != NULL)
						{
							pCBThis = pCBThis->pNext;
						}
					}
					while(pCBThis->pNext != NULL);
					
					ASSERT (pCBThis != NULL);
					if ( (pCBThis->fn == fn) &&
						 (pCBThis->pContext == pContext) &&
						 (pCBThis->pLogicalDev == pLogDevice) )
					{
						/* Duplicate so don't replace*/
						PVROSFree (pCB);					
						return (PVROS_GROOVY);										
					}
					
					pCBThis->pNext = pCB;					
				}
				else
				{
					*pCBRoot = pCB;
				}
				
				pCB->pNext = NULL;
				pCB->fn = fn;
				pCB->pContext = pContext;
				pCB->pLogicalDev = pLogDevice;
				return (PVROS_GROOVY);				
			}
			else
			{
				DPFDEV ((DBG_ERROR, "PVROSCallbackRegister: bad callback type"));			
			}

			PVROSFree (pCB);		
		}
	}
	
	return (PVROS_DODGY);	
}	

PVROSERR CALL_CONV PVROSCallbackUnRegister (void *pLogDevice, CALLBACK_TYPE cType, PVROSCB fn, void *pContext)
{
	if ((cType >= 0) && (cType < CB_MAX))
	{
		PPVRCALLBACK	*pCBRoot = pCallback + cType;
		PPVRCALLBACK	pCB;
		PPVRCALLBACK	pCBLast = NULL;

		pCB = *pCBRoot;
		
		while (pCB)
		{
			if ( (pCB->fn == fn) &&
				 (pCB->pContext == pContext) &&
				 (pCB->pLogicalDev == pLogDevice) )
			{
				DPF ((DBG_MESSAGE, "PVROSCallbackUnRegister: %s callback unregistered OK", pszCB[cType]));
				
				if (pCBLast)
				{
					pCBLast->pNext = pCB->pNext;
				}
				else
				{
					*pCBRoot = pCB->pNext;
				}
				
				PVROSFree (pCB);					
				
				return (PVROS_GROOVY);				
			}
			
			pCBLast = pCB;
			pCB = pCB->pNext;			
		}

		DPFDEV ((DBG_WARNING, "PVROSCallbackUnRegister: couldn't find %s callback", pszCB[cType]));	
	}
	else
	{
		DPFDEV ((DBG_ERROR, "PVROSCallbackUnRegister: bad callback type"));	
	}
	
	return (PVROS_DODGY);	
}	

PVROSERR CALL_CONV PVROSCallback (void *pLogDevice, CALLBACK_TYPE cType, void *pCallbackData)
{
	PVROSERR Err = PVROS_DODGY;
	
	if ((cType >= 0) && (cType < CB_MAX))
	{
		PPVRCALLBACK	*pCBRoot = pCallback + cType;
		PPVRCALLBACK 	pPC;

		DPF ((DBG_MESSAGE, "PVROSCallback: calling %s", pszCB[cType]));

		pPC = *pCBRoot;
		
		while (pPC)
		{
			if (pPC->pLogicalDev == pLogDevice)
			{
				Err = pPC->fn (pCallbackData, pPC->pContext);
				
				if (Err != PVROS_GROOVY)
				{
					DPFDEV ((DBG_ERROR, "PVROSCallback: %s callback failed", pszCB[cType]));					
					break;				
				}
			}
						
			pPC = pPC->pNext;
		}
	}
	else
	{
		DPFDEV ((DBG_ERROR, "PVROSCallback: bad callback type"));
	}
	
	return (Err);	
}	


/* As Windows reuses process Ids, it appears sadly necessary to 
 * examine all the callbacks and remove any redundant ones - 
 * otherwise it'll get all confused and page-fault.
 */

void ResetCallbacks(void *pLogDevice)
{
	int cType;

	for (cType=0 ; cType< CB_MAX ; ++cType)
	{
		PPVRCALLBACK	*pCBRoot = pCallback + cType;
		PPVRCALLBACK	pCB = *pCBRoot;
		PPVRCALLBACK	pCBLast = NULL;
		
	  	while (pCB)
		{
			if ((pCB->pLogicalDev == pLogDevice))
			{
				DPF ((DBG_MESSAGE, "ResetCallbacks: %s callback has being freed.", pszCB[cType]));
	  								
				if (pCBLast)
				{
					pCBLast->pNext = pCB->pNext;
				}
				else
				{
					*pCBRoot = pCB->pNext;
				}

				PVROSFree (pCB);
				break;									
			}
			pCBLast = pCB;			
			pCB = pCB->pNext;			
		}
	}
}

/* end of $RCSfile: callback.c,v $ */

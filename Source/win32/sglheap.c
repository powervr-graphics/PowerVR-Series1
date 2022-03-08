/*****************************************************************************
 *;++
 * Name           : $RCSfile: sglheap.c,v $
 * Title          : SGLHEAP.C
 * C Author       : Jim Page (nicked from microsoft demo sample driver)
 * Created        : 21/04/95
 *
 * Copyright	: 1995-2022 Imagination Technologies (c)
 * License		: MIT
 *
 * Description    :	WIN32 heap services (non-shared!) for SGL and SGLDirect.
 *                   
 * Program Type   :	WIN32 DLL
 *
 * RCS info:
 *
 * $Date: 1997/09/22 09:50:11 $
 * $Revision: 1.1 $
 * $Locker:  $
 * $Log: sglheap.c,v $
 * Revision 1.1  1997/09/22  09:50:11  erf
 * Initial revision
 *
 *
 *****************************************************************************/

#include "sgl_defs.h"
#include "pvrosapi.h"

#include <windows.h>
#include "heap.h"

#pragma warning(disable:4002)

HANDLE g_hDDHAL_DriverHeap = NULL;            // handle to shared heap for this DLL

/*
 * memory track struct and list
 */
#if DEBUG
#define MCOOKIE	0xbaaabaaa
#define MCOOKIE_FREE	0xbabababa
typedef struct _MEMTRACK
{
    DWORD		dwCookie;
    struct _MEMTRACK	FAR *lpNext;
    struct _MEMTRACK	FAR *lpPrev;
    DWORD		dwSize;
    void *		lpAddr;
    DWORD		dwPid;
} MEMTRACK, FAR *LPMEMTRACK;

static LPMEMTRACK	lpHead = 0;
static LPMEMTRACK	lpTail = 0;
static LONG		lAllocCount = 0;

void MemState (void);

#define DEBUG_TRACK( lptr, first ) \
    if( lptr == NULL ) \
    { \
		PVROSPrintf ("Alloc of size %u FAILED!\n", size ); \
		MemState (); \
    } \
    else \
    { \
		LPMEMTRACK	pmt; \
		pmt = (LPMEMTRACK) lptr; \
		pmt->dwSize = size - sizeof( MEMTRACK ); \
		pmt->dwCookie = MCOOKIE; \
		pmt->lpAddr = (void *) *(DWORD *)(((LPBYTE)&first)-4); \
		pmt->dwPid = GetCurrentProcessId(); \
		if( lpHead == NULL ) \
		{ \
		    lpHead = lpTail = pmt; \
		} \
		else \
		{ \
		    lpTail->lpNext = pmt; \
		    pmt->lpPrev = lpTail; \
		    lpTail = pmt; \
		} \
		lptr = (void *) (((LPBYTE) lptr) + sizeof( MEMTRACK )); \
		lAllocCount++; \
    }

#define DEBUG_TRACK_UPDATE_SIZE( s ) s += sizeof( MEMTRACK );

#else

#define DEBUG_TRACK( lptr, first )
#define DEBUG_TRACK_UPDATE_SIZE( size )

#endif

/*
 * MemAlloc - allocate memory from our global pool
 */
void * MemAlloc( unsigned long size )
{
    LPBYTE lptr;

    DEBUG_TRACK_UPDATE_SIZE( size );
    lptr = HeapAlloc( g_hDDHAL_DriverHeap, HEAP_ZERO_MEMORY, size );
    DEBUG_TRACK( lptr, size );

    return lptr;

} /* MemAlloc */

/*
 * MemSize - return size of object
 */
unsigned long MemSize( void * lptr )
{
#if DEBUG
    if (lptr)
    {
        LPMEMTRACK  pmt;
        lptr = (void *) (((LPBYTE)lptr) - sizeof( MEMTRACK ));
        pmt = lptr;
        return pmt->dwSize;
    }
#endif
    return HeapSize(g_hDDHAL_DriverHeap, 0, lptr);

} /* MemSize */

/*
 * MemFree - free memory from our global pool
 */
void MemFree( void * lptr )
{
    if( lptr != NULL )
    {
		#if DEBUG
		{
		    /*
		     * get real pointer and unlink from chain
		     */
		    LPMEMTRACK	pmt;
		    lptr = (void *) (((LPBYTE)lptr) - sizeof( MEMTRACK ));
		    pmt = lptr;

		    if( pmt->dwCookie == MCOOKIE_FREE )
		    {
				DPF((DBG_ERROR,"FREE OF FREED MEMORY! ptr=%08lx", pmt ));
				DPF((DBG_ERROR,"%08lx: dwSize=%08lx, lpAddr=%08lx", pmt, pmt->dwSize, pmt->lpAddr ));
		    }
		    else if( pmt->dwCookie != MCOOKIE )
		    {
				DPF((DBG_ERROR,"INVALID FREE! cookie=%08lx, ptr = %08lx", pmt->dwCookie, lptr ));
				DPF((DBG_ERROR,"%08lx: dwSize=%08lx, lpAddr=%08lx", pmt, pmt->dwSize, pmt->lpAddr ));
		    }
		    else
		    {
				if (!HeapValidate (g_hDDHAL_DriverHeap, 0, NULL))
				{
					DPF((DBG_ERROR,"MemFree: Heap is invalid!"));
				}
				else
				{

					DPF((DBG_VERBOSE,"MemFree: Heap appears to be valid"));

					pmt->dwCookie = MCOOKIE_FREE;
					if( pmt == lpHead && pmt == lpTail )
					{
					    lpHead = NULL;
					    lpTail = NULL;
					}
					else if( pmt == lpHead )
					{
					    lpHead = pmt->lpNext;
					    lpHead->lpPrev = NULL;
					}
					else if( pmt == lpTail )
					{
					    lpTail = pmt->lpPrev;
					    lpTail->lpNext = NULL;
					}
					else
					{
					    pmt->lpPrev->lpNext = pmt->lpNext;
					    pmt->lpNext->lpPrev = pmt->lpPrev;
					}
				}
		    }
		    lAllocCount--;
		    if( lAllocCount < 0 )
		    {
				DPF((DBG_WARNING,"Too Many Frees!" ));
		    }
		}
		#endif

        HeapFree( g_hDDHAL_DriverHeap, 0, lptr );

    }

} /* MemFree */

/*
 * MemReAlloc
 */
void * MemReAlloc( void * lptr, unsigned long size )
{
    void * new;

    DEBUG_TRACK_UPDATE_SIZE( size );
    
    #if DEBUG
	if( lptr != NULL )
	{
	    LPMEMTRACK	pmt;
	    lptr = (void *) (((LPBYTE)lptr) - sizeof( MEMTRACK ));
	    pmt = lptr;
	    if( pmt->dwCookie != MCOOKIE )
	    {
			DPF((DBG_ERROR,"INVALID REALLOC! cookie=%08lx, ptr = %08lx", pmt->dwCookie, lptr ));
			DPF((DBG_ERROR,"%08lx: dwSize=%08lx, lpAddr=%08lx", pmt, pmt->dwSize, pmt->lpAddr ));
	    }
	}
    #endif

    new = HeapReAlloc( g_hDDHAL_DriverHeap, HEAP_ZERO_MEMORY, lptr, size );

    #if DEBUG
    if (new != NULL)
    {
		LPMEMTRACK pmt = new;

		pmt->dwSize = size - sizeof( MEMTRACK );

		if( lptr == (void *)lpHead )
		{
		    lpHead = pmt;
		}
		else
		{
		    pmt->lpPrev->lpNext = pmt;
		}

		if( lptr == (void *)lpTail )
		{
		    lpTail = pmt;
		}
		else
		{
		    pmt->lpNext->lpPrev = pmt;
		}

		new = (void *) (((LPBYTE)new) + sizeof(MEMTRACK));
    }
    #endif

    return new;

} /* MemReAlloc */

/*
 * MemInit - initialize the heap manager
 */
int MemInit( void )
{
    if( g_hDDHAL_DriverHeap == NULL )
    {
		/* Create non-shared memory heap.
		 */
        g_hDDHAL_DriverHeap = HeapCreate( 0, 0x2000, 0 );
        if( g_hDDHAL_DriverHeap == NULL )
		{
		    return FALSE;
		}
    }
    #if DEBUG
   	lAllocCount = 0;
	lpHead = NULL;
	lpTail = NULL;
    #endif
    return TRUE;

} /* MemInit */

#if DEBUG
/*
 * MemState - finished with our heap manager
 */
void MemState( void )
{
    DPF((DBG_MESSAGE,"MemState: checking ..." ));
    
    if( lAllocCount != 0 )
    {
		DPF((DBG_MESSAGE, "Memory still allocated!  Alloc count = %ld", lAllocCount ));
    }
    
    if( lpHead != NULL )
    {
		LPMEMTRACK	pmt;
		pmt = lpHead;
		while( pmt != NULL )
		{
		    DPF((DBG_VERBOSE,"%08lx: dwSize=%08lx, lpAddr=%08lx (pid=%08lx)", pmt, pmt->dwSize, pmt->lpAddr, pmt->dwPid ));
		    pmt = pmt->lpNext;
		}
    }

    DPF((DBG_MESSAGE,"MemState: ...complete" ));
} /* MemState */
#endif

/*
 * MemFini - finished with our heap manager
 */
void MemFini( void )
{
    #if DEBUG
	    DPF ((DBG_MESSAGE,"Destroying heap" ));
    	MemState();
    #endif
    if( g_hDDHAL_DriverHeap )
    {
        HeapDestroy( g_hDDHAL_DriverHeap );
        g_hDDHAL_DriverHeap = NULL;
    }
} /* MemFini */

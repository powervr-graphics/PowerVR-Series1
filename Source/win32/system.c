/*****************************************************************************
 *;++
 * Name           : $RCSfile: system.c,v $
 * Title          : SYSTEM.C
 * C Author       : Jim Page
 * Created        : 21/04/95
 *
 * Copyright	  : 1995-2022 Imagination Technologies (c)
 * License		  : MIT
 *
 * Description    :	WIN32 system API
 *                   
 * Program Type   :	WIN32 DLL
 *
 * RCS info:
 *
 * $Date: 1998/07/14 17:29:15 $
 * $Revision: 1.95 $
 * $Locker:  $
 * $Log: system.c,v $
 * Revision 1.95  1998/07/14  17:29:15  jrc
 * Nick T's fix for multiple library loading in apps that create multiple
 * devices.
 *
 * Revision 1.94  1997/12/18  12:53:12  erf
 * Changed PC98 detection so that we check a range.
 *
 * Revision 1.93  1997/11/21  12:09:08  sxy
 * made DebugInit() available to TIMING build.
 *
 * Revision 1.92  1997/11/07  18:07:14  gdc
 * made fx problem card detection use new pvrsmx.dll
 *
 * Revision 1.91  1997/10/31  15:32:27  erf
 * Added TSP parameter memory allocatoin functions; PVROSTSPAlloc(),
 * PVROSTSPRealloc() and PVROSTSPFree().
 *
 * Revision 1.90  1997/10/23  17:09:11  sxy
 * changed the number of shared buffer for DPFDEV into a parameter NUMBER_OF_SHARED_BUFFER.
 *
 * Revision 1.89  1997/10/22  10:26:31  sxy
 * changed PVROSDbgDevBuffer().
 *
 * Revision 1.88  1997/10/16  11:10:52  gdc
 * chnaged developer build stuff to make it tidier
 *
 * Revision 1.87  1997/10/15  17:08:15  mjg
 * Put back the PC98 changes I made for revision 1.85
 *
 * Revision 1.86  1997/10/14  16:37:50  sxy
 * added a function PVROSDbgDevBuffer() to return the shared buffer for DPFDEVF
 * and find the handle of "Bunny".
 *
 * Revision 1.84  1997/09/29  14:14:17  sxy
 * changed debug level to MESSAGE for DEBUGDEV.
 *
 * Revision 1.83  1997/09/24  16:52:22  sxy
 * Made PrintDLL() available to DEBUGDEV.
 *
 * Revision 1.82  1997/09/17  13:05:57  sxy
 * removed a warning.
 *
 * Revision 1.81  1997/09/09  15:21:48  dm
 * Triton Chipset graphics Cards now read from the registry
 *
 * Revision 1.80  1997/09/04  13:47:37  gdc
 * got rid of compiler warnings
 *
 * Revision 1.79  1997/09/04  11:44:22  gdc
 * added stuff to detect Triton/fx chipsets in combination with matrox or
 * trident graphics controllers and turn of concurrency and turn on bursting
 * note: bursting flags sense for control register is 0 is ON
 *
 * Revision 1.78  1997/09/01  10:31:57  gdc
 * put in (untested on Triton/fx) stuff to tweak triton/fx
 *
 * Revision 1.77  1997/08/27  16:42:16  erf
 * Fixed PCI read and write off PCI configuration space.
 *
 * Revision 1.76  1997/08/13  08:44:11  mjg
 * Commented out some code that was only for debugging.
 *
 * Revision 1.75  1997/07/24  14:02:39  gdc
 * added a debug function to print dll name
 *
 * Revision 1.74  1997/07/21  16:20:16  dm
 * Debug message typo corrected
 *
 * Revision 1.73  1997/07/18  16:10:44  gdc
 * removed redundant code which filled in the device type as this
 * is now done in the Vxd
 *
 * Revision 1.72  1997/07/16  16:18:15  gdc
 * got rid of seemingly useless message
 *
 * Revision 1.71  1997/07/14  13:36:53  gdc
 * made debug output work for pvros and started turning PVROSPrintfs to DPFs
 *
 * Revision 1.70  1997/07/03  15:18:33  gdc
 * added call conv
 *
 * Revision 1.69  1997/07/03  15:17:45  gdc
 * added a stub for a new function
 *
 * Revision 1.68  1997/07/03  14:56:22  mjg
 * Removed a redundant message from (the seemingly redundant) PVROSGetInstance.
 *
 * Revision 1.67  1997/06/26  15:27:16  erf
 * Peachy creamy... The Hellbender issue has been fixed.
 * The problem was due to TlsIndex crap.
 *
 * Revision 1.66  1997/06/24  18:18:22  erf
 * 1. Made OutputLastErrorMessage () a static function.
 * 2. Simplified a condition to do with BogusPCX1.
 *
 * Revision 1.65  1997/06/23  12:22:15  jop
 * Moved Linear->Physical mapping to the VxD
 *
 * Revision 1.64  1997/06/17  20:32:04  mjg
 * Fixed VxD open/close
 *
 * Revision 1.63  1997/06/17  13:23:38  mjg
 * Checked in John's All American changes.
 *
 * Revision 1.62  1997/06/17  10:58:36  mjg
 * Changes for multi-context PCX1 (form USA)
 *
 * Revision 1.61  1997/06/17  09:39:25  mjg
 * Added the FORCE_NO_FPU and BogusPCX1 options.
 *
 * Revision 1.60  1997/06/11  12:35:13  jop
 * Removed some spaces (??)
 *
 * Revision 1.59  1997/06/09  12:13:06  mjg
 * Fixed previous stuff to handle killhelp.
 *
 * Revision 1.58  1997/06/09  10:19:17  mjg
 * Added DestroyRedundantContexts to clean up any old processes -
 * the problem seen when Actua Soccer failed to run a third time.
 *
 * Revision 1.57  1997/06/04  13:25:22  gdc
 * got rid of compiler warnings
 *
 * Revision 1.56  1997/06/02  02:42:37  jop
 * Dynamoc��ically allocated�s PSBs
 *
 * Revision 1.55  1997/05/22  23:29:07  jop
 * Enabled strip rendering and scheduler locking
 *
 * Revision 1.54  1997/05/21  14:10:11  gdc
 * changed the wait for render to complete and reset buffer data
 *
 * Revision 1.53  1997/05/20  08:55:45  jop
 * Removed init stuff from dllmain
 *
 * Revision 1.52  1997/05/19  15:35:51  gdc
 * put in some code to potentially allow pcx1 to use pcx2 buffers
 * but left the code disabled
 *
 * Revision 1.51  1997/05/19  14:53:39  jop
 * Disabled virtual virtual buffers
 *
 * Revision 1.50  1997/05/19  07:40:05  jop
 * Better error reporting (and hopefully fixed!) having a laugh
 * scenario
 *
 * Revision 1.49  1997/05/16  14:43:20  gdc
 * new version of mutex code for buffers
 *
 * Revision 1.48  1997/05/15  19:51:38  gdc
 * added mutex stuff for virtual buffers
 *
 * Revision 1.47  1997/05/15  15:09:35  jop
 * Fix for 'having a laugh' crash
 *
 * Revision 1.46  1997/05/15  13:16:10  gdc
 * added a path to copy a virtul virtual buffer for rendering from
 * virtual memory
 *
 * Revision 1.45  1997/05/15  11:16:35  gdc
 * added a wait loop of Render Pending so only one render is queued
 * per logical device
 *
 * Revision 1.44  1997/05/15  10:28:18  gdc
 * removed some ^M's
 *
 * Revision 1.43  1997/05/14  15:12:19  gdc
 * added a function to reinit static data when ref count reaches zero
 *
 * Revision 1.42  1997/05/14  14:19:09  gdc
 * removed heapman.h for micheal and added a destroy all virtual buffers
 * call when ref count reaches zero on process detach
 *
 * Revision 1.41  1997/05/14  12:47:02  gdc
 * changed size of buffers that are copied in a render and incremented
 * renderpending counter
 *
 * Revision 1.40  1997/05/14  00:25:52  jop
 * Tweaks for scheduler
 *
 * Revision 1.39  1997/05/13  20:54:38  jop
 * Updated structs in line with VXD changes
 * /.
 *
 * Revision 1.38  1997/05/12  11:59:38  erf
 * Changed pvrdmidX.dll to pvrmidX.dll etc...
 *
 * Revision 1.37  1997/05/09  19:34:13  mjg
 * Put Graham's changes for revision 1.35 back in as RCS seems to be
 * playing up again.
 *
 * Revision 1.36  1997/05/09  19:21:14  mjg
 * Reworked GetInterface and libraries.
 *
 * Revision 1.34  1997/05/08  11:30:29  erf
 * Added stuff to load PVRDirect libraries if called from D3D.
 *
 * Revision 1.33  1997/05/07  23:07:38  mjg
 * Commented out Heapman calls.
 *
 * Revision 1.32  1997/05/06  15:42:14  jop
 * Added reference count to instance data to get rid of exit
 * crashes
 *
 * Revision 1.31  1997/05/02  07:30:28  erf
 * Removed UINT32's etc and replaced with sgl_uint32's etc.
 *
 * Revision 1.30  1997/05/01  08:55:27  jop
 * Added pvrd FNBLOCK declaration
 *
 * Revision 1.29  1997/05/01  08:48:48  erf
 * Added PVRDirect API function interface definition, which I forgot the
 * last time.
 *
 * Revision 1.28  1997/05/01  08:25:50  erf
 * Added PVRDirect interface.
 *
 * Revision 1.27  1997/05/01  08:06:43  jop
 * Improved sharedness
 *
 * Revision 1.26  1997/05/01  02:44:25  jop
 * Added some stuff deleted some other stuff
 *
 * Revision 1.25  1997/04/28  17:15:58  mjg
 * Removed pvrosP.h include.
 *
 * Revision 1.24  1997/04/28  14:13:21  mjg
 * Put all SYSTEM_OBJECTS into the .onetime section.
 *
 * Revision 1.23  1997/04/27  22:57:08  jop
 * Fixed some instance nonsense
 *
 * Revision 1.22  1997/04/26  18:54:03  erf
 * Multi-context stuff.
 *
 * Revision 1.21  1997/04/26  14:15:38  erf
 * Added USE_HEAPMAN to get globally accessible heap
 *
 * Revision 1.20  1997/04/26  13:12:08  erf
 * Added shared data stuff
 *
 * Revision 1.19  1997/04/25  14:17:31  gdc
 * added private header file
 *
 * Revision 1.18  1997/04/25  14:13:45  erf
 * 1. Changed loop to get 9 PVRDirect function pointers.
 * 2. Added code to prevent multiple instances of libraries from being loaded.
 *
 * Revision 1.17  1997/04/24  15:10:57  gdc
 * added pvros to string emitted on dll attach
 *
 * Revision 1.16  1997/04/22  16:18:41  gdc
 * added functions to allow sgl.dll to hook up to sglmid?.dll
 *
 * Revision 1.15  1997/04/21  22:28:36  gdc
 * changes for separate pvros.dll
 *
 * Revision 1.14  1997/04/10  20:08:14  mjg
 * Added all-new inline assembly versions of the QueryPerformance funcs.
 *
 * Revision 1.13  1997/04/07  17:04:17  mjg
 * Performance counter wrappers.
 *
 * Revision 1.12  1997/04/07  15:26:56  gdc
 * got rid of some compiler warnings on WIN32 builds
 *
 * Revision 1.11  1997/04/03  19:10:09  mjg
 * Updated UINT32s to sgl_uint32*
 *
 * Revision 1.10  1997/04/03  13:10:17  mjg
 * Updated the calling convention (again).
 *
 * Revision 1.9  1997/04/02  21:25:54  mjg
 * Updated calling conventions.
 *
 * Revision 1.8  1997/04/02  11:55:53  mjg
 * Finished off PVROSAssert.
 *
 * Revision 1.7  1997/04/01  19:22:44  mjg
 * Added PVROSAssert.
 *
 * Revision 1.6  1997/03/31  22:04:18  jop
 * Got rid of a fatal error message that wasn't
 *
 * Revision 1.5  1997/03/29  22:07:39  mjg
 * Inserted PVROSDelay.
 *
 * Revision 1.4  1997/03/27  12:52:01  erf
 * Operating system independent memory allocation/deallocation macros.
 * 1. Changed malloc, calloc, realloc and free to PVROSMalloc, PVROSCalloc,
 *    PVROSRealloc and PVROSFree.
 *
 * Revision 1.3  1997/03/26  15:14:08  jop
 * Prioritised the Forget function so that system resources will
 * not be released from the wrong context, or more than once
 *
 * Revision 1.2  1997/03/25  11:48:24  jop
 * Reflected change in API
 *
 * Revision 1.1  1997/03/25  10:19:21  jop
 * Initial revision
 *
 *
 *;--
 *****************************************************************************/

#define MODULE_ID MODID_SYSTEM

#include "windows.h"

#define API_FNBLOCK
#include "sgl.h"
#undef API_FNBLOCK
#include "sgl_defs.h"
#include "pvrosapi.h"
#include "vsgl.h"
#include "pci.h"
#include "hwregs.h"
#include "debug.h"

#include "heap.h"

#define USE_VIRTUAL_BUFFERS 0
#if USE_VIRTUAL_BUFFERS
#pragma message ("Use of virtual buffers ENABLED!")
#else
#pragma message ("Use of virtual buffers DISABLED!")
#endif

#if DEBUGDEV || TIMING
#pragma data_seg(".onetime")
	#define NUMBER_OF_SHARED_BUFFER 20
	char 	szBuffer[NUMBER_OF_SHARED_BUFFER][256]=
		{"8","8","8","8","8","8","8","8","8","8","8","8","8","8","8","8","8","8","8","8"};
#pragma data_seg()
#endif

#ifdef __WATCOMC__

	
	extern void Int3 (void);
	#pragma aux Int3 = 0xCC;

#elif defined (_MSC_VER)

	#define Int3 DebugBreak

#else

	#define Int3 / ## /

#endif
	
typedef struct SYSTEM_OBJECT
{
	struct SYSTEM_OBJECT	*pNext;
	void					*pObject;
	
} SYSTEM_OBJECT, *PSO;

char* SGLLibNames[] = {"sglmid3.dll", "sglmid4.dll", "sglmid5.dll", "sglmid5a.dll"};
char* PVRDLibNames[] = {"pvrmid3.dll", "pvrmid4.dll", "pvrmid5.dll", "pvrmid5a.dll"};

HANDLE	hBufferMutex = NULL;
HANDLE 	hSystemVxD = NULL;
DWORD	hSysProcess = 0;

#if 0
typedef struct PRIVATE_SYSTEM_BLOCK
{
	struct PRIVATE_SYSTEM_BLOCK	*pNext;

	DWORD						hProcess;	/* current parent process's id */
	HINSTANCE					hInstance;
	HANDLE                      hBufferMutex;

	HANDLE						hVxD;

	LIB_DATA					SGLLibs[4];
	LIB_DATA					PVRDLibs[4];

	DWORD						dwReason;
	int							nRefCnt;
	
} PRIVATE_SYSTEM_BLOCK, *PPSB;

static PPSB pPSBs = NULL;
#endif

static SYSTEM_OBJECT  *gpPhysAlias = NULL;
static SYSTEM_OBJECT  *gpPhysBoard = NULL;
static SYSTEM_OBJECT  *gpSharedBlock = NULL;
static SYSTEM_OBJECT  *gpDMASB = NULL;

static sgl_bool gbNoGDI = TRUE; /* Default is for ASSERTs to go to the */
static sgl_bool gbODS	= TRUE;	/* WinICE output                       */
static sgl_bool gbInitialisedAsserts = FALSE;
static int 		gnRefCount = 0;

sgl_bool gBogusPCX1 = FALSE;

#define API_FNBLOCK
#include "pvrosapi.h"
#undef  API_FNBLOCK

#define API_INSTANTIATE
#include "pvrosapi.h"
#undef  API_INSTANTIATE

#define	API_FNBLOCK
#include "pvrd.h"
#undef	API_FNBLOCK

/************************** Local Prototypes ******************************/
PVROSERR CALL_CONV PVROSCreateSystemContext (HINSTANCE hInstance, DWORD dwReason);
/************************* End Local Prototypes ****************************/

/************************** Private Extern Prototypes **********************/
extern void PVROSDestroyAllVirtualBuffers(void);
/*********************** End Private Extern Prototypes **********************/


char *PVROSDbgDevBuffer(int k)
{
#if DEBUGDEV || TIMING	
	if((sgl_uint32)szBuffer & 0x80000000)
	{
		return szBuffer[k];
	}
	else
	{
		PVROSPrintf("Debug buffer unavailable\n");
		return NULL;
	}
#else
	return NULL;
#endif
}


static void OutputLastErrorMessage ()
{
	LPVOID lpMsgBuf;

	FormatMessage( 
	    FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
	    NULL,
	    GetLastError(),
	    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
	    (LPTSTR) &lpMsgBuf,
	    0,
	    NULL 
	);

	// Display the string.
	PVROSPrintf ("%s\n", lpMsgBuf);

	// Free the buffer.
	LocalFree( lpMsgBuf );
}

HANDLE OpenVxd(void)
{
	HANDLE hVxD;
	DWORD pid = GetCurrentProcessId();

	/* we check for 0 and then don't do anything --
	** I assume then zero is a non significant condition 
	** so I'll compile out this bit of code ...
	** ... by the way is 0 a valid win95 process ID ??
	*/
#if 0
	if (hSysProcess == 0)
	{
		PVROSPrintf ("hSysProcess is uninitialised\n");
	}
#endif

	if (pid == hSysProcess)
	{
		hVxD = hSystemVxD;
	}
	else
	{

		hVxD = CreateFile("\\\\.\\SGL", 
					  0, 
					  0, 
					  NULL, 
					  0, 
					  FILE_FLAG_DELETE_ON_CLOSE, 
					  NULL);
		
		if (hVxD == INVALID_HANDLE_VALUE)
		{
			DPF((DBG_ERROR,"Failed to open VxD"));

			OutputLastErrorMessage ();

			hVxD = NULL;
		}
	}

	return(hVxD);
}

void CloseVxd (HANDLE hVxD)
{
	if (GetCurrentProcessId() == hSysProcess)
	{
		return ;
	}
	else
	{
			CloseHandle (hVxD);
	}
}

void * CALL_CONV PVROSMalloc(sgl_uint32 X)
{
	return (MemAlloc (X));
}

void *CALL_CONV PVROSCalloc(sgl_uint32 X, sgl_uint32 Y)
{
	return (MemAlloc (X*Y));
}

void *CALL_CONV PVROSRealloc(void * X, sgl_uint32 Y)
{
	return (MemReAlloc (X, Y));
}

void CALL_CONV PVROSFree (void * X)
{
	MemFree (X);
}


/******************************************************************************
 * Function Name: PVROSTSPAlloc
 *
 * Inputs       : HANDLE hVxD, sgl_uint32 dwSize
 * Outputs      : -
 * Returns      : Linear memory address.
 * Globals Used : -
 *
 * Description  : Allocates memory from system memory. The memory is allocated
 *				  as locked (ie will always be physically present).
 *				  A 2Mb linear block is resevred but only dwSize is commited.
 *
 *****************************************************************************/
void *CALL_CONV PVROSTSPAlloc(HANDLE hVxD, sgl_uint32 dwSize)
{
	DWORD 	dwInData[2];
	DWORD 	dwOutData;
	DWORD	LinearAddress;

	DPF((DBG_MESSAGE,"PVROSTSPAlloc: Size to allocate %d.\n", dwSize));

	/* Always reserve up to 2Mb i.e. 500 pages.
	 */
	dwInData[0] = 500;

	/* Reserve linear address.
	 */
	if (DeviceIoControl (hVxD, VSGL_PAGE_RESERVE, 
						 dwInData, sizeof (DWORD),
						 &dwOutData, sizeof (dwOutData), 
						 NULL, NULL))
	{
		/* Copy address.
		 */
		LinearAddress = dwOutData;

		/* Setup the parameters.
		 */
		dwInData[0] = (dwOutData >> 12);

		/* Need to calculate the number of pages.
		 */
		dwInData[1] = (dwSize + 4095) >> 12;

		/* Commit linear addresses to physical addresses.
		 */
		if (DeviceIoControl (hVxD, VSGL_PAGE_COMMIT, 
							 dwInData, sizeof (dwInData), 
							 &dwOutData, sizeof (dwOutData), 
							 NULL, NULL))
		{
			DPF((DBG_MESSAGE,"PVROSTSPAlloc: Address allocated 0x%08x.\n", LinearAddress));

			return((void *)LinearAddress);

		}
		else
			return(NULL);
	}
	else
		return(NULL);
}

/******************************************************************************
 * Function Name: PVROSTSPFree
 *
 * Inputs       : HANDLE hVxD, void *Address
 * Outputs      : -
 * Returns      : Non zero if successful, zero otherwise.
 * Globals Used : -
 *
 * Description  : Frees resevred memory pointed to by a linear address.
 *
 *****************************************************************************/
DWORD CALL_CONV PVROSTSPFree (HANDLE hVxD, void *Address)
{
	DWORD	dwInData, dwOutData;

	dwInData = (DWORD) Address;

	DPF((DBG_MESSAGE,"PVROSTSPFree: Address to free 0x%08x.\n", Address));

	DeviceIoControl (hVxD, VSGL_PAGE_FREE,
						 &dwInData, sizeof (dwInData),
						 &dwOutData, sizeof (dwOutData),
						 NULL, NULL);
	/* Return result.
	 */
	return(dwOutData);
}

/******************************************************************************
 * Function Name: PVROSTSPRealloc
 *
 * Inputs       : HANDLE hVxD, void *Address, sgl_uint32 dwSize
 * Outputs      : -
 * Returns      : Linear memory address.
 * Globals Used : -
 *
 * Description  : Reallocates memory from an already reserved block of memory.
 *
 *****************************************************************************/
void *CALL_CONV PVROSTSPRealloc(HANDLE hVxD, void *Address, sgl_uint32 dwSize)
{
	DWORD 	dwInData[2];
	DWORD 	dwOutData;

	/* Setup the parameters.
	 */
	dwInData[0] = (DWORD) Address;

	/* Need to calculate the number of pages.
	 */
	dwInData[1] = (dwSize + 4095) >> 12;

	/* Commit linear addresses to physical addresses.
	 */
	if (DeviceIoControl (hVxD, VSGL_PAGE_REALLOCATE, 
						 dwInData, sizeof (dwInData), 
						 &dwOutData, sizeof (dwOutData), 
						 NULL, NULL))
	{
		DPF((DBG_MESSAGE,"PVROSTSPRealloc: Address allocated 0x%08x.\n", dwOutData));

		return((void *)dwOutData);

	}
	else
		return(NULL);
}






HANDLE PVROSGetInstance ()
{
	/* PPSB pPSB = GetInstanceData (TRUE); 
	 * ASSERT (pPSB); 	 
	 */
	 
	return NULL; /* pPSB->hInstance; */
}
extern sgl_bool GetVirtualBufferMutex(HANDLE hMutex)
{
	sgl_uint32 dwWaitResult; 
	sgl_uint32 TryAgain = 10;

	while(TryAgain)
	{
		/* Request ownership of mutex, five-second time-out interval */
		dwWaitResult = WaitForSingleObject(hMutex,100L);

		switch (dwWaitResult) 
		{
			/* The thread got mutex ownership.*/
			case WAIT_OBJECT_0: 
			{
				return TRUE; 
			}
			/* Cannot get mutex ownership due to time-out.*/
			case WAIT_ABANDONED: 
			case WAIT_TIMEOUT: 
				/* Got ownership of the abandoned mutex object.*/
			default:
			{
				break; 
			}
		}

		TryAgain--;
	}

	PVROSPrintf("Failed to get Mutex\n");

    return FALSE; 
}

extern void ReleaseVirtualBufferMutex(HANDLE hMutex)
{
	/* Release ownership of the mutex object.*/
	if (! ReleaseMutex(hMutex)) 
	{ 
		PVROSPrintf("Error : Failed to release Mutex\n");
	} 
}

/******************************************************************************
 * Function Name: Remember
 *
 * Globals Used : -
 *
 * Description  : place a reference to a system resource in specified list
 *****************************************************************************/
static sgl_uint32 Remember (PSO *pPSO, void *pObject)
{
	PSO pNew = PVROSMalloc (sizeof (SYSTEM_OBJECT));
	PSO pThis = *pPSO;
	
	if (pNew)
	{
		pNew->pNext = NULL;
		pNew->pObject = pObject;
		
		if (pThis)
		{
			while (pThis->pNext)
			{
				pThis = pThis->pNext;
			}
			
			pThis->pNext = pNew;			
		}
		else
		{
			*pPSO = pNew;
		}
	}
	else
	{
		DPF ((DBG_ERROR, "Unable to allocate resource reference"));

		return (0);
	}

	return (1);
}	

/******************************************************************************
 * Function Name: Forget
 *
 * Globals Used : -
 *
 * Description  : Delete a reference to a system resource from specified list
 *****************************************************************************/
static sgl_uint32 Forget (PSO *pPSO, void *pObject)
{
	PSO pThis = *pPSO;
	PSO pLast = NULL;
	
	while (pThis)
	{
		if (pThis->pObject == pObject)
		{
			if (pLast == NULL)
			{
				*pPSO = pThis->pNext;
			}
			else
			{
				pLast->pNext = pThis->pNext;
			}
			
			PVROSFree (pThis);
			
			return (1);
		}
		
		pLast = pThis;
		pThis = pThis->pNext;		
	}

	DPF ((DBG_WARNING, "Unable to find system resource reference in this context"));
	
	return (0);
}	

/******************************************************************************
 * Function Name: AllocatePhysicalAlias
 *
 * Inputs       : SHAREDBLOCK *blk: .PhysicalAddress and .Pages contain area
 *				  to allocate
 * Outputs      : SHAREDBLOCK *blk: Ring0Linear and GlobalAlias 
 * Returns      : TRUE if success, FALSE if failure
 * Globals Used : -
 *
 * Description  : Maps physical addresses into linear address space
 *****************************************************************************/
static int AllocatePhysicalAlias (HANDLE hVxD, SHAREDBLOCK *blk)
{
	DWORD dwBytesReturned;

	ASSERT (hVxD);

	return (DeviceIoControl (hVxD, VSGL_ALLOCPHYSALIAS, blk, sizeof (SHAREDBLOCK), blk, sizeof (SHAREDBLOCK), &dwBytesReturned, NULL));
}

/******************************************************************************
 * Function Name: FreePhysicalAlias
 *
 * Inputs       : SHAREDBLOCK *blk: block to free
 * Outputs      : -
 * Returns      : TRUE if success, FALSE if failure
 * Globals Used : -
 *
 * Description  : frees a physical alias (doesn't do anything actually!)
 *****************************************************************************/
static int FreePhysicalAlias (HANDLE hVxD, SHAREDBLOCK *blk)
{
	DWORD dwBytesReturned;

	ASSERT (hVxD);

	return (DeviceIoControl (hVxD, VSGL_FREEPHYSALIAS, blk, sizeof (SHAREDBLOCK), NULL, 0, &dwBytesReturned, NULL));
}

/******************************************************************************
 * Function Name: AllocateSharedBlock
 *
 * Inputs       : DWORD ParamPages: pages to allocate
 * Outputs      : SHAREDBLOCK *blk: Allocated block
 * Returns      : TRUE if success, FALSE if failure
 * Globals Used : -
 *
 * Description  : Allocates a contiguous sharable phys. mem. block
 *****************************************************************************/
static int AllocateSharedBlock (HANDLE hVxD, DWORD ParamPages, SHAREDBLOCK *blk)
{
	int nRes;
	DWORD dwBytesReturned;
	
	ASSERT (hVxD);

	nRes = DeviceIoControl (hVxD, VSGL_ALLOCATEBLOCK, &ParamPages, sizeof (DWORD), blk, sizeof (SHAREDBLOCK), &dwBytesReturned, NULL);

	if (nRes)
	{
		ASSERT (dwBytesReturned == sizeof (SHAREDBLOCK))
	}

	return (nRes);
}

/******************************************************************************
 * Function Name: FreeSharedBlock
 *
 * Inputs       : SHAREDBLOCK *blk: block to free
 * Outputs      : -
 * Returns      : TRUE if success, FALSE if failure
 * Globals Used : -
 *
 * Description  : frees a block allocated by AllocateSharedBlock
 *****************************************************************************/
static int FreeSharedBlock (HANDLE hVxD, SHAREDBLOCK *blk)
{
	DWORD dwBytesReturned;
	
	ASSERT (hVxD);

	return (DeviceIoControl (hVxD, VSGL_FREEBLOCK, blk, sizeof (SHAREDBLOCK), NULL, 0, &dwBytesReturned, NULL));
}

/******************************************************************************
 * Function Name: MapLinearToPhysical
 *
 * Inputs		: 32 bit linear address
 * Outputs		: 32 bit physical address
 * Returns		: TRUE if ok
 * Globals Used	: hVxD
 *
 * Description  : Converts linear address to physical address
 *****************************************************************************/
static int MapLinearToPhysical(HANDLE hVxD, sgl_uint32 dwLinear, sgl_uint32* pPhysical)
{
	sgl_uint32 dwIoBuffer;
	sgl_uint32 dwBytesReturned;
	sgl_uint16 wRet;

	dwIoBuffer = dwLinear;

	ASSERT (hVxD);

	wRet = (sgl_uint16) DeviceIoControl (hVxD, VSGL_GETPHYSADDRESS, &dwIoBuffer, sizeof (dwIoBuffer),
						&dwIoBuffer, sizeof (dwIoBuffer), &dwBytesReturned, NULL);
	if (!wRet)
	{
		return FALSE;
	}

	if (dwIoBuffer == 0)
	{
		return FALSE;
	}

	*pPhysical = dwIoBuffer;
	return TRUE;

}/*MapLinearToPhysical*/

/******************************************************************************
 * Function Name: EnumeratePhysicalBoards
 *
 * Inputs       : -
 * Outputs      : -
 * Returns      : -
 * Globals Used : -
 *
 * Description  : 
 *****************************************************************************/
static sgl_uint32 EnumeratePhysicalBoards (HANDLE hVxD, void *pBuff, sgl_uint32 dwBuffSize)
{
	ASSERT (hVxD);

	return (DeviceIoControl (hVxD, VSGL_ENUM_PHYSICAL_BOARDS, 
											NULL, 0, 
											pBuff, dwBuffSize, 
											NULL, NULL));
}
/******************************************************************************
 * Function Name: GetPhysicalBoard
 *
 * Inputs       : -
 * Outputs      : -
 * Returns      : -
 * Globals Used : -
 *
 * Description  : clean up at end of session
 *****************************************************************************/
static sgl_uint32 GetPhysicalBoard (HANDLE hVxD, DWORD ID, DWORD *pdwPhysBoardID)
{
	ASSERT (hVxD);

	return (DeviceIoControl (hVxD, VSGL_GET_PHYSICAL_BOARD, 
										&ID, sizeof (ID), 
										pdwPhysBoardID, sizeof (*pdwPhysBoardID), 
										NULL, NULL));
}

/******************************************************************************
 * Function Name: GetPhysicalBoard
 *
 * Inputs       : -
 * Outputs      : -
 * Returns      : -
 * Globals Used : -
 *
 * Description  : clean up at end of session
 *****************************************************************************/
static sgl_uint32 GetPhysicalBoardInfo (HANDLE hVxD, DWORD *pPhysBoardID, BoardDataBlock *pb)
{
	ASSERT (hVxD);
	
	return (DeviceIoControl (hVxD, VSGL_GET_PHYSICAL_BOARD_INFO, 
										pPhysBoardID, sizeof (*pPhysBoardID), 
										pb, sizeof (*pb), 
										NULL, NULL));
}

/******************************************************************************
 * Function Name: ReleasePhysicalBoard
 *
 * Inputs       : -
 * Outputs      : -
 * Returns      : -
 * Globals Used : -
 *
 * Description  : clean up at end of session
 *****************************************************************************/
static sgl_uint32 ReleasePhysicalBoard (HANDLE hVxD, DWORD *pPhysBoardID)
{
	ASSERT (hVxD);
	
	return (DeviceIoControl (hVxD, VSGL_RELEASE_PHYSICAL_BOARD, 
										pPhysBoardID, sizeof (*pPhysBoardID), 
										NULL, 0, 
										NULL, NULL));
}

/******************************************************************************
 * Function Name: EnablePhysicalBoard
 *
 * Inputs       : -
 * Outputs      : -
 * Returns      : -
 * Globals Used : -
 *
 * Description  : clean up at end of session
 *****************************************************************************/
static sgl_uint32 EnablePhysicalBoard (HANDLE hVxD, DWORD *pPhysBoardID)
{
	ASSERT (hVxD);
	
	return (DeviceIoControl (hVxD, VSGL_ENABLE_PHYSICAL_BOARD, 
									pPhysBoardID, sizeof (*pPhysBoardID), 
									NULL, 0, 
									NULL, NULL));
}

/******************************************************************************
 * Function Name: DisablePhysicalBoard
 *
 * Inputs       : -
 * Outputs      : -
 * Returns      : -
 * Globals Used : -
 *
 * Description  : clean up at end of session
 *****************************************************************************/
static sgl_uint32 DisablePhysicalBoard (HANDLE hVxD, DWORD *pPhysBoardID)
{
	ASSERT (hVxD);
	
	return (DeviceIoControl (hVxD, VSGL_DISABLE_PHYSICAL_BOARD, 
									pPhysBoardID, sizeof (*pPhysBoardID), 
									NULL, 0, 
									NULL, NULL));
}

/******************************************************************************
 * Function Name: CreateDMAScatterBuffer
 *
 * Inputs       : -
 * Outputs      : -
 * Returns      : -
 * Globals Used : -
 *
 * Description  : clean up at end of session
 *****************************************************************************/
static sgl_bool CreateDMAScatterBuffer (HANDLE hVxD, DMASCATTERBUFFER *pDMASB, DWORD dwSize)
{
	ASSERT (hVxD);
	ASSERT (pDMASB);
	ASSERT (dwSize >> 12);
													     
	return (DeviceIoControl (hVxD, VSGL_CREATE_DMA_SCATTER_BUFFER, 
									&dwSize, sizeof (dwSize), 
									pDMASB, sizeof (DMASCATTERBUFFER), 
									NULL, NULL));
}

/******************************************************************************
 * Function Name: DestroyDMAScatterBuffer
 *
 * Inputs       : -
 * Outputs      : -
 * Returns      : -
 * Globals Used : -
 *
 * Description  : clean up at end of session
 *****************************************************************************/
static int DestroyDMAScatterBuffer (HANDLE hVxD, DMASCATTERBUFFER *pDMASB)
{
	ASSERT (hVxD);
	ASSERT (pDMASB);
	
	return (DeviceIoControl (hVxD, VSGL_DESTROY_DMA_SCATTER_BUFFER, 
									pDMASB, sizeof (DMASCATTERBUFFER), 
									NULL, 0, 
									NULL, NULL));
}

/******************************************************************************
 * Function Name: AllocVirtualPages
 *
 * Inputs       : -
 * Outputs      : -
 * Returns      : -
 * Globals Used : -
 *
 * Description  : Allocates some virtual memory
 *****************************************************************************/
static void *AllocVirtualPages (HANDLE hVxD, sgl_uint32 dwPages)
{
	void *pNew;

	ASSERT (hVxD);
	ASSERT (dwPages);
	
	if (DeviceIoControl (hVxD, VSGL_ALLOC_VIRTUAL_PAGES, 
									&dwPages, sizeof (dwPages), 
									&pNew, sizeof (void *), 
									NULL, NULL))
	{
		return (pNew);
	}
	else
	{
		return (NULL);
	}
}

/******************************************************************************
 * Function Name: FreeVirtualPages
 *
 * Inputs       : -
 * Outputs      : -
 * Returns      : -
 * Globals Used : -
 *
 * Description  : frees some virtual memory allocated by AllocVirtualPages
 *****************************************************************************/
static void FreeVirtualPages (HANDLE hVxD, void *pPtr)
{
	ASSERT (hVxD);
	
	DeviceIoControl (hVxD, VSGL_FREE_VIRTUAL_PAGES, 
									(LPVOID) &pPtr, sizeof (void *), 
									NULL, 0, 
									NULL, NULL);
}

DMABUFFER * CALL_CONV PVROSCreateDMABuffer (HANDLE hVxD,sgl_uint32 Bytes)
{
	DMABUFFER	*pDMAB	= PVROSMalloc (sizeof (DMABUFFER));
	
	if (pDMAB)
	{
		if (AllocateSharedBlock (hVxD, Bytes >> 12, (SHAREDBLOCK *) pDMAB))
		{
			if (Remember (&gpSharedBlock, pDMAB))
			{
				return (pDMAB);
			}

			PVROSPrintf("PVROSCreateDMABuffer: Couldn't bung block in list\n");
		}
		
		PVROSFree (pDMAB);
	}
	
	return (NULL);	
}	

PVROSERR CALL_CONV PVROSDestroyDMABuffer (HANDLE hVxD,DMABUFFER *pDMAB)
{
	ASSERT (pDMAB);
	
	if (!Forget (&gpSharedBlock, pDMAB))
	{
		PVROSPrintf("Attempt to destroy a DMA buffer more than once\n");
	}
	else
	{
		if (FreeSharedBlock (hVxD, (SHAREDBLOCK *) pDMAB))
		{
			PVROSFree (pDMAB);	
			
			return (PVROS_GROOVY);		
		}
	}

	return (PVROS_DODGY);
}	

void *CALL_CONV PVROSMapPhysicalToLinear (sgl_uint32 dwPhysical, sgl_uint32 Bytes)
{
	HANDLE hVxD = OpenVxd();
	DMABUFFER 	DMAB;

	DMAB.PhysicalAddress = dwPhysical;
	DMAB.Pages = Bytes >> 12;
	
	if (AllocatePhysicalAlias (hVxD, (SHAREDBLOCK *) &DMAB))
	{
		if (Remember (&gpPhysAlias, (void *) dwPhysical))
		{
			CloseVxd (hVxD);

			return ((void *)DMAB.GlobalAlias);
		}
	}
	
	CloseVxd (hVxD);

	return (NULL);	
}	

sgl_uint32 CALL_CONV PVROSMapLinearToPhysical (sgl_uint32 dwLinear)
{
#if 1

	#pragma message ("PVROSMapLinearToPhysical is not doing anything! (it's now done in the VxD)")
	
	return (dwLinear);

#else

	#define MAP_CACHE_SIZE	16

	typedef struct CACHE_LINE
	{
		sgl_uint32	dwPhysical;
		sgl_uint32	dwLinear;
	} CACHE_LINE;
	
	static CACHE_LINE MapCache[MAP_CACHE_SIZE];

	static int nValid = 0;
	int k = 0;

	HANDLE hVxD;
	sgl_uint32	dwPhysical;

	while (k < nValid)
	{
		if (MapCache[k].dwLinear == dwLinear)
		{
			if (k != 0)
			{
				CACHE_LINE t;

				t = MapCache[k];
				MapCache[k] = MapCache[0];
				MapCache[0] = t;
			}

			return (MapCache[0].dwPhysical);
		}

		++k;
	}

	hVxD = OpenVxd();

	if (MapLinearToPhysical (hVxD, dwLinear, &dwPhysical))
	{
		CloseVxd (hVxD);

		if (nValid == MAP_CACHE_SIZE)
		{
			memcpy (MapCache + 1, MapCache, sizeof (CACHE_LINE)*(MAP_CACHE_SIZE-1));
		}
		else
		{
			memcpy (MapCache + 1, MapCache, sizeof (CACHE_LINE)*nValid);
			++nValid;
		}

		PVROSPrintf ("New physical address mapping %d: 0x%08lx->0x%08lx\n", nValid, dwPhysical, dwLinear);

		MapCache[0].dwPhysical = dwPhysical;
		MapCache[0].dwLinear = dwLinear;

		return (dwPhysical);
	}
	
	CloseVxd (hVxD);

	return (0);	

#endif
}	

DMASCATTERBUFFER * CALL_CONV PVROSCreateDMAScatterBuffer (HANDLE hVxD,sgl_uint32 Bytes)
{
	DMASCATTERBUFFER *pDMASB = PVROSMalloc (sizeof (DMASCATTERBUFFER));

	if (pDMASB)
	{
		if (CreateDMAScatterBuffer (hVxD, (DMASCATTERBUFFER *) pDMASB, Bytes))
		{
			if (Remember (&gpDMASB, pDMASB))
			{
				return (pDMASB);
			}
		}
		
		PVROSFree (pDMASB);
	}
	
	return (NULL);	
}	

void * CALL_CONV PVROSAllocSharedMemory (sgl_uint32 Bytes)
{
	HANDLE hVxD = OpenVxd();
	void *pNew;
	
	pNew = AllocVirtualPages (hVxD, Bytes >> 12);

	if (pNew)
	{
		#if 0
		if (Remember (&gpSharedMem, pNew))
		{
			CloseVxd (hVxD);

			return (pNew);
		}
		#else
			CloseVxd (hVxD);

			return (pNew);
		#endif
	}
	
	CloseVxd (hVxD);
	return (NULL);	
}	

void CALL_CONV PVROSFreeSharedMemory (void *pPtr)
{
	HANDLE hVxD = OpenVxd();

	ASSERT (pPtr);
	
	#if 0
	if (!Forget (&gpSharedMem, pPtr))
	{
		PVROSPrintf("Attempt to destroy a peice of shared memory more than once\n");
	}
	else
	{
		FreeVirtualPages (hVxD, pPtr);
	}
	#else
		FreeVirtualPages (hVxD, pPtr);
	#endif

	CloseVxd (hVxD);
}	

PVROSERR CALL_CONV PVROSDestroyDMAScatterBuffer (HANDLE hVxD,DMASCATTERBUFFER *pDMASB)
{
	ASSERT (pDMASB);
	
	if (!Forget (&gpDMASB, pDMASB))
	{
		PVROSPrintf("Attempt to destroy a DMA scatter buffer more than once\n");
	}
	else
	{
		if (DestroyDMAScatterBuffer (hVxD, (DMASCATTERBUFFER *) pDMASB))
		{
			PVROSFree (pDMASB);	
			
			return (PVROS_GROOVY);		
		}
	}
	
	return (PVROS_DODGY);
}	


PVROSERR CALL_CONV PVROSReadPCIConfigSpace(HDEVICE hDevice, sgl_uint32 dwRegNo, sgl_uint32 *dwRegValue)
{
	int nPCIErr;
	BoardDataBlock	pb;
	
	PVROSGetPCIDeviceInfo (hDevice, &pb);

	nPCIErr = PCIRead32 ( (sgl_uint8) pb.PCIBus, 
			 (sgl_uint8)((pb.PCIDev << 3) | pb.PCIFunc), (sgl_uint16) dwRegNo, dwRegValue);

	return(nPCIErr);
}

PVROSERR CALL_CONV PVROSWritePCIConfigSpace(HDEVICE hDevice, sgl_uint32 dwRegNo, sgl_uint32 dwRegValue)
{
	int nPCIErr;
	BoardDataBlock	pb;

	PVROSGetPCIDeviceInfo (hDevice, &pb);

	nPCIErr = PCIWrite32 ( (sgl_uint8) pb.PCIBus, 
			(sgl_uint8) ((pb.PCIDev << 3) |  pb.PCIFunc), (sgl_uint16) dwRegNo, dwRegValue);

	return(nPCIErr);
}

/******************************************************************************
 * Function Name: SetCacheMode
 *
 * Inputs       : DMABUFFER *blk: to set page level caching on (!?)
 * Outputs      : -
 * Returns      : TRUE if success, FALSE if failure
 * Globals Used : -
 *
 * Description  : Set cache mode on a shared block/physical alias
 *
 * Note by NCJ  : mode parameter contains page level cache attribs :
 *
 * mode = page attribute, bit 3 is page level write-through (PWT)
 *                        bit 4 is page level cache disable (PCD)
 *
 * Cache disable is therefore 0x10 for P5 or P6
 * 
 * For P6 processsor, Write Combining requires mode = 0x00 AND the MTRR registers
 * set to give Memory Type 0x01 AND MTRR enable bit = 1 in MTRRdefType register.
 *
 *****************************************************************************/
PVROSERR CALL_CONV PVROSSetCacheMode (DMABUFFER *blk, DWORD mode)
{
	HANDLE hVxD = OpenVxd();

#if PCX2 || PCX2_003
	DWORD 		Data[3];

	/* Only needed for PCX2 to do write combining.
	 */

	if (mode == 0)
	{
		DPF ((DBG_WARNING, "Write combining not supported through this API"));
		return (TRUE);
	}
	
	Data[0] = blk->GlobalAlias;
	Data[1] = blk->Pages;
	Data[2] = mode;
		
	CloseVxd (hVxD);
	return (DeviceIoControl (hVxD, VSGL_SETCACHEMODE, Data, 12, NULL, 0, NULL, NULL));
#else
	CloseVxd (hVxD);
	return(TRUE);
#endif
}

/******************************************************************************
 * Function Name: PVROSCopyBufferToHardware
 *
 * Inputs       : HSYSTEM hSystem, void *Src, void *Dst, sgl_uint32 Bytes, 
 				  sgl_uint32 CheckCopy
 * Outputs      : void
 * Returns      : PVROS_GROOVY if okay, PVROS_DODGY if not.		
 * Globals Used : -
 *
 * Description  : Copies a buffer over to a hardware buffer
 *****************************************************************************/

PVROSERR CALL_CONV PVROSCopyBufferToHardware (void *Src, void *Dst, sgl_uint32 Bytes, sgl_uint32 CheckCopy)
{
	DWORD Data[4];
	HANDLE hVxD = OpenVxd();
	
/*	ASSERT (((DWORD) Src & 0x00000FFFL) == 0);*/

	Data[0] = (DWORD) Src;
	Data[1] = (DWORD) Dst;
	Data[2] = (DWORD) Bytes;
	Data[3] = (DWORD) CheckCopy;
	
	DeviceIoControl (hVxD, VSGL_COPYBUFFER, Data, 16, NULL, 0, NULL, NULL);

	CloseVxd (hVxD);
	return (PVROS_GROOVY);		
}

/**********************************************************************/

sgl_uint32 CALL_CONV PVROSEnumPCIDevices (PciDeviceTable *pPDT)
{
	DWORD dwDevices;
	HANDLE hVxD = OpenVxd();
	
	if (EnumeratePhysicalBoards (hVxD, &dwDevices, sizeof (dwDevices)))
	{
		if (pPDT == NULL)
		{
			CloseVxd (hVxD);
			return (dwDevices);
		}
		else
		{
			if (EnumeratePhysicalBoards (hVxD, pPDT, dwDevices * sizeof (PciDeviceTable)))
			{
				CloseVxd (hVxD);
				return (dwDevices);
			}
		}
	}
	
	CloseVxd (hVxD);
	return (0);
}	

/**********************************************************************/

void GetLibraryModule(HLDEVICE hLDevice,PVR_Interface Spec)
{
	
	char**		pszLibName;

	hLDevice->hLib = NULL;

	switch(Spec)
	{
		case PVR_SGL:
		case PVR_SGLLite:
		{
			pszLibName = SGLLibNames;

			break;
		}
		case PVR_PVRDirect:
		{
			pszLibName = PVRDLibNames;

			break;
		}
		case PVR_Texture:
		default:
		{
			return;
		}
	}

	switch(hLDevice->DeviceType)
	{
		case MIDAS3:
		{
			if (hLDevice->hLib == NULL)
			{
				hLDevice->hLib = LoadLibrary(pszLibName[0]);
			}
			break;
		}
		case MIDAS4:
		{
			if (hLDevice->hLib == NULL)
			{
				hLDevice->hLib = LoadLibrary(pszLibName[1]);
			}
			break;
		}
		case MIDAS5:
		{
			if (hLDevice->hLib == NULL)
			{
				hLDevice->hLib = LoadLibrary(pszLibName[2]);
			}
			break;
		}
		case MIDAS5_003:
		{
			if (hLDevice->hLib == NULL)
			{
				hLDevice->hLib = LoadLibrary(pszLibName[3]);
			}
			break;
		}
	}

	return;
}

/**********************************************************************/

sgl_uint32 CALL_CONV PVROSGetInterface(HLDEVICE			hLDevice,
									   PVR_Interface 	Spec, 
									   void 			**ppIF)
{
	HINSTANCE 	hLib = NULL;
	sgl_uint32	nStructSize = 0;


	GetLibraryModule(hLDevice,Spec);

	if (!hLDevice->hLib)
	{
		DPF((DBG_WARNING,"PVROSGetInterface: Library not found."));
		return (0);
	}

	hLib = hLDevice->hLib;

	/* 
		get the number of functions 
	*/
	switch(Spec)
	{
		case(PVR_SGL):
		case(PVR_SGLLite):
		{
			API_GETFNTYPE(SGL) pfnGetAPI = (void *) GetProcAddress (hLib, "GetSGL_IF");

			if (!pfnGetAPI)
			{
				DPF ((DBG_ERROR, "Could not get SGL API"));
			}
			else 
			{
				if (pfnGetAPI ((SGL_IF **)ppIF) != sizeof (API_STRUCTTYPE(SGL)))
				{
					DPF ((DBG_ERROR, "SGL API was the wrong size"));
				}
				else
				{
					return sizeof (API_STRUCTTYPE(SGL));
				}
			}

			break;
		}
		case(PVR_PVRDirect):
		{
			API_GETFNTYPE(PVRDIRECT) pfnGetAPI = (void *) GetProcAddress (hLib, "GetPVRDIRECT_IF");

			if (!pfnGetAPI)
			{
				DPF ((DBG_ERROR, "Could not get PVRDirect API"));
			}
			else 
			{
				if (pfnGetAPI ((PVRDIRECT_IF **)ppIF) != sizeof (API_STRUCTTYPE(PVRDIRECT)))
				{
					DPF ((DBG_ERROR, "PVRDirect API was the wrong size"));
				}
				else
				{
					return sizeof (API_STRUCTTYPE(PVRDIRECT));
				}
			}

			break;
		}
		default:
		{
			break;
		}
	}

	FreeLibrary (hLib);
	
	/*
		 return the number of functions....NOT
	*/
	return 0;
}

/**********************************************************************/

sgl_uint32 CALL_CONV PVROSReleaseInterface(HLDEVICE hLDevice)
{
	FreeLibrary (hLDevice->hLib);

	return(0);
}


/**********************************************************************/

HDEVICE CALL_CONV PVROSOpenPCIDevice (DEVICEID DeviceID)
{
	DWORD	*pPhysBoardID = PVROSMalloc (sizeof (DWORD));
	HANDLE hVxD = OpenVxd();
	
	if (pPhysBoardID)
	{
		if (GetPhysicalBoard (hVxD, (sgl_uint32) DeviceID, pPhysBoardID))
		{
			if (Remember (&gpPhysBoard, pPhysBoardID))
			{
				CloseVxd (hVxD);
				return (pPhysBoardID);
			}
		}
		
		PVROSFree (pPhysBoardID);
	}
	
	CloseVxd (hVxD);
	return (NULL);	
}	

/**********************************************************************/

PVROSERR CALL_CONV PVROSGetPCIDeviceInfo (HDEVICE hDevice, BoardDataBlock *pBDB)
{
	DWORD 	*pPhysBoardID = hDevice;
	HANDLE hVxD = OpenVxd();
	
	ASSERT (pPhysBoardID);
	
	if (GetPhysicalBoardInfo (hVxD, pPhysBoardID, pBDB))
	{
		CloseVxd (hVxD);
		return (PVROS_GROOVY);
	}

	CloseVxd (hVxD);
	return (PVROS_DODGY);
}	

/**********************************************************************/

PVROSERR CALL_CONV PVROSClosePCIDevice (HDEVICE hDevice)
{
	DWORD 	*pPhysBoardID = hDevice;
	HANDLE hVxD = OpenVxd();
	
	ASSERT (pPhysBoardID);
	
	if (!Forget (&gpPhysBoard, pPhysBoardID))
	{
		PVROSPrintf("Attempt to destroy a physical board more than once\n");
	}
	else
	{
		if (ReleasePhysicalBoard (hVxD, pPhysBoardID))
		{
			PVROSFree (pPhysBoardID);	
			
			CloseVxd (hVxD);
			return (PVROS_GROOVY);		
		}
	}
	
	CloseVxd (hVxD);
	return (PVROS_DODGY);
}	

/**********************************************************************/

/* this function sets up all board specific interrupt handlers */
PVROSERR CALL_CONV PVROSSetupPowerVRDevice (HDEVICE hDevice, DEVICE_TYPE BoardType)
{
	DWORD 	*pPhysBoardID = hDevice;
	HANDLE hVxD = OpenVxd();
	
	ASSERT (pPhysBoardID);
	
	if (BoardType == DISABLE_BOARD)
	{
		if (DisablePhysicalBoard (hVxD, pPhysBoardID))
		{
			CloseVxd (hVxD);
			return (PVROS_GROOVY);
		}
	}
	else
	{
		if (EnablePhysicalBoard (hVxD, pPhysBoardID))
		{
			CloseVxd (hVxD);
			return (PVROS_GROOVY);
		}
	}
	
	CloseVxd (hVxD);
	return (PVROS_DODGY);
}	

/**********************************************************************/

PVROSERR CALL_CONV PVROSCreateSystemContext (HINSTANCE hInstance, DWORD dwReason)
{
	hSysProcess = GetCurrentProcessId();

	ASSERT (sizeof (DMABUFFER) == sizeof (SHAREDBLOCK));
		
	if (hSystemVxD == NULL)
	{
		hSystemVxD = CreateFile ("\\\\.\\SGL", 0, 0, NULL, 0, FILE_FLAG_DELETE_ON_CLOSE, NULL);
		
		if (hSystemVxD == INVALID_HANDLE_VALUE)
		{
			DPF ((DBG_FATAL, "PVROSCreateSystemContext: Unable to load VxD"));
		}
		else
		{
			unsigned char VersionID[4];
			DWORD Dross;
			
#if DEBUG			
			DebugInit (DBGPRIV_WARNING);
#endif

#if DEBUGDEV
		   	DebugInit(DBGPRIV_MESSAGE);
#endif

			hBufferMutex = CreateMutex(NULL,     /* no security attributes*/
									   FALSE,    /* initially not owned*/
									   "VirtualBufferMutex");  /* name of mutex*/

			if (hBufferMutex == NULL) 
			{
				PVROSPrintf("Bad news\n");
			}
			
#if 0
			pPSB->hInstance = hInstance;
			pPSB->dwReason = dwReason;
#endif
			/* get and print version number */
			DeviceIoControl (hSystemVxD, 1, NULL, 0, VersionID, sizeof (VersionID), &Dross, NULL);
		
			#if DEBUG
			
				PVROSPrintf("VxD version: %d.%d\n", (int) VersionID[1], (int) VersionID[0]);

			#endif

			if (VersionID[1] >= 4)
			{
				DPF((DBG_MESSAGE,"PVROSCreateSystemContext: Process ID: %x.\n", hSysProcess));

				return (PVROS_GROOVY);
			}
			else
			{
				PVROSPrintf("Obsolete VxD! Version: %d.%d\n", (int) VersionID[1], (int) VersionID[0]);
			}
		}
	}
	else
	{
		char szParentProcess[256];
		
		GetModuleFileName (NULL, szParentProcess, sizeof (szParentProcess));
		
		if (dwReason == DLL_PROCESS_ATTACH)
		{
			PVROSPrintf("AARGH! process attach in another process's context (%s)\n", szParentProcess);
		}
		else
		{
			PVROSPrintf("Thread attach in existing process's context (%s)\n", szParentProcess);
		}
	
		return (PVROS_GROOVY); 
	}
			
	return (PVROS_DODGY);
}	

PVROSERR CALL_CONV PVROSDestroySystemContextLocal (void)
{
	PSO 	pSO, pSONext;
	HANDLE	hVxD = OpenVxd();
	
	if (gnRefCount == 0)
	{
		PVROSPrintf("PVROSDestroySystemContext: Shutdown\n");

		PVROSDestroyAllVirtualBuffers();

		/* release leaked physical aliases */
	
		pSO = gpPhysAlias;

		while (pSO)
		{
			ASSERT (pSO->pObject);

			PVROSPrintf("PVROSDestroySystemContext: Shutdown: releasing physical alias\n");

			FreePhysicalAlias (hVxD, pSO->pObject);

			pSONext = pSO->pNext;
			PVROSFree (pSO->pObject);
			PVROSFree (pSO);
			pSO = pSONext;		
		}

		/* release leaked physical boards */
		
		pSO = gpPhysBoard;
		
		while (pSO)
		{
			PVROSPrintf("PVROSDestroySystemContext: Shutdown: releasing physical board\n");

			ReleasePhysicalBoard (hVxD, pSO->pObject);

			pSONext = pSO->pNext;
			PVROSFree (pSO->pObject);
			PVROSFree (pSO);
			pSO = pSONext;		
		}

		/* release leaked shared blocks */
		
		pSO = gpSharedBlock;
		
		while (pSO)
		{
			ASSERT (pSO->pObject);

			PVROSPrintf("PVROSDestroySystemContext: Shutdown: releasing DMA buffer\n");

			FreeSharedBlock (hVxD, pSO->pObject);

			pSONext = pSO->pNext;
			PVROSFree (pSO->pObject);
			PVROSFree (pSO);
			pSO = pSONext;		
		}

		/* release leaked pcx buffers */
		
		pSO = gpDMASB;
		
		while (pSO)
		{
			ASSERT (pSO->pObject);

			PVROSPrintf("PVROSDestroySystemContext: Shutdown: releasing DMA scatter buffer\n");

			DestroyDMAScatterBuffer (hVxD, pSO->pObject);

			pSONext = pSO->pNext;
			PVROSFree (pSO->pObject);
			PVROSFree (pSO);
			pSO = pSONext;		
		}

		PVROSPrintf("PVROSDestroySystemContext: Shutdown complete\n");
	}

	//CloseHandle (hBufferMutex);
	CloseVxd (hVxD);
	
	return (PVROS_GROOVY);
}	

PVROSERR CALL_CONV PVROSDestroySystemContext (DWORD dwReason)
{
	PVROSDestroySystemContextLocal ();
	return (PVROS_GROOVY);
}

/******************************************************************************
 * Function Name: PVROSDelay
 *
 * Inputs       : PVR_DELAY eDelay, sgl_uint32 uCount
 * Outputs      : void
 * Returns      : void
 * Globals Used : -
 *
 * Description  : Waits the specified number of bus cycles or milliseconds
 *****************************************************************************/
void CALL_CONV PVROSDelay(PVR_DELAY eDelay, sgl_uint32 uCount)
{
	switch(eDelay)
	{
		case PVR_DELAY_MS:
			Sleep(uCount);
			break;
		case PVR_DELAY_BUS_CYCLES:
			{
				volatile sgl_uint32 i;
				for (i = 0; i < uCount; i++);
			}
			break;	
		default: 
			PVROSPrintf("Invalid type of PVROSDelay\n");
			break;
	}
}

/******************************************************************************
 * Function Name: PVROSPrintf
 *
 * Inputs       : char* pszFormat and more
 * Outputs      : void
 * Returns      : void
 * Globals Used : -
 *
 * Description  : Outputs a string, it uses 'vsprintf' (rather than
 * 				  'wvsprintf') to allow floating point number to be output.
 *****************************************************************************/

void CALL_CONV PVROSPrintf(char* pszFormat, ...)
{
	char szBuffer[128];

	/* Use the windows library function.
	 */	
	wvsprintf (szBuffer, pszFormat, (void *)(((DWORD)(&pszFormat)) + sizeof(void *)));
	OutputDebugString (szBuffer);
}


/******************************************************************************
 * Function Name: PVROSAssert
 *
 * Inputs       : char *szAssertText, char *szFileName, sgl_int32 nLine, int * ask)
 * Outputs      : -
 * Returns      : void
 * Globals Used : Int3
 *
 * Description  : Handler for an ASSERT() statement  				  
 *****************************************************************************/

void InitialiseAsserts(void)
{
	char szBuffer[256];
	
	#if WIN32
	GetEnvironmentVariable ("WINDIR", szBuffer, sizeof (szBuffer)-1);
	#else
	strcpy( szBuffer, getenv("WINDIR") );
	#endif

	strcat( szBuffer, "\\" );
	strcat( szBuffer, "sglhw.ini" );
	
	/* Output Debug String */
	gbODS =	 SglReadPrivateProfileInt ("Debug", "ODS", gbODS, szBuffer);
	gbNoGDI = SglReadPrivateProfileInt ("Debug", "NoGDI", gbNoGDI, szBuffer);

	gbInitialisedAsserts = TRUE;
}

void CALL_CONV PVROSAssert(char *szAssertText, char *szFileName, sgl_int32 nLine, int * ask)
{
	char szBuffer[256];
	
	if (!gbInitialisedAsserts)
	{
		InitialiseAsserts();
	}
	
	if (!gbNoGDI)
	{
		int nRes;
		static sgl_uint32 ReEnterCount=0;	

		ReEnterCount++;

		sprintf (szBuffer, 
		 "Assertion (%s) failed (entry %d)!\nFile %s, line %ld\nAbort?(Cancel=Debug)",
			 szAssertText, ReEnterCount, szFileName, (long)nLine);

		nRes = MessageBox (NULL, szBuffer, "SGL debug", MB_TASKMODAL | MB_ICONSTOP | MB_YESNOCANCEL);

		if (nRes == IDYES)
		{
			ExitProcess(0xFFFFFFFF);
		}
		else if (nRes == IDCANCEL)
		{
			Int3 ();
		}
		else if(*ask == 1)
		{
		
			nRes = MessageBox (NULL, "Ignore this assert in future?(Cancel stops this occuring again)", "SGL debug 2",
			  MB_TASKMODAL | MB_ICONSTOP | MB_YESNOCANCEL);

	 		if (nRes == IDYES)
			{
				*ask = 0;
			}
			else if(nRes == IDCANCEL)
			{
				*ask = 2;
			}

		}
		
		ReEnterCount--;
	}
	else
	{
		PVROSPrintf("Assertion (%s) failed! File %s, line %ld\n", szAssertText, szFileName, (long)nLine);

		Int3 ();
	}
}
/*
 * Performance tools
 */

sgl_uint32 CALL_CONV PVROSQueryPerformanceCounter(sgl_largeint* Val)
{
	/* Read the Time Stamp Counter and adjust it's value down a bit */
	_asm
	{
		_emit 0x0F
		_emit 0x31	
		mov		ecx, DWORD PTR [Val]
		shrd	eax,edx,7
		shr		edx,7
		mov		[ecx], eax
		mov		[ecx+4], edx	
	}
	
	return(TRUE);
}
sgl_uint32 CALL_CONV PVROSQueryPerformanceFrequency(sgl_largeint* Val)
{
	/* Reset the Time Stamp Counter by loading 0's into it */
	_asm
	{
		mov		ecx, 010h
		mov		edx, 000h
		mov		eax, 000h
		_emit	0x0F
		_emit	0x30
	}

	PVROSDelay(PVR_DELAY_MS,1000); /* 1 second pause */
	PVROSQueryPerformanceCounter(Val);

	return( TRUE );
}

/**********************************************************************/

void CALL_CONV PVROSGetWinVersions(HLDEVICE LogicalDevice, char **VersionInfo)
{
}

/**********************************************************************/

#if USE_VIRTUAL_BUFFERS
BUFFER_LIST *PVROSGetSecretBuffer(HDEVICE hDeviceID);
#endif

PVROSERR CALL_CONV PVROSScheduleRender (HLDEVICE hLDevice)
{
	DWORD 	dwData[2];
#if USE_VIRTUAL_BUFFERS
	int i, j, CopyTo;
	sgl_uint32  *from, *to;
#endif
	sgl_uint32  k, count;
#if 0	
	sgl_bool OutputStuff = FALSE;
#endif
		
	ASSERT (hLDevice->hVxD);

#if USE_VIRTUAL_BUFFERS

	if(hLDevice->VirtualBuffer->BufferType == VIRTUAL_BUFFER)
	{
		BUFFER_LIST *pBuffer;
		/* we've got to wait till the secret buffer is available,
		** copy in all our parameters, and the kick of the render
		*/

		pBuffer = PVROSGetSecretBuffer(hLDevice->hDeviceID);

		if(pBuffer == NULL)
		{
			PVROSPrintf("Real bad situation\n");
			hLDevice->VirtualBuffer->bInUse = FALSE;
			return PVROS_DODGY;
		}

		hLDevice->RendersPending++;
		count=0;
		while(pBuffer->bInUse == TRUE)
		{
			count++;
			Sleep(2);
			if(count>1000)
			{
				count=0;
				PVROSPrintf("Waiting for secret buffer\n");
				ASSERT(count);
			}
		}

		if(GetVirtualBufferMutex())
		{
			/* grab the buffer quickish */
			pBuffer->bInUse == TRUE;
			ReleaseVirtualBufferMutex();
		}
		else
		{
			PVROSPrintf("Couln't get mutex for secret buffer\n");
		}

		dwData[0] = (sgl_uint32)hLDevice;
		dwData[1] = (sgl_uint32)pBuffer;

		/* copy in our data */
		for(i=0; i<3; i++)
		{
			CopyTo = hLDevice->Buffers[i].uBufferPos;
			if(pBuffer->PVRParamBuf[i].uBufferLimit < hLDevice->Buffers[i].uBufferPos);
			{
				CopyTo = pBuffer->PVRParamBuf[i].uBufferLimit;
			}
			from = hLDevice->Buffers[i].pBuffer;
			to = pBuffer->PVRParamBuf[i].pBuffer;
			for(j=0; j<CopyTo; j++)
			{
				*to++ = *from++;
			}
		}

		for (k = 0; k < 32; ++k)
		{
			pBuffer->Registers[k] = hLDevice->Registers[k];
		}


		if(hLDevice->DeviceType==MIDAS4)
		{
			hLDevice->Registers[PCX_OBJECT_OFFSET] = 
				pBuffer->PVRParamBuf[PVR_PARAM_TYPE_REGION].pBuffer;
		}
		else
		{
			hLDevice->Registers[PCX_OBJECT_OFFSET] = 
				(int)pBuffer->PVRParamBuf[PVR_PARAM_TYPE_REGION].pBuffer - 
				(int)pBuffer->PVRParamBuf[PVR_PARAM_TYPE_ISP].pBuffer
				| 0x00000001UL;
		}

		pBuffer->PVRParamBuf[0].uBufferPos = hLDevice->Buffers[0].uBufferPos;
		pBuffer->PVRParamBuf[1].uBufferPos = hLDevice->Buffers[1].uBufferPos;
		pBuffer->PVRParamBuf[2].uBufferPos = hLDevice->Buffers[2].uBufferPos;
		
		/* make the virtual virtual buffer free */
		hLDevice->VirtualBuffer->bInUse == FALSE;

		/* in case  the VxD access the buffer thru the logical device */
		hLDevice->VirtualBuffer = pBuffer;

		/* set off the render after waiting */
		count = 0;
		while(hLDevice->RenderStatus == PVR_STATUS_PENDING)
		{
			count++;
			Sleep(2);
			if(count>500)
			{
				count=0;
				ASSERT(count);
			}
		}

		hLDevice->RenderStatus = PVR_STATUS_PENDING;

		if (DeviceIoControl (hLDevice->hVxD, VSGL_SCHEDULE_RENDER, 
							 dwData, sizeof (dwData), 
							 NULL, 0, 
							 NULL, NULL))
		{
			return (PVROS_GROOVY);
		}
		

	}
	else

#endif

#define USE_PCX2_AS_PCX1 0

#if USE_PCX2_AS_PCX1
	if(hLDevice->VirtualBuffer->BufferType == PCX2_AS_PCX1_BUFFER 
	   && hLDevice->DeviceType == MIDAS4)
	{
		BUFFER_LIST *pBuffer;
		/* we've got to wait till the secret buffer is available,
		** copy in all our parameters, and the kick of the render
		*/

		pBuffer = PVROSGetSecretBuffer(hLDevice->hDeviceID);

		if(pBuffer == NULL)
		{
			PVROSPrintf("Real bad situation\n");
			hLDevice->VirtualBuffer->bInUse = FALSE;
			return PVROS_DODGY;
		}

		hLDevice->RendersPending++;
		count=0;
		while(pBuffer->bInUse == TRUE)
		{
			count++;
			Sleep(2);
			if(count>1000)
			{
				count=0;
				PVROSPrintf("Waiting for secret buffer\n");
				ASSERT(count);
			}
		}

		if(GetVirtualBufferMutex())
		{
			/* grab the buffer quickish */
			pBuffer->bInUse == TRUE;
			ReleaseVirtualBufferMutex();
		}
		else
		{
			PVROSPrintf("Couln't get mutex for secret buffer\n");
		}

		dwData[0] = (sgl_uint32)hLDevice;
		dwData[1] = (sgl_uint32)pBuffer;

		/* copy in our data */
		for(i=0; i<3; i++)
		{
			CopyTo = hLDevice->Buffers[i].uBufferPos;
			if(pBuffer->PVRParamBuf[i].uBufferLimit < hLDevice->Buffers[i].uBufferPos);
			{
				CopyTo = pBuffer->PVRParamBuf[i].uBufferLimit;
			}
			from = hLDevice->Buffers[i].pBuffer;
			to = pBuffer->PVRParamBuf[i].pBuffer;
			for(j=0; j<CopyTo; j++)
			{
				*to++ = *from++;
			}
		}

		for (k = 0; k < 32; ++k)
		{
			pBuffer->Registers[k] = hLDevice->Registers[k];
		}


		hLDevice->Registers[PCX_OBJECT_OFFSET] = 
			pBuffer->PVRParamBuf[PVR_PARAM_TYPE_REGION].pBuffer;

		pBuffer->PVRParamBuf[0].uBufferPos = hLDevice->Buffers[0].uBufferPos;
		pBuffer->PVRParamBuf[1].uBufferPos = hLDevice->Buffers[1].uBufferPos;
		pBuffer->PVRParamBuf[2].uBufferPos = hLDevice->Buffers[2].uBufferPos;
		
		/* make the virtual virtual buffer free */
		hLDevice->VirtualBuffer->bInUse == FALSE;

		/* in case  the VxD access the buffer thru the logical device */
		hLDevice->VirtualBuffer = pBuffer;

		/* set off the render after waiting */
		count = 0;
		while(hLDevice->RenderStatus == PVR_STATUS_PENDING)
		{
			count++;
			Sleep(2);
			if(count>500)
			{
				count=0;
				ASSERT(count);
			}
		}

		hLDevice->RenderStatus = PVR_STATUS_PENDING;

		if (DeviceIoControl (hLDevice->hVxD, VSGL_SCHEDULE_RENDER, 
							 dwData, sizeof (dwData), 
							 NULL, 0, 
							 NULL, NULL))
		{
			return (PVROS_GROOVY);
		}
		

	}
	else
#endif
	{
		hLDevice->RendersPending++;

		dwData[0] = (sgl_uint32)hLDevice;
		dwData[1] = (sgl_uint32)hLDevice->VirtualBuffer;

		hLDevice->VirtualBuffer->PVRParamBuf[0].uBufferPos = hLDevice->Buffers[0].uBufferPos;
		hLDevice->VirtualBuffer->PVRParamBuf[1].uBufferPos = hLDevice->Buffers[1].uBufferPos;
		hLDevice->VirtualBuffer->PVRParamBuf[2].uBufferPos = hLDevice->Buffers[2].uBufferPos;
		
		for (k = 0; k < 32; ++k)
		{
			hLDevice->VirtualBuffer->Registers[k] = hLDevice->Registers[k];
		}

#if 0
		while(hLDevice->RendersPending > 1)
		{
			Sleep(1);
		}
#endif

		count = 0;
		while(hLDevice->RenderStatus == PVR_STATUS_PENDING)
		{
			/* if we wait longer than the render timeout for
			** the previous render to finish - somethings wrong
			*/
			Sleep(1);
			if(count>hLDevice->RenderTimeout)
			{
				count=0;
				ASSERT(count);
			}
			count++;
		}

#if 0
		/* Output the object pointers (and plane instructions) -
		 * compile uncommented and toggle on using WinICE when the 
		 * desired frame is reached.
		 */

		if (OutputStuff)
		{
			sgl_uint32 *pObjects = hLDevice->VirtualBuffer->PVRParamBuf[2].pBuffer;
			sgl_uint32 *pPlanes = hLDevice->VirtualBuffer->PVRParamBuf[0].pBuffer;
			sgl_bool Finished = FALSE;
			int nPasses = 0;

			while (!Finished) /* object */
			{
				sgl_uint32 Word = *pObjects;

				pObjects++;

				switch (Word >> 30)
				{
					case 0:
					{
						sgl_uint32 Addr = Word & 0x7ffff;
						sgl_uint32 Instr = (pPlanes[Addr] >> 26) & 0xF;

						#if PCX1
							Instr = (pPlanes[Addr] >> 26) & 0xF;
						#else
							Instr = pPlanes[Addr+3] & 0xF;
						#endif

						switch (Instr)
						{
							case 0xF:
							{
								PVROSPrintf ("\t%d Plane(s) at %X (%x): Trans pass start\n", ((Word >> 19) & 0x3ff), (Word & 0x7ffff) * 4, Instr);
								nPasses++;
								break;
							}

							default :
							{
								PVROSPrintf ("\t%d Plane(s) at %X (%x)\n", ((Word >> 19) & 0x3ff), (Word & 0x7ffff) * 4, Instr);
								break;
							}
						}
						break;
					}

					case 1:
					{
						PVROSPrintf ("TILE at (%d, %d), %d by %d\n", (((Word >> 15) & 0x1f) * 32), ((Word >> 20) & 0x3ff), (((Word & 0x1f)+ 1) * 32), ((Word >> 5) & 0x3ff)+1);
						break;
					}

					case 3:
					{
						PVROSPrintf ("Error in object data\n");
					}
					case 2:
					{
						/* end of list */
						PVROSPrintf ("Passes: %d\n", nPasses);
						Finished = TRUE;
						break;
					}
				}
			}
		}
#endif

		hLDevice->RenderStatus = PVR_STATUS_PENDING;

		if (DeviceIoControl (hLDevice->hVxD, VSGL_SCHEDULE_RENDER, 
							 dwData, sizeof (dwData), 
							 NULL, 0, 
							 NULL, NULL))
		{
			return (PVROS_GROOVY);
		}

		/* invalidate the buffers */
		hLDevice->Buffers[0].uBufferPos = 0;
		hLDevice->Buffers[1].uBufferPos = 0;
		hLDevice->Buffers[2].uBufferPos = 0;
		hLDevice->Buffers[0].pBuffer = NULL;
		hLDevice->Buffers[1].pBuffer = NULL;
		hLDevice->Buffers[2].pBuffer = NULL;
		hLDevice->Buffers[0].uBufferLimit = 0;
		hLDevice->Buffers[1].uBufferLimit = 0;
		hLDevice->Buffers[2].uBufferLimit = 0;
		hLDevice->VirtualBuffer->hLogicalDev = NULL;
		hLDevice->VirtualBuffer = NULL;
		

	}
	return (PVROS_DODGY);
}

PVROSERR PVROSSchedulerLock (HDEVICE hDeviceID)
{
	HANDLE hVxD = OpenVxd();

	DeviceIoControl (hVxD, VSGL_SCHEDULER_LOCK, 
						 hDeviceID, sizeof (int), 
						 NULL, 0, 
						 NULL, NULL);

	CloseVxd (hVxD);
	return (PVROS_GROOVY);
}

PVROSERR PVROSSchedulerUnlock (HDEVICE hDeviceID)
{
	HANDLE hVxD = OpenVxd();

	DeviceIoControl (hVxD, VSGL_SCHEDULER_UNLOCK, 
						 hDeviceID, sizeof (int), 
						 NULL, 0, 
						 NULL, NULL);

	CloseVxd (hVxD);
	return (PVROS_GROOVY);
}

/**********************************************************************/

#define AUTO_98_DETECT 1

/* Checks to see if the system is a PC98 and sets the global wIsPC98 */
void DetectPC98( void )
{
	#if AUTO_98_DETECT
	
		#pragma message ("Auto PC98 detection enabled")

		/* use the windows keyboard type function to determine if we
		   are in a PC98 machine - courtesy NEC */

		if (GetKeyboardType (0) == 7)
		{
			/* Japanese keyboard */
		#if 1
			int	nSubType = GetKeyboardType (1);

			/* Check range since we are having problems detecting
			 * new PC-98 keyboard types.
			 */
			if((nSubType > 0x0D00) && (nSubType < 0x0d08))
			{ 
				wIsPC98 = 1;   // PC-98
				PVROSPrintf("PC98 detected!\n");
			}
			else
			{
				wIsPC98 = 0;   // PC-AT
			}
		#else
			switch (GetKeyboardType (1))  /* get OEM type */
			{
				/* NEC PC98 subtypes */

				case 0x0D01:		/* PC-9800 series */
				case 0x0D02:		/* PC-9801 VX/UX and PC-98XL/XL2 (normal mode) */
				case 0x0D03:		/* PC-98XL/XL2 (high res mode) */
				case 0x0D04:		/* PC-9800 series laptop keyboard */
				{
					wIsPC98 = 1;
					PVROSPrintf("PC98 detected!\n");
					break;
				}

				default:
				{
					wIsPC98 = 0;
					break;
				}
			}
		#endif
		}
		else
		{
			wIsPC98 = 0;
		}

	#else

		#pragma message ("Auto PC98 detection disabled")
		char szIniFile[512];	

		strcpy(szIniFile, getenv ("WINDIR"));
		strcat(szIniFile, "\\");
		strcat(szIniFile, "sglhw.ini");

		wIsPC98 = SglReadPrivateProfileInt ("System", "PC98", 0, szIniFile);

	#endif
}

/**********************************************************************/

#define INTEL_VENDOR_ID (unsigned short)(0x8086)

static BYTE bOrigData;

PVROSERR ResetTritonControlRegister(void)
{
	static BYTE   bBusIDNum, bDevFncID, bData;
	static WORD  device_id;
	WORD wData;
	int    nPCIErr;

	nPCIErr = PCIRead16 (0, 0, 0x02, &wData);

	if ( nPCIErr != 0 )
	{
	    DPF((DBG_ERROR,"Error reading Device Id: Error %d.\n", nPCIErr ));

		return FALSE;
	}

	DPF((DBG_MESSAGE,"Found the following Device Id: %X in Slot 0\n", wData ));

	/* 
		Check if it is an Intel Triton ChipSet capable of handling streaming,
		try the standard one first 
	*/
    nPCIErr = PCIFindBoard (&bBusIDNum, &bDevFncID, INTEL_VENDOR_ID, 0x122D );

	if (nPCIErr != 0)
	{
		DPF((DBG_MESSAGE,"reading PCI registers containing the PCI Vendor and Device ID, returned %x\n", nPCIErr));
		DPF((DBG_MESSAGE,"Triton Chipset not detected\n"));

		/* safe failure - it not a triton chipset */
		return TRUE;
	}
	else
	{ 	   
		/* 
		** Reset the value from stored original value
		*/

	 	nPCIErr = PCIWrite8 (bBusIDNum, bDevFncID, 0x50, bOrigData);

	    if (nPCIErr != 0)
	    {
			DPF ((DBG_ERROR,"writing PCI register containing the PCI Control Byte, returned %x\n", nPCIErr));
			return FALSE;
	    }

		return TRUE;
	}

	
}

/**********************************************************************/

static sgl_bool TridentConditionalGraphicsCard()
{
	short (*pfnGetPCIList)(unsigned short *, unsigned short *, unsigned short);
	static BYTE   bTempBusIDNum, bTempDevFncID;
	sgl_bool retval = FALSE;
	HANDLE hLib;
	sgl_bool bNeedsFree = FALSE;

	if(!(hLib = GetModuleHandle("pvrsmx.dll")))
	{
		hLib = LoadLibrary("pvrsmx.dll");
		bNeedsFree = TRUE;
	}

	if(hLib)
	{
		pfnGetPCIList = (short (*)(unsigned short *, unsigned short *, unsigned short))GetProcAddress(hLib,"GetPCIList");

		if(pfnGetPCIList)
		{
			short ListSize;
			unsigned short vendor, device;

			ListSize = pfnGetPCIList(NULL,NULL,0);
			while(ListSize--)
			{
				pfnGetPCIList(&vendor, &device, ListSize);
				if(!PCIFindBoard(&bTempBusIDNum, &bTempDevFncID, 
								 vendor, device))
				{
					retval = TRUE;
				}
				
			}
			
		}

		if(bNeedsFree)
		{
			FreeLibrary(hLib);
		}
	}

	return retval;
}

/**********************************************************************/

PVROSERR SetupTritonControlRegister(void)
{

	/* this will only change PCI control register on 
	** Triton aka FX chipsets
	*/
	static BYTE   bBusIDNum, bDevFncID, bData;
	static BYTE   bTempBusIDNum, bTempDevFncID;
	static BYTE   bDataMask;
	static WORD  device_id;
	WORD wData;
	int    nPCIErr;
	sgl_uint32 uData;
	sgl_uint32 uPCIBursting, uPCIConcurrent;

	nPCIErr = PCIRead16 (0, 0, 0x02, &wData);

	if ( nPCIErr != 0 )
	{
	    DPF((DBG_ERROR,"Error reading Device Id: Error %d.\n", nPCIErr ));

		return FALSE;
	}

	DPF((DBG_MESSAGE,"Found the following Device Id: %X in Slot 0\n", wData ));

	/* 
		Check if it is an Intel Triton ChipSet capable of handling streaming,
		try the standard one first 
	*/
    nPCIErr = PCIFindBoard (&bBusIDNum, &bDevFncID, INTEL_VENDOR_ID, 0x122D );

	if (nPCIErr != 0)
	{
		DPF((DBG_MESSAGE,"reading PCI registers containing the PCI Vendor and Device ID, returned %x\n", nPCIErr));
		DPF((DBG_MESSAGE,"Triton Chipset not detected\n"));

		/* safe failure - it not a triton chipset */
		return TRUE;
	}
	else
	{ 	   

		DPF((DBG_WARNING,"Triton Chipset Detected!!!\n"));
		/* 
		**	Read the value that is there and save it somewhere safe
		*/
	 	nPCIErr = PCIRead8 (bBusIDNum, bDevFncID, 0x50, &bData);

		if (nPCIErr != 0)
		{
			DPF ((DBG_ERROR,"reading PCI register containing the PCI Control Byte, returned %x\n", nPCIErr));
		    return FALSE;
		} 	   
		else
		{
			SglWritePrivateProfileInt("PCIConfig","OrigPCIControlReg",bData,"sglhw.ini");
		}
		bOrigData = bData;

		/* set the settings to what we've got already */
		uPCIBursting = bData & (1<<2);
		uPCIConcurrent = bData & 1;

		if (TridentConditionalGraphicsCard())
		{
			DPF((DBG_WARNING,"Trident Conditional Graphics Card Detected"));
			uPCIBursting = 0;
			uPCIConcurrent = 1;
		}

		/* 
		** Read the value to set from the ini file to set
		*/

		uData = SglReadPrivateProfileInt("PCIConfig","PCIControlReg",bData,"sglhw.ini");

		if(uData < 256)
		{
			bData = (unsigned char)uData;
		}
		else
		{
			DPF ((DBG_WARNING,"Invalid PCI register setting in ini file"));
		}

		uPCIBursting = SglReadPrivateProfileInt("PCIConfig",
												 "PCIBursting",
												 uPCIBursting,"sglhw.ini");
		uPCIConcurrent = SglReadPrivateProfileInt("PCIConfig",
												  "PCIConcurrent",
												  uPCIConcurrent,"sglhw.ini");

		/* treat the value read as booleans and put the flags in the right place
		** dont trust the values in what was read
		*/
		bDataMask = 0xfa;
		/* bDataMask = ( 0xff & ~(BYTE)(1<<2) & ~(BYTE)(1)); */

		bData &= bDataMask;
		bData |= ((BYTE)(((uPCIBursting)?1:0)<<2) | (BYTE)((uPCIConcurrent)?1:0));

		DPF ((DBG_WARNING,"Writing PCI cotrol register with %.0X\n", bData));

	 	nPCIErr = PCIWrite8 (bBusIDNum, bDevFncID, 0x50, bData);

	    if (nPCIErr != 0)
	    {
			DPF ((DBG_ERROR,"writing PCI register containing the PCI Control Byte, returned %x\n", nPCIErr));
			return FALSE;
	    }

		return TRUE;
	}

}

/**********************************************************************/

#if DEBUG || DEBUGDEV
char sDllName[] = "PVROS: ";
#endif

/**********************************************************************/

BOOL WINAPI _CRT_INIT( HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved );

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved)
{			

   	switch (fdwReason)
	{
		case DLL_PROCESS_ATTACH:
		{
	
			if (gnRefCount == 0)
			{
				if (!_CRT_INIT( hinstDLL, fdwReason, lpReserved ))
				{
					return(FALSE);
				}

				if (!MemInit ())
				{
					return(FALSE);
				}

#if DEBUG
				DebugInit(DBGPRIV_WARNING);
#elif DEBUGDEV || TIMING
				DebugInit(DBGPRIV_MESSAGE);
#endif
				/* Check the the system we're running on */
				DetectPC98();

				/* Setup the PCI Control Resgister - unless it's a PC98 */
				if (!wIsPC98)
				{				
					if(!SetupTritonControlRegister())
					{
						return(FALSE);
					}
				}


			}

			if (hSysProcess == 0)
			{
				if (PVROSCreateSystemContext (hinstDLL, fdwReason) != PVROS_GROOVY)
				{
					return(FALSE);
				}
			}

			DPF((DBG_MESSAGE,"PVROS DLL_PROCESS_ATTACH: Ref. Count = %d",gnRefCount));
			gnRefCount++;

			return(TRUE);
		}

		case DLL_THREAD_ATTACH:		
		{
			DPF((DBG_MESSAGE,"PVROS DLL_THREAD_ATTACH"));
			
			return(TRUE);
		}

		case DLL_PROCESS_DETACH:
		{
			DPF((DBG_MESSAGE,"PVROS DLL_PROCESS_DETACH: Ref. Count = %d",gnRefCount));

			gnRefCount--;		
	
			if (hSysProcess == GetCurrentProcessId ())
			{
				if (gnRefCount != 0)
				{
					DPF((DBG_WARNING,"Killing off system context prematurely"));
				}

				PVROSDestroySystemContext (fdwReason);			

				hSysProcess = 0;
				hSystemVxD = 0;
			}

			if (gnRefCount == 0)
			{
				ResetTritonControlRegister();
				MemFini ();
			}

			return(TRUE);
		}
	
		case DLL_THREAD_DETACH:
		{
			DPF((DBG_MESSAGE,"PVROS DLL_THREAD_DETACH"));

			//PVROSDestroySystemContext (fdwReason);			
			
			return(TRUE);
		}
	}
}

/* end of $RCSfile: system.c,v $ */

/*****************************************************************************
 *;++
 * Name           : $RCSfile: brdsetup.c,v $
 * Title          : BRDSETUP.C
 * C Author       : Jim Page
 * Created        : 21/04/95
 *
 * Copyright	: 1995-2022 Imagination Technologies (c)
 * License		: MIT
 *
 * Description    :	board initialisation
 *                   
 * Program Type   :	WIN32 DLL
 *
 * RCS info:
 *
 * $Date: 1997/11/28 16:17:15 $
 * $Revision: 1.74 $
 * $Locker:  $
 * $Log: brdsetup.c,v $
 * Revision 1.74  1997/11/28  16:17:15  erf
 * Removed unneeded OpenVXD() calls which were causing performance
 * decreases.
 *
 * Revision 1.73  1997/11/03  16:54:48  erf
 * Added SHARED_HEAP define to toggle between VxD or Microsoft shared heap
 * allocation.
 *
 * Revision 1.72  1997/10/31  15:35:37  erf
 * When allocating, realloc'ing and freeing TSP parameter memory use
 * new PVROSTSP..() routines defined in system.c which allocate locked
 * system memory through VxD calls.
 *
 * Revision 1.71  1997/10/28  10:38:03  erf
 * Added routine PVROSExtendTSPBuffer() which is used to extend the TSP
 * if required. Should be here because it is required by both D3D and
 * SGLDirect.
 *
 * Revision 1.70  1997/10/14  16:49:32  gdc
 * added function to allow Virtual Buffers to be freed if address mode
 * callback fails
 *
 * Revision 1.69  1997/10/08  11:10:38  mjg
 * Put the TSP offset for PCX1 back to 12 as Sphere in wireframe with
 * highlights on went pink.
 *
 * Revision 1.68  1997/09/24  09:43:38  gdc
 * changed HWRdavalFile's to a new function which has a revised priority
 * of where to get values
 *
 * Revision 1.67  1997/08/28  09:05:44  erf
 * Added functionality to disable PCI memory write and invalidate if the
 * cache line size has been set to zero. It is enabled if the cache line
 * size is none zero.
 *
 * Revision 1.66  1997/08/27  16:41:38  erf
 * Added code to set latency and cache line size.
 *
 * Revision 1.65  1997/08/26  16:39:33  erf
 * Changed TSP base address for PCX1 and PCX2 to 6. Fix for flat shahed
 * highlighted triangles handled in triangle processing.
 *
 * Revision 1.64  1997/08/19  15:52:54  gdc
 * made TSP buffer start position a define at top of file
 *
 * Revision 1.63  1997/08/08  13:48:59  mjg
 * Added support for PCX2_003.
 *
 * Revision 1.62  1997/08/08  09:42:44  sks
 * As previous branch revision
 *
 * Revision 1.61.0.1  1997/08/01  08:37:40  sks
 * Added GetParamSettings function to cope with dodgy ParamSizes
 * in the registry and cut out repeated registry reads
 *
 * Revision 1.61  1997/07/18  14:49:51  gdc
 * tidy up
 *
 * Revision 1.60  1997/07/18  14:25:31  gdc
 * added a module ID
 *
 * Revision 1.59  1997/07/18  14:00:48  gdc
 * PC'ified
 *
 * Revision 1.58  1997/07/16  16:25:21  gdc
 * put a bit back -- need to look at how IRQ hooking and unhooking
 * is done
 *
 * Revision 1.57  1997/07/16  16:04:06  gdc
 * removed some unused code and put in messages if someone tries
 * to reach this code
 *
 * Revision 1.56  1997/07/14  15:07:00  gdc
 * fixed stupid type in last change
 *
 * Revision 1.55  1997/07/14  14:49:12  gdc
 * made more printf's debug messages
 *
 * Revision 1.54  1997/07/14  13:37:44  gdc
 * turned PVROSPrintfs to DPFs (as both messsages and warnings)
 * should be a lot less crap output
 *
 * Revision 1.53  1997/06/26  10:17:01  erf
 * Fixed mutex stuff. Extra calls that weren't needed.
 * ie in the FindUnusedBuffer().
 *
 * Revision 1.52  1997/06/23  22:00:57  jop
 * Calls GetHighWaterMark before assigning virtual buffer - ensures
 * expansion and contraction are sorted between apps (I hope)
 *
 * Revision 1.51  1997/06/23  16:06:30  jop
 * Went back to economical TSP buffers
 *
 * Revision 1.50  1997/06/22  17:40:34  jop
 * Added adaptive TSP parameter stuff
 *
 * Revision 1.49  1997/06/17  21:14:24  mjg
 * bogus pcx1 mode
 *
 * Revision 1.48  1997/06/17  20:31:39  mjg
 * Fixed VxD open/close
 *
 * Revision 1.47  1997/06/17  13:24:24  mjg
 * Checked in John's All American changes.
 *
 * Revision 1.46  1997/06/17  10:53:20  mjg
 * Changes for multi-context PCX1 (from USA).
 *
 * Revision 1.46  1997/06/17  10:53:20  mjg
 * Changes for multi-context PCX1 (from USA).
 *
 * Revision 1.45  1997/06/17  10:00:59  mjg
 * Removed a couple of lines .
 *
 * Revision 1.44  1997/06/17  09:38:00  mjg
 * Added the FORCE_NO_FPU and BogusPCX1 options.
 *
 * Revision 1.43  1997/06/04  13:53:03  gdc
 * stopped compiler warnings
 *
 * Revision 1.42  1997/06/02  02:41:13  jop
 * Now uses getenv windows functions
 *
 * Revision 1.41  1997/05/30  16:36:59  mjg
 * Passed teh logical device into ResetCallbacks.
 *
 * Revision 1.40  1997/05/28  11:28:24  mjg
 * Changed the default initial TSP buffer position back to 12 for PCX1.
 *
 * Revision 1.39  1997/05/28  08:22:55  gdc
 * nade default render timeout same as current ini file
 *
 * Revision 1.38  1997/05/23  14:36:17  mjg
 * Removed ResetCallbacks to brdsetup.c, to be used when a device is
 * destroyed, rather than simply closed.
 *
 * Revision 1.37  1997/05/21  14:12:00  gdc
 * put in more checks and chnaged the buffer allocation code to avoid
 * sleeping unless there is something to definitly wait for
 *
 * Revision 1.36  1997/05/20  14:08:08  gdc
 * made logical device count match actual sucessfully created devices
 *
 * Revision 1.35  1997/05/19  15:36:36  gdc
 * put in stuff to allow pcx1 to use pcx2 buffers,
 * but left it disabled
 *
 * Revision 1.34  1997/05/19  14:52:33  jop
 * Tried tracking number of LogDevs vs Buffers and keeping one in
 * hand - seems to solve most frag problems. Disabled virtual virtual
 * buffers (?!)
 *
 * Revision 1.33  1997/05/16  17:55:33  gdc
 * more messages
 *
 * Revision 1.32  1997/05/16  17:42:15  gdc
 * added more buffer messages and made some clearer
 *
 * Revision 1.31  1997/05/16  14:43:50  gdc
 * mutex code moved to system
 *
 * Revision 1.30  1997/05/16  11:30:53  gdc
 * made render timeout right size
 *
 * Revision 1.29  1997/05/16  02:02:36  mjg
 * Pointed the PCX1 OBJECT_OFFSET (virtual) register at the right place.
 *
 * Revision 1.28  1997/05/15  19:48:08  gdc
 * added mutex when marking buffers in use etc.
 *
 * Revision 1.27  1997/05/15  17:37:14  gdc
 * some more virtual buffer changes
 *
 * Revision 1.26  1997/05/15  13:19:24  gdc
 * adde rest of virtual virtaul buffers - not really tested, and it uses
 * pvrosmalloc which will probably fail at the same time as the rest
 * I need to try windows allocation stuff
 *
 * Revision 1.25  1997/05/15  11:19:28  gdc
 * checkpoint - started adding virtual virtual buffers
 *
 * Revision 1.24  1997/05/14  18:51:00  gdc
 * fixed typo and removed last conditional pcx1 bit
 *
 * Revision 1.23  1997/05/14  18:38:17  gdc
 * rationalised the memory requests for virtual buffers,
 * reduced the default size, and set the sabre ptr
 *
 * Revision 1.22  1997/05/14  15:11:38  gdc
 * added a (hopeful) function to reinit some static data - called from dllmain
 * on a ref count of zero
 *
 * Revision 1.21  1997/05/14  14:18:10  gdc
 * initialised RenderPending var in logical dev
 * added a destroy all virtual buffers (called from dllmain
 * when process detach refcount is zero)
 * added a virtual buffer creation on logical device creation
 *
 * Revision 1.20  1997/05/14  10:27:14  gdc
 * removed unused code
 *
 * Revision 1.19  1997/05/14  03:43:08  jop
 * Scheduler tweaks and merged PCX1/PCX2 stuff back into hwregs.c
 *
 * Revision 1.18  1997/05/14  00:51:25  mjg
 * Updated for PCX1 - doesn't work mind.
 *
 * Revision 1.17  1997/05/14  00:25:20  jop
 * Replaced WriteVirtualReg and made it unbuildable
 *
 * Revision 1.16  1997/05/13  20:53:42  jop
 * Fixed a couple of bugettes and changed the board data struct
 *
 * Revision 1.15  1997/05/13  16:26:23  gdc
 * checkpoint with virtual buffers - do not expect to work
 *
 * Revision 1.14  1997/05/07  13:53:55  gdc
 * removed some stuff I'd started on but not needed now
 *
 * Revision 1.13  1997/05/07  09:54:35  gdc
 * little workaround for PCX1
 *
 * Revision 1.12  1997/05/06  17:05:48  erf
 * Added lookup table to changed texture filtering settings to hardware
 * required values.
 *
 * Revision 1.11  1997/05/06  16:45:26  gdc
 * intermediate check in of brdsetup with the start of stuff to work
 * on multiple boards
 *
 * Revision 1.10  1997/05/01  17:00:37  mjg
 * Changes to get PCX1 to build and I've replaced UINT's with sgl_uint's
 *
 * Revision 1.9  1997/04/29  15:47:40  erf
 * Removed #include "pvrosP.h".
 *
 * Revision 1.8  1997/04/29  15:32:58  erf
 * Changed CopyParamsIfRequired() to PVROSCopyParamsIfRequired().
 *
 * Revision 1.7  1997/04/26  14:15:05  erf
 * Added USE_HEAPMAN to get globally accessible heap
 *
 * Revision 1.6  1997/04/26  13:06:28  erf
 * Added reference count for physical devices
 *
 * Revision 1.5  1997/04/25  13:52:37  gdc
 * added private pvros header
 *
 * Revision 1.4  1997/04/25  09:35:20  gdc
 * addded call to get a set or virtual buffers and create them
 * but I guess the creation should soon devolve to vxd land
 *
 * Revision 1.2  1997/04/24  17:30:43  gdc
 * added first cut at create/destroy device functions
 *
 * Revision 1.1  1997/04/23  20:31:48  mjg
 * Initial revision
 *
 * Revision 1.4  1997/04/23  18:45:05  gdc
 * update the file - not working though
 *
 * Revision 1.3  1997/04/23  16:29:23  mjg
 * Inclued PCX1 specific globals..
 *
 * Revision 1.2  1997/04/23  15:56:37  gdc
 * update a bit or two - still not working
 *
 * Revision 1.1  1997/04/23  14:03:18  gdc
 * Initial revision
 *
 ***********************************************************************/

#define MODULE_ID MODID_BRDSETUP

#include <math.h>
#include <time.h>
#include <windows.h> /* for GetEnvironmentVariable */

#include "sgl_defs.h"
#include "pvrosapi.h"
#include "pci.h"
#include "brdsetup.h"
#include "fogtable.h"
#include "hwregs.h"
#include "brdcfg.h"
#include "profile.h"
#include "metrics.h"
#include "hwtexas.h"

extern HANDLE OpenVxd(void);
extern void CloseVxd(HANDLE);
 
#define USE_VIRTUAL_BUFFERS 0
#if USE_VIRTUAL_BUFFERS
#pragma message ("Use of virtual buffers ENABLED!")
#else
#pragma message ("Use of virtual buffers DISABLED!")
#endif

/* The actual start address may be modified by PackTexasMask() if a masking
 * plane is required.
 */
#define PCX1_TSP_BASE_POS 12
#define PCX2_TSP_BASE_POS 6

/* TEX_PARAM_SIZE defined in hwtexas.h */
#define ISP_PARAM_SIZE  512*1024
#define OBJECT_PARAM_SIZE  512*1024

#define SCBUS_BURST_PAGES			0x400
#define SCBUS_IO_PAGES				0x10

/*
 * 0x08 = write through
 * 0x10 = cache disable
 */								 

#define DISABLE_CACHE				0x10
#define WRITE_COMBINING				0x00 /* Only valid for P6, P5 does nothing */

/* Globals containing PCI address space locations */

#pragma data_seg(".onetime")


typedef struct tagLIST {
	struct tagLIST *Next;
	struct tagLIST *Prev;
	HLDEVICE hLogicalDev;
} LIST, *PLIST;

static int RealBufferCount = 0;
static int VirtualBufferCount = 0;
static BUFFER_LIST *RealVirtualBuffer = NULL;
static BUFFER_LIST *VirtualVirtualBuffer = NULL;
static char Safety[1024];


#define MAX_BOARDS 16

typedef struct 
{
	int nRefCount;
	DEVICEID DeviceID;
	HDEVICE ghDeviceID;
#if USE_VIRTUAL_BUFFERS
	BUFFER_LIST *SecretBuffer;
#endif
	int			nLogicalDevices;
	LIST        *LDeviceList;
} BOARDREF;

/* this assumes we have a max of 16 boards 
** - Must assume all data initialised to 0 
*/
static	BOARDREF gBoardRef[MAX_BOARDS]; 

volatile sgl_uint32 * volatile pSCBusBurstSpace = NULL;
volatile sgl_uint32 * volatile pSCBusIOSpace 	= NULL;

/*static DMABUFFER SCBusIOSpace = {0};*/

sgl_uint32 physISPObjectData = {0};

sgl_uint32 TSPDoubleBuffer = 1;  /* Needed for dtri.c */

/* Hardware globals */
static sgl_uint16	wPCIVendorID = 0;

#if !DOS32
static sgl_uint32	pciHeader0C = 0;
#endif

/* Required in HW Setup */
sgl_uint16	wPCIDeviceID = 0;

/* Hook IRQ stuff */
static sgl_uint32 fHooked = FALSE;

#pragma data_seg()

void ReleaseVirtualBufferMutex(HANDLE hMutex);
sgl_bool GetVirtualBufferMutex(HANDLE hMutex);
PVROSERR PVROSSetupPCILatency(HDEVICE  hDeviceID);

BUFFER_LIST *AddNewBuffer(HLDEVICE hLogicalDev,sgl_bool AttachToList);

BUFFER_LIST *AddNewBufferPCX1(HANDLE hVxD,
							  DEVICE_TYPE DeviceType,
							  sgl_uint32 PlaneMemBytes, 
							  sgl_uint32 ObjectMemBytes,
							  sgl_uint32 TexasMemBytes);

BUFFER_LIST *AddNewBufferPCX2(HANDLE 		hVxD,
							  DEVICE_TYPE	DeviceType,
							  sgl_uint32 	PlaneMemBytes, 
							  sgl_uint32 	ObjectMemBytes,
							  sgl_uint32 	TexasMemBytes);

void PVROSDestroyAVirtualBuffer(DEVICE_TYPE DeviceType,HANDLE hVxD,HANDLE hBufferMutex);
BUFFER_LIST *FindUnusedBuffer(DEVICE_TYPE DeviceType,HANDLE hBufferMutex);
static void CALL_CONV GetParamSettings(HLDEVICE Device);

/* Select which TSP parameter allocation method to use.
 * 0 allocates memory via the VxD (ie PageReserve, PageCommit etc.) and ensures that
 * it is fixed.
 */
#define	SHARED_HEAP	0

extern void *CALL_CONV PVROSTSPAlloc(HANDLE hVxD, sgl_uint32 dwSize);
extern void *CALL_CONV PVROSTSPRealloc(HANDLE hVxD, void *Address, sgl_uint32 dwSize);
extern void CALL_CONV PVROSTSPFree (HANDLE hVxD, void *Address);



/**********************************************************************/

sgl_uint32 PVROSExtendTSPBuffer(HLDEVICE gHLogicalDev)
{
	sgl_int32 nCurrent, nExtended, nResult;
	PVR_PARAM_BUFF *pBuff = &gHLogicalDev->Buffers[PVR_PARAM_TYPE_TSP];
	
	nCurrent = pBuff->uBufferLimit << 2;
	nExtended = nCurrent + 1024*64;
	
	nResult = PVROSSetTSPHighWaterMark (gHLogicalDev->TexHeap, nExtended, FALSE);
		
	if (nResult == nExtended)
	{
		void *pNew;

#if SHARED_HEAP
		pNew = PVROSRealloc (pBuff->pBuffer, nExtended);
#else
		pNew = PVROSTSPRealloc(gHLogicalDev->hVxD, pBuff->pBuffer, nExtended);
#endif

		if (!pNew)
		{
			PVROSPrintf ("Error reallocating TSP host buffer\n");
		}
		else
		{
			if (pNew != pBuff->pBuffer)
			{
				#if DEBUG
				PVROSPrintf ("Realloc gave new ptr: 0x%08lx->0x%08lx\n", pBuff->pBuffer, pNew);
				#endif
			}
			
			pBuff->pBuffer = pNew;
			pBuff->uBufferLimit = nResult >> 2;
			
			gHLogicalDev->VirtualBuffer->PVRParamBuf[PVR_PARAM_TYPE_TSP] = *pBuff;
		}
	}
	
	return (pBuff->uBufferLimit - pBuff->uBufferPos);
}

/**********************************************************************/

PVROSERR PVROSSetupPCIMemWrite(HDEVICE hDeviceID, sgl_uint32 nMemWrite)
{
	sgl_uint32	pciHeader0C;

	/* Read Bridge header offset 0C into pciHeader0C
	 */
	PVROSReadPCIConfigSpace(hDeviceID, 0x04, &pciHeader0C);
	
	/* If a new cache line size has been specified, programme the bridge
	 */
	pciHeader0C &= 0xFFFFFFEFL;
	pciHeader0C |= (sgl_uint32) (nMemWrite << 4);
	PVROSWritePCIConfigSpace(hDeviceID, 0x04, pciHeader0C);
	
	return PVROS_GROOVY;
}

/**********************************************************************/

PVROSERR PVROSSetupPCICacheLine(HDEVICE hDeviceID)
{
	sgl_uint32	pciHeader0C;
	sgl_uint32	nCacheLineSize = 0;

	/* SETUP CACHE LINE SIZE IF USER SELECTION DIFFERENT FROM BIOS SETTING.
	 */
	/* Read Bridge header offset 0C into pciHeader0C
	 */
	PVROSReadPCIConfigSpace(hDeviceID, 0x0C, &pciHeader0C);
	
	/* Cache line size fix : Allow user override
	 */
	nCacheLineSize =  HWRdValFileUInt ("PCX2CacheLineSize", 0);

	/* Need to disable memory write and invalidate if cache line
	 * size set to zero.
	 */
	if (nCacheLineSize == 0)
	{
		/* Disable.
		 */
		PVROSSetupPCIMemWrite(hDeviceID, 0);
	}
	else
	{
		/* Enable.
		 */
		PVROSSetupPCIMemWrite(hDeviceID, 1);
	}

	/* If a new cache line size has been specified, programme the bridge
	 */
	pciHeader0C &= 0xFFFFFF00L;
	pciHeader0C |= (sgl_uint32)nCacheLineSize;
	PVROSWritePCIConfigSpace(hDeviceID, 0x0C, pciHeader0C);
	
	/* END CACHE LINE LINE SIZE SET.
	 */
	return PVROS_GROOVY;
}

/**********************************************************************/

PVROSERR PVROSSetupPCILatency(HDEVICE hDeviceID)
{					
	sgl_uint32 pciHeader0C;
	sgl_uint8  nUserLatency = 0, nSysLatency = 0;

	/* SETUP LATENCY IF USER SELECTION DIFFERENT FROM BIOS SETTING.
	 */
	/* Read Bridge header offset 0C into pciHeader0C
	 */
	PVROSReadPCIConfigSpace(hDeviceID, 0x0C, &pciHeader0C);
	
	/* Debug : see what the latency was set to by the PCI bios
	 */
	nSysLatency = (sgl_uint8) (pciHeader0C >> 8);
	
	/* Latency fix : Allow user override
	 */
	nUserLatency = (sgl_uint8) HWRdValFileUInt ("Latency", 0);
	
	/* If system and no user setting set to default.
	 */
	if ((nUserLatency == 0) && (nSysLatency == 0))
		nUserLatency = 0x40;
	
	if (nUserLatency)
	{
		/* Read Bridge header offset 0C */
		PVROSReadPCIConfigSpace(hDeviceID, 0x0C, &pciHeader0C);	
		/* If a new latency has been specified, programme the bridge */
		pciHeader0C &= 0xFFFF00FFL;
		pciHeader0C |= (((sgl_uint32)nUserLatency)<<8);
		PVROSWritePCIConfigSpace(hDeviceID, 0x0C, pciHeader0C);
	}

	return PVROS_GROOVY;
}

/*****************************************************************************/

PVROSERR DMASBToPCXTLB(DMASCATTERBUFFER *pDMASB,
					   PCXBUFFER *pPCXBuffer,
					   sgl_uint32 dwBytesInBuffer)
{
	PVROSERR fRet = PVROS_DODGY;

	if (pDMASB->PagesAllocated < (dwBytesInBuffer >> 12))
	{
		DPF((DBG_ERROR,"Not enough memory for ISP buffer"));
	}
	else if (pDMASB->BlockSize < 2)
	{
		DPF((DBG_ERROR,"Not enough contiguous memory for ISP buffer"));
	}
	else
	{
		int i;
		sgl_uint32	Limit;
		
		sgl_uint32 *SrcPhysAddress, *DestPhysAddress, *SrcPhysPages, *DestLimit;
		sgl_uint32 Blocks, BasePhysAddress;

		fRet = PVROS_GROOVY;
		
		DPF((DBG_MESSAGE,"%dk ISP buffer allocated", pDMASB->PagesAllocated * 4));
		
		if (pDMASB->BlockSize >= 4)
		{
			/* configure for 16k pages */
			
			pPCXBuffer->PageSize = 2;
			/*BlockSize = 4;*/
		}
		else if (pDMASB->BlockSize == 2)
		{
			/* configure for 8k pages */
			
			pPCXBuffer->PageSize = 1;
			/*BlockSize = 2;*/
		}
		else
		{
			/* configure for 4k pages */
			
			pPCXBuffer->PageSize = 0;
			/*BlockSize = 1;*/
		}
		
		pPCXBuffer->LinearAddress = pDMASB->LinearAddress;
		
		DestPhysAddress = pPCXBuffer->PhysAddresses;
		DestLimit = pPCXBuffer->Limit;
		SrcPhysAddress = pDMASB->PhysAddress;
		SrcPhysPages = pDMASB->PhysPages;
		
		pPCXBuffer->SlotsUsed = pDMASB->PagesAllocated >> pPCXBuffer->PageSize;
		
		i = pPCXBuffer->SlotsUsed;
			
#define OPTIMISE_LIMITS 1
		
		Limit = 0;
		
		do
		{
#if OPTIMISE_LIMITS
			Limit += 0x400 * *SrcPhysPages;
#endif
			Blocks = *SrcPhysPages >> pPCXBuffer->PageSize;
			BasePhysAddress = *SrcPhysAddress;
			
			SrcPhysPages ++;				
			SrcPhysAddress ++;				
			
			do
			{
#if !OPTIMISE_LIMITS
				Limit += 0x400 << pPCXBuffer->PageSize;
#endif
				*DestLimit = Limit;
				*DestPhysAddress = BasePhysAddress;
				
				DestPhysAddress++;
				DestLimit++;					
				BasePhysAddress += 0x1000 << pPCXBuffer->PageSize;
				i--;
				Blocks--;
			}
			while (i && Blocks);
		}
		while (i);
	}

	return (fRet);
}

/**********************************************************************
** Function Name: GetBuffer1Size
**
** Inputs       : -
** Outputs      : -
** Returns      : Number of pages to be allocated to ObjectPointers
**
** Description  : Attempts to read the environment variable saying
**                where windows booted from and then using this builds
**                a path to the system.ini file which contains the
**                setting the VxD used to initailise the contigious 
**                space used for object pointers 
***********************************************************************/

static int GetBuffer1Size()
{
	int PCXRegionDataPages = PCX_REGION_DATA_PAGES;

	char sWinDir[256] = "\0";

	GetEnvironmentVariable ("WINDIR", sWinDir, sizeof (sWinDir)-1);

	if(sWinDir[0] != 0)
	{
		int len;
		char *SystemIni = NULL;
		
		len = strlen(sWinDir);
		len += (strlen("\\system.ini") + 2);
		SystemIni = PVROSMalloc(len);
		if(SystemIni)
		{
			strcpy(SystemIni, sWinDir);
			strcat(SystemIni, "\\system.ini");
			PCXRegionDataPages = 
				SglReadPrivateProfileInt("VSGL", "Buffer1Size", 
										 PCX_REGION_DATA_PAGES, 
										 SystemIni);
			PVROSFree(SystemIni);
		}
		else
		{
			/* the PVROSMalloc failed, try to read in most common place
			** or set to the default value 
			*/
			PCXRegionDataPages = 
				SglReadPrivateProfileInt("VSGL", "Buffer1Size", 
										 PCX_REGION_DATA_PAGES, 
										 "c:\\windows\\system.ini");

		}
	}
	else
	{
		/* Getenv Failed - this must be a very strange windows
		** behave as above 
		*/
		PCXRegionDataPages = 
			SglReadPrivateProfileInt("VSGL", "Buffer1Size", 
									 PCX_REGION_DATA_PAGES, 
									 "c:\\windows\\system.ini");
	}

	return PCXRegionDataPages;

}

/******************************************************************************
 * Function Name: HookIRQ
 *
 *****************************************************************************/

sgl_uint32 PVROSHookIRQ (sgl_uint32 fEnable, DEVICE_TYPE DeviceType, HDEVICE hDeviceID)
{
	sgl_uint32 fRet = TRUE;
	
	if (fEnable)
	{
		if (!fHooked)
		{
			if (PVROSSetupPowerVRDevice (hDeviceID, DeviceType) != PVROS_GROOVY)
			{
				DPF((DBG_ERROR,"Error enabling physical board"));
			}
			else
			{
				fHooked = TRUE;
			}
		}
	}
	else
	{
		if (fHooked)
		{
			if (PVROSSetupPowerVRDevice (hDeviceID, DISABLE_BOARD) != PVROS_GROOVY)
			{
				DPF((DBG_ERROR,"Error disabling physical board"));
			}
			else
			{
				fHooked = FALSE;
			}
		}
	}

	return (fRet);
}
																		


/******************************************************************************
 * Function Name: FindBoardAndInitAddresses
 *
 * Inputs       : -
 * Outputs      : -
 * Returns      : TRUE of success, FALSE if failure
 * Globals Used : -
 *
 * Description  : find board and decode BIOS allocated memory and get aliases
 *****************************************************************************/

PVROSERR CALL_CONV PVROSInitPCIAddresses (HDEVICE hDeviceID, DEVICE_TYPE DeviceType)
{
	/* Setup PCI latency.
	 */
	PVROSSetupPCILatency(hDeviceID);

	/* create a set of virtual buffers just in case */
	switch(DeviceType)
	{
		case(PCX1_DEVICE_ID):
		break;

		case(PCX2_DEVICE_ID):

			/* Setup PCI Cache line size.
			 */
			PVROSSetupPCICacheLine(hDeviceID);

		break;

		default:
		break;
	}
	
	return (PVROS_GROOVY);						
					
} /*InitAddresses*/

/******************************************************************************
 * Function Name: DetermineTexMemConfig()
 *
 * Hardcoded to 4M currently as there are no other sized cards around
 *
 *****************************************************************************/

sgl_uint32 PVROSDetermineTexMemConfig( sgl_uint32 dummy)
{   
	sgl_uint32 TextureMemorySize;
	
	/* hard code to 4Mb */
	
	TextureMemorySize = (4 << 20);
	return TextureMemorySize;

}

/**********************************************************************/

sgl_uint32 *CALL_CONV PVROSGetEndOfRenderPtr (HLDEVICE hLogDev)
{
	DPF((DBG_WARNING, "PVROSGetEndOfRenderPtr called"));
	return (sgl_uint32 *) hLogDev->BoardData.pStatus;
}

/**********************************************************************/

sgl_uint32 *CALL_CONV PVROSGetVirtualBurstSpace()
{
	/* this function used to return a pointer to the on card memory
	** but this whole thing is now set in the vxd and not available here
	** so return zero and hope it is noticed -- pSCBusBurstSpace is 
	** zero any way
	*/
#if DEBUG
	DPF((DBG_WARNING, "Non-functional PVROSGetVirtualBurstSpace called"));
#else
	PVROSPrintf("Non-functional PVROSGetVirtualBurstSpace called\n");
#endif
	/*
	**return (sgl_uint32 *)pSCBusBurstSpace;
	*/

	return NULL;
}

/**********************************************************************/

int CALL_CONV PVROSGetUsagePhysicalDevice(PciDeviceTable *pBoard)
{
	int i;
	int nBoardID=-1;

	/* see if this board has been used (although its reference count may be zero) */
	for(i=0; i<MAX_BOARDS; i++)
	{
		if(gBoardRef[i].DeviceID==pBoard->DeviceID)
		{
			return gBoardRef[i].nRefCount;
			break;
		}
	}

	/* board wasnt found - so it hasnt been used at all in this session */
	return 0;

}

/**********************************************************************/

static DEVICEID GetDeviceID(HDEVICE hDeviceID)
{
	DEVICEID DeviceID;
	BoardDataBlock BoardData;

	PVROSGetPCIDeviceInfo (hDeviceID, &BoardData);
	
	/* I'm fudging together a DeviceID as there is no other easy access */
	DeviceID = (void *) ((BoardData.PCIBus << 8) | 
						 (BoardData.PCIDev << 3) | 
						 BoardData.PCIFunc);

	return DeviceID;
}

/**********************************************************************/

extern sgl_bool gBogusPCX1;

static DEVICE_TYPE GetDeviceType(HDEVICE hDeviceID)
{
	BoardDataBlock BoardData;

	PVROSGetPCIDeviceInfo (hDeviceID, &BoardData);

	if(gBogusPCX1 && BoardData.DeviceType!=MIDAS4)
	{
		/* set the IEEE hardware reg to FPU OFF */

		DPF((DBG_WARNING,"Forcing PCX2 to run in emulation mode"));

		((sgl_uint32 *) (BoardData.LinearMemWindows[0]))[0x18] = 0;
		BoardData.DeviceType = MIDAS4;
	}
	return BoardData.DeviceType;
}

/**********************************************************************/

#if USE_VIRTUAL_BUFFERS

BUFFER_LIST *PVROSGetSecretBuffer(HDEVICE hDeviceID)
{
	DEVICEID DeviceID;
	DEVICE_TYPE DeviceType;
	int i;

	DeviceID = GetDeviceID(hDeviceID);

	/* see if this board has been used - 
	** if it hasnt this is an error 
	*/
	for(i=0; i<MAX_BOARDS; i++)
	{
		if(gBoardRef[i].DeviceID == DeviceID)
		{
			return(gBoardRef[i].SecretBuffer);
			break;
		}
	}

	PVROSPrintf("Tried to get a secret buffer on a card without one!!!\n");
	return NULL;

}

#endif

/**********************************************************************/

BOARDREF *GetBoardRef(HDEVICE hDeviceID)
{
	DEVICEID DeviceID;
	int i;

	DeviceID = GetDeviceID(hDeviceID);

	/* see if this board has been used - 
	** if it hasnt this is an error 
	*/
	for(i=0; i<MAX_BOARDS; i++)
	{
		if(gBoardRef[i].DeviceID == DeviceID)
		{
			return(gBoardRef + i);
		}
	}

	return NULL;

}

/**********************************************************************/

void CALL_CONV PVROSDestroyPhysicalDevice(HDEVICE hDeviceID)
{
	HANDLE		hVxD = OpenVxd();
	HANDLE		hBufferMutex = CreateMutex(NULL,FALSE,"VirtualBufferMutex");

	DEVICEID DeviceID;
	DEVICE_TYPE DeviceType;
	int i;
	int nBoardID=-1;

	DeviceID = GetDeviceID(hDeviceID);
	DeviceType = GetDeviceType(hDeviceID);

	/* see if this board has been used - 
	** if it hasnt this is an error 
	*/
	for(i=0; i<MAX_BOARDS; i++)
	{
		if(gBoardRef[i].DeviceID == DeviceID)
		{
			nBoardID=i;
			break;
		}
	}

	/* found a board, decrement its reference count and close it
	** if the refcount reaches zero
	*/
	if(nBoardID >= 0)
	{
		ASSERT(gBoardRef[nBoardID].nRefCount);
		ASSERT(gBoardRef[nBoardID].ghDeviceID == hDeviceID);

		gBoardRef[nBoardID].nRefCount--;

		if (gBoardRef[nBoardID].nRefCount <= 0)
		{
			PVROSPrintf ("Closing a physical board\n");
			PVROSHookIRQ (FALSE, DISABLE_BOARD, hDeviceID);
			PVROSDestroyAVirtualBuffer (DeviceType,hVxD,hBufferMutex);
			PVROSClosePCIDevice (hDeviceID);
			gBoardRef[nBoardID].ghDeviceID = NULL;

		#if USE_VIRTUAL_BUFFERS

			if(	gBoardRef[nBoardID].SecretBuffer != NULL)
			{
				if(DeviceType==MIDAS4)
				{
					PVROSDestroyDMABuffer (hVxD,gBoardRef[nBoardID].SecretBuffer->pDMAB);
				}

				PVROSDestroyDMAScatterBuffer (hVxD,gBoardRef[nBoardID].SecretBuffer->pDMASB);
				
				/* destroy the texas memory as its always software */
#if SHARED_HEAP
				PVROSFree(gBoardRef[nBoardID].SecretBuffer->PVRParamBuf[PVR_PARAM_TYPE_TSP].pBuffer);
#else
				PVROSTSPFree(hVxD, gBoardRef[nBoardID].SecretBuffer->PVRParamBuf[PVR_PARAM_TYPE_TSP].pBuffer);
#endif
				PVROSFree(gBoardRef[nBoardID].SecretBuffer);
				gBoardRef[nBoardID].SecretBuffer = NULL;
			}
		#endif

		}
	}
	else
	{
		PVROSPrintf ("Trying to close an un-opened physical board\n");
		ASSERT(nBoardID>0);
	}

	CloseHandle(hBufferMutex);
	CloseVxd(hVxD);
}

/**********************************************************************/

HDEVICE CALL_CONV PVROSCreatePhysicalDevice(PciDeviceTable *pBoard)
{
	int i;
	int nBoardID=-1;

	/* see if this board has been used (although its reference count may be zero) */
	for(i=0; i<MAX_BOARDS; i++)
	{
		if(gBoardRef[i].DeviceID==pBoard->DeviceID)
		{
			nBoardID=i;
			break;
		}
	}

	/* if this board hasnt been mentioned before, find a free slot */
	if(nBoardID==-1)
	{
		for(i=0; i<MAX_BOARDS; i++)
		{
			if(gBoardRef[i].DeviceID==0)
			{
				gBoardRef[i].DeviceID=pBoard->DeviceID;
				nBoardID=i;
				break;
			}
		}
	}

	if (gBoardRef[nBoardID].nRefCount == 0)
	{
		BUFFER_LIST		*pBuffer  = NULL;
		BoardDataBlock	pb;
		
		gBoardRef[nBoardID].ghDeviceID = PVROSOpenPCIDevice(pBoard->DeviceID);

		/* Need to get the PCI device info to see what device we are using.
		 */
		PVROSGetPCIDeviceInfo (gBoardRef[nBoardID].ghDeviceID, &pb);

		/* Initialise PCI stuff such as latency and cache line size.
		 */
		PVROSInitPCIAddresses (gBoardRef[nBoardID].ghDeviceID, pb.PCIDeviceID);

		gBoardRef[nBoardID].nLogicalDevices = 0;
		gBoardRef[nBoardID].LDeviceList = NULL;
	}

	gBoardRef[nBoardID].nRefCount++;

	return gBoardRef[nBoardID].ghDeviceID;
}

/************************************************************************/

void PVROSDestroyAVirtualBuffer(DEVICE_TYPE DeviceType,HANDLE hVxD,HANDLE hBufferMutex)
{
	/* Always destroy the top unused buffer as this was the last created
	** so is probably the crapiest
	*/
	BUFFER_LIST *pBuffer, *pKillBuffer;

	DPF((DBG_MESSAGE,"Finding a virtual buffer to destroy ..."));

	pKillBuffer = FindUnusedBuffer(DeviceType,hBufferMutex);

	if(pKillBuffer)
	{
		if (DeviceType == MIDAS4)
		{
			PVROSDestroyDMABuffer (hVxD,pKillBuffer->pDMAB);
		}

		PVROSDestroyDMAScatterBuffer (hVxD,pKillBuffer->pDMASB);

		/* destroy the texas memory as its always software */
#if SHARED_HEAP
		PVROSFree(pKillBuffer->PVRParamBuf[PVR_PARAM_TYPE_TSP].pBuffer);
#else
		PVROSTSPFree(hVxD,pKillBuffer->PVRParamBuf[PVR_PARAM_TYPE_TSP].pBuffer);
#endif

		pBuffer = RealVirtualBuffer;

		if (pKillBuffer == pBuffer)
		{
			RealVirtualBuffer = pKillBuffer->Next;
			if(RealVirtualBuffer == NULL)
			{
				DPF((DBG_WARNING,"All virtual buffers destroyed"));
			}
		}
		else
		{
			do
			{
				/* walk the buffer list to stitch it together */
				if(pBuffer->Next == pKillBuffer)
				{
					pBuffer->Next = pKillBuffer->Next;
					break;
				}

			}while(pBuffer = pBuffer->Next);
		}
				
		ASSERT(pBuffer);
		PVROSFree(pKillBuffer);
		pKillBuffer = NULL;
		RealBufferCount--;

	}
	else
	{
		DPF((DBG_WARNING,"Went to destroy a virtual buffer ... but none are free"));
	}
}

/**********************************************************************/

void PVROSDestroyAllVirtualBuffers()
{
	HANDLE	hVxD = OpenVxd();

	/* this function forcibly destroys all virtual buffers */

	/* Always destroy the top unused buffer as this was the last created
	** so is probably the crapiest
	*/
	BUFFER_LIST *pKillBuffer;

	DPF((DBG_MESSAGE,"Destroying all virtual buffers ..."));

	while(pKillBuffer = RealVirtualBuffer)
	{
		if(pKillBuffer->DeviceType==MIDAS4)
		{
			PVROSDestroyDMABuffer (hVxD,pKillBuffer->pDMAB);
		}

		PVROSDestroyDMAScatterBuffer (hVxD,pKillBuffer->pDMASB);

		/* destroy the texas memory as its always software */
#if SHARED_HEAP
		PVROSFree(pKillBuffer->PVRParamBuf[PVR_PARAM_TYPE_TSP].pBuffer);
#else
		PVROSTSPFree(hVxD,pKillBuffer->PVRParamBuf[PVR_PARAM_TYPE_TSP].pBuffer);
#endif

		RealVirtualBuffer = pKillBuffer->Next;
		PVROSFree(pKillBuffer);

		RealBufferCount--;
		DPF((DBG_MESSAGE,"Destroyed a real virtual buffer, %d left", RealBufferCount));

	}

	while(pKillBuffer = VirtualVirtualBuffer)
	{

		/* destroy the texas memory as its always software */
		PVROSFree(pKillBuffer->PVRParamBuf[PVR_PARAM_TYPE_ISP].pBuffer);
#if SHARED_HEAP
		PVROSFree(pKillBuffer->PVRParamBuf[PVR_PARAM_TYPE_TSP].pBuffer);
#else
		PVROSTSPFree(hVxD,pKillBuffer->PVRParamBuf[PVR_PARAM_TYPE_TSP].pBuffer);
#endif
		PVROSFree(pKillBuffer->PVRParamBuf[PVR_PARAM_TYPE_REGION].pBuffer);

		VirtualVirtualBuffer = pKillBuffer->Next;
		PVROSFree(pKillBuffer);

		VirtualBufferCount--;
		PVROSPrintf("Destroyed a virtual virtual buffer, %d left\n", VirtualBufferCount);

	}

	CloseVxd(hVxD);
}

/**********************************************************************/

PVROSERR CreateNewPCXBuffer(HANDLE		hVxD,
							DEVICE_TYPE	DeviceType,
							BUFFER_LIST *pBuffer, 
							sgl_uint32 	PlaneMemBytes,
							sgl_uint32 	ObjectMemBytes)
{
	/* fill out the PCX buffer if we can */
	/* find where windows booted from, and so where the
	** system.ini file is located
	*/
	sgl_uint32 dwBytesInBuffer;/* 1024 * 1024 * 1; 1Meg for planes */

	DPF((DBG_MESSAGE,"Creating new PCXBuffer and DMA Scatter Buffer"));

	dwBytesInBuffer = PlaneMemBytes; 
									 
	if ((DeviceType == MIDAS5) || (DeviceType == MIDAS5_003))
	{
		dwBytesInBuffer += ObjectMemBytes;
	}

	pBuffer->pDMASB = PVROSCreateDMAScatterBuffer (hVxD,dwBytesInBuffer);

	if(!pBuffer->pDMASB)
	{
		DPF((DBG_ERROR,"Couldn't create DMA Scatter Buffer"));
		return PVROS_DODGY;
	}

	if (PVROS_DODGY == DMASBToPCXTLB (pBuffer->pDMASB, &(pBuffer->PCXBuffer), 
									  dwBytesInBuffer))
	{
		DPF((DBG_ERROR,"Couldn't get get isp parameter space"));
		PVROSDestroyDMAScatterBuffer (hVxD,pBuffer->pDMASB);
		return PVROS_DODGY;
	}

}

static int CountBuffers (DEVICE_TYPE DeviceType)
{
	BUFFER_LIST *pBuffer = RealVirtualBuffer;
	int count=0;

	while(pBuffer)
	{
		if (count >= RealBufferCount)
		{
			DPF((DBG_ERROR, "More buffers in list than expected: %d vs %d", count, RealBufferCount));
		}
		
		if(pBuffer->DeviceType == DeviceType)
		{
			count++;
		}

		pBuffer = pBuffer->Next;
	}

	return (count);
}

/**********************************************************************/

static void MakeAllBuffersUnused (DEVICE_TYPE DeviceType)
{
	BUFFER_LIST *pBuffer = RealVirtualBuffer;
	HANDLE		hBufferMutex = CreateMutex(NULL,FALSE,"VirtualBufferMutex");


	while(pBuffer)
	{
		if(pBuffer->bInUse)
		{
			DPF((DBG_WARNING, "Releasing buffer, which should be free!!"));
			if(GetVirtualBufferMutex(hBufferMutex))
			{
				/* mark it as unused */
				pBuffer->bInUse = FALSE;
				pBuffer->hLogicalDev = NULL;
				ReleaseVirtualBufferMutex(hBufferMutex);
			}
			else
			{
				DPF((DBG_ERROR,"Couldn't get virtual buffer mutex"));
			}

		}

		pBuffer = pBuffer->Next;
	}

	CloseHandle(hBufferMutex);
}

/**********************************************************************/

static void UnUseLDeviceBuffers (HLDEVICE hLogicalDev)
{
	BUFFER_LIST *pBuffer = RealVirtualBuffer;

	while(pBuffer)
	{
		if(pBuffer->bInUse && (pBuffer->hLogicalDev == hLogicalDev))
		{
			DPF((DBG_WARNING, "Releasing logical device's buffer!\n"));

			if(GetVirtualBufferMutex(hLogicalDev->hBufferMutex))
			{
				/* mark it as unused */
				pBuffer->bInUse = FALSE;
				pBuffer->hLogicalDev = NULL;
				ReleaseVirtualBufferMutex(hLogicalDev->hBufferMutex);
			}
			else
			{
				DPF((DBG_ERROR,"Couldn't get virtual buffer mutex"));
			}

		}

		pBuffer = pBuffer->Next;
	}
}


/**********************************************************************/

void CALL_CONV PVROSFreeVirtualBuffers(HLDEVICE hLogicalDev)
{
	UnUseLDeviceBuffers (hLogicalDev);
}

/**********************************************************************/

BUFFER_LIST *FindUnusedBuffer(DEVICE_TYPE DeviceType,HANDLE hBufferMutex)
{
	BUFFER_LIST *pBuffer = RealVirtualBuffer;
	int count=0;

	if(!pBuffer)
	{
		DPF((DBG_MESSAGE,"Buffer List unitialised"));
		/* probably not yet initialised - return and it'll be done */
		return pBuffer;
	}

	if (GetVirtualBufferMutex(hBufferMutex))
	{
		while(pBuffer)
		{
			if(count>=RealBufferCount)
			{
				DPF((DBG_ERROR, "More buffers in list than expected: %d vs %d\n", 
					 count, RealBufferCount));
			}
			

			if(!(pBuffer->bInUse) && pBuffer->DeviceType == DeviceType)
			{
				/* found a free buffer */
			
				/* Mark it used immediately 
				** - hope we don't get race conditions here 
				*/
				pBuffer->bInUse = TRUE;
				ReleaseVirtualBufferMutex(hBufferMutex);

				/* the buffer initial pointers should not have moved ... */
				
				/* ... and I'm not sure I even have to do this */
				pBuffer->PVRParamBuf[PVR_PARAM_TYPE_REGION].uBufferPos = 0;
				if (DeviceType == MIDAS4)
				{
					pBuffer->PVRParamBuf[PVR_PARAM_TYPE_TSP].uBufferPos = PCX1_TSP_BASE_POS;
				}
				else
				{
					pBuffer->PVRParamBuf[PVR_PARAM_TYPE_TSP].uBufferPos = PCX2_TSP_BASE_POS;
				}
				pBuffer->PVRParamBuf[PVR_PARAM_TYPE_ISP].uBufferPos = 0;

				DPF((DBG_MESSAGE, "Got Buffer %d\n", count));

				return pBuffer;
			}
			pBuffer = pBuffer->Next;
			count++;
		}

		ReleaseVirtualBufferMutex(hBufferMutex);
	}
	else
	{
		DPF((DBG_ERROR,"Couldnt get Mutex"));
	}

	/* fall thru - return the null buffer pointer */
	DPF((DBG_MESSAGE,"Couldn't find an unused buffer - current total %d",RealBufferCount));

	return pBuffer;

}

/**********************************************************************/

BUFFER_LIST *FindUnusedVirtualBuffer(DEVICE_TYPE DeviceType,HANDLE hBufferMutex)
{
	BUFFER_LIST *pBuffer = VirtualVirtualBuffer;
	int count=0;

	if(!pBuffer)
	{
		PVROSPrintf("Virtual Buffer List unitialised\n");
		/* probably not yet initialised - return and it'll be done */
		return pBuffer;
	}

	if(GetVirtualBufferMutex(hBufferMutex))
	{

		while(pBuffer)
		{
			ASSERT(count<VirtualBufferCount);
			PVROSPrintf("Trying Buffer %d\n", count);
			if(!(pBuffer->bInUse) && pBuffer->DeviceType == DeviceType)
			{
				/* found a free buffer */
				
				/* Mark it used immediately 
				** - hope we don't get race conditions here 
				*/
				pBuffer->bInUse = TRUE;
				ReleaseVirtualBufferMutex(hBufferMutex);

				/* the buffer initial pointers should not have moved ... */
				
				/* ... and I'm not sure I even have to do this */
				pBuffer->PVRParamBuf[PVR_PARAM_TYPE_REGION].uBufferPos = 0;
				if (DeviceType == MIDAS4)
				{
					pBuffer->PVRParamBuf[PVR_PARAM_TYPE_TSP].uBufferPos = PCX1_TSP_BASE_POS;
				}
				else
				{
					pBuffer->PVRParamBuf[PVR_PARAM_TYPE_TSP].uBufferPos = PCX2_TSP_BASE_POS;
				}
				pBuffer->PVRParamBuf[PVR_PARAM_TYPE_ISP].uBufferPos = 0;
				
				PVROSPrintf("Got Buffer %d\n", count);
				return pBuffer;
			}
			pBuffer = pBuffer->Next;
			count++;
			
		}
		PVROSPrintf("No free buffers - need a new one\n");
		ReleaseVirtualBufferMutex(hBufferMutex);

	}
	else
	{
		PVROSPrintf("Couldnt get Mutex\n");
	}


	/* fall thru - return the null buffer pointer */
	return pBuffer;

}

/**********************************************************************/

BUFFER_LIST *AddNewBufferPCX1(HANDLE		hVxD,
							  DEVICE_TYPE	DeviceType,
							  sgl_uint32 	PlaneMemBytes, 
							  sgl_uint32 	ObjectMemBytes,
							  sgl_uint32 	TexasMemBytes)
{
	/* add a new one to the head of the list */
	PVROSERR err;
	sgl_uint32 TLBSize;
	BUFFER_LIST *pNewBuffer = NULL;
	sgl_uint32 *pISPObjectData = 0;

	void *pNew;

	pNewBuffer = PVROSMalloc(sizeof(BUFFER_LIST));
	if(!pNewBuffer)
	{
		return NULL;
	}
	/* mark it as used immediatly - no need to mutex as it isnt on the list */
	pNewBuffer->bInUse = TRUE;
	pNewBuffer->DeviceType = MIDAS4;
	pNewBuffer->BufferType = REAL_BUFFER;

	/* Allocate the guts of the buffer i.e. the memory */

	/* TSP Param memory */
#if SHARED_HEAP
	pNew = PVROSMalloc (TexasMemBytes);
#else
	pNew = PVROSTSPAlloc (hVxD,TexasMemBytes);
#endif

	if(!pNew)
	{
		PVROSFree(pNewBuffer);
		return NULL;
	}

   	pNewBuffer->PVRParamBuf[PVR_PARAM_TYPE_TSP].pBuffer			= pNew;
	
	/* Used when problem with mask plane in hardware. Index greater since
	 * full textured plane required to slow down FIFO in hardware.
	 *
	 * Increment StartIndex from 12 to 14. Eventhough this wastes a TSP tag it
	 * prevents the flat shaded highlight problem.
	 */
   	
	pNewBuffer->PVRParamBuf[PVR_PARAM_TYPE_TSP].uBufferPos		= PCX1_TSP_BASE_POS; /* in PCX2 Tag ID 1 & 2 reserved */
   	pNewBuffer->PVRParamBuf[PVR_PARAM_TYPE_TSP].uBufferLimit	= TexasMemBytes>>2;


	/* this create the DMA TLB stuff for planes and object pointers */
	err = CreateNewPCXBuffer(hVxD,DeviceType,pNewBuffer,PlaneMemBytes, 0);

	if(err==PVROS_DODGY)
	{
		/* Few TSP memory since it is in software.
		 */
#if SHARED_HEAP
		PVROSFree(pNewBuffer->PVRParamBuf[PVR_PARAM_TYPE_TSP].pBuffer);
#else
		PVROSTSPFree(hVxD,pNewBuffer->PVRParamBuf[PVR_PARAM_TYPE_TSP].pBuffer);
#endif
		PVROSFree(pNewBuffer);
		return NULL;
	}

	TLBSize = pNewBuffer->PCXBuffer.SlotsUsed * 
		(1024 << pNewBuffer->PCXBuffer.PageSize)<<2;


	if(TLBSize < PlaneMemBytes)
	{
		DPF((DBG_WARNING,"Got a smaller than requested TLB buffer"));
	}


	pNewBuffer->PVRParamBuf[PVR_PARAM_TYPE_ISP].pBuffer		= 
		(unsigned long *) pNewBuffer->PCXBuffer.LinearAddress;
	pNewBuffer->PVRParamBuf[PVR_PARAM_TYPE_ISP].uBufferPos	= 0;
	pNewBuffer->PVRParamBuf[PVR_PARAM_TYPE_ISP].uBufferLimit= TLBSize >> 2;


	/* Now have to get the object pointer buffers - this is either
	** contiguous memory (Buffer1 from the VXD) or virtual and
	** requiring copied 
	*/

	pNewBuffer->pDMAB = PVROSCreateDMABuffer(hVxD,ObjectMemBytes);

	if (!pNewBuffer->pDMAB)
	{
		/*
		** Try reducing allocation size until we manage the allocation.
		*/
		while (!pNewBuffer->pDMAB && (ObjectMemBytes > (1024*128)))
		{
			ObjectMemBytes -= 16*1024;
			pNewBuffer->pDMAB = PVROSCreateDMABuffer(hVxD,ObjectMemBytes);
		}

		if (!pNewBuffer->pDMAB)
		{
			/*
			**	Couldn't allocate a buffer, so free all other mem
			**	and exit.
			*/
			DPF((DBG_ERROR,"Couldn't get allocate shared block for ISPObjectPtrs"));

#if SHARED_HEAP
			PVROSFree(pNewBuffer->PVRParamBuf[PVR_PARAM_TYPE_TSP].pBuffer);
#else
			PVROSTSPFree(hVxD,pNewBuffer->PVRParamBuf[PVR_PARAM_TYPE_TSP].pBuffer);
#endif
			PVROSDestroyDMAScatterBuffer(hVxD,pNewBuffer->pDMASB);
			PVROSFree(pNewBuffer);

			return NULL;
		}

		DPF((DBG_WARNING,"Allocated reduced block for ISPObjectPtrs, size = %d bytes",ObjectMemBytes));
	}

	physISPObjectData = pNewBuffer->pDMAB->PhysicalAddress;
	pISPObjectData = (sgl_uint32 *) pNewBuffer->pDMAB->GlobalAlias;

	pNewBuffer->PVRParamBuf[PVR_PARAM_TYPE_REGION].pBuffer 		= pISPObjectData;
	pNewBuffer->PVRParamBuf[PVR_PARAM_TYPE_REGION].uBufferPos	= 0;
	pNewBuffer->PVRParamBuf[PVR_PARAM_TYPE_REGION].uBufferLimit = ObjectMemBytes>>2;

	return pNewBuffer;

}

/**********************************************************************/

BUFFER_LIST *AddNewBufferPCX2(HANDLE		hVxD,
							  DEVICE_TYPE	DeviceType,
							  sgl_uint32 	PlaneMemBytes, 
							  sgl_uint32 	ObjectMemBytes,
							  sgl_uint32 	TexasMemBytes)
{
	/* add a new one to the head of the list */
	BUFFER_LIST *pNewBuffer = NULL;
	void *pNew;
	PVROSERR err;
	sgl_uint32	TLBSize, TLBSplit, *pTLB;


	pNewBuffer = PVROSMalloc(sizeof(BUFFER_LIST));
	if(!pNewBuffer)
	{
		return NULL;
	}
	/* mark it as used immediatly ... ditto above re. mutex*/
	pNewBuffer->bInUse = TRUE;
	pNewBuffer->DeviceType = DeviceType;
	pNewBuffer->BufferType = REAL_BUFFER;

	/* Allocate the guts of the buffer i.e. the memory */

	/* TSP Param memory */
#if SHARED_HEAP
	pNew = PVROSMalloc (TexasMemBytes);
#else
	pNew = PVROSTSPAlloc (hVxD,TexasMemBytes);
#endif
	if(!pNew)
	{
		PVROSFree(pNewBuffer);
		return NULL;
	}

   	pNewBuffer->PVRParamBuf[PVR_PARAM_TYPE_TSP].pBuffer			= pNew;
	
	/* Used when problem with mask plane in hardware. Index greater since
	 * full textured plane required to slow down FIFO in hardware.
	 *
	 * Increment StartIndex from 12 to 14. Eventhough this wastes a TSP tag it
	 * prevents the flat shaded highlight problem.
	 */
   	
	pNewBuffer->PVRParamBuf[PVR_PARAM_TYPE_TSP].uBufferPos		= PCX2_TSP_BASE_POS; /* in PCX2 Tag ID 1 & 2 reserved */
   	pNewBuffer->PVRParamBuf[PVR_PARAM_TYPE_TSP].uBufferLimit	= TexasMemBytes>>2;


	/* this create the DMA TLB stuff for planes and object pointers */
	err = CreateNewPCXBuffer(hVxD,DeviceType,pNewBuffer,PlaneMemBytes,ObjectMemBytes);

	if(err==PVROS_DODGY)
	{
#if SHARED_HEAP
		PVROSFree(pNewBuffer->PVRParamBuf[PVR_PARAM_TYPE_TSP].pBuffer);
#else
		PVROSTSPFree(hVxD, pNewBuffer->PVRParamBuf[PVR_PARAM_TYPE_TSP].pBuffer);
#endif
		PVROSFree(pNewBuffer);
		return NULL;
	}

	TLBSize = pNewBuffer->PCXBuffer.SlotsUsed * 
		(1024 << pNewBuffer->PCXBuffer.PageSize)<<2;

	if(TLBSize < (PlaneMemBytes+ObjectMemBytes))
	{
		DPF((DBG_WARNING,"Got a smaller than requested TLB buffer: TLBSize=%d, Request=%d",
					TLBSize, (PlaneMemBytes+ObjectMemBytes)));
		/* keep more back for the objects -- sacrifice planes
		** this is a arbitrary decision by me!!
		*/
		TLBSplit = (TLBSize - ObjectMemBytes)>>2;
	}
	else
	{
		TLBSplit = PlaneMemBytes>>2;
	}

	pTLB = (unsigned long *) pNewBuffer->PCXBuffer.LinearAddress;

	pNewBuffer->PVRParamBuf[PVR_PARAM_TYPE_ISP].pBuffer		 	= pTLB;
	pNewBuffer->PVRParamBuf[PVR_PARAM_TYPE_ISP].uBufferPos	 	= 0;
	pNewBuffer->PVRParamBuf[PVR_PARAM_TYPE_ISP].uBufferLimit 	= TLBSplit - 1;

	pNewBuffer->PVRParamBuf[PVR_PARAM_TYPE_REGION].pBuffer 	 	= pTLB + TLBSplit;
	pNewBuffer->PVRParamBuf[PVR_PARAM_TYPE_REGION].uBufferPos	= 0;
	pNewBuffer->PVRParamBuf[PVR_PARAM_TYPE_REGION].uBufferLimit = ObjectMemBytes>>2;

	return pNewBuffer;
}

/**********************************************************************/

BUFFER_LIST *CreateVirtualBuffer(	HLDEVICE hLogicalDev,
									sgl_uint32 PlaneMemBytes, 
									sgl_uint32 ObjectMemBytes,
									sgl_uint32 TexasMemBytes)
{
	/* add a new one to the head of the list */
	BUFFER_LIST *pNewBuffer = NULL;
	void *pNew;

	pNewBuffer = PVROSMalloc(sizeof(BUFFER_LIST));
	if(!pNewBuffer)
	{
		return NULL;
	}
	/* mark it as used immediatly */
	pNewBuffer->bInUse = TRUE;

	/* Allocate the guts of the buffer i.e. the memory */

	/* TSP Param memory */
#if SHARED_HEAP
	pNew = PVROSMalloc (TexasMemBytes);
#else
	pNew = PVROSTSPAlloc (hLogicalDev->hVxD,TexasMemBytes);
#endif
	if(!pNew)
	{
		PVROSFree(pNewBuffer);
		return NULL;
	}

	/* Initialise the TSP buffer - PCX2 default values */
   	pNewBuffer->PVRParamBuf[PVR_PARAM_TYPE_TSP].pBuffer			= pNew;
	pNewBuffer->PVRParamBuf[PVR_PARAM_TYPE_TSP].uBufferPos 		= PCX2_TSP_BASE_POS;
   	pNewBuffer->PVRParamBuf[PVR_PARAM_TYPE_TSP].uBufferLimit	= TexasMemBytes>>2;

	/* this create the DMA TLB stuff for planes and object pointers */

	pNew = PVROSMalloc (PlaneMemBytes);
	if(!pNew)
	{
#if SHARED_HEAP
		PVROSFree(pNewBuffer->PVRParamBuf[PVR_PARAM_TYPE_TSP].pBuffer);
#else
		PVROSTSPFree(hLogicalDev->hVxD, pNewBuffer->PVRParamBuf[PVR_PARAM_TYPE_TSP].pBuffer);
#endif
		PVROSFree(pNewBuffer);
		return NULL;
	}

	pNewBuffer->PVRParamBuf[PVR_PARAM_TYPE_ISP].pBuffer		 	= pNew;
	pNewBuffer->PVRParamBuf[PVR_PARAM_TYPE_ISP].uBufferPos	 	= 0;
	pNewBuffer->PVRParamBuf[PVR_PARAM_TYPE_ISP].uBufferLimit 	= PlaneMemBytes>>2;


	pNew = PVROSMalloc (ObjectMemBytes);
	if(!pNew)
	{
#if SHARED_HEAP
		PVROSFree(pNewBuffer->PVRParamBuf[PVR_PARAM_TYPE_TSP].pBuffer);
#else
		PVROSTSPFree(hLogicalDev->hVxD,pNewBuffer->PVRParamBuf[PVR_PARAM_TYPE_TSP].pBuffer);
#endif
		PVROSFree(pNewBuffer->PVRParamBuf[PVR_PARAM_TYPE_ISP].pBuffer);
		PVROSFree(pNewBuffer);
		return NULL;
	}

	pNewBuffer->PVRParamBuf[PVR_PARAM_TYPE_REGION].pBuffer 	 	= pNew;
	pNewBuffer->PVRParamBuf[PVR_PARAM_TYPE_REGION].uBufferPos	= 0;
	pNewBuffer->PVRParamBuf[PVR_PARAM_TYPE_REGION].uBufferLimit = ObjectMemBytes>>2;

	/* Lastly if we've sucessfully done everything else fix up the list pointers */ 
	return pNewBuffer;
}

/**********************************************************************/

BUFFER_LIST *AddNewBuffer(HLDEVICE hLogicalDev,sgl_bool AttachToList)
{
	BUFFER_LIST *pBuffer = NULL;

	switch(hLogicalDev->DeviceType)
	{
		case(MIDAS4):
		{
			pBuffer = AddNewBufferPCX1(hLogicalDev->hVxD,
									   hLogicalDev->DeviceType,
									   hLogicalDev->PlaneMemBytes, 
									   hLogicalDev->ObjectMemBytes,
									   hLogicalDev->TexasMemBytes);
#define USE_PCX2_AS_PCX1 0
#if USE_PCX2_AS_PCX1
			if(!pBuffer)
			{
				pBuffer = FindUnusedBuffer(hLogicalDev->DeviceType, hLogicalDev->hBufferMutex);
				if(pBuffer)
				{
					pBuffer->BufferType = PCX2_AS_PCX1_BUFFER;
				}
				else
				{
					pBuffer = AddNewBufferPCX2(hLogicalDev->hVxD,
									   		   hLogicalDev->DeviceType,
									   		   hLogicalDev->PlaneMemBytes, 
											   hLogicalDev->ObjectMemBytes,
											   hLogicalDev->TexasMemBytes);
					if(pBuffer)
					{
						pBuffer->BufferType = PCX2_AS_PCX1_BUFFER;
					}
				}
			}
#endif
			break;
		}
		case(MIDAS5):
		case(MIDAS5_003):
		{
			pBuffer = AddNewBufferPCX2(hLogicalDev->hVxD,
									   hLogicalDev->DeviceType,
									   hLogicalDev->PlaneMemBytes, 
									   hLogicalDev->ObjectMemBytes,
									   hLogicalDev->TexasMemBytes);
			break;
		}
		default:
		{
			PVROSPrintf("Cannot assign a buffer for an unknown device\n");
		}
	}

	/* Lastly if we've sucessfully done everything else fix up the list pointers */ 
	if(pBuffer && AttachToList)
	{
		pBuffer->Next = RealVirtualBuffer;
		RealVirtualBuffer = pBuffer;
		RealBufferCount++;
	}

	return pBuffer;
}

/**********************************************************************/

BUFFER_LIST *AddNewVirtualBuffer(HLDEVICE Device)
{
	BUFFER_LIST *pBuffer = NULL;

	pBuffer = CreateVirtualBuffer(	Device,
									Device->PlaneMemBytes, 
								  	Device->ObjectMemBytes,
								  	Device->TexasMemBytes);
	if(pBuffer)
	{
		pBuffer->BufferType = VIRTUAL_BUFFER;
		pBuffer->DeviceType = Device->DeviceType;
		pBuffer->Next = VirtualVirtualBuffer;
		VirtualVirtualBuffer = pBuffer;
		VirtualBufferCount++;
	}

	return pBuffer;
}

/**********************************************************************/

sgl_uint32 RendersPendingOnBoard(HLDEVICE hLogicalDev)
{
	/* walk the Log Device list for the board and count how many have 
	** pending renders
	*/
	sgl_uint32 Pending = 0;
	BOARDREF   *pBoardRef;
	LIST       *pLDevList;

	pBoardRef = GetBoardRef(hLogicalDev->hDeviceID);

	pLDevList = pBoardRef->LDeviceList;
	ASSERT(pLDevList);

	while(pLDevList)
	{
		if(pLDevList->hLogicalDev->RenderStatus != PVR_STATUS_EOR)
		{
			Pending++;
		}
		pLDevList = pLDevList->Next;
	}


	return Pending;

}

/**********************************************************************/

PVROSERR PVROSAssignVirtualBuffers(PVR_PARAM_BUFF *pPVRParamBuf, 
								   HLDEVICE hLogicalDev)
{
	BUFFER_LIST *pBuffer;
	sgl_uint32 Pending;
	sgl_int32 TSPHighWaterMark;

	hLogicalDev->VirtualBuffer = NULL;

	pBuffer = FindUnusedBuffer(hLogicalDev->DeviceType,hLogicalDev->hBufferMutex);

	if(pBuffer == NULL)
	{
		Pending = RendersPendingOnBoard(hLogicalDev);
		if(Pending==0)
		{
			DPF((DBG_WARNING,"PVROSAssignVirtualBuffers: No renders pending, No buffers Avail"));
		}
	}

	while (pBuffer == NULL)
	{
		Pending = RendersPendingOnBoard(hLogicalDev);
		if(Pending==0)
		{
			DPF((DBG_WARNING,"PVROSAssignVirtualBuffers: No renders pending, No buffers Avail"));
			break;
		}
		DPF((DBG_MESSAGE,"PVROSAssignVirtualBuffers: going to sleep"));
		Sleep (1);
	
		pBuffer = FindUnusedBuffer(hLogicalDev->DeviceType,hLogicalDev->hBufferMutex);
	}

	if(pBuffer == NULL)
	{
		DPF((DBG_WARNING,"No free buffers - attempting to create a new physical one"));

		pBuffer = AddNewBuffer(hLogicalDev, TRUE);
  	}


#if USE_VIRTUAL_BUFFERS

	if(pBuffer == NULL)
	{
		/* couldn't get any sort of physical buffer - try a virtual one */
		pBuffer = FindUnusedVirtualBuffer(hLogicalDev->DeviceType,hLogicalDev->hBufferMutex);

		if(pBuffer == NULL)
		{							   /* Dodgy parameters? */
			pBuffer = AddNewVirtualBuffer(hLogicalDev->DeviceType,hLogicalDev->hBufferMutex);
		}

		/* failure at this point means there isn't any virtual memory */
		PVROSPrintf("Failed to assign a buffer\n");
		ASSERT(pBuffer);
		return PVROS_DODGY;
	}

#endif
	if(pBuffer==NULL)
	{
		return PVROS_DODGY;
	}

	pBuffer->hLogicalDev = hLogicalDev;

	pBuffer->DestAddr[PVR_PARAM_TYPE_ISP] = pBuffer->PVRParamBuf[PVR_PARAM_TYPE_ISP].pBuffer;
	pBuffer->DestAddr[PVR_PARAM_TYPE_TSP] = (sgl_uint32 *)hLogicalDev->BoardData.LinearMemWindows[1];
	pBuffer->DestAddr[PVR_PARAM_TYPE_REGION] = pBuffer->PVRParamBuf[PVR_PARAM_TYPE_REGION].pBuffer;

	/* if we got here we've got a valid set of buffers */
	/* fill out the pointers for return */
	hLogicalDev->VirtualBuffer = pBuffer;

	/* get the current setting for TSP params in texture memory */

	TSPHighWaterMark = PVROSGetTSPHighWaterMark (hLogicalDev->TexHeap);

	if ((TSPHighWaterMark >> 2) != (sgl_int32) pBuffer->PVRParamBuf[PVR_PARAM_TYPE_TSP].uBufferLimit)
	{
		void *pNew;
#if SHARED_HEAP
		pNew = PVROSRealloc (pBuffer->PVRParamBuf[PVR_PARAM_TYPE_TSP].pBuffer, TSPHighWaterMark);
#else
		pNew = PVROSTSPRealloc (hLogicalDev->hVxD, pBuffer->PVRParamBuf[PVR_PARAM_TYPE_TSP].pBuffer, TSPHighWaterMark);
#endif

		if (!pNew)
		{
			DPF((DBG_ERROR,"TSP buffer realloc failed!"));
		}
		else
		{
			pBuffer->PVRParamBuf[PVR_PARAM_TYPE_TSP].pBuffer = pNew;
			pBuffer->PVRParamBuf[PVR_PARAM_TYPE_TSP].uBufferLimit = TSPHighWaterMark>>2;
		}
	}

	pPVRParamBuf[PVR_PARAM_TYPE_ISP]  = pBuffer->PVRParamBuf[PVR_PARAM_TYPE_ISP];
	pPVRParamBuf[PVR_PARAM_TYPE_TSP]  = pBuffer->PVRParamBuf[PVR_PARAM_TYPE_TSP];

	pPVRParamBuf[PVR_PARAM_TYPE_REGION]  = pBuffer->PVRParamBuf[PVR_PARAM_TYPE_REGION];

	pPVRParamBuf[PVR_PARAM_TYPE_ISP].uBufferPos  	= 0;
	pPVRParamBuf[PVR_PARAM_TYPE_REGION].uBufferPos  = 0;

	if(hLogicalDev->DeviceType==MIDAS4)
	{
		pPVRParamBuf[PVR_PARAM_TYPE_TSP].uBufferPos  	= PCX1_TSP_BASE_POS;
		hLogicalDev->Registers[PCX_OBJECT_OFFSET] = pBuffer->pDMAB->PhysicalAddress;
	}
	else
	{
		pPVRParamBuf[PVR_PARAM_TYPE_TSP].uBufferPos  	= PCX2_TSP_BASE_POS;
		hLogicalDev->Registers[PCX_OBJECT_OFFSET] = 
			(int)pPVRParamBuf[PVR_PARAM_TYPE_REGION].pBuffer - (int)pPVRParamBuf[PVR_PARAM_TYPE_ISP].pBuffer
			| 0x00000001UL;
	}

	return PVROS_GROOVY;

}

/**********************************************************************/

HLDEVICE CALL_CONV PVROSCreateLogicalDevice(HDEVICE hDeviceID, void *TexHeap)
{
	HLDEVICE Device = NULL;
	int i;
	LIST        *pLDevList;
	BUFFER_LIST *pBuffer;
	BOARDREF	*pBoardRef;
	HANDLE		hVxD = OpenVxd();

	pBoardRef = GetBoardRef (hDeviceID);

	if (pBoardRef == NULL)
	{
		DPF((DBG_FATAL,"PVROSCreateLogicalDevice: bad physical device"));
	}
	else
	{
		pLDevList = PVROSMalloc(sizeof(LIST));
		if(pLDevList==NULL)
		{
				DPF((DBG_FATAL,"PVROSCreateLogicalDevice: out of memory for logical device list"));
		}
		else
		{
			pLDevList->Prev = NULL;
			pLDevList->Next = NULL;

			Device = PVROSMalloc(sizeof(LDEVICE));
			pLDevList->hLogicalDev = Device;

			if (Device == NULL)
			{
				PVROSPrintf("PVROSCreateLogicalDevice: out of memory for logical device\n");
			}
			else
			{
				Device->DeviceType = GetDeviceType(hDeviceID);

				/* initalise the device */
				Device->TexHeap = TexHeap;
				Device->RenderStatus = PVR_STATUS_EOR;
				/* vxd needs milli secs */
				Device->RenderTimeout = HWRdValFileUInt( "RenderTimeout", 5)*100; 
				Device->RendersPending = 0;
				Device->Buffers[0].pBuffer = NULL;
				Device->Buffers[1].pBuffer = NULL;
				Device->Buffers[2].pBuffer = NULL;
				Device->Buffers[0].uBufferPos = 0;
				Device->Buffers[1].uBufferPos = 0;
				Device->Buffers[2].uBufferPos = 0;
				Device->Buffers[0].uBufferLimit = 0;
				Device->Buffers[1].uBufferLimit = 0;
				Device->Buffers[2].uBufferLimit = 0;
				Device->hDeviceID = hDeviceID;
				Device->hVxD = hVxD;
				Device->dwPID = GetCurrentProcessId();

				Device->hBufferMutex = CreateMutex(NULL,     /* no security attributes*/
												   FALSE,    /* initially not owned*/
												   "VirtualBufferMutex");  /* name of mutex*/
				
				PVROSGetPCIDeviceInfo (hDeviceID, &Device->BoardData);

				for(i=0; i<32; i++)
				{
					Device->Registers[i] = 0;
				}
				
				/* create a set of buffers if we don't have enough */
				
				if (CountBuffers (Device->DeviceType) >= (pBoardRef->nLogicalDevices + 2))
				{
					DPF((DBG_MESSAGE,"PVROSCreateLogicalDevice: we have enough buffers (%d, %d)\n", 
								CountBuffers (Device->DeviceType), 
								pBoardRef->nLogicalDevices + 2));
				}
				else
				{
					DPF((DBG_MESSAGE,"PVROSCreateLogicalDevice: need new buffer (%d, %d)\n", 
								CountBuffers (Device->DeviceType), 
								pBoardRef->nLogicalDevices + 2));

					/* Read ISP,Object & TSP ParamSize from the registry */
					GetParamSettings(Device);

					switch(Device->DeviceType)
					{
						case(MIDAS4):
						case(MIDAS5):
						case(MIDAS5_003):
						{
							pBuffer = AddNewBuffer(Device,TRUE);

							break;
						}
						default:
						{
							PVROSPrintf("PVROSCreateLogicalDevice: bad device type\n");
							PVROSFree(Device);

							CloseVxd(hVxD);
							return (NULL);
						}
					}
					
#define TRY_VIRTUAL 0
#if TRY_VIRTUAL
					
					if(pBuffer == NULL)
					{
						switch(Device->DeviceType)
						{
							case(MIDAS4):
							case(MIDAS5):
							case(MIDAS5_003):
							{
								pBuffer = AddNewVirtualBuffer(Device);
								break;
							}
							default:
							{
								PVROSPrintf("Cannot assign a buffer for an unknown device\n");
							}
						}
					}
					
#endif

					if(pBuffer == NULL)
					{
						/* failed buffer creation destroy the logical device and return NULL */
						PVROSFree(pLDevList);
						PVROSFree(Device);
						Device = NULL;
					}
					else
					{
						if(GetVirtualBufferMutex(Device->hBufferMutex))
						{
							/* mark it as unused */
							pBuffer->bInUse = FALSE;
							ReleaseVirtualBufferMutex(Device->hBufferMutex);
						}
						else
						{
							PVROSPrintf("Couln't get virtual buffer mutex\n");
						}
					}
				}
			}
		}
	}

	if(Device && pLDevList)
	{
		pBoardRef->nLogicalDevices ++;

		pLDevList->Next = pBoardRef->LDeviceList;
		pBoardRef->LDeviceList = pLDevList;
		DPF((DBG_MESSAGE,"PVROSCreateLogicalDevice: %d Logical Devices Now Exist",
					pBoardRef->nLogicalDevices));
	}
	else
	{
		CloseVxd(hVxD);
	}

	return Device;

}

/**********************************************************************/

extern void ResetCallbacks(void* pLogDevice);


void CALL_CONV PVROSDestroyLogicalDevice(HLDEVICE hLogicalDev)
{
	BOARDREF	*pBoardRef;
	LIST *pLDevList, *pPrev;

	pBoardRef = GetBoardRef (hLogicalDev->hDeviceID);

	/* must be a valid board! */

	ASSERT (pBoardRef != NULL);

	if (pBoardRef)
	{
				
		/* free buffers that are currently marked used by this ldevice */

		UnUseLDeviceBuffers (hLogicalDev);

		/*
			JWH - Err, seemed like compare against pBoardRef->nLogicalDevices + 1 was incorrect, 
			removing +1 seems to fix virtual buffer leak...
		*/
		if (CountBuffers (GetDeviceType (hLogicalDev->hDeviceID)) <= pBoardRef->nLogicalDevices)
		{
			DPF((DBG_MESSAGE,"PVROSDestroyLogicalDevice: not destroying a buffer"));
		}
		else
		{
			/* wh#at about the virtual buffers ?! */
			DPF((DBG_MESSAGE,"PVROSDestroyLogicalDevice: Destroying a virtual buffer"));

			PVROSDestroyAVirtualBuffer (hLogicalDev->DeviceType,
										hLogicalDev->hVxD,
										hLogicalDev->hBufferMutex);
		}

		/* Examine all the callbacks incase anything has been left behind */
		ResetCallbacks(hLogicalDev);

		pLDevList = pBoardRef->LDeviceList;
		ASSERT(pLDevList);

		pPrev = NULL;
		while(pLDevList->hLogicalDev != hLogicalDev)
		{
			pPrev = pLDevList; 
			pLDevList = pLDevList->Next;
			if(pLDevList == NULL)
			{
				DPF((DBG_WARNING,"PVROSDestroyLogicalDevice: Couldn't find logical device on board list"));
				break;
			}
		}

		if(pLDevList)
		{
			if(pPrev)
			{
				pPrev->Next = pLDevList->Next;
			}
			else /* it was the first on the list */
			{
				pBoardRef->LDeviceList = pLDevList->Next;
			}
			PVROSFree(pLDevList);
		}

		pBoardRef->nLogicalDevices--;
		DPF((DBG_MESSAGE,"PVROSDestroyLogicalDevice: %d Logical Devices Left",
					pBoardRef->nLogicalDevices));

		if(pBoardRef->nLogicalDevices == 0)
		{
			MakeAllBuffersUnused(hLogicalDev->DeviceType);
		}

		CloseHandle(hLogicalDev->hBufferMutex);
		CloseVxd(hLogicalDev->hVxD);

		PVROSFree(hLogicalDev);
	}
}


/**********************************************************************
** Function Name: GetParamSettings
**
** Inputs       : Device
** Outputs      : Device->PlaneMemBytes,ObjectMemBytes,TexasMemBytes
** Returns      : -
**
** Description  : Reads the ISP, Object and TSP ParamSizes from the
**                registry and puts them in the device structure.
**                They're scaled down if their sum is > 2 Megs
***********************************************************************/

static void CALL_CONV GetParamSettings(HLDEVICE Device)
{
	float adjust;

	Device->PlaneMemBytes = 1024*HWRdValFileUInt( "ISPParamSize", (ISP_PARAM_SIZE/1024));
 	Device->TexasMemBytes = 1024*HWRdValFileUInt( "TSPParamSize", (TEX_PARAM_SIZE/1024)); 

	switch (Device->DeviceType)
	{
		case(MIDAS4):
		{
			Device->ObjectMemBytes = 4096*GetBuffer1Size();
		}
		case(MIDAS5):
		case(MIDAS5_003):
		{
			Device->ObjectMemBytes = 1024*HWRdValFileUInt( "ObjectParamSize", (OBJECT_PARAM_SIZE/1024));
		}
	}

	/* If ISPParamSize + ObjectParamSize > 2 Megs, scale them down */
	if((Device->PlaneMemBytes + Device->ObjectMemBytes) > 2*1024*1024)
	{
		adjust = (float)(2*1024*1024) / (float)(Device->PlaneMemBytes + Device->ObjectMemBytes);
		Device->PlaneMemBytes = (sgl_uint32) ((float)Device->PlaneMemBytes * adjust);
		Device->ObjectMemBytes = (sgl_uint32) ((float)Device->ObjectMemBytes * adjust);

		DPF((DBG_WARNING,"Combined ISP & Object Paramsize is > 2 Megs"));
		DPF((DBG_WARNING,"Resized to : %d bytes(ISP), %d bytes(Object).",Device->PlaneMemBytes,Device->ObjectMemBytes));
	}

}
/* end of $RCSfile: brdsetup.c,v $ */





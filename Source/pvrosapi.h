/*****************************************************************************
 *;++
 * Name           : $RCSfile: pvrosapi.h,v $
 * Title          : PVROSAPI.H
 * C Author       : Mick Jones and Jim Page and anyone else who knows me
 * Created        : $Date: 1998/01/27 16:22:55 $
 *
 * Copyright	: 1995-2022 Imagination Technologies (c)
 * License		: MIT
 *
 * Description    :	Generic(ish) cross platform porting API
 *                   
 * Program Type   :	C (SGL cross-platform)
 *
 * RCS info:
 *
 * $Date: 1998/01/27 16:22:55 $
 * $Revision: 1.81 $
 * $Locker:  $
 * $Log: pvrosapi.h,v $
 * Revision 1.81  1998/01/27  16:22:55  dm
 * Changed all registry paths to come off powervr\PCX1&2
 *
 * Revision 1.80  1997/12/12  12:25:51  erf
 * Defined WHICH_CARD_REGISTRY_STRING (defined in pvrosapi.h)
 * to determine where to look for the manufacturer.
 *
 * Revision 1.79  1997/12/11  20:04:14  erf
 * Added enum of possible hardware manufacturers of PCX's.
 *
 * Revision 1.78  1997/11/03  12:07:05  sks
 * Made the exported function table smaller.
 *
 * Revision 1.77  1997/10/28  10:29:46  erf
 * Added PVROSExtendTSPBuffer() to PVROS.dll .
 *
 * Revision 1.76  1997/10/22  10:22:06  sxy
 * changed the prototype of PVROSDbgDevBuffer().
 *
 * Revision 1.75  1997/10/14  16:50:31  gdc
 * new function prototype
 *
 * Revision 1.74  1997/10/14  15:19:32  sxy
 * added a function PVROSDbgDevBuffer() to get the shared buffer.
 *
 * Revision 1.73  1997/09/24  14:42:04  gdc
 * another read func
 *
 * Revision 1.72  1997/09/24  11:11:08  gdc
 * added new recommended functions for reading app hint stuff in correct
 * order
 *
 * Revision 1.71  1997/07/31  12:04:46  sks
 * Added PlaneMemBytes, ObjectMemBytes and TexasMemBytes
 * to the LogicalDevice structure
 *
 * Revision 1.70  1997/07/04  08:17:07  gdc
 * added new stub for a function to get version information etc.
 *
 * Revision 1.69  1997/06/23  21:59:22  jop
 * Added GetHighWaterMark fn and added ForceIt argument to Set fn
 *
 * Revision 1.68  1997/06/22  17:37:59  jop
 * Added adaptive TSP parameter stuff
 *
 * Revision 1.67  1997/06/17  20:31:06  mjg
 * Added BoardData to logical device
 *
 * Revision 1.66  1997/06/17  11:43:29  mjg
 * Added Johns changes
 *
 * Revision 1.65  1997/06/04  13:54:23  gdc
 * chnaged incorrect prototypes
 *
 * Revision 1.64  1997/05/30  16:33:10  mjg
 * Altered PVROS{Open | Close}DisplayDevice to take the logical device.
 *
 * Revision 1.63  1997/05/22  23:26:31  jop
 * Added scheduler locking funtions
 *
 * Revision 1.62  1997/05/19  14:51:39  jop
 * Added logical device ID to buffer struct
 *
 * Revision 1.61  1997/05/16  14:44:07  gdc
 * new enum for buffer type
 *
 * Revision 1.60  1997/05/15  12:42:27  gdc
 * removed a line I put in
 *
 * Revision 1.59  1997/05/15  11:23:39  gdc
 * added render timeout value to logical device
 *
 * Revision 1.58  1997/05/15  11:07:52  gdc
 * addded a pending tyep to previous stuff
 *
 * Revision 1.57  1997/05/15  11:05:54  gdc
 * made renderstatus an enumed type
 *
 * Revision 1.56  1997/05/15  11:01:02  gdc
 * added a is virtual flag to the buffer
 * and a rendertimeout slot to the logical device
 *
 * Revision 1.55  1997/05/14  12:45:27  gdc
 * added a renderspending element to the logical device, and made
 * a few things volatile
 *
 * Revision 1.54  1997/05/14  03:40:28  jop
 * Slashed away at API - more to come soon ...
 *
 * Revision 1.53  1997/05/14  00:26:50  jop
 * Added register map to BUFFER_LIST
 *
 * Revision 1.52  1997/05/13  20:55:32  jop
 * Added DeviceType to the board data struct
 *
 * Revision 1.51  1997/05/13  19:05:19  jop
 * Removed NEW macro - now in sglmem.h
 *
 * Revision 1.50  1997/05/13  17:55:18  mjg
 * Removed InitLogMemFile from the API.
 *
 * Revision 1.49  1997/05/13  16:25:51  gdc
 * added shedule prototype and structs for virtual buffs
 *
 * Revision 1.48  1997/05/13  11:14:55  gdc
 * added a DEVICE_TYPE to the logical device
 *
 * Revision 1.47  1997/05/13  11:05:53  gdc
 * added new structures to virtualise buffers
 *
 * Revision 1.46  1997/05/13  10:48:14  gdc
 * chnaged prototype of assign virtual buffers
 *
 * Revision 1.45  1997/05/09  15:00:39  mjg
 * Added ReleaseInterface.
 *
 * Revision 1.44  1997/05/06  16:31:30  gdc
 * added a new function to get the usage count on a board
 *
 * Revision 1.43  1997/05/01  16:27:37  mjg
 * Removed Texture API functions.
 *
 * Revision 1.42  1997/05/01  09:35:11  jop
 * moved sgl globals to sgl.h
 *
 * Revision 1.41  1997/05/01  02:41:59  jop
 *  Undid Ed's fixes
 *
 * Revision 1.40  1997/04/29  16:49:54  erf
 * 1. Removed #include "pvrosmem.h".
 * 2. Defined prototypes for all PVROS API functions.
 * 3. Defined PVROS API function structure definition.
 * 4. Defined extern PPVROSAPI_IF pIf, which is set to point to the
 *    PVROS API function structure.
 *
 * Revision 1.39  1997/04/28  12:18:05  erf
 * Diddling with fiddles
 *
 * Revision 1.38  1997/04/28  11:48:37  erf
 * Fixed structure definition of PVROS API.
 *
 * Revision 1.37  1997/04/28  11:42:00  erf
 * 1. Added PVR API function export structure.
 * 2. Added stuff previously defined in pvrosP.h.
 * 3. Grouped function prototypes together.
 *
 * Revision 1.36  1997/04/25  19:40:57  gdc
 * chnaged a prototype to have the write name
 *
 * Revision 1.35  1997/04/25  14:16:12  gdc
 * get closer to final public header
 *
 * Revision 1.34  1997/04/24  19:40:50  gdc
 * added SGL_LogicalDev to sgl globals enum
 *
 * Revision 1.33  1997/04/24  17:29:25  gdc
 * added prototypes for create/destroy device stuff
 *
 * Revision 1.32  1997/04/24  14:39:19  gdc
 * hopefully final sequence of protoypes for device initiasation
 *
 * Revision 1.31  1997/04/24  12:19:07  jop
 * Added texture api i/f enumeration
 * /
 *
 * Revision 1.30  1997/04/24  12:05:40  mjg
 * Added the SGL_Globals enumerated type.
 *
 * Revision 1.29  1997/04/23  14:23:18  gdc
 * added prototypes for some more functions - but they may be temporary
 *
 * Revision 1.28  1997/04/22  17:27:13  gdc
 * removed pvrd render context definition as it was causing all sorts
 * of gyp
 *
 * Revision 1.27  1997/04/22  17:02:32  gdc
 * modified RENDERCONTEXT struct and included texapi.h
 *
 * Revision 1.26  1997/04/22  16:47:16  erf
 * Added PVRD context structure definition.
 *
 * Revision 1.25  1997/04/22  16:20:24  gdc
 * added new entry to PciDeviceTable and a function prototype to get
 * the interface
 *
 * Revision 1.24  1997/04/21  22:24:15  gdc
 * changed prototypes to remove hsystem parameter
 *
 * Revision 1.23  1997/04/14  11:07:27  mjg
 * Added Buffer definitions.
 *
 * Revision 1.22  1997/04/07  17:03:29  mjg
 * Performance and largeint.
 *
 * Revision 1.21  1997/04/07  14:14:26  gdc
 * stopped HIWORD and LOWORD macros being defined if a win32 build, as
 * they are conflicting with a windows macro in windefs.h - I guess
 * if ours do different things we should rename them to SGLHIWORD or
 * something, as ours could well mess up windows stuff
 *
 * Revision 1.20  1997/04/04  13:04:42  jop
 * Moved NEW here - forces people to use PVROSAPI if they want
 * to malloc.
 *
 * Revision 1.19  1997/04/03  17:53:01  mjg
 * Replaced UINTs with sgl_uint.
 *
 * Revision 1.18  1997/04/03  13:13:34  mjg
 * Updated the calling convention.
 *
 * Revision 1.17  1997/04/02  21:28:55  mjg
 * Updated calling conventions.
 *
 * Revision 1.16  1997/04/01  19:16:40  mjg
 * ConfigSpace(s), CopyBuffer, Printf and Assert PVROS functions
 *
 * Revision 1.15  1997/03/29  22:01:17  mjg
 * Insert PVROSDelay and a couple of other headers.
 *
 * Revision 1.14  1997/03/27  12:40:44  erf
 * 1. Added #ifndef __PVROSAPI_H_ etc. so that include file is only included
 *    once.
 * 2. Added #include "pvrosmem.h" to include new macros.
 * 3. Changed mallos to PVROSMalloc.
 *
 * Revision 1.13  1997/03/25  11:18:01  jop
 * Merged changes with Mick's and added a file header (isn't it pretty??)
 *
 *;--
 *****************************************************************************/

#ifndef __PVROSAPI_H__
#define __PVROSAPI_H__

#define MAX_MEM_IO_WINDOWS  6

#ifndef __PVROSIO_H__
#include "pvrosio.h"
#endif

typedef void *PVRHANDLE;
typedef PVRHANDLE DISPLAYID;
typedef PVRHANDLE DEVICEID;
typedef PVRHANDLE HDISPLAY;
typedef PVRHANDLE HDEVICE;
typedef PVRHANDLE HMODE;

typedef  void (* PVRFNPTR)();

typedef enum
{
	PVR_Texture,
	PVR_SGL,
	PVR_SGLLite,
	PVR_PVRDirect
} PVR_Interface;

typedef enum
{
	DISABLE_BOARD=0,
	MIDAS3,
	MIDAS4,
	MIDAS5,
	MIDAS5_003
} DEVICE_TYPE;

#define WHICH_CARD_REGISTRY_STRING	"SOFTWARE\\PowerVR\\PCX1&2\\StartUp"
#define VL_REGISTRY_STRING			"SOFTWARE\\PowerVR\\PCX1&2\\"
#define MATROX_REGISTRY_STRING		"SOFTWARE\\PowerVR\\PCX1&2\\"

typedef enum
{
	VIDEOLOGIC_3D,
	MATROX_M3D
} MANUFACTURER_TYPE;


typedef struct PciDeviceTable
{
	DEVICEID   DeviceID;             /* (Bus Number << 8) + (Device Number << 3) + Function Number */
	sgl_uint32 PCIVendorID;
	sgl_uint32 PCIDeviceID;
	DEVICE_TYPE DeviceType;
} PciDeviceTable;

typedef enum
{
	PVR_STATUS_PENDING	= 0x00000000,
	PVR_STATUS_EOR		= 0x00000001
} PVR_STATUS;

typedef struct BoardDataBlock
{
	DEVICE_TYPE	DeviceType;
	sgl_uint32 	PCIVendorID;
	sgl_uint32 	PCIDeviceID;
	sgl_uint32 	PCIBus;
	sgl_uint32 	PCIDev;
	sgl_uint32 	PCIFunc;
	sgl_uint32 	RevisionID;

	sgl_uint32 	IRQ;
	sgl_uint32 	nMemWindows;
	sgl_uint32 	PhysMemWindows   [MAX_MEM_IO_WINDOWS][2];
	sgl_uint32 	LinearMemWindows [MAX_MEM_IO_WINDOWS];
	sgl_uint32 	nIOWindows;
	sgl_uint32 	IOWindows        [MAX_MEM_IO_WINDOWS][2];

	PVR_STATUS volatile	*pStatus;
	
} BoardDataBlock;

typedef struct DisplayModeTable
{
	HMODE		hMode;
	sgl_uint32	XResolution;
	sgl_uint32	YResolution;
	sgl_uint32	BitDepth;
} DisplayModeTable;

typedef struct DisplayDeviceTable
{
	DISPLAYID			ID;
	DisplayModeTable	DMT;
	sgl_uint32 	        DisplayVendorID;
	sgl_uint32 	        DisplayDeviceID;
} DisplayDeviceTable;

typedef struct DisplayModeDataBlock
{
	DisplayModeTable	DMT;
	sgl_uint32			RefreshRate;
	sgl_uint32			RedMask;
	sgl_uint32			GreenMask;
	sgl_uint32			BlueMask;
	sgl_uint32			NumberOfBuffers;
	sgl_uint32			CurrentBuffer;
	sgl_uint32			PhysBufferAddress   [3];
	sgl_uint32			LinearBufferAddress [3];
} DisplayModeDataBlock;

#if !(WIN32)
#ifndef LOWORD
#define  LOWORD(x)           ((sgl_uint16)(sgl_uint32)(x))
#endif

#ifndef  HIWORD
#define  HIWORD(x)           ((sgl_uint16)((((sgl_uint32)(x)) >> 16) & 0xFFFF))
#endif
#endif

typedef enum 
{
	PVROS_GROOVY = 0,
	PVROS_DODGY = -1,
	PVROS_TOO_MANY_BUFFERS = -2,
	
} PVROSERR;

typedef enum
{
	FRAME_BUFFER_ACTIVE,			/* we handle all buffer management, flipping etc. */
	FRAME_BUFFER_PASSIVE			/* the client is handling buffers etc. */
} FRAME_BUFFER_MODE;

/* 	enumerate functions:

	always return number of items in list. ie:
	
	nItems = PVROSEnumerate (hSomething, NULL);
	
	if (nItems)
	{
		// we always return 1 more than there is, and the
		// extra (last) has 0xFFFFFFFF in the first word
		
		PITEM pItem = PVROSMalloc ((nItems + 1) * sizeof (ITEM));
		 
		if (pItem)
		{
			PVROSEnumerate (hSomething, pItem);
			
			... do stuff ...
			
			PVROSFree (pItem);
		}
	}

*/

typedef enum
{
	CB_PRE_RENDER,
	CB_POST_RENDER,
	CB_END_OF_RENDER,
	CB_2D,
	/* max number of callback types, not a type! */
	CB_MAX				
	
} CALLBACK_TYPE;

typedef struct PRE_RENDER_CB_STRUCT
{
	sgl_uint32	FlipRequested;

	sgl_uint32	PhysRenderBufferAddress;
	sgl_uint32	PhysRenderBufferBitsPerPixel;
	sgl_uint32	PhysRenderBufferStride;
	
} PRE_RENDER_CB_STRUCT;

typedef PVROSERR (CALL_CONV * PVROSCB)(void *pData, void *pContext);

typedef enum 
{
	PVR_DELAY_MS,
	PVR_DELAY_BUS_CYCLES
} PVR_DELAY;

typedef struct 
{
	sgl_uint32 LowPart;
	sgl_uint32 HighPart;
} sgl_largeint;

typedef struct 
{
	sgl_uint32 *pBuffer;
	sgl_uint32 uBufferPos;	
	sgl_uint32 uBufferLimit;
} PVR_PARAM_BUFF;

typedef enum
{
	PVR_PARAM_TYPE_ISP,
	PVR_PARAM_TYPE_TSP,
	PVR_PARAM_TYPE_REGION	
} PVR_PARAM_TYPE;

typedef enum
{
	REAL_BUFFER,
	VIRTUAL_BUFFER,
	SECRET_BUFFER,
	PCX2_AS_PCX1_BUFFER
} BUFFER_TYPE;

typedef struct DMABUFFER
{               
	unsigned long	GlobalAlias;
	unsigned long	Ring0Linear;
	unsigned long	Pages;
	unsigned long	PhysicalAddress;
} DMABUFFER;

typedef struct DMASCATTERBUFFER
{
	unsigned long	LinearAddress;
	unsigned long	PagesAllocated;
	unsigned long	BlockSize;
	unsigned long	SlotsUsed;
	unsigned long	PhysAddress[128];
	unsigned long	PhysPages[128];
} DMASCATTERBUFFER;

typedef struct tagPCXBUFFER
{
	unsigned long PageSize;
	unsigned long LinearAddress;
	unsigned long SlotsUsed;
	unsigned long PhysAddresses[128];
	unsigned long Limit[128];
} PCXBUFFER;

#define ISP_BUFFER_IS_VIRTUAL		1
#define REGION_BUFFER_IS_VIRTUAL	2
 
typedef struct tagBUFFER_LIST {
	volatile sgl_bool 		bInUse;
	DEVICE_TYPE 			DeviceType;
	PCXBUFFER 				PCXBuffer;
	DMASCATTERBUFFER 		*pDMASB;
	DMABUFFER 				*pDMAB;
	PVR_PARAM_BUFF 			PVRParamBuf[3];
	sgl_uint32 				*DestAddr[3];
	sgl_uint32 				Registers[32];
	BUFFER_TYPE 			BufferType;
	struct tagBUFFER_LIST 	*Next;
	struct LogicalDevice 	*hLogicalDev;
} BUFFER_LIST;


typedef struct LogicalDevice
{
	PVRHANDLE 			TexHeap; /* this will be pointed at the texture heap - cast to use */
	PVR_PARAM_BUFF 		Buffers[3]; 
	volatile PVR_STATUS RenderStatus;
	sgl_uint32 			RenderTimeout;
	sgl_uint32 			Registers[32];
	HDEVICE 			hDeviceID;
	DEVICE_TYPE 		DeviceType;
	BUFFER_LIST 		*VirtualBuffer;
	volatile sgl_uint32 RendersPending;
	sgl_uint32			dwPID;
	PVRHANDLE 			hVxD;
	PVRHANDLE 			hLib;
	PVRHANDLE 			hBufferMutex;
	BoardDataBlock		BoardData;
	sgl_uint32			PlaneMemBytes;
	sgl_uint32			ObjectMemBytes;
	sgl_uint32			TexasMemBytes;
} LDEVICE, *HLDEVICE;

#endif /* __PVROSAPI_H__ */

/************************************************************************/
/*			API definition for PVROS module (see pvrif.h).				*/
/************************************************************************/

#include "pvrif.h"

/* 
	by default:
	
	The PVROS API is defined as follows:

	types:

		PVROSAPI_IF:				structure type containing API jump table
		PPVROSAPI_IF:				pointer to above type
		PFN_GET_PVROSAPI_IF:		function type that can be used to get the API
									jump table from PVROS.DLL

	implicit declarations:

		int GetPVROSAPI_IF(void):	The call to get an interface 
									object from PVROS.DLL

	example:

	LoadAndUsePVROSAPI ()
	{
		HMODULE			hPVROSLib;
		PPVROSAPI_IF 	pIF;

		// load the PVROS dll 
		hPVROSLib = LoadLibrary ("PVROS.DLL");

		if (hPVROSLIB)
		{
			// get the API function from PVROS.DLL
			PFN_GET_PVROSAPI_IF pfnGetFn = GetProcAddress (hPVROSLib, "GetPVROSAPI_IF");

			// if the fn ptr is ok and the size is what we expect ...
			if (pfnGetFn && (pfnGetFn (&pIF) == sizeof (*pIF)))
			{
				...
				// use the interface!
				pIF->pfnPVROSGetInterface (...);
				...
				return (TRUE);
			}
			else
			{
				// bad DLL or versions!
				return (FALSE);
			}
		}
	}

*/

API_START (PVROSAPI)

	API_FN (sgl_uint32,	PVROSGetInterface, (HLDEVICE hLDevice, PVR_Interface Spec, PVRHANDLE *ppIf))
	API_FN (sgl_uint32,	PVROSReleaseInterface, (HLDEVICE hLDevice))
	API_FN (sgl_uint32,	PVROSGetTexInterface, (HDEVICE hDeviceID, PVRHANDLE *ppIf))

	API_FN (void,		PVROSPrintf, (char* pszFormat,...))
	API_FN (void,		PVROSAssert, (char *szAssertText,char *szFileName,sgl_int32 nLine,int * ask))
	API_FN (sgl_uint32,	PVROSQueryPerformanceCounter, (sgl_largeint* Val))
	API_FN (sgl_uint32, PVROSQueryPerformanceFrequency, (sgl_largeint* Val))

	API_FN (int,        PVROSGetUsagePhysicalDevice, (PciDeviceTable *pBoard))
	API_FN (HDEVICE,	PVROSCreatePhysicalDevice, (PciDeviceTable *pBoard))
	API_FN (void,		PVROSDestroyPhysicalDevice, (HDEVICE hDeviceID))
	API_FN (PVRHANDLE,	PVROSCreateTexHeap, (HDEVICE hDeviceID))
	API_FN (void,		PVROSDestroyTexHeap, (PVRHANDLE TexHeap))
	API_FN (HLDEVICE,	PVROSCreateLogicalDevice, (HDEVICE hDeviceID,PVRHANDLE TexHeap))
	API_FN (void,		PVROSDestroyLogicalDevice, (HLDEVICE Device))

	API_FN (HDISPLAY,	PVROSOpenDisplayDevice, (void *pLogDevice, DISPLAYID DisplayID,FRAME_BUFFER_MODE fbm))
	API_FN (PVROSERR,	PVROSCloseDisplayDevice, (void *pLogDevice, HDISPLAY hDisplay))
	API_FN (sgl_uint32,	PVROSEnumDisplayModes, (HDISPLAY hDisplay,DisplayModeTable *DMT))
	API_FN (PVROSERR,	PVROSSelectDisplayMode, (HDISPLAY hDisplay,HMODE hMode,sgl_uint32 nBuffers))

	API_FN (PVROSERR,	PVROSCallbackRegister, (void *pLogDevice,CALLBACK_TYPE cType,PVROSCB fn,void *pContext))
	API_FN (PVROSERR,	PVROSCallback, (void *pLogDevice,CALLBACK_TYPE cType,void *pCallbackData))

	API_FN (void *, 	PVROSMalloc, (sgl_uint32 X))
	API_FN (void,		PVROSFree, (void * X))

	API_FN (PVROSERR,	PVROSScheduleRender, (HLDEVICE LogicalDevice))

	API_FN (sgl_uint32,	PVROSDetermineTexMemConfig, ( sgl_uint32 fSettings ))
	API_FN (sgl_uint32,	PVROSEnumPCIDevices, (PciDeviceTable *pPDT))

	API_FN (sgl_uint32,	PVROSExtendTSPBuffer, (HLDEVICE gHLogicalDev))

	API_FN (sgl_int32,	HWRdValFileInt, (char * RegName,sgl_int32 DefaultVal))
	API_FN (sgl_uint32,	HWRdValFileUInt, (char * RegName,sgl_uint32 DefaultVal))
	API_FN (float,	    HWRdValFileFloat, (char * RegName,float DefaultVal))
	API_FN (sgl_bool,	HWRdValFileString, (char * RegName, char *pszBuffer,int pszBufferSize))
	API_FN (sgl_bool, 	GetApplicationHintFloat, (char *pszHint,float *pData))
	API_FN (sgl_bool,	GetApplicationHintInt, (char *pszHint,sgl_int32 *pData))
	API_FN (sgl_bool,	GetApplicationHintString, (char *pszHint,char *pszBuffer,int pszBufferSize))
	API_FN (int,		SglReadPrivateProfileInt, (char *szSection,char *szEntry,int nDefault,char *szFilename))
	API_FN (sgl_bool,	SglWritePrivateProfileInt, (char *szSection,char *szEntry,int nValue,char *szFilename))
	API_FN (sgl_bool,	SglReadPrivateProfileString, (char *szSection,char *szEntry,char *szDefault,char *szText,int nTextSize,char *szFilename))
	API_FN (sgl_bool,	SglWritePrivateProfileString, (char *szSection,char *szEntry,char *szText,char *szFilename))
	API_FN (float,      SglReadPrivateProfileFloat, (char *szSection, char *szEntry, float fDefault, char *szFilename))

	#if WIN32

	API_FN (void,		PVROSSetDDrawWindowHandle, (void *hWnd))
	API_FN (PVROSERR,	GetDDraw2dCallbackInfo, ( HDISPLAY hDisplay,void *pCSP ))

	#endif

	API_FN (PVROSERR,	PVROSAssignVirtualBuffers, (PVR_PARAM_BUFF *pPVRParamBuf, HLDEVICE HLogicalDev))
	API_FN (void,       PVROSFreeVirtualBuffers, (HLDEVICE hLogDev))

	API_FN (char *,		PVROSDbgDevBuffer, (int k))

API_END (PVROSAPI)



	/* Functions that are only called internally - i.e. not exported */

#ifdef API_TYPESONLY
#else

	DMABUFFER *PVROSCreateDMABuffer (PVRHANDLE hVxD,sgl_uint32 Bytes);
	DMASCATTERBUFFER *PVROSCreateDMAScatterBuffer (PVRHANDLE hVxD,sgl_uint32 Bytes);
	PVROSERR PVROSDestroyDMABuffer (PVRHANDLE hVxD,DMABUFFER *pBlock);
	PVROSERR PVROSDestroyDMAScatterBuffer (PVRHANDLE hVxD,DMASCATTERBUFFER *pSGB);
	PVROSERR PVROSSetCacheMode (DMABUFFER *blk,sgl_uint32 mode);
	HDEVICE	PVROSOpenPCIDevice (DEVICEID DeviceID);
	PVROSERR PVROSClosePCIDevice (HDEVICE hDevice);
	PVROSERR PVROSSetupPowerVRDevice (HDEVICE hDevice,DEVICE_TYPE BoardType);
	PVROSERR PVROSGetPCIDeviceInfo (HDEVICE hDevice, BoardDataBlock *BDB);
	PVROSERR PVROSInitPCIAddresses (HDEVICE hDeviceID,DEVICE_TYPE DeviceType);
	PVROSERR PVROSReadPCIConfigSpace (HDEVICE hDevice,sgl_uint32 dwRegNo,sgl_uint32 *dwRegValue);
	PVROSERR PVROSWritePCIConfigSpace (HDEVICE hDevice,sgl_uint32 dwRegNo,sgl_uint32 dwRegValue);
	PVROSERR PVROSCopyBufferToHardware (void *Src,void *Dst,sgl_uint32 Bytes,sgl_uint32 CheckCopy);
	void *PVROSMapPhysicalToLinear (sgl_uint32 dwPhysical,sgl_uint32 RangeBytes);
	sgl_uint32 PVROSMapLinearToPhysical (sgl_uint32 dwLinear);

	void PVROSDelay (PVR_DELAY eDelay,sgl_uint32 uCount);

	sgl_uint32 PVROSEnumDisplayDevices (DisplayDeviceTable *DDT);
	PVROSERR PVROSGetDisplayModeInfo (HDISPLAY hDisplay,DisplayModeDataBlock *DMDB);
	PVROSERR PVROSFlipFrameBuffer (HDISPLAY hDisplay);

	PVROSERR PVROSPreRenderCallbackAuto (void *pData,HDISPLAY hDisplay);
	PVROSERR PVROSPostRenderCallbackAuto (void *pData,HDISPLAY hDisplay);
	PVROSERR PVROSCallbackUnRegister (void *pLogDevice,CALLBACK_TYPE cType,PVROSCB fn,void *pContext);


	void *PVROSCalloc (sgl_uint32 X,sgl_uint32 Y);
	void *PVROSRealloc (void * X,sgl_uint32 Y);
	void *PVROSAllocSharedMemory (sgl_uint32 Bytes);
	void PVROSFreeSharedMemory (void *pPtr);

	PVROSERR PVROSSchedulerLock (HDEVICE hDeviceID);
	PVROSERR PVROSSchedulerUnlock (HDEVICE hDeviceID);

	void PVROSGetWinVersions (HLDEVICE LogicalDevice, char **VersionInfo);

	sgl_uint32 *PVROSGetVirtualBurstSpace (void);
	sgl_uint32 *PVROSGetEndOfRenderPtr (HLDEVICE hLogDev);
	sgl_int32 PVROSSetTSPHighWaterMark (PVRHANDLE hTexHeap, sgl_int32 nRequested, sgl_bool bForceIt);
	sgl_int32 PVROSGetTSPHighWaterMark (PVRHANDLE hTexHeap);

#endif

/* end of $RCSfile: pvrosapi.h,v $ */

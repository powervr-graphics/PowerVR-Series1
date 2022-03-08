/******************************************************************************
 * Name			: pvrosapi.c
 * Title		: PowerVR Operating System dependent code (DOS4GW)
 * Author		: Mick Jones
 * Created		: 20/03/1997
 *
 * Copyright	: 1995-2022 Imagination Technologies (c)
 * License		: MIT
 *
 * Description	: Part of the low level operarting system specific API
 *                for DOS4GW, this requires an SVGA card with a VESA
                  UVB v2.0 BIOS 
 *
 * Platform		: ANSI
 *
 * Modifications:
 *			$Log: pvrosapi.c,v $
 * Revision 1.31  1998/02/02  12:13:51  sjf
 * ARGGHHH   Obsoleted and rechecked this version in again because JMD
 * filled it with control Ms. ...
 * Added ResetTritonControlRegister() to ExitHandler()
 *
 * Revision 1.31  1997/09/05  13:51:10  jmd
 * Added ResetTritonControlRegister() to ExitHandler()
 *
 * Revision 1.30  1997/08/19  18:22:22  jmd
 * Fixed prerender callback bug and null status ptr bug
 *
 * Revision 1.29  1997/07/30  15:28:43  mlj
 * Fixed the line stride setup, this will fix the problems
 * all Matrox cards were having at 800x600
 *
 * Revision 1.28  1997/07/25  09:01:19  sjf
 * Disabled the Performace measuring code because it breaks
 * a Cyrix processor.
 *
 * Revision 1.27  1997/07/24  11:33:21  mlj
 * Fixed the VXD calling when running under windows
 *
 * Revision 1.26  1997/07/24  11:17:10  mlj
 * I've made sure the VxD function pointer is setup
 * when running under windows
 *
 * Revision 1.25  1997/06/27  14:47:07  mlj
 * added code to allow the dos lib to run under windows
 *
 * Revision 1.24  1997/06/20  13:55:45  mlj
 * removed all calls to MDAprint in not debug builds
 *
 * Revision 1.23  1997/06/17  18:08:49  mlj
 * fixed page flipping on a GS600 and also added some
 * alternitive ways of getting the end of frame buffer
 * offset. I've also (hopefully) improved performance
 * by stopping the ReturnVBEModeInformation being called
 * at the begining of each frame.
 *
 * Revision 1.22  1997/06/12  10:43:53  mlj
 * Oh oops, I missed one of the SetVBE2Mode functions
 * see what I wrote on the last revision of this file.
 *
 * Revision 1.21  1997/06/12  10:19:05  mlj
 * pvrosapi will no longer try to switch back to the
 * initail video mode on exiting. this should be done
 * by the application
 *
 * Revision 1.20  1997/06/05  12:07:43  sjf
 * Added RCS Log macro and got rid of some ^M's.
 *
 *
 *
 *  		20th March 1997		Initial Version		Mick Jones
 *
 *****************************************************************************/

#include <stdio.h>
#include <dos.h>
#include <malloc.h>
#include <string.h>
#include <stdarg.h>
#include <signal.h>

#include "sgl.h"
#include "sgl_defs.h"
#include "defs.h"
#include "vesa.h"
#include "pci.h"
#include "vds.h" 
#include "pvrosapi.h"
#include "irq.h"
#include "vxd.h"


#define POLL_EOR 1
#if POLL_EOR
#pragma message ("Polling for EOR")
#else
#include "isr.h"
#endif

#include "hwinterf.h" /* for HWFinishedRender */

/* Defined in brdsetup.c, to fix FX430 and Matrox problem */
PVROSERR ResetTritonControlRegister();


ModeInfoBlock        gModeInfo;
sgl_uint32           gFrontBuffer           = 0;
sgl_uint32           gBackBuffer   	        = 0;
sgl_uint32           gWaitForVSync          = 1;
sgl_uint32           gFixOffScreenMemOffset = 0;
sgl_uint32           gModeInfoFlag          = 0;
sgl_uint32           gStartupMode           = 0xFFFFFFFF;
sgl_uint32           gnBuffers              = 1;
sgl_uint32           gBufferOffset          = NULL;
FRAME_BUFFER_MODE    gFrameBufferMode       = ACTIVE;
sgl_uint32			 gBogusStatusFlags		= 0;
/* debug functions */
void mdaPrint   (char *pString);
void mdaNewLine (void);


/* globals */
BoardDataBlock       gBoardDataBlock;
HVXD				 hVxD;
sgl_uint32           gVxdOpen = 0;

/* signal handlers */

void ExitHandler (void)
{
	PVROSPrintf ("ExitHandler: cleaning up ...\n");

	while (!HWFinishedRender ())
	{
		PVROSPrintf ("ExitHandler: Waiting for hardware\n");
	}

	#if !POLL_EOR
	KillISR ();
	EnableISR (DISABLE_BOARD);
	#endif

	ResetTritonControlRegister();

	if (gStartupMode != 0xFFFFFFFF)
	{
	//	SetVBE2Mode (gStartupMode);
		gStartupMode = 0xFFFFFFFF;
	}

	PVROSPrintf ("ExitHandler: Ok\n");
}	

void SignalHandler (int sig_num)
{
	switch (sig_num)
	{
		// an erroneous floating-point operation 
		// occurs (such as division by zero, 
		// overflow and underflow)
		case SIGFPE:
		{
			// ignore
			PVROSPrintf ("SignalHandler: FPE\n");
			break;
		}

		// an interactive attention (CTRL/BREAK 
		// on keyboard) is signalled
		case SIGBREAK:
		// an interactive attention (CTRL/C on 
		// keyboard) is signalled
		case SIGINT:
		// a termination request is sent to the program		
		case SIGTERM:
		{
			PVROSPrintf ("SignalHandler: BREAK/INT/TERM\n");
			exit (0);
		}
		// abnormal termination, such as 
		// caused by the  abort function
		case SIGABRT: 
		// illegal instruction encountered
		case SIGILL:
		// an illegal memory reference is 
		// detected
		case SIGSEGV:
		{
			PVROSPrintf ("SignalHandler: ABRT/ILL/SEGV\n");
			break;
		}
	}

	exit (1);
}

/********************/
/* SYSTEM FUNCTIONS */
/********************/

void Int3 (void);
#pragma aux Int3 = "int 3"

HSYSTEM CALL_CONV PVROSCreateSystemContext (void)
{
	if (AreWeInWindows())
	{
		if (OpenVxD (0x95, hVxD))
		{
			gVxdOpen = 1;
			return (HSYSTEM)1;
		}
		else
		{
			return (HSYSTEM)0;
		}
	}
	
	return (HSYSTEM)1;
}

PVROSERR CALL_CONV PVROSDestroySystemContext (HSYSTEM hSystem)
{
	if (hSystem != 1) return PVROS_DODGY;
	
	return PVROS_GROOVY;
}

/* DANGER this function is making an assumption as  */
/* to how DOS4GW allocates memory, it will most     */
/* probably not work using virtual memory, ie under */
/* windows or with the DOS4VM flag set              */ 

DMABUFFER * CALL_CONV PVROSCreateDMABuffer (HSYSTEM hSystem, sgl_uint32 Bytes)
{
	void             *dmaBuffer;
	DMABUFFER        *pDmaBuffer;
		

	if (hSystem != 1) return NULL;

	if (AreWeInWindows())
	{
		pDmaBuffer = malloc(sizeof(DMABUFFER));

		if (gVxdOpen == 0)
		{
			if (OpenVxD (0x95, hVxD) == 0)
			{
				return NULL;
			}
			else
			{
				gVxdOpen = 1;
			}
		}

		AllocateBlock (hVxD, pDmaBuffer, Bytes >> 12);
	}
	else
	{
		/* no pageing so linear address = physical address */
		
		pDmaBuffer = malloc(sizeof(DMABUFFER));
		dmaBuffer  = malloc(Bytes);

		pDmaBuffer->GlobalAlias     = (sgl_uint32)dmaBuffer;
		pDmaBuffer->Ring0Linear     = (sgl_uint32)dmaBuffer;
		pDmaBuffer->Pages           = Bytes >> 12;
		pDmaBuffer->PhysicalAddress = (sgl_uint32)dmaBuffer;
	}

	return pDmaBuffer;
}

/* DANGER this function is making an assumption as  */
/* to how DOS4GW allocates memory, it will most     */
/* probably not work using virtual memory, ie under */
/* windows or with the DOS4VM flag set              */ 

DMASCATTERBUFFER * CALL_CONV PVROSCreateDMAScatterBuffer (HSYSTEM hSystem, sgl_uint32 Bytes)
{
	void             *dmaBuffer;
	DMASCATTERBUFFER *pDmaScatterBuffer;


	if (hSystem != 1) return NULL;

	if (AreWeInWindows())
	{
		pDmaScatterBuffer = malloc(sizeof(DMASCATTERBUFFER));

		if (gVxdOpen == 0)
		{
			if (OpenVxD (0x95, hVxD) == 0)
			{
				return NULL;
			}
			else
			{
				gVxdOpen = 1;
			}
		}

		CreateScatterBuffer (hVxD, pDmaScatterBuffer, Bytes);
	}
	else
	{
		sgl_uint32 uPageAlignedPhysical;
		sgl_uint32 uPageAlignedLinear;
		
		/* no pageing so linear address = physical address */
	
		pDmaScatterBuffer = malloc(sizeof(DMASCATTERBUFFER));
		dmaBuffer         = malloc(Bytes + 4096);

		uPageAlignedPhysical = ((sgl_uint32) dmaBuffer + 0x00000FFF) & 0xFFFFF000;
		
		uPageAlignedLinear = PVROSMapPhysicalToLinear (hSystem, uPageAlignedPhysical, Bytes);

		pDmaScatterBuffer->LinearAddress   = uPageAlignedLinear;
		pDmaScatterBuffer->PagesAllocated  = Bytes >> 12;
		pDmaScatterBuffer->BlockSize       = Bytes >> 12;
		pDmaScatterBuffer->SlotsUsed       = 1;
		pDmaScatterBuffer->PhysAddress[0]  = uPageAlignedPhysical;
		pDmaScatterBuffer->PhysAddress[1]  = (sgl_uint32)dmaBuffer;
		pDmaScatterBuffer->PhysPages[0]    = Bytes >> 12;
	}

	return pDmaScatterBuffer;
}

PVROSERR CALL_CONV PVROSDestroyDMABuffer (HSYSTEM hSystem, DMABUFFER *pBlock)
{
	if (hSystem  !=  1) return PVROS_DODGY;

	if (pBlock == NULL) return PVROS_DODGY;

	if (AreWeInWindows())
	{
		if (gVxdOpen == 0)
		{
			if (OpenVxD (0x95, hVxD) == 0)
			{
				return PVROS_DODGY;
			}
			else
			{
				gVxdOpen = 1;
			}
		}

		FreeBlock (hVxD, pBlock);
	}
	else
	{
		free ((void *)pBlock->Ring0Linear);
		free(pBlock);
	}
	
	return PVROS_GROOVY;
}

PVROSERR CALL_CONV PVROSDestroyDMAScatterBuffer (HSYSTEM hSystem, DMASCATTERBUFFER *pSGB)
{
	if (hSystem != 1) return PVROS_DODGY;

	if (pSGB == NULL) return PVROS_DODGY;

	if (AreWeInWindows())
	{
		if (gVxdOpen == 0)
		{
			if (OpenVxD (0x95, hVxD) == 0)
			{
				return PVROS_DODGY;
			}
			else
			{
				gVxdOpen = 1;
			}
		}

		DestroyScatterBuffer (hVxD, pSGB);
	}
	else
	{
		free ((void *)pSGB->PhysAddress[1]);
		free (pSGB);
	}
	
	return PVROS_GROOVY;
}

PVROSERR CALL_CONV PVROSSetCacheMode (HSYSTEM hSystem, DMABUFFER *blk, sgl_uint32 mode)
{
	if (hSystem != 1) return PVROS_DODGY;
	
	return PVROS_GROOVY;
}

sgl_uint32 CALL_CONV PVROSMapPhysicalToLinear (HSYSTEM hSystem, sgl_uint32 dwPhysical, sgl_uint32 RangeBytes)
{
	union  REGS       regs;

	
	if (hSystem != 1) return NULL;

	if (AreWeInWindows())  // running in pageing mode ?
	{
		/* DPMI call 800h physical address mapping */
		regs.w.ax=0x0800;
		regs.w.bx=HIWORD(dwPhysical);
		regs.w.cx=LOWORD(dwPhysical);
		regs.w.si=HIWORD(RangeBytes);
		regs.w.di=LOWORD(RangeBytes);
		int386( 0x31, &regs, &regs);
		if ( regs.x.cflag != 0 ) return NULL;

		return ((((sgl_uint32)regs.w.bx) << 16) + (sgl_uint32)regs.w.cx);
	}

	/* if here then virtual memory isn't being use */
	/* therefor a linear address is the same as a  */
	/* physical address                            */

	return dwPhysical;	
}

sgl_uint32 CALL_CONV PVROSMapLinearToPhysical (HSYSTEM hSystem, sgl_uint32 dwLinear)
{
	if (hSystem != 1) return NULL;

	if (AreWeInWindows())  // running in pageing mode ?
	{
		union  REGS     regs;
	
		sgl_uint32 PhysPageDirectoryTable;
		sgl_uint32 PhysPageTable;
		sgl_uint32 PhysAddress;

		sgl_uint32 LinearPageDirectoryTable;
		sgl_uint32 LinearPageTable;
	
		sgl_uint32 DirectoryIndex;
		sgl_uint32 PageIndex;		sgl_uint32 Offset;


		DirectoryIndex = (dwLinear & 0xFFC00000) >> 22;
		PageIndex      = (dwLinear & 0x003FF000) >> 12;
		Offset         = (dwLinear & 0x00000FFF);
		
		/* get a physical address for the page directory table */	
		PhysPageDirectoryTable = GetPDBR();

		/* get a linear address for the page directory table   */	
		regs.w.ax=0x0800;
		regs.w.bx=HIWORD(PhysPageDirectoryTable);
		regs.w.cx=LOWORD(PhysPageDirectoryTable);
		regs.w.si=HIWORD(0);
		regs.w.di=LOWORD(4096);
		int386( 0x31, &regs, &regs);
		if ( regs.x.cflag != 0 ) return NULL;

		LinearPageDirectoryTable = (sgl_uint32)((((sgl_uint32)regs.w.bx) << 16) + (sgl_uint32)regs.w.cx);

		PhysPageTable = (sgl_uint32)((sgl_uint32*)(LinearPageDirectoryTable + (DirectoryIndex << 2)));

		/* get a linear address for the page table */	
		regs.w.ax=0x0800;
		regs.w.bx=HIWORD(PhysPageTable);
		regs.w.cx=LOWORD(PhysPageTable);
		regs.w.si=HIWORD(0);
		regs.w.di=LOWORD(4096);
		int386( 0x31, &regs, &regs);
		if ( regs.x.cflag != 0 ) return NULL;

		LinearPageTable = (sgl_uint32)((((sgl_uint32)regs.w.bx) << 16) + (sgl_uint32)regs.w.cx);

		PhysAddress	=(sgl_uint32)((sgl_uint32*)(LinearPageTable + (PageIndex << 2)));

		return (PhysAddress + Offset);
	}

	/* if here then virtual memory isn't being use */
	/* therefor a linear address is the same as a  */
	/* physical address                            */

	return dwLinear;
}


/************************/
/* PCI DEVICE FUNCTIONS */
/************************/


sgl_uint32 CALL_CONV PVROSEnumPCIDevices (HSYSTEM hSystem, PciDeviceTable *pciDeviceTable)
{
	PciConfigSpace *pciConfigSpace;
	sgl_uint32      retVal;
	sgl_uint32      i;
	sgl_uint32      devID;


	if (hSystem != 1)  return 0;

	retVal = PCIFindAllDevices(NULL);
	pciConfigSpace = malloc((sizeof(PciConfigSpace) * retVal) + 1);
	retVal = PCIFindAllDevices(pciConfigSpace);
	

	if (pciDeviceTable == NULL)
	{               
		free (pciConfigSpace);
		return retVal;
	}
	else
	{
		for (i = 0; i < retVal; i++)
		{
			devID  = pciConfigSpace[i].PciBusID << 8;
			devID += pciConfigSpace[i].PciDevID << 3;
			devID += pciConfigSpace[i].PciFncID;

			pciDeviceTable[i].DeviceID    = (DEVICEID)devID;
			pciDeviceTable[i].PCIVendorID = pciConfigSpace[i].VendorID;
			pciDeviceTable[i].PCIDeviceID = pciConfigSpace[i].DeviceID;
		}
		pciDeviceTable[retVal].DeviceID = (DEVICEID)0xFFFFFFFF;
	}

	free (pciConfigSpace);
	return retVal;
}


HDEVICE CALL_CONV PVROSOpenPCIDevice (HSYSTEM hSystem, DEVICEID DeviceID)
{
	union  REGS     regs;
	
	PciConfigSpace  pciConfigSpace;
	
	sgl_uint32      retVal;
	sgl_uint32      pciRetVal;
	sgl_uint32      pciRegAddr;

	sgl_uint8       busNum;
	sgl_uint8       devNum;
	sgl_uint8       fncNum;
	sgl_uint8       i;

	
	if (hSystem != 1)  return NULL;

	busNum = ((sgl_uint32)DeviceID & 0xFF00) >> 8;
	devNum = ((sgl_uint32)DeviceID & 0x00F8) >> 3;
	fncNum = ((sgl_uint32)DeviceID & 0x0007);

	/* this function sets up the register read write */
	/* functions to point to the device found        */

	retVal = PCISetDevice (busNum, devNum, fncNum);

	if (retVal) return NULL;

	retVal = PCIGetConfigSpace (&pciConfigSpace);

	if (retVal) return NULL;
	

	gBoardDataBlock.PCIBus                = pciConfigSpace.PciBusID;
	gBoardDataBlock.PCIDev                = pciConfigSpace.PciDevID;
	gBoardDataBlock.PCIFunc               = pciConfigSpace.PciFncID;
	gBoardDataBlock.PCIVendorID           = pciConfigSpace.VendorID;
	gBoardDataBlock.PCIDeviceID           = pciConfigSpace.DeviceID;
	gBoardDataBlock.RevisionID            = pciConfigSpace.RevisionID;
	gBoardDataBlock.IRQ                   = pciConfigSpace.InterruptLine;

	gBoardDataBlock.nMemWindows           = 0;
	gBoardDataBlock.nIOWindows            = 0;


	for (i = 0; i < MAX_MEM_IO_WINDOWS; i++)
	{
		pciRegAddr = 0x10 + (i << 2);
		
		if (pciConfigSpace.BaseAddress[i] != 0)
		{
			if ((pciConfigSpace.BaseAddress[i] & 0x01) == 0)
			{
				gBoardDataBlock.PhysMemWindows[i][0] = pciConfigSpace.BaseAddress[i] & 0xFFFFFFF0;	

				/* get pci to return its memory window size */	
				PCIWriteDword(pciRegAddr,0xFFFFFFFF);  
				PCIReadDword (pciRegAddr,&pciRetVal);  
				gBoardDataBlock.PhysMemWindows[i][1] = (~(pciRetVal & 0xFFFFFFF0)) + 1;
				PCIWriteDword(pciRegAddr,pciConfigSpace.BaseAddress[i]);  

				/* lock physical memory using DPMI */
				regs.w.ax=0x0800;
				regs.w.bx=HIWORD(gBoardDataBlock.PhysMemWindows[i][0]);
				regs.w.cx=LOWORD(gBoardDataBlock.PhysMemWindows[i][0]);
				regs.w.si=HIWORD(gBoardDataBlock.PhysMemWindows[i][1]);
				regs.w.di=LOWORD(gBoardDataBlock.PhysMemWindows[i][1]);
				int386( 0x31, &regs, &regs);
			
				if ( regs.x.cflag != 0 ) return NULL;

				gBoardDataBlock.LinearMemWindows[i] = (((sgl_uint32)regs.w.bx) << 16) + (sgl_uint32)regs.w.cx;

				gBoardDataBlock.nMemWindows++;
			}
			else
			{
				gBoardDataBlock.IOWindows[i][0] = pciConfigSpace.BaseAddress[i] & 0xFFFFFFFC;

				/* get pci to return its memory window size */
				PCIWriteDword(pciRegAddr,0xFFFFFFFF);  
				PCIReadDword (pciRegAddr,&pciRetVal);  
				gBoardDataBlock.IOWindows[i][1] = (~(pciRetVal & 0xFFFFFFFC)) + 1;
				PCIWriteDword(pciRegAddr,pciConfigSpace.BaseAddress[i]);

				gBoardDataBlock.nIOWindows++;
			}
		}
	}

	atexit (ExitHandler);
	
//    signal (SIGFPE, SIG_IGN);
    signal (SIGBREAK, SignalHandler);
    signal (SIGINT, SignalHandler);
    signal (SIGTERM, SignalHandler);
//    signal (SIGABRT, SignalHandler);
//    signal (SIGILL, SignalHandler);
//    signal (SIGSEGV, SignalHandler);

	#if !POLL_EOR
	if (SetupISR ())
	{
		return DeviceID;
	}
	else
	{
		return NULL;
	}
	#else
		return DeviceID;
	#endif
}


PVROSERR CALL_CONV PVROSGetPCIDeviceInfo (HSYSTEM hSystem, HDEVICE hDevice, BoardDataBlock *boardDataBlock)
{
	sgl_uint8       i;


	if (hSystem != 1)  return PVROS_DODGY;
	
	boardDataBlock->PCIBus                = gBoardDataBlock.PCIBus;
	boardDataBlock->PCIDev                = gBoardDataBlock.PCIDev;
	boardDataBlock->PCIFunc               = gBoardDataBlock.PCIFunc;
	boardDataBlock->PCIVendorID           = gBoardDataBlock.PCIVendorID;
	boardDataBlock->PCIDeviceID           = gBoardDataBlock.PCIDeviceID;
	boardDataBlock->RevisionID            = gBoardDataBlock.RevisionID;
	boardDataBlock->IRQ                   = gBoardDataBlock.IRQ;

	boardDataBlock->nMemWindows           = gBoardDataBlock.nMemWindows;
	boardDataBlock->nIOWindows            = gBoardDataBlock.nIOWindows;


	for (i = 0; i < MAX_MEM_IO_WINDOWS; i++)
	{
		boardDataBlock->PhysMemWindows   [i][0] = gBoardDataBlock.PhysMemWindows   [i][0];
		boardDataBlock->PhysMemWindows   [i][1] = gBoardDataBlock.PhysMemWindows   [i][1];
		
		boardDataBlock->LinearMemWindows [i]    = gBoardDataBlock.LinearMemWindows [i];

		boardDataBlock->IOWindows        [i][0] = gBoardDataBlock.IOWindows        [i][0];
		boardDataBlock->IOWindows        [i][1] = gBoardDataBlock.IOWindows        [i][1];
	}

	if (pStatusFlags == NULL)
	{
		/* set the status word address to a legal address. The contents
		   will always be zero if there is no ISR, which is the case
		   is pStatusFlags is NULL. Thus the code will then poll the
		   interrupt status register and won't write garbage to address 0 (!)
		*/
		boardDataBlock->pStatus = (PVR_STATUS *) &gBogusStatusFlags;
	}
	else
	{
		boardDataBlock->pStatus = (PVR_STATUS *) pStatusFlags;
	}
	
	return PVROS_GROOVY;
}


PVROSERR CALL_CONV PVROSClosePCIDevice (HSYSTEM hSystem, HDEVICE hDevice)
{
	if (hSystem == 1)
	{
//	    signal (SIGFPE, SIG_DFL);
	    signal (SIGBREAK, SIG_DFL);
	    signal (SIGINT, SIG_DFL);
	    signal (SIGTERM, SIG_DFL);
//	    signal (SIGABRT, SIG_DFL);
//	    signal (SIGILL, SIG_DFL);
//	    signal (SIGSEGV, SIG_DFL);

		return PVROS_GROOVY;
	}
	return PVROS_DODGY;
}


PVROSERR CALL_CONV PVROSSetupPowerVRDevice (HSYSTEM hSystem, HDEVICE hDevice, DEVICE_TYPE BoardType)
{
	#if !POLL_EOR
	if (hSystem == 1)  
	{
		return (EnableISR (BoardType));
	}

	return PVROS_DODGY;
	#else
	return PVROS_GROOVY;
	#endif
}

PVROSERR CALL_CONV PVROSReadPCIConfigSpace	(HDEVICE hDevice, sgl_uint32 dwRegNo, sgl_uint32 *dwRegValue)
{
	BoardDataBlock BDB;

	if (PVROSGetPCIDeviceInfo ((HSYSTEM) 1, hDevice, &BDB) == PVROS_GROOVY)
	{
		if (!PCISetDevice (BDB.PCIBus, BDB.PCIDev, BDB.PCIFunc))
		{
			PCIReadDword (dwRegNo, dwRegValue);  

			return PVROS_GROOVY;
		}
	}

	return PVROS_DODGY;
}

PVROSERR CALL_CONV PVROSWritePCIConfigSpace	 (HDEVICE hDevice, sgl_uint32 dwRegNo, sgl_uint32 dwRegValue)
{
	BoardDataBlock BDB;

	if (PVROSGetPCIDeviceInfo ((HSYSTEM) 1, hDevice, &BDB) == PVROS_GROOVY)
	{
		if (!PCISetDevice (BDB.PCIBus, BDB.PCIDev, BDB.PCIFunc))
		{
			PCIWriteDword (dwRegNo, dwRegValue);  

			return PVROS_GROOVY;
		}
	}

	return PVROS_DODGY;
}

/*********************/
/* DISPLAY FUNCTIONS */
/*********************/

sgl_uint32 CALL_CONV PVROSEnumDisplayDevices (DisplayDeviceTable *displayDeviceTable)
{
	if (displayDeviceTable == NULL)
	{
		return 1;
	}
	else
	{
		displayDeviceTable[0].ID              = (DISPLAYID)1;
		displayDeviceTable[0].DisplayVendorID = 0;
		displayDeviceTable[0].DisplayDeviceID = 0;
	}

	#if 0
	// not sure we need this really
	displayDeviceTable[1].ID = (DISPLAYID)0xFFFFFFFF;
	#endif

	return 1;
}


HDISPLAY CALL_CONV PVROSOpenDisplayDevice (DISPLAYID DisplayID, FRAME_BUFFER_MODE frameBufferMode)
{
	VbeInfoBlock VBEInfo;

	if (DisplayID != 1)
	{
		DPF ((DBG_ERROR, "PVROSOpenDisplayDevice: Bad display ID"));		
	}
	else
	{
		ReturnVBE2ControllerInformation (&VBEInfo);

		if ((VBEInfo.VbeSignature[0]+
			 VBEInfo.VbeSignature[1]+
			 VBEInfo.VbeSignature[2]+
			 VBEInfo.VbeSignature[3]) != ('V'+'E'+'S'+'A'))
		{
			DPF ((DBG_ERROR, "PVROSOpenDisplayDevice: Bad VESA signature"));
		}
		else
		{
			if (VBEInfo.VbeVersion < 0x0200)
			{
				DPF ((DBG_ERROR, "PVROSOpenDisplayDevice: Bad VBE version (%x)", VBEInfo.VbeVersion));
			}
			else
			{
				gFrameBufferMode = frameBufferMode;

				if (gFrameBufferMode == ACTIVE)
				{
					PVROSCallbackRegister (NULL, CB_PRE_RENDER,  PVROSPreRenderCallbackAuto,  NULL);
					PVROSCallbackRegister (NULL, CB_POST_RENDER, PVROSPostRenderCallbackAuto, NULL);
				}

				gStartupMode = GetCurrentVGAModeNumber ();

				DPF ((DBG_MESSAGE, "Current mode: 0x%x", gStartupMode));

				return (HDISPLAY)1;
			}
		}
	}
	
	return (0);
}


PVROSERR CALL_CONV PVROSCloseDisplayDevice (HDISPLAY hDisplay)
{
	if (hDisplay != 1) return PVROS_DODGY;

	if (gFrameBufferMode == ACTIVE)
	{
		PVROSCallbackUnRegister (NULL, CB_PRE_RENDER,  PVROSPreRenderCallbackAuto,  NULL);										
		PVROSCallbackUnRegister (NULL, CB_POST_RENDER, PVROSPostRenderCallbackAuto, NULL);
	}

	if (gStartupMode != 0xFFFFFFFF)
	{
		DPF ((DBG_MESSAGE, "Trying to reset to mode: 0x%x", gStartupMode));

	//	SetVBE2Mode (gStartupMode);
		gStartupMode = 0xFFFFFFFF;
	}

	return PVROS_GROOVY;
}


sgl_uint32 CALL_CONV PVROSEnumDisplayModes (HDISPLAY hDisplay, DisplayModeTable *displayModeTable)
{
	ModeInfoBlockTable  *ModeInfoTable;
	sgl_uint32           numberOfModes;
	sgl_uint32           i;


	if (hDisplay != 1) return 0;

	ModeInfoTable = malloc(EnumerateVBE2Modes(NULL));
	numberOfModes = EnumerateVBE2Modes(ModeInfoTable);

	if (displayModeTable == NULL)
	{
		free (ModeInfoTable);
		return numberOfModes;
	}
	else
	{
		for (i = 0; i < numberOfModes; i++)
		{
			displayModeTable[i].hMode       = (HMODE)ModeInfoTable[i].Mode;
			displayModeTable[i].XResolution = ModeInfoTable[i].XResolution;
			displayModeTable[i].YResolution = ModeInfoTable[i].YResolution;
			displayModeTable[i].BitDepth    = ModeInfoTable[i].BitsPerPixel;
		}

		#if 0
		// not sure we need this
		displayModeTable[numberOfModes].hMode = (HMODE)0xFFFFFFFF;
		#endif
	}

	free (ModeInfoTable);
	return numberOfModes;
}


PVROSERR   CALL_CONV PVROSGetDisplayModeInfo  (HDISPLAY hDisplay, DisplayModeDataBlock *displayModeDataBlock)
{
	union  REGS   regs;


	if (hDisplay  != 1) return PVROS_DODGY;
	if (!gModeInfoFlag) return PVROS_DODGY;

	displayModeDataBlock->DMT.XResolution         = gModeInfo.XResolution;
	displayModeDataBlock->DMT.YResolution         = gModeInfo.YResolution;
	displayModeDataBlock->DMT.BitDepth            = gModeInfo.BitsPerPixel;
	displayModeDataBlock->RefreshRate             = 0;
	displayModeDataBlock->RedMask                 = (((sgl_uint32)1 << gModeInfo.RedMaskSize  ) - 1) << gModeInfo.RedFieldPosition;
	displayModeDataBlock->GreenMask               = (((sgl_uint32)1 << gModeInfo.GreenMaskSize) - 1) << gModeInfo.GreenFieldPosition;
	displayModeDataBlock->BlueMask                = (((sgl_uint32)1 << gModeInfo.BlueMaskSize ) - 1) << gModeInfo.BlueFieldPosition;
	
	displayModeDataBlock->NumberOfBuffers         = gModeInfo.NumberOfImagePages + 1;
	displayModeDataBlock->CurrentBuffer           = 0;

	/* gBufferOffset should equal gModeInfo.OffScreenMemOffset */
	/* but nobody seems to set it up correctly in the BIOS */
	gBufferOffset = gModeInfo.BytesPerScanLine * gModeInfo.YResolution;

	if (gFixOffScreenMemOffset)
	{
		if (gFixOffScreenMemOffset == 1)
		{
			/* this is how it should be done */
			gBufferOffset = gModeInfo.OffScreenMemOffset;   
		}
		else
		{
			/* now were getting desperate    */
			int bytesPerPixel = 2;

			if (gModeInfo.BitsPerPixel ==  8) bytesPerPixel = 1;
			if (gModeInfo.BitsPerPixel == 15) bytesPerPixel = 2;
			if (gModeInfo.BitsPerPixel == 16) bytesPerPixel = 2;
			if (gModeInfo.BitsPerPixel == 24) bytesPerPixel = 3;
			if (gModeInfo.BitsPerPixel == 32) bytesPerPixel = 4;

			gBufferOffset = gModeInfo.XResolution * gModeInfo.YResolution * bytesPerPixel;
		}
	}
        
	/* DPMI call 800h physical address mapping */
	regs.w.ax=0x0800;
	regs.w.bx=HIWORD(gModeInfo.PhysBasePtr);
	regs.w.cx=LOWORD(gModeInfo.PhysBasePtr);
	regs.w.si=HIWORD(gBufferOffset);
	regs.w.di=LOWORD(gBufferOffset);
	int386( 0x31, &regs, &regs);
	if ( regs.x.cflag != 0 ) return PVROS_DODGY;

	displayModeDataBlock->PhysBufferAddress   [0] = gModeInfo.PhysBasePtr;
	displayModeDataBlock->LinearBufferAddress [0] = (((sgl_uint32)regs.w.bx) << 16) + (sgl_uint32)regs.w.cx;


	/* DPMI call 800h physical address mapping */
	regs.w.ax=0x0800;
	regs.w.bx=HIWORD(gModeInfo.PhysBasePtr + gBufferOffset);
	regs.w.cx=LOWORD(gModeInfo.PhysBasePtr + gBufferOffset);
	regs.w.si=HIWORD(gBufferOffset);
	regs.w.di=LOWORD(gBufferOffset);
	int386( 0x31, &regs, &regs);
	if ( regs.x.cflag != 0 ) return PVROS_DODGY;

	displayModeDataBlock->PhysBufferAddress   [1] = gModeInfo.PhysBasePtr + gBufferOffset;     
	displayModeDataBlock->LinearBufferAddress [1] = (((sgl_uint32)regs.w.bx) << 16) + (sgl_uint32)regs.w.cx;


	/* DPMI call 800h physical address mapping */
	regs.w.ax=0x0800;
	regs.w.bx=HIWORD(gModeInfo.PhysBasePtr + gBufferOffset + gBufferOffset);
	regs.w.cx=LOWORD(gModeInfo.PhysBasePtr + gBufferOffset + gBufferOffset);
	regs.w.si=HIWORD(gBufferOffset);
	regs.w.di=LOWORD(gBufferOffset);
	int386( 0x31, &regs, &regs);
	if ( regs.x.cflag != 0 ) return PVROS_DODGY;

	displayModeDataBlock->PhysBufferAddress   [2] = gModeInfo.PhysBasePtr + gBufferOffset + gBufferOffset;
	displayModeDataBlock->LinearBufferAddress [2] = (((sgl_uint32)regs.w.bx) << 16) + (sgl_uint32)regs.w.cx;


	return PVROS_GROOVY;
}

PVROSERR CALL_CONV PVROSSelectDisplayMode (HDISPLAY hDisplay, HMODE hMode, sgl_uint32 nBuffers)
{
	if (hDisplay != 1) 
	{
		DPF ((DBG_ERROR, "Bad display context"));
	}
	else
	{
		if (ReturnVBE2ModeInformation ((sgl_uint16) hMode, &gModeInfo))
		{
			DPF ((DBG_ERROR, "Error getting mode info"));
		}
		else
		{
			if ((gModeInfo.NumberOfImagePages + 1) < nBuffers) 
			{
				return PVROS_TOO_MANY_BUFFERS;
			}

			if (SetVBE2Mode ((sgl_uint16) hMode))
			{
				DPF ((DBG_ERROR, "Error setting mode"));
			}
			else
			{
				gFrontBuffer  = 0;
				gBackBuffer   = nBuffers - 1;

				gnBuffers     = nBuffers;
				gModeInfoFlag = 1;

				return PVROS_GROOVY;
			}
		}
	}

	return PVROS_DODGY;
}


PVROSERR CALL_CONV PVROSFlipFrameBuffer (HDISPLAY hDisplay)
{
	if (hDisplay == 1)
	{
		if (gFrameBufferMode == ACTIVE)
		{
			if (gnBuffers > 1)
			{
				if (!SetVBE2DisplayStart (0, gModeInfo.YResolution * gBackBuffer, gWaitForVSync))
				{
					gFrontBuffer++;
					gBackBuffer++;

					if (gFrontBuffer == gnBuffers)
					{
						gFrontBuffer = 0;
					}
					
					if (gBackBuffer == gnBuffers)
					{
						gBackBuffer = 0;
					}

					// successfully flipped
					return PVROS_GROOVY;
				}
			}
			else
			{
				// single buffering - no flip necessary
				return PVROS_GROOVY;
			}
		}
	}

	return PVROS_DODGY;
}


/**********************/
/* CALLBACK FUNCTIONS */
/**********************/

PVROSERR CALL_CONV PVROSPreRenderCallbackAuto (void *pData, HDISPLAY hDisplay)
{
	PRE_RENDER_CB_STRUCT  *pPRCS = pData;
	
	DPF ((DBG_VERBOSE, "PVROSPreRenderCallbackAuto"));	

	ASSERT (pPRCS);

	#pragma message ("This needs to fixed in sgltri_render")
	
	if (hDisplay != 1)
	{
		hDisplay = (HDISPLAY) 1;
	}
	
	if (hDisplay == 1)
	{		
		if (pPRCS->FlipRequested)
		{
			if (PVROSFlipFrameBuffer (hDisplay) != PVROS_GROOVY)
			{
				return (PVROS_DODGY);
			}
		}

		if (gBufferOffset == NULL)
		{
			DisplayModeDataBlock displayModeDataBlock;
		
			if (PVROSGetDisplayModeInfo  (hDisplay, &displayModeDataBlock) == PVROS_GROOVY)
			{
				pPRCS->PhysRenderBufferAddress = displayModeDataBlock.PhysBufferAddress[gBackBuffer] ;
			}
			else
			{
				return (PVROS_DODGY);
			}
		}
		else
		{
			pPRCS->PhysRenderBufferAddress = gModeInfo.PhysBasePtr + (gBufferOffset * gBackBuffer);
		}
					
		pPRCS->PhysRenderBufferBitsPerPixel = gModeInfo.BitsPerPixel;
		pPRCS->PhysRenderBufferStride       = gModeInfo.BytesPerScanLine;

		return (PVROS_GROOVY);	
	}
			
	return (PVROS_DODGY);	
}
	
PVROSERR CALL_CONV PVROSPostRenderCallbackAuto (void *pData, HDISPLAY hDisplay)
{
	return (PVROS_GROOVY);	
}	

void CALL_CONV PVROSDelay (PVR_DELAY eDelay, sgl_uint32 uCount)
{
	switch (eDelay)
	{
		case PVR_DELAY_BUS_CYCLES:
		{
			UINT32 k;
			for (k = 0; k < uCount; ++k);
			break;
		}

		case PVR_DELAY_MS:
		default:
		{
			delay (uCount);
			break;
		}
	}
}

void CALL_CONV PVROSPrintf (char* pszFormat, ...)
{
    va_list arglist;
	char    buffer[512];

	
	#if DEBUG
	mdaPrint ("SGL: ");
	#endif

    va_start (arglist, pszFormat);
    vsprintf (buffer,pszFormat, arglist);
    va_end   (arglist);

	#if DEBUG
	mdaPrint (buffer);
	#endif
}

void CALL_CONV PVROSAssert (char *szAssertText, char *szFileName, INT32 nLine, int * ask)
{
	char    buffer[512];

    sprintf (buffer, "ASSERT FAILED File: %s, line %d\n%s\n", szFileName, nLine, szAssertText);
	mdaPrint (buffer);

	Int3 ();	

	*ask = FALSE;
}



/*
// Note: For the following functions to operate correctly, you need a genuine
// INTEL processor. It crashes on a Cyrix since it doesn't have these commands.
// I frankly could not be bothered trying to make it work in all cases, so I'm
// going to leave it unmodified!
*/
#define THIS_CRASHES_A_CYRIX 0  /*0 disables the dodgy code*/

sgl_uint32 CALL_CONV PVROSQueryPerformanceCounter(sgl_largeint* Val)
{
#if THIS_CRASHES_A_CYRIX
	RDMSR(Val);
#endif
	return(TRUE);
}

sgl_uint32 CALL_CONV PVROSQueryPerformanceFrequency(sgl_largeint* Val)
{
#if THIS_CRASHES_A_CYRIX
	ResetTSC();
#endif
	PVROSDelay(PVR_DELAY_MS,1000); /* 1 second pause */
	PVROSQueryPerformanceCounter(Val);

	return(TRUE);	
}


/***********************************************/
/* Mono Display Adapter Debug Output Functions */
/***********************************************/


void mdaPrint (char *pString)
{
	unsigned char 	*pMDA;
	static int     	col = 0;
	int            	offset;


	pMDA = (unsigned char *)0xB0000;
	
	while (*pString != '\0')
	{
		if (col == 80)
		{
			col = 0;
			mdaNewLine();
		}

		offset = ((24 * 80) + col) * 2;

		switch (*pString)
		{
			case '\n' :
				col = 0;
				mdaNewLine();
			break;

			case '\r' :
				col = 0;
			break;

			case '\t' :
				*(pMDA + offset) = ' ';
				col++;
			break;

			default :
				*(pMDA + offset) = *pString;
				col++;				
			break;
		}

		pString++;
	}
}


void mdaNewLine (void)
{
	unsigned char *pMDA;
	int            i = 0;
	int            j = 0;
	int            offset1;
	int            offset2;


	pMDA = (unsigned char *)0xB0000;

	for (i = 1; i < 25; i++)
	{
		for (j = 0; j < 80; j++)
		{

			offset1 = (((i-1) * 80) + j) * 2;
			offset2 = ((  i   * 80) + j) * 2;

			*(pMDA + offset1) = *(pMDA + offset2);
		}
	}

	for (i = 0; i < 80; i++)
	{
		offset1 = ((24 * 80) + i) * 2;

		*(pMDA + offset1) = ' ';
	}

}

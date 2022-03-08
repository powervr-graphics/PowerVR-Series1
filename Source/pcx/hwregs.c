/******************************************************************************
 * Name : hwregs.c
 * Title : 
 * Author : Stel Michael
 * Created : 27/06/1995
 *
 * Copyright	: 1995-2022 Imagination Technologies (c)
 * License		: MIT
 *
 * Description : hardware register setting  functions for real HW.
 *
 * Platform : ANSI compatible
 *
 * Modifications:-
 * $Log: hwregs.c,v $
 * Revision 1.85  1997/11/03  11:42:11  sks
 * Removed a bit off non-working code.
 *
 * Revision 1.84  1997/10/22  11:01:20  sxy
 * added timing for waiting for HW in HWStartRender().
 *
 * Revision 1.83  1997/10/16  11:32:25  sxy
 * removed a DPFDEVF, its all in DPFDEV.
 *
 * Revision 1.82  1997/10/15  16:07:00  sxy
 * added DPFDEVF messages.
 *
 * Revision 1.81  1997/10/14  16:50:13  gdc
 * put in code to free buffers if address mode callback fails so that render
 * is cleanly skipped
 *
 * Revision 1.80  1997/10/02  10:08:51  gdc
 * changed sense of bilinear flags to OFF in ON and Black is White
 * cause this is the way the HAL/PVR Page does it
 *
 * Revision 1.79  1997/09/29  11:12:26  sxy
 * added a DPFDEV message.
 *
 * Revision 1.78  1997/09/24  14:45:26  gdc
 * made new ini file reads for bilinear behave like d3d
 *
 * Revision 1.77  1997/09/24  10:32:01  gdc
 * changed HWRdavalFile's to a new function which has a revised priority
 * of where to get values, and changed bilnear override to be like D3D (a bit)
 *
 * Revision 1.76  1997/09/10  17:09:13  erf
 * Added dither control when set the bits per pixel.
 *
 * Revision 1.75  1997/08/28  16:44:37  erf
 * Allow the user to change the ARBMODE register through the registry or
 * sglhw.ini .
 *
 * Revision 1.74  1997/07/24  17:09:36  gdc
 * chnaged HWStartRender to take in HDISPLAY and call 2d callback
 * also removed ^M's
 *
 * Revision 1.73  1997/07/21  10:25:55  gdc
 * removed an unused function
 *
 * Revision 1.72  1997/07/02  14:43:38  jrc
 * Fixed bug where EndOfRender callback was being called before the first
 * render was started!
 *
 * Revision 1.71  1997/06/18  14:46:09  mjg
 *  Replaced Windows types (UINT and the rest) with sgl_ typedefs.
 *
 * Revision 1.70  1997/06/11  16:35:52  mjg
 * Added HWStartRenderStrip and lots of things to HWStartRender.
 *
 * Revision 1.69  1997/06/11  12:36:26  jop
 * Reorganised render commands
 *
 * Revision 1.68  1997/06/04  10:21:23  mjg
 * Passed the Logical Device to the callback.
 *
 * Revision 1.67  1997/06/03  09:21:14  erf
 * Compiled out register initialisation of registers which are only
 * initialised once in the VXD for Window builds.
 *
 * Revision 1.66  1997/05/30  16:41:00  mjg
 * Downgraded the ASSERTs in HWSetXClip to more literate messages.
 *
 * Revision 1.65  1997/05/22  09:00:54  erf
 * Fixed ForceBitsPerPixel.
 *
 * Revision 1.64  1997/05/21  17:21:57  sxy
 * Make sure PVROSCallback() can be called again in HWFinishedRender().
 *
 * Revision 1.63  1997/05/21  17:15:06  erf
 * Added ForceBitsPerPixel setting.
 *
 * Revision 1.62  1997/05/19  13:13:11  jop
 * Fixed SetupBunchOfISPRegs so that it doesn't write into non-
 * existent virtual registers
 *
 * Revision 1.61  1997/05/15  20:16:16  jop
 * Fixed HWFinished render for logical contexts
 *
 * Revision 1.60  1997/05/15  12:39:27  erf
 * Changed texture filtering logic, in accordance with new approach.
 * ie point sampling is the default, the sglhw.ini setting
 * BilinearFilteringMode uses the same settings as defined in sgl.h for
 * the texture filtering setting.
 * See John C. E-mail on the matter.
 *
 * Revision 1.59  1997/05/14  18:47:08  gdc
 * win32'd out the set sabre pt function - this is now done on virtual
 * buffer assignment
 *
 * Revision 1.58  1997/05/14  03:42:06  jop
 * Removed WriteVirtualReg and merged PCX1/PCX2 functionality
 *
 * Revision 1.57  1997/05/07  09:47:28  mjg
 * Temporary measures to get the PCX1 build going.
 *
 * Revision 1.56  1997/04/25  10:35:06  gdc
 * updated to current pcx2 std - untested as yet
 *
 * Revision 1.55  1997/04/23  20:28:32  mjg
 * New PVROS function calls - has even been known to work!
 * ,
 *
 * Revision 1.54  1997/04/22  18:08:42  mjg
 * Tidy up
 *
 * Revision 1.53  1997/04/21  15:33:30  mjg
 * Replaced ParamBufferInfo with PVRParamBuffs.
 *
 * Revision 1.52  1997/04/16  13:30:55  jop
 * Removed DOS interrupt retriggering
 *
 * Revision 1.51  1997/04/15  13:01:38  mjg
 * Tidied up a bit.
 *
 * Revision 1.50  1997/04/10  19:56:33  mjg
 * Removed the redundant HWScheduleScreenSwap function.
 *
 * Revision 1.49  1997/04/09  02:52:01  jop
 * Added real mode ISR retriggering
 *
 * Revision 1.48  1997/04/08  13:52:25  jop
 * Cleaned up includes a bit
 *
 * Revision 1.47  1997/04/04  19:39:38  mjg
 * Moved pvrosapi.h
 *
 * Revision 1.46  1997/04/04  13:17:56  mjg
 * Changed a UINT32 to sgl_uint32.
 *
 * Revision 1.45  1997/04/03  18:00:42  mjg
 * Added a DOS version of GetEnvironment.
 *
 * Revision 1.44  1997/04/03  15:30:50  mjg
 * Fixes for DOSing.
 *
 * Revision 1.43  1997/04/03  13:08:12  mjg
 * Updated the calling convention.
 *
 * Revision 1.42  1997/04/02  18:13:56  gdc
 * chnaged includes to be compatable with PVROS stuff and chnaged and
 * internal return value to be suitable with new stuff
 *
 * Revision 1.41  1997/04/01  18:41:24  mjg
 * Changed notprintf to PVROSPrintf.
 *
 * Revision 1.40  1997/03/31  22:12:28  jop
 * Sorted out WIN32/DOS32 issues
 *
 * Revision 1.39  1997/03/29  22:09:24  mjg
 * Inserted PVROSDelay.
 *
 * Revision 1.38  1997/03/28  14:54:56  mjg
 * PCX1 update for SGL4, includes all the PVROS re-workings.
 *
 * Revision 1.37  1997/03/26  06:39:26  erf
 * Removed stuff which allows sglmid4.dll to initialise PCX2 features.
 *
 * Revision 1.36  1997/03/25  10:04:05  erf
 * 1. Added routine to write the PCX2 bilinear register.
 * 2. Added filtering override control.
 * 3. Added variables to indicate which filtering has been selected
 *    and if override has been selected.
 *
 * Revision 1.35  1997/03/14  10:03:48  erf
 * Change StartIndex for TSP parameters from 12 to 14 to fix flat
 * shaded highlight problem. This is not the best way of solving
 * this problem, but it works for egg and twist.
 *
 * Revision 1.34  1997/02/12  11:05:18  erf
 * Starting TSP id is now 12. Allows for full textured masking plane.
 *
 * Revision 1.33  1997/01/28  12:56:03  erf
 * Set MEMCLK for SDRAM to 0 for PCX2 hardware.
 *
 * Revision 1.32  1997/01/16  16:01:37  gdc
 * included extern declar for PCXRegionDataPages so that correct PCX pointer
 * is set-up
 *
 * Revision 1.31  1997/01/14  12:58:21  erf
 * Enable PowerDown of internal RAM between end and start of render
 * for PCX2 hardware.
 *
 * Revision 1.30  1997/01/10  16:17:27  erf
 * Added PCX2 hardware register initialisation for running sglmid4.dll
 * on PCX2 hardware:
 * 1. Bilinear filtering options.
 * 2. Dithering at 16 bits per pixel.
 * 3. Set ARBMODE register to 0xD.
 * 4. Disable PCX2 software specific options.
 * 5. Set CLK_SELECT register to 0x3.
 * 6. Enable SDRAM caching.
 *
 * Revision 1.29  1996/12/20  16:25:32  erf
 * Add PCX2 sglhw.ini settings conditions to enable/disable features.
 *
 * Revision 1.28  1996/12/12  10:07:19  erf
 * Changed #ifdef WIN32 and #if defined(WIN32) to #if WIN32.
 *
 * Revision 1.27  1996/12/11  01:19:46  jop
 * Removed ../'s from includes
 *
 * Revision 1.26  1996/11/08  12:02:21  erf
 * Added PVR3 compile switch for PCX2 initialisation.
 * Added define SHOW_POPUPS around MessageBox() routines to prevent
 * calls in D3D.
 *
 * Revision 1.25  1996/10/29  18:00:25  jrc
 * Added HWSetXClip function.
 *
 * Revision 1.24  1996/10/28  12:58:07  gdc
 * added facility to change how much texture memory is
 * allocated to TSP parameter space via an ini file setting
 *
 * Revision 1.23  1996/10/25  09:58:52  mal
 * Added an ini file setting for turning on and off
 * Bilinear Filtering for PCX2
 *
 * Revision 1.22  1996/10/16  21:19:39  jop
 * Added PCX region data duoble buffer supporrt
 * ,
 *
 * Revision 1.21  1996/10/09  18:06:39  jop
 * Added region data offset parameter
 *
 * Revision 1.20  1996/09/19  12:00:45  gdc
 * added code to allow control over how long render takes to timeout
 * via an entry in the sglhw.ini file (see d
 * sorry ... (see sgl_init.c)
 *
 * Revision 1.19  1996/09/05  20:41:01  jop
 * Rmoved printfs
 *
 * Revision 1.18  1996/09/04  09:51:57  mal
 * Added some Sleep(10) statements to delay the read and writes
 * to Texture Memory. This should improve the TextureMemorySize
 * detection algorithm.
 *
 * Revision 1.17  1996/07/12  14:14:16  ncj
 * Added 24 bit direct draw support
 *
 * Revision 1.16  1996/07/09  11:45:38  mal
 * Modified DetermineTexMemConfig and making it more stringent in the
 * tmem address aliases and their values.
 *
 * Revision 1.15  1996/07/09  08:24:00  mal
 * Modified ARBMODE default to 0x00000001
 *
 * Revision 1.14  1996/07/03  08:14:14  mal
 * Changed TMEM_SETTINGS to #defines
 *
 * Revision 1.13  1996/06/27  16:59:08  ncj
 * status display now works with pcx
 *
 * Revision 1.12  1996/06/27  13:09:51  ncj
 * Added pcx1 555 mode detection
 *
 * Revision 1.11  1996/06/24  10:17:01  mal
 * Modified DetermineTextureMemorySize()'s algorithm for
 * detecting the type and size of texture memory.
 *
 * Revision 1.10  1996/06/21  14:12:51  mal
 * Added mor TMEM_SETUPs inside #ifdef WIN32
 *
 * Revision 1.9  1996/06/21  13:48:27  mal
 * Added TextureMemorySize setting inside #ifdef WIN32 for
 * sglhw.ini file setting.
 *
 * Revision 1.8  1996/06/21  11:03:43  mal
 * Modified the TMEM Setup, by adding DetermineTextureMEmorySize().
 *
 * Revision 1.7  1996/06/19  09:03:48  mal
 * Added 1,2 and 4 Meg Texture Memory setup
 *
 * Revision 1.6  1996/06/10  14:14:29  jop
 * added brdsetup.h
 *
 * Revision 1.5  1996/05/07  14:19:03  jop
 * Made some changes I can't remember anything about
 *
 * Revision 1.4  1996/04/30  18:08:07  jop
 * More fixes for PCX1
 *
 * Revision 1.3  1996/04/26  17:34:40  pdl
 * made it compilable.
 *
 * Revision 1.2  1996/04/26  16:24:05  pdl
 * lots.
 *
 * Revision 1.1  1996/04/17  17:44:38  pdl
 * Initial revision
 *
 *
 *****************************************************************************/
#define MODULE_ID MODID_HWREGS

/* memory configuration now set in sglhw.ini */

#include "sgl_defs.h"
#include "hwinterf.h"
#include "fogtable.h"
#include "rectable.h"
#include "profile.h"
#include "pvrosapi.h"
#include "hwregs.h"
#include "brdcfg.h"
#include "pci.h"
#include "brdsetup.h"
#include <time.h>

/* JWF added following headers */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#if WIN32
#include <windows.h> /* For MEM_COMMIT and PAGE_READWRITE */
#endif

#include "pvrlims.h"

#include "metrics.h"
SGL_EXTERN_TIME_REF /* if we are timing code */

sgl_uint32 DetermineTexMemConfig( sgl_uint32 uSettings );
PVROSERR CALL_CONV PVROSSetPCIPixelFormat(sgl_uint16 wBitsPerPixel, sgl_bool bDither);

extern HLDEVICE gHLogicalDev;

#if MAC
#include "Null Functions.h"
#include "DriverServices.h"
extern unsigned long	gISPChunk;		/* base 16Mb addr of ISP objs */

#define INITPCXREG(x,y) __stwbrx( SglReadPrivateProfileInt("PCXRegs", #x, y, GetSGLHWIni()), 0, (int)(pSCBusIOSpace+x))
#else
#define INITPCXREG(x,y)	gHLogicalDev->Registers[x] = SglReadPrivateProfileInt("PCXRegs", #x, y, GetSGLHWIni())
#endif

/* Texture Memory Settings for the different configurations */
#define TMEM_4M_SETTING 	0x22212275 
#define TMEM_2M_SETTING 	0x12212275 
#define TMEM_1M_SETTING 	0x02211275 

/*
// Specify the sizes of parameter space. (num of 32 bit words)
// These are bit a nasty at the mo' .. must fix with better external consts
//
// These are set up assuming double buffering in local memory...
// (even though we aren't)
*/

#if !WIN32
static	sgl_uint32	* pTexasStore;
static	sgl_uint32	* pSabreStore;
static	sgl_uint32	* pSWTexasStore;

static	sgl_uint32	SabreMemLimit;
static	sgl_uint32	TexasMemLimit;			
static	sgl_uint32	TexasMemStart;

sgl_uint32 TexParamSize;

static	sgl_bool	Allocated=FALSE;	
#endif

int		nForceBPP=0;

static clock_t		RenderStartTime;

static sgl_uint32	TextureMemorySize=0;

extern sgl_uint16	wPCIDeviceID;

/* Status display */
int nLockUpCount=0;
char szLockupTxt[80];

int nHWWait=0;
char szHWWaitTxt[80];

char szSglRenderTime[80];

extern int 		nTimeOutTenths; 
static clock_t	RenderStartTime;
static sgl_bool	TimedOut = FALSE; /*so we know whether to swap buffers*/
static sgl_bool	bEndOfRender=TRUE;

extern int PCXRegionDataPages;

/*
	extern sgl_uint16 wSurfaceBitsPerPixel;
	volatile sgl_uint32 * volatile pEndOfRender 	= NULL; 
*/

/* Look up table for the texture filtering mode.
 * Used to convert the set filter mode to the hardware required
 * value. Super sampling which isn't supported yet goes to bilinear
 * filtering.
 */
static	sgl_uint32 eFilterLookUp[4]	= {0x3,0x1,0x0,0x0};

/*******************/

void CALL_CONV HWSetFogRegister(sgl_uint32	FogShiftValue)
{
	gHLogicalDev->Registers[PCX_FOGAMOUNT] = FogShiftValue;	
}

/*******************/

void CALL_CONV HWSetXClip( sgl_bool  bClipLeft,
				 int  nLeft,
				 sgl_bool bClipRight,
				 int  nRight )
{
	sgl_uint32  u32Value = 0;

	if (bClipLeft)
	{
		if (nLeft < 1)
		{
			DPF((DBG_MESSAGE,"HWSetXClip: Increasing Left value from %d to 1",nLeft));
			nLeft = 1;
		}
		else if (nLeft > 1023)
		{
			DPF((DBG_MESSAGE,"HWSetXClip: Reducing Left value from %d to 1023",nLeft));
			nLeft = 1023;
		}

		u32Value = 1 << 12 | nLeft;
	}

	if (bClipRight)
	{
		if (nRight < 1)
		{
			DPF((DBG_MESSAGE,"HWSetXClip: Increasing Right value from %d to 1",nRight));
			nRight = 1;
		} 
		else if (nRight > 1023)
		{
			DPF((DBG_MESSAGE,"HWSetXClip: Reducing Right value from %d to 1023",nRight));
			nRight = 1023;
		}

		if (nRight < nLeft)
		{
			DPF((DBG_MESSAGE,"HWSetXClip: Right value was (%d) less than Left (%d)",nRight,nLeft));
			nRight = nLeft;
		}

		u32Value |= 1 << 28 | nRight << 16;
	}

	gHLogicalDev->Registers[PCX_XCLIP] = u32Value;

} /* HWSetXClip */


/*******************/

#if !WIN32

void CALL_CONV HWSetSabPtrRegister(const sgl_uint32 ConstRegionBase, const sgl_uint32 ConstRegionOffset)
{
#if POINTERS_IN_TLB

	/* Need to enable ISP object pointers thru TLB.
	 * Must specify offset in number of bytes !!!! Not in 32bit pointer values.
	 */

	#if POINTERS_FOLLOW_PLANES

		/* The pointers start immediately the planes finish. dangerous if there are a lot */

		gHLogicalDev->Registers[PCX_OBJECT_OFFSET] = ((ConstRegionBase + ConstRegionOffset) << 2) | 0x00000001UL;

	#else

		/* pointers have their own part of the TLB to themselves */
		
		gHLogicalDev->Registers[PCX_OBJECT_OFFSET] = (1*1024*1024) | 0x00000001UL;

	#endif

#else

	gHLogicalDev->Registers[PCX_OBJECT_OFFSET] = ConstRegionOffset;

#endif
}

#endif

/*******************/

sgl_bool CALL_CONV HWFinishedRender(void)
{
	if (!bEndOfRender)
	{
		/*
		// Has an end of render been reported by the ISR ?
		*/
		if (gHLogicalDev->RenderStatus == PVR_STATUS_EOR)
		{
			bEndOfRender = TRUE;

			/* Detected Hardware end-of-render */
			PVROSCallback (gHLogicalDev, CB_END_OF_RENDER, NULL);

			return TRUE;
		}

		return FALSE;
	}
	else
	{
		return TRUE;
	}
}

 /*******************/

void CALL_CONV HWReset(void)
{
/* Apparently this doesn't work,
// so out it goes.
*/
#if 0
	gHLogicalDev->Registers[PCX_SOFTRESET] = 1;
	
	#if MAC
	SynchronizeIO();
	#endif
	
	gHLogicalDev->Registers[PCX_SOFTRESET] = 0;

	#if MAC
	SynchronizeIO();
	#endif

	/* 	we now have to wait for at least 64 clock cycles*/

	PVROSDelay(PVR_DELAY_BUS_CYCLES, 0x100);
#endif
}

 /*******************/
void CALL_CONV HWStartRender(sgl_bool bFlipRequested, HDISPLAY hDisplay, sgl_bool bDither)
{
	PRE_RENDER_CB_STRUCT PRCS;

	SGL_TIME_SUSPEND(TOTAL_RENDER_TIME);
	SGL_TIME_START(RENDER_WAITING_TIME);

	while (!HWFinishedRender ())
	{
		/* Do nothing */
	}
	
	SGL_TIME_STOP(RENDER_WAITING_TIME);
	SGL_TIME_RESUME(TOTAL_RENDER_TIME);

	if(hDisplay)
	{
		PVROSCallback (gHLogicalDev, CB_2D, hDisplay);
	}

	PRCS.FlipRequested = bFlipRequested;

	/* Once we're sure the previous render has finished - start all over again */
	if (PVROSCallback (gHLogicalDev, CB_PRE_RENDER, &PRCS) != PVROS_GROOVY)
	{
		DPFDEV ((DBG_ERROR, "PVROSCallback - PRE_RENDER failed"));
									
		PVROSFreeVirtualBuffers(gHLogicalDev);
		bEndOfRender = FALSE;

	}
	else
	{
		PVROSSetPCIPixelFormat ((sgl_uint16) PRCS.PhysRenderBufferBitsPerPixel, bDither);

		ProgramSOFAddressReg (PRCS.PhysRenderBufferAddress);

		ProgramStrideReg (PRCS.PhysRenderBufferStride);
		
		PVROSScheduleRender (gHLogicalDev);
	}

	bEndOfRender = FALSE;
}

/*****************/
void CALL_CONV HWStartRenderStrip()
{
	PVROSScheduleRender (gHLogicalDev);
	bEndOfRender = FALSE;
}

/**************************************************************************
* Function Name  : HWGetParamMemRefs
* Inputs         : none
* Outputs        : Parameter Buffer management structure 
*
* Returns        : TRUE if a buffer was allocated.
*
* Global Used    : Local allocation "flag" values
*
* Description    : This returns a set of pointers and indices to manage
*				   Hardware parameter buffers.
*
**************************************************************************/

#if !WIN32

sgl_bool CALL_CONV HWGetParamMemRefs (PVR_PARAM_BUFF *pPVRParamBuf)
{
	/* ISP parameters */

	pPVRParamBuf[PVR_PARAM_TYPE_ISP].pBuffer		= pSabreStore;
	pPVRParamBuf[PVR_PARAM_TYPE_ISP].uBufferPos		= 0;
	pPVRParamBuf[PVR_PARAM_TYPE_ISP].uBufferLimit	= (SabreMemLimit >> 1) - 1; 

	/* TSP parameters - NOTE all Texas indexes MUST be even */

   	pPVRParamBuf[PVR_PARAM_TYPE_TSP].pBuffer			= pSWTexasStore;
	
	/* Used when problem with mask plane in hardware. Index greater since
	 * full textured plane required to slow down FIFO in hardware.
	 *
	 * Increment StartIndex from 12 to 14. Eventhough this wastes a TSP tag it
	 * prevents the flat shaded highlight problem.
	 */
   	
	pPVRParamBuf[PVR_PARAM_TYPE_TSP].uBufferPos		= 14; /* in PCX2 Tag ID 1 & 2 reserved */
   	pPVRParamBuf[PVR_PARAM_TYPE_TSP].uBufferLimit	= TexParamSize >> 2;

	/* Region data buffer */

	pPVRParamBuf[PVR_PARAM_TYPE_REGION].pBuffer 		= PVROSGetISPObjectData();
	pPVRParamBuf[PVR_PARAM_TYPE_REGION].uBufferPos		= 0;
	pPVRParamBuf[PVR_PARAM_TYPE_REGION].uBufferLimit 	= (PVROSGetPCXRegionDataPages() << 12) - 1;

	return (TRUE);
}
#endif

/**************************************************************************
* Function Name  : HWInitParamMem
* Inputs         : 
* Outputs        : None
*
* Returns        : 0 if OK.
*
* Global Used    : lots
*
* Description    : Initialises the parameter memory and control flags.
*
**************************************************************************/
#if !WIN32

int	CALL_CONV HWInitParamMem()
{
	int err = -1;
	PCXBUFFER *PCXBuffer;

	if (!Allocated)
	{
		/* Get buffer sizes and pointers */

	 	PCXBuffer = PVROSGetPCXBuffer();
		pSabreStore = (unsigned long *) PCXBuffer->LinearAddress;

	   	pTexasStore = (sgl_uint32 *) PVROSGetVirtualBurstSpace();

		#if WIN32
		pSWTexasStore = VirtualAlloc (NULL, TexParamSize, MEM_COMMIT,	PAGE_READWRITE);
		#elif DOS32
		pSWTexasStore = PVROSMalloc (TexParamSize + 4096);
		pSWTexasStore = ((sgl_uint32) pSWTexasStore + 4095) & 0xfffff000; /* Page align */
		#else
		pSWTexasStore = PVROSMalloc (TexParamSize);
		#endif
		
		if (pSWTexasStore)
		{
			/* 1 page of physical memory in a PC is 4096 bytes.
			   TLB pages are 1 page, 2 pages or 4 pages.
			   so each TLB slot is (in sgl_uint32s): 1024 * no. of slot pages
			 */
		 
			SabreMemLimit = PCXBuffer->SlotsUsed * (1024 << PCXBuffer->PageSize);

		   	TexasMemLimit = TexParamSize >> 2;
		   	TexasMemStart = 0;

			/* i assume the software bufferes are not required for pcx1 */
			
			Allocated = TRUE;

			err = 0;
		}
	}
	
    return err;
}
#endif

char *GetSGLHWIni ()
{
	static char SGLHWIni[128] = "\0";

	if (SGLHWIni[0] == '\0')
	{
		#if WIN32
		GetEnvironmentVariable ("WINDIR", SGLHWIni, sizeof (SGLHWIni));
		#elif DOS32
		{
			char *cName;
		
			cName = getenv("WINDIR");
			if (cName)
			{
				strcpy (SGLHWIni, cName);
			}			
		}
		#endif
		strcat (SGLHWIni, "\\sglhw.ini");
	}

	return (SGLHWIni);
}

/**********************************************************************/
/* Variables required to set the bilinear filtering mode.
 */

static sgl_uint32	uBilinearReg		= 0x0;
static sgl_bool		bFilteringOverRide	= FALSE;
static sgl_bool		bBilinearEnabled	= FALSE;

/******************************************************************************
 * Function Name: HWSetupBunchOfISPRegs
 *
 * Inputs       : -
 * Outputs      : -
 * Returns      : -
 * Globals Used : 
 *
 * Description  : This initialises a number of registers that can be modified
 *				  other than once. All the registers for Window drivers are
 *				  initialised in the VXD, in brdio.c .
 *****************************************************************************/

void CALL_CONV HWSetupBunchOfISPRegs()
{
	sgl_uint32	uFastFogReg = 0x0;
	sgl_uint32  uBilinearCtl;
	sgl_uint32  uBilinearState;

#if !WIN32
	/* Setup registers for non-Windows builds.
	 */
	sgl_uint32	Count;
	sgl_uint32  tMemSize;
	sgl_uint32	uFloatModeReg = 0x0;
	PCXBUFFER	*PCXBuffer;

	/* setup the fog table */

	for(Count=0;Count<128;Count++)
	{
		gHLogicalDev->Registers[PCX_FOG_TABLE+Count] = FogTable[Count];
	}

	for(Count=0;Count<512;Count++)
	{
		gHLogicalDev->Registers[PCX_DIVIDER_TABLE+Count] = ReciprocalTable[Count];
	}	

	INITPCXREG (PCX_INTMASK, 0x00000002);
	
	/* always in bottom 16MB at the moment */
	/* Not on the Mac mate... */
	/*put the sabre overflow area in the bottom of the second bank */

	INITPCXREG (PCX_ISP_BASE, 0x00080000);

	/*put the texas parameter area in the bottom of the first bank */

	INITPCXREG (PCX_PREC_BASE, 		0x00000000);

	INITPCXREG (PCX_TMEM_REFRESH,	0x00000480);

	INITPCXREG (PCX_XCLIP, 			0x00000000);

	/* Initialise PCX2 specific settings.
	 */
	/* Set the delay on MEMCLK used for SDRAM
	 * The default is Delay 1 PVRCLK (ie 0x0, see PCX2 doc).
	 *
	 *	000 Delay 1 from PVRCLK
	 *	001 Delay 1 from CLKCMPI
	 *	010 Delay 2 from CLKCMPI
	 *	011 Delay 2 from PVRCLK
	 *	100 Delay 3 from PVRCLK
	 *	101 Delay 4 from PVRCLK
	 *	110 Delay 5 from PVRCLK
	 *	111 Delay 1 from PVRCLK
	 */
	INITPCXREG (PCX_CLKSELECT,  0x00000000);

	/* Enable/Disable IEEE Floating Point Number support.
	 * Register PCX_IEEEFP bit 0 set.
	 * Should be ON for PCX2.
	 */
	uFloatModeReg |= 0x00000001;

	/* Enable/Disable Edge scaling for perpendicular edges.
	 * Register PCX_IEEEFP bit 1 set.
	 * Should be ON for PCX2.
	 */
	uFloatModeReg |= 0x00000002;

	/* Enable/Disable pointer concatenation.
	 * Register PCX_IEEEFP bit 2 set.
	 * Should be ON by default.
	 */
	if ( HWRdValFile("PointerConcatenationOff",0) == 0)
	    uFloatModeReg |= 0x00000004;

	/* Set Float Mode register. Address 0060H
	 */
	INITPCXREG (PCX_IEEEFP,  uFloatModeReg);

	/* Enable PCI2.1 compliance.
	 * Register PCX_PCI21COMP bit 0 clear.
	 */
	INITPCXREG (PCX_PCI21COMP,  0x00000000);

	/* Enable PCX2 Power Down of internal RAM feature.
	 * Register PCX_POWERDOWN bit 0 set.
	 */
	INITPCXREG (PCX_POWERDOWN,  0x00000001);

#if 0
	ProgramSOFAddressReg (physOutputToPCI);
	ProgramStrideReg (physFrameBufferStride);
#endif

   /* 
     DetermineTextureMemorySize() needs to be after the above register 
     settings otherwise  the system locks up when attempting to do a 
     read from TMEM.
   */
#if DOS32

	if ( HWRdValFile("TEXTURE_MEMORY_SIZE",0) > 0)
	{
		tMemSize = HWRdValFile("TEXTURE_MEMORY_SIZE",4);

		switch ( tMemSize )
		{
			case 4:
			    INITPCXREG (PCX_TMEM_SETUP, 	TMEM_4M_SETTING ); 
			    break;

			case 2:
			    INITPCXREG (PCX_TMEM_SETUP, 	TMEM_2M_SETTING ); 
			    break;

			case 1:
			    INITPCXREG (PCX_TMEM_SETUP, 	TMEM_1M_SETTING ); 
			    break;

			default:
			    INITPCXREG (PCX_TMEM_SETUP, 	TMEM_4M_SETTING ); 
			    tMemSize = ( 4 >> 20 );
			    break;
		}

	}  
	else
#endif
	  tMemSize = ( 4 >> 20 );

	/* End of Non-Window builds register initialisation.
	 */
#endif	/* #if !WIN32	*/

	/* We need to make decisions here to whether the 16 bits per pixel is supported.
	 */
	nForceBPP = HWRdValFileUInt("ForceBitsPerPixel", 0);

	if (nForceBPP)
		PVROSSetPCIPixelFormat ((sgl_uint16) nForceBPP, FALSE);
	else
		PVROSSetPCIPixelFormat (16, FALSE);

	/* Initialise PCX2 specific settings.
	 */

	/* Enable/Disable Bilinear
	 * This should be OFF by default...
	 *
	 *	HW	User
	 *  ===========================================
	 *	00	2	 	bilinear filtering
	 * 	01	1		adaptive bilinear filtering
	 * 	10			reserved (set to 0)
	 * 	11	0		linear mip-mapping.
	 *
	 * The default value is 0xFF to indicate whether the field exists.
	 * If 0xFF is returned then it is assumned that the bilinear filtering
	 * mode is set by the application. In this case bilinear filtering
	 * is set as default.
	 *
	 * Otherwise the filtering setting is set by the ini file setting.
	 */
	/* read FALSE means bilinear controlled form AppHints etc. */
	uBilinearCtl = HWRdValFileUInt("BilinearCtlEnable",0);

	uBilinearState = HWRdValFileUInt("BilinearState",0xFF);

	/* Driver Default state - Bilinear filtering off.
	 */
	uBilinearReg		= 3;
	bFilteringOverRide	= FALSE;
	bBilinearEnabled	= FALSE;

	if(!uBilinearCtl)
	{

		if (uBilinearState == 0xFF)
		{
			/* Driver Default state - Bilinear filtering off.
			 */
			uBilinearReg		= 3;
			bFilteringOverRide	= FALSE;
			bBilinearEnabled	= FALSE;
		}
		else if(uBilinearState == 0)
		{
			uBilinearReg		= 3;
			bFilteringOverRide	= TRUE;
			bBilinearEnabled	= FALSE;
		}
		else
		{
			/* Bilinear has been set in the registry or sglhw.ini.
			 */
			/* bilnear mode if bilinear on - i.e. normal or adaptive */
			uBilinearReg = HWRdValFileUInt("BilinearFilteringMode",2);

			bFilteringOverRide	= TRUE;
			bBilinearEnabled = TRUE;
			
			if(uBilinearReg > 0x3 || !uBilinearReg)
			{
				uBilinearReg = 2;
			}
			uBilinearReg		= eFilterLookUp[uBilinearReg];
		}
	}



	/* Enable/Disable SDRAM caching
	 * This should be ON by default anyway
	 */
	if ( HWRdValFileUInt("SDRAMCachingOff",0) > 0)
		uBilinearReg |= 0x00000004;

	/* Set Bilinear register. Address 0064H
	 */
	INITPCXREG (PCX_BILINEAR,  uBilinearReg);

	/* Enable/Disable fast fog.
	 * Register PCX_FASTFOG bit 8 set.
	 * Should be OFF by default.
	 */
	if ( HWRdValFileUInt("FastFogON",0) > 0)
	{
		/* Enable fast fog and set value to 0xFF.
		 */
		uFastFogReg = 0x1FF;
	}
	else
	{
		/* Disable fast fog.
		 */
		uFastFogReg = 0x00000000;
	}

	/* Set Fast Fog register. Address 0074H
	 */
	INITPCXREG (PCX_FASTFOG,  uFastFogReg);

	/*
	// "Set" the end of render flag, and initialise the frame buffer
	// control
	//
	// We set the end of render, because nothing is happening at the mo'
	*/
	ASSERT (gHLogicalDev);

	/* Select which device we have to decide on the default ARBMODE setting.
	 */	
	switch (gHLogicalDev->DeviceType)
	{
		case MIDAS4:
			/* Need to set ARBMODE register to 0x00000001.
			 */
			INITPCXREG (PCX_ARBMODE, 0x00000001);
		break;

		case MIDAS5:
			/* Need to set ARBMODE register to 0x0000000D.
			 */
			INITPCXREG (PCX_ARBMODE, 0x0000000D);
		break;

		case MIDAS5_003:
			/* Need to set ARBMODE register to 0x0000000D.
			 */
			INITPCXREG (PCX_ARBMODE, 0x0000000D);
		break;

		default:
			/* Incorrect device type specified.
			 */
			ASSERT(FALSE);
		break;
	}

	gHLogicalDev->RenderStatus = PVR_STATUS_EOR;
}

/**********************************************************************/

void CALL_CONV ProgramSOFAddressReg (sgl_uint32 uSOFAddress)
{
	gHLogicalDev->Registers[PCX_SOFADDR] = uSOFAddress;
}

/**********************************************************************/

void CALL_CONV ProgramStrideReg (sgl_uint32 uStride)
{
	gHLogicalDev->Registers[PCX_LSTRIDE] = uStride;
}

#if DEBUG && (WIN32 || DOS32) && 0

void HWDumpRegisters(FILE * outfile)
{

		!!!!!!!!!!!!!!!!!THIS HAS TO BE REWRITTEN!!!!!!!!!!!!!!!!!! 

	int i;
	sgl_uint32 Val;
   	sgl_uint32* pSCBusIOSpace = PVROSGetVirtualBurstSpace();

	fprintf(outfile, "ISP Registers\n===========\n");
	for(i = 0; i <= ISP_SDRAM_SETUP2; i++)
	{
		fprintf(outfile, "0x%lx:\t 0x%lx\n", i*4, *(pSCBusIOSpace+ISP1+i));
	}

	fprintf(outfile, "\n\nTSP Registers\n===========\n");
	for(i = 0; i <= TSP_CAMERA; i++)
	{
		if(i == TSP_CACHETEST)
		{
				fprintf(outfile,"0x%lx:\t CACHE TEST!\n", i);
		}
		else
		{
			fprintf(outfile,"0x%lx:\t 0x%lx\n", i, *(pSCBusIOSpace+i));
		}
	}


	fprintf(outfile, "\n\nBRIDGE Registers\n===========\n");
	for(i = 0; i <= BRIDGE_RESET_CTRL; i+=4)
	{
		Val = InDword(BridgeRegisterIOBase+ i);
		fprintf(outfile,"0x%lx:\t 0x%lx\n", i, Val);
	}

}

#endif

/**********************************************************************/

PVROSERR CALL_CONV PVROSSetPCIPixelFormat(sgl_uint16 wBitsPerPixel, sgl_bool bDither)
{
	sgl_uint32	uDitherBit;

	/* Set the dithering register based on the flag setting.
	 * 0x00000010 used to enable 0x0 to disable.
	 */
	if (bDither)
	{
		/* Enable dithering.
		 */
		uDitherBit = 0x00000010;
	}
	else
	{
		/* Disable dithering. Clear the flag.
		 */
		uDitherBit = 0x0;
	}

	/* If force bits per pixel then set the local copy.
	 */
	if (nForceBPP)
	{
		wBitsPerPixel = nForceBPP;
	}

	/* For PCX2 need to always enable dithering. This is done by setting bit 4
	 * of the PCX_PACKMODE register (address 040H). ie '| 0x00000010'.
	 */
	switch(wBitsPerPixel)
	{
		case 15:
		{
			#if MAC
			IWREG( pSCBusIOSpace, PCX_PACKMODE, (0x00000083 | uDitherBit));
			#else
			gHLogicalDev->Registers[PCX_PACKMODE] = (0x00000003 | uDitherBit);
			#endif
			break;
		}
	
		case 16:
		{
			#if MAC
			IWREG( pSCBusIOSpace, PCX_PACKMODE, (0x00000082 | uDitherBit));
			#else
			gHLogicalDev->Registers[PCX_PACKMODE] = (0x00000002 | uDitherBit);
			#endif
			break;
		}
	
		case 24:
		{
			#if MAC
			/* As yet untested on the mac, I've never seen a
			 * Mac video card that does 24bit as opposed to 32 
			 */
			IWREG( pSCBusIOSpace, PCX_PACKMODE, 0x00000081);
			#else
			gHLogicalDev->Registers[PCX_PACKMODE] = 0x00000001;
			#endif
			break;
		}
	
		case 32:
		{
			#if MAC
			/* Set RB swap and endian swap */
			IWREG( pSCBusIOSpace, PCX_PACKMODE, 0x0000000c);
			#else
			gHLogicalDev->Registers[PCX_PACKMODE] = 0x00000000;
			#endif
			break;
		}

		default:
		{
			return PVROS_DODGY;
		}
	}
	return PVROS_GROOVY;
}

void CALL_CONV PVROSSetupISPRegs()
{
}

sgl_bool PVROSFilteringOveride()
{
	return bFilteringOverRide;
}

sgl_bool PVROSBilinearEnabled()
{
	return bBilinearEnabled;
}

void HWSetBilinearRegister(sgl_texture_filter_type eFilterType)
{
	/* Enable/Disable Bilinear
	 * This should be ON by default anyway
	 *
	 * Range	00	bilinear filtering
	 *			01	adaptive bilinear filtering
	 *			10	reserved (set to 0)
	 *			11	linear mip-mapping.
	 */
	if (!bFilteringOverRide)
	{
		/* Set Bilinear register. Address 0064H
		 * Need to read the uBilinearReg value to determine if SDRAM caching
		 * has been disabled in the ini file.
		 * Annoying but it has to be done since it shares the same
		 * register.
		 */
		uBilinearReg = (uBilinearReg & 0xFFFFFFFC) | eFilterLookUp[eFilterType];
		gHLogicalDev->Registers[PCX_BILINEAR] = uBilinearReg;
	}
}
/*
// END OF FILE
*/

/****************************************************************************
 *;++
 * Name           : $RCSfile: sglwin.c,v $
 * PROGRAM        :	sglwin.c
 * C Author       : Unknown
 * Created        : Unknown
 *
 * Copyright	: 1995-2022 Imagination Technologies (c)
 * License		: MIT
 *
 *                   
 * Program Type   :	WIN32 DLL
 *
 * RCS info:
 *
 * $Date: 1997/10/16 15:45:10 $
 * $Revision: 1.47 $ 
 * $Locker:  $
 * $Log: sglwin.c,v $
 * Revision 1.47  1997/10/16  15:45:10  sxy
 * print out SGL Mode for developers.
 *
 * Revision 1.46  1997/10/16  13:24:15  sxy
 * deliberatly over wrote previous version, this is why there is a missing version number.
 *
 * Revision 1.44  1997/09/29  13:25:58  sxy
 * added some DPFDEV messages.
 *
 * Revision 1.43  1997/07/19  16:54:57  mjg
 * On second thoughts, I removed the call to SglError as it wasn't
 * there before, probably nobody was looking for the error
 * sgl_err_in_address_callback and the pop-up box it created telling
 * you this was bound to get irritating.
 *
 * Revision 1.42  1997/07/19  16:25:16  mjg
 * Made the Address mode callback a static global, in case an app
 * doesn't fill in our callback structure.
 *
 * Revision 1.41  1997/07/18  15:32:39  gdc
 * tidy up
 *
 * Revision 1.40  1997/07/14  19:41:43  mjg
 * Added PCX2_003 compile switch and MODULE_ID.
 *
 * Revision 1.39  1997/07/14  09:59:22  gdc
 * tried to turn off windows include file warnings and failed
 *
 * Revision 1.38  1997/07/03  13:28:16  gdc
 * now gets vxd version into win ver
 *
 * Revision 1.37  1997/06/10  08:41:21  gdc
 * chnaged headers to get prototypes and remove sglwin32.h
 *
 * Revision 1.36  1997/06/05  11:46:51  dm
 * Jim was twiddling about on mi machine and forgot to check this in.[D
 *
 * Revision 1.35  1997/06/04  16:11:06  gdc
 * added better rcs header
 *
 *
 ***************************************************************************/

#define MODULE_ID MODID_SGLWIN

#include <windows.h>
#include <string.h>
#include <stdlib.h>

#include <ddraw.h>
#define API_TYPESONLY
#include "sgl.h"
#undef API_TYPESONLY
#include "pvrosapi.h" 

#include "sgl_defs.h" /* Includes hwregs.h */

/* logical device for the render status word */
extern HLDEVICE gHLogicalDev;

/* publics */
static FRAME_BUFFER_MODE 	FrameBufferMode = FRAME_BUFFER_ACTIVE;
static CALLBACK_ADDRESS_PARAMS gCAP;

/* HWDEVICE.C stuff */
void CALL_CONV HWSetFrameBufferMode( FRAME_BUFFER_MODE FrameBufferMode );

/* Win32 SGL Extension exports */

/******************************************************************************
 * Function Name: AddressModePreRenderCallback 
 * Inputs		: 
 * Outputs		: 
 * Returns		: 
 * Description  : Calls application back to get buffer info for render
 *****************************************************************************/
PVROSERR AddressModePreRenderCallback (void *pData, void *pContext)
{
	PRE_RENDER_CB_STRUCT  *pPRCS = pData;
	PROC_ADDRESS_CALLBACK	fnPAC = (PROC_ADDRESS_CALLBACK) pContext;

	ASSERT (pData);
	ASSERT (pContext);

	if (fnPAC && (fnPAC (&gCAP) == sgl_no_err))
	{
		pPRCS->PhysRenderBufferAddress 		= (sgl_uint32) gCAP.pMem;
		pPRCS->PhysRenderBufferBitsPerPixel = gCAP.bBitsPerPixel;
		pPRCS->PhysRenderBufferStride		= gCAP.wStride;
		/* pPRCS->FlipRequested only used in DirectDraw mode */
		return (PVROS_GROOVY);
	}

	DPFDEV((DBG_MESSAGE,"AddressModePreRenderCallback failed"));
	return (PVROS_DODGY);	
}	

/******************************************************************************
 * Function Name: DDrawMode2dCallback 
 * Inputs		: 
 * Outputs		: 
 * Returns		: 
 * Description  : Calls application back to get buffer info for render
 *****************************************************************************/
PVROSERR DDrawMode2dCallback (void *pData, void *pContext)
{
	PROC_2D_CALLBACK fn2D = (PROC_2D_CALLBACK) pContext;
	HDISPLAY		 hDisplay = pData;

	ASSERT (pData);
	ASSERT (pContext);

	if (fn2D)
	{
		CALLBACK_SURFACE_PARAMS CSP;

		if (GetDDraw2dCallbackInfo (hDisplay, &CSP) == PVROS_GROOVY)
		{
			if (fn2D (&CSP) == sgl_no_err)
			{
				return (PVROS_GROOVY);
			}
		}
	}
	DPFDEV((DBG_MESSAGE,"DDrawMode2dCallback failed"));
	return (PVROS_DODGY);	
}	

/******************************************************************************
 * Function Name: DDrawModeEORCallback 
 * Inputs		: 
 * Outputs		: 
 * Returns		: 
 * Description  : 
 *****************************************************************************/
PVROSERR DDrawModeEORCallback (void *pData, void *pContext)
{
	PROC_END_OF_RENDER_CALLBACK fnEOR = (PROC_END_OF_RENDER_CALLBACK) pContext;

	ASSERT (pContext);

	if (fnEOR && (fnEOR () == sgl_no_err))
	{
		return (PVROS_GROOVY);
	}
	DPFDEV((DBG_MESSAGE,"DDrawModeEORCallback failed"));
	return (PVROS_DODGY);	
}	

/******************************************************************************
 * Function Name: sgl_use_ddraw_mode
 * Inputs		: 
 * Outputs		: 
 * Returns		: 
 * Description  : 
 *****************************************************************************/

int CALL_CONV sgl_use_ddraw_mode (HWND hWnd, PROC_2D_CALLBACK Proc2dCallback)
{
	
	PVROSSetDDrawWindowHandle((void *)hWnd);
	
	FrameBufferMode = FRAME_BUFFER_ACTIVE;
		
	HWSetFrameBufferMode (FRAME_BUFFER_ACTIVE);

	if (Proc2dCallback)
	{
		PVROSCallbackRegister (gHLogicalDev, CB_2D, DDrawMode2dCallback, Proc2dCallback);
	}

	DPFDEV((DBG_MESSAGE,"SGL in DDRAW mode"));
	
	return sgl_no_err;
	
}/*sgl_use_ddraw_mode*/

/******************************************************************************
 * Function Name: sgl_use_address_mode
 * Inputs		: 
 * Outputs		: 
 * Returns		: 
 * Description  : 
 *****************************************************************************/

int CALL_CONV sgl_use_address_mode (PROC_ADDRESS_CALLBACK ProcNextAddress, sgl_uint32 **pStatus)
{

	*pStatus = (sgl_uint32 *) &gHLogicalDev->RenderStatus;
	
	FrameBufferMode = FRAME_BUFFER_PASSIVE;

	HWSetFrameBufferMode (FRAME_BUFFER_PASSIVE);

	PVROSCallbackRegister (gHLogicalDev, CB_PRE_RENDER, AddressModePreRenderCallback, ProcNextAddress);
	
	DPFDEV((DBG_MESSAGE,"SGL in ADDRESS mode"));
	
	return sgl_no_err;
	
}/*sgl_use_address_mode*/

/******************************************************************************
 * Function Name: sgl_use_eor_callback
 * Inputs		: 
 * Outputs		: 
 * Returns		: 
 * Description  : App gives SGL where it's eor callback
 *****************************************************************************/

int CALL_CONV sgl_use_eor_callback(PROC_END_OF_RENDER_CALLBACK ProcEOR)
{

	PVROSCallbackRegister (gHLogicalDev, CB_END_OF_RENDER, DDrawModeEORCallback, ProcEOR);

	return 0;
}


void GetFileVersion(char *File, char *FileVer, int size)
{
	char Buffer[1024];
	sgl_uint32 WhatsThisFor, RetSize, VerSize;
	void *Version;
	char FullFile[512];
	
	GetEnvironmentVariable ("WINDIR", FullFile, sizeof (FullFile)-1);
	strcat(FullFile, File);

	VerSize = (sgl_uint32)GetFileVersionInfoSize(FullFile,&WhatsThisFor);
	if(VerSize)
	{
		GetFileVersionInfo(FullFile,WhatsThisFor,1024,Buffer);
		VerQueryValue(Buffer,
					  TEXT("\\StringFileInfo\\040904E4\\FileVersion"),
					  &Version, &RetSize);
		strncpy(FileVer, (char *)Version, size-1 );
	}

}

/******************************************************************************
 * Function Name: sgl_get_win_versions
 * Inputs		: -
 * Outputs		: -
 * Returns		: Pointer to internal versions structure
 * Description  : Gets win32 specific version info for software and hardware
 *****************************************************************************/
sgl_win_versions *  CALL_CONV sgl_get_win_versions()
{
#if 0
	sgl_uint16 	wISPRev;
	sgl_uint16 	wTSPRev;
#endif
	sgl_uint8	bTSPMem;

	static 	sgl_win_versions  versions;
	static char sgl_vxd_rev[16] = {0};
	static char pci_bridge_vendor_id[16] = {0};
	static char pci_bridge_device_id[16] = {0};
	static char pci_bridge_rev[16] = {0};
	static char pci_bridge_irq[16] = {0};
	static char pci_bridge_io_base[16] = {0};
	static char tsp_rev[16] = {0};
	static char tsp_mem_size[16] = {0};
	static char isp_rev[16] = {0};
	static char mode[16] = {0};
	static char status[16] = {0};

#if !WIN32
    if (SglInitialise())
	{
		SglError(sgl_err_failed_init);
		return NULL;
	}
#endif
	/* required_sglwin32_header */
	versions.required_sglwin32_header = SGL_WIN_HEADER_VERSION;

	/* pci_bridge_vendor_id */
	wsprintf (pci_bridge_vendor_id, "0x1033");
	versions.pci_bridge_vendor_id = pci_bridge_vendor_id;

#if PCX2 || PCX2_003
	/* pci_bridge_device_id */
	wsprintf (pci_bridge_device_id, "0x0046");
	versions.pci_bridge_device_id = pci_bridge_device_id;
#elif PCX1
	/* pci_bridge_device_id */
	wsprintf (pci_bridge_device_id, "0x002A");
	versions.pci_bridge_device_id = pci_bridge_device_id;
#elif ISPTSP
	/* pci_bridge_device_id */
	wsprintf (pci_bridge_device_id, "0x001F");
	versions.pci_bridge_device_id = pci_bridge_device_id;
#endif
	
	/* sgl_vxd_rev */
	GetFileVersion( "\\system\\vsgl.vxd",sgl_vxd_rev, 16);
	versions.sgl_vxd_rev = sgl_vxd_rev;

#if 0
	/* pci_bridge_rev */
	itoa ((int) 0, pci_bridge_rev, 10);
	versions.pci_bridge_rev = pci_bridge_rev;
	
	/* pci_bridge_irq */
	itoa ((int)bIRQ, pci_bridge_irq, 10);
	versions.pci_bridge_irq = pci_bridge_irq;
	
	/* pci_bridge_io_base */
	wsprintf(pci_bridge_io_base, "%08Xh", dwIOBase);
	versions.pci_bridge_io_base = pci_bridge_io_base;
	
	/* tsp_rev */
	wTSPRev = (sgl_uint16)(sgl_uint32)*(pSCBusIOSpace+PCX_ID);
	itoa ((int)wTSPRev, tsp_rev, 10);
	versions.tsp_rev = tsp_rev;
#endif
	
	/* tsp_mem_size */
	bTSPMem = (sgl_uint8)(PVROSDetermineTexMemConfig(0) >> 20);
	itoa ((int)bTSPMem, tsp_mem_size, 10);
	versions.tsp_mem_size = tsp_mem_size;

#if 0
	/* isp_rev */
	wISPRev = (sgl_uint16)(sgl_uint32)*(pSCBusIOSpace+PCX_ID);
	itoa ((int)wISPRev, isp_rev, 10);
	versions.isp_rev = isp_rev;
#endif
	
	/* mode */
	if (FrameBufferMode == FRAME_BUFFER_ACTIVE)
	{
		versions.mode = "active";
	}
	else
	{
		versions.mode = "passive";
	}
	
	/* status */
	versions.status = "";
	
	/* build_info */
	#if DEBUG
		versions.build_info = "Debug build";
	#else
		versions.build_info = "";
	#endif
	
	return &versions;
	
}/*sgl_win_versions*/

/* eof */



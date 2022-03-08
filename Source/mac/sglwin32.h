/*****************************************************************************
Name			:	sglwin32.h
Title			:	Win32 SGL Extensions
Author			:	Videologic PLC
Created			:	9/02/96

Copyright		:	1996 by VideoLogic PLC. All rights reserved.
					No part of this software, either material or conceptual
					may be copied or distributed, transmitted, transcribed,
					stored in a retrieval system or translated into any
					human or computer language in any form by any means,
					electronic, mechanical, manual or other-wise, or
					disclosed to third parties without the express written
					permission of VideoLogic PLC, Unit 8, HomePark
					Industrial Estate, King's Langley, Hertfordshire,
					WD4 8LZ, U.K.

Description		:	Contains DLL extensions for Win32 SGL support

Program Type	:	Win32 DLL.
					requires windows.h and ddraw.h
					
Revision Control System :

	$Revision: 1.1 $ $Date: 1997/03/14 11:03:53 $ $Release$ $Locker:  $

*****************************************************************************/

#ifndef _SGLWIN32_H_
#define _SGLWIN32_H_


/* Detect the Microsoft compiler, set _MICROSOFT_C_ flag */
#if defined(_MSC_VER) && defined(_M_IX86) && defined(__TIMESTAMP__) && defined(_WIN32)
#define _MICROSOFT_C_
#endif

/*
// Byte structure packing for this file
*/

#pragma pack(1)

/*
	Note :
	Direct draw COM model only seems to be
	compatible with the Microsoft compiler.
	Other compatible compilers can be added to the list.
*/
#if defined(_MICROSOFT_C_) || defined (__WATCOMC__)
#include "ddraw.h"
#else
#define LPDIRECTDRAW		LPVOID
#define LPDIRECTDRAWSURFACE	LPVOID
#endif

/* Define the calling convention */
#define CALL_CONV __cdecl

/****************************/
/* SGL/Win32 version info   */
/****************************/

#define SGL_WIN_HEADER_VERSION "1.7"

/***********************/
/* PowerVR Device ID's */
/***********************/

#define NEC_VENDOR_ID					0x1033  /* PCI Vendor ID */
#define MIDAS3_PCI_BRIDGE_DEVICE_ID		0x001F  /* PCI Bridge ID */
#define PCX1_DEVICE_ID					0x002A  /* PCX1 device ID */
#define PCX2_DEVICE_ID					0x0046  /* PCX2 device ID */

/*********************/
/* SGL/Win32 status  */
/*********************/

#define SGL_STATUS_RENDER_BUSY		1


/**************************************/
/* SGL/Win32 extensions : error codes */
/**************************************/

enum
{	
	sgl_err_ddraw_not_available  = -1024,
	sgl_err_mode_already_defined,
	sgl_err_insufficient_surface_memory,
	sgl_err_graphics_not_capable,
	sgl_err_display_mode_not_supported,
	sgl_err_illegal_address,
	sgl_err_bad_hwnd,
	sgl_err_bad_ddraw_vblank,
	sgl_err_bad_ddraw_flip_status,
	sgl_err_bad_ddraw_flip,
	sgl_err_bad_ddraw_vblank_status,
	sgl_err_bad_ddraw_vblank_wait,
	sgl_err_bad_ddraw_restore,
	sgl_err_bad_ddraw_lock,
	sgl_err_bad_ddraw_unlock,
	sgl_err_bad_ddraw_bltfill,
	sgl_err_bad_ddraw_close,
	sgl_err_bad_ddraw_getcaps,
	sgl_err_bad_ddraw_cooperate,
	sgl_err_bad_ddraw_modechange,
	sgl_err_in_address_callback,
	sgl_err_graphics_locked_out,
	sgl_err_pixel_format_not_supported
};


/************************************/
/* SGL/Win32 extensions : type defs */
/************************************/

typedef struct
{
	char *required_sglwin32_header;
	char *sgl_vxd_rev;
	char *pci_bridge_vendor_id;
	char *pci_bridge_device_id;
	char *pci_bridge_rev;
	char *pci_bridge_irq;
	char *pci_bridge_io_base;
	char *tsp_rev;
	char *tsp_mem_size;
	char *isp_rev;
	char *mode;
	char *status;
	char *build_info;

} sgl_win_versions;

typedef struct
{
	LPDIRECTDRAW			pDDObject;
	LPDIRECTDRAWSURFACE		p3DSurfaceObject;
	LPVOID					p3DSurfaceMemory;
	WORD					wBitsPerPixel;
	DWORD					dwStride;
	WORD					wWidth;
	WORD					wHeight;

}CALLBACK_SURFACE_PARAMS, *P_CALLBACK_SURFACE_PARAMS;

typedef struct
{
	LPVOID					pMem;
	WORD 					wStride;
	BYTE 					bBitsPerPixel;

}CALLBACK_ADDRESS_PARAMS, *P_CALLBACK_ADDRESS_PARAMS;

typedef int (CALL_CONV *PROC_2D_CALLBACK) (P_CALLBACK_SURFACE_PARAMS);
typedef int (CALL_CONV *PROC_ADDRESS_CALLBACK) (P_CALLBACK_ADDRESS_PARAMS);
typedef int (CALL_CONV *PROC_END_OF_RENDER_CALLBACK) ();


/************************************/
/* SGL/Win32 extension functions    */
/************************************/

#ifdef __cplusplus
extern "C" {
#endif 

sgl_win_versions* CALL_CONV sgl_get_win_versions();

int CALL_CONV sgl_use_ddraw_mode (HWND hWnd, PROC_2D_CALLBACK Proc2d);
int CALL_CONV sgl_use_address_mode (PROC_ADDRESS_CALLBACK ProcNextAddress, LPDWORD *pStatus);
int CALL_CONV sgl_use_eor_callback(PROC_END_OF_RENDER_CALLBACK ProcEOR);

#ifdef __cplusplus
}
#endif 

/*
// Revert to previous structure packing mode
*/
#pragma pack()


#endif /*_SGLWIN32_H_*/

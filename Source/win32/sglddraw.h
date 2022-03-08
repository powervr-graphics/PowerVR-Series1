/*****************************************************************************
 * Name			:	$RCSfile: sglddraw.h,v $
 * Title		:	SGLDDRAW.H
 * C Author		:	Nick Jones
 * Created		:	9/1/96
 *
 * Copyright	: 1995-2022 Imagination Technologies (c)
 * License		: MIT
 *
 * Description	:	Direct Draw Surface Manager for SGL, creates and manages
 *					the direct draw surfaces.
 *
 * Program Type	:	WIN32 DLL
 *
 * RCS info:
 *
 * $Revision: 1.2 $ $Locker:  $
 ****************************************************************************/

#ifndef _SGLDDRAW_H
#define _SGLDDRAW_H

#include "sglwin32.h"

/* SGL Modes of operation */
#define SGL_MODE_NONE			0
#define SGL_MODE_STANDALONE		1
#define SGL_MODE_OVERLAY24		2
#define SGL_MODE_DCI			3
#define SGL_MODE_DDRAW			4
#define SGL_MODE_MEM_ADDRESS	5

/* Typedefs */
typedef LPBYTE			LPMEM;
typedef LPMEM FAR *		LPMEMADDRESS;


/* Publics */
extern HINSTANCE		ghInstance;
extern int				sgl_x_dimension;
extern int				sgl_y_dimension;
extern BOOL				bRet;
extern BOOL				bQuit;
extern BOOL				bUseSGLWindow;
extern WORD				wSGLMode;
extern HWND				hAppWnd;
extern PROC_2D_CALLBACK	Proc2d;
extern BOOL				bSglDdrawMode;
extern BOOL				bDDEnded;
extern BOOL				bTripleBuffered;
extern LPDIRECTDRAWSURFACE pFrontBuffer;
extern LPDIRECTDRAWSURFACE pBackBuffer;
extern LPMEM			pBackBufferMemory;
extern DWORD			dwBufferStride;
extern int				error;
extern CALLBACK_SURFACE_PARAMS	CallbackSurfaceInfo;
extern BOOL				bFirstTime;
extern BOOL				bSingleShotMode;
extern LPDIRECTDRAW		pDDObject;
extern WORD				wBitsPerPixel;
extern WORD				wWidth;
extern WORD				wHeight;
extern DWORD			dwBackBufferPhysicalAddr;


/* SGL Direct Draw Services */

int DDrawCreateDevice (int device_number, int x_dimension, int y_dimension,
						sgl_device_colour_types device_mode, int double_buffer);

int DDrawDeleteDevice (int deviceID);
int DDrawSetViewport (int parent_device, int left, int top, int right, int bottom);
int DDPrepareForRender(sgl_bool swap_buffers);
int DDCleanUpAfterRender();
void DDEndOfRender();

/* Low Level Direct Draw Services */
BOOL DDHasFlipped(LPDIRECTDRAWSURFACE lpDDSurface);
int DDLock (LPDIRECTDRAWSURFACE pSurface, LPMEMADDRESS pMemAddress, LPDWORD pStride);
int DDUnlock(LPDIRECTDRAWSURFACE pSurface);
void DDEnd();
int DDGetPhysicalAddress (LPMEM pVirtual, LPDWORD lpdwPhysical);
int DisplayStatusMessage(char * szMsg, int nMsgNo);
void int3();

#endif



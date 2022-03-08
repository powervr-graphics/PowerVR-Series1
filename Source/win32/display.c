/*****************************************************************************
 *;++
 * Name           : $RCSfile: display.c,v $
 * Title          : DISPLAY.C
 * C Author       : Jim Page
 * Created        : 21/04/95
 *
 * Copyright	: 1995-2022 Imagination Technologies (c)
 * License		: MIT
 *
 * Description    :	WIN32 display API
 *                   
 * Program Type   :	WIN32 DLL
 *
 * RCS info:
 *
 * $Date: 1997/09/24 09:45:01 $
 * $Revision: 1.26 $
 * $Locker:  $
 * $Log: display.c,v $
 * Revision 1.26  1997/09/24  09:45:01  gdc
 * changed HWRdavalFile's to a new function which has a revised priority
 * of where to get values
 *
 * Revision 1.25  1997/06/17  19:37:30  jop
 * Adjusted strict locks Sleep to 3 ms
 *
 * Revision 1.24  1997/06/04  13:55:28  gdc
 * stopped compiler warnings
 *
 * Revision 1.23  1997/05/30  16:37:33  mjg
 * Added the logical device as an argument to PVROS{Open | Close}
 * DisplayDevice as the Callback registration functioms required it.
 *				 
 * Revision 1.22  1997/05/23  14:37:02  mjg
 * Added ResetCallbacks so that subsequent applications - with the same
 * ProcessId - don't attempt to use an invalid callback.
 *
 * Revision 1.21  1997/05/22  20:23:44  mjg
 * Added the call to ResetCallbacks (extern from callback.c).
 *
 * Revision 1.20  1997/05/22  11:53:18  erf
 * Fixed 15bit 16 bit issue with Direct Draw mode. ie if 16 specified in
 * DDraw mode with a 15 bit card then 15 bit is selected.
 *
 * Revision 1.19  1997/05/19  07:36:23  jop
 * Feeble attempt to get strict locks working better ...
 *
 * Revision 1.18  1997/05/15  20:15:17  jop
 * Put strict locks in
 *
 * Revision 1.17  1997/05/02  07:25:35  erf
 * Removed UINT32's etc and replaced with sgl_uint32 etc.
 *
 * Revision 1.16  1997/05/01  02:43:14  jop
 *  Undid Ed's fixes
 *
 * Revision 1.15  1997/04/29  15:12:40  erf
 * Removed #include "pvrosP.h".
 *
 * Revision 1.14  1997/04/26  14:15:27  erf
 * Added USE_HEAPMAN to get globally accessible heap
 *
 * Revision 1.13  1997/04/25  13:53:49  gdc
 * added private pvros header
 *
 * Revision 1.12  1997/04/21  22:30:09  gdc
 * changes for separate pvros.dll
 *
 * Revision 1.11  1997/04/08  08:55:22  sxy
 * Fixed a potential bug in PVROSSelectDisplayMode.
 *
 * Revision 1.10  1997/04/04  08:50:48  gdc
 * set fEnabled variable properly for windows which install their
 * own direct draw window
 *
 * Revision 1.9  1997/04/03  16:08:44  gdc
 * added ability to iconise and restore default device and to single step
 * and pause anomation using return and space keys respectively
 *
 * Revision 1.8  1997/04/03  12:18:14  mjg
 * Updated calling conventions.
 *
 * Revision 1.7  1997/04/03  11:44:46  gdc
 * trying to make default window message handling a bit better
 *
 * Revision 1.6  1997/04/01  18:45:39  mjg
 * Changed notprintf to PVROSPrintf.
 *
 * Revision 1.5  1997/03/29  22:07:01  mjg
 * Inserted PVROSDelay.
 *
 * Revision 1.4  1997/03/27  12:52:01  erf
 * Operating system independent memory allocation/deallocation macros.
 * 1. Changed malloc, calloc, realloc and free to PVROSMalloc, PVROSCalloc,
 *    PVROSRealloc and PVROSFree.
 *
 * Revision 1.3  1997/03/25  15:22:09  erf
 * 1. Removed nasty ^M.
 * 2. Added code for 24 bit mode.
 *
 * Revision 1.2  1997/03/25  10:17:20  jop
 * more PVROS changes and fixed some bugs
 *
 * Revision 1.1  1997/03/12  18:29:50  jop
 * Initial revision
 *
 *;--
 *****************************************************************************/

#define INITGUID			/* make sure global guids are defined (see ddraw.h) */

#include "windows.h"
#include "ddraw.h"

#include "sgl.h"
#include "sglwin32.h"
#include "sgl_defs.h"
#include "debug.h"
#include "pvrosapi.h"

#if DEBUG
static unsigned char Black[3] = { 0xFF, 0, 0xFF };
#else
static unsigned char Black[3] = { 0, 0, 0 };
#endif

/* Enumerated display mode encodes IDs */

#define MAKE_HMODE(x,y,b,r) ((HMODE) ( (((x)&0xFF8)<<20) | (((y)&0xFF8)<<11) | (((b)&0x3F)<<8) | ((r)&0xFF) ))

#define HMODE_XRES(d)			(((sgl_uint32) (d)>>20)&0x0FF8)
#define HMODE_YRES(d)			(((sgl_uint32) (d)>>11)&0x0FF8)
#define HMODE_BITSPERPIXEL(d)	(((sgl_uint32) (d)>>8)&0x3F)
#define HMODE_REFRESHRATE(d)	((sgl_uint32) (d)&0xFF)

#define INIT_DDSTRUCT(x)		memset ((x), 0, sizeof (*(x))); (x)->dwSize = sizeof (*(x));

/* typedefs */

typedef struct DEVICECBSTRUCT
{
	DisplayDeviceTable	*pDDT;
	int					nDevices;		/* used by Enum callback */

} DEVICECBSTRUCT, *PDEVICECBSTRUCT;

typedef struct MODECBSTRUCT
{
	DisplayModeTable	*pDMT;
	int					nDisplayModes;	/* used by Enum modes */

} MODECBSTRUCT, *PMODECBSTRUCT;

typedef struct PRIVATE_DISPLAY_BLOCK
{
	DWORD					dwSize;
	FRAME_BUFFER_MODE		FrameBufferMode;
	BOOL					fRenderToDesktop;
	BOOL					fRestoreModeNeeded;
	BOOL					fEnabled;
	BOOL					fUsingClientDDrawWindow;
	BOOL                    fCorrectMode;
	BOOL                    fSingleStep;
	BOOL                    fStrictLocks;
	LPDIRECTDRAW			pDDraw;
	LPDIRECTDRAW2			pDDraw2;
	LPDIRECTDRAWSURFACE		pFrontBuffer;
	LPDIRECTDRAWSURFACE		pBackBuffer;
	DisplayModeDataBlock	DMDB;
	HINSTANCE				hInstance;
	HWND					hWnd;
	HWND					hMainWnd;
	
} PRIVATE_DISPLAY_BLOCK;

/* private prototype - function is in system.c */
HINSTANCE CALL_CONV PVROSGetInstance();

PVROSERR CALL_CONV PVROSEndOfRenderCallbackAuto (void *pData, HDISPLAY hDisplay);


/* debugging code */

#if !DEBUG

#define ISDDOK(x)	((x)==DD_OK)
#define DDOK(x)		(x)

#define DumpDisplayModeTable /##/
#define DumpDisplayModeDataBlock /##/

#else

#define ISDDOK(x)		(OutputDDError((x),#x)==DD_OK)
#define DDOK(x)			OutputDDError((x),#x)
#define ERRCASE(x)		case x: pszErr = #x; break;
#define WARNINGCASE(x)	case x: DPF ((DBG_WARNING, "%s: %s", sz, #x)); return (hRes);
#define MESSAGECASE(x)	case x: DPF ((DBG_MESSAGE, "%s: %s", sz, #x)); return (hRes);

/******************************************************************************
 * Function Name: OutputDDError
 *
 * Inputs       : 
 * Outputs      : 
 * Returns      : 
 * Globals Used : -
 *
 * Description  : Macro that translates DD errors to text
 *****************************************************************************/
static HRESULT OutputDDError (HRESULT hRes, char *sz)
{
	if (hRes != DD_OK)
	{
		char *pszErr, *p;

		if (p = strchr (sz, '('), p)
		{
			*p = 0;
		}

		switch (hRes)
		{
			ERRCASE(DDERR_ALREADYINITIALIZED)
			ERRCASE(DDERR_BLTFASTCANTCLIP)
			ERRCASE(DDERR_CANNOTATTACHSURFACE)
			ERRCASE(DDERR_CANNOTDETACHSURFACE)
			ERRCASE(DDERR_CANTCREATEDC)
			ERRCASE(DDERR_CANTDUPLICATE)
			ERRCASE(DDERR_CANTLOCKSURFACE)
			ERRCASE(DDERR_CANTPAGELOCK)
			ERRCASE(DDERR_CANTPAGEUNLOCK)
			ERRCASE(DDERR_CLIPPERISUSINGHWND)
			ERRCASE(DDERR_COLORKEYNOTSET)
			ERRCASE(DDERR_CURRENTLYNOTAVAIL)
			ERRCASE(DDERR_DCALREADYCREATED)
			ERRCASE(DDERR_DIRECTDRAWALREADYCREATED)
			ERRCASE(DDERR_EXCEPTION)
			ERRCASE(DDERR_EXCLUSIVEMODEALREADYSET)
			ERRCASE(DDERR_GENERIC)
			ERRCASE(DDERR_HEIGHTALIGN)
			ERRCASE(DDERR_HWNDALREADYSET)
			ERRCASE(DDERR_HWNDSUBCLASSED)
			ERRCASE(DDERR_IMPLICITLYCREATED)
			ERRCASE(DDERR_INCOMPATIBLEPRIMARY)
			ERRCASE(DDERR_INVALIDCAPS)
			ERRCASE(DDERR_INVALIDCLIPLIST)
			ERRCASE(DDERR_INVALIDDIRECTDRAWGUID)
			ERRCASE(DDERR_INVALIDMODE)
			ERRCASE(DDERR_INVALIDOBJECT)
			ERRCASE(DDERR_INVALIDPARAMS)
			ERRCASE(DDERR_INVALIDPIXELFORMAT)
			ERRCASE(DDERR_INVALIDPOSITION)
			ERRCASE(DDERR_INVALIDRECT)
			ERRCASE(DDERR_INVALIDSURFACETYPE)
			ERRCASE(DDERR_LOCKEDSURFACES)
			ERRCASE(DDERR_NO3D)
			ERRCASE(DDERR_NOALPHAHW)
			ERRCASE(DDERR_NOBLTHW)
			ERRCASE(DDERR_NOCLIPLIST)
			ERRCASE(DDERR_NOCLIPPERATTACHED)
			ERRCASE(DDERR_NOCOLORCONVHW)
			ERRCASE(DDERR_NOCOLORKEY)
			ERRCASE(DDERR_NOCOLORKEYHW)
			ERRCASE(DDERR_NOCOOPERATIVELEVELSET)
			ERRCASE(DDERR_NODC)
			ERRCASE(DDERR_NODDROPSHW)
			ERRCASE(DDERR_NODIRECTDRAWHW)
			ERRCASE(DDERR_NODIRECTDRAWSUPPORT)
			ERRCASE(DDERR_NOEMULATION)
			ERRCASE(DDERR_NOEXCLUSIVEMODE)
			ERRCASE(DDERR_NOFLIPHW)
			ERRCASE(DDERR_NOGDI)
			ERRCASE(DDERR_NOHWND)
			ERRCASE(DDERR_NOMIPMAPHW)
			ERRCASE(DDERR_NOMIRRORHW)
			ERRCASE(DDERR_NOOVERLAYDEST)
			ERRCASE(DDERR_NOOVERLAYHW)
			ERRCASE(DDERR_NOPALETTEATTACHED)
			ERRCASE(DDERR_NOPALETTEHW)
			ERRCASE(DDERR_NORASTEROPHW)
			ERRCASE(DDERR_NOROTATIONHW)
			ERRCASE(DDERR_NOSTRETCHHW)
			ERRCASE(DDERR_NOT4BITCOLOR)
			ERRCASE(DDERR_NOT4BITCOLORINDEX)
			ERRCASE(DDERR_NOT8BITCOLOR)
			ERRCASE(DDERR_NOTAOVERLAYSURFACE)
			ERRCASE(DDERR_NOTEXTUREHW)
			ERRCASE(DDERR_NOTFLIPPABLE)
			ERRCASE(DDERR_NOTFOUND)
			ERRCASE(DDERR_NOTINITIALIZED)
			ERRCASE(DDERR_NOTLOCKED)
			ERRCASE(DDERR_NOTPAGELOCKED)
			ERRCASE(DDERR_NOTPALETTIZED)
			ERRCASE(DDERR_NOVSYNCHW)
			ERRCASE(DDERR_NOZBUFFERHW)
			ERRCASE(DDERR_NOZOVERLAYHW)
			ERRCASE(DDERR_OUTOFCAPS)
			ERRCASE(DDERR_OUTOFMEMORY)
			ERRCASE(DDERR_OUTOFVIDEOMEMORY)
			ERRCASE(DDERR_OVERLAYCANTCLIP)
			ERRCASE(DDERR_OVERLAYCOLORKEYONLYONEACTIVE)
			ERRCASE(DDERR_OVERLAYNOTVISIBLE)
			ERRCASE(DDERR_PALETTEBUSY)
			ERRCASE(DDERR_PRIMARYSURFACEALREADYEXISTS)
			ERRCASE(DDERR_REGIONTOOSMALL)
			ERRCASE(DDERR_SURFACEALREADYATTACHED)
			ERRCASE(DDERR_SURFACEALREADYDEPENDENT)
			ERRCASE(DDERR_SURFACEBUSY)
			ERRCASE(DDERR_SURFACEISOBSCURED)
			ERRCASE(DDERR_SURFACELOST)
			ERRCASE(DDERR_SURFACENOTATTACHED)
			ERRCASE(DDERR_TOOBIGHEIGHT)
			ERRCASE(DDERR_TOOBIGSIZE)
			ERRCASE(DDERR_TOOBIGWIDTH)
			ERRCASE(DDERR_UNSUPPORTED)
			ERRCASE(DDERR_UNSUPPORTEDFORMAT)
			ERRCASE(DDERR_UNSUPPORTEDMASK)
			ERRCASE(DDERR_UNSUPPORTEDMODE)
			ERRCASE(DDERR_VERTICALBLANKINPROGRESS)
			MESSAGECASE(DDERR_WASSTILLDRAWING)
			ERRCASE(DDERR_WRONGMODE)
			ERRCASE(DDERR_XALIGN)
		default: pszErr = "Unknown DDraw error";
			break;
		}

		DPF ((DBG_ERROR, "%s: %s", sz, pszErr));
	}

	return (hRes);
}

/******************************************************************************
 * Function Name: DumpDisplayModeTable
 *
 * Inputs       : 
 * Outputs      : 
 * Returns      : 
 * Globals Used : -
 *
 * Description  : Dumps contents of DisplayModeTable structure
 *****************************************************************************/
static void DumpDisplayModeTable (DisplayModeTable *pDMT)
{
	PVROSPrintf("hMode:\t\t 0x%08lx\n", pDMT->hMode);
	PVROSPrintf("XResolution:\t %d\n", pDMT->XResolution);
	PVROSPrintf("YResolution:\t %d\n", pDMT->YResolution);
	PVROSPrintf("BitDepth:\t %d\n", pDMT->BitDepth);
}

/******************************************************************************
 * Function Name: DumpDisplayModeDataBlock
 *
 * Inputs       : 
 * Outputs      : 
 * Returns      : 
 * Globals Used : -
 *
 * Description  : Dumps the contexnt of a DisplayModeDataBlock
 *****************************************************************************/
static void DumpDisplayModeDataBlock (DisplayModeDataBlock *pDMDB)
{
	PVROSPrintf("RefreshRate:\t 0x%08lx\n", pDMDB->RefreshRate);
	PVROSPrintf("RedMask:\t 0x%08lx\n", pDMDB->RedMask);
	PVROSPrintf("GreenMask:\t 0x%08lx\n", pDMDB->GreenMask);
	PVROSPrintf("BlueMask:\t 0x%08lx\n", pDMDB->BlueMask);
	PVROSPrintf("NumberOfBuffers: %d\n", pDMDB->NumberOfBuffers);
	PVROSPrintf("CurrentBuffer:\t %d\n", pDMDB->CurrentBuffer);
}

#endif


/**********************************************************************/
/**/
/**/
/**/
/**/
/**********************************************************************/
static void FillSurface (LPDIRECTDRAWSURFACE pSurf, unsigned char FillCol[3], int x, int y, int bpp)
{
	if (pSurf)
	{
		DDBLTFX DDBF;
		RECT	rc;
		union
		{
			BYTE	b[4];
			WORD	w[2];
			DWORD	dw;
		} Col;

		INIT_DDSTRUCT (&DDBF);

		rc.left = 0;
		rc.top = 0;
		rc.right = x;
		rc.bottom = y;

		switch (bpp)
		{
			case 15:
			{
				Col.w[0] = (FillCol[0]>>3) | ((FillCol[1]>>3)<<5) | ((FillCol[2]>>3)<<10);
				break;
			}

			case 16:
			{
				Col.w[0] = (FillCol[0]>>3) | ((FillCol[1]>>2)<<5) | ((FillCol[2]>>3)<<11);
				break;
			}

			case 24:
			{
				Col.b[0] = FillCol[0];
				Col.b[1] = FillCol[1];
				Col.b[2] = FillCol[2];
				Col.b[3] = 255;
				break;
			}
			
			case 32:
			{
				Col.b[0] = FillCol[0];
				Col.b[1] = FillCol[1];
				Col.b[2] = FillCol[2];
				Col.b[3] = 255;
				break;
			}
			
			default:
			{
				DPF ((DBG_ERROR, "FillSurface: weird colour format: %d", bpp));
				return;
			}
		}

		DDBF.dwFillColor = Col.dw;

		while (DDOK (IDirectDrawSurface2_Blt (pSurf,
											   &rc, 
											   NULL,
											   NULL, 
											   DDBLT_COLORFILL, 
											   &DDBF)) == DDERR_WASSTILLDRAWING);
	}
}

/* Background window routines */

/******************************************************************************
 * Function Name: DDrawWndProc
 *
 * Inputs       : 
 * Outputs      : 
 * Returns      : 
 * Globals Used : -
 *
 * Description  : Message handler for the DDraw window
 *****************************************************************************/
long FAR PASCAL DDrawWndProc (HWND hWnd, sgl_uint message, sgl_uint wParam, LONG lParam)
{
	PRIVATE_DISPLAY_BLOCK	*pPDB;
	
	pPDB = (PRIVATE_DISPLAY_BLOCK *) GetWindowLong (hWnd, GWL_USERDATA);

	switch (message)
	{
		case WM_ACTIVATEAPP:
		{
			if(pPDB)
			{
				if(wParam)
				{
					/*Sleep(500);*/
					pPDB->fEnabled = TRUE;
				}
				else
				{
					Sleep(500);
					pPDB->fEnabled = FALSE;
				}
			}
			break;
		}

		case WM_ACTIVATE:
		{
			if(pPDB)
			{
				if(LOWORD(wParam)==WA_INACTIVE)
				{
					Sleep(500);
					pPDB->fEnabled = FALSE;
				}
				else if(HIWORD(wParam)) /* true indicates window is minimumised */
				{
					Sleep(500);
					pPDB->fEnabled = TRUE;
				}
			}
			break;
		}

		case WM_DISPLAYCHANGE:
		{
			if(pPDB)
			{
				DisplayModeTable *pDMT;
			
				pDMT = &(pPDB->DMDB.DMT);
				
				if(pDMT->XResolution == LOWORD(lParam) &&
				   pDMT->YResolution == HIWORD(lParam) &&
				   pDMT->BitDepth == wParam)
				{
					pPDB->fCorrectMode = TRUE;
				}
				else
				{
					pPDB->fCorrectMode = FALSE;
				}
			}
			break;
		}
#if 0
	    case WM_WINDOWPOSCHANGING:
		{
			LPWINDOWPOS lpwp = (LPWINDOWPOS) lParam;
			
			if (lpwp->flags & SWP_SHOWWINDOW)
			{
				PVROSPrintf("Opening ...\n");
				if (pPDB)
				{
					pPDB->fEnabled = TRUE;
				}
			}
			else if (lpwp->flags & SWP_HIDEWINDOW)
			{
				PVROSPrintf("Closing ...\n");
				if (pPDB)
				{
					pPDB->fEnabled = FALSE;
				}
			}
			break;
		}
#endif		
		case WM_CREATE:
		{
			return 0;
		}

		case WM_DESTROY:
		{
			if (pPDB)
			{
				if (pPDB->hMainWnd)
				{
					GenerateConsoleCtrlEvent (CTRL_C_EVENT, 0);	
				}
				else
				{
					PostQuitMessage (0);
				}
			}

			return 0;
		}

	    case WM_KEYDOWN:
		{
			if (pPDB)
			{
				switch(wParam)
				{
					case VK_SPACE:
					{
						pPDB->fSingleStep=FALSE;
						pPDB->fEnabled=pPDB->fEnabled?FALSE:TRUE;
						break;
					}
					case VK_RETURN:
					{
						pPDB->fSingleStep=TRUE;
						pPDB->fEnabled = TRUE;
						break;
					}
				}
				
			}
			break;
		}
	}

	return DefWindowProc (hWnd, message, wParam, lParam);
}

/**********************************************************************/
/**/
/**/
/**/
/**/
/**********************************************************************/
static HWND GetConsoleHwnd (void)
{
    #define MY_BUFSIZE 1024 	/* buffer size for console window titles */
    HWND hWndFound = NULL;		/* this is what is returned to the caller */
    char pszNewWindowTitle[MY_BUFSIZE]; /* contains fabricated WindowTitle */
    char pszOldWindowTitle[MY_BUFSIZE]; /* contains original WindowTitle */
 
    /* fetch current window title */
 
    if (GetConsoleTitle (pszOldWindowTitle, MY_BUFSIZE))
    {
	    /* format a "unique" NewWindowTitle */
	 
	    wsprintf (pszNewWindowTitle,"%d/%d", GetTickCount (), GetCurrentProcessId ());
	 
	    /* change current window title */
	 
	    SetConsoleTitle (pszNewWindowTitle);
	 
	    /* ensure window title has been updated */
	 
		PVROSDelay(PVR_DELAY_MS, 40);
	 
	    /* look for NewWindowTitle */
	 
	    hWndFound = FindWindow (NULL, pszNewWindowTitle);
	 
	    /* restore original window title */
	 
	    SetConsoleTitle (pszOldWindowTitle);
    }
 
    return (hWndFound);
}
 

/**********************************************************************/
/**/
/**/
/**/
/**/
/**********************************************************************/
void HandleBackgroundWindowMessage ()
{
	MSG msg;

  	while (PeekMessage (&msg, NULL, 0, 0, PM_NOREMOVE))
    {
        if (!GetMessage (&msg, NULL, 0, 0))
        {
			return;
        }
		else
		{
	        TranslateMessage (&msg); 
	        DispatchMessage (&msg);
        }
    }
}	

/******************************************************************************
 * Function Name: PVROSSetDDrawModeWindowHandle
 * Inputs		: 
 * Outputs		: 
 * Returns		: DDraw window handle 
 * Description  : Sets window handle from last sgl_use_ddraw_mode call
 *****************************************************************************/

static HWND					hAppWnd = NULL;

void PVROSSetDDrawWindowHandle(void *hWnd)
{
	hAppWnd = (HWND)hWnd;
}

/******************************************************************************
 * Function Name: CreateDDrawWnd
 *
 * Inputs       : 
 * Outputs      : 
 * Returns      : 
 * Globals Used : -
 *
 * Description  : Creates and Destroys the DDraw background window
 *****************************************************************************/
BOOL CreateDDrawWnd (PRIVATE_DISPLAY_BLOCK *pPDB, BOOL fCreate)
{
	if (fCreate)
	{
		if (pPDB->hWnd == NULL)
		{
			if (!pPDB->fRenderToDesktop && hAppWnd)
			{
				DPF ((DBG_WARNING, "Using DDraw window handle"));
				
				pPDB->hWnd = hAppWnd;

				pPDB->fUsingClientDDrawWindow = TRUE;
				pPDB->fEnabled = TRUE;
				pPDB->fSingleStep = FALSE;

				return (TRUE);
			}
			else
			{
				WNDCLASS wc;
				DWORD dwExStyle, dwStyle;
				HBRUSH hBackground;
				
				#if 0
				dwExStyle = WS_EX_TOPMOST;
				dwStyle = WS_POPUP|WS_VISIBLE;
				hBackground = GetStockObject (BLACK_BRUSH);
				#endif
				
				dwExStyle = 0;
				dwStyle = WS_POPUP;
				hBackground = NULL;

				wc.style =			0;
				wc.lpfnWndProc =	DDrawWndProc;
				wc.cbClsExtra =		0;
				wc.cbWndExtra =		4;
				wc.hInstance =		pPDB->hInstance;
				wc.hIcon =			NULL;
				wc.hCursor =		LoadCursor (NULL, IDC_ARROW);
				wc.hbrBackground =	hBackground;
				wc.lpszMenuName =	NULL;
				wc.lpszClassName =	"SGLWndClassName";

				if (RegisterClass (&wc))
				{
					char	szTitleBuffer[512];
					
					GetModuleFileName (NULL, szTitleBuffer, sizeof (szTitleBuffer));
					
					pPDB->hWnd = CreateWindowEx (dwExStyle,
												"SGLWndClassName",
												szTitleBuffer,
												dwStyle,
												0, 0,
												GetSystemMetrics (SM_CXSCREEN),
												GetSystemMetrics (SM_CYSCREEN),
												NULL,
												(HMENU) NULL,
												pPDB->hInstance,
												NULL);
					if (pPDB->hWnd)
					{
						SetWindowLong (pPDB->hWnd, GWL_USERDATA, (LONG) pPDB);
						
						pPDB->hMainWnd = GetConsoleHwnd ();
						
						return (TRUE);
					}
					else
					{
						DPF ((DBG_ERROR, "Create window failed"));
					}
				}
				else
				{
					DPF ((DBG_ERROR, "Register class failed"));
				}
				
				return (FALSE);				
			}
		}
	}
	else
	{
		if (pPDB->hWnd != NULL)
		{
			if (pPDB->fUsingClientDDrawWindow)
			{
				pPDB->fUsingClientDDrawWindow = FALSE;
			}
			else
			{
				SetWindowLong (pPDB->hWnd, GWL_USERDATA, 0);
				DestroyWindow (pPDB->hWnd);
				pPDB->hWnd = NULL;
			}
		}
	}

	return (TRUE);
}

/* static functions for this module */

/******************************************************************************
 * Function Name: GetDisplayModeTable
 *
 * Inputs       : 
 * Outputs      : 
 * Returns      : 
 * Globals Used : -
 *
 * Description  : Fills out a DisplayModeTable struct from the supplied DDraw 
 *				  surface description
 *****************************************************************************/
static void GetDisplayModeTable (DisplayModeTable *pDMT, LPDDSURFACEDESC lpDDSurfaceDesc)
{
	pDMT->hMode = MAKE_HMODE (lpDDSurfaceDesc->dwWidth,
							  lpDDSurfaceDesc->dwHeight,
							  lpDDSurfaceDesc->ddpfPixelFormat.dwRGBBitCount,
							  0);

	pDMT->XResolution =	lpDDSurfaceDesc->dwWidth;
	pDMT->YResolution =	lpDDSurfaceDesc->dwHeight;
	pDMT->BitDepth =	lpDDSurfaceDesc->ddpfPixelFormat.dwRGBBitCount;

	#if 0
	DumpDisplayModeTable (pDMT);
	#endif
}


/******************************************************************************
 * Function Name: GetDisplayModeDataBlock
 *
 * Inputs       : 
 * Outputs      : 
 * Returns      : 
 * Globals Used : -
 *
 * Description  : Fills in a DisplayModeDataBlock using the context's current front buffer
 *****************************************************************************/
static void GetDisplayModeDataBlock (DisplayModeDataBlock *pDMDB, PRIVATE_DISPLAY_BLOCK *pPDB)
{
	DDSURFACEDESC DDSD;
	
	ASSERT (pPDB);
	ASSERT (pPDB->pFrontBuffer);

	INIT_DDSTRUCT (&DDSD);
	
	DDOK (IDirectDrawSurface2_GetSurfaceDesc (pPDB->pFrontBuffer, &DDSD));

	GetDisplayModeTable (&pPDB->DMDB.DMT, &DDSD);

	if (!ISDDOK (IDirectDraw2_GetMonitorFrequency (pPDB->pDDraw2, &pDMDB->RefreshRate)))
	{
		pDMDB->RefreshRate = 0;
	}
	
	pDMDB->RedMask =	DDSD.ddpfPixelFormat.dwRBitMask;
	pDMDB->GreenMask =	DDSD.ddpfPixelFormat.dwGBitMask;
	pDMDB->BlueMask =	DDSD.ddpfPixelFormat.dwBBitMask;
	
	/* Check if we are in 15 bit mode.
	 */
	if (DDSD.ddpfPixelFormat.dwGBitMask == 0x3E0)
	   DDSD.ddpfPixelFormat.dwRGBBitCount = 15;

	if (DDSD.dwFlags & DDSD_BACKBUFFERCOUNT)
	{
		pDMDB->NumberOfBuffers = DDSD.dwBackBufferCount;
	}
	else
	{
		pDMDB->NumberOfBuffers = 0;
	}

	pDMDB->CurrentBuffer = 0;
	
/*	pDMDB->PhysBufferAddress   [3];
	pDMDB->LinearBufferAddress [3]; */

	DumpDisplayModeDataBlock (pDMDB);
}

/******************************************************************************
 * Function Name: EnumModesCallback
 *
 * Inputs       : 
 * Outputs      : 
 * Returns      : 
 * Globals Used : -
 *
 * Description  : Called back from the DDraw EnumModes API function
 *****************************************************************************/
static HRESULT WINAPI EnumModesCallback (LPDDSURFACEDESC lpDDSurfaceDesc, LPVOID lpContext)
{
	PMODECBSTRUCT pMCBS = lpContext;

	ASSERT (lpContext);

	if (pMCBS->pDMT)
	{
		GetDisplayModeTable (pMCBS->pDMT + pMCBS->nDisplayModes, lpDDSurfaceDesc);
	}

	++pMCBS->nDisplayModes;

	return (DDENUMRET_OK);
}

/******************************************************************************
 * Function Name: DDEnumDevicesCallback
 *
 * Inputs       : 
 * Outputs      : 
 * Returns      : 
 * Globals Used : -
 *
 * Description  : Called back from the DDraw EnumDevices API function
 *****************************************************************************/
static BOOL WINAPI DDEnumDevicesCallback (GUID FAR * lpGUID,
    							   LPSTR lpDriverDescription, 
								   LPSTR lpDriverName,
								   LPVOID lpContext)
{
	PDEVICECBSTRUCT pDCBS = lpContext;
	BOOL fRet = DDENUMRET_CANCEL;
	
	ASSERT (pDCBS);

	if (!pDCBS->pDDT)
	{
		fRet = DDENUMRET_OK;
		pDCBS->nDevices++;
	}
	else
	{
		LPDIRECTDRAW pDDraw;
			
		if (ISDDOK (DirectDrawCreate (lpGUID, &pDDraw, NULL)))
		{
			LPDIRECTDRAW2	pDDraw2;

			ASSERT (pDDraw);
			
			if (ISDDOK (IDirectDraw_QueryInterface (pDDraw, &IID_IDirectDraw2, &pDDraw2)))
			{
				DDSURFACEDESC	DDSD;

				ASSERT (pDDraw2);

				INIT_DDSTRUCT (&DDSD);

				if (ISDDOK (IDirectDraw2_GetDisplayMode (pDDraw2, &DDSD)))
				{
					if (DDSD.ddpfPixelFormat.dwFlags & DDPF_RGB)
					{
						pDCBS->pDDT[pDCBS->nDevices].ID = lpGUID;

						GetDisplayModeTable (&pDCBS->pDDT[pDCBS->nDevices].DMT, &DDSD);

						pDCBS->nDevices++;
					}

					fRet = DDENUMRET_OK;
				}

				DDOK (IDirectDraw2_Release (pDDraw2));
			}

			DDOK (IDirectDraw_Release (pDDraw));
		}
	}

	return (fRet);
}

/**********************************************************************/
/**/
/**/
/**/
/**/
/**********************************************************************/

sgl_uint32 RestoreSurfacesIfLost (PRIVATE_DISPLAY_BLOCK *pPDB)
{
	ASSERT (pPDB);
	
	if (pPDB->pFrontBuffer)
	{
		HRESULT hRes = DDOK (IDirectDrawSurface2_IsLost (pPDB->pFrontBuffer));
		
		if (hRes == DD_OK)
		{
			/* No action necessary */
			
			return (TRUE);
		}
		else if (hRes == DDERR_SURFACELOST)
		{
			DPF ((DBG_WARNING, "RestoreSurfacesIfLost: restoring lost frame buffers"));

			if (ISDDOK (IDirectDrawSurface2_Restore (pPDB->pFrontBuffer)))
			{
				if (pPDB->DMDB.NumberOfBuffers > 0)
				{
					DDSCAPS DDSC;

					DDSC.dwCaps = DDSCAPS_BACKBUFFER;

					DDOK (IDirectDrawSurface2_GetAttachedSurface (pPDB->pFrontBuffer, 
																   &DDSC, 
																   &pPDB->pBackBuffer));
				}
				else
				{
					pPDB->pBackBuffer = pPDB->pFrontBuffer;
				}

				if (pPDB->pBackBuffer)
				{
					GetDisplayModeDataBlock (&pPDB->DMDB, pPDB);

					return (TRUE);
				}
			}
	
			return (FALSE);		
		}
	}

	return (TRUE);
}	

/* Display API entry points */

/**********************************************************************/
/**/
/**/
/**/
/**/
/**********************************************************************/

sgl_uint32 CALL_CONV PVROSEnumDisplayDevices (DisplayDeviceTable *pDDT)
{
	DEVICECBSTRUCT DCBS;

	DCBS.nDevices = 0;
	DCBS.pDDT = pDDT;
	
	if (ISDDOK (DirectDrawEnumerate (DDEnumDevicesCallback, &DCBS)))
	{
		return ((sgl_uint32) DCBS.nDevices);
	}
	else
	{
		return (0);
	}
}

/**********************************************************************/
/**/
/**/
/**/
/**/
/**********************************************************************/

HDISPLAY CALL_CONV PVROSOpenDisplayDevice( void *pLogDevice,
										   DISPLAYID ID,
										   FRAME_BUFFER_MODE FrameBufferMode )
{
	PRIVATE_DISPLAY_BLOCK *pPDB = PVROSMalloc (sizeof (PRIVATE_DISPLAY_BLOCK));

	if (pPDB)
	{
		INIT_DDSTRUCT (pPDB)

		pPDB->FrameBufferMode = FrameBufferMode;
		pPDB->hInstance = PVROSGetInstance();
		/* not necessarily true, but will do for the moment */
		pPDB->fCorrectMode = TRUE;
		pPDB->fSingleStep = FALSE;

		pPDB->fStrictLocks = (BOOL) HWRdValFileUInt ("StrictLocks", FALSE);

		if (FrameBufferMode == FRAME_BUFFER_ACTIVE)
		{
			if (GetPrivateProfileInt ("Debug", "RenderToDesktop", 0, "sglhw.ini"))
			{
				PVROSPrintf("PVROSOpenDisplayDevice: RenderToDesktop requested\n");	

				pPDB->fRenderToDesktop = TRUE;
			}
			else
			{
				DPF ((DBG_WARNING, "Opening active mode display device"));	
			}
											
			if (ISDDOK (DirectDrawCreate ((LPGUID) ID, &pPDB->pDDraw, NULL)))
			{
				if (ISDDOK (IDirectDraw_QueryInterface (pPDB->pDDraw, &IID_IDirectDraw2, &pPDB->pDDraw2)))
				{
					if (CreateDDrawWnd (pPDB, TRUE))
					{
						if (ISDDOK (IDirectDraw2_SetCooperativeLevel (pPDB->pDDraw2,
																	  pPDB->hWnd, 
																	  DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN)))
						{
							PVROSCallbackRegister (pLogDevice, CB_PRE_RENDER, 
												   PVROSPreRenderCallbackAuto, 
												   pPDB);
													
							PVROSCallbackRegister (pLogDevice, CB_POST_RENDER, 
												   PVROSPostRenderCallbackAuto, 
												   pPDB);

							if (pPDB->fStrictLocks)
							{
								PVROSCallbackRegister (pLogDevice, CB_END_OF_RENDER, 
													   PVROSEndOfRenderCallbackAuto, 
													   pPDB);
							}

							return ((HDISPLAY) pPDB);
						}

						CreateDDrawWnd (pPDB, FALSE);
					}

					DDOK (IDirectDraw2_Release (pPDB->pDDraw2));
				}

				DDOK (IDirectDraw_Release (pPDB->pDDraw));
			}
		}
		else
		{
			ASSERT (FrameBufferMode == FRAME_BUFFER_PASSIVE);	

			DPF ((DBG_WARNING, "Opening passive mode display device"));	

			return ((HDISPLAY) pPDB);
		}

		PVROSFree (pPDB);
	}

	return (0);
}

/**********************************************************************/
/**/
/**/
/**/
/**/
/**********************************************************************/

PVROSERR CALL_CONV PVROSCloseDisplayDevice ( void *pLogDevice,
											 HDISPLAY hDisplay )
{
	PRIVATE_DISPLAY_BLOCK *pPDB = hDisplay;

	if (pPDB)
	{
		if (pPDB->FrameBufferMode == FRAME_BUFFER_ACTIVE)
		{
			if (pPDB->pDDraw)
			{
				if (pPDB->pDDraw2)
				{
					PVROSCallbackUnRegister (pLogDevice, CB_PRE_RENDER, 
											PVROSPreRenderCallbackAuto, 
											pPDB);
											
					PVROSCallbackUnRegister (pLogDevice, CB_POST_RENDER, 
											PVROSPostRenderCallbackAuto, 
											pPDB);

					if (pPDB->fStrictLocks)
					{
						PVROSCallbackUnRegister (pLogDevice, CB_END_OF_RENDER, 
											   PVROSEndOfRenderCallbackAuto, 
											   pPDB);
					}

					if (pPDB->pFrontBuffer)
					{
						DDOK (IDirectDrawSurface2_Release (pPDB->pFrontBuffer));
					}

					if (pPDB->fRestoreModeNeeded)
					{
						DDOK (IDirectDraw2_RestoreDisplayMode (pPDB->pDDraw2));
					}

					if (!pPDB->fRenderToDesktop)
					{
						DDOK (IDirectDraw2_SetCooperativeLevel (pPDB->pDDraw2, pPDB->hWnd, DDSCL_NORMAL));

						CreateDDrawWnd (pPDB, FALSE);
					}

					DDOK (IDirectDraw2_Release (pPDB->pDDraw2));
				}

				DDOK (IDirectDraw_Release (pPDB->pDDraw));
			}
		}
		else
		{
			DPF ((DBG_WARNING, "Closing passive mode display device"));	
		}
	
		PVROSFree (pPDB);
	}

	return (0);
}

/**********************************************************************/
/**/
/**/
/**/
/**/
/**********************************************************************/
sgl_uint32 CALL_CONV PVROSEnumDisplayModes (HDISPLAY hDisplay, DisplayModeTable *pDMT)
{
	MODECBSTRUCT			MCBS;
	PRIVATE_DISPLAY_BLOCK	*pPDB = hDisplay;

	if (pPDB && pPDB->pDDraw && pPDB->pDDraw2)
	{
		MCBS.nDisplayModes = 0;
		MCBS.pDMT = pDMT;

		if (ISDDOK (IDirectDraw_EnumDisplayModes (pPDB->pDDraw2, 
										DDEDM_REFRESHRATES, 
										NULL, 
										&MCBS, 
										EnumModesCallback)))
		{
			return ((sgl_uint32) MCBS.nDisplayModes);
		}
	}

	return (0);
}

/**********************************************************************/
/**/
/**/
/**/
/**/
/**********************************************************************/
PVROSERR CALL_CONV PVROSSelectDisplayMode (HDISPLAY hDisplay, HMODE hMode, sgl_uint32 nBuffers)
{
	PVROSERR Err = PVROS_DODGY;
	PRIVATE_DISPLAY_BLOCK *pPDB = hDisplay;

	if (!pPDB || !pPDB->pDDraw || !pPDB->pDDraw2 || !hMode)
	{
		DPF ((DBG_WARNING, "SelectDisplayMode: bad parameters"));
	}
	else if (pPDB->FrameBufferMode != FRAME_BUFFER_ACTIVE)
	{
		DPF ((DBG_WARNING, "SelectDisplayMode: can't do it in FRAME_BUFFER_PASSIVE mode"));
	}
	else
	{
		DDSURFACEDESC DDSD;

		INIT_DDSTRUCT (&DDSD)

		if (pPDB->pFrontBuffer)
		{
			DDOK (IDirectDrawSurface2_Release (pPDB->pFrontBuffer));
			pPDB->pFrontBuffer = NULL;
		}

		if (pPDB->fRenderToDesktop)
		{
			DDSD.dwFlags = DDSD_CAPS;
			DDSD.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_VIDEOMEMORY;

			if (ISDDOK (IDirectDraw2_CreateSurface (pPDB->pDDraw2, 
													&DDSD, 
													&pPDB->pFrontBuffer, 0)))
			{
				pPDB->pBackBuffer = pPDB->pFrontBuffer;

				GetDisplayModeDataBlock (&pPDB->DMDB, pPDB);

				FillSurface (pPDB->pFrontBuffer, 
							 Black, 
							 HMODE_XRES (hMode), 
							 HMODE_YRES (hMode),
							 pPDB->DMDB.DMT.BitDepth);

				

				return (PVROS_GROOVY);
			}
		}
		else
		{
			DPF ((DBG_WARNING, "Setting mode %d x %d, %d bit (%d)", 
												HMODE_XRES (hMode), 
												HMODE_YRES (hMode), 
												HMODE_BITSPERPIXEL (hMode), 
												HMODE_REFRESHRATE (hMode),
												0));

			if (ISDDOK (IDirectDraw2_SetDisplayMode (pPDB->pDDraw2, 
												HMODE_XRES (hMode), 
												HMODE_YRES (hMode), 
												HMODE_BITSPERPIXEL (hMode), 
												HMODE_REFRESHRATE (hMode),
												0)))
			{
				pPDB->fRestoreModeNeeded = TRUE;
				pPDB->fCorrectMode = TRUE;

				INIT_DDSTRUCT (&DDSD)

				if (nBuffers > 1)
				{
					DDSD.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
					DDSD.dwBackBufferCount = nBuffers - 1;
					DDSD.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE |
											DDSCAPS_FLIP |
											DDSCAPS_COMPLEX |
											DDSCAPS_VIDEOMEMORY;
				}
				else
				{
					DDSD.dwFlags = DDSD_CAPS;
					DDSD.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE |
											DDSCAPS_VIDEOMEMORY;
				}
				
				if (ISDDOK (IDirectDraw2_CreateSurface (pPDB->pDDraw2, 
														&DDSD, 
														&pPDB->pFrontBuffer, 0)))
				{
					if (nBuffers > 1)
					{
						DDSCAPS DDSC;

						DDSC.dwCaps = DDSCAPS_BACKBUFFER;

						DDOK (IDirectDrawSurface2_GetAttachedSurface (pPDB->pFrontBuffer, 
																	   &DDSC, 
																	   &pPDB->pBackBuffer));
					}
					else
					{
						pPDB->pBackBuffer = pPDB->pFrontBuffer;
					}

					if (pPDB->pBackBuffer)
					{
						GetDisplayModeDataBlock (&pPDB->DMDB, pPDB);

						FillSurface (pPDB->pFrontBuffer, 
									 Black, 
									 HMODE_XRES (hMode), 
									 HMODE_YRES (hMode),
									 pPDB->DMDB.DMT.BitDepth);

						return (PVROS_GROOVY);
					}
		
					DDOK (IDirectDrawSurface2_Release (pPDB->pFrontBuffer));
				}
				else
				{
					Err = PVROS_TOO_MANY_BUFFERS;
				}

				DDOK (IDirectDraw2_RestoreDisplayMode (pPDB->pDDraw2));

				pPDB->pFrontBuffer = NULL;
				pPDB->fRestoreModeNeeded = FALSE;
			}
		}
	}

	return (Err);
}

/**********************************************************************/
/**/
/**/
/**/
/**/
/**********************************************************************/
PVROSERR CALL_CONV PVROSGetDisplayModeInfo ( HDISPLAY hDisplay, 
											 DisplayModeDataBlock *pDMDB )
{
	PRIVATE_DISPLAY_BLOCK *pPDB = hDisplay;

	if (!pPDB || !pDMDB)
	{
		DPF ((DBG_WARNING, "GetDisplayModeInfo: bad parameters"));
	}
	else if (pPDB->FrameBufferMode != FRAME_BUFFER_ACTIVE)
	{
		DPF ((DBG_WARNING, "GetDisplayModeInfo: can't do it in FRAME_BUFFER_PASSIVE mode"));
	}
	else if (!pPDB->pFrontBuffer)
	{
		DPF ((DBG_WARNING, "GetDisplayModeInfo: Primary surface not created yet!"));
	}
	else
	{
		*pDMDB = pPDB->DMDB;

		return (PVROS_GROOVY);
	}

	return (PVROS_DODGY);
}

/**********************************************************************/
/**/
/**/
/**/
/**/
/**********************************************************************/
PVROSERR CALL_CONV PVROSFlipFrameBuffer (HDISPLAY hDisplay)
{
	PRIVATE_DISPLAY_BLOCK *pPDB = hDisplay;

	if (!pPDB)
	{
		DPF ((DBG_WARNING, "FlipFrameBuffer: bad parameters"));
	}
	else if (pPDB->FrameBufferMode != FRAME_BUFFER_ACTIVE)
	{
		DPF ((DBG_WARNING, "FlipFrameBuffer: can't do it in FRAME_BUFFER_PASSIVE mode"));
	}
	else if (!pPDB->pFrontBuffer)
	{
		DPF ((DBG_WARNING, "FlipFrameBuffer: Primary surface not created yet!"));
	}
	else
	{
		DDSCAPS DDSC;

		DDSC.dwCaps = DDSCAPS_BACKBUFFER;

		if (RestoreSurfacesIfLost (pPDB))
		{
			if (pPDB->DMDB.NumberOfBuffers < 1)
			{
				/* No flipping surfaces, rendering straight to front buffer */

				return (PVROS_GROOVY);
			}
			else
			{
				/* do flip */
				if (ISDDOK (IDirectDrawSurface2_Flip (pPDB->pFrontBuffer, NULL, DDFLIP_WAIT)))
				{
					if (pPDB->DMDB.NumberOfBuffers == 1)
					{
						/* if we double (as opposed to triple buffering)
						 * we need to wait for the flip.
						 */
						
						while (DDOK (IDirectDrawSurface2_GetFlipStatus (
								pPDB->pFrontBuffer, DDGFS_ISFLIPDONE)) 
								== DDERR_WASSTILLDRAWING);
					}
							
					/* get new back buffer to render into */
					if (ISDDOK (IDirectDrawSurface2_GetAttachedSurface (pPDB->pFrontBuffer, 
																		&DDSC, 
																		&pPDB->pBackBuffer)))
					{
						return (PVROS_GROOVY);
					}
				}
			}
		}
	}

	return (PVROS_DODGY);
}

/**********************************************************************/
/**/
/**/
/**/
/**/
/**********************************************************************/
PVROSERR CALL_CONV PVROSPreRenderCallbackAuto (void *pData, HDISPLAY hDisplay)
{
	PRIVATE_DISPLAY_BLOCK *pPDB = hDisplay;
	PRE_RENDER_CB_STRUCT  *pPRCS = pData;
	DDSURFACEDESC		  DDSD;
	
	DPF ((DBG_VERBOSE, "PVROSPreRenderCallbackAuto"));	

	ASSERT (pPDB);
	ASSERT (pPRCS);
	
	INIT_DDSTRUCT (&DDSD);

	/* if single step mode is specified set Enabled to false so that 
	** application will wait in the post render callback
	*/
	if(pPDB->fSingleStep)
	{
		pPDB->fEnabled = FALSE;
	}
	
	if (pPRCS->FlipRequested)
	{
		if (PVROSFlipFrameBuffer (hDisplay) != PVROS_GROOVY)
		{
			return (PVROS_DODGY);
		}
	}
	else if (!RestoreSurfacesIfLost (pPDB))
	{
		return (PVROS_DODGY);
	}
	
	if (ISDDOK (IDirectDrawSurface2_Lock (pPDB->pBackBuffer, 
					NULL,
					&DDSD,
					DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT | DDLOCK_WRITEONLY, 
					0)))
	{
		pPRCS->PhysRenderBufferAddress		= PVROSMapLinearToPhysical ((sgl_uint32)DDSD.lpSurface);
		pPRCS->PhysRenderBufferBitsPerPixel = DDSD.ddpfPixelFormat.dwRGBBitCount;

		/* Check if we are in 15 bit mode.
		 */
		if (DDSD.ddpfPixelFormat.dwGBitMask == 0x3E0)
			pPRCS->PhysRenderBufferBitsPerPixel = 15;

		pPRCS->PhysRenderBufferStride		= DDSD.lPitch;
		
		if (!pPDB->fStrictLocks)
		{
			DDOK (IDirectDrawSurface2_Unlock (pPDB->pBackBuffer, NULL));
		}

		return (PVROS_GROOVY);	
	}

		
	return (PVROS_DODGY);	
}	

/**********************************************************************/
/**/
/**/
/**/
/**/
/**********************************************************************/
PVROSERR CALL_CONV PVROSPostRenderCallbackAuto (void *pData, HDISPLAY hDisplay)
{
	PRIVATE_DISPLAY_BLOCK *pPDB = hDisplay;

	DPF ((DBG_VERBOSE, "PVROSPostRenderCallbackAuto"));	
	
	ASSERT (pPDB);

	if (!pPDB->fUsingClientDDrawWindow)
	{
		HandleBackgroundWindowMessage ();
	}

	if(pPDB->fEnabled)
	{
		return(PVROS_GROOVY);
	}

	while(1)
	{

		if (!pPDB->fUsingClientDDrawWindow)
		{
			HandleBackgroundWindowMessage ();
		}

		Sleep(2);
		if(pPDB->fEnabled && pPDB->fCorrectMode)
		{
			if(pPDB->fSingleStep)
			{
				pPDB->fEnabled = FALSE;
			}

			break;
		}

	}
	
	return (PVROS_GROOVY);	
}	

/**********************************************************************/
/**/
/**/
/**/
/**/
/**********************************************************************/
PVROSERR CALL_CONV PVROSEndOfRenderCallbackAuto (void *pData, HDISPLAY hDisplay)
{
	PRIVATE_DISPLAY_BLOCK *pPDB = hDisplay;

	DPF ((DBG_VERBOSE, "PVROSEndOfRenderCallbackAuto"));	
	
	ASSERT (pPDB);
	ASSERT (pPDB->fStrictLocks)

	DDOK (IDirectDrawSurface2_Unlock (pPDB->pBackBuffer, NULL));

	Sleep(3);

	return (PVROS_GROOVY);	
}	

/**********************************************************************/
/**/
/**/
/* private entry point for windows implementation */
/**/
/**/
/**********************************************************************/

PVROSERR CALL_CONV GetDDraw2dCallbackInfo( HDISPLAY hDisplay, void *pData )
{
	PRIVATE_DISPLAY_BLOCK *pPDB = hDisplay;
	CALLBACK_SURFACE_PARAMS *pCSP = pData;
	
	ASSERT (pPDB);
	ASSERT (pCSP);
	
	if (RestoreSurfacesIfLost (pPDB))
	{
		DDSURFACEDESC DDSD;
		
		INIT_DDSTRUCT (&DDSD);

		if (ISDDOK (IDirectDrawSurface2_Lock (pPDB->pBackBuffer, 
						NULL,
						&DDSD,
						DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT | DDLOCK_WRITEONLY, 
						0)))
		{
			pCSP->pDDObject = 			pPDB->pDDraw;
			pCSP->p3DSurfaceObject = 	pPDB->pBackBuffer;
			pCSP->p3DSurfaceMemory = 	DDSD.lpSurface;
			pCSP->wBitsPerPixel = 		(sgl_uint16)DDSD.ddpfPixelFormat.dwRGBBitCount;
			pCSP->dwStride = 			DDSD.lPitch;
			pCSP->wWidth = 				(sgl_uint16)pPDB->DMDB.DMT.XResolution;
			pCSP->wHeight = 			(sgl_uint16)pPDB->DMDB.DMT.YResolution;

			DDOK (IDirectDrawSurface2_Unlock (pPDB->pBackBuffer, NULL));

			return (PVROS_GROOVY);	
		}
		else
		{
			DPF ((DBG_ERROR, "GetDDraw2dCallbackInfo: back buffer lock failed"));
		}
	}
	
	return (PVROS_DODGY);	
}	

/* end of $RCSfile: display.c,v $ */

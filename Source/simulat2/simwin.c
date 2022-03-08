/*****************************************************************************
 *;++
 * Name           : $RCSfile: simwin.c,v $
 * Title          : SIMWIN.C
 * C Author       : Jim Page
 * Created        : 21/04/95
 *
 * Copyright	: 1995-2022 Imagination Technologies (c)
 * License		: MIT
 *
 * Description    : SGL simulator window stuff (display/debug)
 *                   
 * Program Type   : C module (.dll)
 *
 * RCS info:
 *
 * $Date: 1995/12/18 14:22:25 $
 * $Revision: 1.1 $
 * $Locker:  $
 * $Log: simwin.c,v $
 * Revision 1.1  1995/12/18  14:22:25  sjf
 * Initial revision
 *
 * Revision 1.6  1995/10/20  10:25:24  jop
 * Mad ICC aware
 *
 * Revision 1.5  1995/09/18  12:04:32  jop
 * Added various features
 *
 * Revision 1.4  1995/08/21  11:56:24  jop
 * Clmping added to nX and nY
 *
 * Revision 1.3  1995/07/26  16:28:03  jop	 
 * added buffer cleaning between passes
 *
 * Revision 1.2  1995/07/26  15:11:55  jop
 * Added some stuff
 *
 * Revision 1.1  1995/07/23  17:25:18  jop
 * Initial revision
 *
 *
 *;--
 *****************************************************************************/

#define __export

#include <stdio.h>
#include <windows.h>

#include "sgl.h"
#include "sgl_defs.h"
#include "simwin.h"
#include "sglres.h"
#include "sglmacro.h"
#include "../profile.h"

HWND 	ghSimWin = NULL;		/* handle of dialog window */
extern HANDLE 	ghInstance;	/* module handle (saved at libmain) */

static HDC		ghMemoryDC = NULL;
static HBITMAP	ghBitmap0 = NULL;
static HBITMAP	ghBitmap1 = NULL;
static int		gnBitmap = 0;
static int		gnXimage, gnYimage;

static int 			gbPreview = FALSE;
static int			gbBufferDirty = FALSE;
static int			gbBufferClean = TRUE;
static BITMAPINFO	gbmi;
static char			*gpBuffer;
static int			gnWidthBytes;

void CheckRenderBuffer (int);

BOOL __export CALLBACK SimWinDlgProc (HWND hDlg, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
	BOOL fRet = TRUE;
	
	switch (wMsg)
	{
		case WM_INITDIALOG:
		{
			RECT rcClient, rcWindow;
			int nBX, nBY;
			
			GetWindowRect (hDlg, &rcWindow);
			GetClientRect (hDlg, &rcClient);
			ClientToScreen (hDlg, (LPPOINT) &rcClient.left);
			ClientToScreen (hDlg, (LPPOINT) &rcClient.right);
			nBX = (rcWindow.right - rcWindow.left) - (rcClient.right - rcClient.left);
			nBY = (rcWindow.bottom - rcWindow.top) - (rcClient.bottom - rcClient.top);
			
			SetWindowPos (hDlg, HWND_TOP, NULL, NULL, gnXimage + nBX, gnYimage + nBY, SWP_NOMOVE | SWP_NOZORDER | SWP_NOREDRAW);
			break;
		}
		
		case WM_PAINT:
		{
			PAINTSTRUCT ps;

			BeginPaint (hDlg, &ps);
			if (gbPreview)
			{
				BitBlt (ps.hdc, 0, 0, gnXimage, gnYimage, ghMemoryDC, 0, 0, SRCCOPY);
			}
			else
			{
				SelectObject (ghMemoryDC, gnBitmap ? ghBitmap0 : ghBitmap1);
				BitBlt (ps.hdc, 0, 0, gnXimage, gnYimage, ghMemoryDC, 0, 0, SRCCOPY);
				SelectObject (ghMemoryDC, gnBitmap ? ghBitmap1 : ghBitmap0);
			}
			EndPaint (hDlg, &ps);
			break;
		}
		
		case WM_COMMAND:
		{
			switch (wParam)
			{
				case IDOK:
				case IDCANCEL:
				{
					EndDialog (hDlg, 0);
					break;
				}

				case IDC_PREVIEW:
				{
					gbPreview = TRUE;
					CheckRenderBuffer (FALSE);
					InvalidateRect (ghSimWin, NULL, FALSE);
					UpdateWindow (ghSimWin);
					gbPreview = FALSE;
					break;
				}
				
				default:
				{
					fRet = FALSE;
					break;
				}
			}
			break;
		}

		default:
		{
			fRet = FALSE;
			break;
		}
	}
	return (fRet);
}

int SimWinInit (int nXImage, int nYImage)
{
	gnXimage = nXImage;
	gnYimage = nYImage;

	#if defined __WATCOMC__
	SetFPUPrecisionLow ();
	#endif

	ghSimWin = CreateDialogParam (ghInstance, "ImageWindow", NULL, SimWinDlgProc, MAKELONG (nXImage, nYImage));

	if (ghSimWin)
	{
		HDC hWinDC;
		
		/* create double buffered memoryDC */

		hWinDC = GetDC (ghSimWin);
		ghMemoryDC = CreateCompatibleDC (NULL);

		if (ghMemoryDC)
		{
			ghBitmap0 = CreateCompatibleBitmap (hWinDC, nXImage, nYImage);
			ghBitmap1 = CreateCompatibleBitmap (hWinDC, nXImage, nYImage);
	
			if (ghBitmap0 && ghBitmap1)
			{
				RECT 	rc;
				
				SelectObject (ghMemoryDC, ghBitmap0);
				SelectObject (ghMemoryDC, GetStockObject (BLACK_PEN));
				SelectObject (ghMemoryDC, GetStockObject (BLACK_BRUSH));

				rc.left = 0;
				rc.top = 0;
				rc.right = nXImage;
				rc.bottom = nYImage;

				FillRect (ghMemoryDC, &rc, GetStockObject (WHITE_BRUSH));
				
				ShowWindow (ghSimWin, SW_SHOW);

				gnWidthBytes = ((nXImage * 3) + 3) & 0xFFFFFFFC;
				
				gbmi.bmiHeader.biSize = sizeof (gbmi.bmiHeader);
				gbmi.bmiHeader.biWidth = nXImage;
				gbmi.bmiHeader.biHeight = nYImage;
				gbmi.bmiHeader.biPlanes = 1;
				gbmi.bmiHeader.biBitCount = 24;
				gbmi.bmiHeader.biCompression = BI_RGB;
				gbmi.bmiHeader.biSizeImage = gnWidthBytes * nYImage;
				gbmi.bmiHeader.biXPelsPerMeter = NULL;
				gbmi.bmiHeader.biYPelsPerMeter = NULL;
				gbmi.bmiHeader.biClrUsed = NULL;
				gbmi.bmiHeader.biClrImportant = NULL;
				
				gpBuffer = malloc (gnWidthBytes * nYImage);
				
				return ((int) ghSimWin);
			}
			else
			{
				DPF ((DBG_ERROR, "SimWinInit: error creating bitmaps"));
			}
		}
		else
		{
			DPF ((DBG_ERROR, "SimWinInit: error creating compatible DC"));
		}

		ReleaseDC (ghSimWin, hWinDC);
	}

	SimWinDeinit ();
	
	return (0);
}

void SimWinDeinit ()
{
	if (ghSimWin)
	{
		DestroyWindow (ghSimWin);
		ghSimWin = NULL;
	}

	if (ghMemoryDC)
	{
		DeleteDC (ghMemoryDC);
		ghMemoryDC = NULL;
	}

	if (ghBitmap0)
	{
		DeleteObject (ghBitmap0);
		ghBitmap0 = 0;
	}
	
	if (ghBitmap1)
	{
		DeleteObject (ghBitmap1);
		ghBitmap1 = 0;
	}

	if (gpBuffer)
	{
		free (gpBuffer);
		gpBuffer = 0;
	}
}

void SimWinLine (int nX1, int nY1, int nX2, int nY2)
{
	if (ghMemoryDC && ghSimWin)
	{
		POINT ptOld;
		
		MoveToEx (ghMemoryDC, nX1, nY1, &ptOld);
		LineTo (ghMemoryDC, nX2, nY2);
	}
}

void SimWinSwapBuffers ()
{
	if (ghMemoryDC && ghSimWin)
	{
		RECT rc;
		int nGridOn = SglReadPrivateProfileInt ("Grid", "Visible", 0, "SGL.INI");		

		/* see if there is any freshly rendered data */

		CheckRenderBuffer (TRUE);

		if (nGridOn)
		{
			int nX, nGridX = SglReadPrivateProfileInt ("Grid", "XSep", 64, "SGL.INI");		
			int nY, nGridY = SglReadPrivateProfileInt ("Grid", "YSep", 64, "SGL.INI");		
			int nGridnX = SglReadPrivateProfileInt ("Grid", "XNum", 10, "SGL.INI");		
			int nGridnY = SglReadPrivateProfileInt ("Grid", "YNum", 8, "SGL.INI");		

			for (nX = 1; nX < nGridnX; ++nX)
			{
				SimWinLine (nX * nGridX, 0, nX * nGridX, nGridnY * nGridY);
			}

			for (nY = 1; nY < nGridnY; ++nY)
			{
				SimWinLine (0, nY * nGridY, nGridnX * nGridX, nY * nGridY);
			}
		}

		/* swap in new buffer */
		
		gnBitmap ^= 1;
		SelectObject (ghMemoryDC, gnBitmap ? ghBitmap1 : ghBitmap0);

		/* clear new buffer */
		
		rc.left = 0;
		rc.top = 0;
		rc.right = gnXimage;
		rc.bottom = gnYimage;

		FillRect (ghMemoryDC, &rc, GetStockObject (WHITE_BRUSH));

		/* paint will use the old buffer */

		InvalidateRect (ghSimWin, NULL, FALSE);
		UpdateWindow (ghSimWin);
	}	
}

void SimWinSetPixel (int nX, int nY, unsigned char nR, unsigned char nG, unsigned char nB)
{
	static nIteration;
	
	if (ghMemoryDC && ghSimWin)
	{
		CHOOSE_MAX (nX, 0);
		CHOOSE_MAX (nY, 0);
		CHOOSE_MIN (nX, gnXimage - 1);
		CHOOSE_MIN (nY, gnYimage - 1);
		
		if (gpBuffer)
		{
			int nOffset = (((gnYimage - 1) - nY) * gnWidthBytes) + (nX * 3);

			gpBuffer[nOffset] = nB;
			gpBuffer[nOffset+1] = nG;
			gpBuffer[nOffset+2] = nR;

			gbBufferDirty = TRUE;
			gbBufferClean = FALSE;
		}

		if (nIteration++ & 0x00000400)
		{
			MSG	msg;
		
			while (PeekMessage (&msg, ghSimWin, 0, 0, PM_REMOVE))
			{
				DispatchMessage (&msg);
			}

			nIteration = 0;
		}
	}
	else
	{
		DPFOO ((DBG_WARNING, "Simulator: window contexts incorrectly set up"));
	}
}

void CheckRenderBuffer (int bClean)
{
	if (gpBuffer)
	{
		if (gbBufferDirty)
		{
			SetDIBitsToDevice (ghMemoryDC, 0, 0, gnXimage, gnYimage, 0, 0, 0, gnYimage, gpBuffer, &gbmi, DIB_RGB_COLORS);
			gbBufferDirty = FALSE;
		}

		if (bClean && !gbBufferClean)
		{
			memset (gpBuffer, 0xFF, gnWidthBytes * gnYimage);
			gbBufferClean = TRUE;
		}
	}
	else
	{
		DPFOO ((DBG_WARNING, "Simulator: window contexts incorrectly set up"));
	}
}

/* end of $RCSfile: simwin.c,v $ */


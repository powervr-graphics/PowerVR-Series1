/**
 * simwin.h 
 *
 * Copyright	: 1995-2022 Imagination Technologies (c)
 * License		: MIT 
 */

int SimWinInit (int nXImage, int nYImage);
void SimWinDeinit (void);
void SimWinLine (int nX1, int nX2, int nY1, int nY2);
void SimWinSetPixel (int nX, int nY, unsigned char nR, unsigned char nG, unsigned char nB);
void SimWinSwapBuffers (void);
void SimWinPreviewImage ();

#ifdef _WINDOWS_
extern HWND	ghSimWin;
#endif

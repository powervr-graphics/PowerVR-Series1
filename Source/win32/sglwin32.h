/*****************************************************************************
Name			:	sglwin32.h
Title			:	Win32 SGL Extensions
Author			:	Videologic PLC
Created			:	9/02/96

Copyright		: 1995-2022 Imagination Technologies (c)
License			: MIT

Description		:	Contains DLL extensions for Win32 SGL support

Program Type	:	Win32 DLL.
					requires windows.h and ddraw.h
					
Revision Control System :

	$Revision: 1.15 $ $Date: 1997/06/05 11:26:08 $ $Release$ $Locker:  $

*****************************************************************************/

#ifdef SGL_DDRAW_STRUCTS_NOT_CONFIGURED
#pragma message ("sglwin32.h: SGL Callback structures not correctly typedef'd")
#pragma message ("sglwin32.h: Please include ddraw.h before including sgl.h")
#endif

#pragma message ("sglwin32.h: this file is obsolete and can be replaced with")
#pragma message ("sglwin32.h: ddraw.h (DirectX SDK)")

#ifndef __DRAW_INCLUDED__
#include "ddraw.h"
#endif


/******************************************************************************
 * Name         : texapip.h
 * Title        : Texture API private header for sgl implementation
 * Author       : Sheila Yang
 * Created      : 17/03/1997
 *
 * Copyright	: 1995-2022 Imagination Technologies (c)
 * License		: MIT
 *
 * Description  :
 * 
 * Platform     : ANSI
 *
 * Modifications:
 * $Date: 1997/06/09 17:18:27 $
 * $Revision: 1.5 $
 * $Locker:  $
 * $Log: texapip.h,v $
 * Revision 1.5  1997/06/09  17:18:27  sxy
 * added an API AutoMipmap.
 *
 * Revision 1.4  1997/05/13  11:20:11  sxy
 * added a new API TextureCopy to copy preprocessed textures into memory.
 *
 * Revision 1.3  1997/04/23  17:12:51  jop
 * Added a missing endif
 *
 * Revision 1.2  1997/04/23  17:03:51  jop
 * fixed filename in header. Nitpicking somewhat.
 *
 * Revision 1.1  1997/04/23  16:57:35  jop
 * Initial revision
 *
 *
 *****************************************************************************/

#ifndef __TEXAPIP_H__
#define __TEXAPIP_H__

#ifndef __TEXAPI_H__
#include "texapi.h"
#endif

sgl_uint32 	CALL_CONV TextureEnumerateFormats (HTEXHEAP hTexHeap, PTEXTUREFORMAT pFormatList);

HTEXTURE 	CALL_CONV TextureCreate (HTEXHEAP hTexHeap, PTEXTURESPEC pTextureSpec);

PVROSERR 	CALL_CONV TextureLoad ( HTEXHEAP hTexHeap, 
								   HTEXTURE		 hTex, 
								   WHICH_MAP DestinationMap, 
								   PTEXTURESOURCE pSource);

PVROSERR 	CALL_CONV TextureCopy (	HTEXHEAP hTexHeap, 
									HTEXTURE hTex, 
									WHICH_MAP DestinationMap, 
									sgl_uint16		*pPixels);
PVROSERR 	CALL_CONV AutoMipmap ( HTEXHEAP hTexHeap, 
								   HTEXTURE		 hTex, 
								   WHICH_MAP DestinationMap, 
								   PTEXTURESOURCE pSource);
									
									
sgl_uint32 	CALL_CONV TextureFree (HTEXHEAP hTexHeap, HTEXTURE hTex);

#endif

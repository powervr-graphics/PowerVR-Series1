/******************************************************************************
 * Name         : texapi.h
 * Title        : Texture API
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
 * $Date: 1997/09/15 15:54:59 $
 * $Revision: 1.22 $
 * $Locker:  $
 * $Log: texapi.h,v $
 * Revision 1.22  1997/09/15  15:54:59  gdc
 * added a new element to texheap struct for counting textures on heap
 *
 * Revision 1.21  1997/08/18  16:45:55  sks
 * Added VertexFogControlWord for per vertex fogging.
 *
 * Revision 1.20  1997/06/09  17:17:42  sxy
 * added an API AutoMipmap.
 *
 * Revision 1.19  1997/06/05  14:20:22  sxy
 * added a field pPalette in TextureSource struct for palettised textures.
 *
 * Revision 1.18  1997/05/22  23:28:15  jop
 * Updated TEXHEAP structure to contain some detaiolls of the board
 *
 * Revision 1.17  1997/05/13  13:49:54  sxy
 * changed the definition of Pitch.
 *
 * Revision 1.16  1997/05/13  10:43:37  sxy
 * added a new API TextureCopy to copy preprocessed textures into memory.
 *
 * Revision 1.15  1997/05/12  13:43:11  sxy
 * Added a flag for colour key: TF_COLOURKEY.
 *
 * Revision 1.14  1997/05/01  16:28:14  mjg
 * Added the GetFreeMemory... functions.
 *
 * Revision 1.13  1997/04/30  19:32:29  mjg
 * Put the texture heap structure back.
 *
 * Revision 1.11  1997/04/30  14:38:26  mjg
 * Renamed FOURCC to PVRFOURCC, to stop compiler warnings.
 *
 * Revision 1.10  1997/04/25  20:50:53  mjg
 * Renamed all the HTEXTURES to PVRHANDLE to stop the HAL complaining.
 *
 * Revision 1.9  1997/04/25  15:46:00  mjg
 * Moved the TEXHEAP structure to tmalloc.h
 *
 * Revision 1.8  1997/04/24  13:29:15  jwh
 * Modified flags to match Direct draw stuff etc.
 *
 * Revision 1.7  1997/04/23  17:02:24  jop
 * added pfn's to make John happy then reformatted it to make him sad.
 *
 * Revision 1.6  1997/04/23  16:56:30  jop
 * defined texapi_if interface object. Actual implementation
 * prototypes are in texapip.h
 *
 * Revision 1.5  1997/04/23  16:07:01  sxy
 * some changes.
 *
 *
 *****************************************************************************/

#ifndef __TEXAPI_H__
#define __TEXAPI_H__

/*****************************************************************************
 D3D compatible flags (low 16 bits) 
*****************************************************************************/
#define TF_TRANSLUCENT	0x00000001	/* Not set = opaque */
#define TF_4CC			0x00000004	/* Four CC used etc */
#define TF_PALETTISED4	0x00000008	/* Is 4 bit palettised */
#define TF_PALETTISED8	0x00000020	/* Is 8 Bit palettised */
#define TF_RGB			0x00000040	/* Is RGB */
#define TF_YUV			0x00000200	/* Is YUV */

/*****************************************************************************
 Our internal flags -> upper 16 bit ONLY
*****************************************************************************/
#define TF_NONSQUARE	0x00010000	/* texture can be non-sqaure */
#define TF_CANMIPMAP	0x00020000	/* texture can be mip mapped */
#define TF_NONTWOSPOWER	0x00040000	/* Not set texture size power of two */
#define TF_HWSUPPORT 	0x00080000  /* for hardware support */
#define TF_OGLALPHA 	0x00100000  /* for alpha conversion */
#define TF_COLOURKEY 	0x00200000  /* set for using colour key */

typedef sgl_uint32 PVRFOURCC;

typedef struct
{
  sgl_uint32	Flags;
  PVRFOURCC		FourCC;
  sgl_uint32	BitDepth;
  sgl_uint32	KeyColour;
  
  union
  {
    struct
    {
      sgl_uint32	RedMask;
      sgl_uint32	GreenMask;
      sgl_uint32	BlueMask;
    } rgb;
    
    struct
    {
      sgl_uint32	YMask;
      sgl_uint32	UMask;
      sgl_uint32	VMask;
    } yuv;
    
  } rgbyuv;
  
  sgl_uint32	AlphaMask;
  
} TEXTUREFORMAT, *PTEXTUREFORMAT;

typedef struct
{
  PTEXTUREFORMAT	pFormat;
  sgl_uint32		Width;
  sgl_uint32		Height;
  sgl_uint32		MipMapped;	/* non zero if texture is to be mipmapped */
  
} TEXTURESPEC, *PTEXTURESPEC;

typedef struct
{
  PTEXTUREFORMAT	pFormat;
  int				Pitch;		/* 'stride' in bytes */
  void				*pPixels;	/* actual pixel data */
  void				*pPalette;	/* palette data in either 4444 or 555 formats */
  
} TEXTURESOURCE, *PTEXTURESOURCE;

typedef struct
{
  void *pPrivateData;
  sgl_uint32	TSPTextureControlWord;

} TEXTUREHANDLE, *HTEXTURE;

typedef struct
{
  void  		*pTextureMemory;
  void  		*pMemoryRoot;
  unsigned int 	TexParamSize;
  unsigned int 	TranslucentControlWord;
  unsigned int 	VertexFogControlWord;
  HDEVICE 		hDeviceID;
  DEVICE_TYPE	DeviceType;
  unsigned int  uTexCount;
	
} TEXTUREHEAP, *HTEXHEAP; /* handle to texture heap */


typedef enum
{
  PALETTE = -1,
  NON_MIPMAPED = 0,	/* Non mipmapped */
  MIPMAP_LEVEL_0,	/* 1x1 pixel map */
  MIPMAP_LEVEL_1,	/* 2x2 pixel map */
  MIPMAP_LEVEL_2, 	/* 4x4 pixel map */
  MIPMAP_LEVEL_3,	/* 8x8 pixel map */
  MIPMAP_LEVEL_4,	/* 16x16 pixel map */
  MIPMAP_LEVEL_5,	/* 32x32 pixel map */
  MIPMAP_LEVEL_6,	/* 64x64 pixel map */
  MIPMAP_LEVEL_7, 	/* 128x128 pixel map */
  MIPMAP_LEVEL_8,	/* 256x256 pixel map */
  
} WHICH_MAP;

#ifndef CALL_CONV
#error "CALL_CONV is undefined. You need to include sgl.h"
#endif

typedef struct TEXAPI_IF
{
	PVRHANDLE	hTexHeap;

	sgl_uint32 	(CALL_CONV *pfnTextureEnumerateFormats) 
					(	
						PVRHANDLE 		hTexHeap, 
						PTEXTUREFORMAT 	pFormatList
					);

	HTEXTURE  	(CALL_CONV *pfnTextureCreate) 			 
					(	
						PVRHANDLE		hTexHeap, 
						PTEXTURESPEC	pTextureSpec
					);

	PVROSERR 	(CALL_CONV *pfnTextureLoad) 
					(	
						PVRHANDLE 		hTexHeap, 
						HTEXTURE		hTex, 
						WHICH_MAP 		DestinationMap, 
						PTEXTURESOURCE	pSource
					);
	PVROSERR 	(CALL_CONV *pfnTextureCopy) 
					(	
						PVRHANDLE 		hTexHeap, 
						HTEXTURE		hTex, 
						WHICH_MAP 		DestinationMap, 
						sgl_uint16		*pPixels
					);
	PVROSERR 	(CALL_CONV	*pfnAutoMipmap)
					(	
						PVRHANDLE		hTexHeap, 
						HTEXTURE 		hTex, 
						WHICH_MAP 		DestinationMap, 
						PTEXTURESOURCE 	pSource
					);
	sgl_uint32	(CALL_CONV *pfnTextureFree) 
					(	
						PVRHANDLE 		hTexHeap, 
						HTEXTURE 		hTex
					);
					
	sgl_uint32 	(CALL_CONV *pfnTextureGetFreeMemory)
					(
						HTEXHEAP hTexHeap
					);

	void 		(CALL_CONV *pfnTextureGetFreeMemoryInfo)
					( 
						HTEXHEAP hTexHeap,
						sgl_texture_mem_info *pInfo 
					);

} TEXAPI_IF, *PTEXAPI_IF;

#endif

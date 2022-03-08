/******************************************************************************
 * Name         : texapi.c
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
 * $Date: 1997/11/13 15:10:45 $
 * $Revision: 1.52 $
 * $Locker:  $
 * $Log: texapi.c,v $
 * Revision 1.52  1997/11/13  15:10:45  erf
 * In ConvertUnknownTextureFormat() check alpha mask to see if translucent.
 * This is better than using a flag incase the AlphaMask is zero which
 * will cause an infinite loop the loop.
 *
 * Revision 1.51  1997/10/06  18:36:19  mjg
 * Replaced the synchronisation of texture memory call - used when a
 * load is attempted during a render - as PCX1 crashed without it. The line
 * had been removed in version 1.42.0.3 onwards.
 *
 * Revision 1.50  1997/09/24  10:29:34  sxy
 * Made conversion of 1555, 565, 555-ck and 565-ck into assembling.
 *
 * Revision 1.49  1997/09/22  13:47:15  sxy
 * Added tow cases for 4444-ck and 1555-ck in AutoMipmap().
 *
 * Revision 1.48  1997/09/15  15:54:11  gdc
 * put in a very hacky hack to reset texture heap ?!
 *
 * Revision 1.47  1997/09/11  15:12:14  sxy
 * turn off ReversedAlpha when we have ColorKey in AutoMipmap().
 *
 * Revision 1.46  1997/08/13  11:45:39  gdc
 * removed ^M's
 *
 * Revision 1.45  1997/08/12  13:23:14  sxy
 * Fixed a bug in ConvertUnknownTextureFormats().
 *
 * Revision 1.44  1997/08/08  16:13:04  sxy
 * 1. Added more check to force the destination texture format into the source textu format.
 * 2. Made a local copy of Palette[].
 *
 * Revision 1.42.0.3  1997/08/05  17:10:51  sxy
 * Made a mistake when set Mask_4444_555 for PCX2_003.
 *
 * Revision 1.42.0.2  1997/08/01  16:41:55  sxy
 * For PCX_003 turn off the alpha bit of 555 textures.
 *
 * Revision 1.42.0.1  1997/07/21  16:49:39  sxy
 * PCX2_003 changes: 1. added 1555 and 444 formats as hardware supportted.
 *                   2. all colour key textures go to 1555.
 *                   3. added AlphaMasks for 4444 or 1555 reversed alpha mask.
 *
 * Revision 1.42  1997/07/18  10:33:25  sxy
 * when texture source format is 555 or 4444 but doesn't match the destination format,
 * force the destination to be the source format.  It happens in Monster Truck.
 *
 * Revision 1.41  1997/07/15  11:03:23  mjg
 * Added a MIDAS5_003 clause to a switch statement.
 *
 * Revision 1.40  1997/07/14  19:40:44  mjg
 * Added PCX2_003 compile switch and MODULE_ID.
 *
 * Revision 1.39  1997/07/08  16:00:03  sxy
 * 1. optimised the code of WriteTexturePalettised.
 * 2. changed local functions to static.
 *
 * Revision 1.38  1997/07/08  11:10:16  sxy
 * changed macros PixelPairPalettised and FourPairsPalettised into a function
 * for performance reason.
 *
 * Revision 1.37  1997/06/24  16:07:11  sxy
 * added a call of TFree when fail to allocat memery for hTex in TextureCreate.
 *
 * Revision 1.36  1997/06/18  14:20:03  sxy
 * turn off reversed alpha for colour key.
 *
 * Revision 1.35  1997/06/17  17:54:07  sxy
 * fixed a bug in ConvertTextureFormats when the map size is 1.
 *
 * Revision 1.34  1997/06/17  16:33:32  sxy
 * switch off assembler in ConvertTextureFormats32.
 *
 * Revision 1.33  1997/06/17  09:08:44  sxy
 * tidy up the colour key and reversed alpha in AutoMipmap.
 *
 * Revision 1.32  1997/06/16  14:46:27  sxy
 * added the feature of reversed alpha into AutoMipmap.
 *
 * Revision 1.31  1997/06/12  16:48:59  sxy
 * fixed a bug in AutoMipmap for textures which the formats need to be converted.
 *
 * Revision 1.30  1997/06/12  14:03:16  sxy
 * fixed a bug that the
 * MapSize was wrong when convert texture formats.
 *
 * Revision 1.29  1997/06/11  16:36:30  sxy
 * changed AutoMipmap so it can take any texture formats.
 *
 * Revision 1.28  1997/06/10  16:48:50  sxy
 * changed AutoMipmap so it can generate Mipmap from lower levels (lower than
 * 32x32).
 *
 * Revision 1.27  1997/06/09  17:19:05  sxy
 * added an API AutoMipmap.
 *
 * Revision 1.26  1997/06/09  13:52:12  sxy
 * changed the Pitch value(in bytes) to texture source for palettised textures.
 *
 * Revision 1.25  1997/06/05  14:13:00  sxy
 * added functions for palettised textures.
 *
 * Revision 1.24  1997/05/28  14:41:59  mjg
 * Fixed a bug with unknown textures when the alpha left shift was 16.
 *
 * Revision 1.23  1997/05/27  17:58:16  jop
 * Added stuff to lock the cheduler when uploading PCX1 tetxurwees
 *
 * Revision 1.22  1997/05/22  23:27:42  jop
 * Added texture load scheduler serialisation
 *
 * Revision 1.21  1997/05/21  11:49:19  sxy
 * Downgraded asserts into warnings in TextureLoad.
 *
 * Revision 1.20  1997/05/16  11:16:38  sxy
 * changed the error message in TextureCreate.
 *
 * Revision 1.19  1997/05/14  14:48:50  sxy
 * put 8888 (ABGR) format into a separate path.
 *
 * Revision 1.18  1997/05/13  17:04:16  sxy
 * 1. Added a new API TextureCopy to copy preprocessed textures into memory.
 * 2. Changed the definition of Pitch value from pixels to bytes.
 *
 * Revision 1.17  1997/05/12  16:05:04  sxy
 * 1. changed the way mipmapped textures are loaded.
 * 2. added supporting for colour key.
 *
 * Revision 1.16  1997/05/06  14:04:37  jop
 * Changed the way textures are allocated - just one PVROSMalloc now!
 *
 * Revision 1.15  1997/05/06  10:49:58  jop
 * Downgraded an assert to a warning
 *
 * Revision 1.14  1997/05/02  07:42:44  erf
 * Removed UINT32's etc and replaced with sgl_uint32's etc.
 *
 * Revision 1.13  1997/04/30  19:21:55  mjg
 * Included tmalloc.c
 *
 * Revision 1.12  1997/04/30  16:16:15  sxy
 * Added new functions to support any texture formats.
 *
 * Revision 1.11  1997/04/29  15:08:18  erf
 * 1. Changed usage of TMalloc() and TFree().
 * 2. Changed free()'s to PVROSFree()'s.
 *
 * Revision 1.10  1997/04/25  17:38:48  mjg
 * Removed 'depthpalette' struct and altered TF_PALETTISED defines in
 * line with John's DirectDraw changes to texapi.h revision 1.8
 *
 * Revision 1.9  1997/04/25  15:45:14  mjg
 * Updated the TMalloc and TFree calls.
 *
 * Revision 1.8  1997/04/24  13:00:08  mjg
 * For now, commented out the PCX1 waiting for the Hardware to finish.
 *
 * Revision 1.7  1997/04/24  11:17:32  sxy
 * Added converting reversed alpha in 4444 and 8888.
 *
 * Revision 1.6  1997/04/23  17:12:25  jop
 * Removed brdsetup.h include and added texapip.h
 *
 * Revision 1.5  1997/04/23  17:04:50  sxy
 * Added assembler functions.
 *
 * Revision 1.4  1997/04/21  22:22:57  gdc
 * chnaged return values from some functions and removed tex heap assignment
 * to global tex heap
 *
 * Revision 1.3  1997/04/21  18:46:11  sxy
 * Using ghTexHeap for MemoryRoot and pSCBusBurstSpace.
 *
 *
 *****************************************************************************/

#define MODULE_ID MODID_TEXAPI

#include "sgl_defs.h"
#include "sgl_init.h"
#include "txmops.h"

#include "pvrosapi.h"
#include "tmalloc.h"
#include "texapi.h"
#include "texapip.h"

#include "memory.h"

typedef struct
{
  PTEXTURESPEC pTextureSpec;
  MNODE_BLOCK MemBlock;

} TPRIVATEDATA;	

/*
// Specify how much padding (in BYTES) is used to get better page alignment.
// PLUS specify the alignment offset IN sgl_uint16s. 
*/
#define ALIGNMENT_PADDING_U8s  (340)
#define ALIGNMENT_OFFSET_U16s  (170)

/*
// Define the Offsets for for the various levels in a MIP map. Note that
// there are currently two different sets: One for Texas 1, and the other
// for PCX1 and external File format.
//
// The offsets are expressed as 16bit pixel positions.
*/

static const sgl_uint32 TEX1MapOffset[]= { 0x0000,0x0001,0x0005,
									   0x0015,0x0055,0x0155,
									   0x0555,0x1555,0x5555};

/* External & PCX1 */
static const sgl_uint32 EXTMapOffset[]= { 0x0001,0x0002,0x0006,
									   0x0016,0x0056,0x0156,
									   0x0556,0x1556,0x5556};
/*
// Define one or the other as the "internal" format depending on
// the compile flag settings
*/
#if PCX1 || PCX2 || PCX2_003
#define  MAP_OFFSET EXTMapOffset
#else
#define  MAP_OFFSET TEX1MapOffset
#endif
				   			  		  
static INLINE void SynchroniseTexMemAccess (HTEXHEAP hTexHeap, sgl_bool bLock)
{
	switch (hTexHeap->DeviceType)
	{
		case MIDAS5:
		case MIDAS5_003:
		{
			break;
		}

		default:
		{
			if (bLock)
			{
				PVROSSchedulerLock (hTexHeap->hDeviceID);
			}
			else
			{
				PVROSSchedulerUnlock (hTexHeap->hDeviceID);
			}
			break;
		}
	}
}

/******************************************************************************
 * Function Name: TextureEnumerateFormats
 *
 * Inputs       : hTexHeap, pFormatList
 *
 * Outputs      : pFormatList
 *
 * Returns      : number of supported texture formats
 *
 * Description  : Returns a list of the texture formats currently supported by the texture API.
 *				  When a NULL ptr to the list is passed in, the number of supported texture 
 *				  formats is returned, allowing the client to malloc the appropriate space. 
 *  			  When a valid ptr is passed in, the funtion fills out the list. Zero is 
 *                returned on error.
 ******************************************************************************/
 sgl_uint32 CALL_CONV TextureEnumerateFormats (HTEXHEAP hTexHeap, PTEXTUREFORMAT pFormatList)
 {
	int i, nFormat;
	static TEXTUREFORMAT TFormatList[4];

	/* number of hardware supported formats */
	nFormat = 2;
	if(hTexHeap->DeviceType == MIDAS5_003)	 nFormat = 4;
   
   	if(pFormatList == NULL)
	{
		return (nFormat);
	}
	else 
	{	
		/* RGB 555 */
		TFormatList[0].Flags = TF_CANMIPMAP | TF_HWSUPPORT | TF_RGB;	   
		TFormatList[0].BitDepth = 	16;
		TFormatList[0].rgbyuv.rgb.RedMask =  	0x00007C00;
		TFormatList[0].rgbyuv.rgb.GreenMask =	0x000003E0;
		TFormatList[0].rgbyuv.rgb.BlueMask =	0x0000001F;
	
		/* ARGB 4444 */
		TFormatList[1].Flags = TF_CANMIPMAP | TF_HWSUPPORT | TF_RGB | TF_TRANSLUCENT | TF_OGLALPHA;
		TFormatList[1].BitDepth = 	16;
		TFormatList[1].rgbyuv.rgb.RedMask =  	0x00000F00;
		TFormatList[1].rgbyuv.rgb.GreenMask =	0x000000F0;
		TFormatList[1].rgbyuv.rgb.BlueMask =	0x0000000F;
		TFormatList[1].AlphaMask =				0x0000F000;
		
		/*  PCX2_003 supports 1555 and 444 */
		if(hTexHeap->DeviceType == MIDAS5_003)	
		{
			/* ARGB 1555 */
			TFormatList[2].Flags = TF_CANMIPMAP | TF_HWSUPPORT | TF_RGB | TF_TRANSLUCENT | TF_OGLALPHA;	   
			TFormatList[2].BitDepth = 	16;
			TFormatList[2].rgbyuv.rgb.RedMask =  	0x00007C00;
			TFormatList[2].rgbyuv.rgb.GreenMask =	0x000003E0;
			TFormatList[2].rgbyuv.rgb.BlueMask =	0x0000001F;
			TFormatList[2].AlphaMask =				0x00008000;

			/* RGB 444 */
			TFormatList[3].Flags = TF_CANMIPMAP | TF_HWSUPPORT | TF_RGB;
			TFormatList[3].BitDepth = 	16;
			TFormatList[3].rgbyuv.rgb.RedMask =  	0x00000F00;
			TFormatList[3].rgbyuv.rgb.GreenMask =	0x000000F0;
			TFormatList[3].rgbyuv.rgb.BlueMask =	0x0000000F;
		}

		for(i=0; i<nFormat; i++)
		{
			pFormatList[i] = TFormatList[i];
		}
	}
	return (sgl_no_err);
 } /* end of TextureEnumerateFormats */

/******************************************************************************
 * Function Name: TextureSize    INTERNAL ONLY
 *
 * Inputs       : pTxtureSpec
 *
 * Outputs      : None
 *
 * Returns      : Size of map in bytes
 *
 * Description  : Given pTxtureSpec, it computes the size in bytes
 *				  of data needed. It assumes that the input parameter is valid.
 *****************************************************************************/
static sgl_uint32 TextureSize(PTEXTURESPEC pTextureSpec)
{
	sgl_uint32 SpaceRequired;
	int n;

	/*work out the size of the of the texture to be allocated*/
	if(!(pTextureSpec->pFormat->Flags & TF_NONSQUARE))
	{
		if(pTextureSpec->MipMapped)
		{
			SpaceRequired = 0;
			for(n=pTextureSpec->Width; n>1; n=n>>1)
			{
				SpaceRequired += n*n;
			}
			SpaceRequired = (SpaceRequired + 2)<<1;
		}
		else
		{
			SpaceRequired=(pTextureSpec->Width*pTextureSpec->Width)<<1;
		}
		/*
		// if 8 instead of 16 bit...
		*/	
		if(pTextureSpec->pFormat->BitDepth == 8)
		{
			SpaceRequired>>=1;
		}	
	}
	else 
	{
		DPF ((DBG_MESSAGE, "TAPI: Nonsquare texture map\n"));
	}

	return SpaceRequired;
} /* end of TextureSize */

/******************************************************************************
 * Function Name: TextureCreate
 *
 * Inputs       : hTexHeap, pTextureSpec
 *
 * Outputs      : None
 *
 * Returns      : handle of the texture
 *
 * Description  : TextureCreate creates a unique 'handle' for a new texture. The texture
 * 				  handle corresponds to a piece (or pieces) of texture memory allocated for
 *				  the new texture; the client can then call 'TextureLoad' to upload data 
 *				  into the texture. The handle is a pointer to a set of unique data for 
 *				  the texture map. On error the function returns zero.
 *****************************************************************************/
HTEXTURE CALL_CONV TextureCreate (HTEXHEAP hTexHeap, PTEXTURESPEC pTextureSpec)
{
	sgl_uint32 SpaceRequired;
	MNODE_BLOCK MemBlock;
	HTEXTURE hTex;
	sgl_uint32 TextureAddress;
	sgl_uint32 BitDepth;
	TPRIVATEDATA *pTPD;
	int i, map_size;
	char *pDataBlock;
	
	ASSERT(hTexHeap != NULL);
	ASSERT(pTextureSpec != NULL);
	ASSERT(pTextureSpec->Width > 0);
	ASSERT(pTextureSpec->Height > 0);
	
	/* only 555 or 4444 are supported by hardware */
	if (!(pTextureSpec->pFormat->Flags & TF_HWSUPPORT))
	{
		// downgraded to a warning. What purpose dows this flag serve now? */
		DPF ((DBG_WARNING, "TF_HWSUPPORT flag not set in texture format"));
	}
	
	for(i=0; i<=8; i++)
	{
		if((pTextureSpec->Width >> i)& 1) break;
	}
	map_size = i + 1;
	
	/* work out the size of the of the texture to be allocated
	// If a MIP map, increase the size to improve page alignment. */
	SpaceRequired=TextureSize(pTextureSpec);
	if(pTextureSpec->MipMapped)
	{
		SpaceRequired+= ALIGNMENT_PADDING_U8s;
	}
	
	/* try and allocate the space in the texture ram. */
	TMalloc(SpaceRequired, hTexHeap, &MemBlock);
	if(MemBlock.Status==-1)
	{
		DPF ((DBG_MESSAGE, "texapi: sgl_err_texture_memory_full.\n"));
		return(NULL);
	}

	hTexHeap->uTexCount++;

	/* Reserve some data space */
	
	pDataBlock = PVROSMalloc(sizeof(TEXTUREHANDLE) + 
							 sizeof(TPRIVATEDATA) + 
							 sizeof(TEXTURESPEC) +
							 sizeof(TEXTUREFORMAT));

	if (pDataBlock == NULL)
	{
		PVROSPrintf("texapi: sgl_err_no_mem.\n");
		TFree(&MemBlock, hTexHeap);
		return(NULL);
	}

	/* construct the texture handle. */

	hTex = (HTEXTURE) pDataBlock;
	hTex->pPrivateData = (void *) (pDataBlock + sizeof (TEXTUREHANDLE));
	pTPD = (TPRIVATEDATA *) hTex->pPrivateData;
	pTPD->pTextureSpec = (void *) (pDataBlock + sizeof (TEXTUREHANDLE) + 
											    sizeof(TPRIVATEDATA));
	*pTPD->pTextureSpec = *pTextureSpec;

	pTPD->pTextureSpec->pFormat = (void *) (pDataBlock + sizeof (TEXTUREHANDLE) + 
										   sizeof(TPRIVATEDATA) + 
										   sizeof(TEXTURESPEC));
	*pTPD->pTextureSpec->pFormat = *pTextureSpec->pFormat;

	pTPD->MemBlock = MemBlock;
	
	/*calculate the texture address*/
	TextureAddress=MemBlock.MNode->MemoryAddress>>1; /* from bytes to 16Bit words*/
	if(MemBlock.Status==B_BANK)
		TextureAddress|=BIG_BANK;

	/* this is to get better page alignment for MIP maps */
	if(pTextureSpec->MipMapped)
		TextureAddress+= ALIGNMENT_OFFSET_U16s;
	
	/* pack up the control word. */
	BitDepth = pTextureSpec->pFormat->BitDepth;
	
	hTex->TSPTextureControlWord=TextureAddress;

	/*  PCX2_003 support 1555 and 444 formats, so set the flag MASK_4444_555
		**  by checking the Masks                                          */
	if(hTexHeap->DeviceType == MIDAS5_003)
	{
		if((pTextureSpec->pFormat->rgbyuv.rgb.RedMask ==	0x00000F00) &&
		   (pTextureSpec->pFormat->rgbyuv.rgb.GreenMask ==	0x000000F0) &&
		   (pTextureSpec->pFormat->rgbyuv.rgb.BlueMask ==	0x0000000F))
			hTex->TSPTextureControlWord |=MASK_4444_555;
	}
	else
	{
		if(pTextureSpec->pFormat->Flags & TF_TRANSLUCENT)
			hTex->TSPTextureControlWord |=MASK_4444_555;
	}

	hTex->TSPTextureControlWord |=((map_size-6)<<SHIFT_MAP_SIZE) & MASK_MAP_SIZE;
	
	if(BitDepth != 8)
		hTex->TSPTextureControlWord |=MASK_8_16_MAPS;

	if(pTextureSpec->MipMapped)
		hTex->TSPTextureControlWord |=MASK_MIP_MAPPED;
	return(hTex);
} /* end of TextureCreate */

/******************************************************************************
 * Function Name: TextureFree
 *
 * Inputs       : hTexHeap, hTex.
 *
 * Outputs      : None
 *
 * Returns      : handle of the texture
 *
 * Description  : Deallocate a texture's resources and invalidate the texture handle. 
 *				  Zero return indicates failure.
 *****************************************************************************/
sgl_uint32 CALL_CONV TextureFree (HTEXHEAP hTexHeap, HTEXTURE hTex)
{
	TPRIVATEDATA *pTPD;

	ASSERT(hTex != NULL);
	ASSERT(hTexHeap != NULL);
	
	pTPD = hTex->pPrivateData;
	
	ASSERT(pTPD != NULL);
	ASSERT(pTPD->pTextureSpec != NULL);
	
	TFree(&pTPD->MemBlock, hTexHeap);

	PVROSFree(hTex);


	hTexHeap->uTexCount--;

	if(!hTexHeap->uTexCount)
	{
		/* No textures loaded re-initialise the heap */
		DPF((DBG_WARNING,"Re-initialising the texture heap"));
		InitTextureMemory(4*1024*1024, hTexHeap);

		/* set up the texture memory for cache overflow and TSP params */
		SetupOverflowArea (FindTexHeap(hTexHeap->hDeviceID));
	}

	return(sgl_no_err);
	
} /* end of TextureFree */

/******************************************************************************
 *  The following functions write 8x8 pixels for different map levels.
 *				  
 *   Write 8 Pixels for the MapLevel with:
 *
 *   pPixels,              pPixels+2, 
 *   pPixels+4*MapWidth,   pPixels+4*MapWidth+2, 
 *   pPixels+4,            pPixels+6, 
 *   pPixels+4*MapWidth+4, pPixels+4*MapWidth+6. 
 * 
 *   Write 2 Pixels for:
 *
 *			  pPixels,              pPixels+MapWidth,
 *			  pPixels+1,            pPixels+MapWidth+1,
 *			  pPixels+2*MapWidth,   pPixels+3*MapWidth,
 *			  pPixels+2*MapWidth+1, pPixels+3*MapWidth+1. 
 *
 *****************************************************************************/
#if 0      		
	/*       c functions      */
	#define PixelPair(pDst, pSrc, DstIndex, TexWidth) \
    	IW (pDst, DstIndex, (((*(pSrc))<<16) | (*(pSrc+TexWidth))))
	#define FourPairs(pDst, pSrc, TexWidth) \
	PixelPair(pDst, pSrc,              0, TexWidth);\
	PixelPair(pDst, pSrc+1,            1, TexWidth);\
	PixelPair(pDst, pSrc+2*TexWidth,   2, TexWidth);\
	PixelPair(pDst, pSrc+2*TexWidth+1, 3, TexWidth)				
	
/******************************************************************************
 * Function Name: Write8x8Pixels   INTERNAL ONLY
 *
 * Inputs       : *pPixels, Pitch, *pAddress.
 *				  
 * Outputs      : None
 * Returns      : None
 * Globals Used : None
 *
 * Description  : Write 8x8 Pixels into texture memory for 8x8. 
 *
 *****************************************************************************/
static void Write8x8Pixels(sgl_uint16  *pPixels, 
				  	int			Pitch,
				  	sgl_uint32  *pAddress)
{
	FourPairs(pAddress, pPixels, 8);
	pAddress += Pitch >> 1;
	FourPairs(pAddress, pPixels+2, 8);
	pAddress += Pitch >> 1;
	FourPairs(pAddress, pPixels+32, 8);
	pAddress += Pitch >> 1;
	FourPairs(pAddress, pPixels+34, 8);
	pAddress += Pitch >> 1;
	FourPairs(pAddress, pPixels+4, 8);
	pAddress += Pitch >> 1;
	FourPairs(pAddress, pPixels+6, 8);
	pAddress += Pitch >> 1;
	FourPairs(pAddress, pPixels+36, 8);
	pAddress += Pitch >> 1;
	FourPairs(pAddress, pPixels+38, 8);
}

/******************************************************************************
 * Function Name: Write8x8PixelsFor16   INTERNAL ONLY
 *
 * Inputs       : *pPixels, Pitch, **pAddress.
 *				  
 * Outputs      : None
 * Returns      : None
 * Globals Used : None
 *
 * Description  : Write 8x8 Pixels into texture memory for 16x16. 
 *
 *****************************************************************************/
static void Write8x8PixelsFor16(	sgl_uint16  *pPixels, 
				  			int			  Pitch,
				  			sgl_uint32  **pAddress)
{
	FourPairs((*pAddress), pPixels, 16);
	(*pAddress) += 4;
	FourPairs((*pAddress), pPixels+2, 16);
	(*pAddress) += 4 + ((Pitch-16) >> 1);	
	FourPairs((*pAddress), pPixels+64, 16);
	(*pAddress) += 4;
	FourPairs((*pAddress), pPixels+66, 16);
	(*pAddress) += 4 + ((Pitch-16) >> 1);	
	FourPairs((*pAddress), pPixels+4, 16);
	(*pAddress) += 4;
	FourPairs((*pAddress), pPixels+6, 16);
	(*pAddress) += 4 + ((Pitch-16) >> 1);	
	FourPairs((*pAddress), pPixels+68, 16);
	(*pAddress) += 4;
	FourPairs((*pAddress), pPixels+70, 16);
	(*pAddress) += 4 + ((Pitch-16) >> 1);	
}

/******************************************************************************
 * Function Name: Write8x8PixelsFor32   INTERNAL ONLY
 *
 * Inputs       : *pPixels, Pitch, **pAddress.
 *				  
 * Outputs      : None
 * Returns      : None
 * Globals Used : None
 *
 * Description  : Write 8x8 Pixels into texture memory for 32x32. 
 *
 *****************************************************************************/
static void Write8x8PixelsFor32(	sgl_uint16  *pPixels, 
				  			int			  Pitch,
				  			sgl_uint32  **pAddress)
{
	FourPairs((*pAddress), pPixels, 32);
	(*pAddress) += 4;
	FourPairs((*pAddress), pPixels+2, 32);
	(*pAddress) += 4;	
	FourPairs((*pAddress), pPixels+128, 32);
	(*pAddress) += 4;
	FourPairs((*pAddress), pPixels+130, 32);
	(*pAddress) += 4 + ((Pitch-32) >> 1);	
	FourPairs((*pAddress), pPixels+4, 32);
	(*pAddress) += 4;
	FourPairs((*pAddress), pPixels+6, 32);
	(*pAddress) += 4;	
	FourPairs((*pAddress), pPixels+132, 32);
	(*pAddress) += 4;
	FourPairs((*pAddress), pPixels+134, 32);
	(*pAddress) += 4 + ((Pitch-32) >> 1);	
}

/******************************************************************************
 * Function Name: Write8x8PixelsFor64   INTERNAL ONLY
 *
 * Inputs       : *pPixels, Pitch, **pAddress.
 *				  
 * Outputs      : None
 * Returns      : None
 * Globals Used : None
 *
 * Description  : Write 8x8 Pixels into texture memory for 64x64. 
 *
 *****************************************************************************/

static void Write8x8PixelsFor64(	sgl_uint16  *pPixels, 
				  			int			  Pitch,
				  			sgl_uint32  **pAddress)
{
	FourPairs((*pAddress), pPixels, 64);
	(*pAddress) += 4;
	FourPairs((*pAddress), pPixels+2, 64);
	(*pAddress) += 4;	
	FourPairs((*pAddress), pPixels+256, 64);
	(*pAddress) += 4;
	FourPairs((*pAddress), pPixels+258, 64);
	(*pAddress) += 4;	
	FourPairs((*pAddress), pPixels+4, 64);
	(*pAddress) += 4;
	FourPairs((*pAddress), pPixels+6, 64);
	(*pAddress) += 4;	
	FourPairs((*pAddress), pPixels+260, 64);
	(*pAddress) += 4;
	FourPairs((*pAddress), pPixels+262, 64);
	(*pAddress) += 4 + ((Pitch-64) >> 1);	
}

/******************************************************************************
 * Function Name: Write8x8PixelsFor128   INTERNAL ONLY
 *
 * Inputs       : *pPixels, **pAddress.
 *				  
 * Outputs      : None
 * Returns      : None
 * Globals Used : None
 *
 * Description  : Write 8x8 Pixels into texture memory for 128x128. 
 *
 *****************************************************************************/
static void Write8x8PixelsFor128(sgl_uint16  *pPixels, 
				  		sgl_uint32  **pAddress)
{
	FourPairs((*pAddress), pPixels, 128);
	(*pAddress) += 4;
	FourPairs((*pAddress), pPixels+2, 128);
	(*pAddress) += 4;	
	FourPairs((*pAddress), pPixels+512, 128);
	(*pAddress) += 4;
	FourPairs((*pAddress), pPixels+514, 128);
	(*pAddress) += 4;	
	FourPairs((*pAddress), pPixels+4, 128);
	(*pAddress) += 4;
	FourPairs((*pAddress), pPixels+6, 128);
	(*pAddress) += 4;	
	FourPairs((*pAddress), pPixels+516, 128);
	(*pAddress) += 4;
	FourPairs((*pAddress), pPixels+518, 128);
	(*pAddress) += 4;	
}

/******************************************************************************
 * Function Name: Write8x8PixelsFor256   INTERNAL ONLY
 *
 * Inputs       : *pPixels, **pAddress.
 *				  
 * Outputs      : None
 * Returns      : None
 * Globals Used : None
 *
 * Description  : Write 8x8 Pixels into texture memory for 256. 
 *
 *****************************************************************************/
static void Write8x8PixelsFor256(sgl_uint16  *pPixels, 
				  		sgl_uint32  **pAddress)
{
	FourPairs((*pAddress), pPixels, 256);
	(*pAddress) += 4;
	FourPairs((*pAddress), pPixels+2, 256);
	(*pAddress) += 4;	
	FourPairs((*pAddress), pPixels+1024, 256);
	(*pAddress) += 4;
	FourPairs((*pAddress), pPixels+1026, 256);
	(*pAddress) += 4;	
	FourPairs((*pAddress), pPixels+4, 256);
	(*pAddress) += 4;
	FourPairs((*pAddress), pPixels+6, 256);
	(*pAddress) += 4;	
	FourPairs((*pAddress), pPixels+1028, 256);
	(*pAddress) += 4;
	FourPairs((*pAddress), pPixels+1030, 256);
	(*pAddress) += 4;	
}
	
#else
	/* assembler functions   */
	extern Write8x8PixelsFor256(void *pSrc, void *pDst, sgl_uint32 MaskRGB);
	extern Write8x8PixelsFor128(void *pSrc, void *pDst, sgl_uint32 MaskRGB);
	extern Write8x8PixelsFor64(void *pSrc, int Pitch, void *pDst, sgl_uint32 MaskRGB);
	extern Write8x8PixelsFor32(void *pSrc, int Pitch, void *pDst, sgl_uint32 MaskRGB);
	extern Write8x8PixelsFor16(void *pSrc, int Pitch, void *pDst, sgl_uint32 MaskRGB);
	extern Write8x8Pixels(void *pSrc, int Pitch, void *pDst, sgl_uint32 MaskRGB);

	extern Write8x8For256ReversedAlpha(void *pSrc, void *pDst, sgl_uint32 AlphaMasks);
	extern Write8x8For128ReversedAlpha(void *pSrc, void *pDst, sgl_uint32 AlphaMasks);
	extern Write8x8For64ReversedAlpha(void *pSrc, int Pitch, void *pDst, sgl_uint32 AlphaMasks);
	extern Write8x8For32ReversedAlpha(void *pSrc, int Pitch, void *pDst, sgl_uint32 AlphaMasks);
	extern Write8x8For16ReversedAlpha(void *pSrc, int Pitch, void *pDst, sgl_uint32 AlphaMasks);
	extern Write8x8ReversedAlpha(void *pSrc, int Pitch, void *pDst, sgl_uint32 AlphaMasks);
#endif

/******************************************************************************
 * Function Name: WriteTexture    INTERNAL ONLY
 *
 * Inputs       : DestAddress, pBusBurstSpace, MapLevel, Pitch, *pPixels, nReversedAlpha.
 *
 * Outputs      : None
 *
 * Returns      : None
 *
 * Description  : Writes one level texture map  directly into the texture
 *				  memory.
 *				  
 *****************************************************************************/
static void WriteTexture(	sgl_uint32		DestAddress,
					sgl_uint32		*pBusBurstSpace,
					int				MapLevel,
					int				Pitch,
					sgl_uint16		*pPixels,
					int				nReversedAlpha,
					sgl_uint32		AlphaMasks,
					sgl_uint32 		MaskRGB)
{ 
	int n16, n32, n64, n128;
	sgl_uint32 *pAddress;
	sgl_uint16 *pPixels16, *pPixels32, *pPixels64, *pPixels128;

	int nIndex32For64[4] = {0, 2048, 32, 2080};
	int nIndex16For64[4] = {0, 1024, 16, 1040};
	
	int nIndex64For128[4] = {0, 8192, 64, 8256};
	int nIndex32For128[4] = {0, 4096, 32, 4128};
	int nIndex16For128[4] = {0, 2048, 16, 2064};
	
	int nIndex128For256[4] = {0, 32768, 128, 32896};
	int nIndex64For256[4] =  {0, 16384,  64, 16448};
	int nIndex32For256[4] =  {0,  8192,  32,  8224};
	int nIndex16For256[4] =  {0,  4096,  16,  4112};

	pAddress = pBusBurstSpace + (DestAddress>>1);
	switch(MapLevel)
	{
		case  MIPMAP_LEVEL_0:
		{
			/* 1x1 */
			sgl_uint32 Value;
			
			#if PCX2 || PCX2_003
				Value = 0;
			#else
				Value = IR( pAddress, 0);
			#endif

			if(DestAddress & 1)
			{
				Value=(Value & 0xFFFF0000ul) | pPixels[0];
			}
			else
			{
				Value=(Value & 0xFFFFul) | (pPixels[0]<<16);
			}
			if(!nReversedAlpha)
			{
				IW( pAddress, 0, (Value & MaskRGB));
			}
			else		/* reversed alpha   */
			{
				IW( pAddress, 0, (Value ^ AlphaMasks));
			}
			break;
		}
		case  MIPMAP_LEVEL_1:
		{
			/* 2x2 */
			if(!nReversedAlpha)
			{
				IW( pAddress, 0, (((pPixels[0] << 16) | pPixels[2]) & MaskRGB));
				pAddress += (Pitch>>1);
				IW( pAddress, 0, (((pPixels[1] << 16) | pPixels[3]) & MaskRGB));
			}
			else		/* reversed alpha   */
			{
				IW( pAddress, 0, (((pPixels[0] << 16) | pPixels[2]) ^ AlphaMasks));
				pAddress += (Pitch>>1);
				IW( pAddress, 0, (((pPixels[1] << 16) | pPixels[3]) ^ AlphaMasks));
			}
			break;
		}
		case  MIPMAP_LEVEL_2:
		{
			/* 4x4 */
			if(!nReversedAlpha)
			{
				IW( pAddress, 0, (((pPixels[0] << 16) | pPixels[4]) & MaskRGB));
				IW( pAddress, 1, (((pPixels[1] << 16) | pPixels[5]) & MaskRGB));
				pAddress += (Pitch>>1);
				IW( pAddress, 0, (((pPixels[8] << 16) | pPixels[12]) & MaskRGB));
				IW( pAddress, 1, (((pPixels[9] << 16) | pPixels[13]) & MaskRGB));
				pAddress += (Pitch>>1);
				IW( pAddress, 0, (((pPixels[2] << 16) | pPixels[6]) & MaskRGB));
				IW( pAddress, 1, (((pPixels[3] << 16) | pPixels[7]) & MaskRGB));
				pAddress += (Pitch>>1);
		 		IW( pAddress, 0, (((pPixels[10] << 16) | pPixels[14]) & MaskRGB));
				IW( pAddress, 1, (((pPixels[11] << 16) | pPixels[15]) & MaskRGB));
			}
			else		/* reversed alpha   */
			{
				IW( pAddress, 0, (((pPixels[0] << 16) | pPixels[4]) ^ AlphaMasks));
				IW( pAddress, 1, (((pPixels[1] << 16) | pPixels[5]) ^ AlphaMasks));
				pAddress += (Pitch>>1);
				IW( pAddress, 0, (((pPixels[8] << 16) | pPixels[12]) ^ AlphaMasks));
				IW( pAddress, 1, (((pPixels[9] << 16) | pPixels[13]) ^ AlphaMasks));
				pAddress += (Pitch>>1);
				IW( pAddress, 0, (((pPixels[2] << 16) | pPixels[6]) ^ AlphaMasks));
				IW( pAddress, 1, (((pPixels[3] << 16) | pPixels[7]) ^ AlphaMasks));
				pAddress += (Pitch>>1);
		 		IW( pAddress, 0, (((pPixels[10] << 16) | pPixels[14]) ^ AlphaMasks));
				IW( pAddress, 1, (((pPixels[11] << 16) | pPixels[15]) ^ AlphaMasks));
			}
			break;
		}
	    case  MIPMAP_LEVEL_3:
		{	
			/* 8x8 */
			if(!nReversedAlpha)
			{
				Write8x8Pixels(pPixels, Pitch, pAddress, MaskRGB);
			}
			else		/* reversed alpha   */
			{
				Write8x8ReversedAlpha(pPixels, Pitch, pAddress, AlphaMasks);
			}
			break;
		}
		case  MIPMAP_LEVEL_4:
		{	
			/* 16x16 */
			/*  Write one quarter of the pixels for each level with:
			//  pPixels,      			pPixels+HalfLevelWidth*MapWidth,
			//  pPixels+HalfLevelWidth, pPixels+HalfLevelWidth*MapWidth+HalfLevelWidth.
			*/
			if(!nReversedAlpha)
			{
				Write8x8PixelsFor16(pPixels, Pitch, &pAddress, MaskRGB);
				Write8x8PixelsFor16(pPixels+128, Pitch, &pAddress, MaskRGB);
				Write8x8PixelsFor16(pPixels+8, Pitch, &pAddress, MaskRGB);
				Write8x8PixelsFor16(pPixels+136, Pitch, &pAddress, MaskRGB);
			}
			else		/* reversed alpha   */
			{
				Write8x8For16ReversedAlpha(pPixels, Pitch, &pAddress, AlphaMasks);
				Write8x8For16ReversedAlpha(pPixels+128, Pitch, &pAddress, AlphaMasks);
				Write8x8For16ReversedAlpha(pPixels+8, Pitch, &pAddress, AlphaMasks);
				Write8x8For16ReversedAlpha(pPixels+136, Pitch, &pAddress, AlphaMasks);
			}
			break;
		}
		case  MIPMAP_LEVEL_5:
		{	
			/* 32x32 */
			if(!nReversedAlpha)
			{
				Write8x8PixelsFor32(pPixels,     Pitch, &pAddress, MaskRGB);
				Write8x8PixelsFor32(pPixels+256, Pitch, &pAddress, MaskRGB);
				Write8x8PixelsFor32(pPixels+8,   Pitch, &pAddress, MaskRGB);
				Write8x8PixelsFor32(pPixels+264, Pitch, &pAddress, MaskRGB);
			
				Write8x8PixelsFor32(pPixels+512, Pitch, &pAddress, MaskRGB);
				Write8x8PixelsFor32(pPixels+768, Pitch, &pAddress, MaskRGB);
				Write8x8PixelsFor32(pPixels+520, Pitch, &pAddress, MaskRGB);
				Write8x8PixelsFor32(pPixels+776, Pitch, &pAddress, MaskRGB);
			
				Write8x8PixelsFor32(pPixels+16,  Pitch, &pAddress, MaskRGB);
				Write8x8PixelsFor32(pPixels+272, Pitch, &pAddress, MaskRGB);
				Write8x8PixelsFor32(pPixels+24,  Pitch, &pAddress, MaskRGB);
				Write8x8PixelsFor32(pPixels+280, Pitch, &pAddress, MaskRGB);
			
				Write8x8PixelsFor32(pPixels+528, Pitch, &pAddress, MaskRGB);
				Write8x8PixelsFor32(pPixels+784, Pitch, &pAddress, MaskRGB);
				Write8x8PixelsFor32(pPixels+536, Pitch, &pAddress, MaskRGB);
				Write8x8PixelsFor32(pPixels+792, Pitch, &pAddress, MaskRGB);
			}
			else		/* reversed alpha   */
			{
				Write8x8For32ReversedAlpha(pPixels,     Pitch, &pAddress, AlphaMasks);
				Write8x8For32ReversedAlpha(pPixels+256, Pitch, &pAddress, AlphaMasks);
				Write8x8For32ReversedAlpha(pPixels+8,   Pitch, &pAddress, AlphaMasks);
				Write8x8For32ReversedAlpha(pPixels+264, Pitch, &pAddress, AlphaMasks);
			
				Write8x8For32ReversedAlpha(pPixels+512, Pitch, &pAddress, AlphaMasks);
				Write8x8For32ReversedAlpha(pPixels+768, Pitch, &pAddress, AlphaMasks);
				Write8x8For32ReversedAlpha(pPixels+520, Pitch, &pAddress, AlphaMasks);
				Write8x8For32ReversedAlpha(pPixels+776, Pitch, &pAddress, AlphaMasks);
			
				Write8x8For32ReversedAlpha(pPixels+16,  Pitch, &pAddress, AlphaMasks);
				Write8x8For32ReversedAlpha(pPixels+272, Pitch, &pAddress, AlphaMasks);
				Write8x8For32ReversedAlpha(pPixels+24,  Pitch, &pAddress, AlphaMasks);
				Write8x8For32ReversedAlpha(pPixels+280, Pitch, &pAddress, AlphaMasks);
			
				Write8x8For32ReversedAlpha(pPixels+528, Pitch, &pAddress, AlphaMasks);
				Write8x8For32ReversedAlpha(pPixels+784, Pitch, &pAddress, AlphaMasks);
				Write8x8For32ReversedAlpha(pPixels+536, Pitch, &pAddress, AlphaMasks);
				Write8x8For32ReversedAlpha(pPixels+792, Pitch, &pAddress, AlphaMasks);
			}
			break;
		}
		case  MIPMAP_LEVEL_6:
		{
			/* 64x64  */
			for(n32=0; n32<4; n32++)
			{
				/* 32x32 for 64x64 */
				pPixels32 = pPixels + nIndex32For64[n32];
				for(n16=0; n16<4; n16++)
				{
					/* 16x16 for 64x64 */
					pPixels16 = pPixels32 + nIndex16For64[n16];
					if(!nReversedAlpha)
					{
						Write8x8PixelsFor64(pPixels16, Pitch, &pAddress, MaskRGB);
						Write8x8PixelsFor64(pPixels16+512, Pitch, &pAddress, MaskRGB);
						Write8x8PixelsFor64(pPixels16+8, Pitch, &pAddress, MaskRGB);
						Write8x8PixelsFor64(pPixels16+520, Pitch, &pAddress, MaskRGB);
					}
					else		/* reversed alpha   */
					{
						Write8x8For64ReversedAlpha(pPixels16, Pitch, &pAddress, AlphaMasks);
						Write8x8For64ReversedAlpha(pPixels16+512, Pitch, &pAddress, AlphaMasks);
						Write8x8For64ReversedAlpha(pPixels16+8, Pitch, &pAddress, AlphaMasks);
						Write8x8For64ReversedAlpha(pPixels16+520, Pitch, &pAddress, AlphaMasks);
					}
				}
			}
			break;
		}
		case  MIPMAP_LEVEL_7:
		{
			/* 128x128 */
			for(n64=0; n64<4; n64++)
			{
				/* 64x64 for 128x128 */
				pPixels64 = pPixels + nIndex64For128[n64];
				for(n32=0; n32<4; n32++)
				{
					/* 32x32 for 128x128 */
					pPixels32 = pPixels64 + nIndex32For128[n32];
					for(n16=0; n16<4; n16++)
					{
						/* 16x16 for 128x128 */
						pPixels16 = pPixels32 + nIndex16For128[n16];	
						
						if(!nReversedAlpha)
						{
							Write8x8PixelsFor128(pPixels16, &pAddress, MaskRGB);
							Write8x8PixelsFor128(pPixels16+1024, &pAddress, MaskRGB);
	    					pAddress += (Pitch-128) >> 1;
							Write8x8PixelsFor128(pPixels16+8, &pAddress, MaskRGB);
							Write8x8PixelsFor128(pPixels16+1032, &pAddress, MaskRGB);
							pAddress += (Pitch-128) >> 1;
						}
						else		/* reversed alpha   */
						{
							Write8x8For128ReversedAlpha(pPixels16, &pAddress, AlphaMasks);
							Write8x8For128ReversedAlpha(pPixels16+1024, &pAddress, AlphaMasks);
	    					pAddress += (Pitch-128) >> 1;
							Write8x8For128ReversedAlpha(pPixels16+8, &pAddress, AlphaMasks);
							Write8x8For128ReversedAlpha(pPixels16+1032, &pAddress, AlphaMasks);
							pAddress += (Pitch-128) >> 1;
						}
					}
				}
			}
			break;
		}		
		case  MIPMAP_LEVEL_8:
		{
			/* 256x256 */
			for(n128=0; n128<4; n128++)
			{
				/* 128x128 for 256x256 */
				pPixels128 = pPixels + nIndex128For256[n128];
				for(n64=0; n64<4; n64++)
				{
					/* 64x64 for 256x256 */
					pPixels64 = pPixels128 + nIndex64For256[n64];
					for(n32=0; n32<4; n32++)
					{
						/* 32x32 for 256x256 */
						pPixels32 = pPixels64 + nIndex32For256[n32];
						for(n16=0; n16<4; n16++)
						{
							/* 16x16 for 256x256 */
							pPixels16 = pPixels32 + nIndex16For256[n16];	
						
							if(!nReversedAlpha)		
							{
								Write8x8PixelsFor256(pPixels16, &pAddress, MaskRGB);
								Write8x8PixelsFor256(pPixels16+2048, &pAddress, MaskRGB);
								Write8x8PixelsFor256(pPixels16+8, &pAddress, MaskRGB);
								Write8x8PixelsFor256(pPixels16+2056, &pAddress, MaskRGB);
							}
							else		/* reversed alpha   */
							{
								Write8x8For256ReversedAlpha(pPixels16, &pAddress, AlphaMasks);
								Write8x8For256ReversedAlpha(pPixels16+2048, &pAddress, AlphaMasks);
								Write8x8For256ReversedAlpha(pPixels16+8, &pAddress, AlphaMasks);
								Write8x8For256ReversedAlpha(pPixels16+2056, &pAddress, AlphaMasks);
							}
	    					pAddress += (Pitch-256) >> 1;
						}
					}
				}
			}
			break;
		}
	}
}	/* end of WriteTexture */
#if 1
	/*       c functions      */
static void FourPairsPalettised(sgl_uint32 *pDst, sgl_uint8 *pSrc, int TexWidth, 
						sgl_uint16 *pPalette, sgl_uint8 *pSrc2) 
{
	IW (pDst, 0, ( ( pPalette[*(pSrc)]<<16) | pPalette[*(pSrc2)] ));
	IW (pDst, 1, ( ( pPalette[*(pSrc+1)]<<16) | pPalette[*(pSrc2+1)] ));
	IW (pDst, 2, ( ( pPalette[*(pSrc+2*TexWidth)]<<16) | pPalette[*(pSrc2+2*TexWidth)] ));
	IW (pDst, 3, ( ( pPalette[*(pSrc+2*TexWidth+1)]<<16) | pPalette[*(pSrc2+2*TexWidth+1)] ));
		
}		

/******************************************************************************
 * Function Name: Write8x8PixelsPalettised   INTERNAL ONLY
 *
 * Inputs       : *pPixels, Pitch, *pAddress.
 *				  
 * Outputs      : None
 * Returns      : None
 * Globals Used : None
 *
 * Description  : Write 8x8 Pixels into texture memory for 8x8. 
 *
 *****************************************************************************/
static void Write8x8PixelsPalettised(sgl_uint8  *pPixels, 
				  	int			Pitch,
				  	sgl_uint32  **pAddress,
					sgl_uint16	*pPalette,
					sgl_uint8  *pPixels2)
{
	int Pitch4 = Pitch << 2;
	FourPairsPalettised((*pAddress), pPixels, Pitch, pPalette, pPixels2);
	FourPairsPalettised(((*pAddress)+4), pPixels+2, Pitch, pPalette, pPixels2+2);
	FourPairsPalettised(((*pAddress)+8), pPixels+Pitch4, Pitch, pPalette, pPixels2+Pitch4);
	FourPairsPalettised(((*pAddress)+12), pPixels+Pitch4+2, Pitch, pPalette, pPixels2+Pitch4+2);
	FourPairsPalettised(((*pAddress)+16), pPixels+4, Pitch, pPalette, pPixels2+4);
	FourPairsPalettised(((*pAddress)+20), pPixels+6, Pitch, pPalette, pPixels2+6);
	FourPairsPalettised(((*pAddress)+24), pPixels+Pitch4+4, Pitch, pPalette, pPixels2+Pitch4+4);
	FourPairsPalettised(((*pAddress)+28), pPixels+Pitch4+6, Pitch, pPalette, pPixels2+Pitch4+6);
	(*pAddress) += 32;
}

#else
	/* assembler functions   */
   	extern Write8x8PixelsPalettised(void *pSrc, int Pitch, void *pDst, 
   									void *pPalette, void *pSrc2);
#endif	

/******************************************************************************
 * Function Name: WriteTexturePalettised    INTERNAL ONLY
 *
 * Inputs       : DestAddress, pBusBurstSpace, MapLevel, Pitch, *pPixels, *pPalette.
 *
 * Outputs      : None
 *
 * Returns      : None
 *
 * Description  : Writes one level palettised texture map  directly into the texture
 *				  memory.
 *				  
 *****************************************************************************/
 static void WriteTexturePalettised(	sgl_uint32		DestAddress,
					sgl_uint32		*pBusBurstSpace,
					int				MapLevel,
					int				Pitch,	/*  the Pitch value of the source */
					sgl_uint8		*pPixels,
					sgl_uint16		*pPalette)
{ 
	int n16, n32, n64, n128, num16, num32, num64, num128;
	sgl_uint32 *pAddress;
	sgl_uint8 *pPixels16, *pPixels32, *pPixels64, *pPixels128;

	int nIndex128[4] = {0, 0, 128, 0};
	int nIndex64[4] =  {0, 0,  64, 0};
	int nIndex32[4] =  {0, 0,  32, 0};
	int nIndex16[4] =  {0, 0,  16, 0};

	pAddress = pBusBurstSpace + (DestAddress>>1);
	switch(MapLevel)
	{
		case  MIPMAP_LEVEL_0:
		{
			/* 1x1 */
			sgl_uint32 Value;
			
			#if PCX2 || PCX2_003
				Value = 0;
			#else
				Value = IR( pAddress, 0);
			#endif

			if(DestAddress & 1)
			{
				Value=(Value & 0xFFFF0000ul) | pPalette[pPixels[0]];
			}
			else
			{
				Value=(Value & 0xFFFFul) | (pPalette[pPixels[0]]<<16);
			}

			IW( pAddress, 0, Value);
			break;
		}
		case  MIPMAP_LEVEL_1:
		{
			/* 2x2 */
			IW( pAddress, 0, ((pPalette[pPixels[0]] << 16) | pPalette[pPixels[Pitch]]));
			pAddress += 1;
			IW( pAddress, 0, ((pPalette[pPixels[1]] << 16) | pPalette[pPixels[Pitch+1]]));
			break;
		}
		case  MIPMAP_LEVEL_2:
		{
			/* 4x4 */
			int Pitch2 = Pitch << 1;
			int Pitch3 = Pitch2 + Pitch;
			IW( pAddress, 0, ((pPalette[pPixels[0]] << 16) | pPalette[pPixels[Pitch]]));
			IW( pAddress, 1, ((pPalette[pPixels[1]] << 16) | pPalette[pPixels[Pitch+1]]));
			pAddress += 2;
			IW( pAddress, 0, ((pPalette[pPixels[Pitch2]] << 16) | pPalette[pPixels[Pitch3]]));
			IW( pAddress, 1, ((pPalette[pPixels[Pitch2+1]] << 16) | pPalette[pPixels[Pitch3+1]]));
			pAddress += 2;
			IW( pAddress, 0, ((pPalette[pPixels[2]] << 16) | pPalette[pPixels[Pitch+2]]));
			IW( pAddress, 1, ((pPalette[pPixels[3]] << 16) | pPalette[pPixels[Pitch+3]]));
			pAddress += 2;
	 		IW( pAddress, 0, ((pPalette[pPixels[Pitch2+2]] << 16) | pPalette[pPixels[Pitch3+2]]));
			IW( pAddress, 1, ((pPalette[pPixels[Pitch2+3]] << 16) | pPalette[pPixels[Pitch3+3]]));
			break;
		}
	    case  MIPMAP_LEVEL_3:
		{	
			/* 8x8 */
			Write8x8PixelsPalettised(pPixels, Pitch, &pAddress, pPalette, pPixels+Pitch);
			break;
		}
		case  	MIPMAP_LEVEL_4:
		case	MIPMAP_LEVEL_5:
		case	MIPMAP_LEVEL_6:
		case	MIPMAP_LEVEL_7:
		case	MIPMAP_LEVEL_8:
		{
			int Pitch8 = Pitch << 3;
			nIndex16[1] = Pitch << 4;
			nIndex16[3] = nIndex16[1] + 16;
			nIndex32[1] = Pitch << 5;
			nIndex32[3] = nIndex32[1] + 32;
			nIndex64[1] = Pitch << 6;
			nIndex64[3] = nIndex64[1] + 64;
			nIndex128[1] = Pitch << 7;
			nIndex128[3] = nIndex128[1] + 128;
			
			num128 = num64 = num32 = num16 = 4;
		  
		  	if(MapLevel < MIPMAP_LEVEL_8) 
			{
				num128 = 1;
				if(MapLevel < MIPMAP_LEVEL_7) 
				{
					num64 = 1;
					if(MapLevel < MIPMAP_LEVEL_6) 
					{
						num32 = 1;
						if(MapLevel < MIPMAP_LEVEL_5) 
						{
							num16 = 1;
						}
					}
				}
			}
			
			for(n128=0; n128<num128; n128++)
			{
				/* write 128x128 pixels */
				pPixels128 = pPixels + nIndex128[n128];		
				for(n64=0; n64<num64; n64++)
				{
					/* write 64x64 pixels */
					pPixels64 = pPixels128 + nIndex64[n64];
					for(n32=0; n32<num32; n32++)
					{
						/* write 32x32 pixels */
						pPixels32 = pPixels64 + nIndex32[n32];
						for(n16=0; n16<num16; n16++)
						{
							/* write 16x16 pixels */
							pPixels16 = pPixels32 + nIndex16[n16];	
						
							Write8x8PixelsPalettised(pPixels16, Pitch, &pAddress, pPalette,
												 pPixels16+Pitch);
							Write8x8PixelsPalettised(pPixels16+Pitch8, Pitch, &pAddress, pPalette,
												 pPixels16+Pitch+Pitch8);
							Write8x8PixelsPalettised(pPixels16+8, Pitch, &pAddress, pPalette,
												 pPixels16+Pitch+8);
							Write8x8PixelsPalettised(pPixels16+Pitch8+8, Pitch, &pAddress, pPalette,
												 pPixels16+Pitch+Pitch8+8);
						}
					}
				}
			}
			break;
		}
	}
}	/* end of WriteTexturePalettised */		

/******************************************************************************
 * Function Name: WriteTextureToMem    INTERNAL ONLY
 *
 * Inputs       : TextureAddress, MapSize, MapLevel, nReversedAlpha 
 *                Pitch, *pPixels.
 *
 * Outputs      : None
 *
 * Returns      : None
 *
 * Description  : Takes the supplied map, and writes it directly into the texture
 *				  memory.
 *				  
 *****************************************************************************/
static void WriteTextureToMem(	sgl_uint32		TextureAddress,
						sgl_uint32		*pBusBurstSpace,
						int				MapSize,
						int				MapLevel,
						int				Pitch,
						void			*pPixels,
						int				nReversedAlpha,
						int 			nPalettised,
						sgl_uint16		*pPalette,
						sgl_uint32		AlphaMasks,
						sgl_uint32 		MaskRGB)
{ 
	ASSERT(MapLevel >= NON_MIPMAPED);
	ASSERT(MapLevel <= MIPMAP_LEVEL_8);
	ASSERT(!(TextureAddress & 0x1));

	if(!MapLevel)
	{
		/* Non MipMapped texture, write directly into memory. */
	   	if(nPalettised)
		{
		 	/*  write translucent textures  */
		 	WriteTexturePalettised(TextureAddress, pBusBurstSpace, MapSize, Pitch,
									(sgl_uint8 *)pPixels, pPalette);
		}
		else
		{	
		 	/*  write translucent textures  */
		 	WriteTexture(TextureAddress, pBusBurstSpace, MapSize, Pitch, 
		 						(sgl_uint16 *)pPixels, nReversedAlpha, AlphaMasks, MaskRGB);
		}
	}
	else
	{
		/* MipMapped texture, write each level into memory in alternate banks.	*/
		sgl_uint32		DestAddress;

		if(!(MapLevel & 1))
		{
			/* Swap Texture Banks so that we write MIP map levels in alternate banks. */
			TextureAddress ^= BIG_BANK;
		}
		DestAddress = TextureAddress + MAP_OFFSET[MapLevel-1];

		if(nPalettised)
		{
		 	/*  write translucent textures  */
		 	WriteTexturePalettised(DestAddress, pBusBurstSpace, MapLevel, Pitch,
									(sgl_uint8 *)pPixels, pPalette);
		}
		else
		{
		 	/*  write translucent textures  */
		 	WriteTexture(DestAddress, pBusBurstSpace, MapLevel, Pitch, 
									(sgl_uint16 *)pPixels, nReversedAlpha, AlphaMasks, MaskRGB);
		}
	}
}  /* end of WriteTextureToMem */	

/* assembler functions   */
extern Pack888To555 (void *pSrc, void *pDst, int n);
extern Pack8888To4444 (void *pSrc, void *pDst, int n);
extern Pack888BGRTo555 (void *pSrc, void *pDst, int n);
extern Pack8888BGRTo4444 (void *pSrc, void *pDst, int n);
extern Pack565To555 (void *pSrc, void *pDst, int n);
extern Pack1555To4444 (void *pSrc, void *pDst, int n);
extern Pack565To4444WithColourKey (void *pSrc, void *pDst, int n, int Key);
extern Pack555To4444WithColourKey (void *pSrc, void *pDst, int n, int Key);

/******************************************************************************
 * Function Name: Convert555WithColourKey   INTERNAL ONLY
 *
 * Inputs       : nMapSize, pSrcPixel, nColourKey, KeyColour, nTrans1555.
 *				  
 * Outputs      : pDestPixel
 * Returns      : None
 * Globals Used : None
 *
 * Description  : Converts 555 texture format with colour key into 4444. 
 *
 *****************************************************************************/
static void Convert555WithColourKey(int nMapSize,  
							sgl_uint16 **pDestPixel, sgl_uint16 *pSrcPixel,
							int nColourKey, sgl_uint32 KeyColour, int nTrans1555) 
{
	int i;
	sgl_uint16 *pPixels;
	
	pPixels = *pDestPixel;

	if(!nTrans1555)
	{
		#if 0
			for(i=0; i<nMapSize; i++)
			{
				*pPixels =  (((*pSrcPixel) >> 3) & 0x0F00) |  /*R*/
							(((*pSrcPixel) >> 2) & 0x00F0) |  /*G*/
							(((*pSrcPixel) >> 1) & 0x000F);   /*B*/
				if(*pSrcPixel == KeyColour)
				{
					*pPixels |= 0xF000;
				}							
				pPixels++; pSrcPixel++; 							
			}
		#else		/* assembler function   */
			if(nMapSize == 1)
			{
				*pPixels =  (((*pSrcPixel) >> 3) & 0x0F00) |  /*R*/
							(((*pSrcPixel) >> 2) & 0x00F0) |  /*G*/
							(((*pSrcPixel) >> 1) & 0x000F);   /*B*/
				if(*pSrcPixel == KeyColour)
				{
					*pPixels |= 0xF000;
				}							
			}
			else
				Pack555To4444WithColourKey (pSrcPixel, pPixels, nMapSize, KeyColour);
		#endif
	}
	else
	{	/*  PCX2_003 support 1555 format for colour key */
		for(i=0; i<nMapSize; i++)
		{
			*pPixels =  *pSrcPixel; 
			if(*pSrcPixel == KeyColour)
			{
				*pPixels |= 0x8000;
			}	
			pPixels++; pSrcPixel++; 							
		}
	}
}	
		
/******************************************************************************
 * Function Name: ConvertTextureFormats16   INTERNAL ONLY
 *
 * Inputs       : nMapSize, nEnum, pSrcPixel, nColourKey, KeyColour, nTrans1555.
 *				  
 * Outputs      : pDestPixel
 * Returns      : None
 * Globals Used : None
 *
 * Description  : Converts 16 bit texture formats. 
 *
 *****************************************************************************/
static void ConvertTextureFormats16(int nMapSize, int nEnum, 
							sgl_uint16 **pDestPixel, sgl_uint16 *pSrcPixel,
							int nColourKey, sgl_uint32 KeyColour, int nTrans1555) 
{
	int i;
	sgl_uint16 *pPixels;
	
	pPixels = *pDestPixel;
		
	switch(nEnum)
	{
	   	case 3:
		{
			/* 1555 to 4444 */
			#if 0
			for(i=0; i<nMapSize; i++)
			{
				*pPixels =  (((*pSrcPixel) >> 3) & 0x0F00) |  /*R*/
							(((*pSrcPixel) >> 2) & 0x00F0) |  /*G*/
							(((*pSrcPixel) >> 1) & 0x000F);   /*B*/
				if((*pSrcPixel) & 0x8000)
				{
					*pPixels |= 0xF000;
				}							
				pPixels++; pSrcPixel++; 							
			}
			#else		/* assembler function   */
				if(nMapSize == 1)
				{
					*pPixels =  (((*pSrcPixel) >> 3) & 0x0F00) |  /*R*/
								(((*pSrcPixel) >> 2) & 0x00F0) |  /*G*/
								(((*pSrcPixel) >> 1) & 0x000F);   /*B*/
					if((*pSrcPixel) & 0x8000)
					{
						*pPixels |= 0xF000;
					}	
				}
				else
					Pack1555To4444 (pSrcPixel, pPixels, nMapSize);
			#endif
			break;
		}
		case 4:
		{
			if(!nColourKey)
			{
				/* 565 to 555 */
				#if 0
				for(i=0; i<nMapSize; i++)
				{
					*pPixels =  (((*pSrcPixel) >> 1) & 0x7FE0) |  /*R* and *G*/
								((*pSrcPixel) & 0x001F); 		  /*B*/
					pPixels++; pSrcPixel++; 							
				}
				#else		/* assembler function   */
					if(nMapSize == 1)
					{
						*pPixels =  (((*pSrcPixel) >> 1) & 0x7FE0) |  /*R* and *G*/
									((*pSrcPixel) & 0x001F); 		  /*B*/
					}
					else
						Pack565To555 (pSrcPixel, pPixels, nMapSize);
				#endif
			}
			else
			{
				if(!nTrans1555)
				{
					/* 565 with colour key to 4444 */
					#if 0
					for(i=0; i<nMapSize; i++)
					{
						*pPixels =  (((*pSrcPixel) >> 4) & 0x0F00) |  /*R*/
									(((*pSrcPixel) >> 3) & 0x00F0) |  /*G*/
									(((*pSrcPixel) >> 1) & 0x000F);   /*B*/
						if(*pSrcPixel == KeyColour)
						{
							*pPixels |= 0xF000;
						}							
						pPixels++; pSrcPixel++; 							
					}
					#else		/* assembler function   */
						if(nMapSize == 1)
						{
							*pPixels =  (((*pSrcPixel) >> 4) & 0x0F00) |  /*R*/
										(((*pSrcPixel) >> 3) & 0x00F0) |  /*G*/
										(((*pSrcPixel) >> 1) & 0x000F);   /*B*/
							if(*pSrcPixel == KeyColour)
							{
								*pPixels |= 0xF000;
							}	
						}
						else
							Pack565To4444WithColourKey (pSrcPixel, pPixels, nMapSize, KeyColour);
					#endif
				}
				else
				{	/*  PCX2_003 support 1555 format for colour key  */
					for(i=0; i<nMapSize; i++)
					{
						*pPixels =  (((*pSrcPixel) >> 1) & 0x7FE0) |  /*R* and *G*/
									((*pSrcPixel) & 0x001F); 		  /*B*/
						if(*pSrcPixel == KeyColour)
						{
							*pPixels |= 0x8000;
						}
						pPixels++; pSrcPixel++; 							
					}
				}
			}
			break;
		}
		default:
			PVROSPrintf("texapi: invalid texture Enumtype.\n");
	}
}

/******************************************************************************
 * Function Name: ConvertTextureFormats32   INTERNAL ONLY
 *
 * Inputs       : nMapSize, nEnum, pSrcPixel, nColourKey, KeyColour, nTrans1555.
 *				  
 * Outputs      : pDestPixel
 * Returns      : None
 * Globals Used : None
 *
 * Description  : Converts texture formats from 8888 or 888 to 555 or 4444. 
 *
 *****************************************************************************/
static void ConvertTextureFormats32( int nMapSize, int nEnum, 
							sgl_uint16 **pDestPixel, sgl_uint32 *pSrcPixel,
							int nColourKey, sgl_uint32 KeyColour, int nTrans1555)
{
	int i;
	sgl_uint16 *pPixels;
	
	pPixels = *pDestPixel;
	switch(nEnum)
	{
		case 5:
		{
			/* 888 or 8888(ARGB) to 555 */
			#if 0
			for(i=0; i<nMapSize; i++)
			{
				*pPixels = (sgl_uint16)( ((*pSrcPixel & 0x00F80000) >> 9) |
			 			    ((*pSrcPixel & 0x0000F800) >> 6) |
			  			    ((*pSrcPixel & 0x000000F8) >> 3));
				pPixels++; pSrcPixel++;							
			}
			#else		/* assembler function   */
				if(nMapSize == 1)
				{
					*pPixels = (sgl_uint16)( ((*pSrcPixel & 0x00F80000) >> 9) |
			 				    ((*pSrcPixel & 0x0000F800) >> 6) |
			  			   		 ((*pSrcPixel & 0x000000F8) >> 3));
				}
				else
					Pack888To555 (pSrcPixel, pPixels, nMapSize);
			#endif
			break;
		}
		case 6:
		{
			if(!nColourKey)
			{
				/* 8888(ARGB) to 4444 */
				#if 0
					for(i=0; i<nMapSize; i++)
					{
						*pPixels = (sgl_uint16)( ((*pSrcPixel & 0xF0000000) >> 16) |
								   ((*pSrcPixel & 0x00F00000) >> 12) |
					  			   ((*pSrcPixel & 0x0000F000) >> 8)  |
					  		   	   ((*pSrcPixel & 0x000000F0) >> 4));
						pPixels++; pSrcPixel++;
					}
				#else		/* assembler function   */
					if(nMapSize == 1)
					{
						*pPixels = (sgl_uint16)( ((*pSrcPixel & 0xF0000000) >> 16) |
								   ((*pSrcPixel & 0x00F00000) >> 12) |
					  			   ((*pSrcPixel & 0x0000F000) >> 8)  |
					  		   	   ((*pSrcPixel & 0x000000F0) >> 4));
					}
					else
						Pack8888To4444 (pSrcPixel, pPixels, nMapSize);
				#endif
			}
			else
			{
				if(!nTrans1555)
				{
					/* 888(ARGB) to 4444 with colour key */
					for(i=0; i<nMapSize; i++)
					{
						*pPixels = (sgl_uint16)( ((*pSrcPixel & 0x00F00000) >> 12) |
								   ((*pSrcPixel & 0x0000F000) >> 8)  |
							   	   ((*pSrcPixel & 0x000000F0) >> 4));
						if(*pSrcPixel == KeyColour)
						{
							*pPixels |= 0xF000;
						}	
						pPixels++; pSrcPixel++;
					}
				}
				else
				{	/*  PCX2_003 support 1555 format for colour key  */
					for(i=0; i<nMapSize; i++)
					{
						*pPixels =  (sgl_uint16)( ((*pSrcPixel & 0x00F80000) >> 9) |
			 			  			((*pSrcPixel & 0x0000F800) >> 6) |
			  						((*pSrcPixel & 0x000000F8) >> 3));
						if(*pSrcPixel == KeyColour)
						{
							*pPixels |= 0x8000;
						}
						pPixels++; pSrcPixel++; 							
					}
				}
			}
			break;
		}
		case 7:
		{
			/* 888 or 8888(ABGR) to 555 */
			#if 0
			for(i=0; i<nMapSize; i++)
			{
				*pPixels = (sgl_uint16)( ((*pSrcPixel & 0x00F80000) >> 19) |   /*B*/
			 			    ((*pSrcPixel & 0x0000F800) >> 6) |	  /*G*/	
			  			    ((*pSrcPixel & 0x000000F8) << 7));	  /*R*/
				pPixels++; pSrcPixel++;							
			}
			#else		/* assembler function   */
				if(nMapSize == 1)
				{
					*pPixels = (sgl_uint16)( ((*pSrcPixel & 0x00F80000) >> 19) |   /*B*/
			 			  	  ((*pSrcPixel & 0x0000F800) >> 6) |	  /*G*/	
			  			 	   ((*pSrcPixel & 0x000000F8) << 7));	  /*R*/
				}
				else
					Pack888BGRTo555 (pSrcPixel, pPixels, nMapSize);
			#endif
			break;
		}
		case 8:
		{
			if(!nColourKey)
			{
				/* 8888(ABGR) to 4444 */
				#if 0
					for(i=0; i<nMapSize; i++)
					{
						*pPixels = (sgl_uint16)( ((*pSrcPixel & 0xF0000000) >> 16) |	/*A*/
								   ((*pSrcPixel & 0x00F00000) >> 20) |  /*B*/
					  			   ((*pSrcPixel & 0x0000F000) >> 8)  |  /*G*/
					  		   	   ((*pSrcPixel & 0x000000F0) << 4));	/*R*/
						pPixels++; pSrcPixel++;
					}
				#else		/* assembler function   */
					if(nMapSize == 1)
					{
						*pPixels = (sgl_uint16)( ((*pSrcPixel & 0xF0000000) >> 16) |	/*A*/
								   ((*pSrcPixel & 0x00F00000) >> 20) |  /*B*/
					  			   ((*pSrcPixel & 0x0000F000) >> 8)  |  /*G*/
					  		   	   ((*pSrcPixel & 0x000000F0) << 4));	/*R*/
					}
					else
						Pack8888BGRTo4444 (pSrcPixel, pPixels, nMapSize);
				#endif
			}
			else
			{
				if(!nTrans1555)
				{
					/* 888(ABGR) to 4444 with colour key */
					for(i=0; i<nMapSize; i++)
					{
						*pPixels = (sgl_uint16)( ((*pSrcPixel & 0x00F00000) >> 20) |	/*B*/
				  				   ((*pSrcPixel & 0x0000F000) >> 8)  |	/*G*/
				  		   		   ((*pSrcPixel & 0x000000F0) << 4));	/*R*/
						if(*pSrcPixel == KeyColour)
						{
							*pPixels |= 0xF000;
						}	
						pPixels++; pSrcPixel++;
					}
				}
				else
				{	/*  PCX2_003 support 1555 format for colour key  */
					for(i=0; i<nMapSize; i++)
					{
						*pPixels = (sgl_uint16)( ((*pSrcPixel & 0x00F80000) >> 19) |   /*B*/
			 			 			((*pSrcPixel & 0x0000F800) >> 6) |	  /*G*/	
			  					    ((*pSrcPixel & 0x000000F8) << 7));	  /*R*/
						if(*pSrcPixel == KeyColour)
						{
							*pPixels |= 0x8000;
						}
						pPixels++; pSrcPixel++; 							
					}
				}
			}
			break;
		}
		default:
			PVROSPrintf("texapi: invalid texture Enumtype.\n");
	}
}

/******************************************************************************
 * Function Name: GetShiftValues   INTERNAL ONLY
 *
 * Inputs       : nMapSize, nEnum, pSrcPixel.
 *				  
 * Outputs      : pDestPixel
 * Returns      : None
 * Globals Used : None
 *
 * Description  : Converts texture formats from 8888 or 888 to 555 or 4444. 
 *
 *****************************************************************************/
static void GetShiftValues(int *nLeftShift, int *nRightShift, int nRight, int Width, sgl_uint32 Mask)
{
	int i, number, nLefton, nRighton, nBitLeft, nBitRight;

	nLefton = nRighton = 0;
	nBitLeft = nBitRight = 0;
	for(i=0; i<32; i++)
	{
		if((Mask & 1) && !nRighton)
		{
			nBitRight = i;
			nRighton = nLefton = 1;
		}
		else if(!(Mask & 1) && nLefton)
		{
			nBitLeft = i;
			break;
		}
		if(i == 31 && (Mask & 1) && nLefton)
		{
			nBitLeft = i+1;
			break;
		}
		Mask >>= 1;
	}
	
	*nLeftShift = *nRightShift = 0;
	number = nBitLeft - nBitRight - Width;
	if(number || nBitRight != nRight)
	{
		if(number >= 0) 
		{
			*nRightShift = number + nBitRight;
			*nLeftShift = nRight;
		}
		else  
		{
			*nRightShift = nBitRight;
			*nLeftShift = nRight - number;
		}
	}
}

/******************************************************************************
 * Function Name: ConvertUnknownTextureFormats   INTERNAL ONLY
 *
 * Inputs       : nMapSize, nEnum, pSrcPixel, nColourKey, KeyColour, nTrans1555.
 *				  
 * Outputs      : pDestPixel
 * Returns      : None
 * Globals Used : None
 *
 * Description  : Converts texture formats from 8888 or 888 to 555 or 4444. 
 *
 *****************************************************************************/
static void ConvertUnknownTextureFormats( int nMapSize, 
									sgl_uint16 **pDestPixel, PTEXTURESOURCE pSource,
									int nColourKey, sgl_uint32 KeyColour, int nTrans1555)
{
	int i, BitDepth16, nAlphaLeftShift, nAlphaRightShift, nRedLeftShift, nRedRightShift,
		nGreenLeftShift, nGreenRightShift, nBlueLeftShift, nBlueRightShift;
	sgl_uint32 RedMask, GreenMask, BlueMask, AlphaMask, *pSrcPixel32;
	sgl_uint16 *pPixels, *pSrcPixel16;
	
	if(pSource->pFormat->BitDepth == 16)
	{
		BitDepth16 = 1;
		pSrcPixel16 = pSource->pPixels;
	}
	else
	{
		BitDepth16 = 0;
		pSrcPixel32 = pSource->pPixels;
	}
	pPixels = *pDestPixel;
	RedMask = pSource->pFormat->rgbyuv.rgb.RedMask;
	GreenMask = pSource->pFormat->rgbyuv.rgb.GreenMask;
	BlueMask = pSource->pFormat->rgbyuv.rgb.BlueMask;
	
	/* Get alpha mask.
	 */
	AlphaMask = pSource->pFormat->AlphaMask;

	/* Check alpha mask to see if translucency. We can't have translucency if we
	 * don't have a mask.
	 * If the AlphaMask is zero then we're in trouble so check based on this.
	 */
	if(!AlphaMask)
	{
		/* get left and right shift values for R G B in 555 */
		GetShiftValues(&nBlueLeftShift, &nBlueRightShift, 0, 5, BlueMask);
		GetShiftValues(&nGreenLeftShift, &nGreenRightShift, 5, 5, GreenMask);
		GetShiftValues(&nRedLeftShift, &nRedRightShift, 10, 5, RedMask);

		/* converting the pixels  */
		if(BitDepth16)
		{
			for(i=0; i<nMapSize; i++)
			{
				*pPixels = (sgl_uint16)( (((*pSrcPixel16 & RedMask) >> nRedRightShift) << nRedLeftShift) |
						   (((*pSrcPixel16 & GreenMask) >> nGreenRightShift) << nGreenLeftShift) |
						   (((*pSrcPixel16 & BlueMask) >> nBlueRightShift) << nBlueLeftShift)); 
				pPixels++; pSrcPixel16++;
			}
		}
		else
		{
			for(i=0; i<nMapSize; i++)
			{
				*pPixels = (sgl_uint16)( (((*pSrcPixel32 & RedMask) >> nRedRightShift) << nRedLeftShift) |
						   (((*pSrcPixel32 & GreenMask) >> nGreenRightShift) << nGreenLeftShift) |
						   (((*pSrcPixel32 & BlueMask) >> nBlueRightShift) << nBlueLeftShift)); 
				pPixels++; pSrcPixel32++;
			}
		}
	}
	else
	{
		if(BitDepth16)
		{
			if(!nColourKey)
			{
				/* get left and right shift values for A R G B in 4444 */
				GetShiftValues(&nBlueLeftShift, &nBlueRightShift, 0, 4, BlueMask);
				GetShiftValues(&nGreenLeftShift, &nGreenRightShift, 4, 4, GreenMask);
				GetShiftValues(&nRedLeftShift, &nRedRightShift, 8, 4, RedMask);
				GetShiftValues(&nAlphaLeftShift, &nAlphaRightShift, 12, 4, AlphaMask);

				for(i=0; i<nMapSize; i++)
				{
					*pPixels = (sgl_uint16)( (((*pSrcPixel16 & RedMask) >> nRedRightShift) << nRedLeftShift) |
						   (((*pSrcPixel16 & GreenMask) >> nGreenRightShift) << nGreenLeftShift) |
						   (((*pSrcPixel16 & BlueMask) >> nBlueRightShift) << nBlueLeftShift));

					if(nAlphaLeftShift <= 12)
					{						
						*pPixels |= ((*pSrcPixel16 & AlphaMask) >> nAlphaRightShift) << nAlphaLeftShift;
					}						   
					else  /* We have less than 4 bits of alpha */
					{
						int num;
						sgl_uint32 Alpha;
						num = 4 - nAlphaLeftShift + 12;
						Alpha = (*pSrcPixel16 & AlphaMask);

						/* Checking for transluceny based on the alpha mask ensures
						 * that num can't be zero.
						 */
						while(num < 4)
						{
							Alpha |= Alpha >> num;
							num += num;
						}
						*pPixels |= (Alpha >> 12) << 12;
					}
					pPixels++; pSrcPixel16++;
				}
			}
			else
			{	
				/* using colour key */
				if(!nTrans1555)
				{
					/* get left and right shift values for A R G B in 4444 */
					GetShiftValues(&nBlueLeftShift, &nBlueRightShift, 0, 4, BlueMask);
					GetShiftValues(&nGreenLeftShift, &nGreenRightShift, 4, 4, GreenMask);
					GetShiftValues(&nRedLeftShift, &nRedRightShift, 8, 4, RedMask);
		
					for(i=0; i<nMapSize; i++)
					{
						*pPixels = (sgl_uint16)( (((*pSrcPixel16 & RedMask) >> nRedRightShift) << nRedLeftShift) |
							   (((*pSrcPixel16 & GreenMask) >> nGreenRightShift) << nGreenLeftShift) |
						   	   (((*pSrcPixel16 & BlueMask) >> nBlueRightShift) << nBlueLeftShift));
						if(*pSrcPixel16 == KeyColour)
						{
							*pPixels |= 0xF000;
						}						   
						pPixels++; pSrcPixel16++;
					}
				}
				else
				{	/*  PCX2_003 support 1555 format for colour key  */
				
					/* get left and right shift values for R G B in 555 */
					GetShiftValues(&nBlueLeftShift, &nBlueRightShift, 0, 5, BlueMask);
					GetShiftValues(&nGreenLeftShift, &nGreenRightShift, 5, 5, GreenMask);
					GetShiftValues(&nRedLeftShift, &nRedRightShift, 10, 5, RedMask);

					for(i=0; i<nMapSize; i++)
					{
						*pPixels = (sgl_uint16)( (((*pSrcPixel16 & RedMask) >> nRedRightShift) << nRedLeftShift) |
							   (((*pSrcPixel16 & GreenMask) >> nGreenRightShift) << nGreenLeftShift) |
						   	   (((*pSrcPixel16 & BlueMask) >> nBlueRightShift) << nBlueLeftShift));
						if(*pSrcPixel16 == KeyColour)
						{
							*pPixels |= 0x8000;
						}						   
						pPixels++; pSrcPixel16++;
					}
				}
			}
		}
		else
		{
			if(!nColourKey)
			{
				/* get left and right shift values for A R G B in 4444 */
				GetShiftValues(&nBlueLeftShift, &nBlueRightShift, 0, 4, BlueMask);
				GetShiftValues(&nGreenLeftShift, &nGreenRightShift, 4, 4, GreenMask);
				GetShiftValues(&nRedLeftShift, &nRedRightShift, 8, 4, RedMask);
				GetShiftValues(&nAlphaLeftShift, &nAlphaRightShift, 12, 4, AlphaMask);

				for(i=0; i<nMapSize; i++)
				{
					*pPixels = (sgl_uint16)( (((*pSrcPixel32 & RedMask) >> nRedRightShift) << nRedLeftShift) |
						   (((*pSrcPixel32 & GreenMask) >> nGreenRightShift) << nGreenLeftShift) |
						   (((*pSrcPixel32 & BlueMask) >> nBlueRightShift) << nBlueLeftShift));
					if(nAlphaLeftShift <= 12)
					{
						*pPixels |= ((*pSrcPixel32 & AlphaMask) >> nAlphaRightShift) << nAlphaLeftShift;
					}						   
					else /* We have less than 4 bits of alpha */
					{
						int num;
						sgl_uint32 Alpha;
						num = 4 - nAlphaLeftShift + 12;
						Alpha = (*pSrcPixel32 & AlphaMask);
						while(num < 4)
						{
							Alpha |= Alpha >> num;
							num += num;
						}
						*pPixels |= (Alpha >> 12) << 12;
					}
					pPixels++; pSrcPixel32++;
				}
			}
			else
			{	
				/* using colour key */
				if(!nTrans1555)
				{
					/* get left and right shift values for A R G B in 4444 */
					GetShiftValues(&nBlueLeftShift, &nBlueRightShift, 0, 4, BlueMask);
					GetShiftValues(&nGreenLeftShift, &nGreenRightShift, 4, 4, GreenMask);
					GetShiftValues(&nRedLeftShift, &nRedRightShift, 8, 4, RedMask);
		
					for(i=0; i<nMapSize; i++)
					{
						*pPixels = (sgl_uint16)( (((*pSrcPixel32 & RedMask) >> nRedRightShift) << nRedLeftShift) |
							   (((*pSrcPixel32 & GreenMask) >> nGreenRightShift) << nGreenLeftShift) |
						   	   (((*pSrcPixel32 & BlueMask) >> nBlueRightShift) << nBlueLeftShift));
						if(*pSrcPixel32 == KeyColour)
						{
							*pPixels |= 0xF000;
						}						   
						pPixels++; pSrcPixel32++;
					}
				}
				else
				{	/*  PCX2_003 support 1555 format for colour key  */
				
					/* get left and right shift values for R G B in 555 */
					GetShiftValues(&nBlueLeftShift, &nBlueRightShift, 0, 5, BlueMask);
					GetShiftValues(&nGreenLeftShift, &nGreenRightShift, 5, 5, GreenMask);
					GetShiftValues(&nRedLeftShift, &nRedRightShift, 10, 5, RedMask);

					for(i=0; i<nMapSize; i++)
					{
						*pPixels = (sgl_uint16)( (((*pSrcPixel32 & RedMask) >> nRedRightShift) << nRedLeftShift) |
							   (((*pSrcPixel32 & GreenMask) >> nGreenRightShift) << nGreenLeftShift) |
						   	   (((*pSrcPixel32 & BlueMask) >> nBlueRightShift) << nBlueLeftShift));
						if(*pSrcPixel32 == KeyColour)
						{
							*pPixels |= 0x8000;
						}						   
						pPixels++; pSrcPixel32++;
					}
				}
			}
		}
	}
}

/******************************************************************************
 * Function Name: TextureLoad
 *
 * Inputs       : hTexHeap, hTex, DestinationMap, pSource.
 *
 * Outputs      : None
 *
 * Returns      : error
 *
 * Description  : TextureLoad takes pixel data of the format described in pSource 
 * 				  and uploads it into the given destination map. Non-Zero return indicates
 *				  faliure.
 *****************************************************************************/
PVROSERR CALL_CONV TextureLoad (	HTEXHEAP hTexHeap, 
									HTEXTURE hTex, 
									WHICH_MAP DestinationMap, 
									PTEXTURESOURCE pSource)
{
	PTEXTUREFORMAT pSFormat, pTFormat;
	sgl_uint32	TextureAddress, KeyColour, AlphaMasks, MaskRGB;
	int map_size, nEnumType, nReversedAlpha, nColourKey, Pitch, nPalettised, nTrans1555, nNotHWSupport;
	TPRIVATEDATA *pTPD;

	pTPD = hTex->pPrivateData;
	ASSERT(hTex != NULL);
	ASSERT(hTexHeap != NULL);
	ASSERT(pSource != NULL);

	nEnumType = 100;	nReversedAlpha = nColourKey = nPalettised = nTrans1555 =  nNotHWSupport = 0;
	pSFormat = pSource->pFormat;
	pTFormat = pTPD->pTextureSpec->pFormat;
	AlphaMasks = 0xF000F000;
	MaskRGB = 0xFFFFFFFF;
	
	/*calculate the texture address (relative to the texture base!)*/
	TextureAddress=(hTex->TSPTextureControlWord) & 0x00ffffff;

	/* pick out the map size and type etc.  */
	map_size=(hTex->TSPTextureControlWord>> SHIFT_MAP_SIZE) & 0x0003;
	map_size += 6;

	/*  Weed out 8 bit textures	*/
	if(hTex->TSPTextureControlWord & MASK_8_16_MAPS==0)
	{
		PVROSPrintf("TAPI: Bad parameter in TextureLoad, 8 bit texture.\n");
		return (PVROS_DODGY);
	}

	/* if we have a MIP mapped texture, choose the correct Map level */
	if(hTex->TSPTextureControlWord & MASK_MIP_MAPPED)
	{
		ASSERT( (DestinationMap >= MIPMAP_LEVEL_0) && (DestinationMap <= map_size));
		map_size = DestinationMap;
	} 
	Pitch = pSource->Pitch >> 1;

	if(!(pSFormat->Flags & TF_RGB))
	{
		DPF ((DBG_MESSAGE, "TEXAPI: texture Source is not in RGB format.\n"));
	}

	/*  check if the device is MIDAS5_003 */ 
	if(hTexHeap->DeviceType == MIDAS5_003 )	  nTrans1555 = 1;

	/* palettised souce texture formats either 4444 (may with colourkey) or 555 */
	if((pSFormat->Flags & TF_PALETTISED4) || (pSFormat->Flags & TF_PALETTISED8)) 
	{ 	
		int i;
		/* local copy of Palette */
		sgl_uint16 Palette[256];
		nPalettised = 256;
		if(pSFormat->Flags & TF_PALETTISED4)
		{
			nPalettised = 16;
		}

		for(i=0; i<nPalettised; i++)
			  Palette[i] = ((sgl_uint16 *)pSource->pPalette)[i];
		
		/* 4444 palette with colour key */
		if(pSFormat->Flags & TF_COLOURKEY)
		{
		   	Palette[pSFormat->KeyColour] |= 0xF000;
		}
		/* 4444 palette with reversed alpha */
		else if((pSFormat->Flags & TF_TRANSLUCENT) &&
			(pSFormat->Flags & TF_OGLALPHA) != (pTFormat->Flags & TF_OGLALPHA))
		{
			for(i=0; i<nPalettised; i++)
			   Palette[i] ^=  0xF000;
		}
		
		/* turn off alpha bit for 555 */
		if( nTrans1555 && !(pSFormat->Flags & TF_TRANSLUCENT))
		{
			for(i=0; i<nPalettised; i++)
				Palette[i] &= 0x7FFF;
		}

		SynchroniseTexMemAccess (hTexHeap, TRUE);

		WriteTextureToMem(TextureAddress,
						  (sgl_uint32 *)(hTexHeap->pTextureMemory),
						  map_size,
						  DestinationMap,
						  pSource->Pitch,
						  pSource->pPixels,
						  nReversedAlpha,
						  nPalettised,
						  Palette, AlphaMasks, MaskRGB);

		SynchroniseTexMemAccess (hTexHeap, FALSE);
		return(PVROS_GROOVY);	
	}

	/* check for colour key */
	if(pSFormat->Flags & TF_COLOURKEY)
	{
		if(!(pTFormat->Flags & TF_TRANSLUCENT))
		{
			DPF ((DBG_MESSAGE, "TEXAPI: using colour key, but texture spec is not in 4444 format.\n"));
		}
		if(pSFormat->Flags & TF_TRANSLUCENT)
		{
			DPF ((DBG_MESSAGE, "TEXAPI: using colour key, but texture source is in 4444 format.\n"));
		}
		nColourKey = 1;
		KeyColour = pSFormat->KeyColour;

		/*  PCX2_003 support 1555, set the flag MASK_TRANS by checking the Alpha Mask. */
		if(nTrans1555)
		{
			pTFormat->AlphaMask = 0x00008000;  	pSFormat->AlphaMask = 0x00008000;
			hTex->TSPTextureControlWord &= ~MASK_4444_555;
		}
		else
		{	/* make sure the destination is 4444 */
			pTFormat->AlphaMask = 0x0000F000; 
			hTex->TSPTextureControlWord |= MASK_4444_555; 	
		}
	}
	
	/* Check that the enumerated type is valid (0 for 555, 1 for 4444)	*/
	if(pSFormat->Flags & TF_HWSUPPORT) 
	{
		/* the source is either 555 or 4444 */
		if(pSFormat->Flags == pTFormat->Flags)
		{
			if((pSFormat->BitDepth != pTFormat->BitDepth) ||
				(pSFormat->rgbyuv.rgb.RedMask != pTFormat->rgbyuv.rgb.RedMask) ||
				(pSFormat->rgbyuv.rgb.GreenMask != pTFormat->rgbyuv.rgb.GreenMask) ||
				(pSFormat->rgbyuv.rgb.BlueMask != pTFormat->rgbyuv.rgb.BlueMask ))
			{
				DPF ((DBG_MESSAGE, "TEXAPI: bad format of texture source, 555 or 4444.\n"));
				 nNotHWSupport = 1; 
			}
			if(pSFormat->Flags & TF_TRANSLUCENT)
			{
				if(pSFormat->AlphaMask != pTFormat->AlphaMask )
				{
					DPF ((DBG_MESSAGE, "TEXAPI: bad format of texture source, 555 or 4444.\n"));
					 nNotHWSupport = 1; 
				}
			}
			if(!nNotHWSupport)
			{
				nEnumType = (pTFormat->Flags & TF_TRANSLUCENT) ? 1 : 0;
				/* turn off alpha bit for 555 */
				if(!nEnumType)	MaskRGB = 0x7FFF7FFF;
			}
		}
		/* Source and Spec flags are not match, but the source texture may not need conversion*/
		else
		{		
			if((pSFormat->rgbyuv.rgb.RedMask == 0x00007C00) &&
			   (pSFormat->rgbyuv.rgb.GreenMask == 0x000003E0) &&
			   (pSFormat->rgbyuv.rgb.BlueMask == 0x0000001F) &&
			   (pSFormat->AlphaMask != 0x00008000) &&
			   !(pSFormat->Flags & TF_COLOURKEY)) 
			{			   
			   
			   	/* Source in 555  with no colour key, no Alpha, leave it as 555. 
			   	**	  It happens in Monster Truck.	*/
			   	nEnumType = 0;
				hTex->TSPTextureControlWord &= ~MASK_4444_555;
				/* turn off alpha bit for 555 */
				MaskRGB = 0x7FFF7FFF;
			}
			else if((pSFormat->rgbyuv.rgb.RedMask == 0x00000F00) &&
			  	  (pSFormat->rgbyuv.rgb.GreenMask == 0x000000F0) &&
			 	  (pSFormat->rgbyuv.rgb.BlueMask == 0x0000000F) &&
			  	  (pSFormat->AlphaMask == 0x0000F000))
			{
				/* Source in 4444, leave it as 4444 */
				nEnumType = 1;
				if((pSFormat->Flags & TF_OGLALPHA) != (pTFormat->Flags & TF_OGLALPHA)) 
				{
					nReversedAlpha = 1;	 	
				}	
			   	hTex->TSPTextureControlWord |= MASK_4444_555;
			   	if(pSFormat->Flags & TF_COLOURKEY)
					nEnumType = 100;
			}
			/*  PCX2_003  */
			if(nTrans1555)
			{
				/*  1555 */
				if((pSFormat->rgbyuv.rgb.RedMask == 0x00007C00) &&
			  		 (pSFormat->rgbyuv.rgb.GreenMask == 0x000003E0) &&
					 (pSFormat->rgbyuv.rgb.BlueMask == 0x0000001F) &&
			   		 (pSFormat->AlphaMask == 0x00008000))
			    {
					/* Source in 1555, leave it as 1555 */
					nEnumType = 1;
					if((pSFormat->Flags & TF_OGLALPHA) != (pTFormat->Flags & TF_OGLALPHA)) 
					{
						nReversedAlpha = 1;	 AlphaMasks = 0x80008000;
					}	
					hTex->TSPTextureControlWord &= ~MASK_4444_555;
				    pTFormat->AlphaMask = 0x00008000;
			   	}
				/*  444  */
				else if((pSFormat->rgbyuv.rgb.RedMask == 0x00000F00) &&
			  		  (pSFormat->rgbyuv.rgb.GreenMask == 0x000000F0) &&
			 	  	  (pSFormat->rgbyuv.rgb.BlueMask == 0x0000000F))
			  	{
					/* Source in 444, leave it as 444 */
					nEnumType = 0;
					hTex->TSPTextureControlWord |= MASK_4444_555;
			   	}
		  	}
		}
	}
	/* texture source format is neither 555 nor 4444, needs to be converted */
	if (nEnumType == 100)
	{
		if(pSFormat->BitDepth == 16)
		{
			if((pSFormat->rgbyuv.rgb.RedMask == 0x00000F00) &&
			   (pSFormat->rgbyuv.rgb.GreenMask == 0x000000F0) &&
			   (pSFormat->rgbyuv.rgb.BlueMask == 0x0000000F) &&
			   (pSFormat->AlphaMask == 0x0000F000) &&
			   (pSFormat->Flags & TF_OGLALPHA) != (pTFormat->Flags & TF_OGLALPHA))
			{
				/* 4444 with reversed alpha */
				if(!((pSFormat->Flags & TF_TRANSLUCENT) && (pTFormat->Flags & TF_TRANSLUCENT)))
				{
					DPF ((DBG_MESSAGE, "TEXAPI: bad format of texture source, 4444 with reversed alpha.\n"));
				}
				nEnumType = 1;	nReversedAlpha = 1;
				if(pSFormat->Flags & TF_COLOURKEY)
					nEnumType = 100;
			}
			else if((pSFormat->rgbyuv.rgb.RedMask == 0x00007C00) &&
			   	   (pSFormat->rgbyuv.rgb.GreenMask == 0x000003E0) &&
			  	   (pSFormat->rgbyuv.rgb.BlueMask == 0x0000001F) &&
				   (pSFormat->Flags & TF_COLOURKEY)) 
			{
				/* 555  with colour key */
				nEnumType = 1;
			}
		   	else if((pSFormat->rgbyuv.rgb.RedMask == 0x0000F800) &&
			   	   (pSFormat->rgbyuv.rgb.GreenMask == 0x000007E0) &&
			  	   (pSFormat->rgbyuv.rgb.BlueMask == 0x0000001F)) 
			{
				/* 565 */
				nEnumType = 4;
				/* turn off alpha bit for 555 */
				if(!nColourKey) MaskRGB = 0x7FFF7FFF;
 			}
			/*  1555 */
			else if((pSFormat->rgbyuv.rgb.RedMask == 0x00007C00) &&
			   	   (pSFormat->rgbyuv.rgb.GreenMask == 0x000003E0) &&
			  	   (pSFormat->rgbyuv.rgb.BlueMask == 0x0000001F) &&
			   	   (pSFormat->AlphaMask == 0x00008000))
			{
				if(!nTrans1555)
				{
					/* 1555 to 4444  */
					nEnumType = 3;
				   	hTex->TSPTextureControlWord |= MASK_4444_555;				
					/* 1555 with reversed alpha */
					if((pSFormat->Flags & TF_OGLALPHA) != (pTFormat->Flags & TF_OGLALPHA)) 
				  	  	nReversedAlpha = 1;	 	
				}
				else 
				{	/*  Don't need to convert 1555 in PCX2_003  */
					nEnumType = 1;
					hTex->TSPTextureControlWord &= ~MASK_4444_555;
				    pTFormat->AlphaMask = 0x00008000;	
					/* 1555  with reversed alpha */
					if((pSFormat->Flags & TF_OGLALPHA) != (pTFormat->Flags & TF_OGLALPHA))	 
					{  nReversedAlpha = 1;	 	AlphaMasks = 0x80008000; }
				}
			}
		}
		else /*  32 bit textures */ 
		{
			if((pSFormat->rgbyuv.rgb.RedMask == 0x00FF0000) &&
			   (pSFormat->rgbyuv.rgb.GreenMask == 0x0000FF00) &&
			   (pSFormat->rgbyuv.rgb.BlueMask == 0x000000FF))
			{
				/* 888 to 555  or 8888(ARGB) to either 4444 or 555  */				
				nEnumType = (pTFormat->Flags & TF_TRANSLUCENT) ? 6 : 5;	
				if(pSFormat->Flags & TF_TRANSLUCENT)
				{
					if(pSFormat->AlphaMask != 0xFF000000)
					{
						DPF ((DBG_MESSAGE, "TEXAPI: bad format of texture source, 8888(ARGB) to 4444.\n"));
					}
					/* 8888 with reversed alpha */
					if(((pSFormat->Flags & TF_OGLALPHA) != (pTFormat->Flags & TF_OGLALPHA)) && 
						nEnumType == 6)
					{
						nReversedAlpha = 1;	   
					}	
				}
				if(!(pSFormat->Flags & TF_TRANSLUCENT))
				{
					/* turn off alpha bit for 555 */
					MaskRGB = 0x7FFF7FFF;
				}
			}	
			else if((pSFormat->rgbyuv.rgb.BlueMask == 0x00FF0000) &&
			   (pSFormat->rgbyuv.rgb.GreenMask == 0x0000FF00) &&
			   (pSFormat->rgbyuv.rgb.RedMask == 0x000000FF))
			{
				/* 888 to 555  or 8888(ABGR) to either 4444 or 555 */				
				nEnumType = (pTFormat->Flags & TF_TRANSLUCENT) ? 8 : 7;	
				if(pSFormat->Flags & TF_TRANSLUCENT)
				{
					if(pSFormat->AlphaMask != 0xFF000000) 
					{
						DPF ((DBG_MESSAGE, "TEXAPI: bad format of texture source, 8888(ABGR) to 4444.\n"));
					}
					/* 8888 with reversed alpha */
					if(((pSFormat->Flags & TF_OGLALPHA) != (pTFormat->Flags & TF_OGLALPHA)) && 
						nEnumType == 8)
					{
						nReversedAlpha = 1;	   
					}	
				}
				if(!(pSFormat->Flags & TF_TRANSLUCENT))
				{
					/* turn off alpha bit for 555 */
					MaskRGB = 0x7FFF7FFF;
				}
			}	
		}	
	}
	
	if(nEnumType > 1 || (nEnumType == 1 && nColourKey))
	{
		int nMapSize, nMapWidth;
		sgl_uint16 *pDestPixels;
		nMapWidth = 1 << (map_size - 1);
		nMapSize = nMapWidth*nMapWidth;
		pDestPixels = PVROSMalloc(nMapSize*sizeof(sgl_uint16));
		if(pDestPixels == NULL)
		{
			PVROSPrintf("TAPI: Out of Memory.\n");
			return (PVROS_DODGY);
		}
		if(nEnumType == 1 && nColourKey)
		{
			Convert555WithColourKey(nMapSize, &pDestPixels,
									pSource->pPixels, nColourKey, KeyColour, nTrans1555);
		}
		else if(nEnumType < 5)
		{
			ConvertTextureFormats16(nMapSize, nEnumType, &pDestPixels,
									pSource->pPixels, nColourKey, KeyColour, nTrans1555);
		}
		else if(nEnumType < 9)
		{
			ConvertTextureFormats32(nMapSize, nEnumType, &pDestPixels,
									pSource->pPixels, nColourKey, KeyColour, nTrans1555);
			Pitch = Pitch >> 1;
		}
		else
		{
			if((pSFormat->Flags & TF_OGLALPHA) != (pTFormat->Flags & TF_OGLALPHA)
				&& (pTFormat->Flags & TF_TRANSLUCENT))
			{
				nReversedAlpha = 1;	    
			}
			ConvertUnknownTextureFormats(nMapSize, &pDestPixels,
											pSource, nColourKey, KeyColour, nTrans1555);
			if(pSFormat->BitDepth == 32)
			{
				Pitch = Pitch >> 1;
			}
		}
		/* turn off reversed alpha for colour key */
		if(nColourKey)	 nReversedAlpha = 0;   
		
		/*
		// OK It appears to be dangerous to write to the texture memory while
		// rendering, so we'll prevent this from happening
		*/
		SynchroniseTexMemAccess (hTexHeap, TRUE);
	
		WriteTextureToMem(TextureAddress,
						  (sgl_uint32 *)(hTexHeap->pTextureMemory),		
						  map_size,
						  DestinationMap,
						  Pitch,
						  pDestPixels, 
						  nReversedAlpha,
						  nPalettised,
						  pSource->pPalette, AlphaMasks, MaskRGB);
		PVROSFree(pDestPixels);	
		SynchroniseTexMemAccess (hTexHeap, FALSE);
	}
	else
	{
		SynchroniseTexMemAccess (hTexHeap, TRUE);

		WriteTextureToMem(TextureAddress,
						  (sgl_uint32 *)(hTexHeap->pTextureMemory),
						  map_size,
						  DestinationMap,
						  Pitch,
						  pSource->pPixels,
						  nReversedAlpha,
						  nPalettised,
						  pSource->pPalette, AlphaMasks, MaskRGB);

		SynchroniseTexMemAccess (hTexHeap, FALSE);
	}

	return(PVROS_GROOVY);					

} /* end of TextureLoad */

/******************************************************************************
 * Function Name: DirectCopyTexture    INTERNAL ONLY
 *
 * Inputs       : DestAddress, pBusBurstSpace, MapLevel, *pPixels.
 *
 * Outputs      : None
 *
 * Returns      : None
 *
 * Description  : Copy one level texture map  directly into the texture
 *				  memory.
 *				  
 *****************************************************************************/
static void DirectCopyTexture(	sgl_uint32		DestAddress,
					sgl_uint32		*pBusBurstSpace,
					int				MapLevel,
					sgl_uint16		*pPixels)
{ 
	sgl_uint32 *pAddress;
	int i, nMapSize;
	
	pAddress = pBusBurstSpace + (DestAddress>>1);
	
	if(MapLevel == MIPMAP_LEVEL_0)
	{
		/* 1x1 */
		sgl_uint32 Value;
			
		#if PCX2 || PCX2_003
			Value = 0;
		#else
			Value = IR( pAddress, 0);
		#endif

		if(DestAddress & 1)
		{
			Value=(Value & 0xFFFF0000ul) | pPixels[0];
		}
		else
		{
			Value=(Value & 0xFFFFul) | (pPixels[0]<<16);
		}
		IW( pAddress, 0, Value);
	}
	else	
	{
		nMapSize = (1<<(MapLevel-1))*(1<<(MapLevel-1));
		for(i=0; i<nMapSize; i += 2)
		{
			IW( pAddress, 0, ((pPixels[i] << 16) | pPixels[i+1]));
			pAddress++;
		}
	}
}

/******************************************************************************
 * Function Name: TextureCopy
 *
 * Inputs       : hTexHeap, hTex, DestinationMap, pSource.
 *
 * Outputs      : None
 *
 * Returns      : error
 *
 * Description  : TextureCopy takes pixel data of the preprocessed texture 
 * 				  and writes it into memory. Non-Zero return indicates faliure.
 *****************************************************************************/
PVROSERR CALL_CONV TextureCopy (	HTEXHEAP hTexHeap, 
									HTEXTURE hTex, 
									WHICH_MAP DestinationMap, 
									sgl_uint16		*pPixels)
{
	sgl_uint32	TextureAddress;
	int map_size;

	ASSERT(hTex != NULL);
	ASSERT(hTexHeap != NULL);
	ASSERT(pPixels != NULL);

	/*calculate the texture address (relative to the texture base!)*/
	TextureAddress=(hTex->TSPTextureControlWord) & 0x00ffffff;

	/* pick out the map size and type etc.  */
	map_size=(hTex->TSPTextureControlWord>> SHIFT_MAP_SIZE) & 0x0003;
	map_size += 6;

	/*  Weed out 8 bit textures	*/
	if(hTex->TSPTextureControlWord & MASK_8_16_MAPS==0)
	{
		PVROSPrintf("TAPI: Bad parameter in TextureLoad, 8 bit texture.\n");
		return (PVROS_DODGY);
	}

	/* if we have a MIP mapped texture, choose the correct Map level */
	if(hTex->TSPTextureControlWord & MASK_MIP_MAPPED)
	{
		ASSERT( (DestinationMap >= MIPMAP_LEVEL_0) && (DestinationMap <= map_size));
		map_size = DestinationMap;
	} 
	
	SynchroniseTexMemAccess (hTexHeap, TRUE);

	if(!DestinationMap)
	{
		/* Non MipMapped texture, write directly into memory. */
		DirectCopyTexture(TextureAddress, (sgl_uint32 *)(hTexHeap->pTextureMemory),
						map_size, pPixels);
	}
	else
	{
		/* MipMapped texture, write each level into memory in alternate banks.	*/
		int 			nLevel;
		sgl_uint32		DestAddress;
		sgl_uint16		*pSrcPixels;

		for(nLevel=0; nLevel<DestinationMap; nLevel++)
		{
			/* get the positions into the Src and Destination maps. */
			pSrcPixels = pPixels + EXTMapOffset[nLevel];
			DestAddress = TextureAddress + MAP_OFFSET[nLevel];

			DirectCopyTexture(DestAddress, (sgl_uint32 *)(hTexHeap->pTextureMemory), 
							nLevel+1, pSrcPixels);

			/* Swap Texture Banks so that we write MIP map levels in alternate banks. */
			TextureAddress ^= BIG_BANK;
			
		}/*end for Map Level*/
	}

	SynchroniseTexMemAccess (hTexHeap, FALSE);

	return(PVROS_GROOVY);					

} /* end of TextureCopy */

#define RB_GA_4444(rb,ga) (((rb >> 20) & 0x000F) | \
						   	((ga >> 8) & 0x00F0) |	\
				       		((rb << 4) & 0x0F00) |	\
				       		((ga >> 16) & 0xF000))

#define RB_GA_555(rb,ga)  (((rb >> 19) & 0x001F) | \
						  ((ga >> 6) & 0x03E0) | \
				       	  ((rb << 7) & 0x7C00))

static sgl_uint32 LUTx[6][128];
static sgl_uint32 LUTy[6][64];
static sgl_uint32 LUTInitialised = FALSE;
#define GetTable(third, second, nTable) \
			nTable[1] = second;\
			nTable[2] = third;\
			nTable[3] = second + third

/******************************************************************************
 * Function Name: ConvertTextureFormats16To8888   INTERNAL ONLY
 *
 * Inputs       : nMapSize, nEnum, pSrcPixel, nColourKey, KeyColour, nReversedAlpha.
 *				  
 * Outputs      : pDestPixel
 * Returns      : None
 * Globals Used : None
 *
 * Description  : Converts 16 bit texture formats. 
 *
 *****************************************************************************/
static void ConvertTextureFormats16To8888(int nMapSize, int nEnum, 
							sgl_uint32 **pDestPixel, sgl_uint16 *pSrcPixel,
							int nColourKey, sgl_uint32 KeyColour, int nReversedAlpha) 
{
	int i;
	sgl_uint32 *pPixels;
	
	pPixels = *pDestPixel;
		
	switch(nEnum)
	{
		case 1:
		{
			/* 4444 to 8888(ABGR) */
			if(!nReversedAlpha)
			{
				if(!nColourKey)
				{
					for(i=0; i<nMapSize; i++)
					{
						*pPixels =  (((*pSrcPixel) & 0x0F00) >> 4) |   /*R*/
									(((*pSrcPixel) & 0x00F0) << 8) |   /*G*/
									(((*pSrcPixel) & 0x000F) << 20) |  /*B*/
									(((*pSrcPixel) & 0xF000) << 16);   /*A*/
						pPixels++; pSrcPixel++; 							
					}
				}
				else
				{	/* has colourkey */
					for(i=0; i<nMapSize; i++)
					{
						*pPixels =  (((*pSrcPixel) & 0x0F00) >> 4) |   /*R*/
									(((*pSrcPixel) & 0x00F0) << 8) |   /*G*/
									(((*pSrcPixel) & 0x000F) << 20) |  /*B*/
									(((*pSrcPixel) & 0xF000) << 16);   /*A*/ 
						if(*pSrcPixel == KeyColour)
						{
							*pPixels |= 0xFF000000;
						}	
						pPixels++; pSrcPixel++; 							
					}
				}
			}
			else
			{
				for(i=0; i<nMapSize; i++)
				{
					*pPixels =  (((*pSrcPixel) & 0x0F00) >> 4) |   /*R*/
								(((*pSrcPixel) & 0x00F0) << 8) |   /*G*/
								(((*pSrcPixel) & 0x000F) << 20) |  /*B*/
								(((*pSrcPixel) & 0xF000) << 16);   /*A*/
					*pPixels ^= 0xFF000000;
					pPixels++; pSrcPixel++; 							
				}
			}
			break;
		}
		case 2:
		{
			/* 555 to 888(BGR) */
			if(!nColourKey)
			{
				for(i=0; i<nMapSize; i++)
				{
					*pPixels =  (((*pSrcPixel) & 0x7C00) >> 7) |   /*R*/
								(((*pSrcPixel) & 0x03E0) << 6) |   /*G*/
								(((*pSrcPixel) & 0x001F) << 19);   /*B*/	
					pPixels++; pSrcPixel++; 							
				}
			}
			else
			{	/* has colourkey */
				for(i=0; i<nMapSize; i++)
				{
					*pPixels =  (((*pSrcPixel) & 0x7C00) >> 7) |   /*R*/
								(((*pSrcPixel) & 0x03E0) << 6) |   /*G*/
								(((*pSrcPixel) & 0x001F) << 19);   /*B*/	
					if(*pSrcPixel == KeyColour)
					{
						*pPixels |= 0xFF000000;
					}	
					pPixels++; pSrcPixel++; 							
				}
			}
			break;
		}
		case 3:
		{
			/* 1555 to 8888(ABGR) */
			if(!nReversedAlpha)
			{
				if(!nColourKey)
				{
					for(i=0; i<nMapSize; i++)
					{
						*pPixels =  (((*pSrcPixel) & 0x7C00) >> 7) |   /*R*/
									(((*pSrcPixel) & 0x03E0) << 6) |   /*G*/
									(((*pSrcPixel) & 0x001F) << 19);   /*B*/
						if((*pSrcPixel) & 0x8000)
						{
							*pPixels |= 0xFF000000;
						}							
						pPixels++; pSrcPixel++; 							
					}
				}	
				else
				{	/* has colourkey */
					for(i=0; i<nMapSize; i++)
					{
						*pPixels =  (((*pSrcPixel) & 0x7C00) >> 7) |   /*R*/
									(((*pSrcPixel) & 0x03E0) << 6) |   /*G*/
									(((*pSrcPixel) & 0x001F) << 19);   /*B*/
						if(*pSrcPixel == KeyColour)
						{
							*pPixels |= 0xFF000000;
						}							
						pPixels++; pSrcPixel++; 							
					}
				}	
			}
			else
			{
				for(i=0; i<nMapSize; i++)
				{
					*pPixels =  (((*pSrcPixel) & 0x7C00) >> 7) |   /*R*/
								(((*pSrcPixel) & 0x03E0) << 6) |   /*G*/
								(((*pSrcPixel) & 0x001F) << 19);   /*B*/
					if((*pSrcPixel) & 0x8000)
					{
						*pPixels |= 0xFF000000;
					}	
					*pPixels ^= 0xFF000000;
					pPixels++; pSrcPixel++; 							
				}	
			}
			break;
		}
		case 4:
		{
			/* 565 to 888(BGR) */
			if(!nColourKey)
			{
				for(i=0; i<nMapSize; i++)
				{
					*pPixels =  (((*pSrcPixel) & 0xF800) >> 8) |   /*R*/
								(((*pSrcPixel) & 0x07E0) << 5) |   /*G*/
								(((*pSrcPixel) & 0x001F) << 19);   /*B*/
					pPixels++; pSrcPixel++; 							
				}
			}
			else
			{	/* has colourkey */
				for(i=0; i<nMapSize; i++)
				{
					*pPixels =  (((*pSrcPixel) & 0xF800) >> 8) |   /*R*/
								(((*pSrcPixel) & 0x07E0) << 5) |   /*G*/
								(((*pSrcPixel) & 0x001F) << 19);   /*B*/
					if(*pSrcPixel == KeyColour)
					{
						*pPixels |= 0xFF000000;
					}							
					pPixels++; pSrcPixel++; 							
				}
			}
			break;
		}
		default:
			PVROSPrintf("texapi: invalid texture Enumtype.\n");
	}
}

/******************************************************************************
 * Function Name: ConvertTextureFormats32To8888   INTERNAL ONLY
 *
 * Inputs       : nMapSize, nEnum, pSrcPixel, nColourKey, KeyColour, nReversedAlpha.
 *				  
 * Outputs      : pDestPixel
 * Returns      : None
 * Globals Used : None
 *
 * Description  : Converts texture formats from 8888 or 888 to 555 or 4444. 
 *
 *****************************************************************************/
static void ConvertTextureFormats32To8888( int nMapSize, sgl_uint32 **pDestPixel, sgl_uint32 *pSrcPixel,
							int nColourKey, sgl_uint32 KeyColour, int nReversedAlpha)
{
	int i;
	sgl_uint32 *pPixels;
	
	pPixels = *pDestPixel;

	if(!nColourKey)
	{
		if(!nReversedAlpha)
		{
			for(i=0; i<nMapSize; i++)
			{
				*pPixels =  ((*pSrcPixel) & 0xFF00FF00) | /*AG*/
	 					    (((*pSrcPixel) & 0x000000FF) << 16) | /*B*/
	  					    (((*pSrcPixel) & 0x00FF0000) >> 16);  /*R*/
				pPixels++; pSrcPixel++;							
			}
		}
		else
		{
			for(i=0; i<nMapSize; i++)
			{
				*pPixels =  ((*pSrcPixel) & 0xFF00FF00) | /*AG*/
	 					    (((*pSrcPixel) & 0x000000FF) << 16) | /*B*/
	  					    (((*pSrcPixel) & 0x00FF0000) >> 16);  /*R*/
				*pPixels ^= 0xFF000000;
				pPixels++; pSrcPixel++;							
			}
		}
	}
	else
	{
		for(i=0; i<nMapSize; i++)
		{
			*pPixels =  ((*pSrcPixel) & 0xFF00FF00) | /*AG*/
 					    (((*pSrcPixel) & 0x000000FF) << 16) | /*B*/
  					    (((*pSrcPixel) & 0x00FF0000) >> 16);  /*R*/
			if(*pSrcPixel == KeyColour)
			{
				*pPixels |= 0xFF000000;
			}	
			pPixels++; pSrcPixel++;							
		}
	}
}

/******************************************************************************
 * Function Name: ConvertUnknownFormatsTo8888   INTERNAL ONLY
 *
 * Inputs       : nMapSize, nEnum, pSrcPixel, nColourKey, KeyColour, nReversedAlpha.
 *				  
 * Outputs      : pDestPixel
 * Returns      : None
 * Globals Used : None
 *
 * Description  : Converts texture formats from 8888 or 888 to 555 or 4444. 
 *
 *****************************************************************************/
static void ConvertUnknownFormatsTo8888( int nMapSize, int nTranslucent,
									sgl_uint32 **pDestPixel, PTEXTURESOURCE pSource,
									int nColourKey, sgl_uint32 KeyColour, int nReversedAlpha)
{
	int i, BitDepth16, nAlphaLeftShift, nAlphaRightShift, nRedLeftShift, nRedRightShift,
		nGreenLeftShift, nGreenRightShift, nBlueLeftShift, nBlueRightShift;
	sgl_uint32 RedMask, GreenMask, BlueMask, AlphaMask, *pSrcPixel32, *pPixels;
	sgl_uint16 *pSrcPixel16;
	
	if(pSource->pFormat->BitDepth == 16)
	{
		BitDepth16 = 1;
		pSrcPixel16 = pSource->pPixels;
	}
	else
	{
		BitDepth16 = 0;
		pSrcPixel32 = pSource->pPixels;
	}
	pPixels = *pDestPixel;
	RedMask = pSource->pFormat->rgbyuv.rgb.RedMask;
	GreenMask = pSource->pFormat->rgbyuv.rgb.GreenMask;
	BlueMask = pSource->pFormat->rgbyuv.rgb.BlueMask;
	
	/* get left and right shift values for R G B  */
	GetShiftValues(&nBlueLeftShift, &nBlueRightShift,  16, 8, BlueMask);
	GetShiftValues(&nGreenLeftShift, &nGreenRightShift, 8, 8, GreenMask);
	GetShiftValues(&nRedLeftShift, &nRedRightShift,     0, 8, RedMask);
	
	if(!nTranslucent)
	{
		/* converting the pixels  */
		if(BitDepth16)
		{
			if(!nColourKey)
			{
				for(i=0; i<nMapSize; i++)
				{
					*pPixels = (((*pSrcPixel16 & RedMask) >> nRedRightShift) << nRedLeftShift) |
							   (((*pSrcPixel16 & GreenMask) >> nGreenRightShift) << nGreenLeftShift) |
							   (((*pSrcPixel16 & BlueMask) >> nBlueRightShift) << nBlueLeftShift); 
					*pPixels &= 0x00FFFFFF;
					pPixels++; pSrcPixel16++;
				}
			}
			else
			{	
				/* using colour key */
				for(i=0; i<nMapSize; i++)
				{
					*pPixels = (((*pSrcPixel16 & RedMask) >> nRedRightShift) << nRedLeftShift) |
							   (((*pSrcPixel16 & GreenMask) >> nGreenRightShift) << nGreenLeftShift) |
							   (((*pSrcPixel16 & BlueMask) >> nBlueRightShift) << nBlueLeftShift);
					if(*pSrcPixel16 == KeyColour)
					{
						*pPixels |= 0xFF000000;
					}	
					pPixels++; pSrcPixel16++;
				}
			}
		}
		else
		{
			if(!nColourKey)
			{
				for(i=0; i<nMapSize; i++)
				{
					*pPixels = (((*pSrcPixel32 & RedMask) >> nRedRightShift) << nRedLeftShift) |
							   (((*pSrcPixel32 & GreenMask) >> nGreenRightShift) << nGreenLeftShift) |
							   (((*pSrcPixel32 & BlueMask) >> nBlueRightShift) << nBlueLeftShift); 
					*pPixels &= 0x00FFFFFF;
					pPixels++; pSrcPixel32++;
				}
			}
			else
			{
				/* using colour key */
			 	for(i=0; i<nMapSize; i++)
			 	{
			 		*pPixels = (((*pSrcPixel32 & RedMask) >> nRedRightShift) << nRedLeftShift) |
			 				   (((*pSrcPixel32 & GreenMask) >> nGreenRightShift) << nGreenLeftShift) |
			 				   (((*pSrcPixel32 & BlueMask) >> nBlueRightShift) << nBlueLeftShift); 
			 		if(*pSrcPixel32 == KeyColour)
			 		{
			 			*pPixels |= 0xFF000000;
			 		}	
			 		pPixels++; pSrcPixel32++;
				}
			}
		}
	}
	else
	{
		AlphaMask = pSource->pFormat->AlphaMask;
		GetShiftValues(&nAlphaLeftShift, &nAlphaRightShift, 24, 8, AlphaMask);

		if(BitDepth16)
		{
			if(!nReversedAlpha)
			{
				for(i=0; i<nMapSize; i++)
				{
					*pPixels = (((*pSrcPixel16 & RedMask) >> nRedRightShift) << nRedLeftShift) |
							   (((*pSrcPixel16 & GreenMask) >> nGreenRightShift) << nGreenLeftShift) |
							   (((*pSrcPixel16 & BlueMask) >> nBlueRightShift) << nBlueLeftShift);

					if(nAlphaLeftShift >= 12)
					{						
						*pPixels |= ((*pSrcPixel16 & AlphaMask) >> nAlphaRightShift) << nAlphaLeftShift;
					}						   
					else  /* We have less than 4 bits of alpha */
					{
						int num;
						sgl_uint32 Alpha;
						num = 4 - nAlphaLeftShift + 12;
						Alpha = (*pSrcPixel16 & AlphaMask);

						while(num < 4)
						{
							Alpha |= Alpha >> num;
							num += num;
						}
						*pPixels |= (Alpha >> 12) << 12;
					}
					pPixels++; pSrcPixel16++;
				}
			}
			else
			{
				for(i=0; i<nMapSize; i++)
				{
					*pPixels = (((*pSrcPixel16 & RedMask) >> nRedRightShift) << nRedLeftShift) |
							   (((*pSrcPixel16 & GreenMask) >> nGreenRightShift) << nGreenLeftShift) |
							   (((*pSrcPixel16 & BlueMask) >> nBlueRightShift) << nBlueLeftShift);

					if(nAlphaLeftShift >= 12)
					{						
						*pPixels |= ((*pSrcPixel16 & AlphaMask) >> nAlphaRightShift) << nAlphaLeftShift;
					}						   
					else  /* We have less than 4 bits of alpha */
					{
						int num;
						sgl_uint32 Alpha;
						num = 4 - nAlphaLeftShift + 12;
						Alpha = (*pSrcPixel16 & AlphaMask);

						while(num < 4)
						{
							Alpha |= Alpha >> num;
							num += num;
						}
						*pPixels |= (Alpha >> 12) << 12;
					}
					*pPixels ^= 0xFF000000;
					pPixels++; pSrcPixel16++;
				}
			}
		}
		else
		{
			if(!nReversedAlpha)
			{
				for(i=0; i<nMapSize; i++)
				{
					*pPixels = (((*pSrcPixel32 & RedMask) >> nRedRightShift) << nRedLeftShift) |
							   (((*pSrcPixel32 & GreenMask) >> nGreenRightShift) << nGreenLeftShift) |
							   (((*pSrcPixel32 & BlueMask) >> nBlueRightShift) << nBlueLeftShift);
					if(nAlphaLeftShift >= 12)
					{
						*pPixels |= ((*pSrcPixel32 & AlphaMask) >> nAlphaRightShift) << nAlphaLeftShift;
					}						   
					else /* We have less than 4 bits of alpha */
					{
						int num;
						sgl_uint32 Alpha;
						num = 4 - nAlphaLeftShift + 12;
						Alpha = (*pSrcPixel32 & AlphaMask);
						while(num < 4)
						{
							Alpha |= Alpha >> num;
							num += num;
						}
						*pPixels |= (Alpha >> 12) << 12;
					}
					pPixels++; pSrcPixel32++;
				}
			}
			else
			{
				for(i=0; i<nMapSize; i++)
				{
					*pPixels = (((*pSrcPixel32 & RedMask) >> nRedRightShift) << nRedLeftShift) |
							   (((*pSrcPixel32 & GreenMask) >> nGreenRightShift) << nGreenLeftShift) |
							   (((*pSrcPixel32 & BlueMask) >> nBlueRightShift) << nBlueLeftShift);
					if(nAlphaLeftShift >= 12)
					{
						*pPixels |= ((*pSrcPixel32 & AlphaMask) >> nAlphaRightShift) << nAlphaLeftShift;
					}						   
					else /* We have less than 4 bits of alpha */
					{
						int num;
						sgl_uint32 Alpha;
						num = 4 - nAlphaLeftShift + 12;
						Alpha = (*pSrcPixel32 & AlphaMask);
						while(num < 4)
						{
							Alpha |= Alpha >> num;
							num += num;
						}
						*pPixels |= (Alpha >> 12) << 12;
					}
					*pPixels ^= 0xFF000000;
					pPixels++; pSrcPixel32++;
				}
			}
		}
	}
}
		
/******************************************************************************
 * Function Name: GenerateLUT  INTERNAL ONLY
 *
 * Inputs       : index, size.
 *
 * Outputs      : None
 *
 * Returns      : None
 *
 * Description  : generate look up tables used for mipmap levels 
 *****************************************************************************/
static void GenerateLUT (int index, int size)
{
	int k, i, j, n, m, num1, num2, num3, nTable1[4], nTable2[4], nTable3[4], ni, nj;
	
	ni = nj = 4;
	nTable1[0] = 0;
	nTable2[0] = 0;
	nTable3[0] = 0;
	
	if (size < 32)  nj = 1;
	if (size < 16)  ni = 1;
	
	GetTable(8, (size<<3), nTable1);
	GetTable(16, (size<<4), nTable2);
	
	for (j = 0; j < nj; j++)
	{
		for (i = 0; i < ni; i++)
		{
			k = (i << 3) + (j << 5);
			n = size << 2;
			m = nTable1[i] + nTable2[j];
			LUTx[index][k] = m;
			LUTx[index][k+1] = m+2;
			LUTx[index][k+2] = m+n;
			LUTx[index][k+3] = m+n+2;
			LUTx[index][k+4] = m+4;
			LUTx[index][k+5] = m+6;
			LUTx[index][k+6] = m+n+4;
			LUTx[index][k+7] = m+n+6;
		}
	}
	
	LUTy[index][0] = 0;
	num1 = num2 = num3 = 1;
	if(size > 32)
	{
		num3 = 4;
		GetTable(32, (size<<5), nTable3);
		if(size > 64)
		{
			num2 = 4;
			GetTable(64, (size<<6), nTable2);
			if(size > 128)
			{
				num1 = 4;
				GetTable(128, (size<<7), nTable1);
			}
		}
	
		for(i=0; i<num1; i++)
		{
			for(j=0; j<num2; j++)
			{
				for(k=0; k<num3; k++)
				{
					m = k + (j << 2) + (i << 4);
					LUTy[index][m] = nTable3[k] + nTable2[j] + nTable1[i];
				}
			}
		}	
	}
}  /* end of GenerateLUT */

/******************************************************************************
 * Function Name: InitLUTs  INTERNAL ONLY
 *
 * Inputs       : None
 *
 * Outputs      : None
 *
 * Returns      : None
 *
 * Description  : initial the look up tables 
 *****************************************************************************/
static void InitLUTs ()
{
	LUTInitialised = TRUE;
	
	GenerateLUT (0, 8);
	GenerateLUT (1, 16);
	GenerateLUT (2, 32);
	GenerateLUT (3, 64);
	GenerateLUT (4, 128);
	GenerateLUT (5, 256);
}	

typedef struct
{
	sgl_uint32 *pMap;
	sgl_uint32 rb;
	sgl_uint32 ga;
	sgl_uint32 buff16;
	sgl_uint32 DestAddress;
	int		   Flag;
	int		   Count;
} LEVEL, *PLEVEL;

/******************************************************************************
 * Function Name: GenerateSmallMipmap555  INTERNAL ONLY
 *
 * Inputs       : nSize, pLevel, *pSrc.
 *
 * Outputs      : None
 *
 * Returns      : None
 *
 * Description  : Automatically generate mipmaps from 555 format level DestinationMap <= 2  
 *				(4x4, and 2x2)  to 1x1 when writing textures into memory.
 *****************************************************************************/
static void	GenerateSmallMipmap555(int nSize, PLEVEL pTopLevel, sgl_uint32 *pSrc)
{
	sgl_uint32		rb, ga,	rb0, ga0, buff16, *pS0, *pS1;
	int				x,  n4;
	PLEVEL pLevel;
	
	int pLUTx[4] = {0, 8, 2, 10};
	n4 = 4;
	if(nSize == 2) n4 = 1;
	
	for (x = 0; x < n4; ++x)
	{
		/* wirte 4 pixels (2 pairs) at a time */
		pS0 = ((sgl_uint32 *) pSrc) + pLUTx[x];
		pS1 = pS0 + nSize;
		pLevel = pTopLevel;				
		
		/* top level map */
		rb = pS0[0] & 0x00FF00FF;
		ga = pS0[0] & 0xFF00FF00;
		rb0 = rb;
		ga0 = ga;
		buff16 = RB_GA_555 (rb,ga);
				
		rb = pS1[0] & 0x00FF00FF;
		ga = pS1[0] & 0xFF00FF00;
		rb0 += rb;
		ga0 += ga;					/* pair 1 */
		pLevel->pMap[0] = (buff16 << 16) | (RB_GA_555 (rb,ga));
				
		rb = pS0[1] & 0x00FF00FF;
		ga = pS0[1] & 0xFF00FF00;
		rb0 += rb;
		ga0 += ga;
		buff16 = RB_GA_555 (rb,ga);
				
		rb = pS1[1] & 0x00FF00FF;
		ga = pS1[1] & 0xFF00FF00;
		rb0 += rb;
		ga0 += ga;							/* pair 2 */
		pLevel->pMap[1] = (buff16 << 16) | (RB_GA_555 (rb,ga));
				
		pLevel->pMap += 2;

		/* second level map */
		pLevel ++;
		rb0 = (rb0 >> 2) & 0x00FF00FF;
		ga0 = (ga0 >> 2) & 0xFF00FF00;

		if(nSize == 4)
		{
			/*    level 2x2   */
			if (pLevel->Flag)
			{
				pLevel->pMap[0] = (pLevel->buff16 << 16) | (RB_GA_555 (rb0,ga0));
				pLevel->pMap ++;
			}
			else
			{
				pLevel->buff16 = RB_GA_555 (rb0,ga0);
			}
			pLevel->Flag ^= 1;
			
			/*    level 1x1   */
			pLevel ++;
			pLevel->rb += rb0;
			pLevel->ga += ga0;
			if (++pLevel->Count == 4)
			{
				rb0 = (pLevel->rb >> 2) & 0x00FF00FF;
				ga0 = (pLevel->ga >> 2) & 0xFF00FF00;
				if(pLevel->DestAddress & 1)
				{
					pLevel->pMap[0] = 0x00000000 | (RB_GA_555 (rb0,ga0));
				}
				else
				{
					pLevel->pMap[0] = ((RB_GA_555 (rb0,ga0)) << 16) | 0x00000000;
				}
			}
		}
		else
		{
			/*    level 1x1   */
			if(pLevel->DestAddress & 1)
			{
				pLevel->pMap[0] = 0x00000000 | (RB_GA_555 (rb0,ga0));
			}
			else
			{
				pLevel->pMap[0] = ((RB_GA_555 (rb0,ga0)) << 16) | 0x00000000;
			}
		}
	}
}

/******************************************************************************
 * Function Name: GenerateSmallMipmap4444  INTERNAL ONLY
 *
 * Inputs       : nSize, pLevel, *pSrc.
 *
 * Outputs      : None
 *
 * Returns      : None
 *
 * Description  : Automatically generate mipmaps from 4444 format level DestinationMap <= 2  
 *				(4x4, and 2x2)  to 1x1 when writing textures into memory.
 *****************************************************************************/
static void	GenerateSmallMipmap4444(int nSize, PLEVEL pTopLevel, sgl_uint32 *pSrc)
{
	sgl_uint32		rb, ga,	rb0, ga0, buff16, *pS0, *pS1;
	int				x,  n4;
	PLEVEL pLevel;
	
	int pLUTx[4] = {0, 8, 2, 10};
	n4 = 4;
	if(nSize == 2) n4 = 1;
	
	for (x = 0; x < n4; ++x)
	{
		/* wirte 4 pixels (2 pairs) at a time */
		pS0 = ((sgl_uint32 *) pSrc) + pLUTx[x];
		pS1 = pS0 + nSize;
		pLevel = pTopLevel;	
		
		/* top level map */
		rb = pS0[0] & 0x00FF00FF;
		ga = pS0[0] & 0xFF00FF00;
		rb0 = rb;
		ga0 = ga >> 2;
		buff16 = RB_GA_4444 (rb,ga);
				
		rb = pS1[0] & 0x00FF00FF;
		ga = pS1[0] & 0xFF00FF00;
		rb0 += rb;
		ga0 += ga >> 2;					/* pair 1 */
		pLevel->pMap[0] = (buff16 << 16) | (RB_GA_4444 (rb,ga));
				
		rb = pS0[1] & 0x00FF00FF;
		ga = pS0[1] & 0xFF00FF00;
		rb0 += rb;
		ga0 += ga >> 2;
		buff16 = RB_GA_4444 (rb,ga);
				
		rb = pS1[1] & 0x00FF00FF;
		ga = pS1[1] & 0xFF00FF00;
		rb0 += rb;
		ga0 += ga >> 2;							/* pair 2 */
		pLevel->pMap[1] = (buff16 << 16) | (RB_GA_4444 (rb,ga));
				
		pLevel->pMap += 2;

		/* second level map */
		pLevel ++;
		rb0 = (rb0 >> 2) & 0x00FF00FF;
		ga0 = ga0 & 0xFF00FF00;

		if(nSize == 4)
		{
			/*    level 2x2   */
			if (pLevel->Flag)
			{
				pLevel->pMap[0] = (pLevel->buff16 << 16) | (RB_GA_4444 (rb0,ga0));
				pLevel->pMap ++;
			}
			else
			{
				pLevel->buff16 = RB_GA_4444 (rb0,ga0);
			}
			pLevel->Flag ^= 1;
			
			/*    level 1x1   */
			pLevel ++;
			pLevel->rb += rb0;
			pLevel->ga += ga0 >> 2;
			if (++pLevel->Count == 4)
			{
				rb0 = (pLevel->rb >> 2) & 0x00FF00FF;
				ga0 = pLevel->ga & 0xFF00FF00;
				if(pLevel->DestAddress & 1)
				{
					pLevel->pMap[0] = 0x00000000 | (RB_GA_4444 (rb0,ga0));
				}
				else
				{
					pLevel->pMap[0] = ((RB_GA_4444 (rb0,ga0)) << 16) | 0x00000000;
				}
			}
		}
		else
		{
			/*    level 1x1   */
			if(pLevel->DestAddress & 1)
			{
				pLevel->pMap[0] = 0x00000000 | (RB_GA_4444 (rb0,ga0));
			}
			else
			{
				pLevel->pMap[0] = ((RB_GA_4444 (rb0,ga0)) << 16) | 0x00000000;
			}
		}
	}
}

/******************************************************************************
 * Function Name: GenerateBigMipmap555  INTERNAL ONLY
 *
 * Inputs       : nSize, pLevel, *pSrc, nDoubleSize, n1024, n8, *pLUTx, *pLUTy.
 *
 * Outputs      : None
 *
 * Returns      : None
 *
 * Description  : Automatically generate mipmaps from 4444 format level DestinationMap > 2  
 *				(8x8 and above)  to 1x1 when writing textures into memory.
 *****************************************************************************/
static void	GenerateBigMipmap555(WHICH_MAP DestinationMap, PLEVEL pTopLevel, sgl_uint32 *pSrc,
							int nDoubleSize, int nSize, int n1024, int n8, 
							sgl_uint32 *pLUTx, sgl_uint32 *pLUTy)
{
	sgl_uint32		rb, ga, rb0, ga0, rb1, ga1, buff16, *pS0, *pS1;
	int				x, y, k, Offset;
	PLEVEL pLevel;
	
	for (y = 0; y < n1024; ++y)
	{
		for (x = 0; x < n8; ++x)
		{
			/* wirte 8 pixels (4 pairs) at a time */
			Offset = pLUTy[y] + pLUTx[x];
			pS0 = ((sgl_uint32 *) pSrc) + Offset;
			pS1 = pS0 + nSize;
					
			/* top level map */
			pLevel = pTopLevel;
			rb = pS0[0] & 0x00FF00FF;
			ga = pS0[0] & 0xFF00FF00;
			rb0 = rb;
			ga0 = ga;
			buff16 = RB_GA_555 (rb,ga);
	
			rb = pS1[0] & 0x00FF00FF;
			ga = pS1[0] & 0xFF00FF00;
			rb0 += rb;
			ga0 += ga;							/* pair 1 */
			pLevel->pMap[0] = (buff16 << 16) | (RB_GA_555 (rb,ga));

			rb = pS0[1] & 0x00FF00FF;
			ga = pS0[1] & 0xFF00FF00;
			rb0 += rb;
			ga0 += ga;
			buff16 = RB_GA_555 (rb,ga);

			rb = pS1[1] & 0x00FF00FF;
			ga = pS1[1] & 0xFF00FF00;
			rb0 += rb;
			ga0 += ga;							/* pair 2 */
			pLevel->pMap[1] = (buff16 << 16) | (RB_GA_555 (rb,ga));
			
			pS0 += nDoubleSize;
			pS1 += nDoubleSize;
			
			rb = pS0[0] & 0x00FF00FF;
			ga = pS0[0] & 0xFF00FF00;
			rb1 = rb;
			ga1 = ga;
			buff16 = RB_GA_555 (rb,ga);

			rb = pS1[0] & 0x00FF00FF;
			ga = pS1[0] & 0xFF00FF00;
			rb1 += rb;
			ga1 += ga;							/* pair 3 */
			pLevel->pMap[2] = (buff16 << 16) | (RB_GA_555 (rb,ga));

			rb = pS0[1] & 0x00FF00FF;
			ga = pS0[1] & 0xFF00FF00;
			rb1 += rb;
			ga1 += ga;
			buff16 = RB_GA_555 (rb,ga);

			rb = pS1[1] & 0x00FF00FF;
			ga = pS1[1] & 0xFF00FF00;
			rb1 += rb;
			ga1 += ga;							/* pair 4 */
			pLevel->pMap[3] = (buff16 << 16) | (RB_GA_555 (rb,ga));
			
			pLevel->pMap += 4;

			/* second level map */
			pLevel ++;
			rb0 = (rb0 >> 2) & 0x00FF00FF;
			ga0 = (ga0 >> 2) & 0xFF00FF00;
			rb1 = (rb1 >> 2) & 0x00FF00FF;
			ga1 = (ga1 >> 2) & 0xFF00FF00;
			
			pLevel->pMap[0] = ((RB_GA_555 (rb0,ga0)) << 16) | (RB_GA_555 (rb1,ga1));
			pLevel->pMap ++;

			/* third level map */
			pLevel ++;
			pLevel->rb += rb0 + rb1;
			pLevel->ga += ga0 + ga1;

			if (++pLevel->Count == 2)
			{
				rb0 = (pLevel->rb >> 2) & 0x00FF00FF;
				ga0 = (pLevel->ga >> 2) & 0xFF00FF00;
				pLevel->rb = 0;
				pLevel->ga = 0;
						
				if (pLevel->Flag)
				{
					pLevel->pMap[0] = (pLevel->buff16 << 16) | (RB_GA_555 (rb0,ga0));
					pLevel->pMap ++;
				}
				else
				{
					pLevel->buff16 = RB_GA_555 (rb0,ga0);
				}

				pLevel->Count = 0;
				pLevel->Flag ^= 1;
			
				/* other levels */
	 			pLevel ++;
				
 				for (k = 1; k < (DestinationMap-2); k++, pLevel++)
				{
					pLevel->rb += rb0;
					pLevel->ga += ga0;

					if (++pLevel->Count == 4)
					{
						rb0 = (pLevel->rb >> 2) & 0x00FF00FF;
						ga0 = (pLevel->ga >> 2) & 0xFF00FF00;
							
						/*    level 1x1   */
						if(k == (DestinationMap-3))
						{
							if(pLevel->DestAddress & 1)
							{
								pLevel->pMap[0] = 0x00000000 | (RB_GA_555 (rb0,ga0));
							}
							else
							{
								pLevel->pMap[0] = ((RB_GA_555 (rb0,ga0)) << 16) | 0x00000000;
							}
							break;
						}
						pLevel->rb = 0;
						pLevel->ga = 0;
						
						if (pLevel->Flag)
						{
							pLevel->pMap[0] = (pLevel->buff16 << 16) | (RB_GA_555 (rb0,ga0));
							pLevel->pMap ++;
						}
						else
						{
							pLevel->buff16 = RB_GA_555 (rb0,ga0);
						}

						pLevel->Count = 0;
						pLevel->Flag ^= 1;
					}
					else
					{
						break;
					}
				}
			}
		}
	}
}

/******************************************************************************
 * Function Name: GenerateBigMipmap4444  INTERNAL ONLY
 *
 * Inputs       : nSize, pLevel, *pSrc, nDoubleSize, n1024, n8, *pLUTx, *pLUTy.
 *
 * Outputs      : None
 *
 * Returns      : None
 *
 * Description  : Automatically generate mipmaps from 4444 format level DestinationMap > 2  
 *				(8x8 and above)  to 1x1 when writing textures into memory.
 *****************************************************************************/
static void	GenerateBigMipmap4444(WHICH_MAP DestinationMap, PLEVEL pTopLevel, sgl_uint32 *pSrc,
							int nDoubleSize, int nSize, int n1024, int n8, 
							sgl_uint32 *pLUTx, sgl_uint32 *pLUTy)
{
	sgl_uint32		rb, ga, rb0, ga0, rb1, ga1, buff16, *pS0, *pS1;
	int				x, y, k, Offset;
	PLEVEL pLevel;
			
	for (y = 0; y < n1024; ++y)
	{
		for (x = 0; x < n8; ++x)
		{
			/* wirte 8 pixels (4 pairs) at a time */
			Offset = pLUTy[y] + pLUTx[x];
			pS0 = ((sgl_uint32 *) pSrc) + Offset;
			pS1 = pS0 + nSize;
						
			/* top level map */
			pLevel = pTopLevel;
			rb = pS0[0] & 0x00FF00FF;
			ga = pS0[0] & 0xFF00FF00;
			rb0 = rb;
			ga0 = ga >> 2;
			buff16 = RB_GA_4444 (rb,ga);
				
			rb = pS1[0] & 0x00FF00FF;
			ga = pS1[0] & 0xFF00FF00;
			rb0 += rb;
			ga0 += ga >> 2;					/* pair 1 */
			pLevel->pMap[0] = (buff16 << 16) | (RB_GA_4444 (rb,ga));
				
			rb = pS0[1] & 0x00FF00FF;
			ga = pS0[1] & 0xFF00FF00;
			rb0 += rb;
			ga0 += ga >> 2;
			buff16 = RB_GA_4444 (rb,ga);
				
			rb = pS1[1] & 0x00FF00FF;
			ga = pS1[1] & 0xFF00FF00;
			rb0 += rb;
			ga0 += ga >> 2;							/* pair 2 */
			pLevel->pMap[1] = (buff16 << 16) | (RB_GA_4444 (rb,ga));
				
			pS0 += nDoubleSize;
			pS1 += nDoubleSize;
			
			rb = pS0[0] & 0x00FF00FF;
			ga = pS0[0] & 0xFF00FF00;
			rb1 = rb;
			ga1 = ga >> 2;
			buff16 = RB_GA_4444 (rb,ga);

			rb = pS1[0] & 0x00FF00FF;
			ga = pS1[0] & 0xFF00FF00;
			rb1 += rb;
			ga1 += ga >> 2;							/* pair 3 */
			pLevel->pMap[2] = (buff16 << 16) | (RB_GA_4444 (rb,ga));

			rb = pS0[1] & 0x00FF00FF;
			ga = pS0[1] & 0xFF00FF00;
			rb1 += rb;
			ga1 += ga >> 2;
			buff16 = RB_GA_4444 (rb,ga);

			rb = pS1[1] & 0x00FF00FF;
			ga = pS1[1] & 0xFF00FF00;
			rb1 += rb;
			ga1 += ga >> 2;							/* pair 4 */
			pLevel->pMap[3] = (buff16 << 16) | (RB_GA_4444 (rb,ga));
			
			pLevel->pMap += 4;

			/* second level map */
			pLevel ++;
			rb0 = (rb0 >> 2) & 0x00FF00FF;
			ga0 = ga0 & 0xFF00FF00;
			rb1 = (rb1 >> 2) & 0x00FF00FF;
			ga1 = ga1 & 0xFF00FF00;
				
			pLevel->pMap[0] = ((RB_GA_4444 (rb0,ga0)) << 16) | (RB_GA_4444 (rb1,ga1));
			pLevel->pMap ++;

			/* third level map */
			pLevel ++;
			pLevel->rb += rb0 + rb1;
			pLevel->ga += (ga0 >> 2) + (ga1 >> 2);

			if (++pLevel->Count == 2)
			{
				rb0 = (pLevel->rb >> 2) & 0x00FF00FF;
				ga0 = pLevel->ga & 0xFF00FF00;
				pLevel->rb = 0;
				pLevel->ga = 0;
					
				if (pLevel->Flag)
				{
					pLevel->pMap[0] = (pLevel->buff16 << 16) | (RB_GA_4444 (rb0,ga0));
					pLevel->pMap ++;
				}
				else
				{
					pLevel->buff16 = RB_GA_4444 (rb0,ga0);
				}

				pLevel->Count = 0;
				pLevel->Flag ^= 1;
			
				/* other levels */
				pLevel ++;
		
				for (k = 1; k < (DestinationMap-2); k++, pLevel++)
				{
					pLevel->rb += rb0;
					pLevel->ga += ga0 >> 2;

					if (++pLevel->Count == 4)
					{
						rb0 = (pLevel->rb >> 2) & 0x00FF00FF;
						ga0 = pLevel->ga & 0xFF00FF00;
						
						/*    level 1x1   */
						if(k == (DestinationMap-3))
						{
							if(pLevel->DestAddress & 1)
							{
								pLevel->pMap[0] = 0x00000000 | (RB_GA_4444 (rb0,ga0));
							}
							else
							{
								pLevel->pMap[0] = ((RB_GA_4444 (rb0,ga0)) << 16) | 0x00000000;
							}
							break;
						}
						pLevel->rb = 0;
						pLevel->ga = 0;
					
						if (pLevel->Flag)
						{
							pLevel->pMap[0] = (pLevel->buff16 << 16) | (RB_GA_4444 (rb0,ga0));
							pLevel->pMap ++;
						}
						else
						{
							pLevel->buff16 = RB_GA_4444 (rb0,ga0);
						}

						pLevel->Count = 0;
						pLevel->Flag ^= 1;
					}
					else
					{
						break;
					}
				}
			}
		}
	}
}

/******************************************************************************
 * Function Name: AutoMipmap
 *
 * Inputs       : hTexHeap, hTex, DestinationMap, pSource.
 *
 * Outputs      : None
 *
 * Returns      : error
 *
 * Description  : Automatically generate mipmaps from level DestinationMap
 *				  to 1x1 when writing textures into memory.
 *				  Non-Zero return indicates faliure.
 *****************************************************************************/
PVROSERR CALL_CONV	AutoMipmap(	HTEXHEAP hTexHeap, 
								HTEXTURE hTex, 
								WHICH_MAP DestinationMap, 
								PTEXTURESOURCE pSource)
{
	int 		k, nDoubleSize, nSize, n1024, n8, nEnumType, nColourKey, nReversedAlpha; 
	sgl_uint32	*pSrc, *pTexMem, TextureAddress, KeyColour, *pLUTx, *pLUTy;
	sgl_bool bMalloced;
	LEVEL Level[9];
	PLEVEL pLevel;
	TPRIVATEDATA *pTPD;
	PTEXTUREFORMAT pTFormat;

	pTPD = hTex->pPrivateData;
	ASSERT(hTex != NULL);
	ASSERT(pSource != NULL);

	nReversedAlpha = 0; 
	pTFormat = pTPD->pTextureSpec->pFormat;

	KeyColour = 0;
	nColourKey = 0;
	bMalloced = FALSE;
	nEnumType = 10;
	/*calculate the texture address (relative to the texture base!)*/
	TextureAddress=(hTex->TSPTextureControlWord) & 0x00ffffff;
	pTexMem = (sgl_uint32 *)(hTexHeap->pTextureMemory);
	
	if((DestinationMap > MIPMAP_LEVEL_8) || (DestinationMap < MIPMAP_LEVEL_1))
	{
		PVROSPrintf("TAPI: Bad Destination Map level.\n");
		return (PVROS_DODGY);
	}
	
	if (!LUTInitialised)
	{
		InitLUTs ();
	}
	switch (DestinationMap)
	{
		/* 2x2 */
		case MIPMAP_LEVEL_1:
		{
			nSize = 2;
			break;
		}
		/* 4x4 */
		case MIPMAP_LEVEL_2:
		{
			nSize = 4;
			break;
		}
		/* 8x8 */
		case MIPMAP_LEVEL_3:
		{
			nDoubleSize = 16;
			nSize = 8;
			n1024 = 1;
			n8 = 8;
			pLUTx = LUTx[0];
			pLUTy = LUTy[0];
			break;
		}
		/* 16x16 */
		case MIPMAP_LEVEL_4:
		{
			nDoubleSize = 32;
			nSize = 16;
			n1024 = 1;
			n8 = 32;
			pLUTx = LUTx[1];
			pLUTy = LUTy[1];
			break;
		}
		/* 32x32 */
		case MIPMAP_LEVEL_5:
		{
			nDoubleSize = 64;
			nSize = 32;
			n1024 = 1;
			n8 = 128;
			pLUTx = LUTx[2];
			pLUTy = LUTy[2];
			break;
		}
		/* 64x64 */
		case MIPMAP_LEVEL_6:
		{
			nDoubleSize = 128;
			nSize = 64;
			n1024 = 4;
			n8 = 128;
			pLUTx = LUTx[3];
			pLUTy = LUTy[3];
			break;
		}
		/* 128x128 */
		case MIPMAP_LEVEL_7:
		{
			nDoubleSize = 256;
			nSize = 128;
			n1024 = 16;
			n8 = 128;
			pLUTx = LUTx[4];
			pLUTy = LUTy[4];
			break;
		}
		/* 256x256 */
		case MIPMAP_LEVEL_8:
		{
			nDoubleSize = 512;
			nSize = 256;
			n1024 = 64;
			n8 = 128;
			pLUTx = LUTx[5];
			pLUTy = LUTy[5];
			break;
		}
	}

	memset (Level, 0, sizeof (LEVEL) * 9);

	for (k = 0; k < DestinationMap; ++k)
	{
		int mLevel = DestinationMap - k - 1;
		
		Level[mLevel].DestAddress = TextureAddress + EXTMapOffset[k]; 
		Level[mLevel].pMap = pTexMem + (Level[mLevel].DestAddress >> 1);
		TextureAddress ^= BIG_BANK;
	}
	
	if((pSource->pFormat->Flags & TF_OGLALPHA) != (pTFormat->Flags & TF_OGLALPHA))
	{
		/* reversed alpha */
		nReversedAlpha = 1;
	}
	if(pSource->pFormat->Flags & TF_COLOURKEY)
	{
		nColourKey = 1;
		KeyColour = pSource->pFormat->KeyColour;
		nReversedAlpha = 0;
	}
	if(pSource->pFormat->BitDepth == 16)
	{
		if((pSource->pFormat->rgbyuv.rgb.RedMask == 0x00000F00) &&
		    (pSource->pFormat->rgbyuv.rgb.GreenMask == 0x000000F0) &&
		    (pSource->pFormat->rgbyuv.rgb.BlueMask == 0x0000000F) &&
			(pSource->pFormat->AlphaMask == 0x0000F000) &&
			(pSource->pFormat->Flags & TF_TRANSLUCENT))
		{
			/* 4444  */
			nEnumType = 1;	
		}
		else if((pSource->pFormat->rgbyuv.rgb.RedMask == 0x00007C00) &&
			   	(pSource->pFormat->rgbyuv.rgb.GreenMask == 0x000003E0) &&
			  	(pSource->pFormat->rgbyuv.rgb.BlueMask == 0x0000001F) &&
				!(pSource->pFormat->Flags & TF_TRANSLUCENT))
		{
			/* 555 */
			nEnumType = 2;
		}
		else if((pSource->pFormat->rgbyuv.rgb.RedMask == 0x00007C00) &&
		   	   (pSource->pFormat->rgbyuv.rgb.GreenMask == 0x000003E0) &&
		  	   (pSource->pFormat->rgbyuv.rgb.BlueMask == 0x0000001F) &&
		   	   (pSource->pFormat->AlphaMask == 0x00008000) &&
			   (pSource->pFormat->Flags & TF_TRANSLUCENT))
		{
			/* 1555 */
			nEnumType = 3;	
		}
		else if((pSource->pFormat->rgbyuv.rgb.RedMask == 0x0000F800) &&
		   	   (pSource->pFormat->rgbyuv.rgb.GreenMask == 0x000007E0) &&
		  	   (pSource->pFormat->rgbyuv.rgb.BlueMask == 0x0000001F) &&
			   !(pSource->pFormat->Flags & TF_TRANSLUCENT)) 
		{
			/* 565 */
			nEnumType = 4;
		}

		pSrc = PVROSMalloc(nSize*nSize*4);
		if(pSrc == NULL)
		{
			PVROSPrintf("TAPI: Out of Memory.\n");
			return (PVROS_DODGY);
		}
		bMalloced = TRUE;
		if(nEnumType != 10)
		{
			ConvertTextureFormats16To8888(nSize*nSize, nEnumType, &pSrc, pSource->pPixels,
										 nColourKey, KeyColour, nReversedAlpha);
		}
		else
		{
			int nTranslucent = 0;
			if((pSource->pFormat->Flags & TF_TRANSLUCENT) && !nColourKey )
			{
				nTranslucent = 1;
			}
			ConvertUnknownFormatsTo8888(nSize*nSize, nTranslucent, &pSrc,
										pSource, nColourKey, KeyColour, nReversedAlpha);
		}
	}
	else if(pSource->pFormat->BitDepth == 32)
	{
		if((pSource->pFormat->rgbyuv.rgb.BlueMask == 0x00FF0000) &&
			(pSource->pFormat->rgbyuv.rgb.GreenMask == 0x0000FF00) &&
			(pSource->pFormat->rgbyuv.rgb.RedMask == 0x000000FF))
		{ 
			if((pSource->pFormat->Flags & TF_TRANSLUCENT) && 
			 	(pSource->pFormat->AlphaMask != 0xFF000000))
			{
				PVROSPrintf("TAPI: Bad AlphaMask.\n");
				return (PVROS_DODGY);
			}
			
			/* 888 or 8888(ABGR) formats */
			pSrc = (sgl_uint32 *)pSource->pPixels;
		}
		else
		{
			/* not in 888 or 8888(ABGR) formats */	
			if((pSource->pFormat->rgbyuv.rgb.RedMask == 0x00FF0000) &&
			   (pSource->pFormat->rgbyuv.rgb.GreenMask == 0x0000FF00) &&
			   (pSource->pFormat->rgbyuv.rgb.BlueMask == 0x000000FF))
			{
				if((pSource->pFormat->Flags & TF_TRANSLUCENT) && 
			 		(pSource->pFormat->AlphaMask != 0xFF000000))
				{
					nEnumType = 10;
				}
				else
				{
					/* 888 or 8888(ARGB)  */				
					nEnumType = 5;	
				}
			}
			
			pSrc = PVROSMalloc(nSize*nSize*4);
			if(pSrc == NULL)
			{
				PVROSPrintf("TAPI: Out of Memory.\n");
				return (PVROS_DODGY);
			}
			bMalloced = TRUE;
			if(nEnumType != 10)
			{
				ConvertTextureFormats32To8888(nSize*nSize, &pSrc, pSource->pPixels,
											 nColourKey, KeyColour, nReversedAlpha);
			}
			else
			{
				int nTranslucent = 0;
				if((pSource->pFormat->Flags & TF_TRANSLUCENT) && !nColourKey )
				{
					nTranslucent = 1;
				}
				ConvertUnknownFormatsTo8888(nSize*nSize, nTranslucent, &pSrc,
											pSource, nColourKey, KeyColour, nReversedAlpha);
			}
		}
	}
	
	SynchroniseTexMemAccess (hTexHeap, TRUE);

	pLevel = Level;
	/* 8888 format into 4444 */
	if( (pSource->pFormat->Flags & TF_TRANSLUCENT) || (pSource->pFormat->Flags & TF_COLOURKEY))
	{
		if(DestinationMap <= MIPMAP_LEVEL_2)
		{
			GenerateSmallMipmap4444(nSize, pLevel, pSrc);
		}
		else
		{
			GenerateBigMipmap4444(DestinationMap, pLevel, pSrc, nDoubleSize, nSize,
									n1024, n8, pLUTx, pLUTy);
		}
	}
	else	/* 888 format into 555 */
	{
		if(DestinationMap <= MIPMAP_LEVEL_2)
		{
			GenerateSmallMipmap555(nSize, pLevel, pSrc);
		}
		else
		{
			GenerateBigMipmap555(DestinationMap, pLevel, pSrc, nDoubleSize, nSize,
								n1024, n8, pLUTx, pLUTy);
		}
	}
	SynchroniseTexMemAccess (hTexHeap, FALSE);
	
	if(bMalloced)
	{
		PVROSFree(pSrc);
	}
	return(PVROS_GROOVY);	
	
} /* end of AutoMipmap */
/* end of texapi.c */


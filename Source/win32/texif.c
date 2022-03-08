/******************************************************************************
 * Name         : texif.c
 * Title        : PVROS Texture Interface
 * Author       : Michael Green
 * Created      : 25/04/1997
 *
 * Copyright	: 1995-2022 Imagination Technologies (c)
 * License		: MIT
 *
 * Description  : Controls the initialistion of texture memory
 *
 * 
 * Platform     : ANSI
 * $Log: texif.c,v $
 * Revision 1.24  1997/09/24  09:51:45  gdc
 * changed HWRdavalFile's to a new function which has a revised priority
 * of where to get values
 *
 * Revision 1.23  1997/08/21  11:09:43  sks
 * Vertex fog texture creation now correct.
 *
 * Revision 1.22  1997/08/19  16:16:39  sks
 * Removed some useless stuff. Added an attempt to create the per
 * vertex fog texture.
 *
 * Revision 1.21  1997/08/18  16:41:28  sks
 * Started adding per vertex fog texture creation
 *
 * Revision 1.20  1997/08/02  16:53:26  mjg
 * White pixel used for non-textures translucency was 0xFFFFFFFF which was
 * fine for 555, but now with 1555 the top bit is used for alpha and so
 * 0xFFFFFFFF was itself translucent! Now uses 0x7FFF7FFF.
 *
 * Revision 1.19  1997/07/24  19:25:15  mjg
 * Reverted the MIDAS4 OutOfCachePlanes calculation to v1.17 way, moved
 * reading the TSPParamSize value, so it can be used in InitTextureMemory
 * and deleted some spaces.
 *
 * Revision 1.18  1997/07/24  09:53:33  mjg
 * Fix to alter the PCX1 OutOfCachePlanes size to be TSPParamSize.
 *
 * Revision 1.17  1997/07/18  14:07:55  gdc
 * typo
 *
 * Revision 1.16  1997/07/18  14:07:39  gdc
 * added a MODULE_ID
 *
 * Revision 1.15  1997/07/18  14:03:28  gdc
 * replaced pvrosprintf's with DPF's
 *
 * Revision 1.14  1997/07/17  13:06:41  gdc
 * device type now set before setup overflow area called which uses it
 *
 * Revision 1.13  1997/07/14  13:36:09  gdc
 * changed hard coded OOCP size, so that right number is used
 * for PCX1 and PCX2, and fixed a crash when 128k TSP param size
 * used
 *
 * Revision 1.12  1997/06/24  18:05:40  gdc
 * chnaged 10000 to OOCP - avoids a magic number problem
 *
 * Revision 1.11  1997/06/23  22:01:53  jop
 * Farted around with base tex mem configurations. Think this one should
 * do now. Only problem is that we can't access the TSP param's buddy
 *
 * Revision 1.10  1997/06/22  17:41:26  jop
 * Added adaptive TSP parameter stuff
 *
 * Revision 1.9  1997/06/09  17:14:04  sxy
 * added an API AutoMipmap.
 *
 * Revision 1.8  1997/05/22  23:29:28  jop
 * Added hardware details to texture jeheap strutcures
 *
 * Revision 1.7  1997/05/13  10:49:39  sxy
 * added a new API TextureCopy to copy preprocessed textures into memory.
 *
 * Revision 1.6  1997/05/02  07:31:13  erf
 * Removed UINT32's etc and replaced with sgl_uint32's etc.
 *
 * Revision 1.5  1997/05/01  16:48:41  mjg
 * Renamed the functions to get the texture memory info and added then to
 * the interface.
 *
 * Revision 1.4  1997/05/01  02:44:39  jop
 * Did some typing
 *
 * Revision 1.3  1997/04/30  19:35:09  mjg
 * First stab at functions to do the job previously performed by
 * the sgl_get_free_texture_mem... functions.
 *
 * Revision 1.2  1997/04/29  15:30:04  erf
 * Changed TMalloc() and TFree().
 *
 * Revision 1.1  1997/04/28  21:15:25  mjg
 * Initial revision
 * *
 *****************************************************************************/

#define MODULE_ID MODID_TEXIF

#include <windows.h>
#define API_TYPESONLY
#include "sgl.h"
#undef API_TYPESONLY
#include "pvrosapi.h"
#include "sgl_defs.h"
#include "texapi.h"
#include "tmalloc.h"
#include "texapip.h"
#include "error.h"
#include "hwtexas.h" /* For TEX_PARAM_SIZE */

#define NO_OF_TEX_HEAPS 4 /* Pick a number ... (VxD uses 4) */

#pragma data_seg(".onetime")

typedef struct texheapstruct
{
	TEXTUREHEAP	TexHeap;
	MNODE MemoryRoot;
	int RefCounter;
	TEXAPI_IF TexIF;
//	MNODE_BLOCK MemBlock[8];
	MNODE_BLOCK MemBlock[10];
} TexHeapStruct;

static TexHeapStruct TexHeaps[NO_OF_TEX_HEAPS] = {0}; 

#pragma data_seg()


void ResetTexHeap(HTEXHEAP hTexHeap)
{
	int i;
		
	for(i=0; i<NO_OF_TEX_HEAPS; i++)
	{
		if(&(TexHeaps[i].TexHeap) == hTexHeap)
		{
			TexHeaps[i].TexHeap.hDeviceID = 0;
   		}
	}
}

/****************************************************************************/

TexHeapStruct* FindOldTexHeap(HTEXHEAP hTexHeap)
{
	int i;
		
	for(i=0; i<NO_OF_TEX_HEAPS; i++)
	{
		if(&(TexHeaps[i].TexHeap) == hTexHeap)
		{
			return(&TexHeaps[i]);
		}
	}
	return (NULL);
}

/*****************************************************************************/

TexHeapStruct* FindTexHeap(HDEVICE hDeviceID)
{
	int i;
		
	for(i=0; i<NO_OF_TEX_HEAPS; i++)
	{
		if(TexHeaps[i].TexHeap.hDeviceID == hDeviceID)
		{
			return(&TexHeaps[i]);
		}
	}

		
	for(i=0; i<NO_OF_TEX_HEAPS; i++)
	{
		if (TexHeaps[i].TexHeap.hDeviceID == 0)
		{
			return(&TexHeaps[i]);
		}
	}	
	return(NULL);
}

/*****************************************************************************/

void SetupOverflowArea (TexHeapStruct* pt)
{
	/*
		this function sets up the texture memory
		as below:

		LOW texture memory
		_____________________
		| TSP prm | ISP ooc | 160000 bytes
		| TSP prm | t/pixel |	   4 bytes
		| TSP prm | free!!  | TSP param size - 160004 bytes
		| free!!  | free!!  | remainder
		---------------------
		HIGH texture memory
	*/

	sgl_uint32 AllExceptTSPParams;
	sgl_uint32 Size = pt->TexHeap.TexParamSize;
	sgl_uint32 OutOffCachePlanes;
	sgl_uint32 AllChunks, Chunk, k;
	HTEXHEAP hTexHeap = &pt->TexHeap;
	sgl_uint32 TextureAddress;
	sgl_uint32 *VertFogTexAddr;
	sgl_uint32 x,y;
	sgl_uint32 OffsetX,OffsetY,totaloff;

	
	#define BODY_BANK_A	0
	#define BODY_BANK_B	1
	#define TRANS_PIXEL_BANK_A	2
	#define TRANS_PIXEL_BANK_B	3
	#define VERT_FOG_TEX_BANK_A 4
	#define VERT_FOG_TEX_BANK_B 5
	#define OUT_OF_CACHE_BANK_A	6
	#define OUT_OF_CACHE_BANK_B	7
	#define TSP_DATA_BANK_A	8
	#define TSP_DATA_BANK_B	9
	
	/* Set up special textures for cache overflow and TSP parameters */
	if(pt->TexHeap.DeviceType==MIDAS4)
	{
		/* DONT USE PVRLIMS.H for these values as they both will
		** end up compiled to be the same dependingf on if its 
		** a pcx1 or pcx2 build
		*/
		OutOffCachePlanes = 3730 - 1024;
	}
	else
	{
		OutOffCachePlanes = 11000 - 1024;
	}
	
	/* force the TSP param space to be big enough for OOCP */
	if(Size < OutOffCachePlanes*4*4 + 4 + 32*32*2)
	{
		Size = pt->TexHeap.TexParamSize = OutOffCachePlanes*4*4 + 4 + 32*32*2;
	} 

	AllExceptTSPParams = ((4*1024*1024) >> 1) - Size;

	TMalloc (AllExceptTSPParams, hTexHeap, &pt->MemBlock[BODY_BANK_A]);
	ASSERT (pt->MemBlock[BODY_BANK_A].Status != -1);		
	TMalloc (AllExceptTSPParams, hTexHeap, &pt->MemBlock[BODY_BANK_B]);
	ASSERT (pt->MemBlock[BODY_BANK_B].Status != -1);		

	/* the part that is now left is the cache overflow area/tsp param block */
	/* allocate the part that is TSP params but not cache overflow */
	/* and leave a tiny gap for the tans opaque pixel */


	Chunk = 0;
	AllChunks = Size - OutOffCachePlanes*4*4 - 4 - 32*32*2;
	

	while (AllChunks)
	{
		sgl_uint32 ThisChunk;
		
	#if 0

		/* this code slices the remainder up into chunks so
		   that applications can make use of the TSP data block's
		   buddy. At the moment this prevents us from extending
		   (adaptively) more than the smallest block's size
		*/

		if (AllChunks > 256*256*2)
		{
			ThisChunk = 256*256*2;
		}
		else if (AllChunks > 128*128*2)
		{
			ThisChunk = 128*128*2;
		}
		else if (AllChunks > 64*64*2)
		{
			ThisChunk = 64*64*2;
		}
		else if (AllChunks > 32*32*2)
		{
			ThisChunk = 32*32*2;
		}
		else if (AllChunks > 0)
		{
			ThisChunk = AllChunks;
		}
		else
		{
			break;
		}
		
	#else
	
		ThisChunk = AllChunks;
	
	#endif
	
		AllChunks -= ThisChunk;
		
		TMalloc (ThisChunk, hTexHeap, &pt->MemBlock[Chunk+TSP_DATA_BANK_A]);
		ASSERT (pt->MemBlock[Chunk+TSP_DATA_BANK_A].Status != -1);		
		TMalloc (ThisChunk, hTexHeap, &pt->MemBlock[Chunk+TSP_DATA_BANK_B]);
		ASSERT (pt->MemBlock[Chunk+TSP_DATA_BANK_B].Status != -1);		

		pt->MemBlock[Chunk+TSP_DATA_BANK_A].MNode->BlockType = TSP_PARAM_BLOCK;
		pt->MemBlock[Chunk+TSP_DATA_BANK_B].MNode->BlockType = TSP_PARAM_BLOCK;
		
		Chunk += 2;
	}
	
	/* allocate a pixel's worth in both banks */

	TMalloc (4, hTexHeap, &pt->MemBlock[TRANS_PIXEL_BANK_A]);
	ASSERT (pt->MemBlock[TRANS_PIXEL_BANK_A].Status != -1);		
	TMalloc (4, hTexHeap, &pt->MemBlock[TRANS_PIXEL_BANK_B]);
	ASSERT (pt->MemBlock[TRANS_PIXEL_BANK_B].Status != -1);		

	pt->MemBlock[TRANS_PIXEL_BANK_A].MNode->BlockType = TSP_PARAM_BLOCK;
	pt->MemBlock[TRANS_PIXEL_BANK_B].MNode->BlockType = TSP_PARAM_BLOCK;

	/* use bank B for special pixel */
	/* from bytes to 16Bit words*/
	TextureAddress = (pt->MemBlock[TRANS_PIXEL_BANK_B].MNode->MemoryAddress >> 1) | BIG_BANK; 

	/* Write Opaque white pixels */
	*((sgl_uint32*) hTexHeap->pTextureMemory + (TextureAddress >> 1)) = 0x7fff7fff; 

	hTexHeap->TranslucentControlWord = TextureAddress | MASK_8_16_MAPS;
	

	/* Create and store the translucent texture for fog per vertex */

	TMalloc (32*32*2, hTexHeap, &pt->MemBlock[VERT_FOG_TEX_BANK_A]);
	ASSERT (pt->MemBlock[VERT_FOG_TEX_BANK_A].Status != -1);		
	TMalloc (32*32*2, hTexHeap, &pt->MemBlock[VERT_FOG_TEX_BANK_B]);
	ASSERT (pt->MemBlock[VERT_FOG_TEX_BANK_B].Status != -1);		

	pt->MemBlock[VERT_FOG_TEX_BANK_A].MNode->BlockType = TSP_PARAM_BLOCK;
	pt->MemBlock[VERT_FOG_TEX_BANK_B].MNode->BlockType = TSP_PARAM_BLOCK;

	/* use bank B for texture */
	TextureAddress = (pt->MemBlock[VERT_FOG_TEX_BANK_B].MNode->MemoryAddress >> 1) | BIG_BANK; 

	VertFogTexAddr = ((sgl_uint32*) (hTexHeap->pTextureMemory) + (TextureAddress >> 1));

	OffsetX = 0;
	OffsetY = 0;

	for (y=0; y<32; y++)
	{
		for (x=0; x<32; x++)
		{
			totaloff = (OffsetX | OffsetY) >> 1;
			VertFogTexAddr[totaloff] = 0x0FFF0FFF | ((15-(x>>1))<<12) | ((15-(x>>1))<<28);
			OffsetX += 0x5556;
			OffsetX &= 0xAAAA;
		}			
		OffsetX  = 0;
		OffsetY += 0x2AAB;
		OffsetY &= 0x5555;
	}

	hTexHeap->VertexFogControlWord = TextureAddress | MASK_8_16_MAPS | MASK_4444_555;

	/* allocate the part that is cache overflow. This will always be allocated in
	   banks A and B, so don't deallocate again */

	TMalloc(OutOffCachePlanes*4*4, hTexHeap, &pt->MemBlock[OUT_OF_CACHE_BANK_A]);
	ASSERT (pt->MemBlock[OUT_OF_CACHE_BANK_A].Status != -1);		
	TMalloc(OutOffCachePlanes*4*4, hTexHeap, &pt->MemBlock[OUT_OF_CACHE_BANK_B]);
	ASSERT (pt->MemBlock[OUT_OF_CACHE_BANK_B].Status != -1);		

	pt->MemBlock[OUT_OF_CACHE_BANK_A].MNode->BlockType = TSP_PARAM_BLOCK;
	pt->MemBlock[OUT_OF_CACHE_BANK_B].MNode->BlockType = ISP_OUT_OF_CACHE_BLOCK;

	/* deallocate the bank B part of the remainder */

	for (k = 0; k < Chunk; k += 2)
	{
		TFree (&pt->MemBlock[k+TSP_DATA_BANK_B], hTexHeap);
	}

	/* deallocate the rest of the texture memory */

	TFree (&pt->MemBlock[BODY_BANK_A], hTexHeap);
	TFree (&pt->MemBlock[BODY_BANK_B], hTexHeap);

	PVROSSetTSPHighWaterMark (hTexHeap, Size, FALSE);
}

/*****************************************************************************/

#if 0 
sgl_uint32 SetupTransPixel(HTEXHEAP hTexHeap)
{
	sgl_uint32	TextureControl,TextureAddress;
	MNODE_BLOCK MemBlock;

	/*try and allocate the space*/

	/* 4 bytes because the base address of a texture has to be two word aligned
	 */
	TMalloc(4, hTexHeap, &MemBlock);

#if DEBUG
	if(MemBlock.Status==-1)
	{
		/*this condition should not happen! */
		ASSERT(FALSE);
	}
#endif

	/*calculate the texture address*/

	TextureAddress=MemBlock.MNode->MemoryAddress>>1; 
	/* from bytes to 16Bit words*/
	if(MemBlock.Status==B_BANK)
		TextureAddress|=BIG_BANK;

	/* Write Opaque white pixels */
	*((sgl_uint32*)hTexHeap->pTextureMemory + (TextureAddress>>1)) = 0xffffffff; 

	TextureControl = TextureAddress | MASK_8_16_MAPS;

	return(TextureControl);
}
#endif

/***********************************************************************/
/* Internals for sgl_get_free_texture_mem */

sgl_uint32 CALL_CONV TextureGetFreeMemory(HTEXHEAP hTexHeap)
{
	return (((MNODE*)hTexHeap->pMemoryRoot)->MemoryLeft);	
}

/******************************************************************************
 * Function Name: AddBuddyInfo
 *
 * Inputs       : pInfo: pointer to the sgl_texture_mem_info structure.
 *				  u32Size: size in bytes of the free buddy block.
 * Outputs      : -
 * Returns      : -
 * Globals Used : -
 *
 * Description  : Increments the info counter for the texture type
 *				  corresponding to the given size.
 *****************************************************************************/

static void AddBuddyInfo (sgl_texture_mem_info *pInfo, sgl_uint32 u32Size)
{
	switch (u32Size)
	{
		case 32*32*1: pInfo->free32_8bit++; break;
		case 64*64*1: pInfo->free64_8bit++; break;
		case 128*128*1: pInfo->free128_8bit++; break;
		case 256*256*1: pInfo->free256_8bit++; break;

		case 32*32*2: pInfo->free32_16bit++; break;
		case 64*64*2: pInfo->free64_16bit++; break;
		case 128*128*2: pInfo->free128_16bit++; break;
		case 256*256*2: pInfo->free256_16bit++; break;

		default:
		{
			if (u32Size > 256*256*2)
			{
				pInfo->free256_16bit_mipmap++;
			}
			else if (u32Size > 128*128*2)
			{
				pInfo->free128_16bit_mipmap++;
			}
			else if (u32Size > 64*64*2)
			{
				pInfo->free64_16bit_mipmap++;
			}
			else if (u32Size > 32*32*2)
			{
				pInfo->free32_16bit_mipmap++;
			}
		}

	} /* switch (u32Size) */

} /* AddBuddyInfo */

/***********************************************************************/
/* Internals for sgl_get_free_texture_mem_info */

void CALL_CONV TextureGetFreeMemoryInfo( HTEXHEAP hTexHeap,
									  	 sgl_texture_mem_info *pInfo )
{
	MNODE *MemWalk;

	if (pInfo == NULL) 
	{
		return;
	}

	/*
	// BUDDY MEMORY
	*/

	/*
	// Note: If we add any new members to the structure we must read
	// pInfo->u32StructSize, and not overwrite application memory beyond the
	// size it has allocated for the structure.
	*/
	pInfo->free32_8bit = 0;
	pInfo->free64_8bit = 0;
	pInfo->free128_8bit = 0;
	pInfo->free256_8bit = 0;

	pInfo->free32_16bit = 0;
	pInfo->free64_16bit = 0;
	pInfo->free128_16bit = 0;
	pInfo->free256_16bit = 0;

	pInfo->free32_16bit_mipmap = 0;
	pInfo->free64_16bit_mipmap = 0;
	pInfo->free128_16bit_mipmap = 0;
	pInfo->free256_16bit_mipmap = 0;

	if ( (((MNODE*)hTexHeap->pMemoryRoot)->UsedStatus == A_BANK) || 
		 (((MNODE*)hTexHeap->pMemoryRoot)->UsedStatus == B_BANK) )
	{
		AddBuddyInfo(pInfo,((MNODE*)hTexHeap->pMemoryRoot)->BlockSize);
	}

	MemWalk = hTexHeap->pMemoryRoot;

	while (MemWalk->NextBuddy != NULL)
	{
		MemWalk = MemWalk->NextBuddy;

		AddBuddyInfo(pInfo,MemWalk->BlockSize);
		
		ASSERT(MemWalk->PrevBuddy->NextBuddy == MemWalk);

		if (MemWalk->NextBuddy != NULL)
		{
			ASSERT(MemWalk->NextBuddy->PrevBuddy == MemWalk);
		}     
	}

	/*
	// NON-BUDDY MEMORY
	*/

	pInfo->u32OtherFreeMem = 0;
	pInfo->u32LargestOtherFreeMem = 0;

	if ( ((MNODE*)hTexHeap->pMemoryRoot)->UsedStatus == 0 )
	{
		pInfo->u32OtherFreeMem = (((MNODE*)hTexHeap->pMemoryRoot)->BlockSize) << 1;
		pInfo->u32LargestOtherFreeMem = ((MNODE*)hTexHeap->pMemoryRoot)->BlockSize;
	}

	MemWalk = hTexHeap->pMemoryRoot;

	while (MemWalk->NextFree != NULL)
	{
		MemWalk = MemWalk->NextFree;

		pInfo->u32OtherFreeMem += MemWalk->BlockSize*2;

		if (MemWalk->BlockSize > pInfo->u32LargestOtherFreeMem)
		{
			pInfo->u32LargestOtherFreeMem = MemWalk->BlockSize;
		}

		/*
		// Ensure the 'free' block has not been used.
		*/
		ASSERT(MemWalk->UsedStatus == 0);

		ASSERT(MemWalk->PrevFree->NextFree == MemWalk);

		if (MemWalk->NextFree != NULL)
		{
			ASSERT(MemWalk->NextFree->PrevFree == MemWalk);
		}

	} /* while (MemWalk->NextFree != NULL) */

}

/*****************************************************************************/

sgl_uint32 CALL_CONV PVROSGetTexInterface (HDEVICE hDeviceID, void **ppIF)
{
	TexHeapStruct* pt;

	pt = FindTexHeap(hDeviceID);

	if (pt == NULL)
	{	
		DPF((DBG_ERROR,"Couldn't find texture heap"));
	}
	else
	{
		pt->TexIF.hTexHeap = &pt->TexHeap;
		
		pt->TexIF.pfnTextureEnumerateFormats	= (void *) TextureEnumerateFormats ;
		pt->TexIF.pfnTextureCreate				= (void *) TextureCreate;
		pt->TexIF.pfnTextureLoad				= (void *) TextureLoad;
		pt->TexIF.pfnTextureCopy				= (void *) TextureCopy;
		pt->TexIF.pfnAutoMipmap					= (void *) AutoMipmap;
	 	pt->TexIF.pfnTextureFree				= (void *) TextureFree;
		pt->TexIF.pfnTextureGetFreeMemory	 	= (void *) TextureGetFreeMemory;
		pt->TexIF.pfnTextureGetFreeMemoryInfo 	= (void *) TextureGetFreeMemoryInfo;
				
		*ppIF = (void *) &pt->TexIF;
		
		return (sizeof (pt->TexIF));
	}

	return (0);
}

/*****************************************************************************/

PVRHANDLE CALL_CONV PVROSCreateTexHeap(HDEVICE hDeviceID)
{
	TexHeapStruct* pt;

	pt = FindTexHeap(hDeviceID);
	
	if (pt == NULL)
	{	
	    DPF((DBG_ERROR,"Couldn't allocate texture heap"));
		return (NULL);
	}
	else if (pt->TexHeap.hDeviceID == 0)
	{
		BoardDataBlock	pb;

		pt->TexHeap.hDeviceID = hDeviceID;			
		pt->RefCounter = 0;

		PVROSGetPCIDeviceInfo (hDeviceID, &pb);

		pt->TexHeap.pMemoryRoot		= &(pt->MemoryRoot);
	 	pt->TexHeap.pTextureMemory	= (void*) pb.LinearMemWindows[1];
		
		/* Read the size for the texture parameter space in bytes */
	    pt->TexHeap.TexParamSize = 1024 * HWRdValFileUInt("TSPParamSize", (TEX_PARAM_SIZE/1024));
		pt->TexHeap.DeviceType = pb.DeviceType;

		InitTextureMemory(4*1024*1024, &(pt->TexHeap));

		/* set up the texture memory for cache overflow and TSP params */
		SetupOverflowArea (pt);

	#if 0 /* now done in SetupOverflowArea */
		/* The translucent pixel HAS to be set up after the texture memory */
	
		pt->TexHeap.TranslucentControlWord = SetupTransPixel (&pt->TexHeap);
	#endif

		/* set stuff up for texture load serialising */


	}

	pt->RefCounter++;

	return(&pt->TexHeap);
}

/*****************************************************************************/

void CALL_CONV PVROSDestroyTexHeap(PVRHANDLE TexHeap)
{	
	TexHeapStruct* pt;

	pt = FindOldTexHeap(TexHeap);
	
	if(pt != NULL)
	{
		if((--(pt->RefCounter)) == 0)
		{
			ResetTexHeap(TexHeap);
		}
	}
	else
	{
		DPF((DBG_ERROR,"Attempt to Destroy Invalid Texture Heap"));
	}							
}
/* -==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==- */

/******************************************************************************
 * Name         : pktsp.c
 * Title        : Performs the TEXAS parameter packing.
 * Author       : Peter Leaback
 * Created      : 21/07/1995
 *
 * Copyright	: 1995-2022 Imagination Technologies (c)
 * License		: MIT
 *
 * Description  :
 * 
 * Platform     : ANSI
 *
 * Modifications:
 * $Log: pktsp.c,v $
 * Revision 1.78  1997/11/26  15:29:19  sxy
 * added DPFDEV messages.
 *
 * Revision 1.77  1997/08/29  14:04:52  gdc
 * made sure background colour doesn't get out of range and over-write
 * other tsp bits - re Krister Wombell bug
 *
 * Revision 1.76  1997/08/26  16:32:50  erf
 * 1. Added facility to specify the colour of the masking plane. Only
 *    effects when blending with the background is required. By default
 *    it is off, ie set to black.
 * 2. If masking plane set then we move the TSP position here.
 *
 * Revision 1.75  1997/07/28  14:00:27  mjg
 *  Removed the choice of masking plane type for PCX2_003
 *
 * Revision 1.74  1997/07/14  19:30:35  mjg
 * Added the PCX2_003 compile switch and MODULE_ID.
 *
 * Revision 1.73  1997/06/23  09:44:59  erf
 * Added new variable which selects whether the full masking plane
 * is enabled or not. The is read from the registry in the PVRHAL
 * section. The default is OFF.
 *
 * Revision 1.72  1997/06/18  14:39:49  mjg
 * Fixed PCX2 slow down with a mask-plane fix - made it PCX2 only.
 *
 * Revision 1.71  1997/06/09  08:38:47  gdc
 * fixed a compiler warning
 *
 * Revision 1.70  1997/05/02  07:42:44  erf
 * Removed UINT32's etc and replaced with sgl_uint32's etc.
 *
 * Revision 1.69  1997/04/24  15:16:20  mjg
 * Removed redundant packing routines.
 *
 * Revision 1.68  1997/04/21  20:00:50  mjg
 * Implemented new parameter buffer structures.
 *
 * Revision 1.67  1997/04/21  15:27:23  mjg
 * Replaced ParamBufferInfo with PVRParamBuffs.
 *
 * Revision 1.66  1997/03/15  12:40:39  jop
 * Added include for pvrosapi.h
 *
 * Revision 1.65  1997/03/14  17:23:08  jwf
 * Added Machine independant macros for building param lists
 *
 * Revision 1.64  1997/03/14  12:18:20  jwf
 * changed so it builds on the mac
 *
 * Revision 1.63  1997/03/10  08:11:12  erf
 * Simplified PackTexasMask() routine.
 *
 * Revision 1.63  1997/03/10  08:11:12  erf
 * Simplified PackTexasMask() routine.
 *
 * Revision 1.62  1997/02/11  14:55:02  erf
 * Added PackTexasMask() routine. Used to define a fully textured masking
 * plane to fix masking with the PCX2 hardware.
 *
 * Revision 1.61  1996/12/13  17:25:19  gdc
 * added individual non-textured param packing funcs as part of a bug fix
 * when textures are turned off by quality settings but translucency is on
 * i.e. so there is no a non-textured flat shaded translucent function
 * also new functions have no 'if's in the planes loop
 *
 * Revision 1.60  1996/12/10  23:29:27  erf
 * Changed PVR1 to ISPTSP.
 *
 * Revision 1.59  1996/12/09  13:57:35  gdc
 * removed reference to SGLTri stuff - assume now that D3DOPTIM stuff is default
 *
 * Revision 1.58  1996/11/28  17:14:03  jrc
 * Added hardware specular highlights for faces enabled for shadows or
 * light volumes.
 *
 * Revision 1.57  1996/11/18  18:34:13  jop
 * Removed PowerSGL direct stuff
 *
 * Revision 1.56  1996/11/09  04:20:22  jrc
 * fix for reusing texturing and shading on successive faces (currently
 * disabled).
 *
 * Revision 1.55  1996/10/30  14:07:35  jop
 * Transparent plane is now fully transparent
 *
 * Revision 1.54  1996/10/24  21:54:14  jop
 * Put flat specular highlights back in
 *
 * Revision 1.53  1996/10/23  16:12:54  jop
 *  Added debug stuff (ifdefed out) for sm/shad/trans bug
 *
 * Revision 1.52  1996/10/18  13:05:01  jop
 * Added late background colour support
 *
 * Revision 1.51  1996/10/17  10:50:00  jrc
 * Terminated a comment which would previously have caused smooth shading code
 * being used for texturing case.
 *
 * Revision 1.50  1996/10/16  21:14:32  jop
 * Added some new pack routines
 *
 * Revision 1.49  1996/10/11  17:08:08  jop
 * Fixed pink/blue bug
 *
 * Revision 1.48  1996/10/09  15:37:40  mal
 * Modified the macros for timing tsp packing routines.
 *
 * Revision 1.47  1996/10/04  16:14:12  sjf
 * Added the missing parameters to jim's PAGE_BREAK_BUG_FIX macro
 *
 * Revision 1.46  1996/10/04  15:27:15  jop
 * Fixed it again (double oops)
 *
 * Revision 1.45  1996/10/04  12:22:14  jop
 * Fixed bug fix macro (oops)
 *
 * Revision 1.44  1996/10/01  15:04:49  mjg
 * Fixed a
 *
 * Revision 1.43  1996/09/30  18:44:58  jop
 * Fixed integer colour packing
 *
 * Revision 1.42  1996/09/30  14:16:41  jop
 * new col format
 *
 * Revision 1.41  1996/09/27  14:27:17  jop
 * Changed specular highlight stuff in tri code
 *
 * Revision 1.40  1996/09/26  16:52:23  gdc
 * put checking in to stop more parameters being written than
 * valid space existing - on a 4meg card this was causing texture
 * corruption, on a 2meg card it caused exception error - Re:
 * Andy Ward from compaq bug demo program
 *
 * Revision 1.39  1996/09/07  22:58:49  jop
 * Added light volumes to triangle api
 *
 * Revision 1.38  1996/09/04  00:00:17  jop
 * Added shadow support to NTT triangle functions
 *
 * Revision 1.37  1996/09/02  16:59:01  mjg
 * Implemented alpha channel translucency for Triangle API
 *
 * Revision 1.36  1996/08/30  14:04:19  pdl
 * made the non-textured translucency use the single pixel white
 *
 * Revision 1.35  1996/07/19  19:32:28  jop
 * Added white texture for nontexttrans and optimised triangle packer
 *
 * Revision 1.34  1996/07/16  10:52:09  jop
 * Division free solution for triangle api
 *
 * Revision 1.33  1996/07/15  17:08:12  jop
 * Triangle packers send number of processed triangles back in case
 * there was not enough room. Could be optimised!
 *
 * Revision 1.32  1996/07/11  16:40:03  sjf
 * Needed to swap order of sgl.h and sgl_defs.h includes.
 *
 * Revision 1.31  1996/07/04  08:57:34  mal
 * Removed the Temp Variables in PackSmoothShadingParams and
 * PackFlatShadingParams that were supposed to give Super Scalar Performance
 * Optimisations. This gives us back around 2% of the overall effecincy
 * that was lost from B6 to B7.
 *
 * Revision 1.30  1996/06/06  23:06:17  jop
 * Fixed shadow bugs in triangle api
 *
 * Revision 1.29  1996/06/06  14:22:09  jop
 * Added shadow version of triangle render code
 *
 * Revision 1.28  1996/05/22  11:05:36  jop
 * Fixed triangle texture bug - ptr to texture results
 * got incremented twice
 *
 * Revision 1.27  1996/05/09  17:32:06  jop
 * Added triangle packing support
 *
 * Revision 1.26  1996/05/03  16:13:47  sjf
 * With a bit of luck, I've optimised the packing a little by
 * 1) With textures, used local variables which should help overcome
 *    compiler restrictions on possible memory aliasing
 * 2) Changed the Smooth shading packing so that it assumes 32 bit input
 *    data and reduced a bit of masking.
 *
 * Revision 1.25  1996/04/22  16:33:19  sjf
 * OOPs: Fixed bug in non textured packer ... was turning on "highlight"
 * flag, which should be off.
 *
 * Revision 1.24  1996/04/22  16:09:48  sjf
 * Added non textured pack routine
 *
 * Revision 1.23  1996/04/20  16:45:23  sjf
 * Added a smooth textured packing routine.
 *
 * Revision 1.22  1996/04/19  19:53:12  sjf
 * Added a Flat Textured specialised packing routine, which SHOULD
 * be a bit faster. Hopefully this is a common case.
 *
 * Revision 1.21  1995/11/27  12:50:54  pdl
 * removed the read-modify-write.
 *
 * Revision 1.20  1995/10/20  11:06:57  jrc
 * Added parentheses to avoid warnings.
 *
 * Revision 1.19  1995/10/20  10:35:57  jrc
 * Avoids the last word of a page of the texas parameter store.
 *
 * Revision 1.18  1995/10/19  12:59:27  sjf
 * Changed name of location of Texas parameter buffer.
 *
 * Revision 1.17  1995/10/11  13:28:11  sjf
 * 1) Added a "fix" so that textured items will align with the start of
 *    Texas cache blocks.  This may not necessarily be a good idea. In fact
 *    it really should only be done for items which are flat shaded and textured
 *    with no offset/highlight parameter because it wastes space in the texas cache.
 * 2) Changed code to use the rnglobals parameter pointer block, so that we
 *    can double buffer the parameters.
 *
 * Revision 1.16  1995/10/06  14:18:05  jop
 * Added 'private shading rep point' (allowing the s/s code to
 * move an offscreen rep point onto the screen with impunity
 *
 * Revision 1.15  1995/10/04  08:32:25  sjf
 * Added routine to get the XY dimension of a texture.
 *
 * Revision 1.14  1995/09/15  19:47:16  sjf
 * Trimmed off a few more cycles per plane by re-arranging code and
 * removing local variables which became unnecessary AND by
 * changes the packing of 24 bit colour into 3 bits as it
 * had to be split up after packing anyway.
 *
 * Revision 1.13  1995/09/13  17:02:22  sjf
 * If shadows were switched off but highlights were on, then it
 * read unitialised data (the data was originally initialised, but
 * the data is no longer set up in the shading).
 *
 * Revision 1.12  1995/09/06  14:04:01  sjf
 * Did a "first pass" lot of optimisation, which seems to have
 * halved the time to "pack". I suspect that a fair amount of
 * time is now spent in converting from floating point colours
 * to integers. It MAY be possible to get a bit more speed (on some
 * processors) by multiplying by 256.0 (instead of 255) and 32 (instead
 * of 31) PROVIDED we can clamp the max colour component to something
 * like 0.999 instead of 1.0
 *
 * Revision 1.11  1995/08/25  12:00:35  pdl
 * added some INLINE.
 *
 * Revision 1.10  1995/08/21  09:48:14  sjf
 * Routines no longer ignore fogging.
 *
 * Revision 1.9  1995/08/09  17:25:47  pdl
 * added the TexasMapOpacityFloatToInt
 *
 * Revision 1.8  1995/08/03  17:31:16  pdl
 * made coersion from float to sgl_uint32 work.
 * removed jim's function.
 *
 * Revision 1.7  1995/08/02  12:14:37  pdl
 * changed some .0 to .0f
 *
 * Revision 1.6  1995/07/27  11:18:02  pdl
 * added PackTexasFlat.
 *
 * Revision 1.5  1995/07/26  17:39:16  pdl
 * changed the parameter store pointer.
 *
 * Revision 1.4  1995/07/26  14:59:36  jop
 * Added Indirect single parameter function
 *
 * Revision 1.3  1995/07/25  14:34:16  pdl
 * allowed smooth shading..
 *
 * Revision 1.2  1995/07/25  08:56:03  sjf
 * Changed orders of include files so typedefs in correct order.
 *
 * Revision 1.1  1995/07/23  15:26:00  pdl
 * Initial revision
 *
 *
 *
 *****************************************************************************/

#define MODULE_ID MODID_PKTSP

#include "sgl_defs.h"
#include "sgl.h"
#include "dlnodes.h"
#include "rnglobal.h"
#include "rnconvst.h"
#include "rnstate.h"
#include "rnfshade.h"
#include "pktsp.h"

#include "nm_intf.h"
#include "dlglobal.h"
#include "metrics.h"

#include "pvrosapi.h"
#include "parmbuff.h"

#if ISPTSP
#define PAGE_BREAK_BUG TRUE  /* TRUE if page break bug in hardware */
#else
#define PAGE_BREAK_BUG FALSE
#endif

#if PAGE_BREAK_BUG
	#define WORDS_IN_PAGE 512  /* 32-bit words */
#endif

SGL_EXTERN_TIME_REF /* if we are timing code */

int gnWhiteTex = -1;

#if PCX2
sgl_bool	bFullMaskingPlane = FALSE;
sgl_bool	bSetMaskingBGColour = FALSE;
#endif

/******************************************************************************
 * Function Name: TexasMapOpacityFloatToInt
 *
 * Inputs       : fOpacity,the float opacity
 * Outputs      : -
 * Returns      : TexasTrans,the texas version.
 * Globals Used : -
 *
 * Description  : Converts an opacity value from 0.0 -> 1.0 to
 *				  a translucency value from 0 -> 15 
 *****************************************************************************/
int TexasMapOpacityFloatToInt(float fOpacity)
{
	int TexasTrans;

	TexasTrans=(int)((1.0f-fOpacity)*16.0f);  /* 16 is correct*/

	TexasTrans =(TexasTrans > 15) ? 15 : TexasTrans;
	TexasTrans =(TexasTrans < 0 ) ?  0 : TexasTrans;
	
	return(TexasTrans);
}

/******************************************************************************
 * Function Name: TexasGetTextureDimension
 *
 * Inputs       : Pointer to the texas precomp struct
 * Outputs      : -
 * Returns      : The size of the texture as an int.
 * Globals Used : -
 *
 * Description  : Assuming that the precomputed structure DOES represent
 *				  a texture, this returns the dimension (in pixels) of the
 *				  texture. (note textures are square).

 *
 *				This is probably a candidate for making inline...
 *****************************************************************************/
int TexasGetTextureDimension (const TEXAS_PRECOMP_STRUCT *pTexasPreComp)
{
	int TextureSize;

	TextureSize = pTexasPreComp->TexAddress >> SHIFT_MAP_SIZE;
	TextureSize &= (MASK_MAP_SIZE >> SHIFT_MAP_SIZE);
	switch(TextureSize)
	{
		case sgl_map_32x32:
			TextureSize = 32;
			break;

		case sgl_map_64x64:
			TextureSize = 64;
			break;

		case sgl_map_128x128:
			TextureSize = 128;
			break;

		default:
			ASSERT(TextureSize == sgl_map_256x256);
			TextureSize = 256;
			break;

	}/*end switch*/

	return (TextureSize);
}


/******************************************************************************
 * Function Name: TexasGetInvTextureDimension
 *
 * Inputs       : Pointer to the texas precomp struct
 * Outputs      : -
 * Returns      : The size of the texture as an int.
 * Globals Used : -
 *
 * Description  : Assuming that the precomputed structure DOES represent
 *				  a texture, this returns the dimension (in pixels) of the
 *				  texture. (note textures are square).

 *
 *				This is probably a candidate for making inline...
 *****************************************************************************/
float TexasGetInvTextureDimension (const TEXAS_PRECOMP_STRUCT *pTexasPreComp)
{
	int TextureSize;

	TextureSize = (pTexasPreComp->TexAddress & MASK_MAP_SIZE) >> SHIFT_MAP_SIZE;

	switch (TextureSize)
	{
		case sgl_map_32x32:
		{
			return (1.0f / 32.0f);
		}

		case sgl_map_64x64:
		{
			return (1.0f / 64.0f);
		}

		case sgl_map_128x128:
		{
			return (1.0f / 128.0f);
		}

		default:
		{
			ASSERT(TextureSize == sgl_map_256x256);
			return (1.0f / 256.0f);
		}

	}/*end switch*/
}



/******************************************************************************
 * Function Name: ConvertRGBtoTexas24
 *
 * Inputs       : Colour,the RGB data in floats in the range 0.0 to 1.0
 * Outputs      : -
 * Returns      : ReturnColour, the RGB data packed into a sgl_uint32
 * Globals Used : -
 *
 * Description  : The format of the output data is 0x00RRGGBB.
 * Pre-condition: Colour[]>=0 && Colour[]<=1.0
 *****************************************************************************/
static INLINE sgl_uint32 ConvertRGBtoTexas24(const sgl_vector Colour)
{
	sgl_uint32	ReturnColour;

#if DEBUG || DEBUGDEV

	if(Colour[0]<0.0f || Colour[0]>1.0f || Colour[1]<0.0f || Colour[1]>1.0f ||
	   Colour[2]<0.0f || Colour[2]>1.0f)
	{
		DPFDEV ((DBG_WARNING, "ConvertRGBtoTexas24: Colour value out of range"));
	}
#endif

	/*
	// NOTE: Convert first to SIGNED int as this is MUCH quicker
	// (generally) than conversion straight to UNSIGNED
	*/
	ReturnColour= (sgl_uint32)((sgl_int32)(Colour[2]*255.0f));
	ReturnColour|=(sgl_uint32)((sgl_int32)(Colour[1]*255.0f))<<8;
	ReturnColour|=(sgl_uint32)((sgl_int32)(Colour[0]*255.0f))<<16;

	return(ReturnColour & 0x00FFFFFF);
}

/******************************************************************************
 * Function Name: ConvertColourFloatTo8bit
 *
 * Inputs       : Colour Component
 * Outputs      : -
 * Returns      : ReturnColour, the Component data packed into a sgl_uint32
 * Globals Used : -
 *
 * Description  : Converts a single RGB Component (i.e. R,G OR B) to int equiv
 * Pre-condition: Colour[]>=0 && Colour[]<=1.0
 *****************************************************************************/
static INLINE sgl_uint32 ConvertColourFloatTo8bit(const float ColComponent)
{

	ASSERT(ColComponent>=0.0f && ColComponent<=1.0f);
	/*
	// NOTE: Convert first to SIGNED int as this is MUCH quicker
	// (generally) than conversion straight to UNSIGNED
	*/
	return (sgl_uint32)((sgl_int32)(ColComponent*255.0f));
}

/******************************************************************************
 * Function Name: ConvertRGBtoTexas16
 *
 * Inputs       : Colour,the RGB data in floats in the range 0.0 to 1.0
 * Outputs      : -
 * Returns      : ReturnColour, the RGB data packed into a sgl_uint32
 * Globals Used : -
 *
 * Description  : The format of the output data is 555, with blue in the least
 *				  significant bits.
 * Pre-condition: Colour[]>=0 && Colour[]<=1.0
 *****************************************************************************/
static INLINE sgl_uint32 ConvertRGBtoTexas16(const sgl_vector Colour)
{
	sgl_uint32	ReturnColour;

	ASSERT(Colour[0]>=0.0f && Colour[0]<=1.0f);
	ASSERT(Colour[1]>=0.0f && Colour[1]<=1.0f);
	ASSERT(Colour[2]>=0.0f && Colour[2]<=1.0f);

	/*
	// NOTE: Convert first to SIGNED int as this is MUCH quicker
	// (generally) than conversion straight to UNSIGNED
	*/
	ReturnColour= (sgl_uint32)((sgl_int32)(Colour[2]*31.0f));
	ReturnColour|=(sgl_uint32)((sgl_int32)(Colour[1]*31.0f))<<5;
	ReturnColour|=(sgl_uint32)((sgl_int32)(Colour[0]*31.0f))<<10;


	return(ReturnColour);
}


/******************************************************************************
 * Function Name: PackTexasFlat
 *
 * Inputs       : rgbColour,The flat shading colour.
 * Outputs      : -
 * Returns      : instructionIndex,the texas tag.
 * Globals Used : parameterStore
 *				  parameterStoreIndex
 *
 * Description  : This packs a flat shading colour into the format that
 *				  Texas can read, and returns the instruction pointer. This will
 *				  probably be used for the background plane.
 *****************************************************************************/
sgl_uint32 PackTexasTransparent(sgl_bool ShadowsOn)
{
	sgl_uint32 instructionIndex;
	sgl_uint32 *pParameterStore;
	
	instructionIndex = PVRParamBuffs[PVR_PARAM_TYPE_TSP].uBufferPos;

#if PAGE_BREAK_BUG
	/*
	// If we would be using the last word in a page (unlikely)...
	*/
	if ( (instructionIndex & (WORDS_IN_PAGE-1)) > WORDS_IN_PAGE-1-2 )
	{
		/*
		// Move to start of next page.
		//
		// (We could just add 2 to the index to move a safe but non-optimal
		// distance away, but who cares becasuse this is only called once per
		// render?)
		*/
		instructionIndex =
		  ( instructionIndex & ~(WORDS_IN_PAGE-1) ) + WORDS_IN_PAGE;

		/*
		// Perhaps this update does not have to be done since this function
		// does the final Texas instructions?
		*/
		PVRParamBuffs[PVR_PARAM_TYPE_TSP].uBufferPos = instructionIndex;
	}
#endif

	pParameterStore = & PVRParamBuffs[PVR_PARAM_TYPE_TSP].pBuffer[instructionIndex];

	if (ShadowsOn)
	{
		IW( pParameterStore, 0, (MASK_TEXTURE | MASK_TRANS | (0x0000000F << 13) | MASK_SHADOW_FLAG));
	}
	else
	{
		IW( pParameterStore, 0, (MASK_TEXTURE | MASK_TRANS | (0x0000000F << 13)));
	}

	IW( pParameterStore, 1, 0);

	IW( pParameterStore, 2+0, 1);									/* r */
	IW( pParameterStore, 2+1, 0);									/* q<<16 | p */
	IW( pParameterStore, 2+2, (TranslucentControlWord << 16));		/* addr<16 | c */
	IW( pParameterStore, 2+3, 0);									/* b<<16 | a */
	IW( pParameterStore, 2+4, (TranslucentControlWord & 0xFFFF0000));/* addr&FFFF0000 | f */
	IW( pParameterStore, 2+5, 0);									/* e<<16 | d */

	PVRParamBuffs[PVR_PARAM_TYPE_TSP].uBufferPos += 8;

	return (instructionIndex >> 1);
}

/******************************************************************************
 * Function Name: PackTexasFlat
 *
 * Inputs       : rgbColour,The flat shading colour.
 * Outputs      : -
 * Returns      : instructionIndex,the texas tag.
 * Globals Used : parameterStore
 *				  parameterStoreIndex
 *
 * Description  : This packs a flat shading colour into the format that
 *				  Texas can read, and returns the instruction pointer. This will
 *				  probably be used for the background plane.
 *****************************************************************************/
sgl_uint32 PackTexasFlat(sgl_vector rgbColour, sgl_bool FogOn, sgl_bool ShadowsOn)
{
	sgl_uint32 instructionIndex;
	
	instructionIndex = PVRParamBuffs[PVR_PARAM_TYPE_TSP].uBufferPos;

#if PAGE_BREAK_BUG
	/*
	// If we would be using the last word in a page (unlikely)...
	*/
	if ( (instructionIndex & (WORDS_IN_PAGE-1)) > WORDS_IN_PAGE-1-2 )
	{
		/*
		// Move to start of next page.
		//
		// (We could just add 2 to the index to move a safe but non-optimal
		// distance away, but who cares becasuse this is only called once per
		// render?)
		*/
		instructionIndex =
		  ( instructionIndex & ~(WORDS_IN_PAGE-1) ) + WORDS_IN_PAGE;

		/*
		// Perhaps this update does not have to be done since this function
		// does the final Texas instructions?
		*/
		PVRParamBuffs[PVR_PARAM_TYPE_TSP].uBufferPos = instructionIndex;
	}
#endif

	instructionIndex >>= 1;
	
	PackTexasFlatAtAddr (rgbColour, FogOn, ShadowsOn, instructionIndex);
	
	/* if we are trying a smooth shaded version ... */
		
	if (ShadowsOn)
	{
		PVRParamBuffs[PVR_PARAM_TYPE_TSP].uBufferPos += (2 + 6);
	}
	else
	{
		PVRParamBuffs[PVR_PARAM_TYPE_TSP].uBufferPos += (2 + 4);
	}
	
	return (instructionIndex);
}

/******************************************************************************
 * Function Name: PackTexasFlatAtAddr
 *
 * Inputs       : rgbColour,The flat shading colour.
 * Outputs      : -
 * Returns      : instructionIndex,the texas tag.
 * Globals Used : parameterStore
 *				  parameterStoreIndex
 *
 * Description  : This packs a flat shading colour into the format that
 *				  Texas can read, and returns the instruction pointer. This will
 *				  probably be used for the background plane.
 *****************************************************************************/
void PackTexasFlatAtAddr(sgl_vector rgbColour, sgl_bool FogOn, sgl_bool ShadowsOn, sgl_uint32 Addr)
{
	sgl_uint32 HoldCol;
	sgl_uint32 *pParameterStore;
	
	pParameterStore = & (PVRParamBuffs[PVR_PARAM_TYPE_TSP].pBuffer[Addr << 1]);

	#if 1

	   	HoldCol = ConvertRGBtoTexas24(rgbColour);

		if (ShadowsOn)
		{
			IW( pParameterStore, 0, (((FogOn) ? 0:MASK_DISABLE_FOG) | ((HoldCol>>16) & 0x000000FF) | MASK_SHADOW_FLAG)); /*red*/
		}
		else
		{
			IW( pParameterStore, 0, (((FogOn) ? 0:MASK_DISABLE_FOG) | ((HoldCol>>16) & 0x000000FF))); /*red*/
		}

		IW( pParameterStore, 1, (HoldCol<<16)); /*green and blue*/

	#else

		/* if we are trying a smooth shaded version ... */

		if (ShadowsOn)
		{
			IW( pParameterStore, 0, (MASK_SMOOTH_SHADE | ((FogOn) ? 0:MASK_DISABLE_FOG) | MASK_SHADOW_FLAG)); /*red*/
			IW( pParameterStore, 1, 0);
		   	IW( pParameterStore, 2, (0x1FFF | (ConvertRGBtoTexas16 (rgbColour) << 16)));
			IW( pParameterStore, 3, 0x000F000F);
		   	IW( pParameterStore, 4, 0);
			IW( pParameterStore, 5, 0);
		}
		else
		{
			IW( pParameterStore, 0, (MASK_SMOOTH_SHADE | ((FogOn) ? 0:MASK_DISABLE_FOG)));
			IW( pParameterStore, 1, 0);
		   	IW( pParameterStore, 2, (0x1FFF | (ConvertRGBtoTexas16 (rgbColour) << 16)));
			IW( pParameterStore, 3, 0x000F000F);
		}

	#endif
}

#if PCX2 || PCX2_003
/******************************************************************************
 * Function Name: PackTexasMask
 *
 * Inputs       : rgbColour,The flat shading colour.
 * Outputs      : -
 * Returns      : instructionIndex,the texas tag.
 * Globals Used : parameterStore
 *				  parameterStoreIndex
 *
 * Description  : This packs a textured plane for the masking plane. Used to fix
 *				  pixel problems with D3D applications.
 *****************************************************************************/
sgl_uint32 PackTexasMask(sgl_vector rgbColour, sgl_bool FogOn, sgl_bool ShadowsOn)
{
	sgl_uint32 HoldCol;
	sgl_uint32 instructionIndex;
	sgl_uint32 *pParameterStore;	
	
	/* Do we allow the user to specify the background colour on the masking plane.
	 */
	if (bSetMaskingBGColour)
	   	HoldCol = ConvertRGBtoTexas24(rgbColour);
	else
		HoldCol = 0;

	/* Masking id is always 2.
	 */
	instructionIndex = 2;

	pParameterStore = & (PVRParamBuffs[PVR_PARAM_TYPE_TSP].pBuffer[instructionIndex]);
	instructionIndex >>= 1;

#if PCX2
	if (bFullMaskingPlane)
	{
		/* fix masking problem. Packs the masking plane as a textured plane.
		 */
		IW( pParameterStore, 0, (MASK_DISABLE_FOG | ((HoldCol>>16) & 0x000000FF)));		/* red				*/
		IW( pParameterStore, 1, (HoldCol<<16));						/* green and blue	*/
		IORW( pParameterStore, 0, MASK_TEXTURE);
		IORW( pParameterStore, 0, (2<<SHIFT_EXPONENT));
		IW( pParameterStore, 2, 0x1);
		IW( pParameterStore, 3, 0x0);
		IW( pParameterStore, 4, 0x0);
		IW( pParameterStore, 5, 0x4000);
		IW( pParameterStore, 6, 0x70010000);
		IW( pParameterStore, 7, 0x40000000);
		IW( pParameterStore, 8, 0xFFFFFFFF);

		/* We are using the large masking plane so the starting TSP address is 12.
		 * i.e. 10 WORDS are being used by the masking plane.
		 */
		PVRParamBuffs[PVR_PARAM_TYPE_TSP].uBufferPos = 12;
	}
	else
#endif /* PCX2 */
	{
		/* Minimalist (and PCX2_003) version - speeds up Terracide, Forsaken etc.
		 * and doesn't have any effect on Mona Lisa
		 */
		IW( pParameterStore, 0, (MASK_DISABLE_FOG | ((HoldCol>>16) & 0x000000FF)) );
		IW( pParameterStore, 1, (HoldCol<<16));						/* green and blue	*/

		/* We are using the small masking plane so the starting TSP address is 6.
		 * i.e. 2 WORDS are being used by the masking plane. We need to skip the
		 * fog plane TSP reserved index of 2 (ie position 4).
		 */
		PVRParamBuffs[PVR_PARAM_TYPE_TSP].uBufferPos = 6;
	}

	/* Return the TSP ID.
	 */
	return (instructionIndex);
}
#endif

#if 0
/******************************************************************************
 * Function Name: PackTexasParams
 *
 * Inputs       : NumberOfPlanes,
 *				  TextR[], an array of texturing results.
 *				  ShadeR[], an array of shading results.
 *				  IsTextured,TRUE/FALSE
 *				  IsSmoothShaded,TRUE/FALSE
 *				  IsShadowed,TRUE/FALSE
 *				  IsHighlighted,TRUE/FALSE
 *				  Fogon, True/False
 *				  Planes[], the projected rep. point. is used
 *				  MState, the texas_precomp.LowWord of the material state is used
 * Outputs      : Planes[], the u32TexasTag is written.
 * Returns      : -
 * Globals Used : parameterStore
 *				  parameterStoreIndex
 *
 * Description  : This packs the texturing and shading data into the format that
 *				  Texas can read, and stores the instruction pointer in the
 *				  transformed planes structure.
 *				  THIS ROUTINE IS VERY SLOW, AND I WILL RE-WRITE IT SOON.
 * Pre-condition: the texture,globalTrans,flipUV and TransPass bits are correctly
 *				  set in the texas precomputed structure and the other bits are 0.
 *****************************************************************************/
void PackTexasParams(int						NumberOfPlanes,
					 TEXTURING_RESULT_STRUCT   *pTextR,
					 SHADING_RESULT_STRUCT     *pShadeR,
					 sgl_bool					IsTextured,
					 sgl_bool					IsSmoothShaded,
					 sgl_bool					IsShadowed,
					 sgl_bool					IsHighlighted,
					 sgl_bool					FogOn, 
					 TRANSFORMED_PLANE_STRUCT  *Planes[],
				const MATERIAL_STATE_STRUCT	   *MState)
{

	/*
	// Pointer into where we are storing the values, PLUS a local
	// copy of the index, as this will probably be kept in a register
	// and thus be quicker to work with.
	*/
	sgl_uint32 *pParameterStore, *pFirstInstrWord,FirstInstrWord;
	sgl_uint32 LocalPStoreIndex;
	sgl_uint32 LocalPStoreEnd;

	/*
	// Precalculate the control word values which will be
	// constant for all the planes
	*/
	sgl_uint32 CommonPstore;

	/*
	// Get quick access to each plane
	*/
	TRANSFORMED_PLANE_STRUCT * pPlane;


	if(IsTextured)
	{
		sgl_uint32 tmp;

		CommonPstore=MState->texas_precomp.LowWord;
		/*
		// For Mark Dunn try aligning to texas Cache lines when weve got
		// a texture.
		//
		// THIS SEEMS POINTLESS GIVEN THAT THE SOFTWARE IS MUCH SLOWER THAN
		// THE HARDWARE.     SJF.
		*/
	}
	else
		CommonPstore=0;


	/*
	// Get the current parameterstore index, and a pointer to the
	// current place to make additions
	*/
	LocalPStoreIndex = PVRParamBuffs[PVR_PARAM_TYPE_TSP].uBufferPos;

	/* find the end of allowable space and knock of about one planes worth */
	LocalPStoreEnd = (PVRParamBuffs[PVR_PARAM_TYPE_TSP].uBufferLimit) - 12;
	pParameterStore=&(PVRParamBuffs[PVR_PARAM_TYPE_TSP].pBuffer[LocalPStoreIndex]);

	CommonPstore |= (IsSmoothShaded) ?MASK_SMOOTH_SHADE : 0;
	CommonPstore |= (IsShadowed) ? MASK_SHADOW_FLAG : 0;
	CommonPstore |= (IsHighlighted) ? MASK_FLAT_HIGHLIGHT : 0;
	CommonPstore |= (FogOn) ? 0:MASK_DISABLE_FOG;

	/*
	// Go through the planes. Move on to the next shading results and
	// and next plane pointer (NOTE Planes is array of pointers)
	*/
	for(/*Nil*/; NumberOfPlanes !=0; NumberOfPlanes--, pShadeR++, Planes++)
	{
	  /* if this plane might write over the end of valid space break */
		if (LocalPStoreIndex >= LocalPStoreEnd)
		{
			break;
		}
		/*
		// Get pointer to this plane (though current place int 
		// the array of pointers)
		*/
		pPlane = *Planes;

#if PAGE_BREAK_BUG
		/*
		// If we would be using the last word in a page...
		//
		// (We are assuming the maximum possible number of words (12) are to be
		// used as this will only waste about 2% of the texas parameter store
		// and this is only a temporary bug fix.)
		*/
		if ((LocalPStoreIndex & (WORDS_IN_PAGE-1)) > WORDS_IN_PAGE-1-12)
		{
			/*
			// move to start of next page
			*/
			LocalPStoreIndex =
			  ( LocalPStoreIndex & ~(WORDS_IN_PAGE-1) ) + WORDS_IN_PAGE;
			pParameterStore = (PVRParamBuffs[PVR_PARAM_TYPE_TSP].pBuffer[LocalPStoreIndex]);
		}
#endif
		/*
		// Remember where we are currently situated
		// This is needed when packing textures
		*/
		pFirstInstrWord = pParameterStore;

		/*
		** Put the index in the projected plane structure.
		*/
		pPlane->u32TexasTag = LocalPStoreIndex>>1;

		/*
		// if flat shading, pack two base colours
		*/
		if(!IsSmoothShaded)
		{
			/*
			// The first control word consists of the common bit and 
			// red component which goes at the end
			*/
			
			FirstInstrWord =CommonPstore | 
				ConvertColourFloatTo8bit(pShadeR->slot[0].flat.baseColour[0]);

			IW( pParameterStore, 0, FirstInstrWord);
 
			/*
			// Add the green and blue components to the shadow colour in
			// the second
			*/
			IW( pParameterStore, 1, 
			  (ConvertColourFloatTo8bit(pShadeR->slot[0].flat.baseColour[1])<< 24 |
			  ConvertColourFloatTo8bit(pShadeR->slot[0].flat.baseColour[2])<< 16 |
			  ConvertRGBtoTexas16(pShadeR->slot[1].flat.baseColour)));
		}
		/*
		// Else for smooth shading pack the x/y offsets
		*/
		else
		{
			sgl_uint32 PSTemp;

			FirstInstrWord =CommonPstore;
			IW( pParameterStore, 0, FirstInstrWord);

			#if 1

				ASSERT(pShadeR->slot[0].smooth.ShadingRepPt[0]>=0.0f)
				ASSERT(pShadeR->slot[0].smooth.ShadingRepPt[1]>=0.0f)
				ASSERT(pShadeR->slot[0].smooth.ShadingRepPt[0]<1024.0f)
				ASSERT(pShadeR->slot[0].smooth.ShadingRepPt[1]<1024.0f)
	
				/*
				// Convert these First to INT then unsigned int as most
				// processors have instructions to handle signed conversion
				// but have to do back flips to do unsigned (which is asking
				// for extra precision) and therefore is slower.
				*/
				PSTemp  = (sgl_uint32)((sgl_int32)(pShadeR->slot[0].smooth.ShadingRepPt[0]))<<16;
				PSTemp |= (sgl_uint32)((sgl_int32)(pShadeR->slot[0].smooth.ShadingRepPt[1]));

			#else
			
				ASSERT(pPlane->projRepPoint[0]>=0.0f)
				ASSERT(pPlane->projRepPoint[1]>=0.0f)
				ASSERT(pPlane->projRepPoint[0]<1024.0f)
				ASSERT(pPlane->projRepPoint[1]<1024.0f)
	
				/*
				// Convert these First to INT then unsigned int as most
				// processors have instructions to handle signed conversion
				// but have to do back flips to do unsigned (which is asking
				// for extra precision) and therefore is slower.
				*/
				PSTemp  = (sgl_uint32)((sgl_int32)(pPlane->projRepPoint[0]))<<16;
				PSTemp |= (sgl_uint32)((sgl_int32)(pPlane->projRepPoint[1]));

			#endif
	
			IW( pParameterStore, 1, PSTemp);
		}

		/*
		// Move on in the parameter memory
		*/
		LocalPStoreIndex +=2;
		pParameterStore  += 2;

		/*
		**pack up the texturing parameters
		*/
		if(IsTextured)				
		{
			/* OR the exponent into the first control word*/

			IORW( pFirstInstrWord, 0, (FirstInstrWord | pTextR->Control1));

			IW( pParameterStore, 0, (pTextR->TexCoeff1));
			IW( pParameterStore, 1, (pTextR->TexCoeff2));
			IW( pParameterStore, 2, (pTextR->TexCoeff3));
			IW( pParameterStore, 3, (pTextR->TexCoeff4));
			IW( pParameterStore, 4, (pTextR->TexCoeff5));
			IW( pParameterStore, 5, (pTextR->TexCoeff6));

			LocalPStoreIndex+=6; 
			pParameterStore +=6;

			/*
			// Move to next lot of texture data
			*/
			pTextR ++;
		}

		if(IsSmoothShaded)
		{
			sgl_uint32 PSTemp;
			/*
			**SMOOTH SHADE PARAMETERS
			**pack up the non-shadowed smooth shade params
			*/
			PSTemp = pShadeR->slot[0].smooth.t0 & 0xffffUL;
			PSTemp |= ConvertRGBtoTexas16(pShadeR->slot[0].smooth.rgbColour)<<16;

		   	IW( pParameterStore, 0, PSTemp);
					
			PSTemp =  pShadeR->slot[0].smooth.t1 << 16;
			PSTemp |= pShadeR->slot[0].smooth.t2 & 0xffffUL;

	   		IW( pParameterStore, 1, PSTemp);

			LocalPStoreIndex+=2; 
			pParameterStore +=2;

			/*
			** pack up the smooth shaded shadow lights
			*/

			if(IsShadowed)	
			{
				PSTemp = pShadeR->slot[1].smooth.t0 & 0xffffUL;

			   	PSTemp |= ConvertRGBtoTexas16(pShadeR->slot[1].smooth.rgbColour)<<16;
			   	
				IW( pParameterStore, 0, PSTemp);

		   		PSTemp  = pShadeR->slot[1].smooth.t1 << 16;
			   	PSTemp |= pShadeR->slot[1].smooth.t2 & 0xffffUL;

				IW( pParameterStore, 1, PSTemp);

				LocalPStoreIndex+=2; 
				pParameterStore +=2;
			}

		}
		else
		{
			
			/*
			**FLAT SHADE PARAMETERS
			**pack up the highlight params
			*/

			if(IsHighlighted)	
			{
				sgl_uint32 PSTemp;
				PSTemp = 
					ConvertRGBtoTexas16(pShadeR->slot[0].flat.highlightColour)<<16;

				if(IsShadowed)
				{
					PSTemp |= 
					ConvertRGBtoTexas16(pShadeR->slot[1].flat.highlightColour);
				}

				IW( pParameterStore, 0, PSTemp);
					
				 /*this is to align the next parameter to a double word boundary*/
				 LocalPStoreIndex+=2; 
				 pParameterStore +=2;

			}/*end if "offset/highlight colour" */

		}/*end if smooth/flat shaded*/

	}/*end for*/


	/*
	// Save where we are up to in the global index
	*/
	PVRParamBuffs[PVR_PARAM_TYPE_TSP].uBufferPos = LocalPStoreIndex;

}
#endif

/******************************************************************************
 * Function Name: PackTexasParamsNonTextured
 *
 * Inputs       : NumberOfPlanes,
 *				  ShadeR[], an array of shading results.
 *				  IsSmoothShaded,TRUE/FALSE
 *				  IsShadowed,TRUE/FALSE
 *				  Fogon, True/False
 *				  Planes[], the projected rep. point. is used
 * Outputs      : Planes[], the u32TexasTag is written.
 * Returns      : -
 * Globals Used : parameterStore
 *				  parameterStoreIndex
 *
 * Description  : As for PackTexasParams except for non textured planes, and
 *				  assumes that higlight is ALWAYS off.
 *****************************************************************************/
void PackTexasParamsNonTextured(int				NumberOfPlanes,
					 SHADING_RESULT_STRUCT     *pShadeR,
					 sgl_bool					IsSmoothShaded,
					 sgl_bool					IsShadowed,
					 sgl_bool					FogOn, 
					 TRANSFORMED_PLANE_STRUCT  *Planes[])
{

	/*
	// Pointer into where we are storing the values, PLUS a local
	// copy of the index, as this will probably be kept in a register
	// and thus be quicker to work with.
	*/
	sgl_uint32 *pParameterStore;
	sgl_uint32 LocalPStoreIndex;
	sgl_uint32 LocalPStoreEnd;

	/*
	// Precalculate the control word values which will be
	// constant for all the planes
	*/
	sgl_uint32 CommonPstore;

	/*
	// Get quick access to each plane
	*/
	TRANSFORMED_PLANE_STRUCT * pPlane;

	SGL_TIME_START(PACK_TEXAS_NT_TIME)

	/*
	// Get the current parameterstore index, and a pointer to the
	// current place to make additions
	*/
	LocalPStoreIndex = PVRParamBuffs[PVR_PARAM_TYPE_TSP].uBufferPos;
	pParameterStore=&(PVRParamBuffs[PVR_PARAM_TYPE_TSP].pBuffer[LocalPStoreIndex]);
	/* find the end of allowable space and knock of about one planes worth */
	LocalPStoreEnd = PVRParamBuffs[PVR_PARAM_TYPE_TSP].uBufferLimit - 12;

	CommonPstore = (IsSmoothShaded) ? MASK_SMOOTH_SHADE : 0;
	CommonPstore |= (IsShadowed) ? MASK_SHADOW_FLAG : 0;
	CommonPstore |= (FogOn) ? 0:MASK_DISABLE_FOG;

	/*
	// Go through the planes. Move on to the next shading results and
	// and next plane pointer (NOTE Planes is array of pointers)
	*/
	for(/*Nil*/; NumberOfPlanes !=0; NumberOfPlanes--, pShadeR++, Planes++)
	{
	  /* if this plane might write over the end of valid space break */
		if (LocalPStoreIndex >= LocalPStoreEnd)
		{
			break;
		}
		/*
		// Get pointer to this plane (though current place int 
		// the array of pointers)
		*/
		pPlane = *Planes;

#if PAGE_BREAK_BUG
		/*
		// If we would be using the last word in a page...
		//
		// (We are assuming the maximum possible number of words (12) are to be
		// used as this will only waste about 2% of the texas parameter store
		// and this is only a temporary bug fix.)
		*/
		if ((LocalPStoreIndex & (WORDS_IN_PAGE-1)) > WORDS_IN_PAGE-1-12)
		{
			/*
			// move to start of next page
			*/
			LocalPStoreIndex =
			  ( LocalPStoreIndex & ~(WORDS_IN_PAGE-1) ) + WORDS_IN_PAGE;
			pParameterStore = &(PVRParamBuffs[PVR_PARAM_TYPE_TSP].pBuffer[LocalPStoreIndex]);
		}
#endif
		/*
		** Put the index in the projected plane structure.
		*/
		pPlane->u32TexasTag = LocalPStoreIndex>>1;

		/*
		// if flat shading, pack two base colours
		*/
		if(!IsSmoothShaded)
		{
			/*
			// The first control word consists of the common bit and 
			// red component which goes at the end
			*/
			
			IW( pParameterStore, 0, (CommonPstore | 
				ConvertColourFloatTo8bit(pShadeR->slot[0].flat.baseColour[0])));

 
			/*
			// Add the green and blue components to the shadow colour in
			// the second
			*/
			IW( pParameterStore, 1, 
			  (ConvertColourFloatTo8bit(pShadeR->slot[0].flat.baseColour[1])<< 24 |
			  ConvertColourFloatTo8bit(pShadeR->slot[0].flat.baseColour[2])<< 16 |
			  ConvertRGBtoTexas16(pShadeR->slot[1].flat.baseColour)));


			/*
			// Move on to the next spot
			*/
			LocalPStoreIndex+=2; 
			pParameterStore +=2;

		}
		/*
		// Else for smooth shading pack the x/y offsets
		*/
		else
		{
			sgl_uint32 PSTemp;

			IW( pParameterStore, 0, CommonPstore);

			#if 1

				ASSERT(pShadeR->slot[0].smooth.ShadingRepPt[0]>=0.0f)
				ASSERT(pShadeR->slot[0].smooth.ShadingRepPt[1]>=0.0f)
				ASSERT(pShadeR->slot[0].smooth.ShadingRepPt[0]<1024.0f)
				ASSERT(pShadeR->slot[0].smooth.ShadingRepPt[1]<1024.0f)
	
				/*
				// Convert these First to INT then unsigned int as most
				// processors have instructions to handle signed conversion
				// but have to do back flips to do unsigned (which is asking
				// for extra precision) and therefore is slower.
				*/
				PSTemp  = (sgl_uint32)((sgl_int32)(pShadeR->slot[0].smooth.ShadingRepPt[0]))<<16;
				PSTemp |= (sgl_uint32)((sgl_int32)(pShadeR->slot[0].smooth.ShadingRepPt[1]));

			#else
			
				ASSERT(pPlane->projRepPoint[0]>=0.0f)
				ASSERT(pPlane->projRepPoint[1]>=0.0f)
				ASSERT(pPlane->projRepPoint[0]<1024.0f)
				ASSERT(pPlane->projRepPoint[1]<1024.0f)
	
				/*
				// Convert these First to INT then unsigned int as most
				// processors have instructions to handle signed conversion
				// but have to do back flips to do unsigned (which is asking
				// for extra precision) and therefore is slower.
				*/
				PSTemp  = (sgl_uint32)((sgl_int32)(pPlane->projRepPoint[0]))<<16;
				PSTemp |= (sgl_uint32)((sgl_int32)(pPlane->projRepPoint[1]));

			#endif
	
			IW( pParameterStore, 1, PSTemp);


			/*
			**SMOOTH SHADE PARAMETERS
			**pack up the non-shadowed smooth shade params
			*/
		   	IW( pParameterStore, 0+2,
				((pShadeR->slot[0].smooth.t0 & 0xffffUL) |
				(ConvertRGBtoTexas16(pShadeR->slot[0].smooth.rgbColour)<<16)));

			IW( pParameterStore, 1+2,
				((pShadeR->slot[0].smooth.t1 << 16) |
				(pShadeR->slot[0].smooth.t2 & 0xffffUL)));


			/*
			** pack up the smooth shaded shadow lights
			*/
			if(IsShadowed)	
			{
				IW( pParameterStore, 0+4,
					((pShadeR->slot[1].smooth.t0 & 0xffffUL) |
					(ConvertRGBtoTexas16(pShadeR->slot[1].smooth.rgbColour)<<16)));

		   		IW( pParameterStore, 1+4,
					((pShadeR->slot[1].smooth.t1 << 16) |
					(pShadeR->slot[1].smooth.t2 & 0xffffUL)));

				LocalPStoreIndex+=6; 
				pParameterStore +=6;
			}
			else
			{
				LocalPStoreIndex+=4; 
				pParameterStore +=4;
			}

		}/*end if smooth shaded or not*/

	}/*end for*/


	/*
	// Save where we are up to in the global index
	*/
	PVRParamBuffs[PVR_PARAM_TYPE_TSP].uBufferPos = LocalPStoreIndex;

	SGL_TIME_STOP(PACK_TEXAS_NT_TIME)
}

/******************************************************************************
 * Function Name: PackTexasParamsFlatNonTexturedTrans
 *
 * Inputs       : NumberOfPlanes,
 *				  ShadeR[], an array of shading results.
 *				  IsShadowed,TRUE/FALSE
 *				  Fogon, True/False
 *				  Planes[], the projected rep. point. is used
 * Outputs      : Planes[], the u32TexasTag is written.
 * Returns      : -
 * Globals Used : parameterStore
 *				  parameterStoreIndex
 *
 * Description  : As for PackTexasParams except for non textured planes, and
 *				  assumes that higlight is ALWAYS off.
 *****************************************************************************/
void PackTexasParamsFlatNonTexturedTrans(int				NumberOfPlanes,
					 SHADING_RESULT_STRUCT     *pShadeR,
					 sgl_bool					IsShadowed,
					 sgl_bool					FogOn, 
					 sgl_uint32                     uAlpha,
					 TRANSFORMED_PLANE_STRUCT  *Planes[])
{

	/*
	// Pointer into where we are storing the values, PLUS a local
	// copy of the index, as this will probably be kept in a register
	// and thus be quicker to work with.
	*/
	sgl_uint32 *pParameterStore;
	sgl_uint32 LocalPStoreIndex;
	sgl_uint32 LocalPStoreEnd;
	int nTooMany;
	sgl_uint32 TexAddr;
	/*
	// Precalculate the control word values which will be
	// constant for all the planes
	*/
	sgl_uint32 CommonPstore;

	/*
	// Get quick access to each plane
	*/
	TRANSFORMED_PLANE_STRUCT * pPlane;

	SGL_TIME_START(PACK_TEXAS_NT_TIME)

	TexAddr = TranslucentControlWord;

	/*
	// Get the current parameterstore index, and a pointer to the
	// current place to make additions
	*/
	LocalPStoreIndex = PVRParamBuffs[PVR_PARAM_TYPE_TSP].uBufferPos;
	pParameterStore=&(PVRParamBuffs[PVR_PARAM_TYPE_TSP].pBuffer[LocalPStoreIndex]);
	/* find the end of allowable space and knock of about one planes worth */
	LocalPStoreEnd = PVRParamBuffs[PVR_PARAM_TYPE_TSP].uBufferLimit;

	/* these are all flat shaded translucent planes so
	** they are all 8 words long
	*/
	nTooMany =  (NumberOfPlanes * 8) - (LocalPStoreEnd - LocalPStoreIndex);
	if( nTooMany > 0 )
	{
		/* we want to pack more texas planes than we have space
		** so reduce the number we pack
		*/
		NumberOfPlanes -= (nTooMany / 8);
		ASSERT((NumberOfPlanes > 0));
	}

	CommonPstore = ((uAlpha & 0x0000000F) << 13) | MASK_TRANS | MASK_TEXTURE;
	CommonPstore |= (IsShadowed) ? MASK_SHADOW_FLAG : 0;
	CommonPstore |= (FogOn) ? 0:MASK_DISABLE_FOG;

	/*
	// Go through the planes. Move on to the next shading results and
	// and next plane pointer (NOTE Planes is array of pointers)
	*/
	for(/*Nil*/; NumberOfPlanes !=0; NumberOfPlanes--, pShadeR++, Planes++)
	{
		/*
		// Get pointer to this plane (though current place int 
		// the array of pointers)
		*/
		pPlane = *Planes;

#if PAGE_BREAK_BUG
		/*
		// If we would be using the last word in a page...
		//
		// (We are assuming the maximum possible number of words (12) are to be
		// used as this will only waste about 2% of the texas parameter store
		// and this is only a temporary bug fix.)
		*/
		if ((LocalPStoreIndex & (WORDS_IN_PAGE-1)) > WORDS_IN_PAGE-1-12)
		{
			/*
			// move to start of next page
			*/
			LocalPStoreIndex =
			  ( LocalPStoreIndex & ~(WORDS_IN_PAGE-1) ) + WORDS_IN_PAGE;
			pParameterStore = &(PVRParamBuffs[PVR_PARAM_TYPE_TSP].pBuffer[LocalPStoreIndex]);
		}
#endif
		/*
		** Put the index in the projected plane structure.
		*/
		pPlane->u32TexasTag = LocalPStoreIndex>>1;

		/*
		// if flat shading, pack two base colours
		// The first control word consists of the common bit and 
		// red component which goes at the end
		*/
			
		IW( pParameterStore, 0, (CommonPstore | 
			ConvertColourFloatTo8bit(pShadeR->slot[0].flat.baseColour[0])));
		
 
		/*
		// Add the green and blue components to the shadow colour in
		// the second
		*/
		IW( pParameterStore, 1,
			(ConvertColourFloatTo8bit(pShadeR->slot[0].flat.baseColour[1])<< 24 |
			ConvertColourFloatTo8bit(pShadeR->slot[0].flat.baseColour[2])<< 16 |
			ConvertRGBtoTexas16(pShadeR->slot[1].flat.baseColour)));

		IW( pParameterStore, 2, 1);						/* r */
		IW( pParameterStore, 3, 0);						/* q<<16 | p */
		IW( pParameterStore, 4, (TexAddr << 16));		/* addr<16 | c */
		IW( pParameterStore, 5, 0);						/* b<<16 | a */
		IW( pParameterStore, 6, (TexAddr & 0xFFFF0000));/* addr&FFFF0000 | f */
		IW( pParameterStore, 7, 0);						/* e<<16 | d */
		

		/*
		// Move on to the next spot
		*/
		LocalPStoreIndex+=8; 
		pParameterStore +=8;

		
	}/*end for*/


	/*
	// Save where we are up to in the global index
	*/
	PVRParamBuffs[PVR_PARAM_TYPE_TSP].uBufferPos = LocalPStoreIndex;

	SGL_TIME_STOP(PACK_TEXAS_NT_TIME)
}

/******************************************************************************
 * Function Name: PackTexasParamsSmoothNonTexturedTrans
 *
 * Inputs       : NumberOfPlanes,
 *				  ShadeR[], an array of shading results.
 *				  IsShadowed,TRUE/FALSE
 *				  Fogon, True/False
 *				  Planes[], the projected rep. point. is used
 * Outputs      : Planes[], the u32TexasTag is written.
 * Returns      : -
 * Globals Used : parameterStore
 *				  parameterStoreIndex
 *
 * Description  : As for PackTexasParams except for non textured planes, and
 *				  assumes that higlight is ALWAYS off.
 *****************************************************************************/

void PackTexasParamsSmoothNonTexturedTrans(int				NumberOfPlanes,
					 SHADING_RESULT_STRUCT     *pShadeR,
					 /*sgl_bool					IsTranslucent,*/
					 sgl_bool					FogOn, 
					 TRANSFORMED_PLANE_STRUCT  *Planes[])
{

	/*
	// Pointer into where we are storing the values, PLUS a local
	// copy of the index, as this will probably be kept in a register
	// and thus be quicker to work with.
	*/
	sgl_uint32 *pParameterStore;
	sgl_uint32 LocalPStoreIndex;
	sgl_uint32 LocalPStoreEnd;
	int nTooMany;

	/*
	// Precalculate the control word values which will be
	// constant for all the planes
	*/
	sgl_uint32 CommonPstore;											
	sgl_uint32 PSTemp;

	/*
	// Get quick access to each plane
	*/
	TRANSFORMED_PLANE_STRUCT * pPlane;

	SGL_TIME_START(PACK_TEXAS_NT_TIME)

	/*
	// Get the current parameterstore index, and a pointer to the
	// current place to make additions
	*/
	LocalPStoreIndex = PVRParamBuffs[PVR_PARAM_TYPE_TSP].uBufferPos;
	pParameterStore=&(PVRParamBuffs[PVR_PARAM_TYPE_TSP].pBuffer[LocalPStoreIndex]);
	/* find the end of allowable space and knock of about one planes worth */
	LocalPStoreEnd = PVRParamBuffs[PVR_PARAM_TYPE_TSP].uBufferLimit;

	/* these are all smooth shaded and non-shadowded planes so
	** they are all 4 words long
	*/
	nTooMany =  (NumberOfPlanes * 4) - (LocalPStoreEnd - LocalPStoreIndex);
	if( nTooMany > 0 )
	{
		/* we want to pack more texas planes than we have space
		** so reduce the number we pack
		*/
		NumberOfPlanes -= (nTooMany / 4);
		ASSERT((NumberOfPlanes > 0));
	}

	CommonPstore = MASK_SMOOTH_SHADE;
	CommonPstore |= (FogOn) ? 0:MASK_DISABLE_FOG;

	/*
	// Go through the planes. Move on to the next shading results and
	// and next plane pointer (NOTE Planes is array of pointers)
	*/
	for(/*Nil*/; NumberOfPlanes !=0; NumberOfPlanes--, pShadeR++, Planes++)
	{
		/*
		// Get pointer to this plane (though current place int 
		// the array of pointers)
		*/
		pPlane = *Planes;

#if PAGE_BREAK_BUG
		/*
		// If we would be using the last word in a page...
		//
		// (We are assuming the maximum possible number of words (12) are to be
		// used as this will only waste about 2% of the texas parameter store
		// and this is only a temporary bug fix.)
		*/
		if ((LocalPStoreIndex & (WORDS_IN_PAGE-1)) > WORDS_IN_PAGE-1-12)
		{
			/*
			// move to start of next page
			*/
			LocalPStoreIndex =
			  ( LocalPStoreIndex & ~(WORDS_IN_PAGE-1) ) + WORDS_IN_PAGE;
			pParameterStore = & (PVRParamBuffs[PVR_PARAM_TYPE_TSP].pBuffer[LocalPStoreIndex]);
		}
#endif
		/*
		** Put the index in the projected plane structure.
		*/
		pPlane->u32TexasTag = LocalPStoreIndex>>1;

		IW( pParameterStore, 0, CommonPstore);

		ASSERT(pShadeR->slot[0].smooth.ShadingRepPt[0]>=0.0f)
		ASSERT(pShadeR->slot[0].smooth.ShadingRepPt[1]>=0.0f)
		ASSERT(pShadeR->slot[0].smooth.ShadingRepPt[0]<1024.0f)
		ASSERT(pShadeR->slot[0].smooth.ShadingRepPt[1]<1024.0f)
	
		/*
		// Convert these First to INT then unsigned int as most
		// processors have instructions to handle signed conversion
		// but have to do back flips to do unsigned (which is asking
		// for extra precision) and therefore is slower.
		*/
		PSTemp  = (sgl_uint32)((sgl_int32)(pShadeR->slot[0].smooth.ShadingRepPt[0]))<<16;
		PSTemp |= (sgl_uint32)((sgl_int32)(pShadeR->slot[0].smooth.ShadingRepPt[1]));
	
		IW( pParameterStore, 1, PSTemp);

		/*
		**SMOOTH SHADE PARAMETERS
		**pack up the non-shadowed smooth shade params
		*/
	   	IW( pParameterStore, 0+2,
			((pShadeR->slot[0].smooth.t0 & 0xffffUL) |
			(ConvertRGBtoTexas16(pShadeR->slot[0].smooth.rgbColour)<<16)));

		IW( pParameterStore, 1+2,
			((pShadeR->slot[0].smooth.t1 << 16) |
			(pShadeR->slot[0].smooth.t2 & 0xffffUL)));


		LocalPStoreIndex+=4; 
		pParameterStore +=4;

	}/*end for*/


	/*
	// Save where we are up to in the global index
	*/
	PVRParamBuffs[PVR_PARAM_TYPE_TSP].uBufferPos = LocalPStoreIndex;

	SGL_TIME_STOP(PACK_TEXAS_NT_TIME)
}

/******************************************************************************
 * Function Name: PackTexasParamsFlatNonTextured
 *
 * Inputs       : NumberOfPlanes,
 *				  ShadeR[], an array of shading results.
 *				  IsShadowed,TRUE/FALSE
 *				  Fogon, True/False
 *				  Planes[], the projected rep. point. is used
 * Outputs      : Planes[], the u32TexasTag is written.
 * Returns      : -
 * Globals Used : parameterStore
 *				  parameterStoreIndex
 *
 * Description  : As for PackTexasParams except for non textured planes, and
 *				  assumes that higlight is ALWAYS off.
 *****************************************************************************/
void PackTexasParamsFlatNonTextured(int				NumberOfPlanes,
					 SHADING_RESULT_STRUCT     *pShadeR,
					 sgl_bool					IsShadowed,
					 sgl_bool					FogOn, 
					 TRANSFORMED_PLANE_STRUCT  *Planes[])
{

	/*
	// Pointer into where we are storing the values, PLUS a local
	// copy of the index, as this will probably be kept in a register
	// and thus be quicker to work with.
	*/
	sgl_uint32 *pParameterStore;
	sgl_uint32 LocalPStoreIndex;
	sgl_uint32 LocalPStoreEnd;
	int nTooMany;

	/*
	// Precalculate the control word values which will be
	// constant for all the planes
	*/
	sgl_uint32 CommonPstore;

	/*
	// Get quick access to each plane
	*/
	TRANSFORMED_PLANE_STRUCT * pPlane;

	SGL_TIME_START(PACK_TEXAS_NT_TIME)

	/*
	// Get the current parameterstore index, and a pointer to the
	// current place to make additions
	*/
	LocalPStoreIndex = PVRParamBuffs[PVR_PARAM_TYPE_TSP].uBufferPos;
	pParameterStore=&(PVRParamBuffs[PVR_PARAM_TYPE_TSP].pBuffer[LocalPStoreIndex]);
	/* find the end of allowable space and knock of about one planes worth */
	LocalPStoreEnd = PVRParamBuffs[PVR_PARAM_TYPE_TSP].uBufferLimit;

	/* these are all flat shaded planes so
	** they are all 2 words long
	*/
	nTooMany =  (NumberOfPlanes * 2) - (LocalPStoreEnd - LocalPStoreIndex);
	if( nTooMany > 0 )
	{
		/* we want to pack more texas planes than we have space
		** so reduce the number we pack
		*/
		NumberOfPlanes -= (nTooMany / 2);
		ASSERT((NumberOfPlanes > 0));
	}

	CommonPstore = 0;
	CommonPstore |= (IsShadowed) ? MASK_SHADOW_FLAG : 0;
	CommonPstore |= (FogOn) ? 0:MASK_DISABLE_FOG;

	/*
	// Go through the planes. Move on to the next shading results and
	// and next plane pointer (NOTE Planes is array of pointers)
	*/
	for(/*Nil*/; NumberOfPlanes !=0; NumberOfPlanes--, pShadeR++, Planes++)
	{
		/*
		// Get pointer to this plane (though current place int 
		// the array of pointers)
		*/
		pPlane = *Planes;

#if PAGE_BREAK_BUG
		/*
		// If we would be using the last word in a page...
		//
		// (We are assuming the maximum possible number of words (12) are to be
		// used as this will only waste about 2% of the texas parameter store
		// and this is only a temporary bug fix.)
		*/
		if ((LocalPStoreIndex & (WORDS_IN_PAGE-1)) > WORDS_IN_PAGE-1-12)
		{
			/*
			// move to start of next page
			*/
			LocalPStoreIndex =
			  ( LocalPStoreIndex & ~(WORDS_IN_PAGE-1) ) + WORDS_IN_PAGE;
			pParameterStore = & (PVRParamBuffs[PVR_PARAM_TYPE_TSP].pBuffer[LocalPStoreIndex]);
		}
#endif
		/*
		** Put the index in the projected plane structure.
		*/
		pPlane->u32TexasTag = LocalPStoreIndex>>1;

		/*
		// if flat shading, pack two base colours
		// The first control word consists of the common bit and 
		// red component which goes at the end
		*/
			
		IW( pParameterStore, 0, (CommonPstore | 
			ConvertColourFloatTo8bit(pShadeR->slot[0].flat.baseColour[0])));
		
 
		/*
		// Add the green and blue components to the shadow colour in
		// the second
		*/
		IW( pParameterStore, 1,
			(ConvertColourFloatTo8bit(pShadeR->slot[0].flat.baseColour[1])<< 24 |
			ConvertColourFloatTo8bit(pShadeR->slot[0].flat.baseColour[2])<< 16 |
			ConvertRGBtoTexas16(pShadeR->slot[1].flat.baseColour)));
		

		/*
		// Move on to the next spot
		*/
		LocalPStoreIndex+=2; 
		pParameterStore +=2;

		
	}/*end for*/


	/*
	// Save where we are up to in the global index
	*/
	PVRParamBuffs[PVR_PARAM_TYPE_TSP].uBufferPos = LocalPStoreIndex;

	SGL_TIME_STOP(PACK_TEXAS_NT_TIME)
}

/******************************************************************************
 * Function Name: PackTexasParamsSmoothNonTextured
 *
 * Inputs       : NumberOfPlanes,
 *				  ShadeR[], an array of shading results.
 *				  IsShadowed,TRUE/FALSE
 *				  Fogon, True/False
 *				  Planes[], the projected rep. point. is used
 * Outputs      : Planes[], the u32TexasTag is written.
 * Returns      : -
 * Globals Used : parameterStore
 *				  parameterStoreIndex
 *
 * Description  : As for PackTexasParams except for non textured planes, and
 *				  assumes that higlight is ALWAYS off.
 *****************************************************************************/
void PackTexasParamsSmoothNonTextured(int				NumberOfPlanes,
					 SHADING_RESULT_STRUCT     *pShadeR,
					 /*sgl_bool					IsTranslucent,*/
					 sgl_bool					FogOn, 
					 TRANSFORMED_PLANE_STRUCT  *Planes[])
{

	/*
	// Pointer into where we are storing the values, PLUS a local
	// copy of the index, as this will probably be kept in a register
	// and thus be quicker to work with.
	*/
	sgl_uint32 *pParameterStore;
	sgl_uint32 LocalPStoreIndex;
	sgl_uint32 LocalPStoreEnd;
	int nTooMany;

	/*
	// Precalculate the control word values which will be
	// constant for all the planes
	*/
	sgl_uint32 CommonPstore;
	sgl_uint32 PSTemp;

	/*
	// Get quick access to each plane
	*/
	TRANSFORMED_PLANE_STRUCT * pPlane;

	SGL_TIME_START(PACK_TEXAS_NT_TIME)

	/*
	// Get the current parameterstore index, and a pointer to the
	// current place to make additions
	*/
	LocalPStoreIndex = PVRParamBuffs[PVR_PARAM_TYPE_TSP].uBufferPos;
	pParameterStore=&(PVRParamBuffs[PVR_PARAM_TYPE_TSP].pBuffer[LocalPStoreIndex]);
	/* find the end of allowable space and knock of about one planes worth */
	LocalPStoreEnd = PVRParamBuffs[PVR_PARAM_TYPE_TSP].uBufferLimit;


	/* these are all smooth shaded and non-shadowded planes so
	** they are all 4 words long
	*/
	nTooMany =  (NumberOfPlanes * 4) - (LocalPStoreEnd - LocalPStoreIndex);
	if( nTooMany > 0 )
	{
		/* we want to pack more texas planes than we have space
		** so reduce the number we pack
		*/
		NumberOfPlanes -= (nTooMany / 4);
		ASSERT((NumberOfPlanes > 0));
	}

	CommonPstore = MASK_SMOOTH_SHADE;
	CommonPstore |= (FogOn) ? 0:MASK_DISABLE_FOG;

	/*
	// Go through the planes. Move on to the next shading results and
	// and next plane pointer (NOTE Planes is array of pointers)
	*/
	for(/*Nil*/; NumberOfPlanes !=0; NumberOfPlanes--, pShadeR++, Planes++)
	{
		/*
		// Get pointer to this plane (though current place int 
		// the array of pointers)
		*/
		pPlane = *Planes;

#if PAGE_BREAK_BUG
		/*
		// If we would be using the last word in a page...
		//
		// (We are assuming the maximum possible number of words (12) are to be
		// used as this will only waste about 2% of the texas parameter store
		// and this is only a temporary bug fix.)
		*/
		if ((LocalPStoreIndex & (WORDS_IN_PAGE-1)) > WORDS_IN_PAGE-1-12)
		{
			/*
			// move to start of next page
			*/
			LocalPStoreIndex =
			  ( LocalPStoreIndex & ~(WORDS_IN_PAGE-1) ) + WORDS_IN_PAGE;
			pParameterStore = & ( PVRParamBuffs[PVR_PARAM_TYPE_TSP].pBuffer[LocalPStoreIndex]);
		}
#endif
		/*
		** Put the index in the projected plane structure.
		*/
		pPlane->u32TexasTag = LocalPStoreIndex>>1;

		IW( pParameterStore, 0, CommonPstore);

		ASSERT(pShadeR->slot[0].smooth.ShadingRepPt[0]>=0.0f)
		ASSERT(pShadeR->slot[0].smooth.ShadingRepPt[1]>=0.0f)
		ASSERT(pShadeR->slot[0].smooth.ShadingRepPt[0]<1024.0f)
		ASSERT(pShadeR->slot[0].smooth.ShadingRepPt[1]<1024.0f)
	
		/*
		// Convert these First to INT then unsigned int as most
		// processors have instructions to handle signed conversion
		// but have to do back flips to do unsigned (which is asking
		// for extra precision) and therefore is slower.
		*/
		PSTemp  = (sgl_uint32)((sgl_int32)(pShadeR->slot[0].smooth.ShadingRepPt[0]))<<16;
		PSTemp |= (sgl_uint32)((sgl_int32)(pShadeR->slot[0].smooth.ShadingRepPt[1]));
	
		IW( pParameterStore, 1, PSTemp);

		/*
		**SMOOTH SHADE PARAMETERS
		**pack up the non-shadowed smooth shade params
		*/
	   	IW( pParameterStore, 0+2,
			((pShadeR->slot[0].smooth.t0 & 0xffffUL) |
			(ConvertRGBtoTexas16(pShadeR->slot[0].smooth.rgbColour)<<16)));

		IW( pParameterStore, 1+2,
			((pShadeR->slot[0].smooth.t1 << 16) |
			(pShadeR->slot[0].smooth.t2 & 0xffffUL)));


		LocalPStoreIndex+=4; 
		pParameterStore +=4;

	}/*end for*/


	/*
	// Save where we are up to in the global index
	*/
	PVRParamBuffs[PVR_PARAM_TYPE_TSP].uBufferPos = LocalPStoreIndex;

	SGL_TIME_STOP(PACK_TEXAS_NT_TIME)
}

/******************************************************************************
 * Function Name: PackTexasParamsSmoothNonTexturedShad
 *
 * Inputs       : NumberOfPlanes,
 *				  ShadeR[], an array of shading results.
 *				  Fogon, True/False
 *				  Planes[], the projected rep. point. is used
 * Outputs      : Planes[], the u32TexasTag is written.
 * Returns      : -
 * Globals Used : parameterStore
 *				  parameterStoreIndex
 *
 * Description  : As for PackTexasParams except for non textured planes, and
 *				  assumes that higlight is ALWAYS off.
 *****************************************************************************/
void PackTexasParamsSmoothNonTexturedShad(int				NumberOfPlanes,
					 SHADING_RESULT_STRUCT     *pShadeR,
					 /*sgl_bool					IsTranslucent,*/
					 sgl_bool					FogOn, 
					 TRANSFORMED_PLANE_STRUCT  *Planes[])
{

	/*
	// Pointer into where we are storing the values, PLUS a local
	// copy of the index, as this will probably be kept in a register
	// and thus be quicker to work with.
	*/
	sgl_uint32 *pParameterStore;
	sgl_uint32 LocalPStoreIndex;
	sgl_uint32 LocalPStoreEnd;

	/*
	// Precalculate the control word values which will be
	// constant for all the planes
	*/
	sgl_uint32 CommonPstore;
	sgl_uint32 PSTemp;

	int nTooMany;

	/*
	// Get quick access to each plane
	*/
	TRANSFORMED_PLANE_STRUCT * pPlane;

	SGL_TIME_START(PACK_TEXAS_NT_TIME)

	/*
	// Get the current parameterstore index, and a pointer to the
	// current place to make additions
	*/
	LocalPStoreIndex = PVRParamBuffs[PVR_PARAM_TYPE_TSP].uBufferPos;
	pParameterStore=&(PVRParamBuffs[PVR_PARAM_TYPE_TSP].pBuffer[LocalPStoreIndex]);
	/* find the end of allowable space */
	LocalPStoreEnd = PVRParamBuffs[PVR_PARAM_TYPE_TSP].uBufferLimit;

	/* these are all smooth shaded and shadowded planes so
	** they are all 6 words long
	*/
	nTooMany =  (NumberOfPlanes * 6) - (LocalPStoreEnd - LocalPStoreIndex);
	if( nTooMany > 0 )
	{
		/* we want to pack more texas planes than we have space
		** so reduce the number we pack
		*/
		NumberOfPlanes -= (nTooMany / 6);
		ASSERT((NumberOfPlanes > 0));
	}

	CommonPstore = MASK_SMOOTH_SHADE | MASK_SHADOW_FLAG;
	CommonPstore |= (FogOn) ? 0:MASK_DISABLE_FOG;

	/*
	// Go through the planes. Move on to the next shading results and
	// and next plane pointer (NOTE Planes is array of pointers)
	*/
	for(/*Nil*/; NumberOfPlanes !=0; NumberOfPlanes--, pShadeR++, Planes++)
	{
		/*
		** Get pointer to this plane 
		** (though current place in the array of pointers)
		*/
		pPlane = *Planes;

#if PAGE_BREAK_BUG
		/*
		// If we would be using the last word in a page...
		//
		// (We are assuming the maximum possible number of words (12) are to be
		// used as this will only waste about 2% of the texas parameter store
		// and this is only a temporary bug fix.)
		*/
		if ((LocalPStoreIndex & (WORDS_IN_PAGE-1)) > WORDS_IN_PAGE-1-12)
		{
			/*
			// move to start of next page
			*/
			LocalPStoreIndex =
			  ( LocalPStoreIndex & ~(WORDS_IN_PAGE-1) ) + WORDS_IN_PAGE;
			pParameterStore =
			  & (PVRParamBuffs[PVR_PARAM_TYPE_TSP].pBuffer[LocalPStoreIndex]);
		}
#endif
		/*
		** Put the index in the projected plane structure.
		*/
		pPlane->u32TexasTag = LocalPStoreIndex>>1;

		IW( pParameterStore, 0, CommonPstore);

		ASSERT(pShadeR->slot[0].smooth.ShadingRepPt[0]>=0.0f)
		ASSERT(pShadeR->slot[0].smooth.ShadingRepPt[1]>=0.0f)
		ASSERT(pShadeR->slot[0].smooth.ShadingRepPt[0]<1024.0f)
		ASSERT(pShadeR->slot[0].smooth.ShadingRepPt[1]<1024.0f)
	
		/*
		// Convert these First to INT then unsigned int as most
		// processors have instructions to handle signed conversion
		// but have to do back flips to do unsigned (which is asking
		// for extra precision) and therefore is slower.
		*/
		PSTemp  = (sgl_uint32)((sgl_int32)(pShadeR->slot[0].smooth.ShadingRepPt[0]))<<16;
		PSTemp |= (sgl_uint32)((sgl_int32)(pShadeR->slot[0].smooth.ShadingRepPt[1]));
	
		IW( pParameterStore, 1, PSTemp);

		/*
		**SMOOTH SHADE PARAMETERS
		**pack up the non-shadowed smooth shade params
		*/
	   	IW( pParameterStore, 0+2,
			((pShadeR->slot[0].smooth.t0 & 0xffffUL) |
			(ConvertRGBtoTexas16(pShadeR->slot[0].smooth.rgbColour)<<16)));

		IW( pParameterStore, 1+2,
			((pShadeR->slot[0].smooth.t1 << 16) |
			(pShadeR->slot[0].smooth.t2 & 0xffffUL)));


		/*
		** pack up the smooth shaded shadow lights
		*/
		IW( pParameterStore, 0+4,
			((pShadeR->slot[1].smooth.t0 & 0xffffUL) |
			(ConvertRGBtoTexas16(pShadeR->slot[1].smooth.rgbColour)<<16)));
			
		IW( pParameterStore, 1+4,
			((pShadeR->slot[1].smooth.t1 << 16) |
			(pShadeR->slot[1].smooth.t2 & 0xffffUL)));
			
		LocalPStoreIndex+=6; 
		pParameterStore +=6;

	}/*end for*/


	/*
	// Save where we are up to in the global index
	*/
	PVRParamBuffs[PVR_PARAM_TYPE_TSP].uBufferPos = LocalPStoreIndex;

	SGL_TIME_STOP(PACK_TEXAS_NT_TIME)
}


/******************************************************************************
 * Function Name: PackTexasParamsFlatTextured
 *
 * Inputs       : NumberOfPlanes,
 *				  TextR[], an array of texturing results.
 *				  ShadeR[], an array of shading results.
 *				  IsShadowed,TRUE/FALSE
 *				  IsHighlighted,TRUE/FALSE
 *				  Fogon, True/False
 *				  Planes[], the projected rep. point. is used
 *				  MState, the texas_precomp.LowWord of the material state is used
 * Outputs      : Planes[], the u32TexasTag is written.
 * Returns      : -
 * Globals Used : parameterStore
 *				  parameterStoreIndex
 *
 * Description  : As for PackTexasParams EXCEPT it assumes flat shading
 *				  and texturing
 *****************************************************************************/
void PackTexasParamsFlatTextured(int			NumberOfPlanes,
					 TEXTURING_RESULT_STRUCT   *pTextR,
					 SHADING_RESULT_STRUCT     *pShadeR,
					 sgl_bool					IsShadowed,
					 sgl_bool					FogOn, 
					 TRANSFORMED_PLANE_STRUCT  *Planes[],
			  const MATERIAL_STATE_STRUCT	   *MState)


{

	/*
	// Pointer into where we are storing the values, PLUS a local
	// copy of the index, as this will probably be kept in a register
	// and thus be quicker to work with.
	*/
	sgl_uint32 *pParameterStore;
	sgl_uint32 LocalPStoreIndex;
	sgl_uint32 LocalPStoreEnd;
	sgl_uint32 PSTemp;

	/*
	// Precalculate the control word values which will be
	// constant for all the planes
	*/
	sgl_uint32 CommonPstore;

	/*
	// Get quick access to each plane
	*/
	TRANSFORMED_PLANE_STRUCT * pPlane;

	SGL_TIME_START(PACK_TEXAS_FT_TIME)


	/*
	// Put in the texture details + the flag to turn on
	// flat highlights (we ALWAYS have this on in SGL)
	*/
	CommonPstore=MState->texas_precomp.LowWord | MASK_FLAT_HIGHLIGHT;

	/*
	// Get the current parameterstore index, and a pointer to the
	// current place to make additions
	*/
	/* find the end of allowable space and knock of about one planes worth */
	LocalPStoreEnd = PVRParamBuffs[PVR_PARAM_TYPE_TSP].uBufferLimit - 12;
	LocalPStoreIndex = PVRParamBuffs[PVR_PARAM_TYPE_TSP].uBufferPos;
	pParameterStore=&(PVRParamBuffs[PVR_PARAM_TYPE_TSP].pBuffer[LocalPStoreIndex]);

	CommonPstore |= (IsShadowed) ? MASK_SHADOW_FLAG : 0;
	CommonPstore |= (FogOn) ? 0:MASK_DISABLE_FOG;

	/*
	// Go through the planes. Move on to the next shading results and
	// and next plane pointer (NOTE Planes is array of pointers)
	*/
	for(/*Nil*/; NumberOfPlanes !=0; NumberOfPlanes--, 
									 pShadeR++, pTextR ++, Planes++)
	{

	  /* if this plane might write over the end of valid space break */
		if (LocalPStoreIndex >= LocalPStoreEnd)
		{
			break;
		}
		/*
		// Get pointer to this plane (though current place int 
		// the array of pointers)
		*/
		pPlane = *Planes;

#if PAGE_BREAK_BUG
		/*
		// If we would be using the last word in a page...
		//
		// (We are assuming the maximum possible number of words (12) are to be
		// used as this will only waste about 2% of the texas parameter store
		// and this is only a temporary bug fix.)
		*/
		if ((LocalPStoreIndex & (WORDS_IN_PAGE-1)) > WORDS_IN_PAGE-1-12)
		{
			/*
			// move to start of next page
			*/
			LocalPStoreIndex =
			  ( LocalPStoreIndex & ~(WORDS_IN_PAGE-1) ) + WORDS_IN_PAGE;
			pParameterStore =
			  &(PVRParamBuffs[PVR_PARAM_TYPE_TSP].pBuffer[LocalPStoreIndex]);
		}
#endif
		/*
		** Put the index in the projected plane structure.
		*/
		pPlane->u32TexasTag = LocalPStoreIndex>>1;

		/*
		// flat shading: pack two base colours
		//
		// The first control word consists of the common bit and 
		// red component which goes at the end
		//
		// We also pack in the texture control word as well
		*/
			
		IW( pParameterStore, 0, (CommonPstore | 
			ConvertColourFloatTo8bit(pShadeR->slot[0].flat.baseColour[0]) |
			pTextR->Control1));

 
		/*
		// Add the green and blue components to the shadow colour in
		// the second
		*/
		IW( pParameterStore, 1,
		  (ConvertColourFloatTo8bit(pShadeR->slot[0].flat.baseColour[1])<< 24 |
		  ConvertColourFloatTo8bit(pShadeR->slot[0].flat.baseColour[2])<< 16 |
		  ConvertRGBtoTexas16(pShadeR->slot[1].flat.baseColour)));


		/*
		**pack up the texturing parameters
		**Removed the Super Scalar Jiggery Pokery.
		**This gives us 2% of our performance back.
		**Better check assembly produced by compiler to verify
		**that Temp Variables will give the SuperScalar optimisations sought after. 
		*/
		IW( pParameterStore, 0+2, (pTextR->TexCoeff1));
		IW( pParameterStore, 1+2, (pTextR->TexCoeff2));
		IW( pParameterStore, 2+2, (pTextR->TexCoeff3));
		IW( pParameterStore, 3+2, (pTextR->TexCoeff4));
		IW( pParameterStore, 4+2, (pTextR->TexCoeff5));
		IW( pParameterStore, 5+2, (pTextR->TexCoeff6));

		/*
		**pack up the highlight params
		*/
			
		PSTemp = 
			ConvertRGBtoTexas16(pShadeR->slot[0].flat.highlightColour)<<16;

		if(IsShadowed)
		{
			PSTemp |= 
			  ConvertRGBtoTexas16(pShadeR->slot[1].flat.highlightColour);
		}

		IW( pParameterStore, 0+8, PSTemp);
					
		/*
		// NOTE: We increment by 10 not 9 so as to align the next parameter
		// to a double word boundary
		*/
		LocalPStoreIndex+=10;
		pParameterStore +=10;
	}/*end for*/


	/*
	// Save where we are up to in the global index
	*/
	PVRParamBuffs[PVR_PARAM_TYPE_TSP].uBufferPos = LocalPStoreIndex;

	SGL_TIME_STOP(PACK_TEXAS_FT_TIME)
}

/******************************************************************************
 * Function Name: PackTexasParamsSmoothTextured
 *
 * Inputs       : NumberOfPlanes,
 *				  TextR[], an array of texturing results.
 *				  ShadeR[], an array of shading results.
 *				  IsShadowed,TRUE/FALSE
 *				  Fogon, True/False
 *				  Planes[], the projected rep. point. is used
 *				  MState, the texas_precomp.LowWord of the material state is used
 * Outputs      : Planes[], the u32TexasTag is written.
 * Returns      : -
 * Globals Used : parameterStore
 *				  parameterStoreIndex
 *
 * Description  : As for PackTexasParams except it assumes smooth shading and
 *				  texturing.
 *****************************************************************************/
void PackTexasParamsSmoothTextured(int			NumberOfPlanes,
					 TEXTURING_RESULT_STRUCT   *pTextR,
					 SHADING_RESULT_STRUCT     *pShadeR,
					 sgl_bool					IsShadowed,
					 sgl_bool					FogOn, 
					 TRANSFORMED_PLANE_STRUCT  *Planes[],
				const MATERIAL_STATE_STRUCT	   *MState)
{

	/*
	// Pointer into where we are storing the values, PLUS a local
	// copy of the index, as this will probably be kept in a register
	// and thus be quicker to work with.
	*/
	sgl_uint32 *pParameterStore;
	sgl_uint32 LocalPStoreIndex;
	sgl_uint32 PSTemp;
	sgl_uint32 LocalPStoreEnd;

	/*
	// Precalculate the control word values which will be
	// constant for all the planes
	*/
	sgl_uint32 CommonPstore;

	/*
	// Get quick access to each plane
	*/
	TRANSFORMED_PLANE_STRUCT * pPlane;

	SGL_TIME_START(PACK_TEXAS_ST_TIME)

	/*
	// Put texture and smooth shading bits into header 
	*/
	CommonPstore=MState->texas_precomp.LowWord | MASK_SMOOTH_SHADE;

	/*
	// Get the current parameterstore index, and a pointer to the
	// current place to make additions
	*/
	LocalPStoreIndex = PVRParamBuffs[PVR_PARAM_TYPE_TSP].uBufferPos;
	pParameterStore=&(PVRParamBuffs[PVR_PARAM_TYPE_TSP].pBuffer[LocalPStoreIndex]);
	/* find the end of allowable space and knock of about one planes worth */
	LocalPStoreEnd = PVRParamBuffs[PVR_PARAM_TYPE_TSP].uBufferLimit - 12;

	/*
	// Put in optional bits
	*/	
	CommonPstore |= (IsShadowed) ? MASK_SHADOW_FLAG : 0;
	CommonPstore |= (FogOn) ? 0:MASK_DISABLE_FOG;

	/*
	// Go through the planes. Move on to the next shading results and
	// and next plane pointer (NOTE Planes is array of pointers)
	*/
	for(/*Nil*/;NumberOfPlanes!=0; NumberOfPlanes--, pShadeR++, pTextR++,Planes++)
	{
	  /* if this plane might write over the end of valid space break */
		if (LocalPStoreIndex >= LocalPStoreEnd)
		{
			break;
		}
		/*
		// Get pointer to this plane (though current place int 
		// the array of pointers)
		*/
		pPlane = *Planes;

#if PAGE_BREAK_BUG
		/*
		// If we would be using the last word in a page...
		//
		// (We are assuming the maximum possible number of words (12) are to be
		// used as this will only waste about 2% of the texas parameter store
		// and this is only a temporary bug fix.)
		*/
		if ((LocalPStoreIndex & (WORDS_IN_PAGE-1)) > WORDS_IN_PAGE-1-12)
		{
			/*
			// move to start of next page
			*/
			LocalPStoreIndex =
			  ( LocalPStoreIndex & ~(WORDS_IN_PAGE-1) ) + WORDS_IN_PAGE;
			pParameterStore =
			  & (PVRParamBuffs[PVR_PARAM_TYPE_TSP].pBuffer[LocalPStoreIndex]);
		}
#endif

		/*
		** Put the index in the projected plane structure.
		*/
		pPlane->u32TexasTag = LocalPStoreIndex>>1;

		/*
		// Put the texture control into the first word, and the shading
		// x/y offsets into the second/
		*/
		IW( pParameterStore, 0, (pTextR->Control1 | CommonPstore));

		ASSERT(pShadeR->slot[0].smooth.ShadingRepPt[0]>=0.0f)
		ASSERT(pShadeR->slot[0].smooth.ShadingRepPt[1]>=0.0f)
		ASSERT(pShadeR->slot[0].smooth.ShadingRepPt[0]<1024.0f)
		ASSERT(pShadeR->slot[0].smooth.ShadingRepPt[1]<1024.0f)
	
		/*
		// Convert these First to INT then unsigned int as most
		// processors have instructions to handle signed conversion
		// but have to do back flips to do unsigned (which is asking
		// for extra precision) and therefore is slower.
		*/
		PSTemp  = (sgl_uint32)((sgl_int32)(pShadeR->slot[0].smooth.ShadingRepPt[0]))<<16;
		PSTemp |= (sgl_uint32)((sgl_int32)(pShadeR->slot[0].smooth.ShadingRepPt[1]));

		IW( pParameterStore, 1, PSTemp);


		/*
		**pack up the texturing parameters
		**Removed the Super Scalar Jiggery Pokery.
		**This gives us 2% of our performance back.
		**Better check assembly produced by compiler to verify
		**that Temp Variables will give the SuperScalar optimisations sought after. 
		*/
		IW( pParameterStore, 0+2, (pTextR->TexCoeff1));
		IW( pParameterStore, 1+2, (pTextR->TexCoeff2));
		IW( pParameterStore, 2+2, (pTextR->TexCoeff3));
		IW( pParameterStore, 3+2, (pTextR->TexCoeff4));
		IW( pParameterStore, 4+2, (pTextR->TexCoeff5));
		IW( pParameterStore, 5+2, (pTextR->TexCoeff6));

		/*
		**SMOOTH SHADE PARAMETERS
		**pack up the non-shadowed smooth shade params
		*/
		PSTemp = pShadeR->slot[0].smooth.t0 & 0xffffUL;
		PSTemp |= ConvertRGBtoTexas16(pShadeR->slot[0].smooth.rgbColour)<<16;

		IW( pParameterStore, 0+8, PSTemp);
	   	
		PSTemp =  pShadeR->slot[0].smooth.t1 << 16;
		PSTemp |= pShadeR->slot[0].smooth.t2 & 0xffffUL;
	   	
	   	IW( pParameterStore, 1+8, PSTemp);


		/*
		// Move the pointers on for Header + texture + non shadowed smooth shading
		*/
		LocalPStoreIndex+=10; 
		pParameterStore +=10;

		/*
		** pack up the smooth shaded shadow lights
		*/

		if(IsShadowed)	
		{
			PSTemp = pShadeR->slot[1].smooth.t0 & 0xffffUL;
		   	PSTemp |= ConvertRGBtoTexas16(pShadeR->slot[1].smooth.rgbColour)<<16;

			IW( pParameterStore, 0,  PSTemp);

	   		PSTemp  = pShadeR->slot[1].smooth.t1 << 16;
		   	PSTemp |= pShadeR->slot[1].smooth.t2 & 0xffffUL;

			IW( pParameterStore, 1, PSTemp);

			LocalPStoreIndex+=2; 
			pParameterStore +=2;
		}

	}/*end for*/


	/*
	// Save where we are up to in the global index
	*/
	PVRParamBuffs[PVR_PARAM_TYPE_TSP].uBufferPos = LocalPStoreIndex;

	SGL_TIME_STOP(PACK_TEXAS_ST_TIME)
}

#if PAGE_BREAK_BUG

	#define PAGE_BREAK_BUG_FIX(a,b) PageBreakBugFix((a),(b));

	static INLINE void PageBreakBugFix (sgl_uint32 *pParameterStore,
										sgl_uint32 LocalPStoreIndex)
	{
		/*
		// If we would be using the last word in a page...
		//
		// (We are assuming the maximum possible number of words (12) are to be
		// used as this will only waste about 2% of the texas parameter store
		// and this is only a temporary bug fix.)
		*/
		if ((LocalPStoreIndex & (WORDS_IN_PAGE-1)) > WORDS_IN_PAGE-1-12)
		{
			/*
			// move to start of next page
			*/
			LocalPStoreIndex =
			  ( LocalPStoreIndex & ~(WORDS_IN_PAGE-1) ) + WORDS_IN_PAGE;
			pParameterStore =
			  & (PVRParamBuffs[PVR_PARAM_TYPE_TSP].pBuffer[LocalPStoreIndex]);
		}
	}

#else

	#define PAGE_BREAK_BUG_FIX(a,b)
	
#endif


/*
// END OF FILE
*/


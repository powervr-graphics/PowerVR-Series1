/******************************************************************************
 * Name         : dtsp.c
 * Title        : TSP parameter packing routines for PowerSGL Direct
 * Author       : Jim Page
 * Created      : 14/10/1996
 *
 * Copyright	: 1995-2022 Imagination Technologies (c)
 * License		: MIT
 *
 *
 * Description  : 
 *
 * Platform     : ANSI
 *
 * Modifications:
 * $Log: dtsp.c,v $
 * Revision 1.17  1997/07/24  11:09:30  gdc
 * masked of base colour bits in shad and LV mode
 * which might have been causing an NEC-HE reported bug
 *
 * Revision 1.16  1997/06/09  08:59:11  gdc
 * got rid of compiler warning
 *
 * Revision 1.15  1997/05/02  07:42:44  erf
 * Removed UINT32's etc and replaced with sgl_uint32's etc.
 *
 * Revision 1.14  1997/04/22  12:35:59  mjg
 * Removed a couple of warnings.
 *
 * Revision 1.13  1997/03/14  18:09:37  jop
 *  Added pvrosapi.h include!
 *
 * Revision 1.12  1997/03/14  15:56:32  jwf
 * Added HWIO macros for macintosh compatability
 *
 * Revision 1.11  1997/02/11  14:43:36  ncj
 * Inline function required prototype
 *
 * Revision 1.10  1997/01/24  19:52:43  gdc
 * Added new version of Pack functions which do not write to TSP memory
 * but require the associated texture function to do it - instead they write
 * back to intermediate structures
 *
 * Revision 1.9  1997/01/23  15:29:27  jrc
 * Added decal mode.
 *
 * Revision 1.8  1996/11/27  15:29:07  gdc
 * obseleted Micks version 1.8, which had ^M's on each line and restored
 * his other changes relating to positions of timing macros
 *
 * Revision 1.7  1996/11/21  16:58:51  gdc
 * include extern reference to TranslucentControlWord
 *
 * Revision 1.6  1996/11/21  11:33:41  gdc
 * added translucent flat shading functions - as yet untested
 *
 * Revision 1.5  1996/11/20  18:33:27  gdc
 * finished adding shadows and light vols to this module
 *
 * Revision 1.4  1996/11/19  19:12:13  gdc
 * started adding shadow and light volume options for flat shading
 *
 * Revision 1.3  1996/11/19  11:29:16  rgi
 * Needed a SGL_EXTERN_TIME_REF if METRICs is to work
 *
 * Revision 1.2  1996/11/19  00:58:46  jop
 * Safe nPolys now calculated in dtri.c and pTSP is passed in
 *
 * Revision 1.1  1996/11/18  18:30:16  jop
 * Initial revision
 *
 *
 *****************************************************************************/

#include "sgl_defs.h"
#include "debug.h"
#include "metrics.h"

#include "dtri.h"
#include "dtsp.h"

#include "pvrosapi.h"

/* this is the single pixel texture for NTT 
** this is usually got from rnglobal.h
*/
extern sgl_uint32 TranslucentControlWord;

SGL_EXTERN_TIME_REF /* if we are timing code */


static INLINE void ConvertD3DColtoFractions (sgl_uint32 Colour, 
											 float f24, float f16, 
											 sgl_uint32 *p24, sgl_uint32 *p16);

/**********************************************************************
** Various flat shading options follow, the first versions
** write back into the intermediate structures
** They are presumed to precede texture functions which will actually
** write the data to the TSP buffer - these are required so that when TSP
** double buffering is on that TSP writes will be in order and buffered
** avoiding too many TSP bursts.  
**
**  pTri - pointer to the list of triangles to pack
**  pMat - pointer tomaterial to use for all these triangles
**  numTriangles - do I really need to describe this one
**  nTSPIncrement - the number of DWORDs needed to pack the triangle
**  pTSP - pointer to the TSP parameters 
**
**********************************************************************/

void PackFlatAndStore (PITRI pTri, PIMATERIAL pMat, int numTriangles)
{
	SGL_TIME_START(PACK_TEXAS_TRI_TIME)
	
	/*
	// Go through the planes. Move on to the next shading results and
	// and next plane pointer (NOTE Planes is array of pointers)
	*/
	while (numTriangles--)
	{
		/*
		** Put the index in the projected plane structure.
		*/
		sgl_uint32 uBaseColour = pTri->BaseColour;
		pMat->Shading.u.WriteBack.ModControl = 
			pTri->TSPControlWord | ((uBaseColour >> 16) & 0x000000FF);
		pMat->Shading.u.WriteBack.ModBaseCol = uBaseColour << 16;
		pTri++;
		pMat++;
	}/*end for*/

	SGL_TIME_STOP(PACK_TEXAS_TRI_TIME)
}

/***********************************************************************/

void PackFlatDecalAndStore (PITRI pTri, PIMATERIAL pMat, int numTriangles)
{
	SGL_TIME_START(PACK_TEXAS_TRI_TIME)

	/*
	// There's some scope for optimising this, but decal should not be used
	// very much as it does no lighting.
	*/

	/*
	// Go through the planes. Move on to the next shading results and
	// and next plane pointer (NOTE Planes is array of pointers)
	*/
	while (numTriangles--)
	{
		/*
		** Put the index in the projected plane structure.
		*/
		pMat->Shading.u.WriteBack.ModControl = 
			pTri->TSPControlWord | ((0x00ffffff >> 16) & 0x000000FF);
		pMat->Shading.u.WriteBack.ModBaseCol = (sgl_uint32) (0x00ffffff << 16);
		
		pTri++;
		pMat++;
		
	}/*end for*/

	SGL_TIME_STOP(PACK_TEXAS_TRI_TIME)
}

/***********************************************************************/

void PackFlatShadAndStore (PITRI pTri, PIMATERIAL pMat, int numTriangles)
{	
	float fShadowCoeff = pMat->v.ShadowBrightness;
	float fNonShadowCoeff = 1.0f - fShadowCoeff;

	sgl_uint32 uFlat0Col24, uFlat1Col16;

 	SGL_TIME_START(PACK_TEXAS_TRI_TIME)

	/*
	// Go through the planes. Move on to the next shading results and
	// and next plane pointer (NOTE Planes is array of pointers)
	*/
	while (numTriangles--)
	{
		sgl_uint32 uBaseColour;
		
		/*
		** Put the index in the projected plane structure.
		*/
		uBaseColour = pTri->BaseColour;
		
		ConvertD3DColtoFractions(uBaseColour, fShadowCoeff, fNonShadowCoeff,
								 &uFlat0Col24, &uFlat1Col16);

		pMat->Shading.u.WriteBack.ModControl = 
			pTri->TSPControlWord | ((uFlat0Col24 >> 16) & 0x000000FF);
		pMat->Shading.u.WriteBack.ModBaseCol = uFlat0Col24 << 16 | uFlat1Col16;
		
		pTri++;
		pMat++;
		
	}/*end for*/

	SGL_TIME_STOP(PACK_TEXAS_TRI_TIME)
}

/***********************************************************************/

void PackFlatLiVolAndStore (PITRI pTri, PIMATERIAL pMat, int numTriangles)
{
	sgl_uint32	nFlat1Col16;

	SGL_TIME_START(PACK_TEXAS_TRI_TIME)
	
	nFlat1Col16 = ((pMat->v.LightVolCol & 0x00F80000) >> 19) |
				  ((pMat->v.LightVolCol & 0x0000F800) >> 6) |
				  ((pMat->v.LightVolCol & 0x000000F8) << 7);

	/*
	// Go through the planes. Move on to the next shading results and
	// and next plane pointer (NOTE Planes is array of pointers)
	*/
	while (numTriangles--)
	{
		sgl_uint32 uBaseColour;
		
		/*
		** Put the index in the projected plane structure.
		*/
		uBaseColour = pTri->BaseColour;
		
		pMat->Shading.u.WriteBack.ModControl = 
			pTri->TSPControlWord | ((uBaseColour >> 16)& 0x000000FF);
		pMat->Shading.u.WriteBack.ModBaseCol =  uBaseColour << 16 | nFlat1Col16;
		
		pTri++;
		pMat++;
		
	}/*end for*/

	SGL_TIME_STOP(PACK_TEXAS_TRI_TIME)
}

/**********************************************************************
** Various flat shading options follow, all have same interface
**
**  pTri - pointer to the list of triangles to pack
**  pMat - pointer tomaterial to use for all these triangles
**  numTriangles - do I really need to describe this one
**  nTSPIncrement - the number of DWORDs needed to pack the triangle
**  pTSP - pointer to the TSP parameters 
**
**********************************************************************/

void PackTSPFlat (PITRI pTri, PIMATERIAL pMat, 
				  int numTriangles, int nTSPIncrement, sgl_uint32 *pTSP)
{
	SGL_TIME_START(PACK_TEXAS_TRI_TIME)
	
	/*
	// Go through the planes. Move on to the next shading results and
	// and next plane pointer (NOTE Planes is array of pointers)
	*/
	while (numTriangles--)
	{
		sgl_uint32 uBaseColour;
		
		/*
		** Put the index in the projected plane structure.
		*/
		uBaseColour = pTri->BaseColour;
		
		IW( pTSP, 0, (pTri->TSPControlWord | ((uBaseColour >> 16) & 0x000000FF)));
		IW( pTSP, 1, (uBaseColour << 16));
		
		pTri++;
		pTSP += nTSPIncrement;
		
	}/*end for*/

	SGL_TIME_STOP(PACK_TEXAS_TRI_TIME)
}

void PackTSPFlatDecal (PITRI pTri, PIMATERIAL pMat, 
					   int numTriangles, int nTSPIncrement, sgl_uint32 *pTSP)
{
	SGL_TIME_START(PACK_TEXAS_TRI_TIME)

	/*
	// There's some scope for optimising this, but decal should not be used
	// very much as it does no lighting.
	*/

	/*
	// Go through the planes. Move on to the next shading results and
	// and next plane pointer (NOTE Planes is array of pointers)
	*/
	while (numTriangles--)
	{
		/*
		** Put the index in the projected plane structure.
		*/
		IW( pTSP, 0, (pTri->TSPControlWord | ((0x00ffffff >> 16) & 0x000000FF)));
		IW( pTSP, 1, (sgl_uint32) (0x00ffffff << 16));
		
		pTri++;
		pTSP += nTSPIncrement;
		
	}/*end for*/

	SGL_TIME_STOP(PACK_TEXAS_TRI_TIME)
}

void PackTSPHigh (PITRI pTri, PIMATERIAL pMat, 
				  int numTriangles, int nTSPIncrement, sgl_uint32 *pTSP)
{
	sgl_uint32 HighlightOffset;

	SGL_TIME_START(PACK_TEXAS_TRI_TIME)

	HighlightOffset = (sgl_uint32) nTSPIncrement - 2;
	
	/*
	// Go through the planes. Move on to the next shading results and
	// and next plane pointer (NOTE Planes is array of pointers)
	*/
	while (numTriangles--)
	{
		sgl_uint32 uBaseColour;
		
		/*
		** Put the index in the projected plane structure.
		*/
		uBaseColour = pTri->BaseColour;
		
		IW( pTSP, 0, (pTri->TSPControlWord | ((uBaseColour >> 16) & 0x000000FF)));
		IW( pTSP, 1, (uBaseColour << 16));

		IW( pTSP, HighlightOffset, (pMat->Shading.u.Highlight));
				
		pTri ++;
		pMat ++;
		pTSP += nTSPIncrement;
		
	}/*end for*/

	SGL_TIME_STOP(PACK_TEXAS_TRI_TIME)
}

/***********************************************************************/
/*            Shadow Equivalents of Above                              */
/***********************************************************************/

static INLINE void ConvertD3DColtoFractions (sgl_uint32 Colour, 
											 float f24, float f16, 
											 sgl_uint32 *p24, sgl_uint32 *p16)
{

	float Red =   (float)((Colour & 0x00FF0000) >> 16);
	float Green = (float)((Colour & 0x0000FF00) >> 8);
	float Blue =  (float)(Colour & 0x000000FF);
	
	f16 *= (31.0f / 255.0f);

	/*
	// NOTE: Convert first to SIGNED int as this is MUCH quicker
	// (generally) than conversion straight to UNSIGNED
	*/
	*p24= (sgl_uint32) ( ((sgl_int32)(Blue*f24)) | 
					(((sgl_int32)(Green*f24))<<8) |
					(((sgl_int32)(Red*f24))<<16)
				   );

	*p16= (sgl_uint32) ( ((sgl_int32)(Blue*f16)) | 
					(((sgl_int32)(Green*f16))<<5) |
					(((sgl_int32)(Red*f16))<<10)
				   );
}

/***********************************************************************/

void PackTSPFlatShad (PITRI pTri, PIMATERIAL pMat, 
					  int numTriangles, int nTSPIncrement, sgl_uint32 *pTSP)
{	
	float fShadowCoeff = pMat->v.ShadowBrightness;
	float fNonShadowCoeff = 1.0f - fShadowCoeff;

	sgl_uint32 uFlat0Col24, uFlat1Col16;

 	SGL_TIME_START(PACK_TEXAS_TRI_TIME)

	/*
	// Go through the planes. Move on to the next shading results and
	// and next plane pointer (NOTE Planes is array of pointers)
	*/
	while (numTriangles--)
	{
		sgl_uint32 uBaseColour;
		
		/*
		** Put the index in the projected plane structure.
		*/
		uBaseColour = pTri->BaseColour;
		
		ConvertD3DColtoFractions(uBaseColour, fShadowCoeff, fNonShadowCoeff,
								 &uFlat0Col24, &uFlat1Col16);

		IW( pTSP, 0, (pTri->TSPControlWord | ((uFlat0Col24 >> 16)& 0x000000FF)));
		IW( pTSP, 1, (uFlat0Col24 << 16 | uFlat1Col16));
		
		pTri++;
		pTSP += nTSPIncrement;
		
	}/*end for*/

	SGL_TIME_STOP(PACK_TEXAS_TRI_TIME)
}

/**********************************************************************/

void PackTSPHighShad (PITRI pTri, PIMATERIAL pMat, 
					  int numTriangles, int nTSPIncrement, sgl_uint32 *pTSP)
{
	sgl_uint32 HighlightOffset;

	float fShadowCoeff = pMat->v.ShadowBrightness;
	float fNonShadowCoeff = 1.0f - fShadowCoeff;

	sgl_uint32 uFlat0Col24, uFlat1Col16;


	SGL_TIME_START(PACK_TEXAS_TRI_TIME)

	HighlightOffset = (sgl_uint32) nTSPIncrement - 2;
	
	/*
	// Go through the planes. Move on to the next shading results and
	// and next plane pointer (NOTE Planes is array of pointers)
	*/
	while (numTriangles--)
	{
		sgl_uint32 uBaseColour;
		
		/*
		** Put the index in the projected plane structure.
		*/
		uBaseColour = pTri->BaseColour;
		
		ConvertD3DColtoFractions(uBaseColour, fShadowCoeff, fNonShadowCoeff,
								 &uFlat0Col24, &uFlat1Col16);

		IW( pTSP, 0, (pTri->TSPControlWord | ((uFlat0Col24 >> 16)& 0x000000FF)));
		IW( pTSP, 1, (uFlat0Col24 << 16 | uFlat1Col16));
		
		IW( pTSP, HighlightOffset, (pMat->Shading.u.Highlight));
				
		pTri ++;
		pMat ++;
		pTSP += nTSPIncrement;
		
	}/*end for*/

	SGL_TIME_STOP(PACK_TEXAS_TRI_TIME)
}

/***********************************************************************/
/*                Light Volume Equivalents of Above                    */
/***********************************************************************/

void PackTSPFlatLiVol (PITRI pTri, PIMATERIAL pMat, 
					   int numTriangles, int nTSPIncrement, sgl_uint32 *pTSP)
{
	sgl_uint32	nFlat1Col16;


	SGL_TIME_START(PACK_TEXAS_TRI_TIME)
	
	nFlat1Col16 = ((pMat->v.LightVolCol & 0x00F80000) >> 19) |
				  ((pMat->v.LightVolCol & 0x0000F800) >> 6) |
				  ((pMat->v.LightVolCol & 0x000000F8) << 7);

	/*
	// Go through the planes. Move on to the next shading results and
	// and next plane pointer (NOTE Planes is array of pointers)
	*/
	while (numTriangles--)
	{
		sgl_uint32 uBaseColour;
		
		/*
		** Put the index in the projected plane structure.
		*/
		uBaseColour = pTri->BaseColour;
		
		IW( pTSP, 0, (pTri->TSPControlWord | ((uBaseColour >> 16)& 0x000000FF)));
		IW( pTSP, 1, (uBaseColour << 16 | nFlat1Col16));
		
		pTri++;
		pTSP += nTSPIncrement;
		
	}/*end for*/

	SGL_TIME_STOP(PACK_TEXAS_TRI_TIME)
}

/**********************************************************************/

void PackTSPHighLiVol (PITRI pTri, PIMATERIAL pMat, 
					   int numTriangles, int nTSPIncrement, sgl_uint32 *pTSP)
{
	sgl_uint32 HighlightOffset;
	sgl_uint32	nFlat1Col16;

	SGL_TIME_START(PACK_TEXAS_TRI_TIME)
	
	
	nFlat1Col16 = ((pMat->v.LightVolCol & 0x00F80000) >> 19) |
				  ((pMat->v.LightVolCol & 0x0000F800) >> 6) |
				  ((pMat->v.LightVolCol & 0x000000F8) << 7);

	HighlightOffset = (sgl_uint32) nTSPIncrement - 2;
	
	/*
	// Go through the planes. Move on to the next shading results and
	// and next plane pointer (NOTE Planes is array of pointers)
	*/
	while (numTriangles--)
	{
		sgl_uint32 uBaseColour;
		
		/*
		** Put the index in the projected plane structure.
		*/
		uBaseColour = pTri->BaseColour;
		
		IW( pTSP, 0, (pTri->TSPControlWord | ((uBaseColour >> 16)& 0x000000FF)));
		IW( pTSP, 1, (uBaseColour << 16 | nFlat1Col16));

		IW( pTSP, HighlightOffset, (pMat->Shading.u.Highlight));
				
		pTri ++;
		pMat ++;
		pTSP += nTSPIncrement;
		
	}/*end for*/

	SGL_TIME_STOP(PACK_TEXAS_TRI_TIME)
}

/***********************************************************************
**
**      Non Textured Translucent Versions of All Above
**
************************************************************************/

void PackTSPFlatTrans (PITRI pTri, PIMATERIAL pMat, 
				  int numTriangles, int nTSPIncrement, sgl_uint32 *pTSP)
{
	sgl_uint32 TexAddr;


	SGL_TIME_START(PACK_TEXAS_TRI_TIME)
	
	TexAddr = TranslucentControlWord;

	/*
	// Go through the planes. Move on to the next shading results and
	// and next plane pointer (NOTE Planes is array of pointers)
	*/
	while (numTriangles--)
	{
		sgl_uint32 uBaseColour;
		
		/*
		** Put the index in the projected plane structure.
		*/
		uBaseColour = pTri->BaseColour;
		
		IW( pTSP, 0, (pTri->TSPControlWord | ((uBaseColour >> 16) & 0x000000FF)));
		IW( pTSP, 1, (uBaseColour << 16));

		IW( pTSP, 2, 1);						/* r */
		IW( pTSP, 3, 0);						/* q<<16 | p */
		IW( pTSP, 4, (TexAddr << 16));			/* addr<16 | c */
		IW( pTSP, 5, 0);						/* b<<16 | a */
		IW( pTSP, 6, (TexAddr & 0xFFFF0000));	/* addr&FFFF0000 | f */
		IW( pTSP, 7, 0);						/* e<<16 | d */

		pTri++;
		pTSP += nTSPIncrement;
		
	}/*end for*/

	SGL_TIME_STOP(PACK_TEXAS_TRI_TIME)
}

/**********************************************************************/

void PackTSPHighTrans (PITRI pTri, PIMATERIAL pMat, 
				  int numTriangles, int nTSPIncrement, sgl_uint32 *pTSP)
{
	sgl_uint32 Highlight, TexAddr;
	int HighlightOffset;


	SGL_TIME_START(PACK_TEXAS_TRI_TIME)
	
	Highlight = (pMat->Shading.u.Highlight & 0xFFFF0000);
	TexAddr = TranslucentControlWord;

	HighlightOffset = (sgl_uint32) nTSPIncrement - 2;
	/*
	// Go through the planes. Move on to the next shading results and
	// and next plane pointer (NOTE Planes is array of pointers)
	*/
	while (numTriangles--)
	{
		sgl_uint32 uBaseColour;
		
		/*
		** Put the index in the projected plane structure.
		*/
		uBaseColour = pTri->BaseColour;
		
		IW( pTSP, 0, (pTri->TSPControlWord | ((uBaseColour >> 16) & 0x000000FF)));
		IW( pTSP, 1, (uBaseColour << 16));

		IW( pTSP, 2, 1);						/* r */
		IW( pTSP, 3, 0);						/* q<<16 | p */
		IW( pTSP, 4, (TexAddr << 16));			/* addr<16 | c */
		IW( pTSP, 5, 0);						/* b<<16 | a */
		IW( pTSP, 6, (TexAddr & 0xFFFF0000));	    /* addr&FFFF0000 | f */
		IW( pTSP, 7, 0);						/* e<<16 | d */

		IW( pTSP, HighlightOffset, Highlight);

		pTri++;
		pTSP += nTSPIncrement;
		
	}/*end for*/

	SGL_TIME_STOP(PACK_TEXAS_TRI_TIME)
}

/***********************************************************************/
/*                Shadow Equivalents of Above                          */
/***********************************************************************/

void PackTSPFlatTransShad (PITRI pTri, PIMATERIAL pMat, 
					  int numTriangles, int nTSPIncrement, sgl_uint32 *pTSP)
{
	sgl_uint32 uFlat0Col24, uFlat1Col16;
	sgl_uint32 TexAddr;

	float fShadowCoeff = pMat->v.ShadowBrightness;
	float fNonShadowCoeff = 1.0f - fShadowCoeff;


	SGL_TIME_START(PACK_TEXAS_TRI_TIME)

	TexAddr = TranslucentControlWord;

	/*
	// Go through the planes. Move on to the next shading results and
	// and next plane pointer (NOTE Planes is array of pointers)
	*/
	while (numTriangles--)
	{
		sgl_uint32 uBaseColour;
		
		/*
		** Put the index in the projected plane structure.
		*/
		uBaseColour = pTri->BaseColour;
		
		ConvertD3DColtoFractions(uBaseColour, fShadowCoeff, fNonShadowCoeff,
								 &uFlat0Col24, &uFlat1Col16);

		IW( pTSP, 0, (pTri->TSPControlWord | ((uFlat0Col24 >> 16)& 0x000000FF)));
		IW( pTSP, 1, (uFlat0Col24 << 16 | uFlat1Col16));
		

		IW( pTSP, 2, 1);						/* r */
		IW( pTSP, 3, 0);						/* q<<16 | p */
		IW( pTSP, 4, (TexAddr << 16));			/* addr<16 | c */
		IW( pTSP, 5, 0);						/* b<<16 | a */
		IW( pTSP, 6, (TexAddr & 0xFFFF0000));	/* addr&FFFF0000 | f */
		IW( pTSP, 7, 0);						/* e<<16 | d */

		pTri++;
		pTSP += nTSPIncrement;
		
	}/*end for*/

	SGL_TIME_STOP(PACK_TEXAS_TRI_TIME)
}

/**********************************************************************/

void PackTSPHighTransShad (PITRI pTri, PIMATERIAL pMat, 
					  int numTriangles, int nTSPIncrement, sgl_uint32 *pTSP)
{
	int HighlightOffset;
	sgl_uint32 Highlight, TexAddr;
	sgl_uint32 uFlat0Col24, uFlat1Col16;

	float fShadowCoeff = pMat->v.ShadowBrightness;
	float fNonShadowCoeff = 1.0f - fShadowCoeff;


	SGL_TIME_START(PACK_TEXAS_TRI_TIME)
	
	Highlight = (pMat->Shading.u.Highlight & 0xFFFF0000);
	TexAddr = TranslucentControlWord;

	HighlightOffset = (sgl_uint32) nTSPIncrement - 2;
	
	/*
	// Go through the planes. Move on to the next shading results and
	// and next plane pointer (NOTE Planes is array of pointers)
	*/
	while (numTriangles--)
	{
		sgl_uint32 uBaseColour;
		
		/*
		** Put the index in the projected plane structure.
		*/
		uBaseColour = pTri->BaseColour;
		
		ConvertD3DColtoFractions(uBaseColour, fShadowCoeff, fNonShadowCoeff,
								 &uFlat0Col24, &uFlat1Col16);

		IW( pTSP, 0, (pTri->TSPControlWord | ((uFlat0Col24 >> 16)& 0x000000FF)));
		IW( pTSP, 1, (uFlat0Col24 << 16 | uFlat1Col16));
		
		IW( pTSP, 2, 1);						/* r */
		IW( pTSP, 3, 0);						/* q<<16 | p */
		IW( pTSP, 4, (TexAddr << 16));			/* addr<16 | c */
		IW( pTSP, 5, 0);						/* b<<16 | a */
		IW( pTSP, 6, (TexAddr & 0xFFFF0000));	/* addr&FFFF0000 | f */
		IW( pTSP, 7, 0);						/* e<<16 | d */

		IW( pTSP, HighlightOffset, Highlight);
				
		pTri ++;
		pMat ++;
		pTSP += nTSPIncrement;
		
	}/*end for*/

	SGL_TIME_STOP(PACK_TEXAS_TRI_TIME)
}

/***********************************************************************/
/*                Light Volume Equivalents of Above                    */
/***********************************************************************/

void PackTSPFlatTransLiVol (PITRI pTri, PIMATERIAL pMat, 
					   int numTriangles, int nTSPIncrement, sgl_uint32 *pTSP)
{
	sgl_uint32	nFlat1Col16;
	sgl_uint32  TexAddr;


	SGL_TIME_START(PACK_TEXAS_TRI_TIME)
	
	TexAddr = TranslucentControlWord;

	nFlat1Col16 = ((pMat->v.LightVolCol & 0x00F80000) >> 19) |
				  ((pMat->v.LightVolCol & 0x0000F800) >> 6) |
				  ((pMat->v.LightVolCol & 0x000000F8) << 7);

	/*
	// Go through the planes. Move on to the next shading results and
	// and next plane pointer (NOTE Planes is array of pointers)
	*/
	while (numTriangles--)
	{
		sgl_uint32 uBaseColour;
		
		/*
		** Put the index in the projected plane structure.
		*/
		uBaseColour = pTri->BaseColour;
		
		IW( pTSP, 0, (pTri->TSPControlWord | ((uBaseColour >> 16)& 0x000000FF)));
		IW( pTSP, 1, (uBaseColour << 16 | nFlat1Col16));
		
		IW( pTSP, 2, 1);						/* r */
		IW( pTSP, 3, 0);						/* q<<16 | p */
		IW( pTSP, 4, (TexAddr << 16));			/* addr<16 | c */
		IW( pTSP, 5, 0);						/* b<<16 | a */
		IW( pTSP, 6, (TexAddr & 0xFFFF0000));	/* addr&FFFF0000 | f */
		IW( pTSP, 7, 0);						/* e<<16 | d */

		pTri++;
		pTSP += nTSPIncrement;
		
	}/*end for*/

	SGL_TIME_STOP(PACK_TEXAS_TRI_TIME)
}

/**********************************************************************/

void PackTSPHighTransLiVol (PITRI pTri, PIMATERIAL pMat, 
					   int numTriangles, int nTSPIncrement, sgl_uint32 *pTSP)
{
	int HighlightOffset;
	sgl_uint32	nFlat1Col16;
	sgl_uint32 Highlight, TexAddr;


	SGL_TIME_START(PACK_TEXAS_TRI_TIME)
	
	Highlight = (pMat->Shading.u.Highlight & 0xFFFF0000);
	TexAddr = TranslucentControlWord;

	nFlat1Col16 = ((pMat->v.LightVolCol & 0x00F80000) >> 19) |
				  ((pMat->v.LightVolCol & 0x0000F800) >> 6) |
				  ((pMat->v.LightVolCol & 0x000000F8) << 7);

	HighlightOffset = (sgl_uint32) nTSPIncrement - 2;
	
	/*
	// Go through the planes. Move on to the next shading results and
	// and next plane pointer (NOTE Planes is array of pointers)
	*/
	while (numTriangles--)
	{
		sgl_uint32 uBaseColour;
		
		/*
		** Put the index in the projected plane structure.
		*/
		uBaseColour = pTri->BaseColour;
		
		IW( pTSP, 0, (pTri->TSPControlWord | ((uBaseColour >> 16)& 0x000000FF)));
		IW( pTSP, 1, (uBaseColour << 16 | nFlat1Col16));

		IW( pTSP, 2, 1);						/* r */
		IW( pTSP, 3, 0);						/* q<<16 | p */
		IW( pTSP, 4, (TexAddr << 16));			/* addr<16 | c */
		IW( pTSP, 5, 0);						/* b<<16 | a */
		IW( pTSP, 6, (TexAddr & 0xFFFF0000));	/* addr&FFFF0000 | f */
		IW( pTSP, 7, 0);						/* e<<16 | d */

		IW( pTSP, HighlightOffset, Highlight);
				
		pTri ++;
		pMat ++;
		pTSP += nTSPIncrement;
		
	}/*end for*/

	SGL_TIME_STOP(PACK_TEXAS_TRI_TIME)
}

/* end of $RCSfile: dtsp.c,v $ */


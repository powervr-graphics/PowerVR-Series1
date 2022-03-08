/******************************************************************************
 * Name         : dtex.c
 * Title        : Texture routines for PowerSGL Direct
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
 * $Log: dtex.c,v $
 * Revision 1.47  1997/09/09  18:10:02  erf
 * Insured that max UVs not zero.
 *
 * Revision 1.46  1997/08/29  09:56:02  gdc
 * fixed a bug for flashing fog planes
 *
 * Revision 1.45  1997/08/26  12:43:12  sks
 * Added smooth highlight code in PackExtra.
 *
 * Revision 1.44  1997/08/22  11:59:29  gdc
 * put in short cut for flat shading vfog and re-activated table for vfog
 *
 * Revision 1.43  1997/08/21  16:32:39  gdc
 * tidied up formatting, centred texels (need to be done in table before
 * re-enabling table), removed V's form calculation
 *
 * Revision 1.42  1997/08/21  14:54:37  sks
 * More fixes to PackExtra for the vertex fogging stuff.
 *
 * Revision 1.41  1997/08/19  10:12:58  sks
 * More tidying. Fixed vertex fog texturing calculation.
 * Removed precalc table function and placed precalc'd table
 * in VFOGTAB.H
 *
 * Revision 1.40  1997/08/18  16:15:12  sks
 * Added shortcut for single level per vertex fog and removed
 * a few obvious mistakes.  A little tidying.
 *
 * Revision 1.39  1997/08/18  11:07:08  sks
 * Started filling in VFog/smooth specular function (roughly)
 * and added function to precalculate fog texture coords.
 *
 * Revision 1.38  1997/08/18  10:53:59  gdc
 * added stub (incomplete) VFog/smooth specular function
 *
 * Revision 1.37  1997/06/05  11:00:25  erf
 * Fixed some compiler warnings.
 *
 * Revision 1.36  1997/06/05  08:43:45  erf
 * Fixed compiler warning.
 *
 * Revision 1.35  1997/05/13  17:52:22  erf
 * 1. Tidied up file.
 * 2. Added defines to facilitate changing from C to assembler.
 *
 * Revision 1.34  1997/05/02  07:42:44  erf
 * Removed UINT32's etc and replaced with sgl_uint32's etc.
 *
 * Revision 1.33  1997/03/25  10:18:30  erf
 * 1. Initialised w0, w1, w2 variables for use with texel offset.
 * 2. Removed special bilinear routines since the functionality has been
 *    included in the normal routines. This is tidier.
 *
 * Revision 1.32  1997/03/21  18:08:08  gdc
 * reworked bilinear fix versions to try and make code neater and less
 * ungainly, but old code is still in here
 *
 * Revision 1.31  1997/03/20  18:02:54  jrc
 * Added bilinear versions of processing functions for OpenGL half texel offset
 *
 * Revision 1.30  1997/03/14  18:09:11  jop
 *  Added pvrosapi.h include!
 *
 * Revision 1.29  1997/03/14  15:56:32  jwf
 * Added HWIO macros for macintosh compatability
 *
 * Revision 1.28  1997/03/14  11:05:01  jwf
 * Made some changes to make it compile on the macintosh
 *
 * Revision 1.27  1997/03/12  17:54:20  gdc
 * put in a bit of (disabled) code which adjusts texels by 0.5
 * in d3d texturing
 *
 * Revision 1.26  1997/03/12  16:11:33  gdc
 * fixed a typo
 *
 * Revision 1.25  1997/03/05  12:20:25  gdc
 * made previously mentioned fix only for pcx1 - and fixed the fix for
 * ultimate race
 *
 * Revision 1.24  1997/03/04  18:13:24  gdc
 * fix textured text in wipeout on pcx1
 *
 * Revision 1.23  1997/01/30  17:03:29  rgi
 * Fixed/Reinserted the DEBUG_ASSEMBLER stuff so I can check
 * the assembler texture functions easily.
 *
 * Revision 1.22  1997/01/30  12:45:02  gdc
 * removed TSP double buffer write loops from C versions as assembler
 * will no do this due to lack of time
 *
 * Revision 1.21  1997/01/28  14:47:07  gdc
 * added the size of Material struct to function parameters
 *
 * Revision 1.20  1997/01/27  18:27:35  gdc
 * modified texture functions to support larger PCI writes if required
 * and changed function interface to support bith this and texture function
 * writing all TSP params regardless
 *
 * Revision 1.19  1997/01/27  09:14:55  gdc
 * modified C versions of texture functions to write out all texas data
 * in correct order - this is only the original C functions as yet, not the
 * re-arranged ones on which the assembler is based
 *
 * Revision 1.18  1997/01/24  19:55:15  gdc
 * changed C versions of Mip-Mapped and non-M-M code so that TSP writes for
 * all parameters are performed sequentially - hopefully heading toward getting
 * TSP double buffering working
 *
 * Revision 1.17  1997/01/24  09:20:11  rgi
 * Altered the ProcessTriFlat and other Process cases called from
 * dtri.c to allow for the core code to increment gpMatCurrent
 * rather than the per polygon function. Also abandoned the
 * wrapper to implement perPoly execution time monitoring it
 * distorts the figures too much.
 *
 * Revision 1.16  1997/01/19  15:08:45  sjf
 * Re-ordered matrix multiplies so that it SHOULD reduce the number
 * of FP stalls.
 *
 * Revision 1.15  1997/01/16  13:05:36  rgi
 * Effectively no change, just added a way to monitor
 * the per polygon function ProcessFlatTex when an assembler
 * Process routine is being used.
 *
 * Revision 1.14  1997/01/15  17:59:13  rgi
 * Added the DEBUG_ASSEMBLER code to allow verification
 * of assembler output vs C output for debugging.
 *
 * Revision 1.13  1997/01/15  13:19:05  rgi
 * New version to remove single line comments and upgrade C
 * implementation to match assembler version of same functions
 *
 * Revision 1.12  1997/01/09  17:24:10  rgi
 * And then he found it would not build so he did it again
 *
 * Revision 1.11  1997/01/09  17:04:31  rgi
 * Major re-work to allow the TexMat(Mul)(WrapUV) pass to
 * be built in to the TriProcess pass in dtri.c. This saves
 * the overheads of an extra pass which copies data rather
 * than doing anything useful in dtri.c.
 * Also divided PackTSPTex into MipMapped and non-MipMapped
 * case. The assember version of the PackTSPMipMapTex will
 * be based on this version.
 *
 * Revision 1.10  1996/11/30  02:06:39  sjf
 * Changed local 3 element array to individual variables. It seems
 * to help the compiler a tad.
 *
 * Revision 1.9  1996/11/30  00:37:02  sjf
 * Hopefully done a small amount of optimisation. NOTE: this stuff hasn't
 * been tested yet.
 *
 * Revision 1.8  1996/11/28  11:54:19  sjf
 * Oooooo! Had loads of fun redoing the maths in here so that the matrix
 * operations assume a different basis. With this basis we have 0's in the
 * matrices which, of course, simplifies the maths enormously.
 *
 * Revision 1.7  1996/11/27  17:29:01  gdc
 * added a function to pack dummy translucent texture
 *
 * Revision 1.6  1996/11/25  13:34:34  jop
 * Fixed TexMat dest overwriting source bug
 *
 * Revision 1.5  1996/11/23  02:32:57  jop
 * Moved matrix mul stuff back in to the pack end
 *
 * Revision 1.4  1996/11/22  17:12:37  rgi
 * Removed TEXTURE_PARAMETER_SETUP time monitoring as this
 * gets called .5Million times in a D3DTEST run and makes you
 * think that this area is taking a lot of time to process.
 * Enable on your own local build not the network copy.
 *
 * Revision 1.3  1996/11/19  11:28:54  rgi
 * Needed a SGL_EXTERN_TIME_REF if METRICs is to work
 *
 * Revision 1.2  1996/11/19  00:56:38  jop
 * Cleaned up i/f, pTSP now passed in from dtri.c
 *
 * Revision 1.1  1996/11/18  18:29:56  jop
 * Initial revision
 *
 *
 *****************************************************************************/

#include "sgl_defs.h"
#include "debug.h"
#include "metrics.h"
#include "sgl_math.h"

#include "dtri.h"
#include "dtex.h"

#include "pvrosapi.h"
#include "vfogtab.h"

/* this is the single pixel texture for NTT 
** this is usually got from rnglobal.h
*/
extern sgl_uint32 TranslucentControlWord;

/*
** Per vertex fog texture
*/
extern sgl_uint32 VertexFogControlWord;

SGL_EXTERN_TIME_REF /* if we are timing code */

typedef union
{
	sgl_uint32	l;
	float	f;
} flong;

/* VFOGTAB.H Contains:
** Table for the precalculated U and V coordinates
** used in per vertex fog.
**
** static float fVertFogUVs[4096][3];
** The table is calculated from:

void CalculateVertexFogTable (void)
{
	sgl_uint32 index;

	for (index=0; index<4096; index++)
	{
		fVertFogUVs[index][0] = ((float)((index & 0x00000F00)>>8)) / 16.0f;
		fVertFogUVs[index][1] = ((float)((index & 0x000000F0)>>4)) / 16.0f;;
		fVertFogUVs[index][2] = ((float) (index & 0x0000000F))     / 16.0f;;
	}
}
*/

/**************************************************************************
 * Function Name  : ProcessFlatTex
 * Inputs         : PITRI pTri
 * Outputs        : None.
 * Returns        : None.
 * Global Used    : 
 * Description    : Calculation of the U, V and W values for the perspective
 *					correction case.
 *
 **************************************************************************/

void ProcessFlatTex( PITRI pTri )
{
	PIMATERIAL pMat = gpMatCurrent;
	float W0, W1, W2, U0, U1, U2, V0, V1, V2;
	float tmp0, tmp1, tmp2;
	/*
	// The adjoint is not modified by this routine, so mark it const
	// so that values will not get re-loaded when mem locations are
	// modified.
	//
	// Similarly, convince it that the vertex data is constant as well.
	*/
	const float	*pfAdj = pTri->fAdjoint[0];
	const SGLVERTEX *pV0, *pV1, *pV2;
#define INV(x,y)	(pfAdj[((x)*3)+(y)])

	float off0, off1;
	float fHalfTexel;

	fHalfTexel = gPDC.fHalfTexel;

	pV0 = gPDC.pV0;
	pV1 = gPDC.pV1;
	pV2 = gPDC.pV2;

	/* Do the basic texture mapping function */
	
	/*
	// Do W here since it is required by the half texel fix.
	*/
	W0 = pV0->fInvW;
	W1 = pV1->fInvW;
	W2 = pV2->fInvW;

	if(fHalfTexel)
	{
		U0 = pV0->fUOverW - (fHalfTexel * W0);
		U1 = pV1->fUOverW - (fHalfTexel * W1);
		U2 = pV2->fUOverW - (fHalfTexel * W2);
		/*
		** Do V.   (V don't)
		*/
		V0 = pV0->fVOverW - (fHalfTexel * W0);
		V1 = pV1->fVOverW - (fHalfTexel * W1);
		V2 = pV2->fVOverW - (fHalfTexel * W2);
	}
	else
	{
		U0 = pV0->fUOverW;
		U1 = pV1->fUOverW;
		U2 = pV2->fUOverW;
		/*
		** Do V.   (V don't)
		*/
		V0 = pV0->fVOverW;
		V1 = pV1->fVOverW;
		V2 = pV2->fVOverW;
	}

	off0 = U0 - U2;
	off1 = U1 - U2;

	tmp2 = INV(2,0) * U0   + INV(2,1) * U1 + INV(2,2) * U2;
	tmp0 = INV(0,0) * off0 + INV(0,1) * off1;
	tmp1 = INV(1,0) * off0 + INV(1,1) * off1;

	pMat->Tex.u0 = tmp0;
	pMat->Tex.u1 = tmp1;
	pMat->Tex.u2 = tmp2;


	/*
	** do rest of V
	*/

	off0 = V0 - V2;
	off1 = V1 - V2;
		
	tmp2 = INV(2,0) * V0 + INV(2,1) * V1 + INV(2,2)*V2;
	tmp0 = INV(0,0) * off0 + INV(0,1) * off1;
	tmp1 = INV(1,0) * off0 + INV(1,1) * off1;

	pMat->Tex.v0 = tmp0;
	pMat->Tex.v1 = tmp1;
	pMat->Tex.v2 = tmp2;

	off0 = W0 - W2;
	off1 = W1 - W2;
		
	tmp2 = (INV(2,0) * W0 + INV(2,1) * W1 + INV(2,2) * W2) * gPDC.invTexSize;
	tmp0 = (INV(0,0) * off0 + INV(0,1) * off1) * gPDC.invTexSize;
	tmp1 = (INV(1,0) * off0 + INV(1,1) * off1) * gPDC.invTexSize;

	pMat->Tex.w0 = tmp0;
	pMat->Tex.w1 = tmp1;
	pMat->Tex.w2 = tmp2;
}


/*******************************************************************/

/*
// If we are under D3D then we need to scale and Wrap texture coordinates.
// We do this here so that the core texture code is more consistant for all
// cases. We also do the basic mapping matrix calc here to deliver a fair
// division of labour so both this and the dtex.c pass can be improved
// independantly.
//
// NOTE: FlatTex can still be called by SGL-Lite if the user specifies that
// U&V have not been pre-scaled by 1/w.
*/

/**************************************************************************
 * Function Name  : ProcessD3DFlatTex
 * Inputs         : PITRI pTri
 * Outputs        : None.
 * Returns        : None.
 * Global Used    : 
 * Description    : Calculation of the U, V and W values for the perspective
 *					correction case.
 *
 **************************************************************************/

void ProcessD3DFlatTex( PITRI pTri )
{
	PIMATERIAL pMat = gpMatCurrent;
	float U0, U1, U2, V0, V1, V2;
	float tmp0, tmp1, tmp2;
	float off0, off1;
	const float	*pfAdj = pTri->fAdjoint[0];
	float fHalfTexel;


	fHalfTexel= gPDC.fHalfTexel;

	#if MULTI_FP_REG
		float W0, W1, W2;

		W0 = gPDC.pV0->fInvW;
		W1 = gPDC.pV1->fInvW;
		W2 = gPDC.pV2->fInvW;
	#else
		#define W0  (gPDC.pV0->fInvW)
		#define W1	(gPDC.pV1->fInvW)
		#define W2  (gPDC.pV2->fInvW)
	#endif

	/*
	// Do W 
	*/
	off0 = W0 - W2;
	off1 = W1 - W2;
	
	tmp0 = (INV(0,0) * off0 + INV(0,1) * off1) * gPDC.invTexSize;
	tmp1 = (INV(1,0) * off0 + INV(1,1) * off1) * gPDC.invTexSize;
	tmp2 = (INV(2,0) * W0   + INV(2,1) * W1 + INV(2,2) * W2) * gPDC.invTexSize;

	pMat->Tex.w0 = tmp0;
	pMat->Tex.w1 = tmp1;
	pMat->Tex.w2 = tmp2;
	
	/*
	// Do U. Note: it has to be scaled by W
	*/
	if(fHalfTexel)
	{
	
		/*
		** Do U. Note: it has to be scaled by W
		*/
		U0 = (gPDC.pV0->fUOverW - fHalfTexel )*W0;
		U1 = (gPDC.pV1->fUOverW - fHalfTexel )*W1;
		U2 = (gPDC.pV2->fUOverW - fHalfTexel )*W2;
		/*
		** Do V. Note: it has to be scaled by W
		*/
		V0 = (gPDC.pV0->fVOverW - fHalfTexel) *W0;
		V1 = (gPDC.pV1->fVOverW - fHalfTexel) *W1;
		V2 = (gPDC.pV2->fVOverW - fHalfTexel) *W2;
	}
	else
	{

		U0 = gPDC.pV0->fUOverW*W0;
		U1 = gPDC.pV1->fUOverW*W1;
		U2 = gPDC.pV2->fUOverW*W2;
		/*
		** Do V. Note: it has to be scaled by W
		*/
		V0 = gPDC.pV0->fVOverW *W0;
		V1 = gPDC.pV1->fVOverW *W1;
		V2 = gPDC.pV2->fVOverW *W2;
	}

	off0 = U0 - U2;
	off1 = U1 - U2;
	
	tmp0 = INV(0,0) * off0 + INV(0,1) * off1;
	tmp1 = INV(1,0) * off0 + INV(1,1) * off1;
	tmp2 = INV(2,0) * U0   + INV(2,1) * U1 + INV(2,2) * U2;
	
	pMat->Tex.u0 = tmp0;
	pMat->Tex.u1 = tmp1;
	pMat->Tex.u2 = tmp2;
	
	/* 
	** Do rest of V
	*/

	off0 = V0 - V2;
	off1 = V1 - V2;

	tmp0 = INV(0,0) * off0 + INV(0,1) * off1;
	tmp1 = INV(1,0) * off0 + INV(1,1) * off1;
	tmp2 = INV(2,0) * V0   + INV(2,1) * V1 + INV(2,2)*V2;
	
	pMat->Tex.v0 = tmp0;
	pMat->Tex.v1 = tmp1;
	pMat->Tex.v2 = tmp2;

	#if !MULTI_FP_REG
		#undef W0
		#undef W1
		#undef W2
	#endif

}


/*******************************************************************/
static INLINE float WrapOffset (float d)
{
	/*
	// OK there are 2 versions. The first is normal
	// FP code:
	// It is both an implmentation AND a description of
	// how the 2nd "integer" version for PC actually works.
	*/
	#if !(SLOW_FCMP && !MULTI_FP_REG)

		if (sfabs (d) < 0.5f)
		{
			return (0.0f);
		}

		if (d > 0.0f)
		{
			return (-1.0f);
		}

		return (+1.0f);

	#else

		/*
		// "integer" version.
		//
		// We need to access the bits of 'd'
		*/
		union {
			sgl_int32 i;
			float f;
		} intD;

		intD.f = d;

		/*
		// Is the magnitude of d less than 0.5?
		// Then leave .
		*/
		if ((intD.i & FABS_MASK) < IEEE_HALF)
		{
			return (0.0f);
		}
		/*
		// Else is it negative?
		*/
		else if (intD.i < 0)
		{
			return (+1.0f);
		}
		else
		{
			return (-1.0f);
		}

	#endif


}

/**************************************************************************
 * Function Name  : ProcessD3DFlatTexWrapU
 * Inputs         : PITRI pTri
 * Outputs        : None.
 * Returns        : None.
 * Global Used    : 
 * Description    : Calculation of the U, V and W values for the perspective
 *					correction case.
 *
 **************************************************************************/

void ProcessD3DFlatTexWrapU( PITRI pTri )
{
	PIMATERIAL pMat = gpMatCurrent;
	float U0, U1, U2, V0, V1, V2;
	float off0, off1;
	float tmp0, tmp1, tmp2;
	float fHalfTexel;

	const float	*pfAdj = pTri->fAdjoint[0];

#if MULTI_FP_REG
	float W0, W1, W2;
	
	W0 = gPDC.pV0->fInvW;
	W1 = gPDC.pV1->fInvW;
	W2 = gPDC.pV2->fInvW;
#else
#define W0  (gPDC.pV0->fInvW)
#define W1	(gPDC.pV1->fInvW)
#define W2  (gPDC.pV2->fInvW)
#endif

	fHalfTexel = gPDC.fHalfTexel;

	/*
	// Do U. Note: it has to be "wrapped" and then scaled by W
	*/
	if(fHalfTexel)
	{
		/*
		** Do U. Note: it has to be "wrapped" and then scaled by W
		*/
		U0 = (gPDC.pV0->fUOverW - fHalfTexel);
		U1 = (gPDC.pV1->fUOverW - fHalfTexel);
		U2 = (gPDC.pV2->fUOverW - fHalfTexel);
		/*
		** Do V. Note: it has to be scaled by W
		*/
		V0 = (gPDC.pV0->fVOverW - fHalfTexel) * W0;
		V1 = (gPDC.pV1->fVOverW - fHalfTexel) * W1;
		V2 = (gPDC.pV2->fVOverW - fHalfTexel) * W2;
	}
	else
	{

		U0 = gPDC.pV0->fUOverW;
		U1 = gPDC.pV1->fUOverW;
		U2 = gPDC.pV2->fUOverW;
		V0 = gPDC.pV0->fVOverW * W0;
		V1 = gPDC.pV1->fVOverW * W1;
		V2 = gPDC.pV2->fVOverW * W2;
	}

	U1 += WrapOffset (U1-U0);
	U2 += WrapOffset (U2-U0);

	U0 *= W0;
	U1 *= W1;
	U2 *= W2;

	off0 = U0 - U2;
	off1 = U1 - U2;

	tmp0 = INV(0,0) * off0 + INV(0,1) * off1;
	tmp1 = INV(1,0) * off0 + INV(1,1) * off1;
	tmp2 = INV(2,0) * U0   + INV(2,1) * U1 + INV(2,2) * U2;

	pMat->Tex.u0 = tmp0;
	pMat->Tex.u1 = tmp1;

	/*
	// Do V. Note: it has to be scaled by W
	*/

	/*Moved U store down here so Intel compiler will overlap * and + */
	pMat->Tex.u2 = tmp2; 

	off0 = V0 - V2;
	off1 = V1 - V2;
		
	tmp0 = INV(0,0) * off0 + INV(0,1) * off1;
	tmp1 = INV(1,0) * off0 + INV(1,1) * off1;
	tmp2 = INV(2,0) * V0   + INV(2,1) * V1 + INV(2,2)*V2;

	pMat->Tex.v0 = tmp0;
	pMat->Tex.v1 = tmp1;
	pMat->Tex.v2 = tmp2; 

	/*
	// Do W
	*/
	off0 = W0 - W2;
	off1 = W1 - W2;
		
	tmp0 = (INV(0,0) * off0 + INV(0,1) * off1) * gPDC.invTexSize;
	tmp1 = (INV(1,0) * off0 + INV(1,1) * off1) * gPDC.invTexSize;
	tmp2 = (INV(2,0) * W0   + INV(2,1) * W1 + INV(2,2) * W2) * gPDC.invTexSize;

	pMat->Tex.w0 = tmp0;
	pMat->Tex.w1 = tmp1;
	pMat->Tex.w2 = tmp2;

	#if !MULTI_FP_REG
		#undef W0
		#undef W1
		#undef W2
	#endif

}


/**************************************************************************
 * Function Name  : ProcessD3DFlatTexWrapV
 * Inputs         : PITRI pTri
 * Outputs        : None.
 * Returns        : None.
 * Global Used    : 
 * Description    : Calculation of the U, V and W values for the perspective
 *					correction case.
 *
 **************************************************************************/

void ProcessD3DFlatTexWrapV( PITRI pTri )
{
	PIMATERIAL pMat = gpMatCurrent;
	float U0, U1, U2, V0, V1, V2;
	const float	*pfAdj = pTri->fAdjoint[0];
	float off0, off1;
	float tmp0, tmp1, tmp2;
	float fHalfTexel;

	#if MULTI_FP_REG
		float W0, W1, W2;

		W0 = gPDC.pV0->fInvW;
		W1 = gPDC.pV1->fInvW;
		W2 = gPDC.pV2->fInvW;
	#else
		#define W0  (gPDC.pV0->fInvW)
		#define W1	(gPDC.pV1->fInvW)
		#define W2  (gPDC.pV2->fInvW)
	#endif

	fHalfTexel = gPDC.fHalfTexel;

	/*
	// Do U. Note: it has to be "wrapped" and then scaled by W
	*/
	if(fHalfTexel)
	{
		/*
		** Do U. Note: it has to be scaled by W
		*/
		U0 = (gPDC.pV0->fUOverW - fHalfTexel) * W0;
		U1 = (gPDC.pV1->fUOverW - fHalfTexel) * W1;
		U2 = (gPDC.pV2->fUOverW - fHalfTexel) * W2;
		/*
		** Do V. Note: it has to be "wrapped" and then scaled by W
		*/
		V0 = (gPDC.pV0->fVOverW - fHalfTexel);
		V1 = (gPDC.pV1->fVOverW - fHalfTexel);
		V2 = (gPDC.pV2->fVOverW - fHalfTexel);
	}
	else
	{

		U0 = gPDC.pV0->fUOverW * W0;
		U1 = gPDC.pV1->fUOverW * W1;
		U2 = gPDC.pV2->fUOverW * W2;
		V0 = gPDC.pV0->fVOverW;
		V1 = gPDC.pV1->fVOverW;
		V2 = gPDC.pV2->fVOverW;
	}
	
	/*
	// Do V. Note: it has to be "wrapped" and then scaled by W
	*/

	V1 += WrapOffset (V1-V0);
	V2 += WrapOffset (V2-V0);

	V0 *= W0;
	V1 *= W1;
	V2 *= W2;

	off0 = V0 - V2;
	off1 = V1 - V2;
		
	tmp0 = INV(0,0) * off0 + INV(0,1) * off1;
	tmp1 = INV(1,0) * off0 + INV(1,1) * off1;
	tmp2 = INV(2,0) * V0   + INV(2,1) * V1 + INV(2,2)*V2;

	pMat->Tex.v0 = tmp0;
	pMat->Tex.v1 = tmp1;

	/*
	// Do U. Note: it has to be scaled by W
	*/
	/*Moved V store down here so Intel compiler will overlap * and + */
	pMat->Tex.v2 = tmp2;

	off0 = U0 - U2;
	off1 = U1 - U2;

	tmp0 = INV(0,0) * off0 + INV(0,1) * off1;
	tmp1 = INV(1,0) * off0 + INV(1,1) * off1;
	tmp2 = INV(2,0) * U0   + INV(2,1) * U1 + INV(2,2) * U2;

	pMat->Tex.u0 = tmp0;
	pMat->Tex.u1 = tmp1;
	pMat->Tex.u2 = tmp2;


	/*
	// Do W
	*/
	off0 = W0 - W2;
	off1 = W1 - W2;
		
	tmp0 = (INV(0,0) * off0 + INV(0,1) * off1) * gPDC.invTexSize;
	tmp1 = (INV(1,0) * off0 + INV(1,1) * off1) * gPDC.invTexSize;
	tmp2 = (INV(2,0) * W0   + INV(2,1) * W1 + INV(2,2) * W2) * gPDC.invTexSize;

	pMat->Tex.w0 = tmp0;
	pMat->Tex.w1 = tmp1;
	pMat->Tex.w2 = tmp2;

	#if !MULTI_FP_REG
		#undef W0
		#undef W1
		#undef W2
	#endif

}


/**************************************************************************
 * Function Name  : ProcessD3DFlatTexWrapUV
 * Inputs         : PITRI pTri
 * Outputs        : None.
 * Returns        : None.
 * Global Used    : 
 * Description    : Calculation of the U, V and W values for non perspective
 *					correction case.
 *
 **************************************************************************/

void ProcessD3DFlatTexWrapUV( PITRI pTri )
{
	PIMATERIAL pMat = gpMatCurrent;
	float U0, U1, U2, V0, V1, V2;
	const float	*pfAdj = pTri->fAdjoint[0];
	float off0, off1;
	float tmp0, tmp1, tmp2;
	float fHalfTexel;

	#if MULTI_FP_REG
		float W0, W1, W2;

		W0 = gPDC.pV0->fInvW;
		W1 = gPDC.pV1->fInvW;
		W2 = gPDC.pV2->fInvW;
	#else
		#define W0  (gPDC.pV0->fInvW)
		#define W1	(gPDC.pV1->fInvW)
		#define W2  (gPDC.pV2->fInvW)
	#endif

	/*
	// Do U. Note: it has to be "wrapped" and then scaled by W
	*/
	fHalfTexel = gPDC.fHalfTexel;

	/*
	// Do U. Note: it has to be "wrapped" and then scaled by W
	*/
	if(fHalfTexel)
	{
		/*
		** Do V. Note: it has to be "wrapped" and then scaled by W
		*/
		V0 = (gPDC.pV0->fVOverW - fHalfTexel);
		V1 = (gPDC.pV1->fVOverW - fHalfTexel);
		V2 = (gPDC.pV2->fVOverW - fHalfTexel);
		/*
		** Do U. Note: it has to be "wrapped" and then scaled by W
		*/
		U0 = (gPDC.pV0->fUOverW - fHalfTexel);
		U1 = (gPDC.pV1->fUOverW - fHalfTexel);
		U2 = (gPDC.pV2->fUOverW - fHalfTexel);
	}
	else
	{
		U0 = gPDC.pV0->fUOverW;
		U1 = gPDC.pV1->fUOverW;
		U2 = gPDC.pV2->fUOverW;
		V0 = gPDC.pV0->fVOverW;
		V1 = gPDC.pV1->fVOverW;
		V2 = gPDC.pV2->fVOverW;

	}

	U1 += WrapOffset (U1-U0);
	U2 += WrapOffset (U2-U0);

	U0 *= W0;
	U1 *= W1;
	U2 *= W2;

	off0 = U0 - U2;
	off1 = U1 - U2;

	tmp0 = INV(0,0) * off0 + INV(0,1) * off1;
	tmp1 = INV(1,0) * off0 + INV(1,1) * off1;
	tmp2 = INV(2,0) * U0   + INV(2,1) * U1 + INV(2,2) * U2;

	pMat->Tex.u0 = tmp0;
	pMat->Tex.u1 = tmp1;
	pMat->Tex.u2 = tmp2;


	/*
	// Do V. Note: it has to be "wrapped" and then scaled by W
	*/

	V1 += WrapOffset (V1-V0);
	V2 += WrapOffset (V2-V0);

	V0 *= W0;
	V1 *= W1;
	V2 *= W2;

	off0 = V0 - V2;
	off1 = V1 - V2;
		
	tmp0 = INV(0,0) * off0 + INV(0,1) * off1;
	tmp1 = INV(1,0) * off0 + INV(1,1) * off1;
	tmp2 = INV(2,0) * V0   + INV(2,1) * V1 + INV(2,2)*V2;

	pMat->Tex.v0 = tmp0;
	pMat->Tex.v1 = tmp1;
	pMat->Tex.v2 = tmp2;

	/*
	// Do W
	*/
	off0 = W0 - W2;
	off1 = W1 - W2;
		
	tmp0 = (INV(0,0) * off0 + INV(0,1) * off1) * gPDC.invTexSize;
	tmp1 = (INV(1,0) * off0 + INV(1,1) * off1) * gPDC.invTexSize;
	tmp2 = (INV(2,0) * W0   + INV(2,1) * W1 + INV(2,2) * W2) * gPDC.invTexSize;

	pMat->Tex.w0 = tmp0;
	pMat->Tex.w1 = tmp1;
	pMat->Tex.w2 = tmp2;

	#if !MULTI_FP_REG
		#undef W0
		#undef W1
		#undef W2
	#endif
}


/**************************************************************************
 * Function Name  : PackTSPMipMapTex
 * Inputs         : PIMATERIAL pMat
 *				 	int nPolys
 *					int TypeFlag
 *					sgl_uint32 TSPIncrement
 *					sgl_uint32 *pTSP
 *					int MatSize
 * Outputs        : 
 * Returns        : Number of polys processed.
 * Global Used    : 
 * Description    : This is the perspective texture mip mapping function.
 *
 **************************************************************************/

#if PACKTSPMIPMAPTEX

int PackTSPMipMapTex (PIMATERIAL pMat, 
					  int nPolys, int TypeFlag, 
					  sgl_uint32 TSPIncrement, sgl_uint32 *pTSP, 
					  int MatSize )
{
	float 	largestT,largestB;
	float	compression,approx1;
	float 	dudx,dvdx,dudy,dvdy;
	float	adj_a,adj_b,adj_c,adj_d,adj_e,adj_f,adj_p,adj_q,adj_r;
	flong 	topExponent, MipMap;
	long	exp;
	long	MipMant,MipExp;
#if !SLOW_FCMP || MULTI_FP_REG
	float 	FabsT1,FabsT2;
#endif
	
	while (nPolys--)
	{
		adj_a = pMat->Tex.u0;
		adj_b = pMat->Tex.u1;
		adj_c = pMat->Tex.u2;

		adj_d = pMat->Tex.v0;
		adj_e = pMat->Tex.v1;
		adj_f = pMat->Tex.v2;

		adj_p = pMat->Tex.w0;
		adj_q = pMat->Tex.w1;
		adj_r = pMat->Tex.w2;

		/*
		**	calculate the MIP-map coefficient.
		*/

		dudx= sfabs(adj_a*adj_r - adj_p*adj_c);
		dvdx= sfabs(adj_d*adj_r - adj_p*adj_f);

		/* 
		//approximate the pythagorean distance 
		//
		// IE dist(a,b) = |a| + |b| - 0.585786f*min(|a|,|b|)
		// I think this is supposed to be about 7% out worst case...
		*/
	#if SLOW_FCMP && !MULTI_FP_REG
		if(FLOAT_TO_LONG(dudx) < FLOAT_TO_LONG(dvdx))
	#else
		if(dudx < dvdx)
	#endif
		{
			approx1= (1.0f-0.585786f) * dudx + dvdx;
		}
		else
		{
			approx1=  dudx + (1.0f-0.585786f) * dvdx;
		}

		/* calculate the gradient in U and V with respect to y. */

		dudy= sfabs(adj_b*adj_r - adj_q*adj_c);
		dvdy= sfabs(adj_e*adj_r - adj_q*adj_f);

		/*
		// And again
		*/
	#if SLOW_FCMP && !MULTI_FP_REG
		if(FLOAT_TO_LONG(dudy) < FLOAT_TO_LONG(dvdy))
	#else
		if( dudy < dvdy )
	#endif
		{
			compression= (1.0f-0.585786f) * dudy + dvdy;
		}
		else
		{
			compression= dudy + (1.0f-0.585786f) * dvdy;
		}
		
 		/* 
 		// pick the larger of the distances and store in
		// compression. Originally there was a variable
		// called approx2, but we re-use compression instead.
		*/
		ASSERT(approx1>= 0.0f);
		ASSERT(compression>= 0.0f);

	#if SLOW_FCMP && !MULTI_FP_REG
		if (FLOAT_TO_LONG(approx1) > FLOAT_TO_LONG(compression))
	#else
		if (approx1>compression)
	#endif
		{
		  	compression=approx1;
		}

		/*
		**	rescale 'c','f', and 'r' to keep the coefficients
		**  in a similar range to a,b,d,e,p and q.
		**  Texas will scale them back to the right values.
		*/

		adj_c*=(1.0f/1023.0f); 
		adj_f*=(1.0f/1023.0f);
		adj_r*=(1.0f/1023.0f); 

		/*
		**	find the largest (magnitude) of a,b,c,d,e,f 
		** 
		** On some slower processors floating point compare
		** etc is very slow. Since these values are all positive, we
		** can just as validly "assume" they are integer as the IEEE
		** fp format will work.
		** Note also that fabs can be replaced with a clearing of the
		** TOP bit.
		** This bit of code also assumes that there are B'all FP registers
		** and therefore most FP values are sitting out in memory (not
		** registers)
		*/
		#if SLOW_FCMP && !MULTI_FP_REG

			{
				long  LargestIntEquiv = SMALL_FLOAT_VALUE;
				long  temp;

				/*
				// Get "fabs" of a and compare it with that of a
				// etc etc etc
				*/
				temp = FLOAT_TO_LONG(adj_a) & FABS_MASK;
				if(temp > LargestIntEquiv)
				{
					LargestIntEquiv = temp;
				}

				temp = FLOAT_TO_LONG(adj_b) & FABS_MASK;
				if(temp > LargestIntEquiv)
				{
					LargestIntEquiv = temp;
				}

				temp = FLOAT_TO_LONG(adj_c) & FABS_MASK;
				if(temp > LargestIntEquiv)
				{
					LargestIntEquiv = temp;
				}

				temp = FLOAT_TO_LONG(adj_d) & FABS_MASK;
				if(temp > LargestIntEquiv)
				{
					LargestIntEquiv = temp;
				}


				temp = FLOAT_TO_LONG(adj_e) & FABS_MASK;
				if(temp > LargestIntEquiv)
				{
					LargestIntEquiv = temp;
				}

				temp = FLOAT_TO_LONG(adj_f) & FABS_MASK;
				if(temp > LargestIntEquiv)
				{
					LargestIntEquiv = temp;
				}

				largestT = LONG_TO_FLOAT(LargestIntEquiv);
			}
			/*
			// Use FP compares to get the largest
			*/

		#else
			largestT= SMALL_FLOAT_VALUE;

			/* 
			// make a temp copy of the result of the sfabs() because
			// this may be faster on compilers that call an sfabs
			// function
			*/
			FabsT1=sfabs(adj_a);
			if(FabsT1>largestT)
				largestT=FabsT1;

			FabsT1=sfabs(adj_b);
			if(FabsT1>largestT)
				largestT=FabsT1;

			FabsT2=sfabs(adj_c);
			if(FabsT2>largestT)
				largestT=FabsT2;

			FabsT1=sfabs(adj_d);
			if(FabsT1>largestT)
				largestT=FabsT1;

			FabsT2=sfabs(adj_e);
			if(FabsT2>largestT)
				largestT=FabsT2;

			FabsT1=sfabs(adj_f);
			if(FabsT1>largestT)
				largestT=FabsT1;
		#endif



		/*
		** find the largest of p,q,r 
		**
		** Again use a suitable method...
		*/
	#if SLOW_FCMP && !MULTI_FP_REG
		{
			long  LargestIntEquiv;
			long  temp;

			/*
			// Get the abs of p, and "convert" it to int.
			*/
			LargestIntEquiv = FLOAT_TO_LONG(adj_p) & FABS_MASK;


			temp = FLOAT_TO_LONG(adj_q) & FABS_MASK;
			if(temp > LargestIntEquiv)
			{
				LargestIntEquiv = temp;
			}

			temp = FLOAT_TO_LONG(adj_r) & FABS_MASK;
			if(temp > LargestIntEquiv)
			{
				LargestIntEquiv = temp;
			}

			largestB = LONG_TO_FLOAT(LargestIntEquiv);
		}
	#else
		largestB= sfabs(adj_p);

		FabsT1=sfabs(adj_q);
		if (FabsT1>largestB)
			  largestB=FabsT1;

		FabsT2=sfabs(adj_r);
		if (FabsT2>largestB)
			  largestB=FabsT2;
	#endif

		/*
		** calculate a fast floor(log2(largest/largest_bot))
		*/

		largestT=1.0f/largestT;

		topExponent.f=largestB*largestT;

		/* extract the offset exponent from the IEEE fp number */

		exp=(topExponent.l>>23) & 0xff; 	

		/* calculate the reciprocal (ignore post normalising) */

		exp=126-exp;

		/*
		** rescale the texturing coefficients to 16 bits.
		*/

		largestT=largestT * 32767.0f;

		adj_a=adj_a * largestT;
		adj_b=adj_b * largestT;
		adj_c=adj_c * largestT;
		adj_d=adj_d * largestT;
		adj_e=adj_e * largestT;
		adj_f=adj_f * largestT;

		/* calculate a fast pow(2.0,floor())*largestT */

		topExponent.l=(exp+127)<<23;

		largestT=largestT * topExponent.f;

		adj_p=adj_p * largestT;
		adj_q=adj_q * largestT;
		adj_r=adj_r * largestT;

		/* solve the compression for 'n'.Compression is
		   pre-multiplied by pqr*pqr, so we only have to multiply by the
		   square of the rescale value */

		MipMap.f=compression*largestT*largestT;

		/* convert from IEEE to the TEXAS floating point format*/

		MipMant =(MipMap.l>>16)&0x7f;
		MipMant+=128; /*add in the implied 0.5*/
		/*126 because of the different decimal point*/
		MipExp = (MipMap.l>>23)-126 + gPDC.n32MipmapOffset;

		/*
		** Texas can't handle an exponent greater than 15,so we will
		** reduce the resolution of 'p', 'q', and 'r'.
		** THIS SHOULD HAPPEN **VERY** RARELY.The Level Of Detail
		** should have removed the texture by now.
		*/

		if(exp>15)
		{
			adj_p=(float)((sgl_int32)adj_p >>(exp-15));
			adj_q=(float)((sgl_int32)adj_q >>(exp-15));
			adj_r=(float)((sgl_int32)adj_r >>(exp-15));

			MipExp-=(exp-15)<<1;

			exp=15;
		}
		
		/*
		** Texas can't handle a negative exponent,so we will
		** reduce the resolution of a,b,c,d,e and f.
		** This condition only happens if the texture is VERY zoomed.
		** It may not be worth the expense of testing for this.
		*/
		else if(exp<0)
		{
			adj_a=(float)((sgl_int32)adj_a >>(0-exp));
			adj_b=(float)((sgl_int32)adj_b >>(0-exp));
			adj_c=(float)((sgl_int32)adj_c >>(0-exp));
			adj_d=(float)((sgl_int32)adj_d >>(0-exp));
			adj_e=(float)((sgl_int32)adj_e >>(0-exp));
			adj_f=(float)((sgl_int32)adj_f >>(0-exp));

			exp=0;
		}

		/*
		**  now pack up the data in TEXAS format
		*/
		/*first instruction word*/

		IW( pTSP, 0, (pMat->Shading.u.WriteBack.ModControl | ((sgl_uint32) exp << SHIFT_EXPONENT)));
		IW( pTSP, 1, (pMat->Shading.u.WriteBack.ModBaseCol));

		/*texture parameters*/ 

		IW( pTSP, 2, (((sgl_uint32) MipMant << SHIFT_PMIP_M) |
				  ((sgl_uint32) MipExp  << SHIFT_PMIP_E) |
				  (((sgl_int32) adj_r) & 0xffffUL)));

        IW( pTSP, 3, (((sgl_int32) adj_q << 16) | (((sgl_int32) adj_p) & 0xffffUL)));

        IW( pTSP, 4, ((gPDC.TexAddress << 16) | (((sgl_int32) adj_c) & 0xffffUL)));

        IW( pTSP, 5, (((sgl_int32) adj_b << 16) | (((sgl_int32) adj_a) & 0xffffUL)));

        IW( pTSP, 6, ((gPDC.TexAddress & 0xffff0000) | (((sgl_int32) adj_f) & 0xffffUL)));

        IW( pTSP, 7, (((sgl_int32) adj_e << 16) | (((sgl_int32) adj_d) & 0xffffUL)));

		switch(TypeFlag)
		{
		case(0):
			break;
		case(1):
			IW( pTSP, 8, (pMat->Shading.u.WriteBack.Smooth1));
			IW( pTSP, 9, (pMat->Shading.u.WriteBack.Smooth2));
			break;
		case(2):
			IW( pTSP, 8, (pMat->Shading.u.Highlight));
			break;
		case(4):
			IW( pTSP, 8, (pMat->Shading.u.WriteBack.Smooth1));
			IW( pTSP, 9, (pMat->Shading.u.WriteBack.Smooth2));
			IW( pTSP, 10, (pMat->v.LightVolCol));
			IW( pTSP, 11, (pMat->Shading.u.Highlight));
			break;
		default:
			ASSERT((TypeFlag!=TypeFlag));
			break;
		}

		pMat++;
		pTSP += TSPIncrement;	
	}
	
	return (nPolys);
}
#else	/* #if PACKTSPMIPMAPTEX	*/
	#if 0
		/* This is what the assembler version for PC was actually made from */

		#define DEBUG_ASSEMBLER 1	/* See the same code in dtexml.asm */

		/* 1 has exp of 127 so 1<<23 is what we need */
		#define MAGIC_FISTP_SINGLE_LONG (((127L+23L)<<23)+0x00400000L)

		#if DEBUG_ASSEMBLER
		int PackTSPMipMapTexInC (PIMATERIAL pMat, int nPolys, int TypeFlag, 
							     sgl_uint32 TSPIncrement, sgl_uint32 *pTSP, int MatSize)
		#else
		int PackTSPMipMapTex (PIMATERIAL pMat, int nPolys,
							  int TypeFlag, sgl_uint32 TSPIncrement, 
							  sgl_uint32 *pTSP, int MatSize)
		#endif
		{
			flong 	largestT, largestB;
			flong 	topExponent, intconv[2], compress;
			flong	intconvAF, intconvPR, intconvt[2];
			long	exp, MipMant, MipExp, temp;
			flong	u2, v2, w2;
			sgl_uint32 *pTSPLW = pTSP;
			int nPlanes = nPolys;
			

			while (nPolys--)
			{
				/*
				**	calculate the MIP-map coefficient.
				*/

				intconv[0].f = sfabs(pMat->Tex.u0*pMat->Tex.w2 - pMat->Tex.w0*pMat->Tex.u2);
				intconv[1].f = sfabs(pMat->Tex.v0*pMat->Tex.w2 - pMat->Tex.w0*pMat->Tex.v2);

				/* 
				//approximate the pythagorean distance 
				//
				// IE dist(a,b) = |a| + |b| - 0.585786f*min(|a|,|b|)
				// I think this is supposed to be about 7% out worst case...
				*/
				
				/* Load -1 if intconv[0].l < intconv[1].l else 0; only safe if both >= 0 */
				temp = ((long) (intconv[0].l - intconv[1].l)) >> 31;
				
				/* If temp is -1 scale intconv[0].f, else scale intconv[1].f */
				intconvt[0].f = (1.0f-0.585786f) * intconv[1+temp].f + intconv[-temp].f;

				/* calculate the gradient in U and V with respect to y. */
				intconv[0].f = sfabs(pMat->Tex.u1*pMat->Tex.w2 - pMat->Tex.w1*pMat->Tex.u2);
				intconv[1].f = sfabs(pMat->Tex.v1*pMat->Tex.w2 - pMat->Tex.w1*pMat->Tex.v2);

				/* Load -1 if intconv[0].l < intconv[1].l else 0; only safe if both >= 0 */
				temp = ((long) (intconv[0].l - intconv[1].l)) >> 31;

				/* If temp is -1 scale intconv[0].f, else scale intconv[1].f */
				intconvt[1].f = (1.0f-0.585786f) * intconv[1+temp].f + intconv[-temp].f;
				
				/*
				**	rescale 'c','f', and 'r' to keep the coefficients
				**  in a similar range to a,b,d,e,p and q.
				**  Texas will scale them back to the right values.
				*/

				u2.f = pMat->Tex.u2*(1.0f/1023.0f); 
				v2.f = pMat->Tex.v2*(1.0f/1023.0f);
				w2.f = pMat->Tex.w2*(1.0f/1023.0f); 
				
		 		/* 
		 		// pick the larger of the distances and store in
				// compress.f. Originally there was a variable
				// called approx2, but we re-use compress.f instead.
				*/
				ASSERT(intconvt[0].f>= 0.0f);
				ASSERT(intconvt[1].f>= 0.0f);

				/* Again this is only safe if both intconv[0 & 1] are >= 0 */
				compress.l = intconvt[1+(((long) (intconvt[1].l - intconvt[0].l))>>31)].l;

				/*
				**	find the largest (magnitude) of a,b,c,d,e,f 
				** 
				** On some slower processors floating point compare
				** etc is very slow. Since these values are all positive, we
				** can just as validly "assume" they are integer as the IEEE
				** fp format will work.
				** Note also that fabs can be replaced with a clearing of the
				** TOP bit.
				** This bit of code also assumes that there are B'all FP registers
				** and therefore most FP values are sitting out in memory (not
				** registers)
				*/
				{
					/*
					// Get the abs of a, and "convert" it to int.
					*/
					largestT.l = FLOAT_TO_LONG(pMat->Tex.u0) & FABS_MASK;


					/*
					// Get "fabs" of b and compare it with that of a
					// etc etc etc
					*/
					temp = FLOAT_TO_LONG(pMat->Tex.u1) & FABS_MASK;
					if(temp > largestT.l)
					{
						largestT.l = temp;
					}

					temp = u2.l & FABS_MASK;
					if(temp > largestT.l)
					{
						largestT.l = temp;
					}

					temp = FLOAT_TO_LONG(pMat->Tex.v0) & FABS_MASK;
					if(temp > largestT.l)
					{
						largestT.l = temp;
					}


					temp = FLOAT_TO_LONG(pMat->Tex.v1) & FABS_MASK;
					if(temp > largestT.l)
					{
						largestT.l = temp;
					}

					temp = v2.l & FABS_MASK;
					if(temp > largestT.l)
					{
						largestT.l = temp;
					}
				}

				/* Let's do this while we do some integer stuff please! */
				largestT.f = 1.0f/largestT.f;

				/*
				** find the largest of p,q,r 
				**
				** Again use a suitable method...
				*/
				{
					/*
					// Get the abs of p, and "convert" it to int.
					*/
					largestB.l = FLOAT_TO_LONG(pMat->Tex.w0) & FABS_MASK;


					temp = FLOAT_TO_LONG(pMat->Tex.w1) & FABS_MASK;
					if(temp > largestB.l)
					{
						largestB.l = temp;
					}

					temp = w2.l & FABS_MASK;
					if(temp > largestB.l)
					{
						largestB.l = temp;
					}
				}

				/*
				** calculate a fast floor(log2(largest/largest_bot))
				*/

				topExponent.f=largestB.f*largestT.f;

				/* extract the offset exponent from the IEEE fp number */

				exp=(topExponent.l>>23) & 0xff; 	

				/* calculate the reciprocal (ignore post normalising) */

				exp=126-exp;

				/*
				** rescale the texturing coefficients to 16 bits.
				*/

				largestT.f=( largestT.f * 32767.0f );

				/* calculate a fast pow(2.0,floor())*largestT.f */

				topExponent.l=(exp+127)<<23;

				/* Setup for common case with no scaling requirement */
				intconvAF.l = MAGIC_FISTP_SINGLE_LONG;
				intconvPR.l = MAGIC_FISTP_SINGLE_LONG;
				MipExp = 0;

				if(exp>15)
				{
					/* adj_(p to r) =(float)((sgl_int32)adj_(p to r) >>(exp-15)); */
					intconvPR.l += ((exp-15)<<23);

					/* Offset MipExp by same amount */			
					MipExp = -((exp-15)<<1);

					/* Clip max exponent specified to Texas */
					exp=15;
				}
				else
				if(exp < 0)
				{
					/* adj_(a to f) =(float)((sgl_int32)adj_(a to f) >>(0-exp)); */
					intconvAF.l += ((0-exp)<<23);

					/* Clip max exponent specified to Texas */
					exp=0;
				}

				pTSP[0] = 
					pMat->Shading.u.WriteBack.ModControl | ((sgl_uint32) exp << SHIFT_EXPONENT);
				pTSP[1] = pMat->Shading.u.WriteBack.ModBaseCol;


				intconv[0].f =(u2.f * largestT.f) + intconvAF.f;

		        pTSP[4] = (gPDC.TexAddress << 16) | (intconv[0].l & 0xffffUL);

				intconv[0].f =(pMat->Tex.u0 * largestT.f) + intconvAF.f;
				intconv[1].f =(pMat->Tex.u1 * largestT.f) + intconvAF.f;

		        pTSP[5] = (intconv[1].l << 16) | (intconv[0].l & 0xffffUL);
				
				intconv[0].f =(v2.f * largestT.f) + intconvAF.f;
				
		        pTSP[6] = (gPDC.TexAddress & 0xffff0000) | (intconv[0].l & 0xffffUL);
				
				intconv[0].f =(pMat->Tex.v0 * largestT.f) + intconvAF.f;
				intconv[1].f =(pMat->Tex.v1 * largestT.f) + intconvAF.f;

		        pTSP[7] = (intconv[1].l << 16) | (intconv[0].l & 0xffffUL);

				largestT.f=largestT.f * topExponent.f;

				intconv[0].f =(pMat->Tex.w0 * largestT.f) + intconvPR.f;
				intconv[1].f =(pMat->Tex.w1 * largestT.f) + intconvPR.f;

		        pTSP[3] = (intconv[1].l << 16) | (intconv[0].l & 0xffffUL);

				intconv[0].f =(w2.f * largestT.f) + intconvPR.f;

				/* solve the compression for 'n'.Compression is
				   pre-multiplied by pqr*pqr, so we only have to multiply by the
				   square of the rescale value */
				compress.f=compress.f*largestT.f*largestT.f;

				/* convert from IEEE to the TEXAS floating point format*/
				MipMant =(compress.l>>16)&0x7f;
				MipMant+=128; /*add in the implied 0.5*/
				/*126 because of the different decimal point*/
				MipExp += (compress.l>>23)-126 + gPDC.n32MipmapOffset;

				pTSP[2] = ((sgl_uint32) MipMant << SHIFT_PMIP_M) |
						  ((sgl_uint32) MipExp  << SHIFT_PMIP_E) |
						  (intconv[0].l & 0xffffUL);
						  
				switch(TypeFlag)
				{
				case(0):
					break;
				case(1):
					pTSP[8] = pMat->Shading.u.WriteBack.Smooth1;
					pTSP[9] = pMat->Shading.u.WriteBack.Smooth2;
					break;
				case(2):
					pTSP[8] = pMat->Shading.u.Highlight;
					break;
				case(4):
					pTSP[8] = pMat->Shading.u.WriteBack.Smooth1;
					pTSP[9] = pMat->Shading.u.WriteBack.Smooth2;
					pTSP[10] = pMat->v.LightVolCol;
					pTSP[11] = pMat->Shading.u.Highlight;
					break;
				default:
					ASSERT((TypeFlag!=TypeFlag));
					break;
				}
				pMat ++;
				pTSP += TSPIncrement;	
			}

			return (nPolys);
		}

		#if DEBUG_ASSEMBLER
		int PackTSPMipMapTex (PIMATERIAL pMat, int nPolys, int TypeFlag, 
							  sgl_uint32 TSPIncrement, sgl_uint32 *pTSP, int MatSize)
		{
			sgl_uint32 FakeTSP[20];
			
			while ( nPolys != 0 )
			{
				PackTSPMipMapTexInAsm(pMat, 1, TypeFlag, TSPIncrement, FakeTSP, MatSize);
				PackTSPMipMapTexInC(pMat, 1, TypeFlag, TSPIncrement, pTSP, MatSize);
				
				if ( FakeTSP[0] != pTSP[0] )
				{
					__asm { int 3; }
				}
				else
				if ( FakeTSP[1] != pTSP[1] )
				{
					__asm { int 3; }
				}
				else
				if ( FakeTSP[2] != pTSP[2] )
				{
					__asm { int 3; }
				}
				else
				if ( FakeTSP[3] != pTSP[3] )
				{
					__asm { int 3; }
				}
				else
				if ( FakeTSP[4] != pTSP[4] )
				{
					__asm { int 3; }
				}
				else
				if ( FakeTSP[5] != pTSP[5] )
				{
					__asm { int 3; }
				}
				else
				if ( FakeTSP[6] != pTSP[6] )
				{
					__asm { int 3; }
				}
				else
				if ( FakeTSP[7] != pTSP[7] )
				{
					__asm { int 3; }
				}
				else
				{
					/* Done one correctly */
					(sgl_uint32) pMat += MatSize;
					pTSP += TSPIncrement;
					
					nPolys--;
				}
			}
		}
		#endif		/* #if DEBUG_ASSEMBLER	*/
	#else		/* #if 0				*/
		extern int PackTSPMipMapTex (PIMATERIAL pMat, 
							  int nPolys, int TypeFlag, 
							  sgl_uint32 TSPIncrement, sgl_uint32 *pTSP, 
							  int MatSize );
	#endif		/* #if 0				*/
#endif		/* #if PACKTSPMIPMAPTEX	*/


/**************************************************************************
 * Function Name  : PackTSPTex
 * Inputs         : PIMATERIAL pMat
 *				 	int nPolys
 *					int TypeFlag
 *					sgl_uint32 TSPIncrement
 *					sgl_uint32 *pTSP
 *					int MatSize
 * Outputs        : 
 * Returns        : Number of polys processed.
 * Global Used    : 
 * Description    : This is the perspective texture function.
 *
 **************************************************************************/

#if PACKTSPTEX
int PackTSPTex (PIMATERIAL pMat, 
				int nPolys, int TypeFlag, 
				sgl_uint32 TSPIncrement, 
				sgl_uint32 *pTSP, int MatSize)
{
	float 	largestT,largestB;
	float	adj_a,adj_b,adj_c,adj_d,adj_e,adj_f,adj_p,adj_q,adj_r;
	flong 	topExponent;
	long	exp;
#if !SLOW_FCMP || MULTI_FP_REG
	float 	FabsT1,FabsT2;
#endif

	while (nPolys--)
	{
		adj_a = pMat->Tex.u0;
		adj_b = pMat->Tex.u1;
		adj_c = pMat->Tex.u2;

		adj_d = pMat->Tex.v0;
		adj_e = pMat->Tex.v1;
		adj_f = pMat->Tex.v2;

		adj_p = pMat->Tex.w0;
		adj_q = pMat->Tex.w1;
		adj_r = pMat->Tex.w2;

		/*
		**	rescale 'c','f', and 'r' to keep the coefficients
		**  in a similar range to a,b,d,e,p and q.
		**  Texas will scale them back to the right values.
		*/

		adj_c*=(1.0f/1023.0f); 
		adj_f*=(1.0f/1023.0f);
		adj_r*=(1.0f/1023.0f); 

		/*
		**	find the largest (magnitude) of a,b,c,d,e,f 
		** 
		** On some slower processors floating point compare
		** etc is very slow. Since these values are all positive, we
		** can just as validly "assume" they are integer as the IEEE
		** fp format will work.
		** Note also that fabs can be replaced with a clearing of the
		** TOP bit.
		** This bit of code also assumes that there are B'all FP registers
		** and therefore most FP values are sitting out in memory (not
		** registers)
		*/
		#if SLOW_FCMP && !MULTI_FP_REG
			{
				long  LargestIntEquiv = SMALL_FLOAT_VALUE;
				long  temp;

				/*
				// Get "fabs" of a and compare it with that of a
				// etc etc etc
				*/
				temp = FLOAT_TO_LONG(adj_a) & FABS_MASK;
				if(temp > LargestIntEquiv)
				{
					LargestIntEquiv = temp;
				}

				temp = FLOAT_TO_LONG(adj_b) & FABS_MASK;
				if(temp > LargestIntEquiv)
				{
					LargestIntEquiv = temp;
				}

				temp = FLOAT_TO_LONG(adj_c) & FABS_MASK;
				if(temp > LargestIntEquiv)
				{
					LargestIntEquiv = temp;
				}

				temp = FLOAT_TO_LONG(adj_d) & FABS_MASK;
				if(temp > LargestIntEquiv)
				{
					LargestIntEquiv = temp;
				}


				temp = FLOAT_TO_LONG(adj_e) & FABS_MASK;
				if(temp > LargestIntEquiv)
				{
					LargestIntEquiv = temp;
				}

				temp = FLOAT_TO_LONG(adj_f) & FABS_MASK;
				if(temp > LargestIntEquiv)
				{
					LargestIntEquiv = temp;
				}

				largestT = LONG_TO_FLOAT(LargestIntEquiv);
			}
			/*
			// Use FP compares to get the largest
			*/

		#else

			largestT= SMALL_FLOAT_VALUE;

			/* 
			// make a temp copy of the result of the sfabs() because
			// this may be faster on compilers that call an sfabs
			// function
			*/
			FabsT1=sfabs(adj_a);
			if(FabsT1>largestT)
				largestT=FabsT1;

			FabsT1=sfabs(adj_b);
			if(FabsT1>largestT)
				largestT=FabsT1;

			FabsT2=sfabs(adj_c);
			if(FabsT2>largestT)
				largestT=FabsT2;

			FabsT1=sfabs(adj_d);
			if(FabsT1>largestT)
				largestT=FabsT1;

			FabsT2=sfabs(adj_e);
			if(FabsT2>largestT)
				largestT=FabsT2;

			FabsT1=sfabs(adj_f);
			if(FabsT1>largestT)
				largestT=FabsT1;

		#endif



		/*
		** find the largest of p,q,r 
		**
		** Again use a suitable method...
		*/
	#if SLOW_FCMP && !MULTI_FP_REG
		{
			long  LargestIntEquiv;
			long  temp;

			/*
			// Get the abs of p, and "convert" it to int.
			*/
			LargestIntEquiv = FLOAT_TO_LONG(adj_p) & FABS_MASK;


			temp = FLOAT_TO_LONG(adj_q) & FABS_MASK;
			if(temp > LargestIntEquiv)
			{
				LargestIntEquiv = temp;
			}

			temp = FLOAT_TO_LONG(adj_r) & FABS_MASK;
			if(temp > LargestIntEquiv)
			{
				LargestIntEquiv = temp;
			}

			largestB = LONG_TO_FLOAT(LargestIntEquiv);
		}
	#else
		largestB= sfabs(adj_p);

		FabsT1=sfabs(adj_q);
		if (FabsT1>largestB)
			  largestB=FabsT1;

		FabsT2=sfabs(adj_r);
		if (FabsT2>largestB)
			  largestB=FabsT2;
	#endif

		/*
		** calculate a fast floor(log2(largest/largest_bot))
		*/

		largestT=1.0f/largestT;

		topExponent.f=largestB*largestT;

		/* extract the offset exponent from the IEEE fp number */

		exp=(topExponent.l>>23) & 0xff; 	

		/* calculate the reciprocal (ignore post normalising) */

		exp=126-exp;

		/*
		** rescale the texturing coefficients to 16 bits.
		*/

		largestT=largestT * 32767.0f;

		adj_a=adj_a * largestT;
		adj_b=adj_b * largestT;
		adj_c=adj_c * largestT;
		adj_d=adj_d * largestT;
		adj_e=adj_e * largestT;
		adj_f=adj_f * largestT;

		/* calculate a fast pow(2.0,floor())*largestT */

		topExponent.l=(exp+127)<<23;

		largestT=largestT * topExponent.f;

		adj_p=adj_p * largestT;
		adj_q=adj_q * largestT;
		adj_r=adj_r * largestT;

		/*
		** Texas can't handle an exponent greater than 15,so we will
		** reduce the resolution of 'p', 'q', and 'r'.
		** THIS SHOULD HAPPEN **VERY** RARELY.The Level Of Detail
		** should have removed the texture by now.
		*/

		if(exp>15)
		{
			adj_p=(float)((sgl_int32)adj_p >>(exp-15));
			adj_q=(float)((sgl_int32)adj_q >>(exp-15));
			adj_r=(float)((sgl_int32)adj_r >>(exp-15));

			exp=15;
		}
		
		/*
		** Texas can't handle a negative exponent,so we will
		** reduce the resolution of a,b,c,d,e and f.
		** This condition only happens if the texture is VERY zoomed.
		** It may not be worth the expense of testing for this.
		*/
		else if(exp<0)
		{
			adj_a=(float)((sgl_int32)adj_a >>(0-exp));
			adj_b=(float)((sgl_int32)adj_b >>(0-exp));
			adj_c=(float)((sgl_int32)adj_c >>(0-exp));
			adj_d=(float)((sgl_int32)adj_d >>(0-exp));
			adj_e=(float)((sgl_int32)adj_e >>(0-exp));
			adj_f=(float)((sgl_int32)adj_f >>(0-exp));

			exp=0;
		}




		/*
		**  now pack up the data in TEXAS format
		*/
		/*first instruction word*/

		IW( pTSP, 0, (pMat->Shading.u.WriteBack.ModControl | ((sgl_uint32) exp << SHIFT_EXPONENT)));
		IW( pTSP, 1, (pMat->Shading.u.WriteBack.ModBaseCol));

		/*texture parameters*/ 

		IW( pTSP, 2, (((sgl_int32) adj_r) & 0xffffUL));

        IW( pTSP, 3, (((sgl_int32) adj_q << 16) | (((sgl_int32) adj_p) & 0xffffUL)));

        IW( pTSP, 4, ((gPDC.TexAddress << 16) | (((sgl_int32) adj_c) & 0xffffUL)));

        IW( pTSP, 5, (((sgl_int32) adj_b << 16) | (((sgl_int32) adj_a) & 0xffffUL)));

        IW( pTSP, 6, ((gPDC.TexAddress & 0xffff0000) | (((sgl_int32) adj_f) & 0xffffUL)));

        IW( pTSP, 7, (((sgl_int32) adj_e << 16) | (((sgl_int32) adj_d) & 0xffffUL)));

		switch(TypeFlag)
		{
		case(0):
			break;
		case(1):
			IW( pTSP, 8, (pMat->Shading.u.WriteBack.Smooth1));
			IW( pTSP, 9, (pMat->Shading.u.WriteBack.Smooth2));
			break;
		case(2):
			IW( pTSP, 8, (pMat->Shading.u.Highlight));
			break;
		case(4):
			IW( pTSP, 8, (pMat->Shading.u.WriteBack.Smooth1));
			IW( pTSP, 9, (pMat->Shading.u.WriteBack.Smooth2));
			IW( pTSP, 10, (pMat->v.LightVolCol));
			IW( pTSP, 11, (pMat->Shading.u.Highlight));
			break;
		default:
			ASSERT((TypeFlag!=TypeFlag));
			break;
		}

		pMat++;
		pTSP += TSPIncrement;	
	}
	
	return (nPolys);
}
#else		/* #if PACKTSPTEX		*/
	#if 0
		/* Simplified version of the MipMap case above, this is what the asm does */

		#ifndef DEBUG_ASSEMBLER
		#define DEBUG_ASSEMBLER 1	/* See the same code in dtexml.asm */
		#endif

		/* 1 has exp of 127 so 1<<23 is what we need */
		#ifndef MAGIC_FISTP_SINGLE_LONG
		#define MAGIC_FISTP_SINGLE_LONG (((127L+23L)<<23)+0x00400000L)
		#endif

		#if DEBUG_ASSEMBLER
		int PackTSPTexInC (PIMATERIAL pMat, int nPolys,
						   int TypeFlag, sgl_uint32 TSPIncrement, 
						   sgl_uint32 *pTSP, int MatSize)
		#else
		int PackTSPTex (PIMATERIAL pMat, int nPolys,
						int TypeFlag, sgl_uint32 TSPIncrement, 
						sgl_uint32 *pTSP, int MatSize)
		#endif
		{
			flong 	largestT, largestB;
			flong 	topExponent, intconv[2];
			flong	intconvAF, intconvPR;
			long	exp, temp;
			flong	u2, v2, w2;

			while (nPolys--)
			{
				/*
				**	rescale 'c','f', and 'r' to keep the coefficients
				**  in a similar range to a,b,d,e,p and q.
				**  Texas will scale them back to the right values.
				*/

				u2.f = pMat->Tex.u2*(1.0f/1023.0f); 
				v2.f = pMat->Tex.v2*(1.0f/1023.0f);
				w2.f = pMat->Tex.w2*(1.0f/1023.0f); 
				
				/*
				**	find the largest (magnitude) of a,b,c,d,e,f 
				** 
				** On some slower processors floating point compare
				** etc is very slow. Since these values are all positive, we
				** can just as validly "assume" they are integer as the IEEE
				** fp format will work.
				** Note also that fabs can be replaced with a clearing of the
				** TOP bit.
				** This bit of code also assumes that there are B'all FP registers
				** and therefore most FP values are sitting out in memory (not
				** registers)
				*/
				{
					/*
					// Get the abs of a, and "convert" it to int.
					*/
					largestT.l = FLOAT_TO_LONG(pMat->Tex.u0) & FABS_MASK;


					/*
					// Get "fabs" of b and compare it with that of a
					// etc etc etc
					*/
					temp = FLOAT_TO_LONG(pMat->Tex.u1) & FABS_MASK;
					if(temp > largestT.l)
					{
						largestT.l = temp;
					}

					temp = u2.l & FABS_MASK;
					if(temp > largestT.l)
					{
						largestT.l = temp;
					}

					temp = FLOAT_TO_LONG(pMat->Tex.v0) & FABS_MASK;
					if(temp > largestT.l)
					{
						largestT.l = temp;
					}


					temp = FLOAT_TO_LONG(pMat->Tex.v1) & FABS_MASK;
					if(temp > largestT.l)
					{
						largestT.l = temp;
					}

					temp = v2.l & FABS_MASK;
					if(temp > largestT.l)
					{
						largestT.l = temp;
					}
				}

				/* Let's do this while we do some integer stuff please! */
				largestT.f = 1.0f/largestT.f;

				/*
				** find the largest of p,q,r 
				**
				** Again use a suitable method...
				*/
				{
					/*
					// Get the abs of p, and "convert" it to int.
					*/
					largestB.l = FLOAT_TO_LONG(pMat->Tex.w0) & FABS_MASK;


					temp = FLOAT_TO_LONG(pMat->Tex.w1) & FABS_MASK;
					if(temp > largestB.l)
					{
						largestB.l = temp;
					}

					temp = w2.l & FABS_MASK;
					if(temp > largestB.l)
					{
						largestB.l = temp;
					}
				}

				/*
				** calculate a fast floor(log2(largest/largest_bot))
				*/

				topExponent.f=largestB.f*largestT.f;

				/* extract the offset exponent from the IEEE fp number */

				exp=(topExponent.l>>23) & 0xff; 	

				/* calculate the reciprocal (ignore post normalising) */

				exp=126-exp;

				/*
				** rescale the texturing coefficients to 16 bits.
				*/

				largestT.f=( largestT.f * 32767.0f );

				/* calculate a fast pow(2.0,floor())*largestT.f */

				topExponent.l=(exp+127)<<23;

				/* Setup for common case with no scaling requirement */
				intconvAF.l = MAGIC_FISTP_SINGLE_LONG;
				intconvPR.l = MAGIC_FISTP_SINGLE_LONG;

				if(exp>15)
				{
					/* adj_(p to r) =(float)((sgl_int32)adj_(p to r) >>(exp-15)); */
					intconvPR.l += ((exp-15)<<23);

					/* Clip max exponent specified to Texas */
					exp=15;
				}
				else
				if(exp < 0)
				{
					/* adj_(a to f) =(float)((sgl_int32)adj_(a to f) >>(0-exp)); */
					intconvAF.l += ((0-exp)<<23);

					/* Clip max exponent specified to Texas */
					exp=0;
				}


				intconv[0].f =(u2.f * largestT.f) + intconvAF.f;

		        pTSP[4] = (gPDC.TexAddress << 16) | (intconv[0].l & 0xffffUL);

				intconv[0].f =(pMat->Tex.u0 * largestT.f) + intconvAF.f;
				intconv[1].f =(pMat->Tex.u1 * largestT.f) + intconvAF.f;

		        pTSP[5] = (intconv[1].l << 16) | (intconv[0].l & 0xffffUL);
				
				intconv[0].f =(v2.f * largestT.f) + intconvAF.f;
				
		        pTSP[6] = (gPDC.TexAddress & 0xffff0000) | (intconv[0].l & 0xffffUL);
				
				intconv[0].f =(pMat->Tex.v0 * largestT.f) + intconvAF.f;
				intconv[1].f =(pMat->Tex.v1 * largestT.f) + intconvAF.f;

		        pTSP[7] = (intconv[1].l << 16) | (intconv[0].l & 0xffffUL);

				largestT.f=largestT.f * topExponent.f;

				intconv[0].f =(w2.f * largestT.f) + intconvPR.f;

				pTSP[0] = 
					pMat->Shading.u.WriteBack.ModControl | ((sgl_uint32) exp << SHIFT_EXPONENT);
				pTSP[1] = pMat->Shading.u.WriteBack.ModBaseCol;

				pTSP[2] = (intconv[0].l & 0xffffUL);

				intconv[0].f =(pMat->Tex.w0 * largestT.f) + intconvPR.f;
				intconv[1].f =(pMat->Tex.w1 * largestT.f) + intconvPR.f;

		        pTSP[3] = (intconv[1].l << 16) | (intconv[0].l & 0xffffUL);

				switch(TypeFlag)
				{
				case(0):
					break;
				case(1):
					pTSP[8] = pMat->Shading.u.WriteBack.Smooth1;
					pTSP[9] = pMat->Shading.u.WriteBack.Smooth2;
					break;
				case(2):
					pTSP[8] = pMat->Shading.u.Highlight;
					break;
				case(4):
					pTSP[8] = pMat->Shading.u.WriteBack.Smooth1;
					pTSP[9] = pMat->Shading.u.WriteBack.Smooth2;
					pTSP[10] = pMat->v.LightVolCol;
					pTSP[11] = pMat->Shading.u.Highlight;
					break;
				default:
					ASSERT((TypeFlag!=TypeFlag));
					break;
				}
						  
				pMat++;
				pTSP += TSPIncrement;	
			}

			return (nPolys);
		}

		#if DEBUG_ASSEMBLER
		int PackTSPTex (PIMATERIAL pMat, int nPolys,
						int nCWOffset, sgl_uint32 TSPIncrement, 
						sgl_uint32 *pTSP, int nMatInc)
		{
			sgl_uint32 FakeTSP[20];
			
			while ( nPolys != 0 )
			{
				PackTSPTexInC(pMat, 1, TypeFlag, TSPIncrement, FakeTSP, MatSize);
				PackTSPTexInAsm(pMat, 1, TypeFlag, TSPIncrement, pTSP, MatSize);
				
				if ( FakeTSP[0] != pTSP[0] )
				{
					__asm { int 3; }
				}
				else
				if ( FakeTSP[1] != pTSP[1] )
				{
					__asm { int 3; }
				}
				else
				if ( FakeTSP[2] != pTSP[2] )
				{
					__asm { int 3; }
				}
				else
				if ( FakeTSP[3] != pTSP[3] )
				{
					__asm { int 3; }
				}
				else
				if ( FakeTSP[4] != pTSP[4] )
				{
					__asm { int 3; }
				}
				else
				if ( FakeTSP[5] != pTSP[5] )
				{
					__asm { int 3; }
				}
				else
				if ( FakeTSP[6] != pTSP[6] )
				{
					__asm { int 3; }
				}
				else
				if ( FakeTSP[7] != pTSP[7] )
				{
					__asm { int 3; }
				}
				else
				{
					/* Done one correctly */
					(sgl_uint32) pMat += MatSize;
					pTSP += TSPIncrement;
					
					nPolys--;
				}
			}
		}
		#endif		/* #if DEBUG_ASSEMBLER	*/
	#else		/* #if 0				*/
		extern int PackTSPTex (PIMATERIAL pMat,
							  int nPolys, int TypeFlag, 
							  sgl_uint32 TSPIncrement, sgl_uint32 *pTSP, 
							  int MatSize );
	#endif		/* #if 0				*/
#endif		/* #if PACKTSPTEX		*/


int PackTSPTransTex (PITRI pTri, PIMATERIAL pMat, int nPolys, int nCWOffset, 
				sgl_uint32 nTSPIncrement, sgl_uint32 *pTSP)
{
	/* this packs the dummy one-pixel translucent texture */
	sgl_uint32 TexAddr;

	SGL_TIME_START(PACK_TEXAS_TRI_TIME)
	
	TexAddr = TranslucentControlWord;

	/*
	// Go through the planes. Move on to the next shading results and
	// and next plane pointer (NOTE Planes is array of pointers)
	*/
	while (nPolys--)
	{
		/*
		** Put the index in the projected plane structure.
		*/
		IW( pTSP, 0, 1);						/* r */
		IW( pTSP, 1, 0);						/* q<<16 | p */
		IW( pTSP, 2, (TexAddr << 16));			/* addr<16 | c */
		IW( pTSP, 3, 0);						/* b<<16 | a */
		IW( pTSP, 4, (TexAddr & 0xFFFF0000));   /* addr&FFFF0000 | f */
		IW( pTSP, 5, 0);						/* e<<16 | d */

		pTSP += nTSPIncrement;
		
	}/*end for*/

	SGL_TIME_STOP(PACK_TEXAS_TRI_TIME)

	return (nPolys);
}

/**********************************************************************
**
**
**
**
**********************************************************************/

#define 		INV_VFOG_TEXSIZE  (1.0f/32.0f)
#ifndef INV
	#define 	INV(x,y)	(pfAdj[((x)*3)+(y)])
#endif

int PackExtra(PITRI pTri, PIMATERIAL pMat, sgl_uint32 nPolys, sgl_uint32 TSPInc, sgl_uint32 *pTSP)
{
	sgl_uint32 TexAddr;
	sgl_uint32 FlatTexAddr;
	sgl_uint32 TSP0;
	sgl_uint32 TSP1;

	float W0, W1, W2, U0, U1, U2, V0, V1, V2;
	float tmp0, tmp1, tmp2;
	float off0, off1;

	float 	largestT,largestB;
	float	adj_a,adj_b,adj_c,adj_p,adj_q,adj_r;
	flong 	topExponent;
	long	exp;
	SGLVERTEX 	*pV0, *pV1, *pV2;
#if !SLOW_FCMP || MULTI_FP_REG
	float 	FabsT1,FabsT2;
#endif
	
	TexAddr = VertexFogControlWord;
	FlatTexAddr = TranslucentControlWord;
	TSP0 = gPDC.VFogTSPControlWord[0];
	TSP1 = gPDC.VFogTSPControlWord[1];

	while (nPolys--)
	{
		float *pfAdj = pTri->fAdjoint[0];

/* **************************************************************************** */
		if(pMat->nSmoothSpecularIndex)
		{
			sgl_uint32 nSmoothSpecularIndex = pMat->nSmoothSpecularIndex;


			sgl_uint32 uSSpecColour = pMat->nSmoothSpecularColour;

			sgl_uint32 TSP0S,TSP1S;
			TSP0S = ((uSSpecColour >> 16) & 0x000000FF);
			TSP0S|= (MASK_TEXTURE | MASK_TRANS | MASK_DISABLE_FOG);
			TSP1S = (uSSpecColour << 16);

			/* if the highlight is constant over a poly shortcut it and
			** use the translucent texture
			*/
			if(((nSmoothSpecularIndex & 0x00fUL) == ((nSmoothSpecularIndex & 0x0f0UL)>>4)) &&
			   (((nSmoothSpecularIndex & 0xf00UL)>>8) == (nSmoothSpecularIndex & 0x00fUL)))
			{
				sgl_uint32 Alpha;

				Alpha = (nSmoothSpecularIndex & 0x00fUL);
				Alpha = ~Alpha &0xfUL;

				IW( pTSP, 0, (TSP0S | Alpha << 13));
				IW( pTSP, 1, TSP1S);
				IW( pTSP, 2, 1);						/* r */
				IW( pTSP, 3, 0);						/* q<<16 | p */
				IW( pTSP, 4, (FlatTexAddr << 16));			/* addr<16 | c */
				IW( pTSP, 5, 0);						/* b<<16 | a */
				IW( pTSP, 6, (FlatTexAddr & 0xFFFF0000));   /* addr&FFFF0000 | f */
				IW( pTSP, 7, 0);						/* e<<16 | d */

			}
			else 
			{

				/* ************************************************************** */
				
				W0 = pTri->fZ[0]*100.0f;
				W1 = pTri->fZ[1]*100.0f;
				W2 = pTri->fZ[2]*100.0f;
				
#define USE_VFOG_TABLE 1
#if USE_VFOG_TABLE
				U0 = fVertFogUVs[nSmoothSpecularIndex][0];
				U1 = fVertFogUVs[nSmoothSpecularIndex][1];
				U2 = fVertFogUVs[nSmoothSpecularIndex][2];
#else
				U0 = ((nSmoothSpecularIndex & 0xF00) >> 8) * 0.0625f + 0.03125f;
				U1 = ((nSmoothSpecularIndex & 0x0F0) >> 4) * 0.0625f + 0.03125f;
				U2 = (nSmoothSpecularIndex & 0x00F) * 0.0625f + 0.03125f;
#endif
				U0 *= W0;
				U1 *= W1;
				U2 *= W2;
				
				off0 = U0 - U2;
				off1 = U1 - U2;
				
				adj_a = INV(0,0) * off0 + INV(0,1) * off1;
				adj_b = INV(1,0) * off0 + INV(1,1) * off1;
				adj_c = INV(2,0) * U0   + INV(2,1) * U1 + INV(2,2) * U2;
				
				
				off0 = W0 - W2;
				off1 = W1 - W2;
				
				adj_p = (INV(0,0) * off0 + INV(0,1) * off1) * INV_VFOG_TEXSIZE;
				adj_q = (INV(1,0) * off0 + INV(1,1) * off1) * INV_VFOG_TEXSIZE;
				adj_r = (INV(2,0) * W0 + INV(2,1) * W1 + INV(2,2) * W2) * INV_VFOG_TEXSIZE;
				
				/*
				**	rescale 'c','f', and 'r' to keep the coefficients
				**  in a similar range to a,b,d,e,p and q.
				**  Texas will scale them back to the right values.
				*/

				adj_c*=(1.0f/1023.0f);
				adj_r*=(1.0f/1023.0f); 
			
				/*
				**	find the largest (magnitude) of a,b,c,d,e,f 
				** 
				** On some slower processors floating point compare
				** etc is very slow. Since these values are all positive, we
				** can just as validly "assume" they are integer as the IEEE
				** fp format will work.
				** Note also that fabs can be replaced with a clearing of the
				** TOP bit.
				** This bit of code also assumes that there are B'all FP registers
				** and therefore most FP values are sitting out in memory (not
				** registers)
				*/
#if SLOW_FCMP && !MULTI_FP_REG
			
				{
					long  LargestIntEquiv = SMALL_FLOAT_VALUE;
					long  temp;
					
					/*
					// Get "fabs" of a and compare it with that of a
					// etc etc etc
					*/
					temp = FLOAT_TO_LONG(adj_a) & FABS_MASK;
					if(temp > LargestIntEquiv)
					{
						LargestIntEquiv = temp;
					}
					
					temp = FLOAT_TO_LONG(adj_b) & FABS_MASK;
					if(temp > LargestIntEquiv)
					{
						LargestIntEquiv = temp;
					}
					
					temp = FLOAT_TO_LONG(adj_c) & FABS_MASK;
					if(temp > LargestIntEquiv)
					{
						LargestIntEquiv = temp;
					}
					
					largestT = LONG_TO_FLOAT(LargestIntEquiv);
				}
				/*
				// Use FP compares to get the largest
				*/

#else

				largestT= SMALL_FLOAT_VALUE;
				
				/* 
				// make a temp copy of the result of the sfabs() because
				// this may be faster on compilers that call an sfabs
				// function
				*/
				FabsT1=sfabs(adj_a);
				if(FabsT1>largestT)
					largestT=FabsT1;
				
				FabsT1=sfabs(adj_b);
				if(FabsT1>largestT)
					largestT=FabsT1;
				
				FabsT2=sfabs(adj_c);
				if(FabsT2>largestT)
					largestT=FabsT2;
				
#endif



				/*
				** find the largest of p,q,r 
				**
				** Again use a suitable method...
				*/
#if SLOW_FCMP && !MULTI_FP_REG
				{
					long  LargestIntEquiv;
					long  temp;
				
					/*
					// Get the abs of p, and "convert" it to int.
					*/
					LargestIntEquiv = FLOAT_TO_LONG(adj_p) & FABS_MASK;


					temp = FLOAT_TO_LONG(adj_q) & FABS_MASK;
					if(temp > LargestIntEquiv)
					{
						LargestIntEquiv = temp;
					}

					temp = FLOAT_TO_LONG(adj_r) & FABS_MASK;
					if(temp > LargestIntEquiv)
					{
						LargestIntEquiv = temp;
					}
					
					largestB = LONG_TO_FLOAT(LargestIntEquiv);
				}
#else
				largestB= sfabs(adj_p);
			
				FabsT1=sfabs(adj_q);
				if (FabsT1>largestB)
					largestB=FabsT1;
				
				FabsT2=sfabs(adj_r);
				if (FabsT2>largestB)
					largestB=FabsT2;
#endif
				
				/*
				** calculate a fast floor(log2(largest/largest_bot))
				*/
				
				largestT=1.0f/largestT;
				
				topExponent.f=largestB*largestT;
				
				/* extract the offset exponent from the IEEE fp number */
			
				exp=(topExponent.l>>23) & 0xff; 	
				
				/* calculate the reciprocal (ignore post normalising) */
				
				exp=126-exp;
			
				/*
				** rescale the texturing coefficients to 16 bits.
				*/
			
				largestT=largestT * 32767.0f;
				
				adj_a=adj_a * largestT;
				adj_b=adj_b * largestT;
				adj_c=adj_c * largestT;
				
				/* calculate a fast pow(2.0,floor())*largestT */
				
				topExponent.l=(exp+127)<<23;
				
				largestT=largestT * topExponent.f;
				
				adj_p=adj_p * largestT;
				adj_q=adj_q * largestT;
				adj_r=adj_r * largestT;
				
				/*
				** Texas can't handle an exponent greater than 15,so we will
				** reduce the resolution of 'p', 'q', and 'r'.
				** THIS SHOULD HAPPEN **VERY** RARELY.The Level Of Detail
				** should have removed the texture by now.
				*/
			
				if(exp>15)
				{
					adj_p=(float)((sgl_int32)adj_p >>(exp-15));
					adj_q=(float)((sgl_int32)adj_q >>(exp-15));
					adj_r=(float)((sgl_int32)adj_r >>(exp-15));
					
					exp=15;
				}
				
				/*
				** Texas can't handle a negative exponent,so we will
				** reduce the resolution of a,b,c,d,e and f.
				** This condition only happens if the texture is VERY zoomed.
				** It may not be worth the expense of testing for this.
				*/
				else if(exp<0)
				{
					adj_a=(float)((sgl_int32)adj_a >>(0-exp));
					adj_b=(float)((sgl_int32)adj_b >>(0-exp));
					adj_c=(float)((sgl_int32)adj_c >>(0-exp));
					
					exp=0;
				}



				/* ****************************************************************** */
		
				IW( pTSP, 0, TSP0S | ((sgl_uint32) exp << SHIFT_EXPONENT));
				IW( pTSP, 1, TSP1S);
				IW( pTSP, 2, (((sgl_int32) adj_r) & 0xffffUL));
				IW( pTSP, 3, (((sgl_int32) adj_q << 16) | (((sgl_int32) adj_p) & 0xffffUL)));
				IW( pTSP, 4, ((TexAddr << 16) | (((sgl_int32) adj_c) & 0xffffUL)));
				IW( pTSP, 5, (((sgl_int32) adj_b << 16) | (((sgl_int32) adj_a) & 0xffffUL)));
				IW( pTSP, 6, (TexAddr & 0xffff0000)) ;
				IW( pTSP, 7, (0x0UL));
				

			}
			pTSP += TSPInc;
		}
		
/* **************************************************************************** */


		if (pMat->nFogIndex)
		{
			sgl_uint32 nFogIndex = pMat->nFogIndex;

			/* if the fog is constant over a poly shortcut it and
			** use the translucent texture
			*/
			if(((nFogIndex & 0x00fUL) == ((nFogIndex & 0x0f0UL)>>4)) &&
			   (((nFogIndex & 0xf00UL)>>8) == (nFogIndex & 0x00fUL)))
			{
				sgl_uint32 Alpha;

				Alpha = (nFogIndex & 0x00fUL);
				Alpha = ~Alpha &0xfUL;

				IW( pTSP, 0, (TSP0 | Alpha << 13));
				IW( pTSP, 1, TSP1);
				IW( pTSP, 2, 1);						/* r */
				IW( pTSP, 3, 0);						/* q<<16 | p */
				IW( pTSP, 4, (FlatTexAddr << 16));			/* addr<16 | c */
				IW( pTSP, 5, 0);						/* b<<16 | a */
				IW( pTSP, 6, (FlatTexAddr & 0xFFFF0000));   /* addr&FFFF0000 | f */
				IW( pTSP, 7, 0);						/* e<<16 | d */

			}
			else 
			{

				/* ************************************************************** */
				
				W0 = pTri->fZ[0]*100.0f;
				W1 = pTri->fZ[1]*100.0f;
				W2 = pTri->fZ[2]*100.0f;
				
#define USE_VFOG_TABLE 1
#if USE_VFOG_TABLE
				U0 = fVertFogUVs[nFogIndex][0];
				U1 = fVertFogUVs[nFogIndex][1];
				U2 = fVertFogUVs[nFogIndex][2];
#else
				U0 = ((nFogIndex & 0xF00) >> 8) * 0.0625f + 0.03125f;
				U1 = ((nFogIndex & 0x0F0) >> 4) * 0.0625f + 0.03125f;
				U2 = (nFogIndex & 0x00F) * 0.0625f + 0.03125f;
#endif
				U0 *= W0;
				U1 *= W1;
				U2 *= W2;
				
				off0 = U0 - U2;
				off1 = U1 - U2;
				
				adj_a = INV(0,0) * off0 + INV(0,1) * off1;
				adj_b = INV(1,0) * off0 + INV(1,1) * off1;
				adj_c = INV(2,0) * U0   + INV(2,1) * U1 + INV(2,2) * U2;
				
				
				off0 = W0 - W2;
				off1 = W1 - W2;
				
				adj_p = (INV(0,0) * off0 + INV(0,1) * off1) * INV_VFOG_TEXSIZE;
				adj_q = (INV(1,0) * off0 + INV(1,1) * off1) * INV_VFOG_TEXSIZE;
				adj_r = (INV(2,0) * W0 + INV(2,1) * W1 + INV(2,2) * W2) * INV_VFOG_TEXSIZE;
				
				/*
				**	rescale 'c','f', and 'r' to keep the coefficients
				**  in a similar range to a,b,d,e,p and q.
				**  Texas will scale them back to the right values.
				*/

				adj_c*=(1.0f/1023.0f); 
				adj_r*=(1.0f/1023.0f); 
			
				/*
				**	find the largest (magnitude) of a,b,c,d,e,f 
				** 
				** On some slower processors floating point compare
				** etc is very slow. Since these values are all positive, we
				** can just as validly "assume" they are integer as the IEEE
				** fp format will work.
				** Note also that fabs can be replaced with a clearing of the
				** TOP bit.
				** This bit of code also assumes that there are B'all FP registers
				** and therefore most FP values are sitting out in memory (not
				** registers)
				*/
#if SLOW_FCMP && !MULTI_FP_REG
			
				{
					long  LargestIntEquiv = SMALL_FLOAT_VALUE;
					long  temp;
					
					/*
					// Get "fabs" of a and compare it with that of a
					// etc etc etc
					*/
					temp = FLOAT_TO_LONG(adj_a) & FABS_MASK;
					if(temp > LargestIntEquiv)
					{
						LargestIntEquiv = temp;
					}
					
					temp = FLOAT_TO_LONG(adj_b) & FABS_MASK;
					if(temp > LargestIntEquiv)
					{
						LargestIntEquiv = temp;
					}
					
					temp = FLOAT_TO_LONG(adj_c) & FABS_MASK;
					if(temp > LargestIntEquiv)
					{
						LargestIntEquiv = temp;
					}
					
					largestT = LONG_TO_FLOAT(LargestIntEquiv);
				}
				/*
				// Use FP compares to get the largest
				*/

#else

				largestT= SMALL_FLOAT_VALUE;
				
				/* 
				// make a temp copy of the result of the sfabs() because
				// this may be faster on compilers that call an sfabs
				// function
				*/
				FabsT1=sfabs(adj_a);
				if(FabsT1>largestT)
					largestT=FabsT1;
				
				FabsT1=sfabs(adj_b);
				if(FabsT1>largestT)
					largestT=FabsT1;
				
				FabsT2=sfabs(adj_c);
				if(FabsT2>largestT)
					largestT=FabsT2;
				
#endif



				/*
				** find the largest of p,q,r 
				**
				** Again use a suitable method...
				*/
#if SLOW_FCMP && !MULTI_FP_REG
				{
					long  LargestIntEquiv;
					long  temp;
				
					/*
					// Get the abs of p, and "convert" it to int.
					*/
					LargestIntEquiv = FLOAT_TO_LONG(adj_p) & FABS_MASK;


					temp = FLOAT_TO_LONG(adj_q) & FABS_MASK;
					if(temp > LargestIntEquiv)
					{
						LargestIntEquiv = temp;
					}

					temp = FLOAT_TO_LONG(adj_r) & FABS_MASK;
					if(temp > LargestIntEquiv)
					{
						LargestIntEquiv = temp;
					}
					
					largestB = LONG_TO_FLOAT(LargestIntEquiv);
				}
#else
				largestB= sfabs(adj_p);
			
				FabsT1=sfabs(adj_q);
				if (FabsT1>largestB)
					largestB=FabsT1;
				
				FabsT2=sfabs(adj_r);
				if (FabsT2>largestB)
					largestB=FabsT2;
#endif
				
				/*
				** calculate a fast floor(log2(largest/largest_bot))
				*/
				
				largestT=1.0f/largestT;
				
				topExponent.f=largestB*largestT;
				
				/* extract the offset exponent from the IEEE fp number */
			
				exp=(topExponent.l>>23) & 0xff; 	
				
				/* calculate the reciprocal (ignore post normalising) */
				
				exp=126-exp;
			
				/*
				** rescale the texturing coefficients to 16 bits.
				*/
			
				largestT=largestT * 32767.0f;
				
				adj_a=adj_a * largestT;
				adj_b=adj_b * largestT;
				adj_c=adj_c * largestT;
				
				/* calculate a fast pow(2.0,floor())*largestT */
				
				topExponent.l=(exp+127)<<23;
				
				largestT=largestT * topExponent.f;
				
				adj_p=adj_p * largestT;
				adj_q=adj_q * largestT;
				adj_r=adj_r * largestT;
				
				/*
				** Texas can't handle an exponent greater than 15,so we will
				** reduce the resolution of 'p', 'q', and 'r'.
				** THIS SHOULD HAPPEN **VERY** RARELY.The Level Of Detail
				** should have removed the texture by now.
				*/
			
				if(exp>15)
				{
					adj_p=(float)((sgl_int32)adj_p >>(exp-15));
					adj_q=(float)((sgl_int32)adj_q >>(exp-15));
					adj_r=(float)((sgl_int32)adj_r >>(exp-15));
					
					exp=15;
				}
				
				/*
				** Texas can't handle a negative exponent,so we will
				** reduce the resolution of a,b,c,d,e and f.
				** This condition only happens if the texture is VERY zoomed.
				** It may not be worth the expense of testing for this.
				*/
				else if(exp<0)
				{
					adj_a=(float)((sgl_int32)adj_a >>(0-exp));
					adj_b=(float)((sgl_int32)adj_b >>(0-exp));
					adj_c=(float)((sgl_int32)adj_c >>(0-exp));
					
					exp=0;
				}



				/* ****************************************************************** */
		
				IW( pTSP, 0, TSP0 | ((sgl_uint32) exp << SHIFT_EXPONENT));
				IW( pTSP, 1, TSP1);
				IW( pTSP, 2, (((sgl_int32) adj_r) & 0xffffUL));
				IW( pTSP, 3, (((sgl_int32) adj_q << 16) | (((sgl_int32) adj_p) & 0xffffUL)));
				IW( pTSP, 4, ((TexAddr << 16) | (((sgl_int32) adj_c) & 0xffffUL)));
				IW( pTSP, 5, (((sgl_int32) adj_b << 16) | (((sgl_int32) adj_a) & 0xffffUL)));
				IW( pTSP, 6, (TexAddr & 0xffff0000)) ;
				IW( pTSP, 7, (0x0UL));
				

			}
			pTSP += TSPInc;
		}

		/* overwrite the indices to stop them being accidently picked up
		** if two lots of tris end up in one buffers worth
		*/
		pMat->nFogIndex=0;
		pMat->nSmoothSpecularIndex=0;
			
		pTri++;
		pMat++;
		
	}/*end for*/

	return (nPolys);
}

/* end of $RCSfile: dtex.c,v $ */


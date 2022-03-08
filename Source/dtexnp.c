/******************************************************************************
 * Name         : dtexnp.c
 * Title        : Texture routines for PowerSGL Direct
 * Author       : Edward Forde
 * Created      : 13/05/1997
 *
 * Copyright	: 1995-2022 Imagination Technologies (c)
 * License		: MIT
 *
 *
 * Description  : These are the non perspective correct texture routines.
 *				  Based on the functions in dtex.c.
 *
 * Platform     : ANSI
 *
 * Modifications:
 * $Log: dtexnp.c,v $
 * Revision 1.4  1997/09/09  18:11:15  erf
 * Insured that max UVs not zero.
 *
 * Revision 1.3  1997/06/20  14:43:46  erf
 * The non-perspective routines for D3D and SGLDirect are the same
 * so I removed one.
 *
 * Revision 1.2  1997/06/05  08:41:30  erf
 * Fixed a few compiler warnings.
 *
 * Revision 1.1  1997/05/13  17:55:21  erf
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

SGL_EXTERN_TIME_REF /* if we are timing code */

typedef union
{
	sgl_uint32	l;
	float	f;
} flong;

/**************************************************************************
 * Function Name  : ProcessFlatTexNonPer
 * Inputs         : PITRI pTri
 * Outputs        : None.
 * Returns        : None.
 * Global Used    : 
 * Description    : Calculation of the U, V and W values for non perspective
 *					correction case.
 *					Used by both SGLDirect and D3D. This is possible because
 *					the difference between SGLDirect and D3D is the 1/w value
 *					which is 1 in both cases so the same function can be used.
 *
 **************************************************************************/

void ProcessFlatTexNonPer( PITRI pTri )
{
	PIMATERIAL	pMat = gpMatCurrent;
	float		U0, U1, U2, V0, V1, V2;
	float		tmp0, tmp1, tmp2;
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
	
	if(fHalfTexel)
	{
		U0 = pV0->fUOverW - fHalfTexel;
		U1 = pV1->fUOverW - fHalfTexel;
		U2 = pV2->fUOverW - fHalfTexel;
		/*
		** Do V.   (V don't)
		*/
		V0 = pV0->fVOverW - fHalfTexel;
		V1 = pV1->fVOverW - fHalfTexel;
		V2 = pV2->fVOverW - fHalfTexel;
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

	/*
	// Do W. With non perspective correction w0, w1 are zero while w2 is the
	// size.
	*/
	pMat->Tex.w0 = 0.0f;
	pMat->Tex.w1 = 0.0f;
	pMat->Tex.w2 = (INV(2,0) + INV(2,1) + INV(2,2)) * gPDC.invTexSize;
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
		// Then leave Y .
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
 * Function Name  : ProcessD3DFlatTexWrapUNonPer
 * Inputs         : PITRI pTri
 * Outputs        : None.
 * Returns        : None.
 * Global Used    : 
 * Description    : Calculation of the U, V and W values for non perspective
 *					correction case.
 *
 **************************************************************************/

void ProcessD3DFlatTexWrapUNonPer( PITRI pTri )
{
	PIMATERIAL	pMat = gpMatCurrent;
	float		U0, U1, U2, V0, V1, V2;
	float		off0, off1;
	float		tmp0, tmp1, tmp2;
	float		fHalfTexel;
	const float	*pfAdj = pTri->fAdjoint[0];

	fHalfTexel = gPDC.fHalfTexel;

	/* No scaling by w required since it is 1.
	 */
	if(fHalfTexel)
	{
		U0 = (gPDC.pV0->fUOverW - fHalfTexel);
		U1 = (gPDC.pV1->fUOverW - fHalfTexel);
		U2 = (gPDC.pV2->fUOverW - fHalfTexel);
		V0 = (gPDC.pV0->fVOverW - fHalfTexel);
		V1 = (gPDC.pV1->fVOverW - fHalfTexel);
		V2 = (gPDC.pV2->fVOverW - fHalfTexel);
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
	// Do W. With non perspective correction w0, w1 are zero while w2 is the
	// size.
	*/
	pMat->Tex.w0 = 0.0f;
	pMat->Tex.w1 = 0.0f;
	pMat->Tex.w2 = (INV(2,0) + INV(2,1) + INV(2,2)) * gPDC.invTexSize;
}


/**************************************************************************
 * Function Name  : ProcessD3DFlatTexWrapVNonPer
 * Inputs         : PITRI pTri
 * Outputs        : None.
 * Returns        : None.
 * Global Used    : 
 * Description    : Calculation of the U, V and W values for non perspective
 *					correction case.
 *
 **************************************************************************/

void ProcessD3DFlatTexWrapVNonPer( PITRI pTri )
{
	PIMATERIAL	pMat = gpMatCurrent;
	float		U0, U1, U2, V0, V1, V2;
	const float	*pfAdj = pTri->fAdjoint[0];
	float		off0, off1;
	float		tmp0, tmp1, tmp2;
	float		fHalfTexel;

	fHalfTexel = gPDC.fHalfTexel;

	/* No scaling by w required since it is 1.
	 */
	if(fHalfTexel)
	{
		U0 = (gPDC.pV0->fUOverW - fHalfTexel);
		U1 = (gPDC.pV1->fUOverW - fHalfTexel);
		U2 = (gPDC.pV2->fUOverW - fHalfTexel);
		V0 = (gPDC.pV0->fVOverW - fHalfTexel);
		V1 = (gPDC.pV1->fVOverW - fHalfTexel);
		V2 = (gPDC.pV2->fVOverW - fHalfTexel);
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
	
	/*
	// Do V. Note: it has to be "wrapped" and then scaled by W
	*/
	V1 += WrapOffset (V1-V0);
	V2 += WrapOffset (V2-V0);

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
	// Do W. With non perspective correction w0, w1 are zero while w2 is the
	// size.
	*/
	pMat->Tex.w0 = 0.0f;
	pMat->Tex.w1 = 0.0f;
	pMat->Tex.w2 = (INV(2,0) + INV(2,1) + INV(2,2)) * gPDC.invTexSize;
}


/**************************************************************************
 * Function Name  : ProcessD3DFlatTexWrapUVNonPer
 * Inputs         : PITRI pTri
 * Outputs        : None.
 * Returns        : None.
 * Global Used    : 
 * Description    : Calculation of the U, V and W values for non perspective
 *					correction case.
 *
 **************************************************************************/

void ProcessD3DFlatTexWrapUVNonPer( PITRI pTri )
{
	PIMATERIAL pMat = gpMatCurrent;
	float U0, U1, U2, V0, V1, V2;
	const float	*pfAdj = pTri->fAdjoint[0];
	float off0, off1;
	float tmp0, tmp1, tmp2;
	float fHalfTexel;

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

	off0 = V0 - V2;
	off1 = V1 - V2;
		
	tmp0 = INV(0,0) * off0 + INV(0,1) * off1;
	tmp1 = INV(1,0) * off0 + INV(1,1) * off1;
	tmp2 = INV(2,0) * V0   + INV(2,1) * V1 + INV(2,2)*V2;

	pMat->Tex.v0 = tmp0;
	pMat->Tex.v1 = tmp1;
	pMat->Tex.v2 = tmp2;

	/*
	// Do W. With non perspective correction w0, w1 are zero while w2 is the
	// size.
	*/
	pMat->Tex.w0 = 0.0f;
	pMat->Tex.w1 = 0.0f;
	pMat->Tex.w2 = (INV(2,0) + INV(2,1) + INV(2,2)) * gPDC.invTexSize;
}


/**************************************************************************
 * Function Name  : PackTSPMipMapTexNonPer
 * Inputs         : PIMATERIAL pMat
 *				 	int nPolys
 *					int TypeFlag
 *					sgl_uint32 TSPIncrement
 *					sgl_uint32 *pTSP
 *					int MatSize
 * Outputs        : 
 * Returns        : Number of polys processed.
 * Global Used    : 
 * Description    : This is the non perspective texture mip mapping function.
 *
 **************************************************************************/

#if !PACKTSPMIPMAPTEXNONPER

#define PackTSPMipMapTexNonPer _PackTSPMipMapTexNonPer

extern int PackTSPMipMapTexNonPer (PIMATERIAL pMat, 
					  int nPolys, int TypeFlag, 
					  sgl_uint32 TSPIncrement, sgl_uint32 *pTSP, 
					  int MatSize );
#else
int PackTSPMipMapTexNonPer (PIMATERIAL pMat, 
					  int nPolys, int TypeFlag, 
					  sgl_uint32 TSPIncrement, sgl_uint32 *pTSP, 
					  int MatSize )
{
	float 	largestT;
	float	compression,approx1;
	float 	dudx,dvdx,dudy,dvdy;
	float	adj_a,adj_b,adj_c,adj_d,adj_e,adj_f,adj_r;
	flong 	topExponent, MipMap;
	long	exp;
	long	MipMant,MipExp;

	while (nPolys--)
	{
		adj_a = pMat->Tex.u0;
		adj_b = pMat->Tex.u1;
		adj_c = pMat->Tex.u2;

		adj_d = pMat->Tex.v0;
		adj_e = pMat->Tex.v1;
		adj_f = pMat->Tex.v2;


		adj_r = pMat->Tex.w2;

		/*
		**	calculate the MIP-map coefficient.
		*/
		dudx= sfabs(adj_a);
		dvdx= sfabs(adj_d);

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
		dudy= sfabs(adj_b);
		dvdy= sfabs(adj_e);

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

			{
				float 	FabsT1,FabsT2;

				largestT = SMALL_FLOAT_VALUE;

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
			}

		#endif

		/*
		** calculate a fast floor(log2(largest/largest_bot))
		*/

		largestT=1.0f/largestT;

		/* Largest is r since p and q are 0.
		 */
		topExponent.f = sfabs(adj_r) * largestT;

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

		adj_r = adj_r * largestT;

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

		/*texture parameters
		*/ 
		IW( pTSP, 2, (((sgl_uint32) MipMant << SHIFT_PMIP_M) |
				  ((sgl_uint32) MipExp  << SHIFT_PMIP_E) |
				  (((sgl_int32) adj_r) & 0xffffUL)));

		/* p and q are zero.
		 */
        IW( pTSP, 3, 0);

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
#endif	/* #if PACKTSPMIPMAPTEXNONPER	*/


/**************************************************************************
 * Function Name  : PackTSPTexNonPer
 * Inputs         : PIMATERIAL pMat
 *				 	int nPolys
 *					int TypeFlag
 *					sgl_uint32 TSPIncrement
 *					sgl_uint32 *pTSP
 *					int MatSize
 * Outputs        : 
 * Returns        : Number of polys processed.
 * Global Used    : 
 * Description    : This is the non perspective texture function.
 *
 **************************************************************************/

#if !PACKTSPTEXNONPER

#define PackTSPTexNonPer _PackTSPTexNonPer

extern int PackTSPTexNonPer (PIMATERIAL pMat, 
				int nPolys, int TypeFlag, 
				sgl_uint32 TSPIncrement, 
				sgl_uint32 *pTSP, int MatSize);

#else
int PackTSPTexNonPer (PIMATERIAL pMat, 
				int nPolys, int TypeFlag, 
				sgl_uint32 TSPIncrement, 
				sgl_uint32 *pTSP, int MatSize)
{
	float 	largestT;
	float	adj_a,adj_b,adj_c,adj_d,adj_e,adj_f,adj_r;
	flong 	topExponent;
	long	exp;


	while (nPolys--)
	{
		adj_a = pMat->Tex.u0;
		adj_b = pMat->Tex.u1;
		adj_c = pMat->Tex.u2;

		adj_d = pMat->Tex.v0;
		adj_e = pMat->Tex.v1;
		adj_f = pMat->Tex.v2;

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
			{
				float 	FabsT1,FabsT2;

				largestT = SMALL_FLOAT_VALUE;

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
			}
		#endif

		/*
		** calculate a fast floor(log2(largest/largest_bot))
		*/
		largestT=1.0f/largestT;

		/* r is the largest of p, q & r, since p & q are zero.
		 */
		topExponent.f = sfabs(adj_r) * largestT;

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

		adj_r=adj_r * largestT;

		/*
		** Texas can't handle an exponent greater than 15,so we will
		** reduce the resolution of 'p', 'q', and 'r'.
		** THIS SHOULD HAPPEN **VERY** RARELY.The Level Of Detail
		** should have removed the texture by now.
		*/
		if(exp > 15)
		{
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

		/* p & q are zero.
		 */
        IW( pTSP, 3, 0);

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
#endif	/* #if PACKTSPTEXNONPER */

/* end of $RCSfile: dtexnp.c,v $ */


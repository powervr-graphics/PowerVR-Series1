/******************************************************************************
 * Name         : dshade.c
 * Title        : Smooth shading routines for PowerSGL Direct
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
 * $Log: dshade.c,v $
 * Revision 1.26  1997/06/18  14:39:09  mjg
 * Replaced windows types (UINT and the rest) with sgl_ typedefs.
 *
 * Revision 1.25  1997/06/09  08:58:50  gdc
 * got rid of compiler warning
 *
 * Revision 1.24  1997/05/29  11:53:47  sjf
 * 1) Found another problem with the smooth shading clipping - fixed it.
 * 2) Tidied up the code by removing a silly macro etc.
 *
 * Revision 1.23  1997/04/24  19:48:02  sjf
 * Urkk Missed another debug message. Gone now.
 *
 * Revision 1.22  1997/04/24  19:09:39  sjf
 * Missed commenting out a debug message.
 *
 * Revision 1.21  1997/04/24  19:02:57  sjf
 * Fixed the Clipping of smooth shading once and for all
 *        (touch wood)
 * There were two problems with it.
 * 1) The code to move the rep point when X or Y was > 1023 was
 *    completely wrong and changed the rep value the wrong way.
 * 2) When the point was moved back on screen, the rep value could
 *    go out of the -1 to 1 range. The original code simply clamped
 *    it.
 *    The new code now tries to find a better on screen point that
 *    gives either a -1 or 1 result.
 *
 * Revision 1.20  1997/03/14  18:09:03  jop
 *  Added pvrosapi.h include!
 *
 * Revision 1.19  1997/03/14  15:56:32  jwf
 * Added HWIO macros for macintosh compatability
 *
 * Revision 1.18  1997/02/04  14:46:07  mjg
 * SJF routines to prevent intensity overloads and hence stopping sparklies.
 *
 * Revision 1.17  1997/01/24  19:51:56  gdc
 * Added new version of Pack functions which do not write to TSP memory
 * but require the associated texture function to do it - instead they write
 * back to intermediate structures
 *
 * Revision 1.16  1997/01/19  15:31:11  sjf
 * Added code based on "HAS_FAST_BYTE_LOAD" which will speed up
 * MIPS. It was intended to speed up pentiums as well, but the
 * compiler squandered its oportunity.
 *
 * Revision 1.15  1997/01/16  21:23:42  sjf
 * Change conversion of float intensities to Fixed point intensities in 2 of
 * the routines to use the magically fp add method which shifts bits into
 * the correct positions.
 *
 * Revision 1.14  1997/01/15  09:46:51  sjf
 * Moved the mulitply by the Inverse Determinant from the matrix
 * multiply part of the code into the Intensity calc, as this
 * will save one floating point multiply per conversion.
 *
 * Revision 1.13  1997/01/15  08:34:49  sjf
 * 1) Removed dead macros
 * 2) Found a couple of float to UINT32s I'd missed.
 * 3) Also put in the better cliiping code into the light volume stuff.
 *
 * Revision 1.12  1997/01/14  17:20:58  sjf
 * Mostly optimisations plus fix of (rarely occuring) bug.
 * Details are...
 *
 * 1) Deleted Shadow Intensity routine. Shadow smooth shading now uses
 *    the one intensity routine. I put in a better factorisation which
 *    does less work.
 *
 * 2) Changed float-to-UINT conversions to float-to-int, as these should
 *    be faster on MIPS and use less stack space on PC.
 *
 * 3) Improved the Clip/NoClip decision code greatly in favour of the most
 *    likey case: i.e. no clipping.
 *
 * 4) Fixed a bug when clipping - the "base" intensity could go out of range
 *    and mess up the packing.
 *
 * 5) Made input-only parameters "const"
 *
 * 6) Changed some Right/Left Shifts to shift and masks: This should
 *    allow better optimisation on Pentium where shifts can only go
 *    down one pipe.
 *
 * 7) Tidied up comments.
 *
 * Revision 1.11  1997/01/10  01:31:37  sjf
 * 1) Optimised (well at least I think I have) the production of 3 intensity
 *    and average colour values from 3 colours routine. I've looked at the
 *    pentium assembler and it seems better, but I didn't notice any speed
 *    improvement.
 *
 * 2) Changed the code that handles the "small" polygon cases. These should
 *    also be faster (PLUS the code should be smaller too)
 *
 * 3) Noticed that R&B were being swapped in some of the packing cases eg. small
 *    polygons and shadow params. Have reversed these. I hope this wasn't some
 *    sort of bug-art requirement.
 *
 * Revision 1.10  1996/12/20  19:12:39  jop
 * Now downgrades shading to flat if less than a pixel in size
 *
 * Revision 1.9  1996/12/09  12:11:12  gdc
 * re-enabled clipped smooth sahding on L-V option, and removed a
 * couple of ^M's
 *
 * Revision 1.8  1996/12/06  12:45:22  gdc
 * re-enabled correction of smooth shading on clipped polys
 *
 * Revision 1.7  1996/11/28  11:53:02  sjf
 * Oooooo! Had loads of fun redoing the maths in here so that the matrix
 * operations assume a different basis. With this basis we have 0's in the
 * matrices which, of course, simplifies the maths enormously.
 *
 * Revision 1.6  1996/11/26  15:19:32  jop
 * Fixed r/b swap
 *
 * Revision 1.5  1996/11/22  09:38:53  gdc
 * removed some unecessary maths I'd left in
 *
 * Revision 1.4  1996/11/20  18:33:56  gdc
 * finished adding shadows and light volumes to this module
 *
 * Revision 1.3  1996/11/19  11:28:27  rgi
 * Needed a SGL_EXTERN_TIME_REF if METRICs is to work
 *
 * Revision 1.2  1996/11/19  00:55:41  jop
 * Got it to work
 *
 * Revision 1.1  1996/11/18  19:06:15  jop
 * Initial revision
 *
 * Revision 1.1  1996/11/18  18:29:36  jop
 * Initial revision
 *
 *
 *****************************************************************************/

#include "sgl_defs.h"
#include "metrics.h"
#include "sgl_math.h"

#include "dtri.h"
#include "dshade.h"

#include "pvrosapi.h"

SGL_EXTERN_TIME_REF /* if we are timing code */

/*
// In converting 3 Colours to Average and 3 Intensities, we need to extract
// each and every colour component. We can either do this via Shifts and
// masks OR use Byte pointers
*/
#define  READ_RBGS_VIA_BYTE_PTRS (HAS_FAST_BYTE_LOAD)

#if READ_RBGS_VIA_BYTE_PTRS

	#if BIG_ENDIAN
		#define R_BYTE_OFFSET 3
		#define G_BYTE_OFFSET 2
		#define B_BYTE_OFFSET 1
	#else
		#define R_BYTE_OFFSET 0
		#define G_BYTE_OFFSET 1
		#define B_BYTE_OFFSET 2
	#endif

#define GET_R(UINT32_LOCATION) (((const sgl_uint8 *) & UINT32_LOCATION) [R_BYTE_OFFSET])
#define GET_G(UINT32_LOCATION) (((const sgl_uint8 *) & UINT32_LOCATION) [G_BYTE_OFFSET])
#define GET_B(UINT32_LOCATION) (((const sgl_uint8 *) & UINT32_LOCATION) [B_BYTE_OFFSET])

#else

#define GET_R(x)				(((x) >> 16) & 0x000000FF)
#define GET_G(x)				(((x) >> 8) & 0x000000FF)
#define GET_B(x)				((x) & 0x000000FF)

#endif 

#define DOWNGRADE_TO_FLAT		(0.375f)   /* 2^(-2) + 2^(-3) */
/* To prevent the intensity of a single face becoming too large, and
 * causing sparklies, we have the test the Adjoint matrix.
 */
#define INTENSITY_OVERFLOW		(0.21875f) /* 2^(-3) + 2^(-4) + 2^(-5) */

/**********************************************************************/
static sgl_uint32 /*INLINE*/ PackTSPSmoothGetI ( const IMATERIAL *pMat, 
											 	 const float	 InverseDet,
												 float	 		 fIntensity[3] )
{
	int RTotal, GTotal, BTotal;
	int V0Int,	V1Int,	V2Int;
	int IntensityTotals;
	int  nTmp;
	float fLargest;
	float fIntensityScale, IScaleByIDet;
	float fColourScale;
	int	  nColourScale;

	int FinalColour;
    /*
	// Extract each of the red green and blue components from each of the
	// three vertex colours, and sum up the totals of the R,G,Bs PLUS the
	// total of each component for the three verts, which will be used to
	// compute an "intensity"
	*/
#if READ_RBGS_VIA_BYTE_PTRS
	/* Vertex 0 */
	RTotal = GET_R(pMat->Shading.u.Smooth.Col0);
	GTotal = GET_G(pMat->Shading.u.Smooth.Col0);
	BTotal = GET_B(pMat->Shading.u.Smooth.Col0);
	V0Int  = RTotal + GTotal + BTotal;
	/* Vertex 1 */
	V1Int  = GET_R(pMat->Shading.u.Smooth.Col1);
	RTotal+= V1Int;

	nTmp   = GET_G(pMat->Shading.u.Smooth.Col1);
	GTotal+= nTmp;
	V1Int += nTmp;

	nTmp   = GET_B(pMat->Shading.u.Smooth.Col1);
	BTotal+= nTmp;
	V1Int += nTmp;
	/* Vertex 2 */
	V2Int  = GET_R(pMat->Shading.u.Smooth.Col2);
	RTotal+= V2Int;

	nTmp   = GET_G(pMat->Shading.u.Smooth.Col2);
	GTotal+= nTmp;
	V2Int += nTmp;

	nTmp   = GET_B(pMat->Shading.u.Smooth.Col2);
	BTotal+= nTmp;
	V2Int += nTmp;

#else

	int Colour;

	/* Vertex 0 */
	Colour = pMat->Shading.u.Smooth.Col0;
	RTotal = GET_R(Colour);
	GTotal = GET_G(Colour);
	BTotal = GET_B(Colour);
	V0Int  = RTotal + GTotal + BTotal;

	/* Vertex 1 */
	Colour = pMat->Shading.u.Smooth.Col1;
	V1Int  = GET_R(Colour);
	RTotal+= V1Int;

	nTmp   = GET_G(Colour);
	GTotal+= nTmp;
	V1Int += nTmp;

	nTmp   = GET_B(Colour);
	BTotal+= nTmp;
	V1Int += nTmp;

	/* Vertex 2 */
	Colour = pMat->Shading.u.Smooth.Col2;
	V2Int  = GET_R(Colour);
	RTotal+= V2Int;

	nTmp   = GET_G(Colour);
	GTotal+= nTmp;
	V2Int += nTmp;

	nTmp   = GET_B(Colour);
	BTotal+= nTmp;
	V2Int += nTmp;

#endif

	/*
	// We now want to find the largest overal colour component, so that
	// we can scale up the colours. Our interpolation of intensity can't make
	// things brighter, only darker, so to allow us to interpolate outside
	// of the triangle we base the calc on, we need to make the colours as
	// bright as possible, and at the same time reduce the intensities by
	// the same factor.
	*/
	if(RTotal > GTotal)
	{
		if(RTotal > BTotal)
		{
			fLargest = (float) RTotal;
		}
		else
		{
			fLargest = (float) BTotal;
		}
	}
	else if(GTotal > BTotal)
	{
		fLargest = (float) GTotal;
	}
	else if(BTotal > 0)
	{
		fLargest = (float) BTotal;
	}
	/*
	// Else the whole thing is black. Exit out now
	*/
	else
	{
		fIntensity[0] = 0.0f;
		fIntensity[1] = 0.0f;
		fIntensity[2] = 0.0f;

		return 0;
	}

	/*
	// At this point we could check to see if the largest is so close to its
	// maximum value, that scaling it up futher would be a complete waste of
	// time. We need to do some investigation to see if this would be
	// worthwhile for various benchmarks ;)
	*/


	/*
	// Compute scaling factors: One to scale the colours up, the other to scale
	// the intensity values down (as soon as we compute them!).
	//
	// Note we do the colour scale as a "fixed point" value which is equiv to
	// (255/Largest)*256. It's difficult to explain. This, of course, contains a
	// disgusting division followed by a float to int conversion. URK!
	*/
	fColourScale = (256.0f * 255.0f)/ fLargest;

	/*
	// Add all the "intensity" values to get the "average" intensity.
	// Note that we can use the RTotal etc values just as well. These are
	// more likely to be sitting in registers than V0Int etc since they
	// were used more recently.
	//
	// Hopefully it should also give us some integer stuff to do while
	// the divide occurs?
	*/
	IntensityTotals	= RTotal + GTotal + BTotal;

	/*
	// Convert the colour scaling to our integer
	*/
	nColourScale = (int) (fColourScale);

	/*
	// Compute the scaling factor for the Intensities.  Now the overall colours
	// have been sort of scaled up by fColourScale, so the intensities should
	// come down by 1/fColour scale. With a bit of luck the floating point in
	// this should be overlapable with the previous code, since they are
	// independant.
	//
	// Being more precise...
	//
	// If we ignore the colour rescaling for the moment, then the scaling factor
	// for the intensities should be 3/(IntensityTotals). 
	//
	// Since each of the colours have been made brighter by (3*255)/fLargest
	// (note we have cunningly hidden the 3x in the integer maths), we should
	// therefore scale down our intensity by fLargest/(3*255). This then gives
	// a combined scaling of...
	//
	//  scaling = fLargest * (1.0f/255)) /(IntensityTotals).
	*/
	fIntensityScale = fLargest * (1.0f/255.0f) / ((float) IntensityTotals);

	/* 
	// Scale the RGB components so that they only occupy 5 bits. Int Muls are
	// slow on a Pentium, but should be 2x faster on MIPS 5000. However, with
	// a bit of luck, these will go on while the previous division is chugging
	// away.
	*/
	RTotal = (nColourScale * RTotal) >> (11 - 10);
	GTotal = (nColourScale * GTotal) >> (11 -  5);
	BTotal = (nColourScale * BTotal) >>  11;
	FinalColour	= (RTotal & (0x1f << 10)) | (GTotal & (0x1f << 5)) | BTotal;

	/*
	// lastly, calc the scaled intensity (delta) values...
	*/
	IScaleByIDet = fIntensityScale * InverseDet;

	fIntensity[0] = ((float) (V0Int - V2Int)) * IScaleByIDet;
	fIntensity[1] = ((float) (V1Int - V2Int)) * IScaleByIDet;
	fIntensity[2] = ((float) (V0Int)) * fIntensityScale;

	return (FinalColour);
}

/**********************************************************************/
/**************************************************************************
 * Function Name  : ConvertIntensitiesToFixedPoint (internal only)
 * Inputs         : fIntensity - float point intensity values
 * Outputs        : nIntensity - fixed point intensity equivalents
 * Returns        : nix.
 * Global Used    : nix
 * Description    : Converts the floating point values to their fixed point
 *					equivalents. This does in one of 2 ways depending on the
 *					target hardware. On normal processors (eg MIPS) it will
 *					multiply the values by a scaling factor and then convert
 *					to integer.
 *
 *					On Pentium style CPUs, it will do the conversion by adding
 *					"magic" large integers which effectively shift the floats
 *					during the fp-add denormalise part, so that the important bits 
 *					occupy the lower bits of the number. These can then be masked
 *					off.
 *
 *					HMM Perhaps the second might be quicker on the Mips as well.
 *
 *					NOTE: We mask the off the upper bits on the converted 
 *					fbase and fT2, and keep just the lower 16. 
 *					For the converted fT1, we want it's bits in the upper 16 bits.
 *
 *
 *					The "ORIGINAL" version assumes that fBase is POSITIVE.
 **************************************************************************/

static void INLINE ConvertIntensitiesToFixedPoint( const float	 fBase,
												   const float	 fT1,
												   const float	 fT2,
													sgl_uint32	 nIntensity[2])
{
#if 0 /* !(SLOW_FTOI) */

	ASSERT(fBase >= 0.0f);

	nIntensity[0] = (sgl_int32)(fBase * (float) 0x4000);
	nIntensity[1] = ( ((sgl_int32)(fT2  * (float) 0x10000)) & 0xFFFF) |
					( ((sgl_int32)(fT1  * (float) 0x10000)) << 16);
#else
/*
// To convert a float to an int, we need to add 1.5 * 2^23. To allow for the
// extra "fraction" bits in the fixed point number, reduce the number of
// shifts that occur in the FP add by the appropriate amount
*/
#define F_TO_INT_AND_SCALE_BY_0x4000  (1.5f * ((float) (1 << 23)) / ((float) 0x4000))
#define F_TO_INT_AND_SCALE_BY_0x10000 (1.5f * ((float) (1 << 23)) / ((float) 0x10000))
	union
	{
		float  f;
		sgl_uint32 i;
	} flongFBase, flongFT1, flongFT2;

	flongFBase.f = fBase + F_TO_INT_AND_SCALE_BY_0x4000;
	flongFT1.f	 = fT1	 + F_TO_INT_AND_SCALE_BY_0x10000;
	flongFT2.f	 = fT2	 + F_TO_INT_AND_SCALE_BY_0x10000;

	nIntensity[0] = flongFBase.i & 0xFFFF;
	nIntensity[1] = (flongFT2.i	 & 0xFFFF) | (flongFT1.i << 16);

#endif
}

/**************************************************************************
 * Function Name  : SmoothShadingClipper (internal only)
 * Inputs         : fT2, fT1, are the rates of change of intensity for
 *					X and Y respectively.
 * Outputs        : 
 * Input/Ouput	  : fBase- the base intensity
 *					nX, nY: The screen coordinate for the base intensity.
 * Returns        : nix.
 * Global Used    : nix
 * Description    : This routine should be called if nX and/or nY are outside
 *					the [0..1023] range.  The routine produces a new rep' location
 *					that is not only on screen, but that the intensity is in
 *					range as well. (i.e. -1 to 1).
 **************************************************************************/
static void INLINE SmoothShadingClipper( int * pnX, 
										 int *pnY,
										 float *pfBase,
										 float fT1,
										 float fT2)
{
	int nX, nY;
	float fBase;

	/*
	// Get local copies of the values
	*/
	nX = *pnX;
	nY = *pnY;
	fBase = *pfBase;

	if (nY < 0)
	{
		fBase -= fT1 * nY;
		nY = 0;
	}
	else if (nY > 1023)
	{
	  /*DPF(( DBG_FATAL, "nY= %d fBASE %f => %f ", 
		   nY, fBase, fBase - fT1 * (nY -1023))); */
	
		fBase += fT1 * (1023 - nY);
		nY = 1023;
	}
	
	if (nX < 0)
	{
		fBase -= fT2 * nX;
		nX = 0;
	}
	else if (nX > 1023)
	{
		fBase += fT2 * (1023 - nX);
		nX = 1023;
	}
	/*
	// If we have gone outside our legal base value range,
	// try to find a pixel where it is OK again
	*/
	if(sfabs(fBase) > 1.0f)
	{
		float TargetVal;
	    float EndPoint;
	
		/*	
		DPF(( DBG_FATAL, "Out of Range fBase:%f fT2:%f fT1:%f",
			  fBase,  fT2, fT1));
	    DPF(( DBG_FATAL, "    X:%d  y:%d", nX, nY));
		*/

		if(fBase < 0.0f)
			TargetVal = -1.0f;
		else
			TargetVal =  1.0f;
	
	    /*
		// search in the Y direction first
		*/
		if(sfabs(fT1) >= 1.0E-20f)
		{
			EndPoint = ((TargetVal - fBase) / fT1) + nY;
	
			/*DPF(( DBG_FATAL, "Move Y from % d to %d", 
				   nY, EndPoint));*/
			if(EndPoint < 0)
			{
				fBase -= (nY * fT1);
				nY = 0;
			}
			else if(EndPoint > 1023)
			{
				fBase += ((1023 - nY) * fT1);
				nY = 1023;
			}
			else
			{
				fBase = TargetVal;
				nY =    (int) EndPoint;

				/*
				// In Range so save back the final values and exit
				*/
				*pnX = nX;
				*pnY = nY;
				*pfBase = fBase;
				return;

			}
		}
	
	    /*
		// Now search in the X direction
		*/
		if(sfabs(fT2) >= 1.0E-20f)
		{
			EndPoint = ((TargetVal - fBase) / fT2) + nX;
	
			/*			DPF(( DBG_FATAL, "Move X from % d to %d", 
				   nX, EndPoint)); */
	
			if(EndPoint < 0)
			{
				fBase -= (nX * fT2);
				nX = 0;
			}
			else if(EndPoint > 1023)
			{
				fBase += ((1023 - nX) * fT2);
				nX = 1023;
			}
			else
			{
				fBase = TargetVal;
				nX = (int) EndPoint;

				/*
				// In Range so save back the final values and exit
				*/
				*pnX = nX;
				*pnY = nY;
				*pfBase = fBase;
				return;
			}
		}
	
		/*
		// Finally, if we are still out of range, clamp it
		*/
		if(fBase < -1.0f)
		{
		  /*DPF((DBG_FATAL, "Fbase STILL OUT of range:%f ", fBase));*/
			fBase = -1.0f;
	
		}
		else /*MUST be > 1.0*/
		{
		  /*DPF((DBG_FATAL, "Fbase STILL OUT of range:%f ", fBase));*/
			fBase = 1.0f;
		}
	}/*end if fbase is out of the legal range*/

	/*
	// save back the final values if we haven't already done so
	*/
	*pnX = nX;
	*pnY = nY;
	*pfBase = fBase;

}



/**********************************************************************/
/**********************************************************************/
/* Two versions of Smooth packing follow the first stores data back into
** the local storage and assumes that it precedes the texture function 
** which will write the data, the second writes the TSP data directly
*/
/**********************************************************************/
/**********************************************************************/

void PackSmoothAndStore ( const ITRI *pTri,
						  IMATERIAL *pMat, 
						  int nPolys)
{
	float 		fIntensity[3],  fT1, fT2;
	int			nX, nY;

	SGL_TIME_START(SMOOTH_TRI_CLIP_PARAM_TIME)

	#define INV(x,y)	pTri->fAdjoint[x][y]
	#define INTEN(x)	fIntensity[x]

	while (nPolys--)
	{
		if (pTri->f1OverDet > DOWNGRADE_TO_FLAT)
		{
			/* if it's a small triangle, flat shade */
			sgl_uint32  Av555Colour;

			/*
			// Compute the average of the colours and put the result in
			// the top bits
			*/
			AVERAGE_RBG_TO_555_TOP(Av555Colour, pMat->Shading.u.Smooth.Col0,
												pMat->Shading.u.Smooth.Col1,
												pMat->Shading.u.Smooth.Col2);

			pMat->Shading.u.WriteBack.ModControl = pTri->TSPControlWord;
			pMat->Shading.u.WriteBack.ModBaseCol = 0;

			pMat->Shading.u.WriteBack.Smooth1 = 0x4000 | Av555Colour;
			pMat->Shading.u.WriteBack.Smooth2 = 0;
		}
		else
		{
			float fBase;
			sgl_uint32 Col16;

			Col16 = PackTSPSmoothGetI(pMat, pTri->f1OverDet, fIntensity);
					
			/* 
			// pre-multiply intensities by inverse of face matrix 
			// Note Have changed the "basis" so that Intensities 0&1 are 
			// relative to 2. This makes the matrix multiply cheaper.
			// The absolute value of 0 is stored in offset 2 (just to
			// confuse you)
			*/

			fT2 = INV(0,0) * fIntensity[0] + INV(0,1) * fIntensity[1];
			fT1 = INV(1,0) * fIntensity[0] + INV(1,1) * fIntensity[1];

			if ((sfabs(fT1) > INTENSITY_OVERFLOW) || (sfabs(fT2) > INTENSITY_OVERFLOW))
			{
				/* If it's going to overflow (and sparkle), flat shade */
				sgl_uint32  Av555Colour;

				AVERAGE_RBG_TO_555_TOP(Av555Colour, pMat->Shading.u.Smooth.Col0,
													pMat->Shading.u.Smooth.Col1,
													pMat->Shading.u.Smooth.Col2);

				pMat->Shading.u.WriteBack.ModControl = pTri->TSPControlWord;
				pMat->Shading.u.WriteBack.ModBaseCol = 0;

				pMat->Shading.u.WriteBack.Smooth1 = 0x4000 | Av555Colour;
				pMat->Shading.u.WriteBack.Smooth2 = 0;

				pTri ++;
				pMat ++;
				continue;
			}

			fBase = fIntensity[2];

			ASSERT(fBase >= 0.0f);
			
			/* adjust for offscreen rep point if necessary */
			nX = pMat->Shading.u.Smooth.nX;
			nY = pMat->Shading.u.Smooth.nY;

			/*
			// if we need to do any clipping. This is only necessary
			// when nX or NY are outside the range [0..1023]. Since
			// these as special binary numbers we can do the range 
			// checking quite quickly. The test becomes...
			// Is the sign bit set on either of them OR any bit
			// implying 1024 or greater.
			*/
			if( (nX | nY) & (~1023) )
			{
				SmoothShadingClipper(&nX, &nY, &fBase, fT1, fT2);
			}/* end if clipping necessary */

			pMat->Shading.u.WriteBack.ModControl = pTri->TSPControlWord;
			pMat->Shading.u.WriteBack.ModBaseCol = nY | (nX << 16);
			{
				sgl_uint32		u32Intensity[2];

				ConvertIntensitiesToFixedPoint(fBase, fT1, fT2, u32Intensity);

				pMat->Shading.u.WriteBack.Smooth1 = u32Intensity[0] | (Col16 << 16);
				pMat->Shading.u.WriteBack.Smooth2 = u32Intensity[1];
			}
		}

		pTri ++;
		pMat ++;
	}

	#undef INV

	SGL_TIME_STOP(SMOOTH_TRI_CLIP_PARAM_TIME);
}

/**********************************************************************/

void PackTSPSmooth ( const ITRI *pTri,
			   const IMATERIAL *pMat, 
						   int nPolys, 
						sgl_uint32 TSPIncrement, 
						sgl_uint32 TSPGap, 
						sgl_uint32 *pTSP)
{
	float 		fIntensity[3],  fT1, fT2;
	int			nX, nY;

	SGL_TIME_START(SMOOTH_TRI_CLIP_PARAM_TIME)

	#define INV(x,y)	pTri->fAdjoint[x][y]

	while (nPolys--)
	{
		if (pTri->f1OverDet > DOWNGRADE_TO_FLAT) 
		{
			/* if it's a small triangle, flat shade */
			sgl_uint32  Av555Colour;

			/*
			// Compute the average of the colours and put the result in
			// the top bits
			*/
			AVERAGE_RBG_TO_555_TOP(Av555Colour, pMat->Shading.u.Smooth.Col0,
												pMat->Shading.u.Smooth.Col1,
												pMat->Shading.u.Smooth.Col2);

			IW( pTSP, 0, (pTri->TSPControlWord));
			IW( pTSP, 1, 0);
			IW( pTSP, TSPGap + 2, (0x4000 | Av555Colour));
			IW( pTSP, TSPGap + 3, 0);
		}
		else
		{
			float fBase;
			sgl_uint32 Col16;

			Col16 = PackTSPSmoothGetI(pMat, pTri->f1OverDet, fIntensity);
					
			/* 
			// pre-multiply intensities by inverse of face matrix 
			// Note Have changed the "basis" so that Intensities 0&1 are 
			// relative to 2. This makes the matrix multiply cheaper.
			// The absolute value of 0 is stored in offset 2 (just to
			// confuse you)
			*/

			fT2 = INV(0,0) * fIntensity[0] + INV(0,1) * fIntensity[1];
			fT1 = INV(1,0) * fIntensity[0] + INV(1,1) * fIntensity[1];

			if ((sfabs(fT1) > INTENSITY_OVERFLOW) || (sfabs(fT2) > INTENSITY_OVERFLOW))
			{
				/* If it's going to overflow (and sparkle), flat shade */
				sgl_uint32  Av555Colour;

				AVERAGE_RBG_TO_555_TOP(Av555Colour, pMat->Shading.u.Smooth.Col0,
													pMat->Shading.u.Smooth.Col1,
													pMat->Shading.u.Smooth.Col2);


				IW( pTSP, 0, (pTri->TSPControlWord));
				IW( pTSP, 1, 0);
				IW( pTSP, TSPGap + 2, (0x4000 | Av555Colour));
				IW( pTSP, TSPGap + 3, 0);

				pTri ++;
				pMat ++;
				pTSP += TSPIncrement;
				continue;
			}
			
			fBase = fIntensity[2];

			ASSERT(fBase >= 0.0f);
			
			/* adjust for offscreen rep point if necessary */
			nX = pMat->Shading.u.Smooth.nX;
			nY = pMat->Shading.u.Smooth.nY;

			/*
			// if we need to do any clipping. This is only necessary
			// when nX or NY are outside the range [0..1023]. Since
			// these as special binary numbers we can do the range 
			// checking quite quickly. The test becomes...
			// Is the sign bit set on either of them OR any bit
			// implying 1024 or greater.
			*/
			if( (nX | nY) & (~1023) )
			{
				SmoothShadingClipper(&nX, &nY, & fBase, fT1, fT2);
			}/* end if clipping necessary */


			IW( pTSP, 0, (pTri->TSPControlWord));
			IW( pTSP, 1, (nY | (nX << 16)));
			{
				sgl_uint32		u32Intensity[2];

				ConvertIntensitiesToFixedPoint(fBase, fT1, fT2, u32Intensity);

				IW( pTSP, TSPGap + 2, (u32Intensity[0] | (Col16 << 16)));
				IW( pTSP, TSPGap + 3, u32Intensity[1]);
			}
		}

		pTri ++;
		pMat ++;
		pTSP += TSPIncrement;
	}

	#undef INV

	SGL_TIME_STOP(SMOOTH_TRI_CLIP_PARAM_TIME);
}


/**********************************************************************/
/*                   Shadow Version of Above                          */
/**********************************************************************/

/**********************************************************************/
/**********************************************************************/

void PackTSPSmoothShad ( const ITRI 	 *pTri, 
						 const IMATERIAL *pMat, 
						 int 			 nPolys, 
						 sgl_uint32 	 TSPIncrement, 
						 sgl_uint32 	 TSPGap, 
						 sgl_uint32 	 *pTSP )
{
	float 		fIntensity[3], fT1, fT2;
	int			nX, nY;
	
	float 	fShadowCoeff	= pMat->v.ShadowBrightness;
	float 	fNonShadowCoeff = 1.0f - fShadowCoeff;
	int		iShad, iNonShad;

	float fShadowCoeff0x4000,	 fShadowCoeff0x10000;
	float fNonShadowCoeff0x4000, fNonShadowCoeff0x10000;

	SGL_TIME_START(SMOOTH_TRI_CLIP_PARAM_TIME)

	/*
	// Compute integer coeffs for when we resort to "flat" shading because the
	// poly is small. Note that these have been "adjusted" so that the 
	// conversion/averaging process closesly mirrors the macro AVERAGE_RBG_TO_555_TOP
	//
	// Note iShad and iNonShad have been shifted up for the Green component, so that
	// it won't involve any shifting. We still, of course have top shift down for
	// red and blue, so they will need to shift down by a further 2, giving a total
	// of 3.
	*/
	iShad	 = ((int) (85 * fShadowCoeff)) << 2;
	iNonShad = ((int) (85 * fNonShadowCoeff)) << 2;

	/*
	// Pre-multiply the packing scale values by the shadow and non shadow
	// co-efficients since they are loop invariant.
	*/
	fShadowCoeff0x4000  = fShadowCoeff * (float) 0x4000;
	fShadowCoeff0x10000 = fShadowCoeff * (float) 0x10000;

	fNonShadowCoeff0x4000  = fNonShadowCoeff * (float) 0x4000;
	fNonShadowCoeff0x10000 = fNonShadowCoeff * (float) 0x10000;

	#define INV(x,y)	pTri->fAdjoint[x][y]

	while (nPolys--)
	{
		if (pTri->f1OverDet > DOWNGRADE_TO_FLAT)
		{
			/*
			// if it's a small triangle, "flat" shade
			//
			// The "averageing" and scaling process uses the same technique as the macro
			//   "AVERAGE_RBG_TO_555_TOP" in sglmacro.h.  Well, at least I hope so.
			// It's been adapted to cope with the extra scaling info. Go look it at
			// it for some explanation.
			*/
			sgl_uint32 Col;
			sgl_uint32 RedNBlue, GreenNTop;
			sgl_uint32 ShadRedNBlue, ShadGreenNTop;

			Col = pMat->Shading.u.Smooth.Col0;
			RedNBlue  = Col;
			GreenNTop = Col & 0xFF00FF00;

			Col = pMat->Shading.u.Smooth.Col1;
			RedNBlue  += Col;
			GreenNTop += Col & 0xFF00FF00;

			Col = pMat->Shading.u.Smooth.Col2;
			RedNBlue  += Col;
			GreenNTop += Col & 0xFF00FF00;

			/*
			// Remove the Green and top bits from RedNBlue
			*/
			RedNBlue -= GreenNTop;

			/*
			// Scale each of R,G & B appropriately so that they are not only scaled, BUT
			// MOSTLY land in the right places so we don't have to do too much shifting
			//
			// Do the shadow one first
			*/
			ShadRedNBlue = RedNBlue  * (iShad >> 3);
			ShadGreenNTop= GreenNTop *  iShad;

			/*
			// And now do the non-shadowed
			*/
			RedNBlue = RedNBlue  * (iNonShad >> 3);
			GreenNTop= GreenNTop *  iNonShad;


			IW( pTSP, 0, (pTri->TSPControlWord));
			IW( pTSP, 1, 0);
			IW( pTSP, TSPGap + 2, (0x4000 | (ShadRedNBlue  & 0x7C000000)
									  | (ShadGreenNTop & 0x03E00000)
									  |((ShadRedNBlue  & 0xF800) << (20 - 14))));
			IW( pTSP, TSPGap + 3, 0);
			IW( pTSP, TSPGap + 4, (0x4000 | (RedNBlue  & 0x7C000000)
									  | (GreenNTop & 0x03E00000)
									  |((RedNBlue  & 0xF800) << (20 - 14))));
			IW( pTSP, TSPGap + 5, 0);
		}
		else
		{
			float fBase;
			sgl_uint32 Col16;

			/*
			// get the overall colour and intensity, and shift the colour into
			// the correct position.
			*/
			Col16 = PackTSPSmoothGetI(pMat, pTri->f1OverDet, fIntensity);
			Col16 <<= 16;

			/* 
			// pre-multiply intensities by inverse of face matrix 
			// Note: By changing the basis slightly, I've reduced the maths! SJF
			*/
			fT2 = INV(0,0) * fIntensity[0] + INV(0,1) * fIntensity[1];
			fT1 = INV(1,0) * fIntensity[0] + INV(1,1) * fIntensity[1];

			if ((sfabs(fT1) > INTENSITY_OVERFLOW) || (sfabs(fT2) > INTENSITY_OVERFLOW))
			{
				/* If it's going to overflow (and sparkle), flat shade */
				sgl_uint32 Col;
				sgl_uint32 RedNBlue, GreenNTop;
				sgl_uint32 ShadRedNBlue, ShadGreenNTop;

				Col = pMat->Shading.u.Smooth.Col0;
				RedNBlue  = Col;
				GreenNTop = Col & 0xFF00FF00;

				Col = pMat->Shading.u.Smooth.Col1;
				RedNBlue  += Col;
				GreenNTop += Col & 0xFF00FF00;

				Col = pMat->Shading.u.Smooth.Col2;
				RedNBlue  += Col;
				GreenNTop += Col & 0xFF00FF00;

				RedNBlue -= GreenNTop;

				ShadRedNBlue = RedNBlue  * (iShad >> 3);
				ShadGreenNTop= GreenNTop *  iShad;
		
				/* And now do the non-shadowed */
			
				RedNBlue = RedNBlue  * (iNonShad >> 3);
				GreenNTop= GreenNTop *  iNonShad;

				IW( pTSP, 0, (pTri->TSPControlWord));
				IW( pTSP, 1, 0);
				IW( pTSP, TSPGap + 2, (0x4000 | (ShadRedNBlue  & 0x7C000000)
										  | (ShadGreenNTop & 0x03E00000)
										  |((ShadRedNBlue  & 0xF800) << (20 - 14))));
				IW( pTSP, TSPGap + 3, 0);
				IW( pTSP, TSPGap + 4, (0x4000 | (RedNBlue  & 0x7C000000)
										  | (GreenNTop & 0x03E00000)
										  |((RedNBlue  & 0xF800) << (20 - 14))));
				IW( pTSP, TSPGap + 5, 0);

				pTri ++;
				pMat ++;
				pTSP += TSPIncrement;
				continue;
			}				
			
			fBase = fIntensity[2];
			
			ASSERT(fBase >= 0.0f);
			
			/* adjust for offscreen rep point if necessary */
			nX = pMat->Shading.u.Smooth.nX;
			nY = pMat->Shading.u.Smooth.nY;

			/*
			// If we need to clip the shading rep point
			// (See previous routine for an explanation of the logic)
			*/
			if( (nX | nY) & (~1023) )
			{
				SmoothShadingClipper(&nX, &nY, & fBase, fT1, fT2);
			}/* end if clipping necessary */

			IW( pTSP, 0, (pTri->TSPControlWord));
			IW( pTSP, 1, (nY | (nX << 16)));

			/*
			// Compute and save the bit which can be in the shadow.
			*/
			IW( pTSP, TSPGap + 2, (((sgl_int32) (fBase * fShadowCoeff0x4000)) | Col16));
			IW( pTSP, TSPGap + 3, ((((sgl_int32) (fT2  * fShadowCoeff0x10000)) & 0xFFFF) | 
							   (((sgl_int32) (fT1  * fShadowCoeff0x10000)) << 16)));
			/*
			// And save the bit which is added if you are outside of the shadow.
			*/
			IW( pTSP, TSPGap + 4, (((sgl_int32) (fBase * fNonShadowCoeff0x4000)) | Col16));
			IW( pTSP, TSPGap + 5, ((((sgl_int32) (fT2  * fNonShadowCoeff0x10000)) & 0xFFFF) | 
							   (((sgl_int32) (fT1  * fNonShadowCoeff0x10000)) << 16)));
		}/*end if*/
		
		pTri ++;
		pMat ++;
		pTSP += TSPIncrement;
	}/*end while*/

	#undef INV

	SGL_TIME_STOP(SMOOTH_TRI_CLIP_PARAM_TIME)
}

/**********************************************************************/

void PackSmoothShadAndStore ( const ITRI *pTri, 
							  IMATERIAL *pMat, 
							  int nPolys)
{
	float 		fIntensity[3], fT1, fT2;
	int			nX, nY;
	
	float 	fShadowCoeff	= pMat->v.ShadowBrightness;
	float 	fNonShadowCoeff = 1.0f - fShadowCoeff;
	int		iShad, iNonShad;

	float fShadowCoeff0x4000,	 fShadowCoeff0x10000;
	float fNonShadowCoeff0x4000, fNonShadowCoeff0x10000;

	SGL_TIME_START(SMOOTH_TRI_CLIP_PARAM_TIME)

	/*
	// Compute integer coeffs for when we resort to "flat" shading because the
	// poly is small. Note that these have been "adjusted" so that the 
	// conversion/averaging process closesly mirrors the macro AVERAGE_RBG_TO_555_TOP
	//
	// Note iShad and iNonShad have been shifted up for the Green component, so that
	// it won't involve any shifting. We still, of course have top shift down for
	// red and blue, so they will need to shift down by a further 2, giving a total
	// of 3.
	*/
	iShad	 = ((int) (85 * fShadowCoeff)) << 2;
	iNonShad = ((int) (85 * fNonShadowCoeff)) << 2;

	/*
	// Pre-multiply the packing scale values by the shadow and non shadow
	// co-efficients since they are loop invariant.
	*/
	fShadowCoeff0x4000  = fShadowCoeff * (float) 0x4000;
	fShadowCoeff0x10000 = fShadowCoeff * (float) 0x10000;

	fNonShadowCoeff0x4000  = fNonShadowCoeff * (float) 0x4000;
	fNonShadowCoeff0x10000 = fNonShadowCoeff * (float) 0x10000;

	#define INV(x,y)	pTri->fAdjoint[x][y]

	while (nPolys--)
	{
		if (pTri->f1OverDet > DOWNGRADE_TO_FLAT)
		{
			/*
			// if it's a small triangle, "flat" shade
			//
			// The "averageing" and scaling process uses the same technique as the macro
			//   "AVERAGE_RBG_TO_555_TOP" in sglmacro.h.  Well, at least I hope so.
			// It's been adapted to cope with the extra scaling info. Go look it at
			// it for some explanation.
			*/
			sgl_uint32 Col;
			sgl_uint32 RedNBlue, GreenNTop;
			sgl_uint32 ShadRedNBlue, ShadGreenNTop;

			Col = pMat->Shading.u.Smooth.Col0;
			RedNBlue  = Col;
			GreenNTop = Col & 0xFF00FF00;

			Col = pMat->Shading.u.Smooth.Col1;
			RedNBlue  += Col;
			GreenNTop += Col & 0xFF00FF00;

			Col = pMat->Shading.u.Smooth.Col2;
			RedNBlue  += Col;
			GreenNTop += Col & 0xFF00FF00;

			/*
			// Remove the Green and top bits from RedNBlue
			*/
			RedNBlue -= GreenNTop;

			/*
			// Scale each of R,G & B appropriately so that they are not only scaled, BUT
			// MOSTLY land in the right places so we don't have to do too much shifting
			//
			// Do the shadow one first
			*/
			ShadRedNBlue = RedNBlue  * (iShad >> 3);
			ShadGreenNTop= GreenNTop *  iShad;

			/*
			// And now do the non-shadowed
			*/
			RedNBlue = RedNBlue  * (iNonShad >> 3);
			GreenNTop= GreenNTop *  iNonShad;


			pMat->Shading.u.WriteBack.ModControl = pTri->TSPControlWord;
			pMat->Shading.u.WriteBack.ModBaseCol = 0;

			pMat->Shading.u.WriteBack.Smooth1 = 
				0x4000 | (ShadRedNBlue  & 0x7C000000) |
				(ShadGreenNTop & 0x03E00000) |
				((ShadRedNBlue  & 0xF800) << (20 - 14));
			pMat->Shading.u.WriteBack.Smooth2 = 0;

			pMat->v.LightVolCol = 0x4000 | (RedNBlue  & 0x7C000000)
 										 | (GreenNTop & 0x03E00000)
	 									 |((RedNBlue  & 0xF800) << (20 - 14));
			pMat->Shading.u.Highlight = 0;
		}
		else
		{
			float fBase;
			sgl_uint32 Col16;

			/*
			// get the overall colour and intensity, and shift the colour into
			// the correct position.
			*/
			Col16 = PackTSPSmoothGetI(pMat, pTri->f1OverDet, fIntensity);
			Col16 <<= 16;

			/* 
			// pre-multiply intensities by inverse of face matrix 
			// Note: By changing the basis slightly, I've reduced the maths! SJF
			*/
			fT2 = INV(0,0) * fIntensity[0] + INV(0,1) * fIntensity[1];
			fT1 = INV(1,0) * fIntensity[0] + INV(1,1) * fIntensity[1];

			if ((sfabs(fT1) > INTENSITY_OVERFLOW) || (sfabs(fT2) > INTENSITY_OVERFLOW))
			{
				sgl_uint32 Col;
				sgl_uint32 RedNBlue, GreenNTop;
				sgl_uint32 ShadRedNBlue, ShadGreenNTop;

				Col = pMat->Shading.u.Smooth.Col0;
				RedNBlue  = Col;
				GreenNTop = Col & 0xFF00FF00;

				Col = pMat->Shading.u.Smooth.Col1;
				RedNBlue  += Col;
				GreenNTop += Col & 0xFF00FF00;

				Col = pMat->Shading.u.Smooth.Col2;
				RedNBlue  += Col;
				GreenNTop += Col & 0xFF00FF00;

				/*
				// Remove the Green and top bits from RedNBlue
				*/
				RedNBlue -= GreenNTop;

				ShadRedNBlue = RedNBlue  * (iShad >> 3);
				ShadGreenNTop= GreenNTop *  iShad;

				/*
				// And now do the non-shadowed
				*/
				RedNBlue = RedNBlue  * (iNonShad >> 3);
				GreenNTop= GreenNTop *  iNonShad;

				pMat->Shading.u.WriteBack.ModControl = pTri->TSPControlWord;
				pMat->Shading.u.WriteBack.ModBaseCol = 0;
				pMat->Shading.u.WriteBack.Smooth1 = 
					0x4000 | (ShadRedNBlue  & 0x7C000000) |
					(ShadGreenNTop & 0x03E00000) |
					((ShadRedNBlue  & 0xF800) << (20 - 14));
				pMat->Shading.u.WriteBack.Smooth2 = 0;
				pMat->v.LightVolCol = 0x4000 | (RedNBlue  & 0x7C000000)
											 | (GreenNTop & 0x03E00000)
											 |((RedNBlue  & 0xF800) << (20 - 14));
				pMat->Shading.u.Highlight = 0;
				pTri ++;
				pMat ++;
				continue;
			}

			fBase = fIntensity[2];
			
			ASSERT(fBase >= 0.0f);
			
			/* adjust for offscreen rep point if necessary */
			nX = pMat->Shading.u.Smooth.nX;
			nY = pMat->Shading.u.Smooth.nY;

			/*
			// If we need to clip the shading rep point
			// (See previous routine for an explanation of the logic)
			*/
			if( (nX | nY) & (~1023) )
			{
				SmoothShadingClipper(&nX, &nY, & fBase, fT1, fT2);

				if(fBase < 0.0f)
				{
					fBase = 0.0f;
				}
				else if(fBase > 1.0f)
				{
					fBase = 1.0f;
				}
			}/* end if clipping necessary */

			pMat->Shading.u.WriteBack.ModControl = pTri->TSPControlWord;
			pMat->Shading.u.WriteBack.ModBaseCol = nY | (nX << 16);

			/*
			// Compute and save the bit which can be in the shadow.
			*/
			pMat->Shading.u.WriteBack.Smooth1 = 
				((sgl_int32) (fBase * fShadowCoeff0x4000)) | Col16;
			pMat->Shading.u.WriteBack.Smooth2 =
				(((sgl_int32) (fT2  * fShadowCoeff0x10000)) & 0xFFFF) | 
				(((sgl_int32) (fT1  * fShadowCoeff0x10000)) << 16);
			/*
			// And save the bit which is added if you are outside of the shadow.
			*/
			pMat->v.LightVolCol = ((sgl_int32) (fBase * fNonShadowCoeff0x4000)) | Col16;
			pMat->Shading.u.Highlight = (((sgl_int32) (fT2  * fNonShadowCoeff0x10000)) & 0xFFFF) | 
							   (((sgl_int32) (fT1  * fNonShadowCoeff0x10000)) << 16);
		}/*end if*/
		
		pTri ++;
		pMat ++;

	}/*end while*/

	#undef INV

	SGL_TIME_STOP(SMOOTH_TRI_CLIP_PARAM_TIME)
}

/**********************************************************************/
/*                   Light Volume Version of Above                    */
/**********************************************************************/

void PackSmoothLiVolAndStore ( const ITRI *pTri, 
							   IMATERIAL *pMat, 
							   int nPolys)
{
	float 		fIntensity[3], fT1, fT2;
	int			nX, nY;
	sgl_uint32		nFlat1Col16;
		
	SGL_TIME_START(SMOOTH_TRI_CLIP_PARAM_TIME)
	
	nFlat1Col16 = (((pMat->v.LightVolCol & 0x00F80000) << 7)   |
				   ((pMat->v.LightVolCol & 0x0000F800) << 10)  |
				   ((pMat->v.LightVolCol & 0x000000F8) << 13));

	#define INV(x,y)	pTri->fAdjoint[x][y]

	while (nPolys--)
	{
		if (pTri->f1OverDet > DOWNGRADE_TO_FLAT)
		{
			/* if it's a small triangle, flat shade */

			sgl_uint32  Av555Colour;

			/*
			// Compute the average of the colours and put the result in
			// the top bits
			*/
			AVERAGE_RBG_TO_555_TOP(Av555Colour, pMat->Shading.u.Smooth.Col0,
												pMat->Shading.u.Smooth.Col1,
												pMat->Shading.u.Smooth.Col2);

			pMat->Shading.u.WriteBack.ModControl = pTri->TSPControlWord;
			pMat->Shading.u.WriteBack.ModBaseCol = 0;

			pMat->Shading.u.WriteBack.Smooth1 =  0x4000 | Av555Colour;
			pMat->Shading.u.WriteBack.Smooth2 = 0;
			
			pMat->v.LightVolCol = 0x4000 | nFlat1Col16;
			pMat->Shading.u.Highlight = 0;
		}
		else
		{
			float fBase;
			sgl_uint32 Col16;

			Col16 = PackTSPSmoothGetI (pMat, pTri->f1OverDet, fIntensity);
					
			/* pre-multiply intensities by inverse of face matrix */
			fT2 = INV(0,0) * fIntensity[0] + INV(0,1) * fIntensity[1];
			fT1 = INV(1,0) * fIntensity[0] + INV(1,1) * fIntensity[1];

			if ((sfabs(fT1) > INTENSITY_OVERFLOW) || (sfabs(fT2) > INTENSITY_OVERFLOW))
			{
				/* If it's going to overflow (and sparkle), flat shade */
				sgl_uint32  Av555Colour;

				AVERAGE_RBG_TO_555_TOP(Av555Colour, pMat->Shading.u.Smooth.Col0,
													pMat->Shading.u.Smooth.Col1,
													pMat->Shading.u.Smooth.Col2);

				pMat->Shading.u.WriteBack.ModControl = pTri->TSPControlWord;
				pMat->Shading.u.WriteBack.ModBaseCol = 0;

				pMat->Shading.u.WriteBack.Smooth1 =  0x4000 | Av555Colour;
				pMat->Shading.u.WriteBack.Smooth2 = 0;
			
				pMat->v.LightVolCol = 0x4000 | nFlat1Col16;
				pMat->Shading.u.Highlight = 0;
							
				pTri ++;
				pMat ++;
				continue;
			}

			fBase = fIntensity[2];

			ASSERT(fBase >= 0.0f);
			
			/* adjust for offscreen rep point if necessary */
			nX = pMat->Shading.u.Smooth.nX;
			nY = pMat->Shading.u.Smooth.nY;

			/*
			// If we need to clip the shading rep point
			// (See previous routine for an explanation of the logic)
			*/
			if( (nX | nY) & (~1023) )
			{
				SmoothShadingClipper(&nX, &nY, & fBase, fT1, fT2);

				if(fBase < 0.0f)
				{
					fBase = 0.0f;
				}
				else if(fBase > 1.0f)
				{
					fBase = 1.0f;
				}
			}/* end if clipping necessary */

			pMat->Shading.u.WriteBack.ModControl = pTri->TSPControlWord;
			pMat->Shading.u.WriteBack.ModBaseCol = nY | (nX << 16);
			{
				sgl_uint32		u32Intensity[2];

				ConvertIntensitiesToFixedPoint(fBase, fT1, fT2, u32Intensity);

				pMat->Shading.u.WriteBack.Smooth1 = 
					u32Intensity[0] | (Col16 << 16);
				pMat->Shading.u.WriteBack.Smooth2 = u32Intensity[1];

				pMat->v.LightVolCol = u32Intensity[0] | nFlat1Col16;
				pMat->Shading.u.Highlight = u32Intensity[1];
			}

		}
			
		pTri ++;
		pMat ++;
	}

	#undef INV

	SGL_TIME_STOP(SMOOTH_TRI_CLIP_PARAM_TIME)
}

/**********************************************************************/

void PackTSPSmoothLiVol ( const ITRI *pTri, 
					 const IMATERIAL *pMat, 
								 int nPolys, 
							  sgl_uint32 TSPIncrement, 
							  sgl_uint32 TSPGap,
							  sgl_uint32 *pTSP)
{
	float 		fIntensity[3], fT1, fT2;
	int			nX, nY;
	sgl_uint32	nFlat1Col16;
		
	SGL_TIME_START(SMOOTH_TRI_CLIP_PARAM_TIME)
	
	nFlat1Col16 = (((pMat->v.LightVolCol & 0x00F80000) << 7)   |
				   ((pMat->v.LightVolCol & 0x0000F800) << 10)  |
				   ((pMat->v.LightVolCol & 0x000000F8) << 13));

	#define INV(x,y)	pTri->fAdjoint[x][y]

	while (nPolys--)
	{
		if (pTri->f1OverDet > DOWNGRADE_TO_FLAT)
		{
			/* if it's a small triangle, flat shade */

			sgl_uint32  Av555Colour;

			/*
			// Compute the average of the colours and put the result in
			// the top bits
			*/
			AVERAGE_RBG_TO_555_TOP(Av555Colour, pMat->Shading.u.Smooth.Col0,
												pMat->Shading.u.Smooth.Col1,
												pMat->Shading.u.Smooth.Col2);

			IW( pTSP, 0, (pTri->TSPControlWord));
			IW( pTSP, 1, 0);
			IW( pTSP, TSPGap + 2, (0x4000 | Av555Colour));
			IW( pTSP, TSPGap + 3, 0);
			
			IW( pTSP, TSPGap + 4, (0x4000 | nFlat1Col16));
			IW( pTSP, TSPGap + 5, 0);
		}
		else
		{
			float 		fBase;
			sgl_uint32 	Col16;

			Col16 = PackTSPSmoothGetI (pMat, pTri->f1OverDet, fIntensity);
					
			/* pre-multiply intensities by inverse of face matrix */
			fT2 = INV(0,0) * fIntensity[0] + INV(0,1) * fIntensity[1];
			fT1 = INV(1,0) * fIntensity[0] + INV(1,1) * fIntensity[1];

			if ((sfabs(fT1) > INTENSITY_OVERFLOW) || (sfabs(fT2) > INTENSITY_OVERFLOW))
			{
				/* If it's going to overflow (and sparkle), flat shade */
				sgl_uint32  Av555Colour;

				AVERAGE_RBG_TO_555_TOP(Av555Colour, pMat->Shading.u.Smooth.Col0,
													pMat->Shading.u.Smooth.Col1,
													pMat->Shading.u.Smooth.Col2);

				IW( pTSP, 0, (pTri->TSPControlWord));
				IW( pTSP, 1, 0);
				IW( pTSP, TSPGap + 2, (0x4000 | Av555Colour));
				IW( pTSP, TSPGap + 3, 0);

				IW( pTSP, TSPGap + 4, (0x4000 | nFlat1Col16));
				IW( pTSP, TSPGap + 5, 0);

				pTri ++;
				pMat ++;
				pTSP += TSPIncrement;
				continue;
			}
			
			fBase = fIntensity[2];

			ASSERT(fBase >= 0.0f);
			
			/* adjust for offscreen rep point if necessary */
			nX = pMat->Shading.u.Smooth.nX;
			nY = pMat->Shading.u.Smooth.nY;

			/*
			// If we need to clip the shading rep point
			// (See previous routine for an explanation of the logic)
			*/
			if( (nX | nY) & (~1023) )
			{
				SmoothShadingClipper(&nX, &nY, & fBase, fT1, fT2);

				if(fBase < 0.0f)
				{
					fBase = 0.0f;
				}
				else if(fBase > 1.0f)
				{
					fBase = 1.0f;
				}
			}/* end if clipping necessary */

			IW( pTSP, 0, (pTri->TSPControlWord));
			IW( pTSP, 1, (nY | (nX << 16)));
			{
				sgl_uint32		u32Intensity[2];

				ConvertIntensitiesToFixedPoint(fBase, fT1, fT2, u32Intensity);

				IW( pTSP, TSPGap + 2, (u32Intensity[0] | (Col16 << 16)));
				IW( pTSP, TSPGap + 3, u32Intensity[1]);

				IW( pTSP, TSPGap + 4, (u32Intensity[0] | nFlat1Col16));
				IW( pTSP, TSPGap + 5, u32Intensity[1]);
			}

		}
			
		pTri ++;
		pMat ++;
		pTSP += TSPIncrement;
	}

	#undef INV

	SGL_TIME_STOP(SMOOTH_TRI_CLIP_PARAM_TIME)
}

/* end of $RCSfile: dshade.c,v $ */



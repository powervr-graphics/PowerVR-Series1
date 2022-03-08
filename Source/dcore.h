/******************************************************************************
 * Name         : dcore.h
 * Title        : Triangle process routines for D3D and SGLDirect
 * Author       : Edward Forde
 * Created      : 10/9/1997
 *
 * Copyright	: 1995-2022 Imagination Technologies (c)
 * License		: MIT
 *
 * Description  : Holds triangle process routines for D3D and SGLDirect.
 *
 * Platform     : ANSI
 *
 * Modifications:
 * $Log: dcore.h,v $
 * Revision 1.12  1997/11/26  14:54:40  sxy
 * added DPFDEV messages.
 *
 * Revision 1.11  1997/11/18  12:24:58  sxy
 * added DPFDEV messages.
 *
 * Revision 1.10  1997/10/30  17:08:40  sxy
 * when have no sort, put all trans faces into the front list.
 *
 * Revision 1.9  1997/10/16  13:16:38  sxy
 * removed DPFDEVFs, its all in DPFDEV.
 *
 * Revision 1.8  1997/10/15  15:40:24  sxy
 * added DPFDEVF messages.
 *
 * Revision 1.7  1997/10/13  11:08:19  erf
 * Added new parameter to the ProcessTriCoreLite() routine to simplify
 * D3D.
 *
 * Revision 1.6  1997/09/25  16:23:21  sxy
 * added some DPFDEV messages.
 *
 * Revision 1.5  1997/09/16  09:36:33  sxy
 * Added ASSERT to check region box if not do clipping.
 *
 * Revision 1.4  1997/09/15  16:58:36  sxy
 * 1. Reversed 'Force to do clipping'.
 * 2. When RegMax > RegMin, added 0.5 (- fAddToXY) to RegMin instead of 1.
 *
 * Revision 1.3  1997/09/15  08:58:18  sxy
 * Force to do clipping anyway.
 *
 * Revision 1.2  1997/09/12  16:02:44  sxy
 * swap rX0 and rX1 (rY0 and ry1) when they are in the wrong order
 * regardless if we do cliping or not.
 *
 * Revision 1.1  1997/09/10  13:52:15  erf
 * Initial revision
 *
 *
 *****************************************************************************/
 
 extern int gNoSortTransFaces;

/*************************************************************************
 *
 *	SPECIFIC D3D AND SGL-Direct PROCESS ROUTINES.
 *
 *	Includes:	ProcessTriCoreLite
 *				ProcessQuadCore
 *				ProcessD3DStripCore
 *				ProcessD3DStripTransCore
 *
 ************************************************************************/

/**************************************************************************
 * Function Name  : ProcessTriCoreLite
 * Inputs         : PPIR pAddfn, sgl_uint32 NewObject, sgl_int32 nNextTriInc 
 * Outputs        : None
 * Returns        : Number of triangles processed.
 * Global Used    : gPDC, gpPDC
 * Description    : This processes both D3D and SGL-Lite triangles. It
 *					also processes translucent SGL_lite triangles. This
 *					routine is in assembler, for optimisation, in the file
 *					dtriml.asm. This version is used if PROCESSTRICORELITE
 *					is defined as zero in dtri.h.
 *
 *					When called for D3D triangles then NewObject = 0.
 *
 *					nNextTriInc never used but included to help out.
 *	
 **************************************************************************/

#if !PROCESSTRICORELITE
extern int _ProcessTriCoreLite ( PPIR pAddfn, sgl_uint32 NewObject, sgl_int32 nNextTriInc);
#define ProcessTriCoreLite _ProcessTriCoreLite
#else
		   
extern int ProcessTriCoreLite ( PPIR pAddfn, sgl_uint32 NewObject, sgl_int32 nNextTriInc)
{
	PITRI	pTri = gpTri;
	int		ShiftRegX = gpPDC->ShiftRegX;
	sgl_int32	rX0, rY0, rX1, rY1;
	sgl_uint32	uDefaultType, uCurrentType, uOrBackType;
	static	sgl_uint32 uDoneFirst[2];

	#if CHUNKING

		static sgl_uint32 uCurrentIndices[3];
		static sgl_uint32 uPreviousIndices[2][3];
		sgl_uint32 *puPrev;

	#endif

	if (gpPDC->TSPControlWord & MASK_TRANS)
	{
		if (gPDC.Context.u32Flags & SGLTT_OPAQUE)
		{
			uDefaultType = PACKED_TYPE_OPAQUETRANS;
			uCurrentType = PACKED_TYPE_OPAQUETRANS;
			uOrBackType = PACKED_TYPE_OPAQUETRANS;
		}
		else if (gPDC.Context.u32Flags & SGLTT_NEWPASSPERTRI)
		{
			uDefaultType = PACKED_TYPE_TRANSLUCENT | PACKED_TRANSTYPE_SETMARK;
			uCurrentType = PACKED_TYPE_TRANSLUCENT | PACKED_TRANSTYPE_SETMARK;
			uOrBackType = 0;
		}
		/* if no sort, put all trans face in the front list */
		else if (gNoSortTransFaces != MINZ_SORT)
		{
			if (NewObject)
			{
				uDoneFirst[0] = FALSE;
				uDoneFirst[1] = FALSE;
			}

			uDefaultType = PACKED_TYPE_TRANSLUCENT;
			uCurrentType = PACKED_TYPE_TRANSLUCENT;
			uOrBackType = 0;
		}
		else
		{
			if (NewObject)
			{
				uDoneFirst[0] = FALSE;
				uDoneFirst[1] = FALSE;
			}
			
			uDefaultType = PACKED_TYPE_TRANSLUCENT;
			uCurrentType = PACKED_TYPE_TRANSLUCENT;
			uOrBackType = PACKED_TRANSTYPE_BACKBIT;
		}
	}
	else
	{
		uDefaultType = PACKED_TYPE_OPAQUE;
		uCurrentType = PACKED_TYPE_OPAQUE;
		uOrBackType = 0;
	}

	while ( gpPDC->nInputTriangles )
	{
		/*
		// "Temp" variables for holding the calculation of the 
		// adjoint matrix, and the determinant.
		*/
		float AdjX0, AdjX1, AdjX2;
		float fDet;

		/*
		// The boundaries of the triangle: These are loaded as float for
		// normal processors but "hacked" as integer for Pentium.
		*/
		#if (SLOW_FCMP && !MULTI_FP_REG)
			sgl_int32 RegMax, RegMin, RegSwap;
		#else
			float RegMax, RegMin, RegSwap;
		#endif

		/*
		// To help convince the compiler that vertex values won't keep magically
		// changing, get local pointers to the vertices and make the contents
		// constant.
		*/
		const SGLVERTEX *pV0, *pV1, *pV2;

		/*
		// For the positions of the vertices, if we have plenty of FP registers, 
		// "load" them, else just re-access them via the pointers.
		*/
		#if MULTI_FP_REG || MODIFYXY
			float fX0, fX1, fX2, fY0, fY1, fY2;
		#else
			#define fX0 (pV0->fX)
			#define fX1 (pV1->fX)
			#define fX2 (pV2->fX)

			#define fY0 (pV0->fY)
			#define fY1 (pV1->fY)
			#define fY2 (pV2->fY)
		#endif

		int *pnFaces;
		
		/* Decrement nInputTriangles to zero not -1 */

		gpPDC->nInputTriangles--;
		
		pnFaces = (int *) gpPDC->pFace;

		if (gPDC.Context.u32Flags & SGLTT_FACESIND3DFORMAT)
		{
			pV0 = gPDC.pVertices + (pnFaces[0] & 0xFFFF);
			pV1 = gPDC.pVertices + (pnFaces[0] >> 16);
			pV2 = gPDC.pVertices + (pnFaces[1] & 0xFFFF);

			/*
			// Ok save the vertex pointers. Since we made these const pointers
	   		// we have to do a little jiggery pokery with casting
			*/
			gpPDC->pV0 = (PSGLVERTEX) pV0;
			gpPDC->pV1 = (PSGLVERTEX) pV1;
			gpPDC->pV2 = (PSGLVERTEX) pV2;

			*((sgl_uint32 *) &gpPDC->pFace) += sizeof (SGLD3DFACE);
		}
		else
		{
			pV0 = gPDC.pVertices + pnFaces[0];
			pV1 = gPDC.pVertices + pnFaces[1];
			pV2 = gPDC.pVertices + pnFaces[2];

			/*
			// Ok save the vertex pointers. Since we made these const pointers
	   		// we have to do a little jiggery pokery with casting
			*/
			gpPDC->pV0 = (PSGLVERTEX) pV0;
			gpPDC->pV1 = (PSGLVERTEX) pV1;
			gpPDC->pV2 = (PSGLVERTEX) pV2;

			#if CHUNKING
		
				uCurrentIndices[0] = pnFaces[0];
				uCurrentIndices[1] = pnFaces[1];
				uCurrentIndices[2] = pnFaces[2];

			#endif

			gpPDC->pFace = (void *) (pnFaces + 3);
		}

		#if DEBUG || DEBUGDEV

			if ((gpPDC->pV0 == gpPDC->pV1) || (gpPDC->pV1 == gpPDC->pV2) || (gpPDC->pV0 == gpPDC->pV2))
			{
				DPFDEV((DBG_MESSAGE,"ProcessTriCoreLite: Triangle with identical vertices"));	
			}

		#endif
	
		/*
		// Load the vertex values into local variables.
		*/
		#if MULTI_FP_REG || MODIFYXY
			#if MODIFYXY
				fX0 = pV0->fX + fAddToXY;
				fX1 = pV1->fX + fAddToXY;
				fX2 = pV2->fX + fAddToXY;

				fY0 = pV0->fY + fAddToXY;
				fY1 = pV1->fY + fAddToXY;
				fY2 = pV2->fY + fAddToXY;
			#else
				fX0 = pV0->fX;
				fX1 = pV1->fX;
				fX2 = pV2->fX;

				fY0 = pV0->fY;
				fY1 = pV1->fY;
				fY2 = pV2->fY;
			#endif
		#endif

		/*
		// Copy over constant parts for each face, BEFORE we change what
		// pV0 points to in the back face flipping AND
		//
		// Just work out the bit of the Adjoint we need for the determinant.
		// i.e. Row 2
		//
		// I'm sorry about mixing these two lots of code together, but it
		// was the only way to get the  intel compiler to do some overlap
		// of floating point multiplies and integer..(SJF)
		*/
		AdjX0 = fX1*fY2 - fX2*fY1;
		pTri->TSPControlWord = 	gPDC.TSPControlWord;

		AdjX1 = fX2*fY0 - fX0*fY2;
		pTri->BaseColour = 		pV0->u32Colour;

		AdjX2 = fX0*fY1 - fX1*fY0;
				
		/* calculate determinant and see if we can cull this plane */
		fDet = AdjX0 + AdjX1 + AdjX2;

		/*
		// Look at the sign of the determinant to determine if we have
		// a reverse face and whether we should flip or cull the face.
		*/
		/* fDet < LARGE_EPSILON */
		if(LESS_THAN_LARGE_EPSILON(fDet))
		{
			/* fDet > EPSILON  checking is the triangle covering a pixel? */
		
			if(!(LESS_THAN_EPSILON(fDet)) )
			{
	  			/* the triangle does not cover a pixel centre, cull it. */
	 			if(((int)fX0 == (int)fX1 && (int)fX0 == (int)fX2) ||
	 	  		 ((int)fY0 == (int)fY1 && (int)fY0 == (int)fY2))
	 			{		
#if DEBUGDEV
					DPFDEV((DBG_MESSAGE,"ProcessTriCoreLite: culling sub-pixel face"));	
#endif					
					continue;
	   			}
			}
			/* fDet < EPSILON */
			else
			{	
				PSGLVERTEX pV;
				const SGLVERTEX *cpV;
				float	   f;

				/*
				// Ok if we are back face culling skip to the next one
				*/
				if (gpPDC->Context.bCullBackfacing)
				{
#if DEBUGDEV
					DPFDEV((DBG_MESSAGE,"ProcessTriCoreLite: culling back face"));	
#endif					
					continue;
				}

				/*
				// Negate fdet while we probably have it loaded.
				// Integer compares of floating point values work
				// a bit funny, so its easier to make it positive.
				*/
				FP_NEGATE(fDet);

				/*
				// if the face is ridiculously tiny, skip to the next one
				*/
				/* fDet < LARGE_EPSILON */
		   		if(LESS_THAN_LARGE_EPSILON(fDet))
				{
					/* fDet > EPSILON  checking is the triangle covering a pixel? */
					if(!(LESS_THAN_EPSILON(fDet)) )
					{
					  	/* the triangle does not cover a pixel centre, cull it. */
					 	if(((int)fX0 == (int)fX1 && (int)fX0 == (int)fX2) ||
					 	   ((int)fY0 == (int)fY1 && (int)fY0 == (int)fY2))
					 	{	
#if DEBUGDEV
							DPFDEV((DBG_MESSAGE,"ProcessTriCoreLite: culling sub-pixel face"));	
#endif							
							continue;
						}
					}
					else
					{				
#if DEBUGDEV
						DPFDEV((DBG_MESSAGE,"ProcessTriCoreLite: culling tiny face"));	
#endif						
						continue;
					}
				}
				/*
				// Otherwise flip the face around
				*/
	 			/*
				// Swap the pointers to two of the vertices in both
				// the global structure and our local copy.
				*/							
				pV = gpPDC->pV2;
				gpPDC->pV2 = gpPDC->pV0;
				gpPDC->pV0 = pV;
				
				cpV = pV2;
				pV2 = pV0;
				pV0 = cpV;
				/*
				// Swap the XY values we previously read from
				// verts 0 & 2 which have now been swapped. NOTE
				// ONLY do this when we actually have real local
				// variables and NOT just aliases!!
				*/
				#if MULTI_FP_REG || MODIFYXY
					f	= fX0;
					fX0 = fX2;
					fX2 = f;

					f	= fY0;
					fY0 = fY2;
					fY2 = f;
				#endif

				/*
				// Negate and swap the various bits of Row 2 of the adjoint
				*/
				AdjX1 = -AdjX1;

				f	  = -AdjX0;
				AdjX0 = -AdjX2;
				AdjX2 =  f;

				uCurrentType |= uOrBackType;
	
	   		}/*end else if(!(LESS_THAN_EPSILON(fDet)) )*/
	   
		}/* end of if(LESS_THAN_LARGE_EPSILON(fDet))*/

		/*
		// Save the bits of the adjoint we currently haven't yet saved.
		*/
		pTri->fAdjoint[2][0] = AdjX0;
		pTri->fAdjoint[2][1] = AdjX1;
		pTri->fAdjoint[2][2] = AdjX2;

		/*
		// Do the rest of the adjoint. Use temp variables so the compiler
		// knows it CAN overlap operations
		*/
		AdjX0 = fY1-fY2;
		AdjX1 = fY2-fY0;
		AdjX2 = fY0-fY1;

		pTri->fAdjoint[0][0] = AdjX0;
		pTri->fAdjoint[0][1] = AdjX1;
		pTri->fAdjoint[0][2] = AdjX2;

		AdjX0 = fX2-fX1;
		AdjX1 = fX0-fX2;
		AdjX2 = fX1-fX0;

		pTri->fAdjoint[1][0] = AdjX0;
		pTri->fAdjoint[1][1] = AdjX1;
		pTri->fAdjoint[1][2] = AdjX2;

		if (gpPDC->Context.u32Flags & SGLTT_DISABLEZBUFFER)
		{
		    /*
			// Note that BogusZ is an arbitrary mem location
			// so we have to convince the compiler it can't overlap
			// where pTri points to
			*/
			float BogusZIncremented = gfBogusInvZ+ (float) BOGUSINVZ_INCREMENT;

			pTri->fZ[0] = pTri->fZ[1] = pTri->fZ[2] = gfBogusInvZ;

			gfBogusInvZ = BogusZIncremented;

		}
		else
		{
			/* Use temp variables to allow FP overlap*/
			AdjX0 = gpPDC->pV0->fInvW * fMinInvZ;
			AdjX1 = gpPDC->pV1->fInvW * fMinInvZ;
			AdjX2 = gpPDC->pV2->fInvW * fMinInvZ;

			pTri->fZ[0] = AdjX0;
			pTri->fZ[1] = AdjX1;
			pTri->fZ[2] = AdjX2;
		}

#if DEBUG || DEBUGDEV

		if (gpPDC->pV0->fInvW > 10.0f || gpPDC->pV1->fInvW > 10.0f || gpPDC->pV2->fInvW > 10.0f)
		{
			DPFDEV ((DBG_WARNING, "ProcessTriCoreLite: fInvW out of range"));
		}
#endif

		
		/* 
		// get region bounding box. Do X extremities first. 
		// Note. We don't worry about negative values since a negative
		// value would screw up the region code anyway (unless the
		// clipping is on, in which case it would get fixed)
		*/
		RegMax = F_TO_I_IF_FP_SLOW(fX1);
		RegMin = F_TO_I_IF_FP_SLOW(fX0);

		if(RegMax < RegMin)
		{
			RegSwap	= RegMax;
			RegMax	= RegMin;
			RegMin	= RegSwap;
		}
		RegSwap = F_TO_I_IF_FP_SLOW(fX2);

		if(RegMax < RegSwap)
		{
			RegMax = RegSwap;
		}
		else if(RegSwap < RegMin)
		{
			RegMin = RegSwap;
		}

		/*
		// Convert the X max and mins to integer region equivalent.
		//
		// NOTE on the Pentium, we'd like RegMin and Max to remain in
		// registers. However, if you take the pointer of a variable,
		// the compiler automatically puts it in memory. Avoid this
		// by using another couple of temporary variables!
		*/
		#if (SLOW_FCMP && !MULTI_FP_REG)
		{
			sgl_int32 RegMaxInMem = RegMax;
			sgl_int32 RegMinInMem = RegMin;
			
			/* Region calculation for X0+1, Y0+1, X1, Y1.
			 */
			if (RegMax > RegMin)
			   rX0 = ((sgl_int32) (LONG_TO_FLOAT(RegMinInMem) - fAddToXY)) >> ShiftRegX;
			else
			   rX0 = ((sgl_int32) LONG_TO_FLOAT(RegMinInMem)) >> ShiftRegX;

			rX1 = ((sgl_int32) LONG_TO_FLOAT(RegMaxInMem)) >> ShiftRegX;
		}
		#else
			/* Region calculation for X0+1, Y0+1, X1, Y1.
			 */
			if (RegMax > RegMin)
				rX0 = ((sgl_int32) (RegMin - fAddToXY)) >> ShiftRegX;
			else
				rX0 = ((sgl_int32) RegMin) >> ShiftRegX;
			rX1 = ((sgl_int32) RegMax) >> ShiftRegX;
		#endif

		/* 
		// Do Y
		*/
		RegMax = F_TO_I_IF_FP_SLOW(fY1);
		RegMin = F_TO_I_IF_FP_SLOW(fY0);

		if(RegMax < RegMin)
		{
			RegSwap	= RegMax;
			RegMax	= RegMin;
			RegMin	= RegSwap;
		}
		RegSwap = F_TO_I_IF_FP_SLOW(fY2);

		if(RegMax < RegSwap)
		{
			RegMax = RegSwap;
		}
		else if(RegSwap < RegMin)
		{
			RegMin = RegSwap;
		}

		/*
		// Convert the "floats" to region equivalent.
		*/
		#if (SLOW_FCMP && !MULTI_FP_REG)
		{
			sgl_int32 RegMaxInMem = RegMax;
			sgl_int32 RegMinInMem = RegMin;

			/* Region calculation for X0+1, Y0+1, X1, Y1.
			 */
			if (RegMax > RegMin)
			   rY0 = (sgl_int32) (LONG_TO_FLOAT(RegMinInMem) - fAddToXY);
			else
			   rY0 = (sgl_int32) LONG_TO_FLOAT(RegMinInMem);

			rY1 = (sgl_int32) LONG_TO_FLOAT(RegMaxInMem);
		}
		#else
			/* Region calculation for X0+1, Y0+1, X1, Y1.
			 */
			if (RegMax > RegMin)
			   rY0 = (sgl_int32) (RegMin - fAddToXY);
			else
			   rY0 = (sgl_int32) RegMin;
			rY1 = (sgl_int32) RegMax;
		#endif

		/*
		// If we used floating point compares BUT all of the 
		// vert values were NEGATIVE, then the Max and Min
		// will be back-the-front. 
		*/
		#if (SLOW_FCMP && !MULTI_FP_REG)
			if(rX0 > rX1)
			{
				sgl_int32 swap;
				swap = rX0;
				rX0	 = rX1;
				rX1	 = swap;
			}

			if(rY0 > rY1)
			{
				sgl_int32 swap;
				swap = rY0;
				rY0	 = rY1;
				rY1	 = swap;
			}
		#endif

#if DEBUGDEV
		
		/* Check for Region clipping when bDoClipping is FALSE */
		if (!(gpPDC->Context.bDoClipping))
		{
			if( (rX0 < gpPDC->Context.FirstXRegion) ||
				(rY0 < gpPDC->Context.FirstYRegion) ||
				(rX1 > gpPDC->Context.LastXRegion)  ||
				(rY1 > gpPDC->Context.LastYRegion))
			{
			 	DPFDEV ((DBG_FATAL, "ProcessTriCoreLite: need clipping but bDoClipping is FALSE"));				
			}
		}
#endif
	
		/*
		// Check for Region clipping
		*/
	   	if (gpPDC->Context.bDoClipping)
		{
			if (rX0 < gpPDC->Context.FirstXRegion)
			{				
				if ( rX1 < gpPDC->Context.FirstXRegion )
				{
					/* Very Unlikely */
					continue;
				}
				
				rX0 = gpPDC->Context.FirstXRegion;
			}
		
			if (rY0 < gpPDC->Context.FirstYRegion)
			{			
				if ( rY1 < gpPDC->Context.FirstYRegion )
				{
					/* Very Unlikely */
					continue;
				}

				rY0 = gpPDC->Context.FirstYRegion;
			}
		
			if (rX1 > gpPDC->Context.LastXRegion)
			{		
				if ( rX0 > gpPDC->Context.LastXRegion )
				{
					/* Very Unlikely */
					continue;
				}

				rX1 = gpPDC->Context.LastXRegion;
			}
		
			if (rY1 > gpPDC->Context.LastYRegion)
			{				
				if ( rY0 > gpPDC->Context.LastYRegion )
				{
					/* Very Unlikely */
					continue;
				}

				rY1 = gpPDC->Context.LastYRegion;
			}
		}
		else
		{
			/* ASSERT the regions to avoid crashing later in dregion.c */
			ASSERT(rX0 >= gpPDC->Context.FirstXRegion);
			ASSERT(rY0 >= gpPDC->Context.FirstYRegion);
			ASSERT(rX1 <= gpPDC->Context.LastXRegion);
			ASSERT(rY1 <= gpPDC->Context.LastYRegion);
		}

		if (gpPDC->TSPControlWord & MASK_TRANS)
		{
			sgl_uint32 uDFIndex = (uCurrentType & 1);

			#if CHUNKING
				puPrev = &uPreviousIndices[uDFIndex];
			#endif

			if (!uDoneFirst[uDFIndex])
			{
				uCurrentType |= PACKED_TRANSTYPE_SETMARK;
				uDoneFirst[uDFIndex] = TRUE;
			}

			#if CHUNKING

				else 
				{				
					sgl_int32 nXORResult;

					nXORResult = (puPrev[0] == uCurrentIndices[0])
									+ (puPrev[0] == uCurrentIndices[1])
									+ (puPrev[0] == uCurrentIndices[2])
									+ (puPrev[1] == uCurrentIndices[0])
									+ (puPrev[1] == uCurrentIndices[1])
									+ (puPrev[1] == uCurrentIndices[2])
									+ (puPrev[2] == uCurrentIndices[0])
									+ (puPrev[2] == uCurrentIndices[1])
									+ (puPrev[2] == uCurrentIndices[2]);

					nXORResult --;
					nXORResult &= PACKED_TRANSTYPE_SETMARK;

					uCurrentType |= nXORResult;
				}

				puPrev[0] = uCurrentIndices[0];
				puPrev[1] = uCurrentIndices[1];
				puPrev[2] = uCurrentIndices[2];

			#endif
		}
		
		/*
		// Get the inverse determinant at a point where we can overlap
		// the FP with loads of integer ops... ie good for Pentium
		*/
		pTri->f1OverDet = 1.0f / fDet;
		pTri->reg.u = ENCODE_OBJXYDATA( uCurrentType, rX0, rY0, rX1, rY1 );

		uCurrentType = uDefaultType;
		
		if ( pAddfn != NULL )
		{
			/* Call the type specific thing here */
			pAddfn( pTri );
		}
						
		if ( ProcessFlatTexFn != NULL )
		{
			/* Texture stuff in line */
			ProcessFlatTexFn( pTri );
		}
			
		/* Next material structure */
		gpMatCurrent++;
						
		if ( ++pTri == &gpTri[IBUFFERSIZE] )
		{
			/* Filled up the buffer! */
			break;
		}

		#if !MULTI_FP_REG && !MODIFYXY
			#undef fX0
			#undef fX1
			#undef fX2

			#undef fY0
			#undef fY1
			#undef fY2
		#endif
	}/*end while*/

	/* Return number in intermediate buffer this time */
	return( (int) ( pTri - gpTri ) );

} /* ProcessTriCoreLite */
#endif

/*
//===================================================
*/

static int ProcessQuadCore ( PPIR pAddfn, sgl_uint32 NewObject )
{
	PITRI pTri = gpTri;
	int	ShiftRegX = gpPDC->ShiftRegX;

	sgl_int32 rX0, rY0, rX1, rY1;

	sgl_uint32 uDefaultType, uCurrentType, uOrBackType;
	static sgl_uint32 uDoneFirst[2];

	PIMATERIAL	pMat = gpMat;
	PIEDGE		pEdge = gpEdge;
	int 		*pnFace;
	
	#if CHUNKING
		/* I don't think one of these variables is initialised properly. SJF */
		sgl_uint32 *puPrev;
		sgl_uint32 uCurrentIndices[4];
		static sgl_uint32 uPreviousIndices[2][4];

	#endif

	if (gpPDC->TSPControlWord & MASK_TRANS)
	{
		if (gPDC.Context.u32Flags & SGLTT_OPAQUE)
		{
			uDefaultType = PACKED_TYPE_OPAQUETRANS;
			uCurrentType = PACKED_TYPE_OPAQUETRANS;
			uOrBackType = PACKED_TYPE_OPAQUETRANS;
		}
		else if (gPDC.Context.u32Flags & SGLTT_NEWPASSPERTRI)
		{
			uDefaultType = PACKED_TYPE_TRANSLUCENT | PACKED_TRANSTYPE_SETMARK;
			uCurrentType = PACKED_TYPE_TRANSLUCENT | PACKED_TRANSTYPE_SETMARK;
			uOrBackType = 0;
		}
		/* if no sort, put all trans face in the front list */
		else if (gNoSortTransFaces != MINZ_SORT)
		{
			if (NewObject)
			{
				uDoneFirst[0] = FALSE;
				uDoneFirst[1] = FALSE;
			}

			uDefaultType = PACKED_TYPE_TRANSLUCENT;
			uCurrentType = PACKED_TYPE_TRANSLUCENT;
			uOrBackType = 0;
		}
		else
		{
			if (NewObject)
			{
				uDoneFirst[0] = FALSE;
				uDoneFirst[1] = FALSE;
			}
			
			uDefaultType = PACKED_TYPE_TRANSLUCENT;
			uCurrentType = PACKED_TYPE_TRANSLUCENT;
			uOrBackType = PACKED_TRANSTYPE_BACKBIT;
		}
	}
	else
	{
		uDefaultType = PACKED_TYPE_OPAQUE;
		uCurrentType = PACKED_TYPE_OPAQUE;
		uOrBackType = 0;
	}

	while ( gpPDC->nInputTriangles )
	{
		/*
		// "Temp" variables for holding the calculation of the 
		// adjoint matrix, and the determinant.
		*/
		float AdjX0, AdjX1, AdjX2;
		float fDet;

		/*
		// The boundaries of the triangle: These are loaded as float for
		// normal processors but "hacked" as integer for Pentium.
		*/
		#if (SLOW_FCMP && !MULTI_FP_REG)
			sgl_int32 RegMax, RegMin, RegSwap;
		#else
			float RegMax, RegMin, RegSwap;
		#endif

		/*
		// To help convince the compiler that vertex values won't keep magically
		// changing, get local pointers to the vertices and make the contents
		// constant.
		*/
		const SGLVERTEX *pV0, *pV1, *pV2, *pV3;

		/*
		// For the positions of the vertices, if we have plenty of FP registers, 
		// "load" them, else just re-access them via the pointers.
		*/
		#if MULTI_FP_REG || MODIFYXY
			float fX0,fX1,fX2,fX3, fY0,fY1,fY2, fY3 ;
		#else
			#define fX0 (pV0->fX)
			#define fX1 (pV1->fX)
			#define fX2 (pV2->fX)
			#define fX3 (pV3->fX)

			#define fY0 (pV0->fY)
			#define fY1 (pV1->fY)
			#define fY2 (pV2->fY)
			#define fY3 (pV3->fY)
		#endif

		/* Decrement nInputTriangles to zero not -1 */
		gpPDC->nInputTriangles--;
		
		pnFace = (int *) gpPDC->pFace;

		/*
		// Compute pointers to the various vertices
		*/
		pV0 = gPDC.pVertices + pnFace[0];
		pV1 = gPDC.pVertices + pnFace[1];
		pV2 = gPDC.pVertices + pnFace[2];
		pV3 = gPDC.pVertices + pnFace[3];

		/*
		// Save the three we need for the texturing etc. Note the
		// jiggery pokery to cope with the "const".
		*/
		gpPDC->pV0 = (PSGLVERTEX) pV0;
		gpPDC->pV1 = (PSGLVERTEX) pV1;
		gpPDC->pV2 = (PSGLVERTEX) pV2;
		
		gpPDC->pFace = (void *) (pnFace + 4);
		
#if DEBUGDEV

	if ((gpPDC->pV0 == gpPDC->pV1) || (gpPDC->pV1 == gpPDC->pV2) || (gpPDC->pV0 == gpPDC->pV2))
	{
		DPFDEV((DBG_MESSAGE,"ProcessQuadCore: Quad with identical vertices"));
	}

#endif

		ASSERT(gpPDC->pV0 != gpPDC->pV1 && gpPDC->pV1 != gpPDC->pV2);

		/*
		// Load the vertex values into local variables.
		*/
		#if MULTI_FP_REG
			fX0 = pV0->fX;
			fX1 = pV1->fX;
			fX2 = pV2->fX;
				  
			fY0 = pV0->fY;
			fY1 = pV1->fY;
			fY2 = pV2->fY;
		#elif MODIFYXY
			fX0 = pV0->fX + fAddToXY;
			fX1 = pV1->fX + fAddToXY;
			fX2 = pV2->fX + fAddToXY;
				  
			fY0 = pV0->fY + fAddToXY;
			fY1 = pV1->fY + fAddToXY;
			fY2 = pV2->fY + fAddToXY;
		#endif

		/*
		// Copy over constant parts for each face, BEFORE we change what
		// pV0 points to in the back face flipping AND
		//
		// Just work out the bit of the Adjoint we need for the determinant.
		// i.e. Row 2
		//
		// I'm sorry about mixing these two lots of code together, but it
		// was the only way to get the intel compiler to do some overlap
		// of floating point multiplies and integer   (SJF)
		*/
		AdjX0 = fX1*fY2 - fX2*fY1;
		pTri->TSPControlWord = 	gPDC.TSPControlWord;

		AdjX1 = fX2*fY0 - fX0*fY2;
		pTri->BaseColour = 		pV0->u32Colour;

		AdjX2 = fX0*fY1 - fX1*fY0;

		/* calculate determinant and see if we can cull this plane */
		fDet = AdjX0 + AdjX1 + AdjX2;

		/*
		// Look at the sign of the determinant to determine if we have
		// a reverse face and whether we should flip or cull the face.
		*/
		/* fDet < LARGE_EPSILON */
		if(LESS_THAN_LARGE_EPSILON(fDet))
		{
			/* fDet > EPSILON  checking is the triangle covering a pixel? */
			if(!(LESS_THAN_EPSILON(fDet)) )
			{
	  			/* the triangle does not cover a pixel centre, cull it. */
	 			if(((int)fX0 == (int)fX1 && (int)fX0 == (int)fX2) ||
	 	  		 ((int)fY0 == (int)fY1 && (int)fY0 == (int)fY2))
	 			{		
#if DEBUGDEV
					DPFDEV((DBG_MESSAGE,"ProcessQuadCore: culling sub-pixel face"));	
#endif					
					continue;
	   			}
			}
			/* fDet < EPSILON */
			else
			{	
				PSGLVERTEX pV;
				const SGLVERTEX *cpV;
				float	   f;
				/*
				// Ok if we are back face culling skip to the next one
				*/
				if (gpPDC->Context.bCullBackfacing)
				{
#if DEBUGDEV
					DPFDEV((DBG_MESSAGE,"ProcessQuadCore: culling back face"));	
#endif					
					continue;
				}
				/*
				// Negate fdet while we probably have it loaded.
				// Integer compares of floating point values work
				// a bit funny, so its easier to make it positive.
				*/
				FP_NEGATE(fDet);

				/*
				// if the face is ridiculously tiny, skip to the next one
				*/
		   		/* fDet < LARGE_EPSILON */
		   		if(LESS_THAN_LARGE_EPSILON(fDet))
				{
					/* fDet > EPSILON  checking is the triangle covering a pixel? */
					if(!(LESS_THAN_EPSILON(fDet)) )
					{
					  	/* the triangle does not cover a pixel centre, cull it. */
					 	if(((int)fX0 == (int)fX1 && (int)fX0 == (int)fX2) ||
					 	   ((int)fY0 == (int)fY1 && (int)fY0 == (int)fY2))
					 	{	
#if DEBUGDEV
							DPFDEV((DBG_MESSAGE,"ProcessQuadCore: culling sub-pixel face"));	
#endif							
							continue;
						}
					}
					else
					{				
#if DEBUGDEV
						DPFDEV((DBG_MESSAGE,"ProcessQuadCore: culling tiny face"));	
#endif						
						continue;
					}
				}
		  	 	/*
				// Otherwise flip the face around
				*/
			
				/*
				// Swap the pointers to two of the vertices in both
				// the global structure and our local copy.
				*/							
				pV = gpPDC->pV2;
				gpPDC->pV2 = gpPDC->pV0;
				gpPDC->pV0 = pV;

				cpV = pV2;
				pV2 = pV0;
				pV0 = cpV;

				/*
				// Swap the XY values we previously read from
				// verts 0 & 2 which have now been swapped. NOTE
				// ONLY do this when we actually have real local
				// variables and NOT just aliases!!
				*/
				#if MULTI_FP_REG || MODIFYXY
					f	= fX0;
					fX0 = fX2;
					fX2 = f;

					f	= fY0;
					fY0 = fY2;
					fY2 = f;
				#endif

				/*
				// Negate and swap the various bits of Row 2 of the adjoint
				*/
				AdjX1 = -AdjX1;

				f	  = -AdjX0;
				AdjX0 = -AdjX2;
				AdjX2 =  f;

				uCurrentType |= uOrBackType;
			}/*end else if(!(LESS_THAN_EPSILON(fDet)) )*/
	   
		}/* end of if(LESS_THAN_LARGE_EPSILON(fDet))*/

		/*
		// Save the bits of the adjoint we currently haven't yet saved.
		*/
		pTri->fAdjoint[2][0] = AdjX0;
		pTri->fAdjoint[2][1] = AdjX1;
		pTri->fAdjoint[2][2] = AdjX2;

		/*
		// Do the rest of the adjoint. Use temp variables so the compiler
		// knows it CAN overlap operations
		*/
		AdjX0 = fY1-fY2;
		AdjX1 = fY2-fY0;
		AdjX2 = fY0-fY1;

		pTri->fAdjoint[0][0] = AdjX0;
		pTri->fAdjoint[0][1] = AdjX1;
		pTri->fAdjoint[0][2] = AdjX2;

		AdjX0 = fX2-fX1;
		AdjX1 = fX0-fX2;
		AdjX2 = fX1-fX0;

		pTri->fAdjoint[1][0] = AdjX0;
		pTri->fAdjoint[1][1] = AdjX1;
		pTri->fAdjoint[1][2] = AdjX2;
	
		if (gpPDC->Context.u32Flags & SGLTT_DISABLEZBUFFER)
		{
		    /*
			// Note that BogusZ is an arbitrary mem location
			// so we have to convince the compiler it can't overlap
			// where pTri points to
			*/
			float BogusZIncremented = gfBogusInvZ+ (float) BOGUSINVZ_INCREMENT;

			pTri->fZ[0] = pTri->fZ[1] = pTri->fZ[2] = gfBogusInvZ;

			gfBogusInvZ = BogusZIncremented;
			
		}
		else
		{
			/* Use temp variables to allow FP overlap*/
			AdjX0 = gpPDC->pV0->fInvW * fMinInvZ;
			AdjX1 = gpPDC->pV1->fInvW * fMinInvZ;
			AdjX2 = gpPDC->pV2->fInvW * fMinInvZ;

			pTri->fZ[0] = AdjX0;
			pTri->fZ[1] = AdjX1;
			pTri->fZ[2] = AdjX2;
		}

#if DEBUG || DEBUGDEV

		if (gpPDC->pV0->fInvW > 10.0f || gpPDC->pV1->fInvW > 10.0f || gpPDC->pV2->fInvW > 10.0f)
		{
			DPFDEV ((DBG_WARNING, "ProcessQuadCore: fInvW out of range"));
		}
#endif

		/* 
		// get region bounding box. Do X extremities first. 
		// Note. We don't worry about negative values since a negative
		// value would screw up the region code anyway (unless the
		// clipping is on, in which case it would get fixed)
		*/
		RegMax = F_TO_I_IF_FP_SLOW(fX1);
		RegMin = F_TO_I_IF_FP_SLOW(fX0);

		if(RegMax < RegMin)
		{
			RegSwap	= RegMax;
			RegMax	= RegMin;
			RegMin	= RegSwap;
		}
		RegSwap = F_TO_I_IF_FP_SLOW(fX2);

		if(RegMax < RegSwap)
		{
			RegMax = RegSwap;
		}
		else if(RegSwap < RegMin)
		{
			RegMin = RegSwap;
		}

		/* 
		// vertex 3 ...  Need to read it first!
		*/
		#if MULTI_FP_REG || MODIFYXY
			#if MODIFYXY
				fX3 = pV3->fX + fAddToXY;
			#else
				fX3 = pV3->fX;
			#endif
		#endif

		RegSwap = F_TO_I_IF_FP_SLOW(fX3);

		if(RegMax < RegSwap)
		{
			RegMax = RegSwap;
		}
		else if(RegSwap < RegMin)
		{
			RegMin = RegSwap;
		}

		/*
		// Convert the X max and mins to integer region equivalent.
		//
		// NOTE on the Pentium, we'd like RegMin and Max to remain in
		// registers. However, if you take the pointer of a variable,
		// the compiler automatically puts it in memory. Avoid this
		// by using another couple of temporary variables!
		*/
		#if (SLOW_FCMP && !MULTI_FP_REG)
		{
			sgl_int32 RegMinInMem = RegMin;
			sgl_int32 RegMaxInMem = RegMax;
			
			/* Calculate bounding box for X0+1, Y0+1, X1, Y1
			 */
			if (RegMax > RegMin)
			   rX0 = ((sgl_int32) (LONG_TO_FLOAT(RegMinInMem) - fAddToXY)) >> ShiftRegX;
			else
			   rX0 = ((sgl_int32) LONG_TO_FLOAT(RegMinInMem)) >> ShiftRegX;

			rX1 = ((sgl_int32) LONG_TO_FLOAT(RegMaxInMem)) >> ShiftRegX;
		}
		#else
			/* Calculate bounding box for X0+1, Y0+1, X1, Y1
			 */
			if (RegMax > RegMin)
			   rX0 = ((sgl_int32) (RegMin - fAddToXY)) >> ShiftRegX;
			else
			   rX0 = ((sgl_int32) RegMin) >> ShiftRegX;

			rX1 = ((sgl_int32) RegMax) >> ShiftRegX;
		#endif

		/* 
		// Do Y
		*/
		RegMax = F_TO_I_IF_FP_SLOW(fY1);
		RegMin = F_TO_I_IF_FP_SLOW(fY0);

		if(RegMax < RegMin)
		{
			RegSwap	= RegMax;
			RegMax	= RegMin;
			RegMin	= RegSwap;
		}
		RegSwap = F_TO_I_IF_FP_SLOW(fY2);

		if(RegMax < RegSwap)
		{
			RegMax = RegSwap;
		}
		else if(RegSwap < RegMin)
		{
			RegMin = RegSwap;
		}

		/* 
		// vertex 3 ...  Need to read it first!
		*/
		#if MULTI_FP_REG || MODIFYXY
			#if MODIFYXY
				fY3 = pV3->fY + fAddToXY;
			#else
				fY3 = pV3->fY;
			#endif
		#endif

		RegSwap = F_TO_I_IF_FP_SLOW(fY3);

		if(RegMax < RegSwap)
		{
			RegMax = RegSwap;
		}
		else if(RegSwap < RegMin)
		{
			RegMin = RegSwap;
		}

		/*
		// Convert the "floats" to region equivalent.
		*/
		#if (SLOW_FCMP && !MULTI_FP_REG)
		{
			sgl_int32 RegMinInMem = RegMin;
			sgl_int32 RegMaxInMem = RegMax;
			
			/* Calculate bounding box for X0+1, Y0+1, X1, Y1
			 */
			if (RegMax > RegMin)
			   rY0 = (sgl_int32) (LONG_TO_FLOAT(RegMinInMem) - fAddToXY);
			else
			   rY0 = (sgl_int32) LONG_TO_FLOAT(RegMinInMem);

			rY1 = (sgl_int32) LONG_TO_FLOAT(RegMaxInMem);
		}
		#else
			/* Calculate bounding box for X0+1, Y0+1, X1, Y1
			 */
			if (RegMax > RegMin)
			   rY0 = (sgl_int32) (RegMin - fAddToXY);
			else
			   rY0 = (sgl_int32) RegMin;

			rY1 = (sgl_int32) RegMax;
		#endif

		/*
		// If we used floating point compares BUT all of the 
		// vert values were NEGATIVE, then the Max and Min
		// will be back-the-front. 
		*/
		#if (SLOW_FCMP && !MULTI_FP_REG)
			if(rX0 > rX1)
			{
				sgl_int32 swap;
				swap = rX0;
				rX0	 = rX1;
				rX1	 = swap;
			}

			if(rY0 > rY1)
			{
				sgl_int32 swap;
				swap = rY0;
				rY0	 = rY1;
				rY1	 = swap;
			}
		#endif

#if DEBUGDEV
		
		/* Check for Region clipping when bDoClipping is FALSE */
		if (!(gpPDC->Context.bDoClipping))
		{
			if( (rX0 < gpPDC->Context.FirstXRegion) ||
				(rY0 < gpPDC->Context.FirstYRegion) ||
				(rX1 > gpPDC->Context.LastXRegion)  ||
				(rY1 > gpPDC->Context.LastYRegion))
			{
			 	DPFDEV ((DBG_FATAL, "ProcessQuadCore: need clipping but bDoClipping is FALSE"));		
			}
		}
#endif
	
		/*
		// Check for Region clipping
		*/
		if (gpPDC->Context.bDoClipping)
		{
			if (rX0 < gpPDC->Context.FirstXRegion)
			{
				if ( rX1 < gpPDC->Context.FirstXRegion )
				{
					/* Very Unlikely */
					continue;
				}
				
				rX0 = gpPDC->Context.FirstXRegion;
			}
		
			if (rY0 < gpPDC->Context.FirstYRegion)
			{
				if ( rY1 < gpPDC->Context.FirstYRegion )
				{
					/* Very Unlikely */
					continue;
				}

				rY0 = gpPDC->Context.FirstYRegion;
			}
		
			if (rX1 > gpPDC->Context.LastXRegion)
			{
				if ( rX0 > gpPDC->Context.LastXRegion )
				{
					/* Very Unlikely */
					continue;
				}

				rX1 = gpPDC->Context.LastXRegion;
			}
		
			if (rY1 > gpPDC->Context.LastYRegion)
			{
				if ( rY0 > gpPDC->Context.LastYRegion )
				{
					/* Very Unlikely */
					continue;
				}

				rY1 = gpPDC->Context.LastYRegion;
			}
		}
		else
		{
			/* ASSERT the regions to avoid crashing later in dregion.c */
			ASSERT(rX0 >= gpPDC->Context.FirstXRegion);
			ASSERT(rY0 >= gpPDC->Context.FirstYRegion);
			ASSERT(rX1 <= gpPDC->Context.LastXRegion);
			ASSERT(rY1 <= gpPDC->Context.LastYRegion);
		}
		
		(pMat++)->NumExtraVertices = 1;

		/*
		// Use the AdjX as temp vars to convince the compilers that
		// overlapping FP ops is quite OK
		*/
		AdjX0 = fY2 - fY3;
		AdjX1 = fX3 - fX2;
		AdjX2 = fX2*fY3 - fX3*fY2;

		pEdge[0].fA = AdjX0;
		pEdge[0].fB = AdjX1;
		pEdge[0].fC = AdjX2;

		AdjX0 = fY3 - fY0;
		AdjX1 = fX0 - fX3;
		AdjX2 = fX3*fY0 - fX0*fY3;

		pEdge[1].fA = AdjX0;
		pEdge[1].fB = AdjX1;
		pEdge[1].fC = AdjX2;

		pEdge += 2;

		if (gpPDC->TSPControlWord & MASK_TRANS)
		{
			sgl_uint32 uDFIndex = (uCurrentType & 1);

			#if CHUNKING
				puPrev = &uPreviousIndices[uDFIndex];
			#endif
			
			if (!uDoneFirst[uDFIndex])
			{
				uCurrentType |= PACKED_TRANSTYPE_SETMARK;
				uDoneFirst[uDFIndex] = TRUE;
			}

			#if CHUNKING
	
				else 
				{				
					sgl_int32 nXORResult;

					nXORResult = (puPrev[0] == uCurrentIndices[0])
							   + (puPrev[0] == uCurrentIndices[1])
							   + (puPrev[0] == uCurrentIndices[2])
							   + (puPrev[0] == uCurrentIndices[3])
							   + (puPrev[1] == uCurrentIndices[0])
							   + (puPrev[1] == uCurrentIndices[1])
							   + (puPrev[1] == uCurrentIndices[2])
							   + (puPrev[1] == uCurrentIndices[3])
							   + (puPrev[2] == uCurrentIndices[0])
							   + (puPrev[2] == uCurrentIndices[1])
							   + (puPrev[2] == uCurrentIndices[2])
							   + (puPrev[2] == uCurrentIndices[3])
							   + (puPrev[3] == uCurrentIndices[0])
							   + (puPrev[3] == uCurrentIndices[1])
							   + (puPrev[3] == uCurrentIndices[2])
							   + (puPrev[3] == uCurrentIndices[3]);

					nXORResult --;
					nXORResult &= PACKED_TRANSTYPE_SETMARK;

					uCurrentType |= nXORResult;
				}

				puPrev[0] = uCurrentIndices[0];
				puPrev[1] = uCurrentIndices[1];
				puPrev[2] = uCurrentIndices[2];
				puPrev[3] = uCurrentIndices[3];

			#endif
		}
		
		/*
		// Get the inverse determinant at a point where we can overlap
		// the FP with loads of integer ops... ie good for Pentium
		*/
		pTri->f1OverDet = 1.0f / fDet;
		pTri->reg.u = ENCODE_OBJXYDATA( uCurrentType, rX0, rY0, rX1, rY1 );

		uCurrentType = uDefaultType;
		
		if ( pAddfn != NULL )
		{
			/* Call the type specific thing here */
			pAddfn( pTri );
		}
						
		if ( ProcessFlatTexFn != NULL )
		{
			/* Texture stuff in line */
			ProcessFlatTexFn( pTri );
		}
			
		/* Next material structure */
		gpMatCurrent++;

		if ( ++pTri == &gpTri[IBUFFERSIZE] )
		{
			/* Filled up the buffer! */
			break;
		}
		#if !MULTI_FP_REG && !MODIFYXY
			#undef fX0
			#undef fX1
			#undef fX2
			#undef fX3

			#undef fY0
			#undef fY1
			#undef fY2
			#undef fY3
		#endif
	}/*end while*/

	/* Return number in intermediate buffer this time */
	return( (int) ( pTri - gpTri ) );

} /* ProcessQuadCore */

/*
//===================================================
*/

#if !PROCESSD3DSTRIPCORE
/* Assembler version of ProcessD3DStripTransCore does non-translucent too */
#define ProcessD3DStripCore( pAddFn ) _ProcessD3DStripTransCore( pAddFn, 0 )
#else
static int ProcessD3DStripCore( PPIR pAddfn )
{
	PITRI pTri = gpTri;
	int	ShiftRegX = gpPDC->ShiftRegX;
	float fX0,fX1,fX2, fY0,fY1,fY2;
	float fDet;
	sgl_int32 rX0, rY0, rX1, rY1, nXRgn, nYRgn;
	sgl_uint32 u1, u2;

	PIMATERIAL 	pMat = gpMat;
	int 		nNumAttachedTriangles;
	int			Backwards;
	PIEDGE		pEdge, pStartEdge = gpEdge;
	sgl_uint32		u2NextFace;

	while (gpPDC->nInputTriangles > 0)
	{
		pEdge = pStartEdge;

		u1 = ((sgl_uint32 *) gpPDC->pFace)[0];
		u2 = ((sgl_uint32 *) gpPDC->pFace)[1];

		/* advance the face pointer */
		*((sgl_uint32 *) &gpPDC->pFace) += sizeof (SGLD3DFACE);
		
		gpPDC->nInputTriangles--;
		
		/*
		// Number of coplanar triangles (excluding this one) or an ODD/EVEN
		// flag.
		*/
		nNumAttachedTriangles = (u2 >> 16) & 0x1f;

		/* 
		   do a bit of work on the first triangle to get it 
		   oriented correctly - it may be part of a strip
		*/

 		/*
 		// When clipping is taking place D3D for some reason sets the flag of
 		// the second triangle in a quad to 1 instead of even or odd, so we
		// need to check that we actually have 'even' or 'odd' on that triangle
 		*/
 
		if ( ((nNumAttachedTriangles > 0) &
 		      (nNumAttachedTriangles < 30) &
 			  (nNumAttachedTriangles <= gpPDC->nInputTriangles)) &&
 			 (((u2NextFace = ((sgl_uint32*)gpPDC->pFace)[1]) & 0x1F0000) >=
 			  (30 << 16)) )
		{
			/* start of a coplanar strip/fan */
			
			if ((u2NextFace & 0x1F0000) == (SGLD3DFACEFLAG_EVEN << 16))
			{
				/* do it the easy normal way */
				
				gpPDC->pV0 = gpPDC->pVertices + (u1 & 0xFFFF);
				gpPDC->pV1 = gpPDC->pVertices + (u1 >> 16);
				gpPDC->pV2 = gpPDC->pVertices + (u2 & 0xFFFF);
			}
			else
			{
				ASSERT ((u2NextFace & 0x1F0000) == (SGLD3DFACEFLAG_ODD << 16));

				/* 
				   rotate triangle by 1 to allow the correct edges
				   to be added to the hardware. Attaching an ODD
				   triangle to a triangle rotated by 1 is like attaching
				   an EVEN triangle to the unrotated original, but the 
				   packer reads the correct edges from the adjoint matrix. 
				   We always therefore need to treat tri2 as EVEN
				*/

				gpPDC->pV1 = gpPDC->pVertices + (u1 & 0xFFFF);
				gpPDC->pV2 = gpPDC->pVertices + (u1 >> 16);
				gpPDC->pV0 = gpPDC->pVertices + (u2 & 0xFFFF);
				
				u2NextFace = (u2NextFace & 0x0000FFFF) | (SGLD3DFACEFLAG_EVEN << 16);
			}

		} /* if it's a coplanar fan with all triangles present */

		else
		{
			nNumAttachedTriangles = 0;
			
			gpPDC->pV0 = gpPDC->pVertices + (u1 & 0xFFFF);
			gpPDC->pV1 = gpPDC->pVertices + (u1 >> 16);
			gpPDC->pV2 = gpPDC->pVertices + (u2 & 0xFFFF);
		}
		
#if DEBUGDEV

	if ((gpPDC->pV0 == gpPDC->pV1) || (gpPDC->pV1 == gpPDC->pV2) || (gpPDC->pV0 == gpPDC->pV2))
	{
		DPFDEV((DBG_MESSAGE,"ProcessD3DStripCore: Triangle with identical vertices"));	
	}

#endif	
		ASSERT(gpPDC->pV0 != gpPDC->pV1 && gpPDC->pV0 != gpPDC->pV2 &&
		  gpPDC->pV1 != gpPDC->pV2);


#if MODIFYXY
		/* Fix offset promblem by adding an offset value.
		 */
		fX0 = gpPDC->pV0->fX + fAddToXY;
		fX1 = gpPDC->pV1->fX + fAddToXY;
		fX2 = gpPDC->pV2->fX + fAddToXY;
	
		fY0 = gpPDC->pV0->fY + fAddToXY;
		fY1 = gpPDC->pV1->fY + fAddToXY;
		fY2 = gpPDC->pV2->fY + fAddToXY;
#else
		fX0 = gpPDC->pV0->fX;
		fX1 = gpPDC->pV1->fX;
		fX2 = gpPDC->pV2->fX;
	
		fY0 = gpPDC->pV0->fY;
		fY1 = gpPDC->pV1->fY;
		fY2 = gpPDC->pV2->fY;
#endif

		/*
		// Just work out the bit we need for the determinant
		*/
		pTri->fAdjoint[2][0] = fX1*fY2 - fX2*fY1;
		pTri->fAdjoint[2][1] = fX2*fY0 - fX0*fY2;
		pTri->fAdjoint[2][2] = fX0*fY1 - fX1*fY0;
	
		/* calculate determinant and see if we can cull this plane */
	
		fDet =
		  pTri->fAdjoint[2][0] + pTri->fAdjoint[2][1] + pTri->fAdjoint[2][2];
	
		/*
		// Cull or reverse backfacing triangles, and store 1/determinant:
		*/
		/* fDet < LARGE_EPSILON */
		if(LESS_THAN_LARGE_EPSILON(fDet))
		{
			/* fDet > EPSILON  checking is the triangle covering a pixel? */
			if(!(LESS_THAN_EPSILON(fDet)) )
			{
	  			/* the triangle does not cover a pixel centre, cull it. */
	 			if(((int)fX0 == (int)fX1 && (int)fX0 == (int)fX2) ||
	 	  		 ((int)fY0 == (int)fY1 && (int)fY0 == (int)fY2))
	 			{		
					if (nNumAttachedTriangles)
					{
						/*
						// Skip all these triangles.
						*/
						gpPDC->nInputTriangles -= nNumAttachedTriangles;

						*((sgl_uint32 *) &gpPDC->pFace) +=
					 	 sizeof (SGLD3DFACE) * nNumAttachedTriangles;
					}
#if DEBUGDEV
					DPFDEV((DBG_MESSAGE,"ProcessD3DStripCore: culling sub-pixel face"));	
#endif
					continue;
	   			}
			}
			/* fDet < EPSILON */
			else
			{	
				PSGLVERTEX pV;
				float	   f;

				if (gpPDC->Context.bCullBackfacing)
				{
					if (nNumAttachedTriangles)
					{
						/*
						// Skip all these triangles.
						*/
						gpPDC->nInputTriangles -= nNumAttachedTriangles;

						*((sgl_uint32 *) &gpPDC->pFace) +=
					  	sizeof (SGLD3DFACE) * nNumAttachedTriangles;
					}
#if DEBUGDEV
					DPFDEV((DBG_MESSAGE,"ProcessD3DStripCore: culling back face"));	
#endif
					continue;
				}

				FP_NEGATE(fDet);

				/* fDet < LARGE_EPSILON */
		   		if(LESS_THAN_LARGE_EPSILON(fDet))
				{
					/* fDet > EPSILON  checking is the triangle covering a pixel? */
					if(!(LESS_THAN_EPSILON(fDet)) )
					{
					  	/* the triangle does not cover a pixel centre, cull it. */
					 	if(((int)fX0 == (int)fX1 && (int)fX0 == (int)fX2) ||
					 	   ((int)fY0 == (int)fY1 && (int)fY0 == (int)fY2))
					 	{	
							if (nNumAttachedTriangles)
							{
								/*
								// Skip all these triangles.
								*/
								gpPDC->nInputTriangles -= nNumAttachedTriangles;

								*((sgl_uint32 *) &gpPDC->pFace) +=
					  			sizeof (SGLD3DFACE) * nNumAttachedTriangles;
							}
#if DEBUGDEV
							DPFDEV((DBG_MESSAGE,"ProcessD3DStripCore: culling sub-pixel face"));	
#endif
							continue;
						}
					}
					else
					{				
						if (nNumAttachedTriangles)
						{
							/*
							// Skip all these triangles.
							*/
							gpPDC->nInputTriangles -= nNumAttachedTriangles;

							*((sgl_uint32 *) &gpPDC->pFace) +=
					 		 sizeof (SGLD3DFACE) * nNumAttachedTriangles;
						}
#if DEBUGDEV
						DPFDEV((DBG_MESSAGE,"ProcessD3DStripCore: culling tiny face"));	
#endif
						continue;
					}
				}

				/*
				// Reverse the triangle by swapping vertices 0 and 2 (which
				// even in an n-polygon situation ensures that the first three
				// supplied vertices are used for texturing and shading).
				*/
											
				pV = gpPDC->pV2;
				gpPDC->pV2 = gpPDC->pV0;
				gpPDC->pV0 = pV;
				
#if MODIFYXY
				/* Fix offset promblem by adding an offset value.
				 */
				fX0 = gpPDC->pV0->fX + fAddToXY;
				fX2 = gpPDC->pV2->fX + fAddToXY;
	
				fY0 = gpPDC->pV0->fY + fAddToXY;
				fY2 = gpPDC->pV2->fY + fAddToXY;
#else
				fX0 = gpPDC->pV0->fX;
				fX2 = gpPDC->pV2->fX;
	
				fY0 = gpPDC->pV0->fY;
				fY2 = gpPDC->pV2->fY;
#endif
	
				/* swap over [2][0] and [2][2] of the adjoint */
	
				f = pTri->fAdjoint[2][2];
				pTri->fAdjoint[2][2] = pTri->fAdjoint[2][0];
				pTri->fAdjoint[2][0] = f;
	
				/* and then negate everything */
				
				*((sgl_uint32 *) &pTri->fAdjoint[2][0]) ^= 0x80000000;
				*((sgl_uint32 *) &pTri->fAdjoint[2][1]) ^= 0x80000000;
				*((sgl_uint32 *) &pTri->fAdjoint[2][2]) ^= 0x80000000;
	
				/* Not needed as fDet already negated */
				/* *((sgl_uint32 *) &fDet) ^= 0x80000000;*/

				Backwards = TRUE;
			}/*end else if(!(LESS_THAN_EPSILON(fDet)) )*/   
		}
	   	else
		{
			Backwards = FALSE;
		}/* end of if(LESS_THAN_LARGE_EPSILON(fDet))*/

	
		/*
		// Do the rest of the adjoint.
		*/

		pTri->fAdjoint[0][0] = fY1-fY2;
		pTri->fAdjoint[0][1] = fY2-fY0;
		pTri->fAdjoint[0][2] = fY0-fY1;
	
		pTri->fAdjoint[1][0] = fX2-fX1;
		pTri->fAdjoint[1][1] = fX0-fX2;
		pTri->fAdjoint[1][2] = fX1-fX0;
	
		pTri->f1OverDet = 1.0f / fDet;

		if (gpPDC->Context.u32Flags & SGLTT_DISABLEZBUFFER)
		{
		    /*
			// Note that BogusZ is an arbitrary mem location
			// so we have to convince the compiler it can't overlap
			// where pTri points to
			*/
			float BogusZIncremented = gfBogusInvZ + (float) BOGUSINVZ_INCREMENT;

			pTri->fZ[0] = pTri->fZ[1] = pTri->fZ[2] = gfBogusInvZ;

			gfBogusInvZ = BogusZIncremented;
		}
		else
		{
			pTri->fZ[0] = gpPDC->pV0->fInvW * fMinInvZ;
			pTri->fZ[1] = gpPDC->pV1->fInvW * fMinInvZ;
			pTri->fZ[2] = gpPDC->pV2->fInvW * fMinInvZ;
		}

#if DEBUG || DEBUGDEV

		if (gpPDC->pV0->fInvW > 10.0f || gpPDC->pV1->fInvW > 10.0f || gpPDC->pV2->fInvW > 10.0f)
		{
			DPFDEV ((DBG_WARNING, "ProcessD3DStripCore: fInvW out of range"));
		}
#endif
		
		/*
		// Do common region bounding box calculations
		*/
		rX0 = ((sgl_int32) fX0) >> ShiftRegX;
		rX1 = ((sgl_int32) fX1) >> ShiftRegX;
		nXRgn = ((sgl_int32) fX2) >> ShiftRegX;
				
		if ( ( ( rX0 ^ rX1 ) | ( rX1 ^ nXRgn ) ) != 0 )
		{
			/* Relatively Unlikely */
			int temp;

			if (rX0 > rX1)
			{
				/* Swap them */
				temp = rX0;
				rX0 = rX1;
				rX1 = temp;
			}
			
			if ( nXRgn <= rX0 )
			{
				rX0 = nXRgn;
			}
			else
			if ( nXRgn > rX1 )
			{
				rX1 = nXRgn;
			}
		}
		
		rY0 = ((sgl_int32) fY0) & (~((1<<Y_SHIFT)-1)) ;
		rY1 = ((sgl_int32) fY1) & (~((1<<Y_SHIFT)-1)) ;
		nYRgn = ((sgl_int32) fY2) & (~((1<<Y_SHIFT)-1)) ;

		if ( ( ( rY0 ^ rY1 ) | ( rY1 ^ nYRgn ) ) != 0 )
		{
			/* Relatively Unlikely */
			int temp;

			if (rY0 > rY1)
			{
				/* Swap them */
				temp = rY0;
				rY0 = rY1;
				rY1 = temp;
			}
			
			if ( nYRgn < rY0 )
			{
				rY0 = nYRgn;
			}
			else
			if ( nYRgn > rY1 )
			{
				rY1 = nYRgn;
			}
		}
		
#if 0
		/* original version without small quads */

		/* this is intended! the compiler likes it ... */
		if ((pMat->NumExtraVertices = nNumAttachedTriangles) > 0)
#else
		/* Do small quads if special flag is set */
		if ((gpPDC->Context.u32Flags & 0x80000000) && fDet > 20.0f)
		{
			pMat->NumExtraVertices = 0;
		}
		else if ((pMat->NumExtraVertices = nNumAttachedTriangles) > 0)
#endif
		{
			PSGLVERTEX	pV3;
			float 		fX3, fY3;

			u1 = ((sgl_uint32 *) gpPDC->pFace)[0];
			u2 = ((sgl_uint32 *) gpPDC->pFace)[1];

			/*
			// get second triangle's X and Y
			*/
			
			pV3 = gpPDC->pVertices + (u2NextFace & 0xFFFF);

#if MODIFYXY
			fX3 = pV3->fX + fAddToXY;
			fY3 = pV3->fY + fAddToXY;
#else
			fX3 = pV3->fX;
			fY3 = pV3->fY;
#endif
			/*
			// move ptrs past the second triangle
			*/

			*((sgl_uint32 *) &gpPDC->pFace) += sizeof(SGLD3DFACE);

			/* might as well do this in 1 go */
			gpPDC->nInputTriangles -= nNumAttachedTriangles;

			/*
			// N-POLYGONS (where n >= 4)
			//
			// If a polygon's triangles go past the end of the buffer then we
			// currently treat them all as separate triangles.
			*/

			/* 
			   iterate through all triangles except the last. If we have a quad
			   this block is bypassed completely
		    */
			
			while (--nNumAttachedTriangles)
			{
				/* do region stuff for current face
				 */
				nXRgn = ((sgl_int32) fX3) >> ShiftRegX;
				
				if (nXRgn < rX0)
				{
					rX0 = nXRgn;
				}
				else if (nXRgn > rX1)
				{
					rX1 = nXRgn;
				}
				
				nYRgn = (sgl_int32) fY3;
				
				if (nYRgn < rY0)
				{
					rY0 = nYRgn;
				}
				else if (nYRgn > rY1)
				{
					rY1 = nYRgn;
				}

				/* get v3 and odd/even status from next face */
				u2NextFace = ((sgl_uint32 *) gpPDC->pFace)[1];

				/* calculate edge equation and reset X and Y variables */
				
				if (((u2NextFace & 0x1F0000) == (SGLD3DFACEFLAG_EVEN << 16)) ^ Backwards)
				{
					/* we need to join current vertex (v3) to right vertex (v2) */

					pEdge[0].fA = fY2 - fY3;
					pEdge[0].fB = fX3 - fX2;
					pEdge[0].fC = fX2*fY3 - fX3*fY2;
				
					/* v3->v2, v0->v0 */
				
					fX2 = fX3;
					fY2 = fY3;
				}
				else
				{
					/* we need to join current vertex (v3) to left vertex (v0) */

					pEdge[0].fA = fY3 - fY0;
					pEdge[0].fB = fX0 - fX3;
					pEdge[0].fC = fX3*fY0 - fX0*fY3;
					
					/* v3->v0, v1->v1 */
					
					fX0 = fX3;
					fY0 = fY3;
				}
				
				pEdge++;

				/*
				// triangle who's flags we have just used becomes the
				// current triangle
				*/

				pV3 = gpPDC->pVertices + (u2NextFace & 0xFFFF);

#if MODIFYXY
				fX3 = pV3->fX + fAddToXY;
				fY3 = pV3->fY + fAddToXY;
#else
				fX3 = pV3->fX;
				fY3 = pV3->fY;
#endif
				/*
				// advance to next triangle ...
				*/

				*((sgl_uint32 *) &gpPDC->pFace) += sizeof(SGLD3DFACE);

			} /* while (--nNumAttachedTriangles) */
			
			/* close last triangle off. NB this closes off quads too */
	
			pEdge[0].fA = fY3 - fY0;
			pEdge[0].fB = fX0 - fX3;
			pEdge[0].fC = fX3*fY0 - fX0*fY3;
		
			pEdge[1].fA = fY2 - fY3;
			pEdge[1].fB = fX3 - fX2;
			pEdge[1].fC = fX2*fY3 - fX3*fY2;
			
			/* do region stuff for last face */
			
			nXRgn = ((sgl_int32) fX3) >> ShiftRegX;
			
			if (nXRgn < rX0)
			{
				rX0 = nXRgn;
			}
			else if (nXRgn > rX1)
			{
				rX1 = nXRgn;
			}
			
			nYRgn = (sgl_int32) fY3;
			
			if (nYRgn < rY0)
			{
				rY0 = nYRgn;
			}
			else if (nYRgn > rY1)
			{
				rY1 = nYRgn;
			}

			pEdge += 2;
			
			ASSERT (pEdge < (gpEdge + IEDGEBUFFERSIZE));
			
		} /* n-polygons */	

#if DEBUGDEV
		
		/* Check for Region clipping when bDoClipping is FALSE */
		if (!(gpPDC->Context.bDoClipping))
		{
			if( (rX0 < gpPDC->Context.FirstXRegion) ||
				(rY0 < gpPDC->Context.FirstYRegion) ||
				(rX1 > gpPDC->Context.LastXRegion)  ||
				(rY1 > gpPDC->Context.LastYRegion))
			{
			 	DPFDEV ((DBG_FATAL, "ProcessD3DStripCore: need clipping but bDoClipping is FALSE"));
							
			}
		}
#endif
		
		if (gpPDC->Context.bDoClipping)
		{
			if (rX0 < gpPDC->Context.FirstXRegion)
			{
				if ( rX1 < gpPDC->Context.FirstXRegion )
				{
					/* Very Unlikely */
					continue;
				}
				
				rX0 = gpPDC->Context.FirstXRegion;
			}
		
			if (rY0 < gpPDC->Context.FirstYRegion)
			{
				if ( rY1 < gpPDC->Context.FirstYRegion )
				{
					/* Very Unlikely */
					continue;
				}

				rY0 = gpPDC->Context.FirstYRegion;
			}
		
			if (rX1 > gpPDC->Context.LastXRegion)
			{
				if ( rX0 > gpPDC->Context.LastXRegion )
				{
					/* Very Unlikely */
					continue;
				}

				rX1 = gpPDC->Context.LastXRegion;
			}
		
			if (rY1 > gpPDC->Context.LastYRegion)
			{
				if ( rY0 > gpPDC->Context.LastYRegion )
				{
					/* Very Unlikely */
					continue;
				}

				rY1 = gpPDC->Context.LastYRegion;
			}
		}
		else
		{
			/* ASSERT the regions to avoid crashing later in dregion.c */
			ASSERT(rX0 >= gpPDC->Context.FirstXRegion);
			ASSERT(rY0 >= gpPDC->Context.FirstYRegion);
			ASSERT(rX1 <= gpPDC->Context.LastXRegion);
			ASSERT(rY1 <= gpPDC->Context.LastYRegion);
		}

		pTri->reg.u = ENCODE_OBJXYDATA( PACKED_TYPE_OPAQUE, rX0, rY0, rX1, rY1 );

		pTri->TSPControlWord = 	gpPDC->TSPControlWord;
		pTri->BaseColour = 		gpPDC->pV0->u32Colour;

		if ( pAddfn != NULL )
		{
			/* Call the type specific thing here */
			pAddfn( pTri );
		}
						
		if ( ProcessFlatTexFn != NULL )
		{
			/* Texture stuff in line */
			ProcessFlatTexFn( pTri );
		}
			
		/* Next material structure */
		gpMatCurrent = ++pMat;

		pStartEdge = pEdge;

		if ( ++pTri == &gpTri[IBUFFERSIZE] )
		{
			/* Filled up the buffer! */
			break;
		}
	}

	/* Return number in intermediate buffer this time */
	return( (int) ( pTri - gpTri ) );

} /* ProcessD3DStripCore */
#endif

/*
//===================================================
*/

#if !PROCESSD3DSTRIPCORE
extern int _ProcessD3DStripTransCore( PPIR pAddfn, sgl_uint32 NewObject );
#define ProcessD3DStripTransCore _ProcessD3DStripTransCore
#else
static int ProcessD3DStripTransCore( PPIR pAddfn, sgl_uint32 NewObject )
{
	PITRI pTri = gpTri;
	PIMATERIAL pMat = gpMat;
	int	ShiftRegX = gpPDC->ShiftRegX;
	float fX0,fX1,fX2, fY0,fY1,fY2;
	float fDet;
	sgl_int32 rX0, rY0, rX1, rY1, nXRgn, nYRgn;
	sgl_uint32 u1, u2;
	sgl_uint32 uDefaultType, uCurrentType, uOrBackType;
	static sgl_uint32 uDoneFirst[2];

	int 		nNumAttachedTriangles;
	int			Backwards;
	PIEDGE		pEdge = gpEdge, pStartEdge;
	sgl_uint32		u2NextFace;
			
	#if CHUNKING

		sgl_uint32 *puPrev;
		static sgl_uint32 uCurrentIndices[3];
		static sgl_uint32 uPreviousIndices[2][3];

	#endif

	if (gPDC.Context.u32Flags & SGLTT_OPAQUE)
	{
		uDefaultType = PACKED_TYPE_OPAQUETRANS;
		uCurrentType = PACKED_TYPE_OPAQUETRANS;
		uOrBackType	 = 0;
	}
	else if (gPDC.Context.u32Flags & SGLTT_NEWPASSPERTRI)
	{
		uDefaultType = PACKED_TYPE_TRANSLUCENT | PACKED_TRANSTYPE_SETMARK;
		uCurrentType = PACKED_TYPE_TRANSLUCENT | PACKED_TRANSTYPE_SETMARK;
		uOrBackType = 0;
	}
	/* if no sort, put all trans face in the front list */
	else if (gNoSortTransFaces != MINZ_SORT)
	{
		if (NewObject)
		{
			uDoneFirst[0] = FALSE;
			uDoneFirst[1] = FALSE;
		}

		uDefaultType = PACKED_TYPE_TRANSLUCENT;
		uCurrentType = PACKED_TYPE_TRANSLUCENT;
		uOrBackType = 0;
	}
	else
	{
		if (NewObject)
		{
			uDoneFirst[0] = FALSE;
			uDoneFirst[1] = FALSE;
		}
			
		uDefaultType = PACKED_TYPE_TRANSLUCENT;
		uCurrentType = PACKED_TYPE_TRANSLUCENT;
		uOrBackType = PACKED_TRANSTYPE_BACKBIT;
	}

	while (gpPDC->nInputTriangles > 0)
	{
		pStartEdge = pEdge;

		u1 = ((sgl_uint32 *) gpPDC->pFace)[0];
		u2 = ((sgl_uint32 *) gpPDC->pFace)[1];

		/* advance the face pointer
		 */
		*((sgl_uint32 *) &gpPDC->pFace) += sizeof (SGLD3DFACE);
		
		gpPDC->nInputTriangles--;
		
		/*
		// Number of coplanar triangles (excluding this one) or an ODD/EVEN
		// flag.
		*/
		nNumAttachedTriangles = (u2 >> 16) & 0x1f;

		/* 
		   do a bit of work on the first triangle to get it 
		   oriented correctly - it may be part of a strip
		*/
		if ((nNumAttachedTriangles > 0) &
		    (nNumAttachedTriangles < 30) &
			(nNumAttachedTriangles <= gpPDC->nInputTriangles))
		{
			/* start of a coplanar strip/fan */
			
			u2NextFace = ((sgl_uint32 *) gpPDC->pFace)[1];
			
			if ((u2NextFace & 0x1F0000) == (SGLD3DFACEFLAG_EVEN << 16))
			{
				/* do it the easy normal way */
				
				gpPDC->pV0 = gpPDC->pVertices + (u1 & 0xFFFF);
				gpPDC->pV1 = gpPDC->pVertices + (u1 >> 16);
				gpPDC->pV2 = gpPDC->pVertices + (u2 & 0xFFFF);
			}
			else
			{
				ASSERT ((u2NextFace & 0x1F0000) == (SGLD3DFACEFLAG_ODD << 16));

				/* 
				   rotate triangle by 1 to allow the correct edges
				   to be added to the hardware. Attaching an ODD
				   triangle to a triangle rotated by 1 is like attaching
				   an EVEN triangle to the unrotated original, but the 
				   packer reads the correct edges from the adjoint matrix. 
				   We always therefore need to treat tri2 as EVEN
				*/
				gpPDC->pV1 = gpPDC->pVertices + (u1 & 0xFFFF);
				gpPDC->pV2 = gpPDC->pVertices + (u1 >> 16);
				gpPDC->pV0 = gpPDC->pVertices + (u2 & 0xFFFF);
				
				u2NextFace = (u2NextFace & 0x0000FFFF) | (SGLD3DFACEFLAG_EVEN << 16);
			}
		} /* if it's a coplanar fan with all triangles present */

		else
		{
			nNumAttachedTriangles = 0;
			
			gpPDC->pV0 = gpPDC->pVertices + (u1 & 0xFFFF);
			gpPDC->pV1 = gpPDC->pVertices + (u1 >> 16);
			gpPDC->pV2 = gpPDC->pVertices + (u2 & 0xFFFF);
		}

#if DEBUGDEV

	if ((gpPDC->pV0 == gpPDC->pV1) || (gpPDC->pV1 == gpPDC->pV2) || (gpPDC->pV0 == gpPDC->pV2))
	{
		DPFDEV((DBG_MESSAGE,"ProcessD3DStripTransCore: Triangle with identical vertices"));
			
	}

#endif	
		ASSERT(gpPDC->pV0 != gpPDC->pV1 && gpPDC->pV0 != gpPDC->pV2 &&
		  gpPDC->pV1 != gpPDC->pV2);
				
#if MODIFYXY
		/* Fix offset promblem by adding an offset value.
		 */
		fX0 = gpPDC->pV0->fX + fAddToXY;
		fX1 = gpPDC->pV1->fX + fAddToXY;
		fX2 = gpPDC->pV2->fX + fAddToXY;
	
		fY0 = gpPDC->pV0->fY + fAddToXY;
		fY1 = gpPDC->pV1->fY + fAddToXY;
		fY2 = gpPDC->pV2->fY + fAddToXY;
#else
		fX0 = gpPDC->pV0->fX;
		fX1 = gpPDC->pV1->fX;
		fX2 = gpPDC->pV2->fX;
	
		fY0 = gpPDC->pV0->fY;
		fY1 = gpPDC->pV1->fY;
		fY2 = gpPDC->pV2->fY;
#endif

		/*
		// Just work out the bit we need for the determinant
		*/
		pTri->fAdjoint[2][0] = fX1*fY2 - fX2*fY1;
		pTri->fAdjoint[2][1] = fX2*fY0 - fX0*fY2;
		pTri->fAdjoint[2][2] = fX0*fY1 - fX1*fY0;
	
		/* calculate determinant and see if we can cull this plane */
	
		fDet =
		  pTri->fAdjoint[2][0] + pTri->fAdjoint[2][1] + pTri->fAdjoint[2][2];
	
		/*
		// Cull or reverse backfacing triangles, and store 1/determinant:
		*/
		/* fDet < LARGE_EPSILON */
		if(LESS_THAN_LARGE_EPSILON(fDet))
		{
			/* fDet > EPSILON  checking is the triangle covering a pixel? */
			if(!(LESS_THAN_EPSILON(fDet)) )
			{
	  			/* the triangle does not cover a pixel centre, cull it. */
	 			if(((int)fX0 == (int)fX1 && (int)fX0 == (int)fX2) ||
	 	  		 ((int)fY0 == (int)fY1 && (int)fY0 == (int)fY2))
	 			{		
					if (nNumAttachedTriangles)
					{
						/*
						// Skip all these triangles.
						*/
						gpPDC->nInputTriangles -= nNumAttachedTriangles;

						*((sgl_uint32 *) &gpPDC->pFace) +=
					 	 sizeof (SGLD3DFACE) * nNumAttachedTriangles;
					}
#if DEBUGDEV
					DPFDEV((DBG_MESSAGE,"ProcessD3DStripTransCore: culling sub-pixel face"));	
#endif
					continue;
	   			}
			}
			/* fDet < EPSILON */
			else
			{	
				PSGLVERTEX pV;
				float	   f;

				if (gpPDC->Context.bCullBackfacing)
				{
					if (nNumAttachedTriangles)
					{
						/*
						// Skip all these triangles.
						*/
						gpPDC->nInputTriangles -= nNumAttachedTriangles;
				
						*((sgl_uint32 *) &gpPDC->pFace) +=
						  sizeof (SGLD3DFACE) * nNumAttachedTriangles;
					}
#if DEBUGDEV
					DPFDEV((DBG_MESSAGE,"ProcessD3DStripTransCore: culling back face"));	
#endif
					continue;
				}

				FP_NEGATE(fDet);

				/* fDet < LARGE_EPSILON */
		   		if(LESS_THAN_LARGE_EPSILON(fDet))
				{
					/* fDet > EPSILON  checking is the triangle covering a pixel? */
					if(!(LESS_THAN_EPSILON(fDet)) )
					{
					  	/* the triangle does not cover a pixel centre, cull it. */
					 	if(((int)fX0 == (int)fX1 && (int)fX0 == (int)fX2) ||
					 	   ((int)fY0 == (int)fY1 && (int)fY0 == (int)fY2))
					 	{	
							if (nNumAttachedTriangles)
							{
								/*
								// Skip all these triangles.
								*/
								gpPDC->nInputTriangles -= nNumAttachedTriangles;

								*((sgl_uint32 *) &gpPDC->pFace) +=
					  			sizeof (SGLD3DFACE) * nNumAttachedTriangles;
							}
#if DEBUGDEV
							DPFDEV((DBG_MESSAGE,"ProcessD3DStripTransCore: culling sub-pixel face"));	
#endif
							continue;
						}
					}
					else
					{				
						if (nNumAttachedTriangles)
						{
							/*
							// Skip all these triangles.
							*/
							gpPDC->nInputTriangles -= nNumAttachedTriangles;

							*((sgl_uint32 *) &gpPDC->pFace) +=
					 		 sizeof (SGLD3DFACE) * nNumAttachedTriangles;
						}
#if DEBUGDEV
						DPFDEV((DBG_MESSAGE,"ProcessD3DStripTransCore: culling tiny face"));	
#endif
						continue;
					}
				}

				/*
				// Reverse the triangle by swapping vertices 0 and 2 (which
				// even in an n-polygon situation ensures that the first three
				// supplied vertices are used for texturing and shading).
				*/
			   							
				pV = gpPDC->pV2;
				gpPDC->pV2 = gpPDC->pV0;
				gpPDC->pV0 = pV;
				
#if MODIFYXY
				/* Fix offset promblem by adding an offset value.
				 */
				fX0 = gpPDC->pV0->fX + fAddToXY;
				fX2 = gpPDC->pV2->fX + fAddToXY;
	
				fY0 = gpPDC->pV0->fY + fAddToXY;
				fY2 = gpPDC->pV2->fY + fAddToXY;
#else
				fX0 = gpPDC->pV0->fX;
				fX2 = gpPDC->pV2->fX;
	
				fY0 = gpPDC->pV0->fY;
				fY2 = gpPDC->pV2->fY;
#endif
	
				/* swap over [2][0] and [2][2] of the adjoint */
	
				f = pTri->fAdjoint[2][2];
				pTri->fAdjoint[2][2] = pTri->fAdjoint[2][0];
				pTri->fAdjoint[2][0] = f;
	
				/* and then negate everything */
				
				*((sgl_uint32 *) &pTri->fAdjoint[2][0]) ^= 0x80000000;
				*((sgl_uint32 *) &pTri->fAdjoint[2][1]) ^= 0x80000000;
				*((sgl_uint32 *) &pTri->fAdjoint[2][2]) ^= 0x80000000;
	
				/* Not needed as fDet already negated */
				/* *((sgl_uint32 *) &fDet) ^= 0x80000000;*/

				uCurrentType |= uOrBackType;
				
				Backwards = TRUE;
			}/*end else if(!(LESS_THAN_EPSILON(fDet)) )*/
		}
		else
		{
			Backwards = FALSE;
		}/* end of if(LESS_THAN_LARGE_EPSILON(fDet))*/

	
		/*
		// Do the rest of the adjoint.
		*/

		pTri->fAdjoint[0][0] = fY1-fY2;
		pTri->fAdjoint[0][1] = fY2-fY0;
		pTri->fAdjoint[0][2] = fY0-fY1;
	
		pTri->fAdjoint[1][0] = fX2-fX1;
		pTri->fAdjoint[1][1] = fX0-fX2;
		pTri->fAdjoint[1][2] = fX1-fX0;
	
		pTri->f1OverDet = 1.0f / fDet;

		if (gpPDC->Context.u32Flags & SGLTT_DISABLEZBUFFER)
		{
		    /*
			// Note that BogusZ is an arbitrary mem location
			// so we have to convince the compiler it can't overlap
			// where pTri points to
			*/
			float BogusZIncremented = gfBogusInvZ + (float) BOGUSINVZ_INCREMENT;

			pTri->fZ[0] = pTri->fZ[1] = pTri->fZ[2] = gfBogusInvZ;

			gfBogusInvZ = BogusZIncremented;
		}
		else
		{
			pTri->fZ[0] = gpPDC->pV0->fInvW * fMinInvZ;
			pTri->fZ[1] = gpPDC->pV1->fInvW * fMinInvZ;
			pTri->fZ[2] = gpPDC->pV2->fInvW * fMinInvZ;
		}

#if DEBUG || DEBUGDEV

		if (gpPDC->pV0->fInvW > 10.0f || gpPDC->pV1->fInvW > 10.0f || gpPDC->pV2->fInvW > 10.0f)
		{
			DPFDEV ((DBG_WARNING, "ProcessD3DStripTransCore: fInvW out of range"));
		}
#endif		
		/*
		// Do common region bounding box calculations
		*/
		rX0 = ((sgl_int32) fX0) >> ShiftRegX;
		rX1 = ((sgl_int32) fX1) >> ShiftRegX;
		nXRgn = ((sgl_int32) fX2) >> ShiftRegX;
				
		if ( ( ( rX0 ^ rX1 ) | ( rX1 ^ nXRgn ) ) != 0 )
		{
			/* Relatively Unlikely */
			int temp;

			if (rX0 > rX1)
			{
				/* Swap them */
				temp = rX0;
				rX0 = rX1;
				rX1 = temp;
			}
			
			if ( nXRgn <= rX0 )
			{
				rX0 = nXRgn;
			}
			else
			if ( nXRgn > rX1 )
			{
				rX1 = nXRgn;
			}
		}
		
		rY0 = ((sgl_int32) fY0) & (~((1<<Y_SHIFT)-1)) ;
		rY1 = ((sgl_int32) fY1) & (~((1<<Y_SHIFT)-1)) ;
		nYRgn = ((sgl_int32) fY2) & (~((1<<Y_SHIFT)-1)) ;

		if ( ( ( rY0 ^ rY1 ) | ( rY1 ^ nYRgn ) ) != 0 )
		{
			/* Relatively Unlikely */
			int temp;

			if (rY0 > rY1)
			{
				/* Swap them */
				temp = rY0;
				rY0 = rY1;
				rY1 = temp;
			}
			
			if ( nYRgn < rY0 )
			{
				rY0 = nYRgn;
			}
			else
			if ( nYRgn > rY1 )
			{
				rY1 = nYRgn;
			}
		}
		
		/* this is intended! the compiler likes it ... */
		if ((pMat->NumExtraVertices = nNumAttachedTriangles) > 0)
		{
			PSGLVERTEX	pV3;
			float 		fX3, fY3;

			u1 = ((sgl_uint32 *) gpPDC->pFace)[0];
			u2 = ((sgl_uint32 *) gpPDC->pFace)[1];

			/*
			// get second triangle's X and Y
			*/
			
			pV3 = gpPDC->pVertices + (u2NextFace & 0xFFFF);

#if MODIFYXY
			fX3 = pV3->fX + fAddToXY;
			fY3 = pV3->fY + fAddToXY;
#else
			fX3 = pV3->fX;
			fY3 = pV3->fY;
#endif
			/*
			// move ptrs past the second triangle
			*/
			*((sgl_uint32 *) &gpPDC->pFace) += sizeof(SGLD3DFACE);

			/* might as well do this in 1 go */
			gpPDC->nInputTriangles -= nNumAttachedTriangles;

			/*
			// N-POLYGONS (where n >= 4)
			//
			// If a polygon's triangles go past the end of the buffer then we
			// currently treat them all as separate triangles.
			*/

			/* 
			   iterate through all triangles except the last. If we have a quad
			   this block is bypassed completely
		    */
			while (--nNumAttachedTriangles)
			{
				/* do region stuff for current face */
				
				nXRgn = ((sgl_int32) fX3) >> ShiftRegX;
				
				if (nXRgn < rX0)
				{
					rX0 = nXRgn;
				}
				else if (nXRgn > rX1)
				{
					rX1 = nXRgn;
				}
				
				nYRgn = (sgl_int32) fY3;
				
				if (nYRgn < rY0)
				{
					rY0 = nYRgn;
				}
				else if (nYRgn > rY1)
				{
					rY1 = nYRgn;
				}

				/* get v3 and odd/even status from next face */
				u2NextFace = ((sgl_uint32 *) gpPDC->pFace)[1];

				/* calculate edge equation and reset X and Y variables */
				
				if (((u2NextFace & 0x1F0000) == (SGLD3DFACEFLAG_EVEN << 16)) ^ Backwards)
				{
					/* we need to join current vertex (v3) to right vertex (v2) */

					pEdge[0].fA = fY2 - fY3;
					pEdge[0].fB = fX3 - fX2;
					pEdge[0].fC = fX2*fY3 - fX3*fY2;
				
					/* v3->v2, v0->v0 */
				
					fX2 = fX3;
					fY2 = fY3;
				}
				else
				{
					/* we need to join current vertex (v3) to left vertex (v0) */

					pEdge[0].fA = fY3 - fY0;
					pEdge[0].fB = fX0 - fX3;
					pEdge[0].fC = fX3*fY0 - fX0*fY3;
					
					/* v3->v0, v1->v1 */
					
					fX0 = fX3;
					fY0 = fY3;
				}
				
				pEdge++;

				/*
				// triangle who's flags we have just used becomes the
				// current triangle
				*/

				pV3 = gpPDC->pVertices + (u2NextFace & 0xFFFF);

#if MODIFYXY
				fX3 = pV3->fX + fAddToXY;
				fY3 = pV3->fY + fAddToXY;
#else
				fX3 = pV3->fX;
				fY3 = pV3->fY;
#endif
				/*
				// advance to next triangle ...
				*/
				*((sgl_uint32 *) &gpPDC->pFace) += sizeof(SGLD3DFACE);

			} /* while (--nNumAttachedTriangles) */
			
			/* close last triangle off. NB this closes off quads too */
	
			pEdge[0].fA = fY3 - fY0;
			pEdge[0].fB = fX0 - fX3;
			pEdge[0].fC = fX3*fY0 - fX0*fY3;
		
			pEdge[1].fA = fY2 - fY3;
			pEdge[1].fB = fX3 - fX2;
			pEdge[1].fC = fX2*fY3 - fX3*fY2;
			
			/* do region stuff for last face */
			
			nXRgn = ((sgl_int32) fX3) >> ShiftRegX;
			
			if (nXRgn < rX0)
			{
				rX0 = nXRgn;
			}
			else if (nXRgn > rX1)
			{
				rX1 = nXRgn;
			}
			
			nYRgn = (sgl_int32) fY3;
			
			if (nYRgn < rY0)
			{
				rY0 = nYRgn;
			}
			else if (nYRgn > rY1)
			{
				rY1 = nYRgn;
			}

			pEdge += 2;
			
			ASSERT (pEdge < (gpEdge + IEDGEBUFFERSIZE));
			
		} /* n-polygons */	
	
#if DEBUGDEV
		
		/* Check for Region clipping when bDoClipping is FALSE */
		if (!(gpPDC->Context.bDoClipping))
		{
			if( (rX0 < gpPDC->Context.FirstXRegion) ||
				(rY0 < gpPDC->Context.FirstYRegion) ||
				(rX1 > gpPDC->Context.LastXRegion)  ||
				(rY1 > gpPDC->Context.LastYRegion))
			{
			 	DPFDEV ((DBG_FATAL, "ProcessD3DStripTransCore: need clipping but bDoClipping is FALSE"));
			   
			}
		}
#endif
	
		if (gpPDC->Context.bDoClipping)
		{
			if (rX0 < gpPDC->Context.FirstXRegion)
			{
				if ( rX1 < gpPDC->Context.FirstXRegion )
				{
					/* Very Unlikely */
					pEdge = pStartEdge;
					continue;
				}
				
				rX0 = gpPDC->Context.FirstXRegion;
			}
		
			if (rY0 < gpPDC->Context.FirstYRegion)
			{
				if ( rY1 < gpPDC->Context.FirstYRegion )
				{
					/* Very Unlikely */
					pEdge = pStartEdge;
					continue;
				}

				rY0 = gpPDC->Context.FirstYRegion;
			}
		
			if (rX1 > gpPDC->Context.LastXRegion)
			{
				if ( rX0 > gpPDC->Context.LastXRegion )
				{
					/* Very Unlikely */
					pEdge = pStartEdge;
					continue;
				}

				rX1 = gpPDC->Context.LastXRegion;
			}
		
			if (rY1 > gpPDC->Context.LastYRegion)
			{
				if ( rY0 > gpPDC->Context.LastYRegion )
				{
					/* Very Unlikely */
					pEdge = pStartEdge;
					continue;
				}

				rY1 = gpPDC->Context.LastYRegion;
			}
		}
		else
		{
			/* ASSERT the regions to avoid crashing later in dregion.c */
			ASSERT(rX0 >= gpPDC->Context.FirstXRegion);
			ASSERT(rY0 >= gpPDC->Context.FirstYRegion);
			ASSERT(rX1 <= gpPDC->Context.LastXRegion);
			ASSERT(rY1 <= gpPDC->Context.LastYRegion);
		}

		#if CHUNKING

			puPrev = &uPreviousIndices[uCurrentType & 1];

		#endif

		if (uDefaultType == PACKED_TYPE_TRANSLUCENT)
		{
			if (!uDoneFirst[uCurrentType & 1])
			{
				uCurrentType |= PACKED_TRANSTYPE_SETMARK;
				uDoneFirst[uCurrentType & 1] = TRUE;
			}

		#if CHUNKING

			else 
			{				
				sgl_int32 nXORResult;

				nXORResult = (puPrev[0] == uCurrentIndices[0])
								+ (puPrev[0] == uCurrentIndices[1])
								+ (puPrev[0] == uCurrentIndices[2])
								+ (puPrev[1] == uCurrentIndices[0])
								+ (puPrev[1] == uCurrentIndices[1])
								+ (puPrev[1] == uCurrentIndices[2])
								+ (puPrev[2] == uCurrentIndices[0])
								+ (puPrev[2] == uCurrentIndices[1])
								+ (puPrev[2] == uCurrentIndices[2]);

				nXORResult --;
				nXORResult &= PACKED_TRANSTYPE_SETMARK;

				uCurrentType |= nXORResult;
			}

			puPrev[0] = uCurrentIndices[0];
			puPrev[1] = uCurrentIndices[1];
			puPrev[2] = uCurrentIndices[2];

		#endif
		}
		
		pTri->reg.u = ENCODE_OBJXYDATA( uCurrentType, rX0, rY0, rX1, rY1 );

		uCurrentType = uDefaultType;
		
		pTri->TSPControlWord = 	gpPDC->TSPControlWord;
		pTri->BaseColour = 		gpPDC->pV0->u32Colour;

		if ( pAddfn != NULL )
		{
			/* Call the type specific thing here */
			pAddfn( pTri );
		}
						
		if ( ProcessFlatTexFn != NULL )
		{
			/* Texture stuff in line */
			ProcessFlatTexFn( pTri );
		}
			
		/* Next material structure */
		gpMatCurrent = ++pMat;

		if ( ++pTri == &gpTri[IBUFFERSIZE] )
		{
			/* Filled up the buffer! */
			break;
		}
	}

	/* Return number in intermediate buffer this time */
	return( (int) ( pTri - gpTri ) );

} /* ProcessD3DStripTransCore */
#endif

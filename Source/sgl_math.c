/*************************************************************************
* Name : sgl_math.c
* Title :  Vector maths utility functions 
* Author : Stel Michael
* Created : 7/06/1995
*
* Copyright	: 1995-2022 Imagination Technologies (c)
* License	: MIT
*
* Description :	 Utility routines for vector maths operations
				 commonly used within sgl.
*
* Platform : ANSI compatible
*
* Modifications:-
* $Log: sgl_math.c,v $
 * Revision 1.30  1997/06/04  15:29:46  gdc
 * removed compiler warning
 *
 * Revision 1.29  1997/05/13  17:56:13  mjg
 * Replaced PVROS memory management calls with SGL.
 *
 * Revision 1.28  1997/05/02  07:42:44  erf
 * Removed UINT32's etc and replaced with sgl_uint32's etc.
 *
 * Revision 1.27  1997/04/30  19:21:47  jop
 * Put Ed's changes back to how they were
 *
 * Revision 1.26  1997/04/29  14:51:59  erf
 * Added new PVROS API function call mechanism.
 *
 * Revision 1.25  1997/04/02  21:29:50  mjg
 * Updated calling conventions.
 *
 * Revision 1.24  1997/03/27  12:36:08  erf
 * Operating system independent memory allocation/deallocation macros.
 * 1. Added #include "pvrosapi.h" to include new macros.
 * 2. Changed malloc, calloc, realloc and free to PVROSMalloc, PVROSCalloc,
 *    PVROSRealloc and PVROSFree.
 *
 * Revision 1.23  1997/02/10  13:32:23  sjf
 * Added "negative scaling" flag to transformation matrices.
 *
 * Revision 1.22  1996/10/02  16:39:01  msh
 * Floating point mods for MIDAS Arcade
 *
 * Revision 1.21  1996/07/22  11:55:11  mal
 * Modified function TransformBBox() to nor use local variables
 * on machines without FP registers.
 *
 * Revision 1.20  1996/07/05  08:01:00  sjf
 * Minor optimisation for pentium in matrix code.
 *
 * Revision 1.19  1996/04/24  13:51:18  sjf
 * To get better overlap of FP instructions (well with the ICL compiler) added
 *  in some temporary variables, which convinces the compiler that source and
 * destination dont overlap in memory.
 *
 * Revision 1.18  1995/09/28  16:07:12  sjf
 * Changed sgl_rand to return a positive long rather than an unsigned
 * long as it was tricky to actually use the result correctly.
 *
 * Revision 1.17  1995/09/08  15:38:44  sjf
 * I moved some code so had to put in a definition.
 *
 * Revision 1.16  1995/09/06  14:55:37  sjf
 * Changed fabs to sfabs.
 *
 * Revision 1.15  1995/09/04  13:19:11  jop
 * Added fast inverse square root functions
 *
 * Revision 1.14  1995/08/09  17:15:48  sm
 * added some inverse transformation routine for vectors
 *
 * Revision 1.13  1995/08/08  15:13:56  jop
 * Cured a preprocessor bug
 *
 * Revision 1.12  1995/08/04  14:42:09  sjf
 * 1) Added TransformInvert and Descale routines.
 * 2) Added inverse rescale value to the transform routines.
 *
 * Revision 1.11  1995/07/30  11:21:51  sjf
 * Created two types of Bounding box - code updated to reflect this.
 *
 * Revision 1.10  1995/07/28  15:34:42  jrc
 * Added ClipBBox.
 *
 * Revision 1.9  1995/07/27  08:38:58  sjf
 * Fixed bug in matrix multiply (translation part was wrong).
 *
 * Revision 1.8  1995/07/23  15:43:47  sjf
 * Fixed INLINING bumpf.
 *
 * Revision 1.7  1995/07/16  14:12:54  jop
 * Corrected non-inline setup
 *
 * Revision 1.6  1995/06/30  11:03:05  sjf
 * Moved functions that can be inlined into their own file, and added consts
 * to some more parameters.
 *
 * Revision 1.5  1995/06/26  11:22:13  pdl
 * Added the random number generator.
 *
 * Revision 1.4  1995/06/23  18:58:27  sjf
 * 1) Added const to parameters where appropriate.
 * 2) Added parameter check to cross product routine.
 * 3) Added Transform * transform mulitply (moved from rntransf.c)
 * 4) Added transform bounding box routine.
 *
 * Revision 1.3  1995/06/16  10:56:12  sm
 * added vector and direction vector
 * transformation routines
 *
 * Revision 1.2  1995/06/15  12:36:35  sm
 * added cross product function
 *
 * Revision 1.1  1995/06/07  17:22:13  sm
 * Initial revision
 *
*
**************************************************************************/

#include <math.h>
#include "sgl_defs.h"
#include "dlnodes.h"
#include "nm_intf.h"
#include "dlglobal.h"
#include "sgl_init.h"
#include "sgl_math.h"
#include "pvrosapi.h"
#include "sglmem.h"

/*
// INCLUDE FUNCTIONS WHICH ARE OPTIONALLY INLINED WHEN WE AREN'T INLINING
*/
#if !INLINING
#define STATIC_IN 

#include "sgl_mat2.h"

#endif


/**************************************************************************
 * Function Name  : TransformDirVector
 * Inputs         : Matrix,Vec2 
 * Outputs        : Res
 * Input/Output	  :
 * Returns        : 
 * Global Used    : None
 * Description    : Transforms a vector by a given transformation matrix 
 *
 **************************************************************************/

void  TransformDirVector( const TRANSFORM_STRUCT * Matrix, 
						  const sgl_vector Vec, 
								sgl_vector Res)

{
	/* local copy of Vec for when Vec and Res are same */
	register float x,y,z;

	/*
	// results: This makes the compiler realise there is no overlap between the
	// matrix and the results, thus allowing greater overlap of operations.
	*/
	float xres,yres,zres; 
						  
	x = Vec[0];
	y = Vec[1];
	z = Vec[2];

	xres = x * Matrix->mat[0][0] + y * Matrix->mat[0][1] + z * Matrix->mat[0][2];
	yres = x * Matrix->mat[1][0] + y * Matrix->mat[1][1] + z * Matrix->mat[1][2];
	zres = x * Matrix->mat[2][0] + y * Matrix->mat[2][1] + z * Matrix->mat[2][2];

	Res[0] = xres;
	Res[1] = yres;
	Res[2] = zres;
}


/**************************************************************************
 * Function Name  : InvTransformDirVector
 * Inputs         : Matrix,Vec2 
 * Outputs        : Res
 * Input/Output	  :
 * Returns        : 
 * Global Used    : None
 * Description    : Transforms a vector by the inverse of  given transformation matrix 
 *                  : Uses inverse of the transpose of the inverse, which is just the transpose.				
 **************************************************************************/

void  InvTransformDirVector( const TRANSFORM_STRUCT * Matrix, 
						  const sgl_vector Vec, 
								sgl_vector Res)

{
	register float x,y,z; /* local copy of Vec for when Vec and Res are same */

	/*
	// results: This makes the compiler realise there is no overlap between the
	// matrix and the results, thus allowing greater overlap of operations.
	*/
	float xres,yres,zres; 

	x = Vec[0];
	y = Vec[1];
	z = Vec[2];


	xres = x * Matrix->mat[0][0] + y * Matrix->mat[1][0] + z * Matrix->mat[2][0];
	yres = x * Matrix->mat[0][1] + y * Matrix->mat[1][1] + z * Matrix->mat[2][1];
	zres = x * Matrix->mat[0][2] + y * Matrix->mat[1][2] + z * Matrix->mat[2][2];

	Res[0] = xres;
	Res[1] = yres;
	Res[2] = zres;

}


/**************************************************************************
 * Function Name  : TransformVector
 * Inputs         : Matrix,Vec2 
 * Outputs        : Res
 * Input/Output	  :
 * Returns        : 
 * Global Used    : None
 * Description    : Transforms a vector by a given transformation matrix 
 *
 **************************************************************************/

void  TransformVector( const TRANSFORM_STRUCT * Matrix, 
					   const sgl_vector	Vec, 
							 sgl_vector Res)

{
	register float x,y,z; /* local copy of Vec for when Vec and Res are same */

	/*
	// results: This makes the compiler realise there is no overlap between the
	// matrix and the results, thus allowing greater overlap of operations.
	*/
	float xres,yres,zres; 

	x = Vec[0];
	y = Vec[1];
	z = Vec[2];

	xres = x * Matrix->mat[0][0] + y * Matrix->mat[0][1] + z * Matrix->mat[0][2]
			 + Matrix->mat[0][3];
	yres = x * Matrix->mat[1][0] + y * Matrix->mat[1][1] + z * Matrix->mat[1][2]
			 + Matrix->mat[1][3];
	zres = x * Matrix->mat[2][0] + y * Matrix->mat[2][1] + z * Matrix->mat[2][2]
			 + Matrix->mat[2][3];

	Res[0] = xres;
	Res[1] = yres;
	Res[2] = zres;
}


/**************************************************************************
 * Function Name  : InvTransformVector
 * Inputs         : Matrix,Vec2 
 * Outputs        : Res
 * Input/Output	  :
 * Returns        : 
 * Global Used    : None
 * Description    : Transforms a vector by a given transformation matrix 
 *
 **************************************************************************/

void  InvTransformVector( const TRANSFORM_STRUCT * Matrix, 
					   const sgl_vector	Vec, 
							 sgl_vector Res)

{
	register float x,y,z; /* local copy of Vec for when Vec and Res are same */

	/*
	// results: This makes the compiler realise there is no overlap between the
	// matrix and the results, thus allowing greater overlap of operations.
	*/
	float xres,yres,zres; 

	x = Vec[0];
	y = Vec[1];
	z = Vec[2];

	xres = x * Matrix->inv[0][0] + y * Matrix->inv[0][1] + z * Matrix->inv[0][2]
			 + Matrix->inv[0][3];
	yres = x * Matrix->inv[1][0] + y * Matrix->inv[1][1] + z * Matrix->inv[1][2]
			 + Matrix->inv[1][3];
	zres = x * Matrix->inv[2][0] + y * Matrix->inv[2][1] + z * Matrix->inv[2][2]
			 + Matrix->inv[2][3];

	Res[0] = xres;
	Res[1] = yres;
	Res[2] = zres;

}


/**************************************************************************
 * Function Name  : TransformMultiply
 * Inputs         : Mat1, Mat2
 * Outputs        : Result
 * Input/Output	  :
 * Returns        : None
 * Global Used    : None
 * Description    : Multiplies two matrices together, giving a third.
 *					The result matrix CAN be the same as one of the source 
 *					matrices.
 *
 **************************************************************************/
extern	void	TransformMultiply( const TRANSFORM_STRUCT * Trans1, 
								   const TRANSFORM_STRUCT * Trans2,
										 TRANSFORM_STRUCT * Result)

{
	/*
	// Row and Column Index Counters
	*/
	int Row, Col;

	/*
	// 	local values used to prevent destruction of the input values
	// 	as we overwrite them.
 	*/
	TRANSFORM_STRUCT  Local;


	
	/*
	// Store values that are used multiple times in loops locally.
	// Hopefully these will be put in registers, or at least somewhere
	// quick to access
	*/
	#if MULTI_FP_REG
	float Trans1Mat_Row_0,  Trans1Mat_Row_1, Trans1Mat_Row_2;
	float Trans2Inv_Row_0,	Trans2Inv_Row_1, Trans2Inv_Row_2;
	#endif

	/*
	// Keep track of the scaling behaviour of the matrix
	*/
	Local.has_neg_scaling = Trans1->has_neg_scaling ^ Trans2->has_neg_scaling;

	switch(Trans1->scale_flag)
	{
		/*
		// If arbitrary scale, then the result must have arbitrary scaling
		*/
		case arbitrary_scale:
			 	 /* we dont care about rescale!!! */
				Local.scale_flag=arbitrary_scale;
			break;

		/* 
		// transform 1 has uniform scale 
		*/
		case uniform_scale:

			/* 
			// given transform has arbitrary scaling 
			*/
 			if (Trans2->scale_flag==arbitrary_scale) 	
			{
				Local.scale_flag=arbitrary_scale;
			}
			else if (Trans2->scale_flag==uniform_scale)			
			{
				/* given transform also has uniform scaling */
				Local.rescale	= Trans1->rescale * Trans2->rescale;
				Local.invrescale= Trans1->invrescale * Trans2->invrescale;
				Local.scale_flag=uniform_scale;
  			} 
			/* Transform 2 has no scale */
			else
			{
				Local.rescale	=Trans1->rescale;
				Local.invrescale=Trans1->invrescale;
				Local.scale_flag=uniform_scale;
			}
			break;
			
		/* 
		// transform 1 has NO scale 
		*/
	 	case no_scale:
			if (Trans2->scale_flag==arbitrary_scale) 
			{
				/* given transform has arbitrary scaling */
				Local.scale_flag=arbitrary_scale;
			}
			else if (Trans2->scale_flag==uniform_scale)			
			{
				Local.scale_flag = uniform_scale;
				Local.rescale	 = Trans2->rescale;
				Local.invrescale = Trans2->invrescale;
			}
			else
			{
				Local.rescale	=1.0f;
				Local.invrescale=1.0f;
				Local.scale_flag=no_scale;
			}
			
			break;

#if DEBUG
		default:
			ASSERT(FALSE); /*this is an illegal value!*/
			break;
#endif /*DEBUG*/

	} /*end switch*/

	/*
	// Do the actual matrix multiplication
	*/
	for(Row = 0; Row < 3; Row ++)
	{
	#if MULTI_FP_REG
		/*
		// Load up commonly used values
		*/
		Trans1Mat_Row_0 =  Trans1->mat[Row][0];
		Trans1Mat_Row_1 =  Trans1->mat[Row][1];
		Trans1Mat_Row_2 =  Trans1->mat[Row][2];

		Trans2Inv_Row_0 = Trans2->inv[Row][0];
		Trans2Inv_Row_1 = Trans2->inv[Row][1];
		Trans2Inv_Row_2 = Trans2->inv[Row][2];
	#else
		/*
		// If on Pentium, creating local FP values is
		// probably pointless - #define them instead
		*/
		#define Trans1Mat_Row_0  Trans1->mat[Row][0]
		#define Trans1Mat_Row_1  Trans1->mat[Row][1]
		#define Trans1Mat_Row_2  Trans1->mat[Row][2]

		#define Trans2Inv_Row_0	 Trans2->inv[Row][0]
		#define Trans2Inv_Row_1  Trans2->inv[Row][1]
		#define Trans2Inv_Row_2  Trans2->inv[Row][2]
		 
	#endif

		for(Col = 0; Col < 3; Col ++)
		{
			/*
			//Define local variables so compiler knows there can
			//be no memory aliasing
			*/
			float tmp1, tmp2;

			/*
				unroll the innermost loop to do scale/rotation part
				and for inverse as well

				NOTE: Because the local mat is "local" the compiler should
				know it can overlap the two calculations.
			*/

			tmp1 = 
				 Trans1Mat_Row_0 * Trans2->mat[0][Col] +
				 Trans1Mat_Row_1 * Trans2->mat[1][Col] +
				 Trans1Mat_Row_2 * Trans2->mat[2][Col];


			tmp2 = 
				 Trans2Inv_Row_0 * Trans1->inv[0][Col] +
				 Trans2Inv_Row_1 * Trans1->inv[1][Col] +
				 Trans2Inv_Row_2 * Trans1->inv[2][Col];

			Local.mat[Row][Col] = tmp1;
			Local.inv[Row][Col] = tmp2;
		}; /*end for Col*/



		/*
			Do the translation part
		*/
		Local.mat[Row][3] = 
			 Trans1Mat_Row_0 * Trans2->mat[0][3] +
 			 Trans1Mat_Row_1 * Trans2->mat[1][3] +
			 Trans1Mat_Row_2 * Trans2->mat[2][3] +
			 Trans1->mat[Row][3];


		/*
			***************************************************
			And the inverse translation
			But this may not be needed !!!!!!!!!!
			***************************************************
		*/

		Local.inv[Row][3] = 
			Trans2Inv_Row_0 * Trans1->inv[0][3] +
			Trans2Inv_Row_1 * Trans1->inv[1][3] +
			Trans2Inv_Row_2 * Trans1->inv[2][3] +
			Trans2->inv[Row][3];

		#if ! MULTI_FP_REG
		#undef Trans1Mat_Row_0
		#undef Trans1Mat_Row_1
		#undef Trans1Mat_Row_2

		#undef Trans2Inv_Row_0
		#undef Trans2Inv_Row_1
		#undef Trans2Inv_Row_2
		#endif

	} /*end for Row */


	/*
	//	now copy the local into the result structure.
	*/

	*Result = Local;

}

/**************************************************************************
 * Function Name  : TransformInvert
 * Inputs         : 
 * Outputs        : 
 * Input/Output	  : Mat
  * Returns        : None
 * Global Used    : None
 * Description    : Takes a matrix and inverts it.
 *
 **************************************************************************/
extern void TransformInvert(TRANSFORM_STRUCT * Mat)
{
	float a,b,c,d;
	int i;


	/*
	// Swap over the matrix and inverse matrix bits of the transform
	*/
	for(i = 0; i < 3; i++)
	{
		a = Mat->mat[i][0];
		b = Mat->mat[i][1];
		c = Mat->mat[i][2];
		d = Mat->mat[i][3];


		Mat->mat[i][0]= Mat->inv[i][0];
		Mat->mat[i][1]= Mat->inv[i][1];
		Mat->mat[i][2]= Mat->inv[i][2];
		Mat->mat[i][3]= Mat->inv[i][3];


		Mat->inv[i][0] = a;
		Mat->inv[i][1] = b;
		Mat->inv[i][2] = c;
		Mat->inv[i][3] = d;
	}/*end for*/


	/*
	// swap the rescale values - the scale_flag stays the same
	*/
	a = Mat->rescale;
	Mat->rescale = Mat->invrescale;
	Mat->invrescale = a;

}


/**************************************************************************
 * Function Name  : TransformDescale
 * Inputs         : 
 * Outputs        : 
 * Input/Output	  : Mat
 * Returns        : None
 * Global Used    : None
 * Description    : Takes a matrix and "removes" the scaling information
 *					except for reflections
 *
 **************************************************************************/
extern	void	TransformDescale(TRANSFORM_STRUCT * Mat)
{

	/*
	// squares of the x,y & Z "columns"
	*/
	float XSq, YSq, ZSq;

	/*
	// the "roots" of the above
	*/
	float XMag, YMag, ZMag;

	/*
	// the inverses of the above
	*/
	float InvXMag, InvYMag, InvZMag;

	int i;

	/*
	// Clear out the scaling flag and rescale values
	*/
	Mat->scale_flag = no_scale;
	Mat->rescale	= 1.0f;
	Mat->invrescale = 1.0f;

	/*
	// Get the scaling factors
	*/
	#define SQ(x) ((x)*(x))

	XSq = SQ(Mat->mat[0][0]) + SQ(Mat->mat[1][0]) + SQ(Mat->mat[2][0]);
	YSq = SQ(Mat->mat[0][1]) + SQ(Mat->mat[1][1]) + SQ(Mat->mat[2][1]);
	ZSq = SQ(Mat->mat[0][2]) + SQ(Mat->mat[1][2]) + SQ(Mat->mat[2][2]);

	#undef SQ

	/*
	// Get the square roots and inverses of these.
	//
	// NOTE: If there is an efficient inverse square root function, then
	// it is more efficient to use that instead... 
	*/
	#if defined(INVSQRT)
		InvXMag = sInvsqrt(XSq);
		InvYMag = sInvsqrt(YSq);
		InvZMag = sInvsqrt(ZSq);

		XMag = XSq * InvXMag;
		YMag = YSq * InvYMag;
		ZMag = ZSq * InvZMag;
		
	#else
		XMag = ssqrt(XSq);
		YMag = ssqrt(YSq);
		ZMag = ssqrt(ZSq);

		InvXMag = 1.0f / XMag;
		InvYMag = 1.0f / YMag;
		InvZMag = 1.0f / ZMag;

	#endif

	/*
	// Multiply the matrix by the x,y & z scaling factors
	*/
	for(i = 0; i < 3; i++)
	{
		Mat->mat[i][0] *= InvXMag;
		Mat->mat[i][1] *= InvYMag;
		Mat->mat[i][2] *= InvZMag;

		Mat->inv[0][i] *= XMag;
		Mat->inv[1][i] *= YMag;
		Mat->inv[2][i] *= ZMag;

	}/*end for*/

	/*
	// Do the translation bit of the inverse as well
	*/
	Mat->inv[0][3] *= XMag;
	Mat->inv[1][3] *= YMag;
	Mat->inv[2][3] *= ZMag;


	
}/*end function*/


/**************************************************************************
 * Function Name  : TransformBBox
 * Inputs         : Mat, BBox
 * Outputs        : Result
 * Input/Output	  :
 * Returns        : None
 * Global Used    : None
 * Description    : Multiplies a matrix by a bounding box, giving a transformed
 *					bounding box.
 *					NOTE The input bounding box must be in the "centre" format
 *					while the result is in the min max format, because that
 *					is more convenient after transformation.
 *
 **************************************************************************/
extern	void	TransformBBox( const TRANSFORM_STRUCT * Matrix, 
							   const BBOX_CENT_STRUCT * BBox,
									 BBOX_MINMAX_STRUCT * Result)

{

#if !MULTI_FP_REG
	/*
	//		 Use FP registers (where available) to speed it up.
	*/
	#define centreX (BBox->boxCentre[0])
	#define centreY (BBox->boxCentre[1])
	#define centreZ (BBox->boxCentre[2])
	#define offsetX (BBox->boxOffsets[0])
	#define offsetY (BBox->boxOffsets[1])
	#define offsetZ (BBox->boxOffsets[2])
	#define matXA   (Matrix->mat[0][0])
	#define matXB   (Matrix->mat[0][1])
	#define matXC   (Matrix->mat[0][2])
	#define matYA   (Matrix->mat[1][0])
	#define matYB   (Matrix->mat[1][1])
	#define matYC   (Matrix->mat[1][2])
	#define matZA   (Matrix->mat[2][0])
	#define matZB   (Matrix->mat[2][1])
	#define matZC   (Matrix->mat[2][2])

	float ResultCentre, ResultOffset;

	/*
	// Do the X components
	*/
	ResultCentre = centreX * matXA +  centreY * matXB + centreZ * matXC +
							 Matrix->mat[0][3];
	ResultOffset = sfabs(offsetX * matXA) +  sfabs(offsetY * matXB) + 
							sfabs(offsetZ * matXC);

	Result->boxMin[0]  = ResultCentre - ResultOffset;
	Result->boxMax[0]  = ResultCentre + ResultOffset;


	/*
	// Do the Y components
	*/
	ResultCentre  = centreX * matYA +  centreY * matYB + centreZ * matYC +
							 Matrix->mat[1][3];

	ResultOffset = sfabs(offsetX * matYA) +  sfabs(offsetY * matYB) + 
							sfabs(offsetZ * matYC);

	Result->boxMin[1]  = ResultCentre - ResultOffset;
	Result->boxMax[1]  = ResultCentre + ResultOffset;

	/*
	// Do the Z components
	*/
	ResultCentre  = centreX * matZA +  centreY * matZB + centreZ * matZC +
							 Matrix->mat[2][3];

	ResultOffset = sfabs(offsetX * matZA) +  sfabs(offsetY * matZB) + 
							sfabs(offsetZ * matZC);

	Result->boxMin[2]  = ResultCentre - ResultOffset;
	Result->boxMax[2]  = ResultCentre + ResultOffset;

	#undef centreX 
	#undef centreY 
	#undef centreZ 
	#undef offsetX 
	#undef offsetY 
	#undef offsetZ 
	#undef matXA   
	#undef matXB   
	#undef matXC   
	#undef matYA   
	#undef matYB   
	#undef maYYC   
	#undef matZA   
	#undef matZB   
	#undef matZC   

#else
	/*
	// local "copies" of the bounding box values to
	//		 1) prevent overwriting when the result and the BBox are the same
	//		 2) use FP registers (where available) to speed it up.
	*/
	float centreX, centreY, centreZ;
	float offsetX, offsetY, offsetZ;

	float ResultCentre, ResultOffset;

	/*
	// local copy of some of the matrix values, as these get used twice
	*/
	float matA, matB, matC;
	
	centreX = BBox->boxCentre[0];
	centreY = BBox->boxCentre[1];
	centreZ = BBox->boxCentre[2];

	offsetX = BBox->boxOffsets[0];
	offsetY = BBox->boxOffsets[1];
	offsetZ = BBox->boxOffsets[2];

	/*
	// Do the X components
	*/
	matA = Matrix->mat[0][0];
	matB = Matrix->mat[0][1];
	matC = Matrix->mat[0][2];

	ResultCentre = centreX * matA +  centreY * matB + centreZ * matC +
							 Matrix->mat[0][3];
	ResultOffset = sfabs(offsetX * matA) +  sfabs(offsetY * matB) + 
							sfabs(offsetZ * matC);

	Result->boxMin[0]  = ResultCentre - ResultOffset;
	Result->boxMax[0]  = ResultCentre + ResultOffset;


	/*
	// Do the Y components
	*/
	matA = Matrix->mat[1][0];
	matB = Matrix->mat[1][1];
	matC = Matrix->mat[1][2];

	ResultCentre  = centreX * matA +  centreY * matB + centreZ * matC +
							 Matrix->mat[1][3];

	ResultOffset = sfabs(offsetX * matA) +  sfabs(offsetY * matB) + 
							sfabs(offsetZ * matC);

	Result->boxMin[1]  = ResultCentre - ResultOffset;
	Result->boxMax[1]  = ResultCentre + ResultOffset;

	/*
	// Do the Z components
	*/
	matA = Matrix->mat[2][0];
	matB = Matrix->mat[2][1];
	matC = Matrix->mat[2][2];

	ResultCentre  = centreX * matA +  centreY * matB + centreZ * matC +
							 Matrix->mat[2][3];

	ResultOffset = sfabs(offsetX * matA) +  sfabs(offsetY * matB) + 
							sfabs(offsetZ * matC);

	Result->boxMin[2]  = ResultCentre - ResultOffset;
	Result->boxMax[2]  = ResultCentre + ResultOffset;
#endif

}


/******************************************************************************
 * Function Name: ClipMMBBox
 * Inputs       : pBox, fClipZ
 * Outputs      : pBox
 * Returns      : -
 * Globals Used : -
 * Description  : Given a Min Max bounding box that crosses the given foreground
 *				  clipping plane this routine clips the box so that it is
 *				  completely beyond (but touching) the clipping plane.
 *****************************************************************************/
extern	void	ClipMMBBox( BBOX_MINMAX_STRUCT *pBox, float fClipFrontZ)
{
	/* Ensure we have a bounding box that crosses the clipping plane: */
	ASSERT(pBox->boxMin[2] < fClipFrontZ &&
		   pBox->boxMax[2] > fClipFrontZ);

	pBox->boxMin[2] = fClipFrontZ;
}


static sgl_uint32 RandomNumber=0; /* the random number generator's state variable*/

/******************************************************************************
 * Function Name: sgl_rand
 *
 * Inputs       : -
 * Outputs      : -
 * Returns      : RandomNumber
 * Globals Used : RandomNumber
 *
 * Description  : A portable L.C.G. random number generator taken from
 *				  Numerical Recipies. It does assume the unsigned 32bit mul
 *				  does behave correctly. ie returns the bottom 32bits of a
 *				  64Bit result. With a seed of 0, the next few numbers should
 *				  be 0x3C6EF35F, 0x47502932, 0xD1CCF6E9, and 0xAAF95334.
 *				  INTERNALLY! To make it easier to use, we mask off the top
 *				  bit so that we can return it as a positive signed integer.
 *****************************************************************************/
extern long CALL_CONV sgl_rand()
{
	const long TOPBIT = 0x80000000L;

	RandomNumber=1664525UL*RandomNumber + 1013904223UL;

	return(RandomNumber & ~TOPBIT);
}

/******************************************************************************
 * Function Name: sgl_srand
 *
 * Inputs       : Seed
 * Outputs      : -
 * Returns      : -
 * Globals Used : RandomNumber
 *
 * Description  : Seeds the random number generator. It can take any value
 *****************************************************************************/
extern void CALL_CONV sgl_srand(unsigned long Seed)
{	
	RandomNumber=Seed;
}

/******************************************************************************/
/*                    Fast inverse square root function                       */
/*                                                                            */
/*	                       Source: graphics gems 5                            */
/******************************************************************************/

#include <stdlib.h>

unsigned char *iSqrt = NULL;

/******************************************************************************
 * Function Name: MakeInvSqrtLookupTable
 *
 * Inputs       : -
 * Outputs      : -
 * Returns      : -
 * Globals Used : iSqrt
 *
 * Description  : prepares the InvSqrt lookup table
 *****************************************************************************/
void MakeInvSqrtLookupTable (void)
{
    register long f;
    register unsigned char *h;
    union
	{
		sgl_int32 i;
		float f;
	} fi, fo;

    iSqrt = SGLMalloc (TABLE_SIZE);

	ASSERT (iSqrt);
	
    h = iSqrt;

    for (f = 0, h = iSqrt; f < TABLE_SIZE; f++)
	{
        fi.i = ((EXP_BIAS-1) << EXP_POS) | (f << LOOKUP_POS);
	  #if defined (MIDAS_ARCADE)
        fo.f = 1.0f / ssqrt(fi.f);
	  #else
        fo.f = 1.0f / (float)sqrt(fi.f);
	  #endif
        *h++ = ((fo.i + (1<<(SEED_POS-2))) >> SEED_POS) & 0xFF; /* rounding */
    }

	iSqrt[TABLE_SIZE / 2] = 0xFF;    /* Special case for 1.0 */
}

/*---------------------------- End of File -------------------------------*/


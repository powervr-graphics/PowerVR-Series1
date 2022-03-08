/*************************************************************************
* Name : sgl_math.h
* Title :  Vector maths utility functions 
* Author : Stel Michael
* Created : 7/06/1995
*
* Copyright	: 1995-2022 Imagination Technologies (c)
* License	: MIT
*
* Description :	prototypes for sgl_math utility vector routines 
*
* Platform : ANSI compatible
*
* Modifications:-
* $Log: sgl_math.h,v $
 * Revision 1.22  1996/05/14  12:48:40  sjf
 * 1) added prototype for "fast" float to int conversion
 * 2) Changed some "double" consts to float consts.
 *
 * Revision 1.21  1995/09/30  19:57:38  jop
 * Gave non-GCC compilers a chance to inline properly
 *
 * Revision 1.20  1995/09/23  15:09:19  sjf
 * changed static inline to extern inline for correct
 * behaviour of gcc 2.7.0
 *
 * Revision 1.19  1995/09/15  18:21:05  jrc
 * Added prototype for InvTransformVector.
 *
 * Revision 1.18  1995/09/08  15:39:15  sjf
 * Part of optimisations.
 *
 * Revision 1.17  1995/09/04  13:19:33  jop
 * Added defines and statics for fast inverse sqrt function
 *
 * Revision 1.16  1995/08/24  12:12:40  pdl
 * changed an int to a void.
 *
 * Revision 1.15  1995/08/23  15:58:39  jop
 * Added VecNegate function
 *
 * Revision 1.14  1995/08/15  11:37:53  pdl
 * added some PI defines.
 *
 * Revision 1.13  1995/08/11  15:08:46  jop
 * Added VecCopy
 *
 * Revision 1.12  1995/08/04  14:42:44  sjf
 * Added Descale and Invert routines.
 *
 * Revision 1.11  1995/08/02  15:40:51  jop
 * Corrected bug stopping sfabs from appearing in non-inline code
 *
 * Revision 1.10  1995/08/02  11:32:09  pdl
 * included singmath.h
 *
 * Revision 1.9  1995/07/30  11:21:51  sjf
 * Created two types of Bounding box - code updated to reflect this.
 *
 * Revision 1.8  1995/07/28  15:34:42  jrc
 * Added ClipBBox.
 *
 * Revision 1.7  1995/07/23  15:44:00  sjf
 * Fixed INLINING bumpf.
 *
 * Revision 1.6  1995/07/16  14:13:46  jop
 * Corrected VecSub and VecAdd non-inline prototypes
 *
 * Revision 1.5  1995/06/30  11:03:43  sjf
 * Added INLINE capability for the small functions, and extra "const" qualifiers.
 *
 * Revision 1.4  1995/06/23  18:48:39  sjf
 * 1) Added const to parameters where applicable
 * 2) Added Transform Multiply routine.
 * 3) Added transform bounding box routine.
 *
 * Revision 1.3  1995/06/16  10:57:17  sm
 * added prototypes for vector and dir vector
 * transform routines
 *
 * Revision 1.2  1995/06/15  12:36:12  sm
 * added cross product func
 *
 * Revision 1.1  1995/06/07  17:22:50  sm
 * Initial revision
 *
*
**************************************************************************/

/*
** math.h does not have a standard pi define, so here it is 
*/

#define	SGL_PI		3.14159265358979323846f
#define	SGL_PI_2   	1.57079632679489661923f
#define	SGL_2_PI   	6.283185307f


/*****************************************************************************
// External prototypes - only wanted if NOT inlining
*/

#include <math.h>
#include "singmath.h"

#if !INLINING
extern	float	InvSqrt (float x);

extern  float	ApproxRecip (const float fVal);

extern	void	VecNormalise(sgl_vector	Vec);	
extern	void	VecNegate(sgl_vector Vec);	
extern	float	SqrVecLength(const sgl_vector Vec);
extern	float	VecLength(const sgl_vector Vec);

extern	void   	VecCopy(const sgl_vector Src, sgl_vector Dst);
extern	void	VecSub(const sgl_vector Vec1, const sgl_vector Vec2, sgl_vector Result);
extern	void	VecAdd(const sgl_vector Vec1, const sgl_vector Vec2, sgl_vector Result);

extern	float 	DotProd(const sgl_vector Vec1, const sgl_vector Vec2);

extern	void 	CrossProd(const sgl_vector Vec1, const sgl_vector Vec2, sgl_vector Vec3);


#if SLOW_FTOI
extern long		FastSmallFloatToIntUnMasked(float val);
#endif

#else
/*
// Define the routines as inline
*/
#if defined(GCC)
#define STATIC_IN INLINE extern
#else
#define STATIC_IN INLINE static
#endif

#include "sgl_mat2.h"
#undef STATIC_IN

#endif




/*
// Define the larger maths functions
*/

extern	void	TransformDirVector( const TRANSFORM_STRUCT * Matrix, 
									const sgl_vector Vec, 
										  sgl_vector Result);

extern	void	TransformVector( const TRANSFORM_STRUCT * Matrix, 
								 const sgl_vector Vec, 
									   sgl_vector Result);

extern	void	InvTransformVector( const TRANSFORM_STRUCT * Matrix, 
									const sgl_vector Vec, 
									sgl_vector Result);

extern	void	TransformMultiply( const TRANSFORM_STRUCT * Mat1, 
								   const TRANSFORM_STRUCT * Mat2,
								   TRANSFORM_STRUCT * Result);

extern	void	TransformDescale(TRANSFORM_STRUCT * Mat);

extern	void	TransformInvert(TRANSFORM_STRUCT * Mat);

extern	void	TransformBBox( const TRANSFORM_STRUCT * Mat, 
							   const BBOX_CENT_STRUCT * BBox,
							   BBOX_MINMAX_STRUCT * Result);

extern	void	ClipMMBBox( BBOX_MINMAX_STRUCT *pBox, float fClipFrontZ);

/* this must be called before using InvSqrt function! */
extern	void	MakeInvSqrtLookupTable (void);

/*------------------------------- End of File -------------------------------*/

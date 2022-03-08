/*************************************************************************
* Name : transfor.h
* Title : Transform Struct definition
* Author : Simon Fenney
* Created : 1/05/1995
*
* Copyright	: 1995-2022 Imagination Technologies (c)
* License	: MIT
*
* Description : Definition of the common transform structure which
*				is used throughout SGL.
*				Most of this is covered in TRDD027x.doc
*
* Platform : ANSI compatible
*
* Modifications:-
* $Log: transfor.h,v $
 * Revision 1.7  1997/02/10  13:55:42  sjf
 * Added "has neg sacling" flag to the transformation matrices.
 *
 * Revision 1.6  1995/08/04  14:43:33  sjf
 * Added inverse of rescale.
 *
 * Revision 1.5  1995/07/30  11:22:37  sjf
 * Created two types of bounding box, because of efficiency concerns.
 *
 * Revision 1.4  1995/06/23  15:23:32  sjf
 * Added a bounding box structure type.
 *
 * Revision 1.3  1995/05/26  14:44:50  sjf
 * Fixed filename to be VideoLogic standard, i.e. 8.3
 *
*
*
*
**************************************************************************/

/*
// define the types of scaling that could occur with
// a transformation matrix, so that we can optimise certain
// cases.
*/
typedef enum
{
	no_scale,
	uniform_scale,
	arbitrary_scale
} SCALE_FLAG_ENUM;



typedef struct
{
	SCALE_FLAG_ENUM	 scale_flag;
	float rescale;
	float invrescale;
	int	  has_neg_scaling;

	float mat[3][4];
	float inv[3][4]; /*Note sometime the 4th column may be undefined*/

} TRANSFORM_STRUCT;


/*
// Define 2 types of bounding box - the first expressed as a centre point
//		  and offsets to the corners, and the second as the Min and Max values
//		  of the corners.
//
//	The reason for the two types is that it is more efficient to do a 
// transformation on data in the first format, while other calculations are
// cheaper with the second. It is also quite cheap to go from the first format
// to the second (3 add, 3 subs), but not so cheap the other way 
// (3 adds, 3 mults, 3 subs).
*/
typedef struct
{
	sgl_vector boxCentre;
	sgl_vector boxOffsets;

} BBOX_CENT_STRUCT;


typedef struct
{
	sgl_vector boxMin;
	sgl_vector boxMax;

} BBOX_MINMAX_STRUCT;


/*---------------------------- End of File -------------------------------*/


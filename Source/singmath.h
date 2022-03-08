/*************************************************************************
* Name : singmath.h
* Title :  Single Precision Maths Functions
* Author : Simon Fenney
* Created : 13/06/1995
*
* Copyright	: 1995-2022 Imagination Technologies (c)
* License	: MIT
*
* Description :	 Header file for singmath.c
*
* Platform : ANSI compatible
*
* Modifications:-
* $Log: singmath.h,v $
 * Revision 1.5  1997/04/03  08:38:43  rgi
 * Added ZEUS_ARCADE support
 *
 * Revision 1.4  1996/09/30  10:57:43  msh
 * Uses new fabsf and sqrtf functions for MIDASA Arcade
 *
 * Revision 1.3  1995/08/04  14:43:00  sjf
 * "added" single precision square root.
 *
 * Revision 1.2  1995/08/02  11:26:11  pdl
 * added sfabs().
 *
 * Revision 1.1  1995/06/14  09:53:52  sjf
 * Initial revision
 *
*
**************************************************************************/


extern float s_cos(float x);
extern float s_sin(float x);
extern void s_sincos(float x, float * s, float * c);


/*
// single precision fabs and sqrt
// These can be customised later wherever possible, to use in line
// assembly code.
*/

#if defined (MIDAS_ARCADE) || defined (ZEUS_ARCADE)
 float fabsf(float);
 float sqrtf(float);
 #define sfabs(x)	fabsf(x)
 #define ssqrt(x)	sqrtf(x)
#else
 #define sfabs(x)	((float)fabs(x))
 #define ssqrt(x)	((float)sqrt(x))
#endif

/*
// End of file
*/

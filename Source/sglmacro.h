/*************************************************************************
* Name : sglmacro.h
* Title : Useful set of macros for SGL
* Author : Simon Fenney
* Created : 3/05/1995
*
* Copyright	: 1995-2022 Imagination Technologies (c)
* License	: MIT
*
* Description : A set of standard macros for SGL.
*
* Platform : ANSI compatible
*
* Modifications:-
* $Log: sglmacro.h,v $
 * Revision 1.26  1997/11/24  14:56:55  gdc
 * added a define for REVERSED_SORT
 *
 * Revision 1.25  1997/09/09  18:11:34  erf
 * Created define SMALL_FLOAT_VALUE which is used to insure that
 * max UVs not zero.
 * See dtex.c, dtexnp.c, dtexml.asm and dtexnpml.asm .
 *
 * Revision 1.24  1997/09/05  16:18:35  sxy
 * Increased the value of LARGE_EPSILON from 0.1875 to 0.46875.
 *
 * Revision 1.23  1997/09/05  13:05:22  sxy
 * Added a large Epsilon to cull more small triangles.
 *
 * Revision 1.22  1997/05/02  07:45:14  erf
 * Removed UINT32's etc and replaced with sgl_uint32's etc.
 *
 * Revision 1.21  1997/04/04  13:06:09  jop
 * Removed implicit #include "pvrosapi.h" and NEW macro. People
 * should now explicitly use PVROSAPI from malloc etc.
 *
 * Revision 1.20  1997/03/27  19:23:43  erf
 * Removed defines to monitor memory management.
 *
 * Revision 1.19  1997/03/27  12:43:30  erf
 * Operating system independent memory allocation/deallocation macros.
 *  1. Added #include "pvrosapi.h" to include new macros.
 *  2. Changed malloc, calloc, realloc and free to PVROSMalloc, PVROSCalloc,
 *     PVROSRealloc and PVROSFree.
 *
 * Revision 1.18  1997/02/24  11:42:27  gdc
 * chnaged the value of epsilon to reject at 0.0625(fDet). This is a pretty
 * big difference from previous, but is absolutely necessary for MonaLisa to
 * stop yucky-noise stuff when AK47 gets small - doesnt hurt D3dTest either
 * as a few more polys are rejected on basis of size.
 *
 * Revision 1.17  1997/01/10  14:12:59  sjf
 * Added another couple of macros to muck around with floats
 * as integers.
 *
 * Revision 1.16  1997/01/10  01:36:02  sjf
 * Moved in a colour avergaing macro from dtri.c as another file needed it.
 *
 * Revision 1.15  1997/01/07  19:42:12  sjf
 * Added some more Float/Integer macros and values. Main one is
 * a comparison with "epsilon" a small value.
 *
 * Revision 1.14  1996/12/10  23:30:08  erf
 * Changed PVR3 to PCX2.
 *
 * Revision 1.13  1996/11/30  00:38:34  sjf
 * Added a Hex define for 0.5f so we can access it as if it were an int.
 *
 * Revision 1.12  1996/11/14  15:51:20  erf
 * Use PVR3 compile switch to use FLOAT_TO_LONG and LONG_TO_FLOAT defines for PCX2.
 *
 * Revision 1.11  1996/07/08  04:16:10  sjf
 * Added temp define to allow mem logging  of release builds.
 *
 * Revision 1.10  1996/04/23  19:00:50  sjf
 * Moved some "pentium" macros from rntex.c so they can be used
 * to the good of all.
 *
 * Revision 1.9  1996/03/22  16:52:25  sjf
 * Made mem logging work on *NIX.
 *
 * Revision 1.8  1996/03/19  17:11:27  pdl
 * included WIN32 into the condition for memory logging.
 *
 * Revision 1.7  1996/02/26  15:38:34  sjf
 * Added memory logging functions.
 *
 * Revision 1.6  1995/09/25  15:50:01  pdl
 * added the undefs for min and max.
 *
 * Revision 1.5  1995/08/11  15:09:11  jop
 * Added APPROXIMATELY
 *
 * Revision 1.4  1995/07/26  16:30:07  jop
 * Added choose min and max macros
 *
 * Revision 1.3  1995/06/05  14:35:29  sjf
 * Stole JimP's CLAMP macro out of dlmater.c and put in in here for the
 * greater good of all mankind. Also added MIN and MAX macros.
 *
 * Revision 1.2  1995/05/26  14:44:17  sjf
 * Fixed file name to be VL standard. i.e. 8.3
 *
 * Revision 1.1  1995/05/09  16:19:32  sjf
 * Initial revision
 *
*
*
**************************************************************************/

/*
// Clamp value to lie between min and max, and return result. x is not
// actually modified.
*/
#define CLAMP(x,min,max)  (((x) < (min)) ? (min) : (((x) > (max)) ? (max) : (x)))

/*
// The HP has a MAX and MIN defined already.
*/

#ifdef MAX
#undef MAX
#endif

#ifdef MIN
#undef MIN
#endif

#define MIN(a,b)  (((a) < (b)) ? (a) : (b))
#define MAX(a,b)  (((a) > (b)) ? (a) : (b))

/* macros to put the larger/smaller of 2 variables into the first */
#define CHOOSE_MAX(x,y)	if(y>x){x=y;}
#define CHOOSE_MIN(x,y)	if(y<x){x=y;}

/* a convenient way to see if a variable is outside a range of values */
#define APPROXIMATELY(x,y,z)    (((x) < (y)) && ((x) > (z)))



/*
// Put in some macros
// These are also used to manipulate floats and integers, in the case
// where fp compare is slow!!
//
// Also, because PCX2 takes FP as standard input, we want access to the
// macros which pretend Floats are integers
*/
#if PCX2 || (SLOW_FCMP && !MULTI_FP_REG)

#define FABS_MASK 0x7FFFFFFFl
#define FSIGN_BIT 0x80000000l
#define IEEE_HALF 0x3F000000l				/*This SHOULD be 0.5f*/
#define FLOAT_TO_LONG(x) (* ((long *)( & x)))
#define LONG_TO_FLOAT(x) (* ((float*)( & x)))


#endif


/* Define a small floating point value incase the UV values are
 * the same resulting in a 1/0 execution. See dtex.c, dtexnp.c,
 * dtexml.asm and dtexnpml.asm.
 */
#define SMALL_FLOAT_VALUE	0x1e3ce508	/* 2^-20	*/


/*
// Define an "epsilon" value for doing tests to prevent division by Zero.
// By making this a power of 2, we can make the value only occupy the top
// 16bits of the float. This makes it more efficient as a "immediate" data
// on MIPS processors, and is not detrimental to, say, Pentium.
//
// Define the value as both floating point AND equiv int value.
//
// The value is 2^-60
*/
/*
**#define EPSILON_AS_FLOAT	(8.673617385e-19f)
**#define EPSILON_AS_I32		(0x21800000l)
*/
/* 
** For the purposes of fixing the noisy bits in MonaLisa
** Epsilon has been increased to 0.0625f (2^-4). This will cause
** Substantially more polys to be rejected - which might actually
** speed up D3dtest, but if holes start appearing in things, then 
** this is probably the cause - try reverting to above values
*/
#define EPSILON_AS_FLOAT	(0.0625f)
#define EPSILON_AS_I32		(0x3D800000l)

/*
// Define a comparison macro to return whether a value is less than
// Epsilon (note this does not mean its absolute value). 
// It will use integer on SLOW_FCMP type cpus and normal float stuff
// otherwise.
// Please Note: It may still be better to use FP stuff in some cases
// since it may save storing values in mem and re-reading them back.
// If the value has to be stored, then the integer one is probably the
// one to use.
*/
#if (SLOW_FCMP && !MULTI_FP_REG)
#define LESS_THAN_EPSILON(X) (FLOAT_TO_LONG(X) < EPSILON_AS_I32)
#else
#define LESS_THAN_EPSILON(X) ((X) < EPSILON_AS_FLOAT)
#endif


/* large Epsilon version */
#define LARGE_EPSILON_AS_FLOAT	(0.46875f)
#define LARGE_EPSILON_AS_I32	(0x3EF00000l)

#if (SLOW_FCMP && !MULTI_FP_REG)
#define LESS_THAN_LARGE_EPSILON(X) (FLOAT_TO_LONG(X) < LARGE_EPSILON_AS_I32)
#else
#define LESS_THAN_LARGE_EPSILON(X) ((X) < LARGE_EPSILON_AS_FLOAT)
#endif

/*
// Define a NEGATE macro for negating FP values which uses integers
// when the FP unit is not brilliant. NOTE:  This is ONLY useful if
// the values are sitting out in mem AND NOT in the FP unit. Make sure
// this is the case before you use it, otherwise just use "-" :)
//
// Note also that it can't work on expressions, only variables.
*/
#if (SLOW_FCMP && !MULTI_FP_REG)
#define FP_NEGATE(X) {FLOAT_TO_LONG(X) ^= FSIGN_BIT;}
#else
#define FP_NEGATE(X)	{X = - X;}
#endif


/*
// Define a FP comparsion macro for comparing either 2 POSITIVE float values
// OR a positive and a negative float. It could get the answer wrong if they
// are both negative. (In some cases that is not a problem)
//
// On decent processors it will use the normal floating point compare, otherwise
// it will retype them as int.
//
// NOTE the parameters must be addressable variables and NOT expressions.
*/
#if (SLOW_FCMP && !MULTI_FP_REG)
#define POS_FP_CMP(X, Y) (FLOAT_TO_LONG(X) < FLOAT_TO_LONG(Y))
#else
#define POS_FP_CMP(X, Y) ((X) < (Y))
#endif

/*
// Another Hacky macro: Retype a Float as an Int ONLY if the 
// we have the slow floating point case
*/
#if (SLOW_FCMP && !MULTI_FP_REG)
#define F_TO_I_IF_FP_SLOW(X) (FLOAT_TO_LONG(X))
#else
#define F_TO_I_IF_FP_SLOW(X) (X)
#endif

/*
// Average 3 24bit Packed RBG colours (sgl_uint32s) to produce one single
// "16 bit" colour in 555 format with the Colour in bits 30-16 and the
// other bits 0.
//
// IE    XXXXXXXX RRRRRRRR GGGGGGGG BBBBBBBB  x 3   (X= Dont care)
//
// is averaged to produce
//
//			0 RRRRR GGGGG BBBBB 00000000 00000000
//
// This is the format we need for smooth shading.
//
// We do this calculation by doing Red and Blue at the same time.
// There's a bit of a description in a similar macro in dtri.c
//														SJF.
*/
#define AVERAGE_RBG_TO_555_TOP(RESULT, SRC0, SRC1, SRC2)	\
{															\
	sgl_uint32 RedNBlue, GreenNTop;								\
	RedNBlue   = SRC0;										\
	GreenNTop  = SRC0 & 0xFF00FF00;							\
	RedNBlue  += SRC1;										\
	GreenNTop += SRC1 & 0xFF00FF00;							\
	RedNBlue  += SRC2;										\
	GreenNTop += SRC2 & 0xFF00FF00;							\
	RedNBlue  -= GreenNTop;									\
	GreenNTop *= (85 * 4);	/*put most sig bit at 25*/		\
	RedNBlue  *= 42;		/*RED most sig bit at 30*/		\
															\
	RESULT =	(RedNBlue  & 0x7C000000)					\
			  | (GreenNTop & 0x03E00000)					\
			  |((RedNBlue  & 0xF800) << (20 - 14));			\
}

/* hidden debug sort method for transluceny - it is unsorted,
** but passes emitted in reverse order from receipt
** must not conflict with enum SGLTRANSSORT
*/
#define	REVERSED_NO_SORT (2)

/*
// End of file
*/

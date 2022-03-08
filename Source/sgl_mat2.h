/*************************************************************************
* Name : sgl_mat2.h
* Title :  Vector maths utility INLINE functions 
* Author : Stel Michael
* Created : 7/06/1995
*
* Copyright	: 1995-2022 Imagination Technologies (c)
* License	: MIT
*
* Description :	Code for maths routines that are optionally inlined.
*
*				This is either included as part of the header file
*				(with suitable "qualifiers") when inlining, or is
*				included in the main "c" file when we aren't inlining.
*				This way there is only one copy of the actual code, so
*				it should be less prone to errors.
*
*
*				A special macro "STATIC_IN" is used to switch the modes.
*
* Platform : ANSI compatible
*
* Modifications:-
* $Log: sgl_mat2.h,v $
 * Revision 1.17  1997/04/03  10:43:25  rgi
 * Ported ZEUS_ARCADE support to top level
 *
 * Revision 1.16  1997/03/31  22:10:07  jop
 * Sorted out WIN32/DOS32 issues
 *
 * Revision 1.15.1.1  1997/04/03  08:35:00  rgi
 * Added ZEUS_ARCADE support
 *
 * Revision 1.15  1996/10/02  16:38:24  msh
 * Floating point mods for MIDAS Arcade
 *
 * Revision 1.14  1996/09/30  10:58:22  msh
 * InvSqrt function modified for MIDASA Arcade
 *
 * Revision 1.13  1996/08/05  11:02:55  mal
 * reverted back tmp changes
 *
 * Revision 1.11  1996/05/14  12:46:39  sjf
 * Added a "fast" float to int conversion routine which can be used when
 * the built-in conversion is slow.
 *
 * Revision 1.10  1996/04/24  13:50:42  sjf
 *  To get better overlap of FP instructions (well with the ICL compiler) added
 * in some temporary variables, which convinces the compiler that source and
 *  destination dont overlap in memory.
 *
 * Revision 1.9  1996/04/02  12:35:57  sjf
 * Added slow FDIV value so that on MIPS etc we will use a true divide.
 *
 * Revision 1.8  1995/09/30  19:56:50  jop
 * Reconfigured to make more sense in the post-watcom owrld
 *
 * Revision 1.7  1995/09/13  18:27:43  jop
 * Added ApproxRecip function
 *
 * Revision 1.6  1995/09/08  15:37:58  sjf
 * Did some minor optimisations (after looking at assembly output) on
 * inverse square root routine.
 *
 * Revision 1.5  1995/09/04  13:18:50  jop
 * Added WATCOM style inline macros for maths functions
 *
 * Revision 1.4  1995/08/23  15:58:24  jop
 * Added VecNegate function
 *
 * Revision 1.3  1995/08/11  15:09:00  jop
 * Added VecCopy
 *
 * Revision 1.2  1995/07/23  15:43:06  sjf
 * Fixed INLINE etc so that it doesn't try to use a compiled/linked version of
 * the maths routines.
 *
 * Revision 1.1  1995/06/30  11:02:36  sjf
 * Initial revision
 *
**************************************************************************/


/******************************************************************************/
/*                         defines and typedefs for                           */
/*                    Fast inverse square root function                       */
/*                                                                            */
/*	                       Source: graphics gems 5                            */
/******************************************************************************/

/* Specified constants */

#define LOOKUP_BITS    6   /* Number of mantissa bits for lookup */
#define EXP_POS       23   /* Position of the exponent */
#define EXP_BIAS     127   /* Bias of exponent */

/* The mantissa is assumed to be just down from the exponent */

/* Derived constants */

/* Position of mantissa lookup */
#define LOOKUP_POS   (EXP_POS-LOOKUP_BITS)

/* Position of mantissa seed */
#define SEED_POS     (EXP_POS-8)

/* Number of entries in table */
#define TABLE_SIZE   (2 << LOOKUP_BITS)

/* Mask for table input */
#define LOOKUP_MASK  (TABLE_SIZE - 1)

/* Extract exponent */
#define GET_EXP(a)   (((a) >> EXP_POS) & 0xFF)

/* Set exponent */
#define SET_EXP(a)   ((a) << EXP_POS)

/* Extended mantissa MSB's */
#define GET_EMANT(a) (((a) >> LOOKUP_POS) & LOOKUP_MASK)

/* Set mantissa 8 MSB's */
#define SET_MANTSEED(a) (((unsigned long)(a)) << SEED_POS)



/* InvSqrt's lookup table */

extern unsigned char *iSqrt;



/******************************************************************************
 * Function Name: InvSqrt
 *
 * Inputs       : float x: number to find the inverse square root of
 * Outputs      : -
 * Returns      : float: 1/sqrt(x)
 * Globals Used : iSqrt
 *
 * Description  : There are 3 different versions
 *				  1) for Pentium with Watcom (hand assembled)
 *				  2) An iterative solution for machines with slow/no
 *					 SQRT and Divides.
 *				  3) A default version.
 *
 *				The first 2 use Use a seeded Newton's method 
 *				algorithm to calc 1/sqrt(x)
 *				  Taken from graphics gems 5
 *
 *	
 *****************************************************************************/
#if defined(__WATCOMC__) && INLINING

/* constants - we need these for InvSqrt lookup */
static float f3_0 = 3.0f;
static float f0_5 = 0.5f;

	/* assembler inline pragma */
	float InvSqrt (float x);
	#pragma aux InvSqrt = 					\
        "fst     DWORD PTR [esp-4]",		\
        "mov     ebx, DWORD PTR [esp-4]",	\
        "shld    ecx, ebx, 9",				\
        "shr     ebx, 17",					\
        "and     ecx, 0FFh",				\
        "and     ebx, 7Fh",					\
        "neg     ecx",						\
        "add     ebx, [iSqrt]",				\
        "add     ecx, 380",					\
        "movzx   ebx, BYTE PTR [ebx]",		\
        "shr     ecx, 1",					\
        "shl     ebx, 15",					\
        "shl     ecx, 23",					\
        "or      ecx, ebx",					\
        "mov     DWORD PTR [esp-4], ecx",	\
        "fld     DWORD PTR [esp-4]",		\
        "fld     ST",						\
        "fxch    ST(2)",					\
        "fmulp   ST(2), ST",				\
        "fmul    ST(1), ST",				\
        "fxch    ST(1)",					\
        "fsubr   DWORD PTR [f3_0]",			\
        "fmulp   ST(1), ST",				\
        "fmul    DWORD PTR [f0_5]",			\
		parm [8087]							\
		modify [ebx ecx]					\
		value [8087];



#elif SLOW_SQRT_DIV

STATIC_IN float InvSqrt (float x)
{
	union
	{
    	unsigned long    i;
		float            f;
	} seed;
	float r;

	ASSERT (iSqrt != NULL);

	/*
	// Pretend the float is an int
	*/
	seed.f = x;

    seed.i = SET_EXP (((3*EXP_BIAS-1) - GET_EXP (seed.i)) >> 1) 
		   | SET_MANTSEED (iSqrt[GET_EMANT (seed.i)]);

    /* Seed: accurate to LOOKUP_BITS */
    r = seed.f;

    /* First iteration: accurate to 2*LOOKUP_BITS */
    r = (3.0f - r * r * x) * r * 0.5f;

	#if 0
    /* Second iteration: accurate to 4*LOOKUP_BITS */
    r = (3.0f - r * r * x) * r * 0.5f;
	#endif

    return (r);
}

/*
// Else Define the default. 
*/
#else 
 #if defined (MIDAS_ARCADE)  || defined (ZEUS_ARCADE)

  STATIC_IN float InvSqrt (float x)
  {
  	return (1.0f / ssqrt(x));
  }	     

 #else

  STATIC_IN float InvSqrt (float x)
  {
  	return (1.0f / ((float)sqrt(x)));
  }	     

 #endif
#endif


/**************************************************************************
 * Function Name  : VecCopy
 * Inputs         :  
 * Outputs        : 
 * Input/Output	  : Vec		- Source
 *					Dst		- Destination
 * Returns        : None
 * Global Used    : None
 * Description    : copies source vector to dest vector
 *
 **************************************************************************/
#if defined(__WATCOMC__) && INLINING

	/* assembler inline pragma */

	void VecCopy (const sgl_vector Src, sgl_vector Dst);
	#pragma aux VecCopy = 			\
		"mov	ecx, [eax + 0]",	\
		"mov	edx, [eax + 4]",	\
		"mov	[ebx + 0], ecx",	\
		"mov	ecx, [eax + 8]",	\
		"mov	[ebx + 4], edx",	\
		"mov	[ebx + 8], ecx"		\
		parm [eax] [ebx] 			\
		modify [ecx edx];

#else

STATIC_IN void	VecCopy(const sgl_vector Src, sgl_vector Dst)
{
	/*
	// Use tmp variables to inform the compiler that the src and destination
	// do not overlap, so we can do operations in parallel
	*/
	float tmp1, tmp2, tmp3;
	tmp1 = Src[0];
	tmp2 = Src[1];
	tmp3 = Src[2];

	Dst[0] = tmp1;
	Dst[1] = tmp2;
	Dst[2] = tmp3;
}	
#endif


/**************************************************************************
 * Function Name  : VecNegate
 * Inputs         :  
 * Outputs        : 
 * Input/Output	  : Vec		- Source
 * Returns        : None
 * Global Used    : None
 * Description    : Negates given vector 
 *
 **************************************************************************/
#if defined(__WATCOMC__) && INLINING

	/* assembler inline pragma */
	void VecNegate(sgl_vector Vec);
	#pragma aux VecNegate = 			\
		"xor	[eax + 0], 80000000h",	\
		"xor	[eax + 4], 80000000h",	\
		"xor	[eax + 8], 80000000h"	\
		parm [eax];

#else
STATIC_IN void	VecNegate(sgl_vector Vec)
{
	Vec[0] = -Vec[0];
	Vec[1] = -Vec[1];
	Vec[2] = -Vec[2];
}	
#endif

/**************************************************************************
 * Function Name  : VecNormalise
 * Inputs         :  
 * Outputs        : 
 * Input/Output	  : Vec		- Vector to be normalised
 * Returns        : None
 * Global Used    : None
 * Description    : Normalises given vector 
 *
 **************************************************************************/

#if defined(__WATCOMC__) && INLINING

	/* assembler inline pragma */
	void VecNormalise_1 (sgl_vector Vec);
	#pragma aux VecNormalise_1 = 		\
        "fld     DWORD PTR [eax + 0]"	\
        "fld     DWORD PTR [eax + 4]"	\
        "fld     DWORD PTR [eax + 8]"	\
        "fld     ST(2)"					\
        "fmul    ST, ST"				\
        "fld     ST(2)"					\
        "fmul    ST, ST"				\
        "fld     ST(2)"					\
        "fmul    ST, ST"				\
        "fxch    ST(2)"					\
        "faddp   ST(1), ST"				\
        "faddp   ST(1), ST"				\
        "fst     DWORD PTR [esp-4]",	\
        "mov     ebx, DWORD PTR [esp-4]",	\
        "shld    ecx, ebx, 9",			\
        "shr     ebx, 17",				\
        "and     ecx, 0FFh",			\
        "and     ebx, 7Fh",				\
        "neg     ecx",					\
        "add     ebx, iSqrt",			\
        "add     ecx, 380",				\
        "movzx   ebx, BYTE PTR [ebx]",	\
        "shr     ecx, 1",				\
        "shl     ebx, 15",				\
        "shl     ecx, 23",				\
        "or      ecx, ebx",				\
		parm [eax]						\
		modify [ebx ecx];

	void VecNormalise_2 ();
	#pragma aux VecNormalise_2 = 			\
        "mov     DWORD PTR [esp-4], ecx",	\
        "fld     DWORD PTR [esp-4]",		\
        "fld     ST",					\
        "fxch    ST(2)",				\
        "fmulp   ST(2), ST",			\
        "fmul    ST(1), ST",			\
        "fxch    ST(1)",				\
        "fsubr   DWORD PTR [f3_0]",		\
        "fmulp   ST(1), ST",			\
        "fmul    DWORD PTR [f0_5]",		\
        "fmul    ST(1), ST",			\
        "fmul    ST(2), ST",			\
        "fxch    ST(1)",				\
        "fstp    DWORD PTR [eax + 8]",	\
        "fmulp   ST(2), ST",			\
        "fxch    ST(1)",				\
        "fstp    DWORD PTR [eax + 0]",	\
        "fstp    DWORD PTR [eax + 4]",	\
		modify [ebx ecx];

	#define VecNormalise(a) VecNormalise_1(a); VecNormalise_2();

#if 0
/*#else*/

	/* PC version is in fast.asm! */
	extern void VecNormalise (sgl_vector Vec);

/*#endif*/
#endif

#else

STATIC_IN void	VecNormalise(sgl_vector Vec)
{
	float	InvVecLen;

	#if 0

		InvVecLen = 1.0f/ 
		( (float) sqrt((double) (Vec[0]*Vec[0]+Vec[1]*Vec[1]+Vec[2]*Vec[2])));

	#else

		InvVecLen = InvSqrt (Vec[0]*Vec[0]+Vec[1]*Vec[1]+Vec[2]*Vec[2]);

	#endif
		
	Vec[0]*=InvVecLen;
	Vec[1]*=InvVecLen;
	Vec[2]*=InvVecLen;
}	

#endif



/**************************************************************************
 * Function Name  : SqrVecLength
 * Inputs         : Vec - given vector 
 * Outputs        : 
 * Input/Output	  : 
 * Returns        : square vector length
 * Global Used    : None
 * Description    : Returns the square length of a given vector 
 *
 **************************************************************************/
#if defined(__WATCOMC__) && INLINING

	/* assembler inline pragma */
	float SqrVecLength(const sgl_vector Vec);
	#pragma aux SqrVecLength = 			\
		"fld	DWORD PTR [eax + 0]",	\
		"fmul	ST, ST",				\
		"fld	DWORD PTR [eax + 4]",	\
		"fmul	ST, ST",				\
		"fld	DWORD PTR [eax + 8]",	\
		"fmul	ST, ST",				\
		"fxch	ST(2)",					\
		"faddp	ST(1), ST",				\
		"faddp	ST(1), ST"				\
		parm [eax]						\
		value [8087];

#else
STATIC_IN float SqrVecLength(const sgl_vector Vec)
{
	return (Vec[0]*Vec[0]+Vec[1]*Vec[1]+Vec[2]*Vec[2]);
} 
#endif





/**************************************************************************
 * Function Name  : VecLength
 * Inputs         : Vec		- given Vector  	 
 * Outputs        : 
 * Input/Output	  : 
 * Returns        : length of vector
 * Global Used    : None
 * Description    : Return the length of a given vector 
 *
 **************************************************************************/

STATIC_IN float VecLength(const sgl_vector Vec)
{
  #if defined (MIDAS_ARCADE)  || defined (ZEUS_ARCADE)
	return ((float)  ssqrt((float)(Vec[0]*Vec[0]+Vec[1]*Vec[1]+Vec[2]*Vec[2])));
  #else
	return ((float)  sqrt((double)(Vec[0]*Vec[0]+Vec[1]*Vec[1]+Vec[2]*Vec[2])));
  #endif
} 




/**************************************************************************
 * Function Name  : DotProd
 * Inputs         : Vec1 ,Vec2 
 * Outputs        : 
 * Input/Output	  :
 * Returns        : dot product of Vec1 and Vec2
 * Global Used    : None
 * Description    : return dot product of two given vectors
 *
 **************************************************************************/
#if defined(__WATCOMC__) && INLINING

	/* assembler inline pragma */
	float DotProd(const sgl_vector Vec1, const sgl_vector Vec2);
	#pragma aux DotProd = 				\
		"fld	DWORD PTR [eax + 0]",	\
		"fld	DWORD PTR [ebx + 0]",	\
		"fmulp	ST(1), ST",				\
		"fld	DWORD PTR [eax + 4]",	\
		"fld	DWORD PTR [ebx + 4]",	\
		"fmulp	ST(1), ST",				\
		"faddp	ST(1), ST",				\
		"fld	DWORD PTR [eax + 8]",	\
		"fld	DWORD PTR [ebx + 8]",	\
		"fmulp	ST(1), ST",				\
		"faddp	ST(1), ST"				\
		parm [eax] [ebx]				\
		value [8087];

#else

STATIC_IN float DotProd(const sgl_vector Vec1, const sgl_vector Vec2)
{
	return (Vec1[0] * Vec2[0] + Vec1[1] * Vec2[1] + Vec1[2] * Vec2[2]);
}

#endif




/**************************************************************************
 * Function Name  : CrossProd
 * Inputs         : Vec1 ,Vec2 
 * Outputs        : 
 * Input/Output	  : Vec3
 * Returns        : 
 * Global Used    : None
 * Description    : returns cross product of two given vectors in Vec3
 *
 *					NOTE: The result MUST NOT be the same as either of the
 *					input vectors
 *
 **************************************************************************/
#if defined(__WATCOMC__) && INLINING

	/* assembler inline pragma */
	void CrossProd(const sgl_vector Vec1, const sgl_vector Vec2, sgl_vector Vec3);
	#pragma aux CrossProd = 			\
		"fld	DWORD PTR [eax + 4]",		/*	0			*/	\
		"fmul	DWORD PTR [ebx + 8]",		/*	0			*/	\
		"fld	DWORD PTR [eax + 8]",		/*	1	0		*/	\
		"fmul	DWORD PTR [ebx + 4]",		/*	1	0		*/	\
		"fld	DWORD PTR [eax + 8]",		/*	2	1	0	*/	\
		"fmul	DWORD PTR [ebx + 0]",		/*	2	1	0	*/	\
		"fxch	ST(2)",						/*	0	1	2	*/	\
		"fsubrp	ST(1), ST",					/*	0	2		*/	\
		"fld	DWORD PTR [eax + 0]",		/*	3	0	2	*/	\
		"fmul	DWORD PTR [ebx + 8]",		/*	3	0	2	*/	\
		"fxch	ST(1)",						/*	0	3	2	*/	\
		"fstp	DWORD PTR [ecx + 0]",		/*	3	2		*/	\
		"fld	DWORD PTR [eax + 0]",		/*	4	3	2	*/	\
		"fmul	DWORD PTR [ebx + 4]",		/*	4	3	2	*/	\
		"fxch	ST(2)",						/*	2	3	4	*/	\
		"fsubrp	ST(1), ST",					/*	2	4		*/	\
		"fld	DWORD PTR [eax + 4]",		/*	5	2	4	*/	\
		"fmul	DWORD PTR [ebx + 0]",		/*	5	2	4	*/	\
		"fxch	ST(1)",						/*	2	5	4	*/	\
		"fstp	DWORD PTR [ecx + 4]",		/*	5	4		*/	\
		"fsubp	ST(1), ST",					/*	4			*/	\
		"fstp	DWORD PTR [ecx + 8]"		/*	-			*/	\
		parm [eax] [ebx] [ecx];

#else



STATIC_IN void CrossProd(const sgl_vector Vec1, const sgl_vector Vec2, sgl_vector Vec3)

{
	/*
	// Use tmp variables to inform the compiler that the src and destination
	// do not overlap, so we can do operations in parallel
	*/
	float tmp1, tmp2, tmp3;

	/*
	// check that parameters are distinct
	*/
	ASSERT(Vec1 != Vec3)
	ASSERT(Vec2 != Vec3)

	/*
	// Do cross prod
	*/
	tmp1 = (Vec1[1] * Vec2[2]) - (Vec1[2] * Vec2[1]);
	tmp2 = (Vec1[2] * Vec2[0]) - (Vec1[0] * Vec2[2]);
	tmp3 = (Vec1[0] * Vec2[1]) - (Vec1[1] * Vec2[0]);

	Vec3[0] =tmp1; 
	Vec3[1] =tmp2;
	Vec3[2] =tmp3;
}
#endif






/**************************************************************************
 * Function Name  : VecSub
 * Inputs         : Vec1 ,Vec2 
 * Outputs        : 
 * Input/Output	  : Vec3
 * Returns        : 
 * Global Used    : None
 * Description    : Does Vec1 - Vec2 putting result in Vec3
 *
 **************************************************************************/
#if defined(__WATCOMC__) && INLINING

	/* assembler inline pragma */
	void VecSub(const sgl_vector Vec1, const sgl_vector Vec2, sgl_vector Result);
	#pragma aux VecSub = 			\
		"fld	DWORD PTR [eax + 0]",	\
		"fsub	DWORD PTR [ebx + 0]",	\
		"fld	DWORD PTR [eax + 4]",	\
		"fsub	DWORD PTR [ebx + 4]",	\
		"fld	DWORD PTR [eax + 8]",	\
		"fsub	DWORD PTR [ebx + 8]",	\
		"fxch	ST(2)",					\
		"fstp	DWORD PTR [ecx + 0]",	\
		"fstp	DWORD PTR [ecx + 4]",	\
		"fstp	DWORD PTR [ecx + 8]"	\
		parm [eax] [ebx] [ecx];

#else
STATIC_IN void	VecSub(const sgl_vector Vec1, const sgl_vector Vec2, sgl_vector Result)
{
	/*
	// Use tmp variables to inform the compiler that the src and destination
	// do not overlap, so we can do operations in parallel
	*/
	float tmp1, tmp2, tmp3;


	tmp1 = Vec1[0] - Vec2[0];
	tmp2 = Vec1[1] - Vec2[1];
	tmp3 = Vec1[2] - Vec2[2];

	Result[0] = tmp1;
	Result[1] = tmp2;
	Result[2] = tmp3; 
}
#endif

/**************************************************************************
 * Function Name  : VecAdd
 * Inputs         : Vec1 ,Vec2 
 * Outputs        : 
 * Input/Output	  : Vec3
 * Returns        : 
 * Global Used    : None
 * Description    : Does Vec1 + Vec2 putting result in Vec3
 *
 **************************************************************************/
#if defined(__WATCOMC__) && INLINING

	/* assembler inline pragma */
	void VecAdd (const sgl_vector Vec1, const sgl_vector Vec2, sgl_vector Result);
	#pragma aux VecAdd = 			\
		"fld	DWORD PTR [eax + 0]",	\
		"fadd	DWORD PTR [ebx + 0]",	\
		"fld	DWORD PTR [eax + 4]",	\
		"fadd	DWORD PTR [ebx + 4]",	\
		"fld	DWORD PTR [eax + 8]",	\
		"fadd	DWORD PTR [ebx + 8]",	\
		"fxch	ST(2)",					\
		"fstp	DWORD PTR [ecx + 0]",	\
		"fstp	DWORD PTR [ecx + 4]",	\
		"fstp	DWORD PTR [ecx + 8]"	\
		parm [eax] [ebx] [ecx];

#else
STATIC_IN void	VecAdd(const sgl_vector Vec1, const sgl_vector Vec2, sgl_vector Result)
{
	/*
	// Use tmp variables to inform the compiler that the src and destination
	// do not overlap, so we can do operations in parallel
	*/
	float tmp1, tmp2, tmp3;


	tmp1 = Vec1[0] + Vec2[0];
	tmp2 = Vec1[1] + Vec2[1];
	tmp3 = Vec1[2] + Vec2[2];

	Result[0] = tmp1;
	Result[1] = tmp2;
	Result[2] = tmp3; 

}
#endif

/**************************************************************************
 * Function Name  : ApproxRecip
 * Inputs         : float fVal
 * Outputs        : 
 * Input/Output	  : 
 * Returns        : float <> 1/fVal
 * Global Used    : None
 * Description    : Does an approximate reciprocal of a float.
 *				  : error:  0% <= err <= 12.5%
 *
 **************************************************************************/
#if defined(__WATCOMC__) && INLINING

	/* assembler inline pragma */
	float ApproxRecip (const float fVal);
	#pragma aux ApproxRecip =			\
		"mov	ebx, eax"				\
		"and	eax, 7F800000h"			\
		"neg	eax"					\
		"and	ebx, 807FFFFFh"			\
		"add	eax, 7E800000h"			\
		"xor	ebx, 007FFFFFh"			\
		"and	eax, 7F800000h"			\
		"or		eax, ebx"				\
		parm [eax]						\
		modify [ebx]					\
		value struct no8087;
	
#elif SLOW_FDIV
STATIC_IN float ApproxRecip (const float fVal)
{
	union
	{
		float f;
		long  l;
	} fl;
	long	exp;
	
	fl.f = fVal;
	
	/* get exponent, negate it and sub 1 */
	
	exp = fl.l & (0xFFL << 23);
	exp = ((253L << 23) - exp) & (0xFFL << 23);

	/* and out exponent, preserve sign bit and 1's complement mantissa */

	fl.l &= 0x807FFFFF;
	fl.l ^= 0x007FFFFF;

	/* recombine */

	fl.l |= exp;

	return (fl.f);
}
#else

STATIC_IN float ApproxRecip (const float fVal)
{
	return (1.0f/fVal);
}
#endif





/**************************************************************************
 * Function Name  : FastSmallFloatToIntUnMasked
 * Inputs         : float Val
 * Outputs        : 
 * Input/Output	  : 
 * Returns        : The equivalent integer in the LOWER 22 (?)bits. NOTE
 *				    THE UPPER BITS are RUBBISH and must be masked off in
 *					some manner.
 * Global Used    : None
 * Description    : For intel processors (and perhaps others) this does
 *					a float to int conversion that should run faster (in
 *					the long run) than the actual float to int conversion.
 *
 *					NOTE: The rounding mode of the processor affects this
 *					routine. Unlike C convention, this will truncate ONLY
 *					if the processor is in truncate mode.
 *
 *					It uses a large value to add to our "integer" which
 *					causes the integer to be shifted down into the right
 *					place in the mantissa. We can then mask off the upper
 *					bits (which contain the exponent etc). We dont do the
 *					masking here, as it would be pointless in cases where
 *					we shift the value and hence lose them anyway.
 *
 **************************************************************************/

#if SLOW_FTOI

STATIC_IN long	FastSmallFloatToIntUnMasked(float val)
{
	float result;
	/*
	// The following is 1.5 * 2^23
	*/
	result = val + (1.5f * 8388608.0f);

	return *((long *) (&result));
}

#endif



/*
// END OF FILE
*/ 


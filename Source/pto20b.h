/**************************************************************************
 * Name         : pto20b.h
 * Title        : Pack to 20 bit routine.
 * Author       : Simon Fenney
 * Created      : 28th Nov 1996
 *
 * Copyright	: 1995-2022 Imagination Technologies (c)
 * License		: MIT
 *
 * Description  : This is the Pack to 20 bit routine, ripped out of pkisp.c
 *				  (and where it was duplicated in disp.c) so that there is
 *				  only one copy, that can be either inlined or copied as
 *				  required.
 *
 *				  You need to define the macro "INLINE_P20" to either a null
 *				  string OR "static INLINE" etc.
 *
 * Platform     : ANSI
 *								 
 * Modifications:-
 * $Log: pto20b.h,v $
 * Revision 1.6  1997/07/14  19:30:52  mjg
 * Added the PCX2_003 compile switch.
 *
 * Revision 1.5  1997/05/02  07:45:14  erf
 * Removed UINT32's etc and replaced with sgl_uint32's etc.
 *
 * Revision 1.4  1997/04/22  17:21:19  mjg
 * Added a redundant return value to remove PCX2 compiler warnings.
 *
 * Revision 1.3  1996/12/10  23:33:34  erf
 * Changed compile switches:
 * 1. PVR1 to ISPTSP.
 * 2. PVR2 to PCX1.
 * 3. PVR3 to PCX2.
 *
 * Revision 1.2  1996/12/02  08:59:40  sjf
 * Rob Isherwoord pointed out that taking a pointer of a parameter stops
 * the compiler from inlining - oops - so I've changed the "float to long"
 * conversion back to flong, in pack to 20 bit.
 *
 * Revision 1.1  1996/11/28  15:56:03  sjf
 * Initial revision
 *
 *************************************************************************/



/**************************************************************************
 * Function Name  : PackTo20Bit
 * Inputs         : Value 
 * Outputs        : 
 * Input/Output	  : packed 20bit number
 * Returns        :
 * Global Used    : 
 * Description    : Take a floating point number and packs it into a floating 
 *					20 bit value with a 4 bit exponent and 16 bit 2's complement 
 *					mantissa, as used by Sabre.
 *
 *					NOTE: There are two versions of this routine. One which
 *					assumes IEEE floats, and a more general, but slower, one
 *					which does not. By default we use the IEEE version.
 *				   
 **************************************************************************/
/*
// Define the largest exponent the SABRE hardware can handle
*/
#define MAX_LEGAL_20_EXPONENT (14)


/*
// Define whether we should have an extra 1/2 bits accuracy
// when changing from IEEE 24 bit mantissa to the 16 bit 
// mantissa of the Sabre Float
*/
#define USE_ROUNDING 0

/*///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////// 
// IEEE version 
///////////////////////////////////////////////////////////////////// 
////////////////////////////////////////////////////////////////// */


#define IEEE_SIGN_BIT (0x80000000L)

/*
// Mantissa is in lower 23 bits
*/
#define IEEE_MANTISSA_MASK		((1L<<23)-1)
#define IEEE_ASSUMED_MANTISSA_BIT (1L<<23L)

/*
// Values for getting the IEEE exponent
*/
#define IEEE_E_MASK  (0xFF)
#define IEEE_E_SHIFT (23)

/*
// Define how much we have to shift down the IEEE mantissa
// to get the right number of bits for the 20bit float
// NOTE this INCLUDES the assumed Most sig bit.
*/
#define IEEE_TO_20_SHIFT (24 - 15)

/*
// Define how to round from the IEEE float to the 20 bit float.
// the rounding bit is the bit JUST after the where the
// 20 bit mantissa runs out
*/
#define MANTISSA_ROUND (1L<<(IEEE_TO_20_SHIFT - 1))

/*
// Define the maximum mantissa we can hold, so we can reset a value
// if we get an overflow in rounding.
//
// Also define which bit will get set if we get an overflow
*/
#define MAX_MANTISSA    ((0x8000L << (IEEE_TO_20_SHIFT)) - 1)
#define MANTISSA_OVERFLOW (0x8000L << (IEEE_TO_20_SHIFT))

/*
// A mask so we can negate JUST the mantissa
*/
#define MASK_20_MANTISSA (0xFFFFL)  /* 16 bits */

/*
// Define the IEEE exponent which is effectively the same
// as a "zero" exponent in our 20bit "sabre floats"
*/
#define IEEE_E_SHIFT_DOWN (102)

/*
// Define range of exponents we can handle
*/
#define IEEE_E_SMALLEST  (IEEE_E_SHIFT_DOWN - 15)
#define IEEE_E_LARGEST	 (IEEE_E_SHIFT_DOWN + MAX_LEGAL_20_EXPONENT )

/*
// Where to put the exponent in the 20 bit number
*/
#define SHIFT_20_EXP (16)

/*
// Where the sign bit is for 20 bit S float
*/
#define SIGN_BIT_FOR_20BIT_POS (15)
#define SIGN_BIT_FOR_20BIT	   (1<<SIGN_BIT_FOR_20BIT_POS)



INLINE_P20 sgl_int32 PackTo20Bit(const float	Value)
{

	/*
	// We need to "retype" the floating point value as an Integer
	// so we can muck around with the bits. The fastest method seems
	// to depend on the compiler/CPU.
	*/
	union
	{
		sgl_uint32 ui;
		float f;
	}flong;

	sgl_uint32 intVal;
	int exp;
	int mantissa;

	/*SGL_TIME_START(PACK_20BIT_TIME) Don't monitor this as it
							 will distort the figures very badly */

	/*
	// Get the bits of the float
	*/
	flong.f = Value;
	intVal	= flong.ui;


	/*/////////
	// get the IEEE exponent, and then get it into our range.
	///////// */ 
	exp = ((intVal >> IEEE_E_SHIFT) & IEEE_E_MASK) - IEEE_E_SHIFT_DOWN;

	ASSERT((exp + IEEE_E_SHIFT_DOWN)<= IEEE_E_LARGEST);
	
	/*/////////
	// Get the mantissa, and whack on the "understood" extra
	// bit. This is present for most numbers, except for zero,
	// and VERY SMALL denormalised numbers, which we handle
	// as a special case anyway.
	///////// */ 
	mantissa = (intVal & IEEE_MANTISSA_MASK) | IEEE_ASSUMED_MANTISSA_BIT;

	/*/////////
	// Determine how to treat the number based on the 
	// exponent
	//
	// Now the most common case is likely to be when the exponent is
	// of a reasonable size:i.e. our 20 bit number won't be "de-normalised"
	///////// */
	if(exp >= 0)
	{
		/* //////////////
		// I don't there is much point in doing the rounding personally.
		// It does give more accuracy .. but at a cost!
		// Usually the USE_ROUNDING will be set to 0.   SJF
		////////////// */
		#if USE_ROUNDING
			/*
			// As we are truncating down to 15 bits, we should round
			// by adding.
			// Must check for overflow.
			*/
			mantissa = mantissa + MANTISSA_ROUND;

			if(mantissa & MANTISSA_OVERFLOW)
			{
				mantissa >>= 1;
				exp += 1;
				DPF((DBG_MESSAGE, "Doing Overflow 1 in Pack"));
				/*
				// If this overflows our exponent, then stop it!
				*/
				if(exp > MAX_LEGAL_20_EXPONENT)
				{
					exp = MAX_LEGAL_20_EXPONENT;

					mantissa = MAX_MANTISSA;

					DPF((DBG_MESSAGE, "Doing Overflow 2 in Pack"));
				}/*end if exponent too large*/
			}/*end if mantissa overflow*/

		#endif USE_ROUNDING
		/* ////////////// */

		/*
		// Get the top 15 bits of the mantissa - NOTE the
		// mantissa from the IEEE is now effectively 24 bit
		*/
		mantissa >>= IEEE_TO_20_SHIFT;

		/*
		// negate the mantissa if need be
		*/
		#if ISPTSP
			if( ((sgl_int32)intVal) < 0 )
			{
				mantissa = (-mantissa) & MASK_20_MANTISSA;
			}
		#elif PCX1 || PCX2 || PCX2_003
			/*
			// Do this with branch free code, by shifting the sign bit
			// of the "float" down to the sign position of the 20 bit.
			*/
			mantissa |= (intVal >> (31 - SIGN_BIT_FOR_20BIT_POS)) &
								SIGN_BIT_FOR_20BIT;
		#else
		   ARGGHHHH
		#endif


		/*
		// Check we haven't been given an invalid number
		*/
		ASSERT(exp <= MAX_LEGAL_20_EXPONENT);
		ASSERT(exp >= 0);

		/*
		// Combine exponent and mantissa
		*/
		mantissa = (exp << SHIFT_20_EXP) | mantissa;

		/*SGL_TIME_STOP(PACK_20BIT_TIME) */

		return mantissa;
	}
	/*
	// Else the number is either TOO small for us to represent as anything
	// other than 0 OR we must put it in a de-normalised format.
	*/
	else
	{
		/*
		// Is it less than we can possibly represent?
		// Then represent it as 0.
		*/
		if(exp < (IEEE_E_SMALLEST  - IEEE_E_SHIFT_DOWN))
		{
			return (0);
		}
		/*
		// Else the number is too small to use the upper bits of our
		// 20 bit mantissa, so shift down by the required amount.
		//
		// The 20 bit exponent will be zero
		*/
		else
		{
			/*
			// Shift down by the denormalising amount.
			*/ 
			mantissa >>= (- exp);
		
			/*
			// Add the rounding bit - note we don't have to worry about
			// overflow because we are going to shift down anyway
			*/
			mantissa = mantissa + MANTISSA_ROUND;

			/*
			// Shift the number down by the remaining amount
			// to the right place.
			*/
			mantissa >>= IEEE_TO_20_SHIFT;


			/*
			// negate the mantissa if need be
			*/
			#if ISPTSP
				if( ((sgl_int32)intVal) < 0 )
				{
					mantissa = (-mantissa) & MASK_20_MANTISSA;
				}
			#elif PCX1 || PCX2 || PCX2_003
				/*
				// Do this with branch free code, by shifting the sign bit
				// of the "float" down to the sign position of the 20 bit.
				*/
				mantissa |= (intVal >> (31 - SIGN_BIT_FOR_20BIT_POS)) &
								SIGN_BIT_FOR_20BIT;
			#else
				ARGGHHHH
			#endif

			return mantissa;
		}/*end if choose between  0 or denormaised*/
		
	}/*end if/else choose between denormalised and normal numbers*/

}	



/**************************************************************************
 * Function Name  : NegatePacked20Bit   (LOCAL ONLY)
 * Inputs         : Value 
 * Outputs        : Negated  20bit SFLOAT
 * Input/Output	  : 
 * Returns        :
 * Global Used    : 
 * Description    :  Negates a 20 bit sabre "float"
 *				   
 **************************************************************************/

static INLINE sgl_int32 NegatePacked20Bit(sgl_int32 val)
{
	#if ISPTSP
		int mantissa;
		/*
		// negate the mantissa part, and throw away the top exponent bits
		*/
		mantissa =  ( - val ) & MASK_20_MANTISSA;

		/*
		// Add the mantissa back on to the original exponent bits
		*/
		return (val & ~ MASK_20_MANTISSA) | mantissa;

	#elif PCX1

		/*
		// On PCX1 simply toggle the sign bit
		*/
		return ((sgl_int32) (((sgl_uint32) val) ^ SIGN_BIT_FOR_20BIT));

	#else
		return (0); 			
	#endif
}



/*
// End of file
*/


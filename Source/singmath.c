/*************************************************************************
* Name : singmath.c
* Title :  Singl Precision Maths Functions
* Author : Simon Fenney
* Created : 13/06/1995
*
* Copyright	: 1995-2022 Imagination Technologies (c)
* License	: MIT
*
* Description :	 Versions of some of the maths functions
*				 specifically coded for single precision.
*
* Platform : ANSI compatible
*
* Modifications:-
* $Log: singmath.c,v $
 * Revision 1.5  1997/03/31  23:41:40  mlj
 * Fixes to get DSO32 driver to build
 *
 * Revision 1.4  1997/03/31  22:10:46  jop
 * Sorted out WIN32/DOS32 issues
 *
 * Revision 1.3  1997/02/05  12:28:27  sjf
 * Fixed problem with Win32 builds: The compiler options use
 * a "fast float to int" conversion that doesn't explicitly
 * set the rounding mode to Truncate on the Pentium. By default
 * this means you get Round to nearest rather than truncate.
 * Originally we were setting this flag in the Pentium for
 * all of SGL, but this was changed to just the render loops.
 *
 * The mode is now also (temporarily) set for the sin and cos
 * routines, which means they now work a LOT more accurately!
 *
 * Revision 1.2  1996/10/02  13:41:41  sjf
 * Changed fabs to sfabs
 *
 * Revision 1.1  1995/06/14  09:53:40  sjf
 * Initial revision
 *
*
**************************************************************************/

#include <stdio.h>
#include <math.h>

#include "sgl_defs.h"

#include "singmath.h"


/*
// Define the following to be 1 to include some crap test prog code,
// and 0 to NOT include it.
*/
#define TESTCODE 0


#if 0
/*
// Under windows, we fiddle with the int-conversion of the compiler, and
// as such, must set the FPU appropriately. This WAS being set all the time
// but got changed to just around the display list traversal etc. 
// We, however, strictly need Truncate and not Round when converting
// floats to ints
*/
#if WIN32 || DOS32
#define DO_FPU_PRECISION TRUE
extern	void SetupFPU (void);
extern	void RestoreFPU (void);

#else
#define DO_FPU_PRECISION FALSE
#endif
#endif




/*
// Define constants used: i.e. PI etc PLUS Taylor series values used to
// compute Sine and Cosine.
*/
#define pi		(3.141592654f)
#define inv_pi  (1.0f / pi)
#define pi_on_2 (pi * 0.5f)

/*
// define the taylor terms for cosine
*/
#define ct1 (-1.0f)
#define ct3 ( 1.0f / 6.0f)
#define ct5 (-1.0f / 120.0f)
#define ct7 ( 1.0f / 5040.0f)
#define ct9 (-1.0f / 362880.0f)

/*
// ..and for sine
*/
#define st2 (-1.0f / 2.0f)
#define st4 ( 1.0f / 24.0f)
#define st6 (-1.0f / 720.0f)
#define st8 ( 1.0f / 40320.0f)

/**************************************************************************
 * Function Name  : s_cos
 * Inputs         : angle x in radians
 * Outputs        : NONE
 * Input/Output	  : NONE
 * Returns        : cos of x
 * Global Used    : NONE
 *
 * Description    : Computes Cos in single precision
 **************************************************************************/


float s_cos(float x)
{
	
	int n;
	float x_sq;
	float res;

	#if DO_FPU_PRECISION
		SetupFPU ();
	#endif

	/*
	// Cos(-x)==Cos(x)
	*/
	x = sfabs(x);
	
	/*
	// Cos(N Pi + x) = (-1)^N Cos(x)
	// Where N is an integer
	*/
	n = (int)(x * inv_pi);
	x = x - ((float) n * pi);
	
	/*
	// So x should now lie in the range [0 , Pi]
	//
	// Now the taylor series for COS about the point PI/2
	// which is in the middle of this range is...
	//
	// let y = x -  Pi/2
	//
	// cos(x) = -y + y^3/3! - y^5/5! + y^7 /7! - y^9/9!
	//
	// Which when factored ....
	//    y(-1.0 + y^2 (1/3! + y^2(-1/5! + y^2(1/7! +(y^2 * -1/9!)))));
	*/
	x = x - pi_on_2;
	x_sq = x * x;
	
	res = x * (ct1 + x_sq*(ct3 + x_sq * (ct5 + x_sq *(ct7 +  ct9 * x_sq))));

	/*
	// Keep within COS's legal range (as we seem to sometimes 
	// have an overflow)
	*/
	if(res > 1.0f)
	{
		res = 1.0f;
	}
	else if(res < -1.0f)
	{
		res = -1.0f;
	}


	/*
	// Now if n is Odd, then negate the result
	*/
	if(n & 0x1)
	{
		res = -res;
	}

	#if DO_FPU_PRECISION
		RestoreFPU ();
	#endif

    return (res);     
}


/**************************************************************************
 * Function Name  : s_sin
 * Inputs         : angle x in radians
 * Outputs        : NONE
 * Input/Output	  : NONE
 * Returns        : sin of x
 * Global Used    : NONE
 *
 * Description    : Computes Sin in single precision
 **************************************************************************/

float s_sin(float x)
{

	int negate;

	int n;
	float x_sq;
	float res;

	#if DO_FPU_PRECISION
		SetupFPU ();
	#endif
	
	/*
	// Sin(-x)== -Sin(x)
	*/
	negate = (x < 0.0f);
	x = sfabs(x);
	
	/*
	// Sin(N Pi + x) = (-1)^N Sin(x)
	// Where N is an integer
	*/
	n = (int)(x * inv_pi);
	x = x - ((float) n * pi);
	
	/*
	// So x should now lie in the range [0 , Pi]
	//
	// Now the taylor series for Sin about the point PI/2
	// which is in the middle of this range is...
	//
	// let y = x -  Pi/2
	//
	// sin(x) = 1 -y^2/2! + y^4/4! - y^6 /6! + y^8/8! - y^10/10!
	//
	// Which when factored ....
	//   1 +y^2(-1/2 +  y^2(1/4!  +y^2(-1/6! +  y^2* 1/8!))); etc
	*/
	x = x - pi_on_2;
	x_sq = x * x;
	
	res = 1.0f + x_sq*(st2 + x_sq *(st4 + x_sq *(st6 + x_sq * st8 ) ) );

	/*
	// Now if n is Odd, then negate the result, BUT if x was originally negative,
	// then it also must be negated.
	*/
	if(n & 0x1)
	{
		if(negate)
		{
			/* Negate the negate. I.E leave as is */
		}
		else
		{
			res = -res;
		}
	}
	else if(negate)
	{
		res = -res;
	}

	#if DO_FPU_PRECISION
		RestoreFPU ();
	#endif

    return (res);     
}

/**************************************************************************
 * Function Name  : s_sincos
 * Inputs         : angle x in radians
 * Outputs        : sinw and cosine of angle
 * Input/Output	  : NONE
 * Returns        : NONE
 * Global Used    : NONE
 *
 * Description    : Computes Sine and cosine in single precision
 **************************************************************************/

void s_sincos(float x, float * s, float * c)
{
	int negate;

	int n;
	float x_sq;
	float sinRes;
	float cosRes;

	#if DO_FPU_PRECISION
		SetupFPU ();
	#endif
	
	/*
	// Sin(-x)== -Sin(x) AND  Cos(-x)==Cos(x)
	//
	// So get ABS of x (but remember its sign)
	//
	*/
	negate = (x < 0.0f);
	x = sfabs(x);
	
	/*
	// Sin(N Pi + x) == (-1)^N Sin(x) AND  Cos(N Pi + x) == (-1)^N Cos(x)
	// Where N is an integer
	*/
	n = (int)(x * inv_pi);
	x = x - ((float) n * pi);
	
	/*
	// So x should now lie in the range [0 , Pi]
	//
	// Now the taylor series for Sin about the point PI/2
	// which is in the middle of this range is...
	//
	// let y = x -  Pi/2
	//
	// sin(x) = 1 -y^2/2! + y^4/4! - y^6 /6! + y^8/8! - y^10/10!
	//
	// Which when factored ....
	//   1 +y^2(-1/2 +  y^2(1/4!  +y^2(-1/6! +  y^2* 1/8!))); etc
	//
	//
	// And that for COS is
	//
	// cos(x) = -y + y^3/3! - y^5/5! + y^7 /7! - y^9/9!
	//
	// Which when factored ....
	//    y(-1.0 + y^2 (1/3! + y^2(-1/5! + y^2(1/7! +(y^2 * -1/9!)))));
	*/
	x = x - pi_on_2;
	x_sq = x * x;
	
	

	cosRes = x * (ct1 + x_sq*(ct3 + x_sq * (ct5 + x_sq *(ct7 +  ct9 * x_sq))));
	sinRes = 1.0f + x_sq*(st2 + x_sq *(st4 + x_sq *(st6 + x_sq * st8 ) ) );


	/*
	// Keep within COS's legal range (as we seem to sometimes 
	// have an overflow)
	*/
	if(cosRes > 1.0f)
	{
		cosRes = 1.0f;
	}
	else if(cosRes < -1.0f)
	{
		cosRes = -1.0f;
	}


	/*
	// Now if n is Odd, then negate the results, 
	// BUT if x was originally negative, then 
	// dont bother negating SIN twice.
	*/
	if(n & 0x1)
	{
		if(negate)
		{
			/* Negate the negate fo SIN. I.E leave as is */
			cosRes = - cosRes;
		}
		/*
		// Else negate both
		*/
		else
		{
			cosRes = - cosRes;
			sinRes = - sinRes;
		}
	}
	else if(negate)
	{
		sinRes = -sinRes;
	}

	#if DO_FPU_PRECISION
		RestoreFPU ();
	#endif
    
	/*
	// send back the results
	*/
	*c = cosRes;
	*s = sinRes;
}






#if TESTCODE

#include <float.h>
#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#define PI 0.31415926535897932e+01



/*
// Accuracy test program 1
*/
#define TOP (10.0f * 3.141592654f)
#define BOT (10.0f *-3.141592654f)
#define STEPS 100000

void main(void)
{
	float x;
	
	float y1, y2, err, max_err, worst_val;
	float s, c;	

	long i;

	int overflowed; 

	/*
	// DO SIN 
	*/
	printf ("Checking Single Precision Sin\n"); 

	overflowed = 0;

	max_err = -1.0f;
	for(i = 0; i < STEPS; i ++)
	{
		
		x = ((float) i / STEPS) * (TOP - BOT)  + BOT;
	
	    y1 = s_sin(x);
	    y2 = (float) sin(x);

	    err = (float) fabs(y1 - y2);
	    if(err > max_err)
	    {
	     	worst_val = x;
	     	max_err = err;
	    }

		if(!overflowed && (fabs(y1) > 1.0f))
		{
			overflowed = 1;
			printf("Exceeded 1.0 at %f\n", x);
		}
	    
	}

	printf ("Worst error:%f   at x =%f    Double result= %f     single =%f\n", 
				max_err, worst_val, (float) sin(worst_val), s_sin(worst_val));

	/*
	// DO COS
	*/
	printf ("\nChecking Single Precision Cos\n"); 

	overflowed = 0;
	max_err = -1.0f;
	for(i = 0; i < STEPS; i ++)
	{
		
		x = ((float) i / STEPS) * (TOP - BOT)  + BOT;
	
	    y1 = s_cos(x);
	    y2 = (float) cos(x);

	    err = (float) fabs(y1 - y2);
	    if(err > max_err)
	    {
	     	worst_val = x;
	     	max_err = err;
	    }

		if(!overflowed && (fabs(y1) > 1.0f))
		{
			overflowed = 1;
			printf("Exceeded 1.0 at %f\n", x);
		}
	    
	}

	printf ("Worst error:%f   at x =%f    Double result= %f     single =%f\n", 
				max_err, worst_val, (float) cos(worst_val), s_cos(worst_val));




	/*
	// DO Combined Cos and Sin
	*/
	printf ("\nChecking Combined Single Precision Cos & Sin\n"); 

	overflowed = 0;
	max_err = -1.0f;
	for(i = 0; i < STEPS; i ++)
	{
		
		x = ((float) i / STEPS) * (TOP - BOT)  + BOT;
	
	    s_sincos(x, &s, &c);

	    y1 = (float) sin(x);
	    y2 = (float) cos(x);

	    err = MAX((float) fabs(s - y1), (float) fabs(c - y2));
	    if(err > max_err)
	    {
	     	worst_val = x;
	     	max_err = err;
	    }

		if(!overflowed && ((fabs(s) > 1.0f) || (fabs(c) > 1.0f)))
		{
			overflowed = 1;
			printf("Exceeded 1.0 at %f\n", x);
		}
	    
	}

	
	    s_sincos(worst_val, &s, &c);

	printf ("Worst error:%f   at x =%f    Double results(s,c)= %f %f    single =%f %f\n", 
			max_err, worst_val,	(float) sin(worst_val), (float) cos(worst_val),	s, c);


}




/*
// Accuracy test program 2
*/
qmain (void)
{
	int ct;
	double a, e, i, worstp;
	double worste = 0.0;
	double f = -PI;

	printf("Verfiy sin(2*x) == 2*sin(x)*cos(x)\n");
	for(i = (PI/100.0); (f + i) > f; i *=0.01)
	{
		for(ct = 200, a = f;  --ct; a += i)
		{
			e = fabs(s_sin(a+a) - (2.0*s_sin(a)*s_cos(a)));
			if(e > worste)
			{
				worste = e;
				worstp = a;
			}
		}
		f = worstp - i;
	}

	printf("Worst error %.17e at %.17e\n", worste, worstp);
	printf("sin(2x)=%.17e 2*sin(x)*cos(x)=%.17e\n", f = s_sin(worstp+worstp),
						  2.0*s_sin(worstp) * s_cos(worstp));

	printf("Epsilon is %.17e\n", fabs(f) * DBL_EPSILON);
	return(0);

}


#endif /*TESTCODE*/



/*
// END OF FILE
*/

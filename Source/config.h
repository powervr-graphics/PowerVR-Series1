/*************************************************************************
* Name : config.h
* Title :  Configuration header file
* Author : Simon Fenney
* Created : 28/06/1995
*
* Copyright	: 1995-2022 Imagination Technologies (c)
* License	: MIT
*
* Description :	File that sets up particular system configuration
*				based on Compiler and Target Hardware.
*
*				It is assumed that the following MACROS are
*				passed in from the command line...
*
*				COMPILER    :   certain "extensions" eg inline may
*								only be available on particular compilers
*
*
*				ARCH		:	The particular CPU architecture (or class
*								of architectures)
*
*
*		These set various flags which choose different "versions" of bits of
*		code in SGL.
*
*				
* Platform : ANSI compatible
*
* Modifications:-
* $Log: config.h,v $
 * Revision 1.20  1997/04/11  14:47:24  gdc
 * removed fossil WATCOM include
 *
 * Revision 1.19  1997/03/31  22:05:17  jop
 * rationalised WIN32s, #include "windows.h", and stuck in some DOS32s
 *
 * Revision 1.18  1997/03/14  11:05:01  jwf
 * Made some changes to make it compile on the macintosh
 *
 * Revision 1.17  1997/01/19  15:30:46  sjf
 * Added HAS_FAST_BYTE_LOAD
 *
 * Revision 1.16  1997/01/19  15:08:08  sjf
 * Fixed Little/Big endian macros
 *
 * Revision 1.15  1996/05/14  12:45:19  sjf
 * Added yet another configuration macro - one that indicates if the built in
 * float to int conversion is slow, and whether we can do better.
 *
 * Revision 1.14  1996/04/19  16:15:30  sjf
 * Added a "slow floating point compare" flag for, you guessed it, pentium.
 *
 * Revision 1.13  1996/04/02  16:21:25  jop
 * Added support for inlining with ICL
 *
 * Revision 1.12  1996/04/02  12:31:35  sjf
 * Added SLOW_FDIV definition
 *
 * Revision 1.11  1995/12/19  20:23:59  jop
 * Fixed annoying warning message
 *
 * Revision 1.10  1995/09/30  19:55:17  jop
 * Added some Intel Reference Compiler config info
 *
 * Revision 1.9  1995/09/08  15:34:57  sjf
 * Added a "slow qrt div" identifier to decide how we do inverse square roots.
 *
 * Revision 1.8  1995/09/04  13:13:18  jop
 * Changed watcom define from WATCOM to __WATCOMC__
 *
 * Revision 1.7  1995/08/03  09:01:20  sjf
 * OOPS, had endian of ix86 and FP reg macros COMPLETELY wrong.
 *
 * Revision 1.6  1995/08/02  19:01:14  sjf
 * Added ix86 configuration.
 *
 * Revision 1.5  1995/07/27  10:05:43  jop
 * changed #ifs to #if defined()
 *
 * Revision 1.4  1995/07/23  15:41:32  sjf
 * Added some "silly" code which should quickly point out if INLINE is
 * not definde properly...
 *
 * Revision 1.3  1995/07/10  12:15:20  jop
 * Added endian stuff to architecture definitions
 *
 * Revision 1.2  1995/07/10  10:40:46  sjf
 * fixed my stupidity with specifying compiler/cpu config (I didn't
 * understand how #if worked with ==
 *
 * Revision 1.1  1995/06/30  11:01:13  sjf
 * Initial revision
 *
*
**************************************************************************/


/* //////////////////////////////////////////////////////////////////////
//
// The following is a list of Macro settings that can be turned ON/OFF
// by various compiler/CPU combinations
//
//
// INLINE:	 		Defines the "inline" keyword. Empty if not supported
// INLINING:		Defined to be 1 if inline available. Can be undefined
//					(or 0) otherwise
//
// MULTI_FP_REG:	IF 1, it indicates that the CPU has many FP registers, and
//					code can take advantage. EG new RISC processors (R4xxx, PowerPC)
//					IF 0 (or not defined) then it doesn't try to hoard things 
//					locally.
//
//
////////////////////////////////////////////////////////////////////// */

/* Make sure that the compiler and processor type macro's that the Mac uses
 * are defined. ie. stop the other compilers from complaining.
 * We have to do it this way since the Metro werks compiler doesn't
 * support #if defined(xxx)
 */
#ifndef __POWERPC__
	#define __POWERPC__				0
	#define __GET_RID_OF_POWERPC__	1
#else
	#define __GET_RID_OF_POWERPC__	0
#endif
#ifndef __MWERKS__
	#define __MWERKS__				0
	#define __GET_RID_OF_MWERKS__	1
#else
	#define __GET_RID_OF_MWERKS__	0
#endif

/* Machine type
 */
#ifdef macintosh
	#define MAC		1
#else
	#define MAC		0
#endif


/*
// Compiler options....
*/
/* /////////////////////
// Varieties of systems for GCC.
//  GCC allows inlining of functions
///////////////////// */
#if defined(GCC)
#include "platform/gcc_opts.h"

/* /////////////////////
// Else if SDE_MIPS Compiler.
//  This is a variant of GCC, so use gcc
///////////////////// */
#elif defined(SDE_MIPS)

#include "platform/gcc_opts.h"


/* /////////////////////
// Else if NEC MIPS 4000 Compiler
//
// This isn't important at the moment, so justdo default..
//  
///////////////////// */
#elif defined(NEC_MIPS)

#define INLINE
#define INLINING 0


/* /////////////////////
// Else if WATCOM x86 Compiler
///////////////////// */
#elif defined(__WATCOMC__)

#define INLINING 1
#define INLINE	 

/* /////////////////////
// Else if intel reference compiler
///////////////////// */
#elif defined(ICC)

#define INLINE
#define INLINING 1

/* /////////////////////
// Else if intel reference compiler W95 version
///////////////////// */
#elif defined(ICL)

#define INLINE	__inline
#define INLINING 1

/* /////////////////////
// Else if Mac Codewarrior compiler
///////////////////// */
#elif __MWERKS__

/* I though that It would all be good
 * but metroworks seems to fall over with inlining on
 * #define INLINE  inline
 * #define INLINING 1
 */
#define INLINE
#define INLINING 0

/* /////////////////////
// ELSE default - play it safe, disable any special items
///////////////////// */
#else

#define INLINE
#define INLINING 0

#endif





/* ///////////////////////////////////////////////
// CPU options....
/////////////////////////////////////////////// */

/* /////////////////////
// MIPS 4000/5000
///////////////////// */

#if defined(R4000BIG)

#include "platform/r4000.h"

#define BIG_ENDIAN		1


#elif defined(R4000LITTLE)

#include "platform/r4000.h"

#define LITTLE_ENDIAN	1


#elif defined(ip22)

#include "platform/r4000.h"

#define BIG_ENDIAN		1



/* /////////////////////
// Other possible Risc targets
// (for simulator)
///////////////////// */

#elif defined(hp700) || defined(sun4)

#define BIG_ENDIAN		1
#define MULTI_FP_REG	1


#elif defined(i386) || defined(i486) || defined(i586) || defined(WIN32) || defined(DOS32)

#define LITTLE_ENDIAN		1
#define MULTI_FP_REG		0

/*
// Now here's a surprise ... Slow settings for the Intel processors
*/
#define SLOW_SQRT_DIV		1
#define SLOW_FDIV			1
#define SLOW_FCMP			1
/*
//NOTE the SLOW_FTOI function is affected by the rounding mode of the processor.
// if you want to truncate when getting integers, it had better be in truncate
// mode. I believe Jim sets this mode.
*/
#define SLOW_FTOI			1     

/* /////////////////////
// Macintosh PowerPC
///////////////////// */
#elif __POWERPC__

#define BIG_ENDIAN			1
#define MULTI_FP_REG		1

/* /////////////////////
// Default
///////////////////// */

#else

#endif


/*
// To prevent some compilers from complaining, make sure certain macros
// are defined
*/
#ifndef SLOW_SQRT_DIV
#define SLOW_SQRT_DIV 0
#endif

#ifndef SLOW_FDIV
#define SLOW_FDIV	  0
#endif

#ifndef SLOW_FCMP
#define SLOW_FCMP	  0
#endif

#ifndef SLOW_FTOI
#define SLOW_FTOI	  0
#endif


#ifndef HAS_FAST_BYTE_LOAD
#define HAS_FAST_BYTE_LOAD	  0
#endif


/*
// EXACTLY one of LITTLE_ENDIAN or BIG_ENDIAN must have been defined to be
// '1' at this stage. If not, we've had a configuration error, and we should 
// refuse to put up with anymore of this nonsense. I'm taking my compiler and
// going home.
//
// First if either have not been defined, define them to be zero.
*/
#ifndef LITTLE_ENDIAN
#define LITTLE_ENDIAN 0
#endif

#ifndef BIG_ENDIAN
#define BIG_ENDIAN 0
#endif

/*
// Now just make sure EXACTLY ONE of the pair is non-zero
*/
#if LITTLE_ENDIAN

	#if BIG_ENDIAN
		ERROR!! BOTH Big and little endian have been set!
	#endif

#elif !BIG_ENDIAN

    ERROR!! They have BOTH been defined to be Zero

#endif



/* /////////////////////
// CHECK THAT CERTAIN IMPORTANT VARIABLES ARE ACTUALLY DEFINED
///////////////////// */
#ifndef INLINING
#pragma message ("INLINING variable not set")
#define INLINING 0
#endif

#ifndef INLINE
#pragma message ("INLINE variable not set")
#define INLINE
#endif


/* Clean up the Mac compiler and processor type macros if they are not needed
 * since #ifdef __POWERPC__ / __MWERKS__ could be used in the code.
 */
#if __GET_RID_OF_POWERPC__
	#undef __POWERPC__
#endif
#if __GET_RID_OF_MWERKS__
	#undef __MWERKS__
#endif

/*
// End of file
*/


/*************************************************************************
* Name : gcc_opts.h
* Title :  Configuration header file for gcc compilation
* Author : Simon Fenney
* Created : 28/06/1995
*
* Copyright	: 1995-2022 Imagination Technologies (c)
* License		: MIT
*
* Description :	Defines various macro options for GCC
*				
* Platform : ANSI compatible
*
* Modifications:-
* $Log: gcc_opts.h,v $
 * Revision 1.2  1995/09/23  15:05:50  sjf
 * Until I get a better solution, had to stop inlining when optimisation
 * was off - unfortunately, I only know that is (probably) off if
 * debugging is on.
 *
 * Revision 1.1  1995/06/28  16:15:00  sjf
 * Initial revision
 *
*
**************************************************************************/

/*
// Allows inlining
*/
#if ! DEBUG
#define INLINING 1
#define INLINE	 inline

#else

#define INLINING 0
#define INLINE

#endif

/*
// END OF FILE
*/


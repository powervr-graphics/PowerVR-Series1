/*************************************************************************
* Name : r4000.h
* Title :  Configuration header file for GENERIC Mips 4000 type processors
* Author : Simon Fenney
* Created : 28/06/1995
*
* Copyright	    : 1995-2022 Imagination Technologies (c)
* License		: MIT
*
* Description :	Defines various macro options for MIPS 4000s
*				
* Platform : ANSI compatible
*
* Modifications:-
* $Log: r4000.h,v $
 * Revision 1.2  1997/01/19  15:29:17  sjf
 * Added fast byte macro
 * ,.
 *
 * Revision 1.1  1995/06/28  16:15:21  sjf
 * Initial revision
 *
*
**************************************************************************/

/*
// Assume plenty of FP registers
*/
#define MULTI_FP_REG 1

/*
// You can also load Byte values from memory into integers quickly
*/
#define HAS_FAST_BYTE_LOAD 1

/*
// END OF FILE
*/


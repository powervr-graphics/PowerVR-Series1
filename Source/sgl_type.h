/*****************************************************************************
;++
Name           	:   $RCSfile: sgl_type.h,v $ ($Source: /user/rcs/revfiles/sabre/sgl/RCS/sgl_type.h,v $)
Title           :   SGL data types
C Author       	:   Jim Page
Created         :   19/04/95

Copyright		: 	1995-2022 Imagination Technologies (c)
License			: 	MIT

Description     :   Data types and structures used in SGL
                    
Program Type    :   Header file (ANSI)

RCS info:

  $Date: 1997/05/02 07:45:14 $
  $Revision: 1.10 $
  $Locker:  $
  $Log: sgl_type.h,v $
 * Revision 1.10  1997/05/02  07:45:14  erf
 * Removed UINT32's etc and replaced with sgl_uint32's etc.
 *
 * Revision 1.9  1997/01/19  15:10:24  sjf
 * Fixed little/big endian macros
 *
 * Revision 1.8  1995/08/03  12:52:20  sjf
 * put "unsigned" into the endian macros .. they looked suspicious.
 *
 * Revision 1.7  1995/07/27  10:06:24  jop
 * changed #ifs to #if defined()
 *
 * Revision 1.6  1995/07/10  12:23:31  jop
 * Removed extra endif
 *
 * Revision 1.5  1995/07/10  12:14:40  jop
 * config.h now defines xx_ENDIAN
 *
 * Revision 1.4  1995/07/10  10:42:14  sjf
 * Removed some dead code.
 *
 * Revision 1.3  1995/06/01  14:35:04  jop
 * Ifdef'd out the INT8,INT16 etc typedefs
 *
 * Revision 1.2  1995/04/27  13:43:08  jop
 * Changed Text format to UNIX
 *
 * Revision 1.1  1995/04/20  14:56:43  jop
 * Initial revision
 *

;--
*****************************************************************************/

#ifndef __SGL_TYPE__
#define __SGL_TYPE__

	#ifndef TRUE
		#define TRUE 1
	#endif
	
	#ifndef FALSE
		#define FALSE 0
	#endif
	
	/*
	 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	 * ++	                			Endian swap macros	                     	 ++
	 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	 */

	#define ENDIAN_SWAP_16(x)		(((((sgl_uint16)x)&0xFF00U)>>8)| \
									 ((((sgl_uint16)x)&0x00FFU)<<8))

	#define ENDIAN_SWAP_32(x)		(((((sgl_uint32)x)&0xFF000000UL)>>24)| \
								 	 ((((sgl_uint32)x)&0x00FF0000UL)>>8)|	 \
								 	 ((((sgl_uint32)x)&0x0000FF00UL)<<8)|	 \
								 	 ((((sgl_uint32)x)&0x000000FFUL)<<24))

	#ifdef LONG_LONGS

		#define ENDIAN_SWAP_64(x)	(((((UINT64)x)&0xFF00000000000000UL)>>56)| \
									 ((((UINT64)x)&0x00FF000000000000UL)>>40)| \
									 ((((UINT64)x)&0x0000FF0000000000UL)>>24)| \
									 ((((UINT64)x)&0x000000FF00000000UL)>>8)|  \
									 ((((UINT64)x)&0x00000000FF000000UL)<<8)|  \
									 ((((UINT64)x)&0x0000000000FF0000UL)<<24)| \
									 ((((UINT64)x)&0x000000000000FF00UL)<<40)| \
									 ((((UINT64)x)&0x00000000000000FFUL)<<56))
	#endif

	#if (BIG_ENDIAN)
	
		#define BIG_ENDIAN_16(x)	x
		#define BIG_ENDIAN_32(x)	x
		#define BIG_ENDIAN_64(x)	x
		#define LITTLE_ENDIAN_16(x)	ENDIAN_SWAP_16(x)
		#define LITTLE_ENDIAN_32(x)	ENDIAN_SWAP_32(x)	
		#define LITTLE_ENDIAN_64(x)	ENDIAN_SWAP_64(x)

	#elif (LITTLE_ENDIAN)
	
		#define BIG_ENDIAN_16(x)	ENDIAN_SWAP_16(x)
		#define BIG_ENDIAN_32(x)	ENDIAN_SWAP_32(x)	
		#define BIG_ENDIAN_64(x)	ENDIAN_SWAP_64(x)
		#define LITTLE_ENDIAN_16(x)	x
		#define LITTLE_ENDIAN_32(x)	x
		#define LITTLE_ENDIAN_64(x)	x
	#else
		ARGHHHHH!!!! One of these must be defined
	#endif

#endif /* __SGL_TYPE__ */

/* end of $RCSfile: sgl_type.h,v $ */

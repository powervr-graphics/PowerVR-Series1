/**************************************************************************
 * Name         : texas.h
 * Title        : Texas code external defs
 *				  
 * Author       : Simon Fenney
 * Created      : 09 May 1995
 *
 * Copyright : 1995-2022 Imagination Technologies (c)
 * License	 : MIT
 *
 * Description  : External defs of functions and data structures
 *				  for the code that handles Texas (either HW or Sim)
 *				  Note this file will effectively include the relevant
 *				  files (either HW or Sim) for rest of SGL.
 *
 * Platform     : ANSI
 *
 * Modifications:-
 * $Log: texas.h,v $
 * Revision 1.23  1997/06/04  13:52:24  gdc
 * stopped compiler warnings
 *
 * Revision 1.22  1997/06/04  11:16:51  gdc
 * added a prototype
 *
 * Revision 1.21  1997/05/02  07:45:14  erf
 * Removed UINT32's etc and replaced with sgl_uint32's etc.
 *
 * Revision 1.20  1997/04/30  19:37:45  mjg
 * Removed tmalloc include.
 *
 * Revision 1.19  1997/04/03  10:55:57  rgi
 * Ported ZUES_ARCADE support to top level
 *
 * Revision 1.18  1997/03/31  22:10:54  jop
 * Sorted out WIN32/DOS32 issues
 *
 * Revision 1.17  1997/03/14  11:06:53  jwf
 * changed some stuff to make it compile on the macintosh
 *
 * Revision 1.16.1.1  1997/04/03  08:40:20  rgi
 * Added ZEUS_ARCADE support
 *
 * Revision 1.16  1996/12/10  23:23:43  erf
 * Changed compile switches:
 * 1. PVR1 to ISPTSP.
 * 2. PVR2 to PCX1.
 * 3. PVR3 to PCX2.
 *
 * Revision 1.15  1996/11/14  12:22:23  erf
 * Fix comment.
 *
 * Revision 1.14  1996/11/14  12:17:59  erf
 * For PCX2 simulation include "simulat3/smtexas.h".
 *
 * Revision 1.13  1996/11/06  16:47:16  erf
 * PCX2 and PVR3 conditions added.
 *
 * Revision 1.12  1996/07/17  14:00:27  sjf
 * Added #define to (hopefully) determine if we are building the
 * simulator.
 *
 * Revision 1.11  1996/07/16  10:51:26  jop
 * Division free solution for trangle api
 *
 * Revision 1.10  1996/04/26  17:31:05  pdl
 * added pcx1 support.
 *
 * Revision 1.9  1995/12/18  17:49:08  sjf
 * Allowed for different PVR Instruction sets.
 *
 * Revision 1.8  1995/10/27  10:03:49  pdl
 * added the HW/SW includes.
 *
 * Revision 1.7  1995/10/04  08:38:37  sjf
 * Added routine to return size of a texture from the texas precomp struct.
 *
 * Revision 1.6  1995/08/09  17:22:45  pdl
 * added the TexasMapOpacityFloatToInt
 *
 * Revision 1.5  1995/07/21  11:06:11  pdl
 * Added two structures.
 *
 * Revision 1.4  1995/07/17  16:02:44  pdl
 * rewrote completely.
 *
 * Revision 1.3  1995/06/01  12:40:41  jop
 * Added opacity mapping function
 *
 * Revision 1.2  1995/05/25  14:22:27  sjf
 * Wasted time changing identifiers to vl standard.
 *
 * Revision 1.1  1995/05/09  16:20:58  sjf
 * Initial revision
 *
 *
 **************************************************************************/

#ifndef __TEXAS_H__
#define __TEXAS_H__

/*
// Define the number of light slots Texas can handle
*/
#define NUM_LIGHT_SLOTS 2

#if WIN32 || DOS32 || MAC || ZEUS_ARCADE

	#define SIMULATOR 0
	
	#if ZEUS_ARCADE
		#include "zeus/hwtexas.h"
	#else
		#include "hwtexas.h"
	#endif

#else
	#define SIMULATOR 1

	#if PCX1
		#include "simulat2/smtexas.h"   /*texas simulation 2*/
	#elif PCX2
		#include "simulat3/smtexas.h"   /*texas simulation 3*/
	#else
		#include "simulat/smtexas.h"	/*texas simulation 1 */
	#endif

#endif

typedef struct
{
	sgl_uint32	TexAddress;
	sgl_uint32	LowWord;
} TEXAS_PRECOMP_STRUCT;

typedef struct
{
	sgl_uint32	Control1;
	sgl_uint32	TexCoeff1;
	sgl_uint32	TexCoeff2;
	sgl_uint32	TexCoeff3;
	sgl_uint32	TexCoeff4;
	sgl_uint32	TexCoeff5;
	sgl_uint32	TexCoeff6;
} TEXTURING_RESULT_STRUCT;

int TexasMapOpacityFloatToInt (float fOpacity);
int TexasGetTextureDimension  (const TEXAS_PRECOMP_STRUCT *pTexasPreComp);
float TexasGetInvTextureDimension (const TEXAS_PRECOMP_STRUCT *pTexasPreComp);

#endif

/*--------------------------- End of File --------------------------------*/



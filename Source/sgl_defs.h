/*************************************************************************
* Name : sgl_defs.h
* Title : Standard includes/defs for SGL
* Author : Simon Fenney
* Created : 3/05/1995
*
* Copyright	: 1995-2022 Imagination Technologies (c)
* License	: MIT
*
* Description : A convenient #include that includes all the
*				necessary files for SGL.
*
* Platform : ANSI compatible
*
* Modifications:-
* $Log: sgl_defs.h,v $
 * Revision 1.17  1997/06/18  14:44:07  mjg
 * Commented out the typedefs of Windows types (UINT and the rest) -
 * sgl_ typedefs should be used instead as thery don't cause
 * compiler warnings from Windows header files.
 *
 * Revision 1.16  1996/05/10  20:52:29  jop
 * modified typedefs to conform with new sgl_ typedefs in sgl.h
 *
 * Revision 1.15  1996/05/10  16:32:33  jrc
 * Added sentry against multiple inclusion.
 *
 * Revision 1.14  1995/10/04  10:39:07  jop
 * Added DOS cptble SEEK_SET ifdef
 *
 * Revision 1.13  1995/09/25  11:20:19  sjf
 * Added <unistd.h>
 *
 * Revision 1.12  1995/07/25  11:21:56  sjf
 * Added stdio as standard.
 *
 * Revision 1.11  1995/07/17  16:04:12  jop
 * removed PSTR typedef - clashed with windows.h def.
 *
 * Revision 1.10  1995/06/30  11:01:55  sjf
 * Added config.h as standard, and removed "inline" def.
 *
 * Revision 1.9  1995/06/23  18:46:56  sjf
 * Added sgl.h as standard include.
 *
 * Revision 1.8  1995/06/08  17:18:11  sjf
 * Added temp definitions of INLINE and INLINING until compiler
 * specific #includes are done.
 *
 * Revision 1.7  1995/05/30  12:28:41  sjf
 * fixed include filenames to be 8.3 compatible.
 *
 * Revision 1.6  1995/05/26  14:35:27  sjf
 * Renamed sgl_macros.h to sgl_macro.h so that it was 8.3 compatible.
 *
 * Revision 1.5  1995/05/26  12:38:57  sjf
 *  moved ANSI .h's to sgl_defs.h + other common includes.
 *
 * Revision 1.4  1995/05/25  14:22:12  sjf
 * Wasted time changing identifiers to vl standard.
 *
 * Revision 1.3  1995/05/12  11:32:07  sjf
 * Added mod_id.h
 *
 * Revision 1.2  1995/05/11  14:11:49  sjf
 * Added error.h to standard includes.
 *
 * Revision 1.1  1995/05/09  16:19:10  sjf
 * Initial revision
 *
*
*
**************************************************************************/


#ifndef __SGL_DEFS_H
#define __SGL_DEFS_H


/*
// Include standard ANSI headers
*/
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#ifndef SEEK_SET
#include <unistd.h>  /*needed for SEEK_SET etc on suns*/
#endif

#include "sgl.h"

/*
// Define special integer types
 
typedef sgl_int8 	INT8;
typedef sgl_uint8	UINT8;
typedef sgl_int16	INT16;
typedef sgl_uint16	UINT16;
typedef sgl_int32	INT32;
typedef sgl_uint32	UINT32;
typedef	sgl_uint	UINT;
*/

/*
// Include Compiler/CPU specific stuff.
*/
#include "config.h"

#include "sgl_type.h"
#include "sglmacro.h"
#include "mod_id.h"
#include "debug.h"

#include "error.h"

/*
// Include transfor.h etc
// as these definitions are pretty much needed globally...
*/
#include "transfor.h"
#include "texas.h"


#endif  /* ifndef __SGL_DEFS_H */

/*
// End of file
*/

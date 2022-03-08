/******************************************************************************
 * Name         : sgltri.h
 * Title        : Mac Rave to SGL interface for rendering triangles.
 * Author       : John Catchpole
 * Created      : 30/04/1996
 *
 * Copyright	: 1995-2022 Imagination Technologies (c)
 * License		: MIT
 *
 * Description  : Contains structures and prototype for SglRenderTriangles.
 *				  This function renders a scene of triangles output by the
 *				  Apple Macintosh Rave transformation and lighting stages.
 *
 * Platform     : ANSI
 *
 * Modifications:
 * $Log: sgltri.h,v $
 * Revision 1.35  1996/12/09  14:15:07  gdc
 * removed redundant stuff from this module - assume that D3DOPTIM stuff
 * is now default - Note: THIS FILE IS NOW REDUNDANT
 *
 * Revision 1.34  1996/11/19  01:01:48  jop
 * Replaced something I shouldn't have removed ...
 *
 * Revision 1.33  1996/11/18  18:32:55  jop
 * Removed PowerSGL direct stuff
 *
 * Revision 1.32  1996/11/18  17:42:48  jop
 * Changed 'new' structures
 *
 * Revision 1.31  1996/11/15  08:09:19  jrc
 * COMBINE and HALF_TEXTURE are now only used if we are not using D3DOPTIM
 * code.
 *
 * Revision 1.30  1996/11/11  21:38:16  jrc
 * Added some information to the comments on COMBINE and HALF_TEXTURE.
 *
 * Revision 1.29  1996/11/09  04:25:27  jrc
 * Switches added for triangle joining hacks.  'COMBINE' is currently enabled
 *
 * Revision 1.28  1996/11/07  13:00:57  erf
 * Added PVR3 compile switch for PCX2 to:
 * 1. Change n32A, B & C to f32A, B & C.
 *
 * Revision 1.27  1996/11/06  05:11:57  jrc
 * Added u32NumSides to n-polygon parameter structure.
 *
 * Revision 1.26  1996/11/04  15:32:38  jrc
 * 1. Removed SGLQUADPRECALC because we now use SGLTRIPRECALC for all types.
 * 2. Added some stuff for n-sided polygons.
 *
 * Revision 1.25  1996/10/30  14:10:13  jop
 * Removed NETRACE stuff
 *
 * Revision 1.24  1996/10/24  16:08:41  jrc
 * An effort at working around smooth shading/translucency/shadows problem.
 *
 * Revision 1.23  1996/10/23  16:14:48  jop
 * Fixed bugus Z rockem bug
 *
 * Revision 1.22  1996/10/16  21:17:55  jop
 * Added new structures for lite+
 *
 * Revision 1.21  1996/10/02  16:50:46  jop
 * Added more adapt stuff
 *
 * Revision 1.20  1996/09/27  14:29:07  jop
 * Publshed gfBogusZ
 *
 * Revision 1.19  1996/09/05  17:03:04  jop
 * Went back to adjoint method (sparklies fix)
 *
 * Revision 1.18  1996/09/04  00:05:26  jop
 * trimmed down precalc struct
 *
 * Revision 1.17  1996/08/14  14:40:26  pdl
 * removed an '_'
 *
 * Revision 1.16  1996/08/02  16:26:33  jrc
 * Added definition of D3DCOLOR for non WIN32 builds.
 *
 * Revision 1.15  1996/07/31  21:49:16  jop
 * Added inverse matrix to precalc struct
 *
 * Revision 1.14  1996/07/15  17:10:32  jop
 * Added quads
 *
 * Revision 1.13  1996/05/15  10:44:08  jop
 * moved to new triPrecalc structure
 *
 * Revision 1.12  1996/05/10  20:53:34  jop
 * Moved public structs and typedefs to sgl.h
 *
 * Revision 1.11  1996/05/10  16:18:41  jrc
 * Removed INT16 etc definitions and did #include "sgl_defs.h" instead.
 *
 * Revision 1.10  1996/05/10  16:04:48  jrc
 * Removed inclusion of windows.h and changed WORD and DWORDs to UINT16 and
 * UINT32s.
 *
 * Revision 1.9  1996/05/10  14:47:40  jop
 * Added structures and macros
 *
 * Revision 1.8  1996/05/09  19:11:04  jrc
 * 1. Fixed typo in typedef command.
 * 2. Added SGLTRIBUFFERSIZE definition.
 *
 * Revision 1.7  1996/05/09  18:55:58  jrc
 * Added brackets around definition of PSGLFACE.
 *
 * Revision 1.6  1996/05/09  18:54:06  jrc
 * Added precalc structure.
 *
 * Revision 1.5  1996/05/09  18:02:39  jrc
 * Made more optimal for Direct3D.
 *
 * Revision 1.4  1996/05/08  17:38:05  jrc
 * Removed carriage returns from end of lines and added modulation colour
 * values.
 *
 * Revision 1.3  1996/05/07  14:16:50  jop
 * made sgltri.h UNIX freindly
 *
 * Revision 1.2  1996/05/03  17:59:49  jop
 * Some changes
 *
 * Revision 1.1  1996/05/03  15:26:12  jrc
 * Initial revision
 *
 *****************************************************************************/

#ifndef __SGLTRI_H__
#define __SGLTRI_H__


#endif /* if __SGLTRI_H__ */

/*------------------------------- End of File -------------------------------*/

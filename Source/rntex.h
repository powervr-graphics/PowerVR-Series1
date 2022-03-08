/******************************************************************************
 * Name         : rntex.h
 * Title        : Performs all the texture parameter calculation.
 * Author       : Peter Leaback
 * Created      : 17/07/1995
 *
 * Copyright	: 1995-2022 Imagination Technologies (c)
 * License		: MIT
 *
 * Description  :
 * 
 * Platform     : ANSI
 *
 * Modifications:
 * $Log: rntex.h,v $
 * Revision 1.12  1996/12/09  13:35:53  gdc
 * removed now unused Triangle API stuff - assume new D3DOPTIM stuff used
 * without exception
 *
 * Revision 1.11  1996/11/18  18:35:24  jop
 * Removed PowerSGL direct stuff
 *
 * Revision 1.10  1996/10/16  21:17:12  jop
 * New pack routines for lite+
 *
 * Revision 1.9  1996/08/29  11:15:59  jop
 * Revised parms to triangle api texturing funcs
 *
 * Revision 1.8  1996/07/10  14:36:34  pdl
 * added the accurate/fast texturing.
 *
 * Revision 1.7  1996/07/10  13:44:09  jop
 * Modified tri texturing to do invw mul if need be
 *
 * Revision 1.6  1996/05/21  15:38:58  jop
 * Made triangle vertex ptrs indirect
 *
 * Revision 1.5  1996/05/10  14:46:40  jop
 * Unified triangle support
 *
 * Revision 1.4  1996/03/27  15:33:33  pdl
 * added a few consts.
 *
 * Revision 1.3  1995/09/28  10:27:25  pdl
 * *** empty log message ***
 *
 * Revision 1.2  1995/07/25  08:23:36  sjf
 * Got rid of CFRrecip as its in global projection matrix.
 *
 * Revision 1.1  1995/07/21  15:19:55  pdl
 * Initial revision
 *
 *
 *****************************************************************************/

#ifndef __SGL_H__
#include "sgl.h"
#endif

void DoTextureWrapping(int						NumberOfPlanes,
					  TRANSFORMED_PLANE_STRUCT *Planes[],
					  TRANSFORM_STRUCT		   *ObjToEye,
					  MATERIAL_STATE_STRUCT	   *MState,
					  TEXTURING_RESULT_STRUCT  *Results);

void DoTextureMappingFast(int							 NumberOfPlanes,
						  const TRANSFORMED_PLANE_STRUCT * const Planes[],
						  const TRANSFORM_STRUCT		 *ObjToEye,
						  const MATERIAL_STATE_STRUCT	 *MState,
						  TEXTURING_RESULT_STRUCT  		 *Results);

void DoTextureMappingAccurate(int							 NumberOfPlanes,
							  const TRANSFORMED_PLANE_STRUCT * const Planes[],
							  const TRANSFORM_STRUCT		 *ObjToEye,
							  const MATERIAL_STATE_STRUCT	 *MState,
							  TEXTURING_RESULT_STRUCT  		 *Results);

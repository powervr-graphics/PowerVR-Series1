/******************************************************************************
 * Name         : rnshadow.h
 * Title        : Shadow volume generation.
 * Author       : Simon Fenney; converted to SGL by John Catchpole.
 * Created      : RGL: 06/01/1993, SGL: 11/08/1995
 *
 * Copyright	: 1995-2022 Imagination Technologies (c)
 * License		: MIT
 *
 * Description  : Prototype for an internal routine for shadow volume
 *				  generation.
 *
 * Platform     : ANSI
 *
 * Modifications:
 *
 * RGL version:
 * Revision 1.1  93/09/03  11:04:48  SimonF
 * Initial revision 
 *
 * SGL version:
 * $Log: rnshadow.h,v $
 * Revision 1.3  1997/04/07  14:45:08  gdc
 * added shadow limit planes as a function parameter
 *
 * Revision 1.2  1995/08/27  15:30:22  sjf
 * Removed uneeded parameters, and made a couple of others consts.
 *
 * Revision 1.1  1995/08/14  15:47:55  jrc
 * Initial revision
 *
 *****************************************************************************/
#ifndef __RNSHADOW_H
#define __RNSHADOW_H


void RnGenerateShadowVolume(const CONVEX_NODE_STRUCT 	*pConvex,
							const TRANS_PLANE_ARRAY_TYPE Planes,

							const TRANSFORM_STRUCT *pCurrentTransform,
							const SHADOW_LIM_STRUCT *pShadowLimitPlanes,

							const sgl_bool   bParallelLight,
							const sgl_vector directionPosition,


							TRANS_PLANE_ARRAY_TYPE  ShadowPlanes,
							int					   *pnNumShadowPlanes,
							sgl_bool			   *pbLightInside,
							int					   *pNumOrigObjPlanes);


#endif
/*------------------------------- End of File -------------------------------*/

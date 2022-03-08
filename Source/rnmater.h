/**************************************************************************
 * Name         : rnmater.c
 * Title        : SGL Render traversal for materials
 * Author       : Stel Michael
 * Created      : 13th June 1995
 *
 * Copyright : 1995-2022 Imagination Technologies (c)
 * License	 : MIT
 *
 * Description  : prototypes for routines that handle material
				  nodes during diplay list traversal.
					
 *
 * Platform     : ANSI
 *
 * Modifications:-
 * $Log: rnmater.h,v $
 * Revision 1.5  1996/06/06  23:05:49  sjf
 * Added material instancing.
 *
 * Revision 1.4  1996/05/30  20:32:40  sjf
 *  Added texture caching.
 *
 * Revision 1.3  1995/09/28  10:31:06  pdl
 * RnProcessMaterialNode has changed.
 *
 * Revision 1.2  1995/08/27  15:28:20  sjf
 * Removed local only function prototype.
 *
 * Revision 1.1  1995/06/13  16:21:25  sm
 * Initial revision
 *
 *
 **************************************************************************/
#ifndef __RNMATER_H__
#define __RNMATER_H__

 							/* material node */
void RnProcessMaterialNode(const MATERIAL_NODE_STRUCT * matNode,
								MATERIAL_STATE_STRUCT * stateMaterial,
								 MASTER_STATE_STRUCT *pState);


void RnCTPreprocessMaterialNode(const MATERIAL_NODE_STRUCT  * matNode,
									  MASTER_STATE_STRUCT *pState,
							void **	pCachedTexture);


#endif __RNMATER_H__

/*
// End of file
*/

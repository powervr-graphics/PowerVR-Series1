/******************************************************************************
 * Name         : rnlod.h
 * Title        : SGL Render traversal for level of detail
 * Author       : John Catchpole
 * Created      : 06/07/1995
 *
 * Copyright	: 1995-2022 Imagination Technologies (c)
 * License		: MIT
 *
 * Description  : Prototype for the routine that handles level of detail nodes
 *				  during display list traversal.
 *
 * Platform     : ANSI
 *
 * Modifications:
 * $Log: rnlod.h,v $
 * Revision 1.3  1995/07/10  12:05:46  jrc
 * Made RnProcessLodNode return an int.
 *
 * Revision 1.2  1995/07/10  11:55:38  jrc
 * Added end of file marker and multiple #include sentry.
 *
 * Revision 1.1  1995/07/06  16:50:08  jrc
 * Initial revision
 *
 *****************************************************************************/
#ifndef __RNLOD_H
#define __RNLOD_H


int RnProcessLodNode(LOD_NODE_STRUCT *pNode,
  TRANSFORM_STRUCT *pTransformState);


#endif
/*------------------------------- End of File -------------------------------*/

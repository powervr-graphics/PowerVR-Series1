/******************************************************************************
 * Name         : rnqualit.h
 * Title        : SGL render traversal for quality nodes.
 * Author       : John Catchpole
 * Created      : 10/07/1995
 *
 * Copyright	: 1995-2022 Imagination Technologies (c)
 * License		: MIT
 *
 * Description  : Prototype for the routine that handles quality nodes during
 *				  display list traversal.
 *
 * Platform     : ANSI
 *
 * Modifications:
 * $Log: rnqualit.h,v $
 * Revision 1.3  1995/09/25  18:12:33  sjf
 * added const keyword.
 *
 * Revision 1.2  1995/07/10  12:50:28  jrc
 * Changed return value to void.
 *
 * Revision 1.1  1995/07/10  12:30:04  jrc
 * Initial revision
 *
 *****************************************************************************/
#ifndef __RNQUALIT_H
#define __RNQUALIT_H


void RnProcessQualityNode(const QUALITY_NODE_STRUCT *pNode,
  QUALITY_STATE_STRUCT *pQualityState);


#endif
/*------------------------------- End of File -------------------------------*/

/**************************************************************************
 * Name         : rntransf.c
 * Title        : Rendering tranformation
 * Author       : Stel Michael
 * Created      : 12th June 1995
 *
 * Copyright : 1995-2022 Imagination Technologies (c)
 * License	 : MIT
 *
 * Description  : 
 *
 * Platform     : ANSI
 *
 * Modifications:-
 * $Log: rntransf.h,v $
 * Revision 1.2  1995/06/15  11:09:47  sm
 * changed to take transform node struct
 *
 * Revision 1.1  1995/06/12  16:04:01  sm
 * Initial revision
 *
 *
 **************************************************************************/





void	RnProcessTransformNode(TRANSFORM_NODE_STRUCT * tranNode,  /* transform mode */
						 TRANSFORM_STRUCT  * stateTransform); 	/* Updated state transform node */



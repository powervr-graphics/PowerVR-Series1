/**************************************************************************
 * Name         : rnlights.h
 * Title        : SGL Render traversal for lights
 * Author       : Stel Michael
 * Created      : 15th June 1995
 *
 * Copyright : 1995-2022 Imagination Technologies (c)
 * License	 : MIT
 *
 * Description  : prototypes for routines that handle light nodes during display list traversal.
 *
 * Platform     : ANSI
 *
 * Modifications:-
 * $Log: rnlights.h,v $
 * Revision 1.6  1995/09/25  18:11:36  sjf
 * Added multi pseudo shadow support.
 *
 * Revision 1.5  1995/09/19  09:11:06  sjf
 * Added EOF comment.
 *
 * Revision 1.4  1995/06/27  10:03:43  sm
 * changed some params
 *
 * Revision 1.3  1995/06/20  17:14:32  sm
 * added prototype for CalcAverageLight
 *
 * Revision 1.2  1995/06/16  17:47:58  sm
 * added prototype for switch node processing func
 *
 * Revision 1.1  1995/06/15  16:01:34  sm
 * Initial revision
 *
 *
 **************************************************************************/

extern void	RnProcessLightNode( const LIGHT_NODE_STRUCT	* lightNode, 
									const TRANSFORM_STRUCT * tranNode, 
									LIGHTS_STATE_STRUCT	* lightState);

extern void	RnProcessLightSwitchNode( const LIGHT_SWITCH_NODE_STRUCT* pSwitchNode,
									  LIGHTS_STATE_STRUCT * plightState);

extern void	RnProcessMultiShadowNode(const MULTI_SHADOW_NODE_STRUCT * pNode,  
									LIGHTS_STATE_STRUCT * plightState);

extern	void	CalcAverageCol(LIGHTS_STATE_STRUCT	* lightState);

				

/*
// End of file
*/

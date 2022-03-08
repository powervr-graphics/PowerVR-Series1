/**************************************************************************
 * Name         : rntrav.h
 * Title        : SGL Render traversal Header file
 * Author       : Simon Fenney
 * Created      : 7th June 1995
 *
 * Copyright : 1995-2022 Imagination Technologies (c)
 * License	 : MIT
 *
 * Description  : Prototypes for Routine(s) that recursively traverse(s) a display
 *				  list during rendering.
 *
 * Platform     : ANSI
 *
 * Modifications:-
 * $Log: rntrav.h,v $
 * Revision 1.2  1995/07/27  11:00:49  sjf
 * Added prototype.
 *
 * Revision 1.1  1995/06/12  16:20:10  sjf
 * Initial revision
 *
 *
 **************************************************************************/


extern int RnTraverseDisplayList( const LIST_NODE_STRUCT   *pList, 
						   const CAMERA_NODE_STRUCT *pCamera);



/*
// End of file
*/

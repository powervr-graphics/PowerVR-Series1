/**************************************************************************
 * Name         : rnshadrj.h
 * Title        : Render: Shadow Reject
 * Author       : Simon Fenney
 * Created      : 19th August 1995
 *
 * Copyright : 1995-2022 Imagination Technologies (c)
 * License	 : MIT
 *
 * Description  : Prototypes for rnshadrj.c
 *
 * Platform     : ANSI
 *
 * Modifications:-
 * $Log: rnshadrj.h,v $
 * Revision 1.1  1995/08/27  15:31:15  sjf
 * Initial revision
 *
 **************************************************************************/


extern sgl_bool RnRejectBoxShadow( const BBOX_MINMAX_STRUCT * pBBox,
						  const sgl_vector LightVec,
						  const	sgl_bool   IsParallel);


/*
// END OF FILE
*/


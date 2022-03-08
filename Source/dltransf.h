/******************************************************************************
 * Name : dltransf.h
 * Title : 
 * Author : Stel Michael
 * Created : 30/06/1995
 *
 * Copyright : 1995-2022 Imagination Technologies (c)
 * License	 : MIT
 *
 * Description :
 *
 * Platform : ANSI compatible
 *
 * Modifications:-
 * $Log: dltransf.h,v $
 * Revision 1.4  1995/08/04  14:40:53  sjf
 * Removed routines that were made static.
 *
 * Revision 1.3  1995/07/06  12:06:53  sm
 * remove rangecheck proto
 *
 * Revision 1.2  1995/07/06  11:06:07  sm
 * added proto for check matrix
 *
 * Revision 1.1  1995/06/30  12:27:58  sm
 * Initial revision
 *
 *
 *****************************************************************************/

#if DEBUG
extern	int	CheckMatrix(TRANSFORM_STRUCT * Mat);
#endif

extern	void SetIdentityMatrix(TRANSFORM_STRUCT * Mat);




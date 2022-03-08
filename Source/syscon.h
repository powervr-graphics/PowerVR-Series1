/******************************************************************************
 * Name : syscon.h
 * Title : System configuration 
 * Author : Stel Michael
 * Created : 22/06/1995
 *
 * Copyright : 1995-2022 Imagination Technologies (c)
 * License	 : MIT
 *
 * Description :defines for system
 *
 *				Most of this is covered in TRDD027x.doc
 *
 * Platform : ANSI compatible
 *
 * Modifications:-
 * $Log: syscon.h,v $
 * Revision 1.3  1997/05/02  07:45:14  erf
 * Removed UINT32's etc and replaced with sgl_uint32's etc.
 *
 * Revision 1.2  1995/11/16  11:45:00  pdl
 * enabled multiple sabres.
 *
 * Revision 1.1  1995/07/24  10:43:18  sm
 * Initial revision
 *
 *
 *****************************************************************************/


extern sgl_uint32 NoOfSabres;

#define NUM_SABRE_CELLS	32
#define NUM_DEVICES	1


#define NUM_Y_REGIONS	2
#define NUM_X_REGIONS	2

/*---------------------------- End of File -------------------------------*/

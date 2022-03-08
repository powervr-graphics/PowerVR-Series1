/******************************************************************************
 * Name         : ldbmp.h
 * Title        : BMP manipulation routines.
 * Author       : Peter Leaback
 * Created      : 03/08/1995
 *
 * Copyright	: 1995-2022 Imagination Technologies (c)
 * License		: MIT
 *
 * Description  : sgl.h defines the bmp loaded. The functions below are used
 *				  by the simulator to write out the bmp.
 * 
 * Platform     : ANSI
 *
 * Modifications:
 * $Log: ldbmp.h,v $
 * Revision 1.1  1995/08/03  12:52:34  pdl
 * Initial revision
 *
 *
 *****************************************************************************/



unsigned int GetShort(FILE *fp);

unsigned long GetLong(FILE *fp);

void PutShort(FILE *fp,int i);

void PutLong(FILE *fp,long i);

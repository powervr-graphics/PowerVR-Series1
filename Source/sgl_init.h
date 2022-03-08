/**************************************************************************
 * Name         : sgl_init.h
 * Title        : Global variables Prototypes for display list editing in SGL
 * Author       : Simon James Fenney
 * Created      : 22 May 1995
 *
 * Copyright : 1995-2022 Imagination Technologies (c)
 * License	 : MIT
 *
 * Description  : Global variables Prototypes for display list editing in SGL
 *
 * Platform     : ANSI 
 *
 * Modifications:-
 * $Log: sgl_init.h,v $
 * Revision 1.4  1996/06/28  09:49:33  mal
 * Added the HighPriority and DefaultPriority global variables
 *
 * Revision 1.3  1995/05/30  12:29:34  sjf
 *  Made function name VL standard.
 *
 * Revision 1.2  1995/05/26  16:21:54  jrc
 * Corrected filename in file header.
 *
 * Revision 1.1  1995/05/26  12:39:42  sjf
 * Initial revision
 *
 *
 **************************************************************************/

/*
// Global that determines if the system is initialised. 
// It is Non Zero if it ash been initialised.
*/
extern int sglSystemInitialised;


/*
// Routine to actually initialise SGL. Returns 0 if successful.
*/
extern int SglInitialise(void);

/*
// Globals to identify the High and Default Priority Settings 
*/
extern int HighPrioritySetting;
extern int DefaultPrioritySetting;


/*
// End of file
*/



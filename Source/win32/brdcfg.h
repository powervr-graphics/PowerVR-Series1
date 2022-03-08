 /******************************************************************************
 * Name : brdcfg.h
 * Title : Midas Board Config   Header file
 * Author : Simon Fenney
 * Created : 27/06/1995
 *
 * Copyright	: 1995-2022 Imagination Technologies (c)
 * License		: MIT
 *
 * Description : Reads configuration file from windows to decide what
 *				 kind of board is being used...
 *
 * Platform : ANSI compatible
 *
 * Modifications:-
 * $Log: brdcfg.h,v $
 * Revision 1.5  1997/05/09  12:02:58  gdc
 * commented a couple of probably un-needed prototypes to stop compiler warnings
 * will remove if no probs
 *
 * Revision 1.4  1997/05/02  07:25:15  erf
 * Removed UINT32's etc and replaced with sgl_uint32 etc.
 *
 * Revision 1.3  1997/04/04  13:04:10  mjg
 * Added (for a change) a CALL_CONV.
 *
 * Revision 1.2  1996/11/07  15:59:31  jop
 * Added app hint stuff here instead
 *
 * Revision 1.1  1996/11/07  14:30:43  jop
 * Initial revision
 *
 * Revision 1.1  1995/10/25  17:51:18  sjf
 * Initial revision
 *
 ******************************************************************************/

/*
// Return TRUE if Midas should be configured for overlay mode, or FALSE for
// standalone.
*/
extern sgl_bool IsOverlayMode(void);

extern sgl_uint32 CALL_CONV HWRdValFile(char * RegName , sgl_uint32 DefaultVal);

/*extern sgl_bool CALL_CONV GetApplicationHintInt (char *pszHint, sgl_uint32 *pData);
**extern sgl_bool CALL_CONV GetApplicationHintFloat (char *pszHint, float *pData);
*/

/*
// End of da file
*/


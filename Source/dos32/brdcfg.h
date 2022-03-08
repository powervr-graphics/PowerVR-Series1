 /******************************************************************************
 * Name : brdcfg.h
 * Title : Midas Board Config   Header file
 * Author : Simon Fenney
 * Created : 27/06/1995
 *
 * Copyright : 1995-2022 Imagination Technologies (c)
 * License	 : MIT
 *
 * Description : Reads configuration file from windows to decide what
 *				 kind of board is being used...
 *
 * Platform : ANSI compatible
 *
 * Modifications:-
 * $Log: brdcfg.h,v $
 * Revision 1.3  1997/06/18  14:46:25  mjg
 *  Replaced Windows types (UINT and the rest) with sgl_ typedefs.
 *
 * Revision 1.2  1997/04/03  19:45:24  mjg
 * Added CALL_CONVs
 *
 * Revision 1.1  1997/03/31  19:01:07  mlj
 * Initial revision
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

extern UINT32 CALL_CONV HWRdValFile(char * RegName , sgl_uint32 DefaultVal);


extern sgl_bool GetApplicationHintInt (char *pszHint, sgl_int32 *pData);
extern sgl_bool GetApplicationHintFloat (char *pszHint, float *pData);

extern sgl_uint32 CALL_CONV InitEnvironment(void);

/*
// End of da file
*/


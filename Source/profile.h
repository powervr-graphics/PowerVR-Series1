/*****************************************************************************
;++
Name            :   $Source: /user/rcs/revfiles/sabre/sgl/RCS/profile.h,v $
Title           :   Portable profile (ini file) services
C Author        :   Jim Page
Created         :   19/04/95

Copyright		: 	1995-2022 Imagination Technologies (c)
License			: 	MIT

Description     :   Like Windows profile commands
                    
Program Type    :   C module (ANSI)

RCS info:

  $Date: 1997/04/03 17:52:50 $
  $Revision: 1.4 $
  $Locker:  $
  $Log: profile.h,v $
 * Revision 1.4  1997/04/03  17:52:50  mjg
 * Another CALL_CONV.
 *
 * Revision 1.3  1996/10/03  16:33:30  mal
 * Reverted back to the previous version.
 *
 * Revision 1.2  1996/10/02  17:22:27  mal
 * Added the two functions: SglTimeNow() and SglCPUFreq()
 *
 * Revision 1.1  1995/09/22  19:18:30  jop
 * Initial revision
 *
;--
*****************************************************************************/

/*===========================================
 * Function:	SglWritePrivateProfileString
 *===========================================
 *
 * Scope:		SGL
 *
 * Purpose:		Write a string into an ini file, section and entry, creating new ones
 *				if they don't exist.
 *
 * Params:		char *szSection: secion in ini file
 *				char *szEntry: entry in section
 *				char *szText: text to insert
 *				char *szFilename: name of initialisation file
 *
 * Return:		TRUE if successfully written
 *========================================================================================*/
sgl_bool CALL_CONV SglWritePrivateProfileString (char *szSection, char *szEntry, char *szText, char *szFilename);

/*===========================================
 * Function:	SglWritePrivateProfileInt
 *===========================================
 *
 * Scope:		SGL
 *
 * Purpose:		Write a number (int) into an ini file, section and entry, creating new ones
 *				if they don't exist.
 *
 * Params:		char *szSection: secion in ini file
 *				char *szEntry: entry in section
 *				int nValue: number to insert
 *				char *szFilename: name of initialisation file
 *
 * Return:		TRUE if successfully written
 *========================================================================================*/
sgl_bool CALL_CONV SglWritePrivateProfileInt (char *szSection, char *szEntry, int nValue, char *szFilename);

/*===========================================
 * Function:	SglReadPrivateProfileString
 *===========================================
 *
 * Scope:		SGL
 *
 * Purpose:		read a string from an ini file, section and entry, creating new ones
 *				if they don't exist.
 *
 * Params:		char *szSection: ini file section
 *				char *szEntry: entry in section
 *				char *szDefault: default string to return if unsuccsesful (non NULL!)
 *				char *szText: buffer for return string
 *				int nTextSize: length of buffer
 *				char *szFilename: name of ini file
 *
 * Return:		TRUE if successfully read
 *========================================================================================*/
sgl_bool CALL_CONV SglReadPrivateProfileString (char *szSection, char *szEntry, char *szDefault, char *szText, int nTextSize, char *szFilename);

/*===========================================
 * Function:	SglReadPrivateProfileInt
 *===========================================
 *
 * Scope:		SGL
 *
 * Purpose:		read a string from an ini file, section and entry, creating new ones
 *				if they don't exist.
 *
 * Params:		char *szSection: ini file section
 *				char *szEntry: entry in section
 *				char *Default: default number to return on failure
 *				char *szFilename: name of ini file
 *
 * Return:		int value from ini file if successful, or nDefault on failure
 *========================================================================================*/

int CALL_CONV SglReadPrivateProfileInt (char *szSection, char *szEntry, int nDefault, char *szFilename);


/* end of $Source: /user/rcs/revfiles/sabre/sgl/RCS/profile.h,v $ */


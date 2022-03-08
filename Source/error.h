/******************************************************************************
 * Name  		: error.h
 * Title        : Error and warning functions.
 * Author  	    : John Catchpole
 * Created  	: 11/05/1995
 *
 * Copyright	: 1995-2022 Imagination Technologies (c)
 * License		: MIT
 *
 * Description  : See comment below.
 * 
 * Platform     : ANSI
 *
 * Modifications:
 * $Log: error.h,v $
 * Revision 1.12  1997/04/07  12:13:30  gdc
 * added prototype for SglGetLastWarning function
 *
 * Revision 1.11  1997/04/03  17:52:19  mjg
 * Another CALL_CONV.
 *
 * Revision 1.10  1996/07/05  07:55:41  sjf
 * Red sky etc ... added sgl warning.
 *
 * Revision 1.9  1996/06/27  11:42:41  jrc
 * Removed warning stuff and updated comment because error handling stuff is
 * now used for warnings as well.
 *
 * Revision 1.8  1996/06/26  17:11:57  jrc
 * Added visibility of gnMostRecentWarning for functions to set it to zero
 * when they start (via SglInitialise).
 *
 * Revision 1.7  1996/06/26  14:37:08  jrc
 * Rewrote the comment for SglError to include stuff about warning codes.
 *
 * Revision 1.6  1995/05/31  16:46:21  jrc
 * Ammended comment because we now accept a name (positive value) as no error.
 *
 * Revision 1.5  1995/05/25  17:29:56  jrc
 * Added a usage warning.
 *
 * Revision 1.4  1995/05/25  14:19:42  sjf
 * Wasted time changing identifiers to vl standard.
 *
 * Revision 1.3  1995/05/12  11:14:47  jrc
 * Now using lower case 'sgl_no_err'.
 *
 * Revision 1.2  1995/05/11  12:45:03  jrc
 * Added reminder to comment.
 *
 * Revision 1.1  1995/05/11  12:35:22  jrc
 * Initial revision
 *
 *****************************************************************************/


/*
// --------
// SglError
// --------
// Each public SGL function should use SglError to update the error/no error
// status on exit.  (This status is read by sgl_get_errors.)  SglError should
// be called exactly once per public function.
//
// sgl_no_err and positive values (e.g. name values) are taken to mean no
// error: a positive value is stored as sgl_no_err but the return value is
// always the same as the input.
//
// Returns the 'nCode' parameter to enable 'return SglError(code)' instead of
// 'SglError(code); return(code);'.  Of course the return value from SglError
// can be ignored.
*/
int  CALL_CONV SglError ( int nError );


/*
// ----------
// SglWarning
// ----------
// This should be called to generate a warning status in a public function.  If
// it is not called the warning status will be set to zero (sgl_no_err) for
// that function.
*/
void CALL_CONV  SglWarning ( int nWarning );

/*
// ----------
// SglGetLastWarning
// ----------
// This is required because dlmesh.c reports colinear face warnings directly  
// It used to use the gnWarning varaible, but we are trying to reduce
// global variables
*/
int CALL_CONV  SglGetLastWarning (void );

/*------------------------------- End of File -------------------------------*/

/******************************************************************************
 * Name         : error.c
 * Title        : Definitions of the SGL error handling functions.
 * Author  	    : John Catchpole
 * Created  	: 11/05/1995
 *
 * Copyright	: 1995-2022 Imagination Technologies (c)
 * License		: MIT
 *
 * Description  : The SGL API function 'sgl_get_errors' and the function
 *                'SglError'.  See comment in 'error.h'.
 * 
 * Platform     : ANSI
 *
 * Modifications:
 * $Log: error.c,v $
 * Revision 1.16  1997/04/07  12:13:01  gdc
 * change scope of gnWarning from global to static and added an internal
 * function to interogate it
 *
 * Revision 1.15  1997/04/03  17:51:08  mjg
 * Another CALL_CONV.
 *
 * Revision 1.14  1997/04/03  13:12:53  mjg
 * Updated the calling convention.
 *
 * Revision 1.13  1996/07/11  16:39:47  sjf
 * Needed to swap order of sgl.h and sgl_defs.h includes.
 *
 * Revision 1.12  1996/07/05  07:55:15  sjf
 * Added sgl warning.
 *
 * Revision 1.11  1996/06/27  11:41:57  jrc
 * Removed sgl_get_warnings: old error handling is now used for warnings as
 * well.
 *
 * Revision 1.10  1996/06/26  17:10:54  jrc
 * Essential additions to the warning scenario.
 *
 * Revision 1.9  1996/06/26  14:36:19  jrc
 * Added warning code handling for SglError and the new function
 * sgl_get_warnings.
 *
 * Revision 1.8  1995/11/30  11:37:26  pdl
 * added a debug message when there is an sgl error.
 *
 * Revision 1.7  1995/05/31  16:46:21  jrc
 * Now SglError returns exactly the same value as it was given.
 *
 * Revision 1.6  1995/05/31  16:20:29  jop
 * Made SglError regard +ve codes as sgl_no_error
 *
 * Revision 1.5  1995/05/25  17:29:56  jrc
 * Changed function headers to VL standard.
 *
 * Revision 1.4  1995/05/25  14:19:24  sjf
 * Wasted time changing identifiers to vl standard.
 *
 * Revision 1.3  1995/05/12  11:14:47  jrc
 * Now using lower case 'sgl_no_err'.
 *
 * Revision 1.2  1995/05/11  14:19:40  jrc
 * Changed #include to correct file for SGL_NO_ERR definition.
 *
 * Revision 1.1  1995/05/11  12:39:10  jrc
 * Initial revision
 *
 *****************************************************************************/


#include "sgl_defs.h"
#include "sgl.h"


/*
// Storage for errors:
*/
static int  gnEarliest = sgl_no_err, gnMostRecent = sgl_no_err;


/*
// Storage for warnings - interogatted by the internal SglGetLastWarning()
*/
static int gnWarning = sgl_no_err;


/******************************************************************************
 * Function Name: SglError
 *
 * Inputs       : nError
 * Outputs      : -
 * Returns      : nError
 * Globals Used : gnEarliest, gnMostRecent
 *
 * Description  : See comment in 'error.h'.
 *****************************************************************************/
int CALL_CONV SglError ( int nError )
{
	#if DEBUG
	if (nError < 0)
	{
   		DPF((DBG_ERROR, "Sgl error number %d", nError));
	}
	#endif


	if (nError >= 0)
	{
		gnMostRecent = gnWarning; /* warning for this function or sgl_no_err */
	}
	else
	{
		gnMostRecent = nError;
	}


	if (gnEarliest == sgl_no_err) 
	{
		gnEarliest = gnMostRecent;
	}


	/*
	// Clear the warning state for the next function.
	*/
	gnWarning = sgl_no_err;


	/*
	// Ignore the warning state in the return value.
	*/
	return nError;

} /* SglError */


/******************************************************************************
 * Function Name: SglWarning
 *
 * Inputs       : nWarning
 * Outputs      : -
 * Returns      : -
 * Globals Used : gnWarning
 *
 * Description  : See comment in 'error.h'.
 *****************************************************************************/
void CALL_CONV SglWarning ( int nWarning )
{
	gnWarning = nWarning;
}

/******************************************************************************
 * Function Name: SglWarning
 *
 * Inputs       : nWarning
 * Outputs      : -
 * Returns      : -
 * Globals Used : gnWarning
 *
 * Description  : See comment in 'error.h'.
 *****************************************************************************/
int CALL_CONV SglGetLastWarning ( )
{
	return gnWarning;
}

/******************************************************************************
 * Function Name: sgl_get_errors
 *
 * Inputs       : -
 * Outputs      : pnEarliest, pnMostRecent
 * Returns      : -
 * Globals Used : gnEarliest, gnMostRecent, gnWarning
 *
 * Description  : Public function: see API document.
 *****************************************************************************/
void CALL_CONV sgl_get_errors ( int *pnEarliest, int *pnMostRecent )
{
    *pnEarliest   = gnEarliest;
	*pnMostRecent = gnMostRecent;

    gnEarliest = gnMostRecent = sgl_no_err;
}


/*------------------------------- End of File -------------------------------*/








/*****************************************************************************
;++
Name           	:   $RCSfile: debug.h,v $
Title           :   DEBUG.H
C Author       	:   Jim Page
Created         :   20/4/95

Copyright		: 1995-2022 Imagination Technologies (c)
License			: MIT

Description     :   Debug macros that access debug.c functions
                    
Program Type    :   C module (.dll)

RCS info:

  $Date: 1997/11/24 10:41:44 $
  $Revision: 1.26 $
  $Locker:  $
  $Log: debug.h,v $
 * Revision 1.26  1997/11/24  10:41:44  sxy
 * added a macro DPFTIME(()) for timing build.
 *
 * Revision 1.25  1997/11/10  16:26:57  sxy
 * added the function DebugDeinit() in developer's driver.
 *
 * Revision 1.24  1997/10/16  11:11:00  gdc
 * chnaged developer build stuff to make it tidier
 *
 * Revision 1.23  1997/10/14  14:00:48  sxy
 * added a macro DPFDEVF() for output to log file in developer's driver.
 *
 * Revision 1.22  1997/10/02  16:33:06  sxy
 * added a function dfprintf() and a macro DFPFDEV to print debug messages into
 * a log file for developers.
 *
 * Revision 1.21  1997/09/24  08:57:55  sxy
 * Added a new macro DPFDEV for build DEBUGDEV which is a developer's version.
 * DEBUG build displays DPF, DPFOO and DPFDEV while DEBUGDEV build only display
 * DPFDEV that contents FATAL, ERROR and some of WARNING level messages of DPF
 * in a clean and clear manner.
 *
 * Revision 1.20  1997/06/04  13:24:43  gdc
 * changed to stop compiler warnings
 *
 * Revision 1.19  1997/04/08  11:08:27  jop
 * Added CALL_CONV to dprintf and dassert so they can be used from
 * PVROS stuff
 *
 * Revision 1.18  1997/04/03  19:28:28  jop
 * Cleaned things up a bit
 *
 * Revision 1.17  1997/03/27  19:21:22  erf
 * Removed routines to monitor memory management.
 *
 * Revision 1.16  1996/07/08  04:15:29  sjf
 * Added temp define to allow mem logging  of release builds.
 *
 * Revision 1.15  1996/05/09  12:03:02  sjf
 * Added "Ignore" option to assert.
 *
 * Revision 1.14  1996/02/26  15:36:14  sjf
 * added memory usage trace functions
 *
 * Revision 1.13  1995/12/19  20:24:11  jop
 * Fixed warning
 *
 * Revision 1.12  1995/09/05  10:04:47  sjf
 * WHOOPS, left an invalid # construct in that last version that I was
 * using to trace what #if's were being used.
 *
 * Revision 1.11  1995/09/05  09:07:57  sjf
 * Unfortunately, the last change meant that debug code
 * automatically got included in the NON debug versions. I have
 * changed the #if defined  back to #if  BUT have made sure
 * that DEBUG and _DEBUG are always defined. (to be 0 if they
 * were undefined)
 *
 * Revision 1.10  1995/09/04  13:14:25  jop
 * changed #if to #if defined()
 *
 * Revision 1.9  1995/07/26  19:14:59  sjf
 * Made debug more verbose (prints file and line)
 *
 * Revision 1.8  1995/07/23  17:04:40  jop
 * Added Init and Deinit functions for windowy type stuff
 *
 * Revision 1.7  1995/07/17  16:03:54  jop
 * Changed refs to PSTR to char *
 *
 * Revision 1.6  1995/07/14  15:29:32  sjf
 * 1) Fixed syntax error with DPFOO
 * 2) Fixed problem if user accidently forgot to define a module
 *   id. This would cause all debugging code to be ignored. Now uses
 *   a default id if none is defined.
 *
 *
 * Revision 1.5  1995/07/14  13:04:08  sjf
 * Added DPFOO - i.e. DPF that prints once only.
 *
 * Revision 1.4  1995/07/10  10:41:33  sjf
 * Fixed problem with NEC compiler and #if and long ints.
 *
 * Revision 1.3  1995/06/02  13:22:43  jop
 * Now checks that MODULE_ID is non-zero to enable debugging
 *
 * Revision 1.2  1995/04/27  13:37:44  jop
 * Added DebugSetDebugLevel function
 *
 * Revision 1.1  1995/04/20  14:56:08  jop
 * Initial revision
 *

;--
*****************************************************************************/

/*
 * Interface to debug macros (in DEBUG builds):
 *
 * ASSERT:
 *
 * 		example:  ASSERT (x1 == x2);
 *
 * 		If the statement passed is true, nothing happens. If false, 
 *		the following is printed to stderr:
 *
 * 		Assertion (x1 == x2) failed! File xxx.c, Line n. 
 *		Continue or Abort [c/a]?
 *
 *		If A is pressed, abort() is called; if C is pressed the function returns.
 *
 * DPF:
 *
 *		example:  DPF ((DBG_MESSAGE, "Rectangle: %d, %d, %d, %d", x1, y1, x2, y2));
 *
 *		If MESSAGE level tracing is enabled for the module in which DPF is called,
 *
 *		Rectangle: n1, n2, n3, n4
 *
 *		The 2 sets of brackets are mandatory I'm afraid.
 *
 * DPFOO:
 * 		Is a "once only" version of DPF so that the message will only be displayed
 *		a single time.
 *
 *		example:  DPFOO ((DBG_MESSAGE, "Rectangle: %d, %d, %d, %d", x1, y1, x2, y2));
 *
 *
 *
 * DPFDEV:
 *
 * 		Is a "developer" version of DPF so that the message will only be displayed
 *		in level FATAL or ERROR for developers.
 *
 *		example:  DPFDVE ((DBG_MESSAGE, "Rectangle: %d, %d, %d, %d", x1, y1, x2, y2));
 *		
 */


#ifndef __DEBUG_H__

#define __DEBUG_H__


/*
// If debug is not defined, then define it to be zero
*/

#ifndef DEBUG 
  #ifdef _DEBUG
   #define DEBUG _DEBUG
  #else 
   #define DEBUG 0
  #endif
#endif

#ifndef _DEBUG
#define _DEBUG DEBUG
#endif

/* 
 * These are privately used by DEBUG.C - don't use them, use the DBG_ defines.
 */

enum
{
	DBGPRIV_FATAL,
	DBGPRIV_ERROR,
	DBGPRIV_WARNING,
	DBGPRIV_MESSAGE,
	DBGPRIV_VERBOSE
};

/* 
 * I hope everyone will let me off this little workaround as it adds features
 * without typing loads of stuff each time!
 */

#define	DBG_FATAL		MODULE_ID,DBGPRIV_FATAL,__FILE__, __LINE__
#define DBG_ERROR		MODULE_ID,DBGPRIV_ERROR,__FILE__, __LINE__
#define	DBG_WARNING		MODULE_ID,DBGPRIV_WARNING,__FILE__, __LINE__
#define	DBG_MESSAGE		MODULE_ID,DBGPRIV_MESSAGE,__FILE__, __LINE__
#define	DBG_VERBOSE		MODULE_ID,DBGPRIV_VERBOSE,__FILE__, __LINE__


#ifndef __DEBUG_C__


	/*
	// For the times when MODULE_ID hasn't been defined (by accident)
	// or has been defined too late ...
	*/
	#ifndef MODULE_ID
	#define MODULE_ID MODID_DEFAULT
/*	#pragma message ("File " __FILE__ ": MODULE_ID not #define'd (above debug.h) - Using default ID")*/
	#endif 

	/*
	// Due to limitation in NEC compiler (i.e. can't handle Longs in
	// #if commands, have to put "defined" in front of MODULE_ID.
	*/
	#if ((DEBUG || _DEBUG) && defined(MODULE_ID) && !DEBUGDEV)

		/* debug version - display all messages */
		#define ASSERT(x)	{ static int ask = 1;\
							  if(!(x)&& ask) \
								 dassert (#x, __FILE__, __LINE__, &ask);\
							}
  

		#define DPF(x) 		dprintf x
		#define DPFOO(x)	if(1){static char printed = 0; if(!printed){dprintf x; printed = 1;}}
		#define DPFDEV(x) 	 dprintf x
		#define DPFTIME(x)   

		void CALL_CONV dprintf (sgl_int32 nModuleID,  sgl_int32 nDebugLevel, char* FileName, int Line, char *szFormat, ...);
		void CALL_CONV dassert (char *szAssertText, char *szFile, sgl_int32 nLine, int *ask);
		void DebugSetDebugLevel (sgl_int32 nCodeModule, sgl_int32 nDebugLevel);
		void DebugInit (sgl_uint32 hInstance);
		void DebugDeinit (void);

	#elif DEBUGDEV

	   /* developers version - only display DPFDEV(())  */
		#define ASSERT(x)	
		#define DPF(x)
		#define DPFOO(x)
		#define DPFDEV(x) 		dfprintf x
		#define DPFTIME(x) 		
       
		void CALL_CONV dfprintf (sgl_int32 nModuleID,  sgl_int32 nDebugLevel, char* FileName,
						 int Line, char *szFormat, ...);
		void DebugInit (sgl_uint32 hInstance);
		void DebugDeinit (void);

	#elif TIMING

	   /* developers timing version - only display DPFTIME(())  */
		#define ASSERT(x)	
		#define DPF(x)
		#define DPFOO(x)
		#define DPFDEV(x) 		
		#define DPFTIME(x) 		dtprintf x
       
		void CALL_CONV dtprintf (char *szFormat, ...);
		void DebugInit (sgl_uint32 hInstance);
		void DebugDeinit (void);
			
	#else
		/* release version - all stripped out */

		#define ASSERT(x)
		#define DPF(x)
		#define DPFOO(x)
		#define DPFDEV(x)
		#define DPFTIME(x) 
        #define DebugInit(x)
        #define DebugDeinit()

	#endif

#endif

#endif /* __DEBUG_H__ */

/* 
// end of File 
*/

/******************************************************************************
 * Name         : pvrosmem.h
 * Title        : Defines memory allocation/deallocation macros.
 * Author       : Edward Forde
 * Created      : 26/03/1997
 *
 * Copyright	: 1995-2022 Imagination Technologies (c)
 * License		: MIT
 *
 * Description  : Use defines the allocate/deallocate memory so that there aren't
 *				  operating system specific. Defines the Windows specific calls.
 * 
 * Platform     : ANSI
 *
 * Modifications:
 * $Log: pvrosmem.h,v $
 * Revision 1.3  1997/04/08  11:45:25  jop
 * Added CALL_CONV to memory logging routines
 *
 * Revision 1.2  1997/04/08  11:10:34  jop
 * *** empty log message ***
 *
 * Revision 1.1  1997/03/27  19:31:46  erf
 * Initial revision
 *
 *
 *
 *****************************************************************************/


#ifndef __PVROSMEM_H__

#define __PVROSMEM_H__

#if !DEBUG || defined (__PVROSMEM_C__)

#define	PVROSMalloc(X)		malloc(X)
#define	PVROSCalloc(X,Y)	calloc(X,Y)
#define	PVROSRealloc(X,Y)	realloc(X,Y)
#define	PVROSFree(X)		free(X)

#else

#define	PVROSMalloc(X) 		PVROSLogMalloc	((X),		__FILE__, __LINE__)
#define	PVROSCalloc(X,Y)	PVROSLogCalloc	((X), (Y),  __FILE__, __LINE__)
#define	PVROSRealloc(X,Y)	PVROSLogRealloc	((X), (Y),  __FILE__, __LINE__)
#define	PVROSFree(X)		PVROSLogFree	((X),		__FILE__, __LINE__)

/* Prototypes for routines.
 */
void CALL_CONV InitLogMemFile(void);
void * CALL_CONV PVROSLogMalloc(size_t size, char * fname, int lineNum);
void * CALL_CONV PVROSLogCalloc(size_t count, size_t size, char * fname, int lineNum);
void * CALL_CONV PVROSLogRealloc(void* ptr, size_t size, char * fname, int lineNum);
void CALL_CONV PVROSLogFree(void* ptr, char * fname, int lineNum);
#endif

#endif /* __PVROSMEM_H__ */
/*
// END OF FILE
*/


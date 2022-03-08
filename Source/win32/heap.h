/*****************************************************************************
 *;++
 * Name           : $RCSfile: heap.h,v $
 * Title          : HEAP.H
 * C Author       : Jim Page (nicked from microsoft demo sample driver)
 * Created        : 21/04/95
 *
 * Copyright	: 1995-2022 Imagination Technologies (c)
 * License		: MIT
 *
 * Description    :	WIN32 heap services (shared!)
 *                   
 * Program Type   :	WIN32 DLL
 *
 * RCS info:
 *
 * $Date: 1997/06/02 02:42:00 $
 * $Revision: 1.1 $
 * $Locker:  $
 * $Log: heap.h,v $
 * Revision 1.1  1997/06/02  02:42:00  jop
 * Initial revision
 *
 *
 *;--
 *****************************************************************************/

#ifndef __HEAP_C__
#define __HEAP_C__

/*
 * MemAlloc - allocate memory from our global pool
 */
void * MemAlloc( unsigned long size );

/*
 * MemSize - return size of object
 */
unsigned long MemSize( void * lptr );

/*
 * MemFree - free memory from our global pool
 */
void MemFree( void * lptr );

/*
 * MemReAlloc
 */
void * MemReAlloc( void * lptr, unsigned long size );

/*
 * MemInit - initialize the heap manager
 */
int MemInit( void );

/*
 * MemFini - finished with our heap manager
 */
void MemFini( void );

#endif

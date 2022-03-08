/**
 * Copyright	: 1995-2022 Imagination Technologies (c)
 * License		: MIT
 */
#ifndef __PVROSIO_H__
#define __PVROSIO_H__

#define IW(ptr,offset,val)		(((unsigned long *) ptr)[(offset)]=(val))
#define FW(ptr,offset,val)		(((float *) ptr)[(offset)]=(val))
#define IORW(ptr,offset,val)	(((unsigned long *) ptr)[(offset)]|=(val))

#define IWREG(ptr,offset,val)	IW(ptr,offset,val)
#define FWREG(ptr,offset,val)	FW(ptr,offset,val)

#define IR(ptr,offset)			(((unsigned long *) ptr)[(offset)])
#define FR(ptr,offset)			(((float *) ptr)[(offset)])

#endif

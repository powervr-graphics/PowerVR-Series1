
#ifndef __PVROSIO_H__
#define __PVROSIO_H__

#define IW(ptr,offset,val)		(((unsigned long *) ptr)[(offset)]=(val))
#define FW(ptr,offset,val)		(((float *) ptr)[(offset)]=(val))
#define IORW(ptr,offset,val)	(((unsigned long *) ptr)[(offset)]|=(val))

#define IWREG(ptr,offset,val)	IW(ptr,offset,val)
#define FWREG(ptr,offset,val)	FW(ptr,offset,val)

#define IR(ptr,offset)			(((unsigned long *) ptr)[(offset)])
#define FR(ptr,offset)			(((float *) ptr)[(offset)])

#ifdef __WATCOMC__

void OUT8  (sgl_uint16 Base, sgl_uint16 Offset, sgl_uint32 Val);
void OUT16 (sgl_uint16 Base, sgl_uint16 Offset, sgl_uint32 Val);
void OUT32 (sgl_uint16 Base, sgl_uint16 Offset, sgl_uint32 Val);

#pragma aux OUT8 =		\
	"add	dx, cx"		\
	"out	dx, al"		\
	parm [dx] [cx] [al]	\
	modify [dx];

#pragma aux OUT16 =		\
	"add	dx, cx"		\
	"out	dx, ax"		\
	parm [dx] [cx] [ax]	\
	modify [dx];

#pragma aux OUT32 =		\
	"add	dx, cx"		\
	"out	dx, eax"	\
	parm [dx] [cx] [eax]\
	modify [dx];

sgl_uint8  IN8  (sgl_uint16 Base, sgl_uint16 Offset);
sgl_uint16 IN16 (sgl_uint16 Base, sgl_uint16 Offset);
sgl_uint32 IN32 (sgl_uint16 Base, sgl_uint16 Offset);

#pragma aux OUT8 =		\
	"add	dx, cx"		\
	"in		al, dx"		\
	parm [dx] [cx]		\
	modify [dx]			\
	value [al];

#pragma aux OUT16 =		\
	"add	dx, cx"		\
	"in		ax, dx"		\
	parm [dx] [cx]		\
	modify [dx]			\
	value [ax];
	
#pragma aux OUT32 =		\
	"add	dx, cx"		\
	"in		eax, dx"	\
	parm [dx] [cx]		\
	modify [dx]			\
	value [eax];

sgl_uint32 RDMSR(void *Val);

/* Read the  Time Stamp Counter */

#pragma aux RDMSR = 		\
	"db 	0Fh	"			\
	"db 	31h	"			\
	"shrd	eax,edx,7"		\
	"shr	edx,7"			\	
	"mov	0[ebx], eax"	\
	"mov	4[ebx],	edx"	\
	parm   [ebx] 			\
	modify [eax edx];

void ResetTSC( void );

/* Reset the  Time Stamp Counter (used when querying frequency) */

#pragma aux ResetTSC = 		\
	"mov 	ecx, 010h"		\
	"mov 	edx, 000h"		\
	"mov 	eax, 000h"		\	
	"db 	0Fh	"			\
	"db 	30h	"			\
	modify [eax ecx edx];


#endif /*__WATCOMC__*/

#endif /*__PVROSIO_H__*/

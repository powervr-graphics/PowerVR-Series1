/*
 * Copyright	: 1995-2022 Imagination Technologies (c)
 * License		: MIT
 * brdsetup.h 
 */

#if PCX2 || PCX2_003
	#define POINTERS_IN_TLB	1
	#define POINTERS_FOLLOW_PLANES 0

	#if POINTERS_IN_TLB
		#if POINTERS_FOLLOW_PLANES
			#pragma message ("Region data in TLB (following plane data)")
		#else
			#pragma message ("Region data in TLB (separate partition to plane data)")
		#endif
	#else
		#pragma message ("Configured for seperate object pointer buffer")
	#endif
#endif


#define PCX1_DEVICE_ID				0x002A
#define PCX2_DEVICE_ID				0x0046

#if !WIN32
#define PCX_REGION_DATA_PAGES			128

typedef struct tagPCXBUFFER
{
	unsigned long PageSize;
	unsigned long LinearAddress;
	unsigned long SlotsUsed;
	unsigned long PhysAddresses[128];
	unsigned long Limit[128];
} PCXBUFFER;

extern PCXBUFFER PCXBuffer;
#else
#define PCX_REGION_DATA_PAGES			256
/* PCXBUFFER struct defined in pvrosapi.h */
#endif

/* end of $RCSfile: brdsetup.h,v $ */


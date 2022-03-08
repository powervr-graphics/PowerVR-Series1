
#include "metrics.h"

SGL_EXTERN_TIME_REF /* if we are timing code */

#ifdef ZEUS_ARCADE
#include <mips/r4kc0.h>
#endif

#if DEBUGDEV || TIMING
sgl_uint32 nTotalPolygonsInFrame, nTransPolygonsInFrame, nOpaquePolygonsInFrame;
#endif

#ifdef WIN32

/************************************************/
/*												*/
/* Non-Arcade performance measurement functions	*/
/*												*/
/************************************************/

#pragma warning ( disable : 117 )
#include "windows.h"
#pragma warning ( default : 117 )

/*===========================================
 * Function:	SglTimeNow 
 *===========================================
 *
 * Scope:		SGL
 *
 * Purpose:		to get a clock tick quickly and effeciently
 *
 * Params:		none
 *
 * Return:		unsigned int value being the current 32bit clock tick with ref
 *		        to the CPU Speed.
 *========================================================================================*/
sgl_uint32  SglTimeNow(void)
{
	static sgl_uint32 Time;

	__asm
		{
			push	eax
			push	edx

			_emit 0Fh
			_emit 31h	

			shrd	eax, edx, 4

			mov		Time, eax
			pop		edx
			pop		eax
		}

	return Time;
}

/*===========================================
 * Function:	SglCPUFreq
 *===========================================
 *
 * Scope:		SGL
 *
 * Purpose:		to return the CPU Frequency
 *
 * Params:		none
 *
 * Return:		float value being the the CPU frequency.
 *========================================================================================*/

float  SglCPUFreq(void)
{
 static sgl_uint32 Time1, Time2;
   LARGE_INTEGER AccClock1, AccClock2, AccFreq;
   sgl_uint32 AccTime;
   float TPS;

   QueryPerformanceCounter( &AccClock1);
   Time1 = SglTimeNow();
      Sleep(1000);
   Time2 = SglTimeNow();
   QueryPerformanceCounter( &AccClock2);

   QueryPerformanceFrequency( &AccFreq );

   Time1 = (Time2 - Time1);
   AccTime = ( AccClock2.LowPart - AccClock1.LowPart);
   TPS = (float)Time1 / ((float)AccTime/(float)(AccFreq.LowPart));

   if ( Time1 != 0 ) 
      return TPS;
   else
      return 1.0f;

}
#else

#if defined (MIDAS_ARCADE) || defined (ZEUS_ARCADE)

/* The unmodified tick frequency for our boards is nearly 100MHz which	*/
/* means we don't get a lot of elapsed time (around 42 seconds) in our	*/
/* 32 bit counters.  So	to avoid overflow we shift by the value below.	*/

#define	TICK_SHIFTER	4		/*	Divide by 16 */
#define	BIG_TICK		(0x00000001 << (32 - TICK_SHIFTER))

/************************************************/
/*												*/
/* Arcade performance measurement functions		*/
/*												*/
/************************************************/

sgl_uint32 HWGetTick(void);
static sgl_uint32	BigTick = 0;
static sgl_uint32	LastTick = 0;
static sgl_bool BallRolling = FALSE;

/*===========================================
 * Function:	SglTimeNow - ARCADE VERSION
 *===========================================
 *
 * Scope:		SGL
 *
 * Purpose:		to get a clock tick quickly and effeciently
 *
 * Params:		none
 *
 * Return:		unsigned int value being the current 32bit clock tick with ref
 *		        to the CPU Speed.
 *========================================================================================*/
sgl_uint32  SglTimeNow(void)
{
	sgl_uint32	u32Temp;


	u32Temp = (HWGetTick() >> TICK_SHIFTER) & 0x0FFFFFFF;

	/* Have we gone around the clock again ?? */
	if (BallRolling)
	{
		if (u32Temp < LastTick)
		{
			/* Yup - we've gone around the clock */
			BigTick += BIG_TICK;
		}
	}
	else
	{
		BallRolling = TRUE;
	}
	LastTick = u32Temp;

	return(u32Temp + BigTick);
}

/*===========================================
 * Function:	SglCPUFreq - ARCADE VERSION
 *===========================================
 *
 * Scope:		SGL
 *
 * Purpose:		to return the CPU Frequency
 *
 * Params:		none
 *
 * Return:		float value being the the CPU frequency.
 *========================================================================================*/

long _sbd_cpufreq (void);

float  SglCPUFreq(void)
{
	  return (float)( /* (_sbd_cpufreq()) */ 100000000 >> TICK_SHIFTER);
}

/*===========================================
 * Function:	SglMetricInit - ARCADE VERSION
 *===========================================
 *
 * Scope:		SGL
 *
 * Purpose:		to initialise metrics stuff
 *
 * Params:		none
 *
 * Return:		none
 *========================================================================================*/
void  SglMetricInit(void)
{
	BigTick = 0;
	LastTick = 0;
	BallRolling = FALSE;
}

#ifdef ZEUS_ARCADE
sgl_uint32 HWGetTick(void)
{
	sgl_uint32 ticks;
	
	ticks = r4kc0_get(9); /* Read coprocessor 0 counter */
	
	return ticks;
}

#endif /*ZEUS_ARCADE*/


#endif

#endif

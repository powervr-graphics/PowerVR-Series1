/******************************************************************************
 * Name : hwinterf.h
 * Title : 
 * Author : Stel Michael
 * Created : 22/06/1995
 *
 * Copyright : 1995-2022 Imagination Technologies (c)
 * License	 : MIT
 *
 * Description :Prototypes for hardware interface routines
 *
 *				Most of this is covered in TRDD027x.doc
 *
 * Platform : ANSI compatible
 *
 * Modifications:-
 * $Log: hwinterf.h,v $
 * Revision 1.43  1997/08/08  19:43:45  mjg
 * Removed global bilinear option for PCX2_003 (no longer a feature).
 *
 * Revision 1.42  1997/07/24  17:08:18  gdc
 * remved HWStartRender proto to hwregs.h
 *
 * Revision 1.41  1997/07/14  19:29:06  mjg
 * Added the PCX2_003 compile switch.
 *
 * Revision 1.40  1997/07/14  10:00:24  gdc
 * removed compile message about FORCE_FPU
 *
 * Revision 1.39  1997/06/17  09:22:20  mjg
 * Added the FORCE_NO_FPU option.
 *
 * Revision 1.38  1997/06/11  16:32:31  mjg
 * Added parameter to HWStartRender and HWStartRenderStrip.
 *
 * Revision 1.37  1997/05/02  07:45:14  erf
 * Removed UINT32's etc and replaced with sgl_uint32's etc.
 *
 * Revision 1.36  1997/04/21  15:26:15  mjg
 * Replaced ParamBufferInfo with PVRParamBuffs.
 *
 * Revision 1.35  1997/04/15  13:08:02  mjg
 * Tidied up and made HWSetupBunchOfBRIDGERegs() ISPTSP only.
 *
 * Revision 1.34  1997/04/10  19:53:14  mjg
 * Removed HWScheduleScreenSwap and HWAdaptRegionSizeToScene.
 *
 * Revision 1.33  1997/04/08  13:54:51  jop
 * Added DEVICE_REGION_INFO_STRUCT
 *
 * Revision 1.32  1997/04/03  13:13:09  mjg
 * Updated the calling convention.
 *
 * Revision 1.31  1997/03/31  22:07:22  jop
 * Sorted out WIN32/DOS32 issues
 *
 * Revision 1.30  1997/03/26  06:47:57  erf
 * Removed stuff which provides PCX2 bilinear functionality for sglmid4.dll
 * running on PCX2.
 *
 * Revision 1.29  1997/03/25  09:57:38  erf
 * 1. Created variables to set bilinear filtering mode.
 * 2. Added prototype for HWSetBilinearRegister().
 *
 * Revision 1.28  1996/12/18  18:49:54  erf
 * For PCX2 PARAM_PRECISION_DIFF set to 10. Need to scale A, B & C
 * parameters to prevent overflow in hardware.
 * Forget Rev 1.26 1.
 *
 * Revision 1.27  1996/12/10  23:34:39  erf
 * Changed compile switches:
 * 1. PVR1 to ISPTSP.
 * 2. PVR2 to PCX1.
 * 3. PVR3 to PCX2.
 *
 * Revision 1.26  1996/11/15  17:12:11  erf
 * Use PVR3 compile switch for PCX2 to:
 * 1. Change PARAM_PRECISION_DIFF to 0. A, B & C parameters are all 32 bit.
 * 2. New #elif PVR3 for PCX2 instructions (cleaner).
 *
 * Revision 1.25  1996/11/06  16:58:33  erf
 * Add PCX2 define conditions. If PCX2 then PCX1 defined. Define PVR3 as 0 if not defined.
 * Define instructions for PVR3.
 *
 * Revision 1.24  1996/10/29  17:58:16  jrc
 * Added device X clipping for D3D tunnel numbers bug fix.
 *
 * Revision 1.23  1996/10/09  18:09:01  jop
 * Added RegionOffset parameter
 *
 * Revision 1.22  1996/10/02  16:50:01  jop
 * Fixed parameter in adapt func
 *
 * Revision 1.21  1996/09/07  22:58:20  jop
 * Bodged adaptive region sizing a bit more
 *
 * Revision 1.20  1996/09/04  11:51:30  jop
 * Added adaptregionsize stuff (diasbled)
 *
 * Revision 1.19  1996/08/09  14:15:57  gdc
 * pre-shifted Sabre intructions in INSTR_CODE_ENUMS for PVR2,
 * might save a few cycles when intruction is a function call param
 * accompanying changes in pkisp.c
 *
 * Revision 1.18  1996/03/11  17:16:21  sjf
 * Added Dump hardware registers desbugging routine.
 *
 * Revision 1.17  1995/12/18  17:47:10  sjf
 * Moved some values to hardware specific files, and now
 * knows about PVR1 and PVR2 architectures.
 *
 * Revision 1.16  1995/10/27  14:24:04  jop
 * Added proto for HWGetRegionSize
 *
 * Revision 1.15  1995/10/20  20:19:22  pdl
 * added some register setup calls.
 *
 * Revision 1.14  1995/10/19  14:14:12  sjf
 * Changed parameter memory block allocation interface, so that we can
 * single/double buffer sabre and texas independantly.
 *
 * Revision 1.13  1995/10/11  14:19:00  sjf
 * Added "screen swap" routine, and #defines for Sabre Object pointers.
 *
 * Revision 1.12  1995/07/26  18:04:56  sm
 * nothing!
 *
 * Revision 1.11  1995/07/26  17:51:44  sm
 * blah blah said pete
 * oh year, I also added Sabre and Texas index to Getmemref ..
 *
 * Revision 1.10  1995/07/26  14:53:37  sm
 * added HWGetRegionInfo proto
 *
 * Revision 1.9  1995/07/25  17:18:31  sjf
 * Made use of parameter mem init routine a bit clearer.
 *
 * Revision 1.8  1995/07/25  16:23:39  sjf
 * 1) Added parameter buffer ID enum.
 * 2) Fixed HwGetParame... and added release and init routine defs
 * 3) Added guards on file.
 *
 * Revision 1.7  1995/07/24  14:38:42  sjf
 * Added support for Texturing scaling (CFR Value)
 *
 * Revision 1.6  1995/07/23  16:04:10  sm
 * added precision diff define
 *
 * Revision 1.5  1995/07/21  12:27:28  sm
 * added proto for HWGetParam...
 *
 * Revision 1.4  1995/07/07  12:33:17  sm
 * added hwgetdevice prototype
 *
 * Revision 1.3  1995/06/29  15:52:47  sm
 * remove reg defs
 *
 * Revision 1.2  1995/06/29  15:11:53  sm
 * added register functions
 *
 * Revision 1.1  1995/06/23  17:17:43  sm
 * Initial revision
 *
 *
 *****************************************************************************/


#ifndef __hwinterf_h__
#define __hwinterf_h__

/******************************************************/
/*
// EXACTLY One of the following environment variables must be defined (and
// set to 1
//
// This tells rnrender (and possibly other files as well) what version of
// PowerVR we are using.
*/
#if ! defined(ISPTSP)
#define ISPTSP 0
#endif

#if ! defined(PCX1)
#define PCX1 0
#endif

#if ! defined(PCX2)
#define PCX2 0
#endif

#if ! defined(PCX2_003)
#define PCX2_003 0
#endif

#ifndef FORCE_NO_FPU
#include "pvrlims.h"
#endif

#if ISPTSP

#elif PCX1

#elif PCX2

#elif PCX2_003

#else
/*cause a syntax error */
XXXXXXXX ERROR XXXXXXXXX  PowerVR architecture not defined...
#endif

/******************************************************/



/*
// Number of bits "smaller" in magnitude A&B params are than C
// in sabre
*/
#define PARAM_PRECISION_DIFF	10

/*
// Maximum value CFR regsiter can be set to in Texas
*/
#define MAX_CFR_VALUE ((1L << 10) - 1)




/******************************
*	sabre instruction codes	  *	
******************************/

typedef enum {

/* Midas3 and PCX2 pack instructions in the same position.
 * ie no shift required, bits 0-3.
 */ 
#if ISPTSP
	/* forward */

	forw_visib,					/* 0x0  I though I'd put the codes in as a quick debugging reference */
	forw_invis,					/* 0x1 */
   	forw_perp,					/* 0x2 */

   	/* shadow */			

	test_shad_forw,				/* 0x3 */
	test_shad_perp,				/* 0x4 */
	
	/* reverse planes */

	rev_visib,					/* 0x5 */
	rev_invis,					/* 0x6 */	
	rev_replace_if,				/* 0x7 */

	/* forward first planes */

	forw_visib_fp,				/* 0x8 */ 
	forw_invis_fp,				/* 0x9 */
	forw_perp_fp,				/* 0xA */

	/* shadow first planes */

	test_shad_forw_fp,			/* 0xB */
	test_shad_perp_fp,			/* 0xC */

	/* reverse shadow planes */

	test_shadow_rev,			/* 0xD */
	test_light_rev,				/* 0xE */

	/* begin translucent pass */

	begin_trans 				/* 0xF */
#elif PCX1  || (PCX2 && FORCE_NO_FPU)
	/* pre-shift for packing routines, might save couple of cycles
	** where intructions are passed in - i.e. from PackOpaqueParams
	*/
	/* forward */

	forw_visib = 0x0 << 26,		/* 0x0  I though I'd put the codes in as a quick debugging reference */
	forw_invis = 0x1 << 26,		/* 0x1 */
   	forw_perp  = 0x2 << 26,		/* 0x2 */

   	/* shadow */			

	test_shad_forw = 0x3 << 26,	/* 0x3 */
	test_shad_perp = 0x4 << 26,	/* 0x4 */
	
	/* reverse planes */

	rev_visib = 0x5 << 26,		/* 0x5 */
	rev_invis = 0x6 << 26,		/* 0x6 */	
	rev_replace_if = 0x7 << 26,	/* 0x7 */

	/* forward first planes */

	forw_visib_fp = 0x8 << 26,	/* 0x8 */ 
	forw_invis_fp = 0x9 << 26,	/* 0x9 */
	forw_perp_fp = 0xA << 26,	/* 0xA */

	/* shadow first planes */

	test_shad_forw_fp = 0xB << 26,	/* 0xB */
	test_shad_perp_fp = 0xC << 26,	/* 0xC */

	/* reverse shadow planes */

	test_shadow_rev = 0xD << 26,	/* 0xD */
	test_light_rev = 0xE << 26,	/* 0xE */

	/* begin translucent pass */

	begin_trans = 0xF << 26 	/* 0xF */

#elif PCX2 || PCX2_003
	/* forward */

	forw_visib,					/* 0x0  I though I'd put the codes in as a quick debugging reference */
	forw_invis,					/* 0x1 */
   	forw_perp,					/* 0x2 */

   	/* shadow */			

	test_shad_forw,				/* 0x3 */
	test_shad_perp,				/* 0x4 */
	
	/* reverse planes */

	rev_visib,					/* 0x5 */
	rev_invis,					/* 0x6 */	
	rev_replace_if,				/* 0x7 */

	/* forward first planes */

	forw_visib_fp,				/* 0x8 */ 
	forw_invis_fp,				/* 0x9 */
	forw_perp_fp,				/* 0xA */

	/* shadow first planes */

	test_shad_forw_fp,			/* 0xB */
	test_shad_perp_fp,			/* 0xC */

	/* reverse shadow planes */

	test_shadow_rev,			/* 0xD */
	test_light_rev,				/* 0xE */

	/* begin translucent pass */

	begin_trans 				/* 0xF */
#endif
} INSTR_CODE_ENUMS;


/********************************
* Device and viewport routines  *
********************************/

typedef	struct	_DEVICE_REGION_INFO_STRUCT 
{

	/* number of regions in x */

	int	NumXRegions;

	/* number of regions in y */

	int NumYRegions;

	int LeftOverY;

	int HasLeftOver;

	/* size of x regions */
	int XSize;
	
	/* With Dynamic Regions, should be interpreted as the _minimum_
	 * X size. Or, as there can only be two tile widths, if X = 32
	 * then split up to 64 if possible; or if X = 64, don't split.
	 */

	/* size of y regions  - With Dynamic Regions, this is the _maximum_ Y */
	int YSize;

	/* Highest tile size */
	int	MinYSize;

	int MergeHeight; 
	/* Height at which tiles will be reduced in width (if possible)
	 * as well as horizontally;
	 */

} DEVICE_REGION_INFO_STRUCT;

extern int CALL_CONV HWCreateDevice(int *device_number,int *x_dimension,int *y_dimension,sgl_device_colour_types *device_mode, 
																											int *double_buffer);

extern int CALL_CONV HWSetViewport(int	parent_device, int	* left, int	* top, int	* right, int	* bottom);

extern int CALL_CONV HWDeleteDevice(int deviceID);

extern int CALL_CONV HWGetDeviceSize(int deviceID, int *x_dimension,int *y_dimension);

extern int CALL_CONV HWGetRegionInfo(int deviceID, DEVICE_REGION_INFO_STRUCT * pRegionInfo);

extern void CALL_CONV HWGetRegionSize (int *pnXSize, int *pnYSize);

extern void CALL_CONV HWSetXClip( sgl_bool bClipLeft,
								  int nLeft,
								  sgl_bool bClipRight,
						 		  int nRight );
/*
// return TRUE if we  have free buffers
*/

extern int CALL_CONV HWInitParamMem(); /*return 0 if ok*/

/**************************
*  ISP register routines  *
**************************/

extern	void CALL_CONV HWSetFogRegister(sgl_uint32	FogShiftValue);


#if PCX2
/* For global bilinear - PCX2_002 only */
extern sgl_bool	bFilteringOverRide;
extern sgl_bool	bBilinearEnabled;
#endif

#if PCX2 || PCX2_003
extern	void CALL_CONV HWSetBilinearRegister(sgl_texture_filter_type eFilterType);
#endif


#if ISPTSP
/*
// The Regions register only exists for MIDAS3, whiich is also the 
// only board with a brudge chip.
*/
extern void CALL_CONV HWSetRegionsRegister(sgl_uint32	NumRegions);
extern void	CALL_CONV HWSetupBunchOfBRIDGERegs();
#endif

extern void CALL_CONV HWSetSabPtrRegister(const sgl_uint32 RegionBase, const sgl_uint32 RegionOffset);

extern  sgl_bool CALL_CONV HWFinishedRender(void);

extern	void CALL_CONV HWReset(void);

extern	void CALL_CONV HWStartRenderStrip();

/*****************************
* Routine to used to communicate with the 
* the Interrupt service routine to cause swapping of frame buffers.
* It is assumed that the ISR (if any) will detect when rendering is complete
* and perform the pending request to swap buffers
**************************/

extern void	CALL_CONV HWSetupBunchOfISPRegs();

extern void CALL_CONV HWSetupBunchOfTSPRegs();

#if DEBUG && (WIN32 || DOS32)
extern void CALL_CONV HWDumpRegisters(FILE * outfile);
#endif

#endif /* __hwinterf_h__ */

/*---------------------------- End of File -------------------------------*/

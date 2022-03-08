/**************************************************************************
 * Name         : pkisp.c
 * Title        : Plane data packing for ISP chip
 * Author       : Stel Michael
 * Created      : 19th July 1995
 *
 * Copyright : 1995-2022 Imagination Technologies (c)
 * License	 : MIT
 *
 * Description  : 
 *
 * Platform     : ANSI
 *								 
 * Modifications:-
 * $Log: pkisp.c,v $
 * Revision 1.110  1997/10/16  13:21:19  sxy
 * removed DPFDEVFs, its all in DPFDEV.
 *
 * Revision 1.109  1997/10/15  13:08:15  sxy
 * added DPFDEV messages.
 *
 * Revision 1.108  1997/09/29  09:47:06  sxy
 * added some DPFDEV messages.
 *
 * Revision 1.107  1997/08/02  16:39:19  mjg
 * Removed the BUG_IN_C_PARAMETER test switch.
 *
 * Revision 1.106  1997/07/14  19:29:32  mjg
 * Added the PCX2_003 compile switch and MODULE_ID - note that file
 * currently has BUG_IN_C_PARAMETER define for testing on a PCX2_002.
 *
 * Revision 1.105  1997/06/17  09:33:39  mjg
 * Added the FORCE_NO_FPU option.
 *
 * Revision 1.104  1997/06/05  09:06:50  mjg
 * Removed various compiler warnings.
 *
 * Revision 1.103  1997/05/19  14:17:22  gdc
 * and for shadow l-v objects
 *
 * Revision 1.102  1997/05/19  14:10:34  gdc
 * added some limit checking for opaque convexes to see if we can
 * stop them going over page boundaries
 *
 * Revision 1.101  1997/05/19  08:59:45  gdc
 * made GetSabreLimit be included from parambuff.h - so we only have one version
 * to maintain
 *
 * Revision 1.100  1997/05/09  11:24:36  jrc
 * Changed some static functions to extern.
 *
 * Revision 1.99  1997/05/09  09:17:17  erf
 * Moved AddDummyPlanesL() to this file. All functions used by this function
 * are in this file so it makes sense to put it here. Also it prevents having
 * an additional copy of this routine in the PVRDirect library.
 * Made some functions static.
 *
 * Revision 1.98  1997/05/08  11:20:13  mjg
 * Fixed a syntax error.
 *
 * Revision 1.97  1997/05/08  10:14:21  erf
 * Added function call to get the fixed clipping distance directly
 * from the projection matrix. This makes life easier for the PVRDirect
 * driver.
 *
 * Revision 1.96  1997/05/02  07:42:44  erf
 * Removed UINT32's etc and replaced with sgl_uint32's etc.
 *
 * Revision 1.95  1997/04/22  21:40:54  mjg
 * Little twiddle as this version of GetSabreLimit is different from the
 * ones in disp.c and dpoint.c .
 *
 * Revision 1.94  1997/04/22  18:02:02  mjg
 * Removed brdsetup.h and altred the SabreMem functions.
 *
 * Revision 1.93  1997/04/21  20:00:39  mjg
 * Implemented new parameter buffer structures.
 * n
 * ,
 *
 * Revision 1.92  1997/04/21  15:27:05  mjg
 * Replaced ParamBufferInfo with PVRParamBuffs.
 *
 * Revision 1.91  1997/04/09  16:35:03  gdc
 * removed reference to global projection_mat variable
 *
 * Revision 1.90  1997/03/31  22:08:28  jop
 * Sorted out WIN32/DOS32 issues
 * y
 *
 * Revision 1.89  1997/03/14  18:09:58  jop
 *  Added pvrosapi.h include!
 *
 * Revision 1.88  1997/03/14  15:56:32  jwf
 * Added HWIO macros for macintosh compatability
 *
 * Revision 1.87  1997/03/14  12:18:20  jwf
 * changed so it builds on the mac
 *
 * Revision 1.86  1997/03/05  18:07:06  erf
 * 1. Change NUM_DUMMY_OBJECT_PLANES to NUM_DUMMY_OBJECT_PLANES_LARGE in
 *    PackOpaqueDummyLarge().
 * 2. Added checking of C parameter in PackNegatedEdge() and PackEdge().
 *
 * Revision 1.86  1997/03/05  18:07:06  erf
 * 1. Change NUM_DUMMY_OBJECT_PLANES to NUM_DUMMY_OBJECT_PLANES_LARGE in
 *    PackOpaqueDummyLarge().
 * 2. Added checking of C parameter in PackNegatedEdge() and PackEdge().
 *
 * Revision 1.85  1997/02/24  17:43:25  erf
 * Do PCX2 hardware check of C parameter for PackPlane() and PackOnFlatPlane().
 *
 * Revision 1.84  1997/02/19  14:37:51  erf
 * Create PackOpaqueDummyLarge() routine to fix vignetting. Large dummy
 * objects used to pad to 1024 cache boundary if needed.
 *
 * Revision 1.83  1997/02/12  12:30:04  erf
 * New routine PackTranslucentBeginDummy() which creates proper translucent
 * dummy plane to start a translucent pass.
 * Redefined PackTranslucentDummy() routine which now only defines a dummy
 * object for the translucent pass.
 *
 * Revision 1.82  1997/01/29  18:15:00  mjg
 * Checks to see if we're running out of ISP space, occurs with very
 * complicated meshes. This version of GetPVRParamBuffs[PVR_PARAM_TYPE_ISP].uBufferLimit is currently different
 * from the one in disp.c
 *
 * Revision 1.81  1997/01/16  21:32:28  erf
 * Change FLOAT_TO_LONG to LONG_TO_FLOAT for PCX2 ISP parameter packing.
 *
 * Revision 1.80  1996/12/18  17:22:22  erf
 * Used new GetPVRParamBuffs[PVR_PARAM_TYPE_ISP].uBufferLimit() & GetStartOfObject() from disp.c.
 *
 * Revision 1.79  1996/12/10  23:19:30  erf
 * Changed compile switches:
 * 1. PCX1 to PCX1 || PCX2.
 * 2. PVR1 to ISPTSP.
 * 3. PVR2 to PCX1.
 * 4. PVR3 to PCX2.
 * 5. #ifdef WIN32 to #if WIN32.
 *
 * Revision 1.78  1996/12/09  13:03:13  gdc
 * removed redundant sgltri stuff - making assumption that D3DOPTIM stuff
 * now the only option.
 *
 * Revision 1.77  1996/11/28  15:54:38  sjf
 * Moved pack to 20 bit into its own file as someone else had stolen a copy.
 * Hopefully this makes it easier to maintain.
 *
 * Revision 1.76  1996/11/19  11:29:34  rgi
 * Returned this to it's pre-pkispml module state. Optimised
 * stuff now uses disp.c module and dispml.asm. This is close
 * to it's 'original' state .
 *
 * Revision 1.75  1996/11/18  18:30:40  jop
 * Removed PowerSGL direct stuff
 *
 * Revision 1.74  1996/11/18  17:06:54  rgi
 * Temporarily suspended the use of the assember version of
 * the pack functions.
 * Fixed the old flashing tiles bug in the PackRegionOpaque
 * call et al.
 *
 * Revision 1.73  1996/11/15  09:22:36  rgi
 * Fixed carriage returns added recently.
 * Also added D3DOPTIM flag and PackISPCore function.
 * PackTo20Bit and PackISPCore have assembler equivalents if
 * D3DOPTIM is set and we are running on PVR2
 *
 * Revision 1.72  1996/11/14  10:51:50  erf
 * Use PVR3 compile switch to change the following for PCX2.
 * 1. Don't compile out PackTo20Bit() function. (needed for simulation).
 * 2. Use FLOAT_TO_LONG to copy plane parameters.
 * 3. Remove scaling for perpendicular planes in PackISPTri().
 *
 * Revision 1.71  1996/11/07  11:40:21  erf
 * Added PVR3 compile switch for PCX2 to implement new floating
 * point support. Changes made:
 * 1. 4 words for plane parameter information.
 * 2. Remove calls to PackTo20Bit().
 * 3. Compile out PackTo20Bit() code in case called.
 * 4. Change n32A,B & C to f32A,B & C.
 * 5. Change n32FixedClipDist to f32FixedClipDist.
 * 6. Change parameters in PackFlatOnPlane() and PackBackgroundPlane()
 *    from INT32 to float.
 *
 * Revision 1.70  1996/11/06  06:19:40  jrc
 * Fixed a couple of typos - the previous revision didn't compile.
 *
 * Revision 1.69  1996/11/06  05:10:33  jrc
 * Added n-polygon packing.  Hmmm.
 *
 * Revision 1.68  1996/10/30  14:07:07  jop
 * Uncommented trans/smooth/shad fluch plane
 *
 * Revision 1.67  1996/10/23  16:12:10  jop
 * Added debug stuff (ifdefed out) for sm/shad/trans bug
 *
 * Revision 1.66  1996/10/16  21:18:31  jop
 * Added new ISP pack routines for lite+
 *
 * Revision 1.65  1996/10/11  12:04:06  mal
 * Modified some metric macros.
 *
 * Revision 1.64  1996/10/09  15:37:12  mal
 * Created some new macros for timing isp packing routines.
 *
 * Revision 1.63  1996/10/04  17:11:27  mal
 * Added some metric macros for timing code.
 *
 * Revision 1.62  1996/09/04  10:35:57  sjf
 * Made a constant name read more like English!
 *
 * Revision 1.61  1996/08/09  14:22:22  gdc
 * removed leftshift 26's on Sabre intruct packing, accompanying change
 * to hwinterf.h, when enum has intructions pre-shifted
 *
 * Revision 1.60  1996/07/19  11:34:53  sjf
 * 1) Noticed a small optimisation in the Negation of PVR1 20
 *    bit floats
 * 2) Put in some temporary variables in the pack plane routine. This
 *    should speed things up (on pentium, by getting better usage of
 *    the 2 pipelines). This may, of course, do nothing at all. Time
 *    will tell.
 *
 * Revision 1.59  1996/07/15  17:07:29  jop
 * Added quad packing routines
 *
 * Revision 1.58  1996/06/10  19:28:53  jop
 * Fixed defines for dummy objects
 *
 * Revision 1.57  1996/06/10  14:11:25  jop
 * Added brdsetup.h
 *
 * Revision 1.56  1996/05/16  12:19:51  pdl
 * added a defined (WIN32)
 *
 * Revision 1.55  1996/05/15  10:43:45  sjf
 * Fixed bug with shadows and perpendicular planes - incorrect instruction
 * was being used. This was introduced when the PVR2 and PVR1 versions were
 * merged back together again.
 *
 * Revision 1.54  1996/05/15  10:33:44  jop
 * Fixed limit bug intriangles code
 *
 * Revision 1.53  1996/05/10  14:43:48  jop
 * Added triangle packing support
 *
 * Revision 1.52  1996/05/07  14:15:35  jop
 * Fixed sgltri bug
 *
 * Revision 1.51  1996/05/03  14:48:04  jop
 * Returns a more sensible value on error (PackTriangle)
 *
 * Revision 1.50  1996/05/03  14:42:48  jop
 * Added triangle api pack call
 *
 * Revision 1.49  1996/04/30  18:05:38  jop
 * merged pvr1/pvr2 build versions
 *
 ************************************************************************
 ******* CODE MOVED BACK TO MAIN DIRECTORY AND MERGED WITH PVR2 STUFF ****
 ************************************************************************
 *
 * Revision 1.9  1996/04/25  13:19:21  jop
 * Wrote double barelled 20bit routine
 *
 * Revision 1.8  1996/04/23  16:42:14  sjf
 * 1) Deleted the NON-IEEE version of Pack to 20 bit (who cares)
 * 2) Added #if to not include the code which does rounding in
 *   one case in pack. This doesn't appear to have affected accuracy
 *   significantly (ie. I couldnt see any probs)
 *
 * Revision 1.7  1996/04/20  13:18:50  sjf
 * Deleted unused edge flags field from edge structure.
 *
 * Revision 1.6  1996/04/02  13:06:26  sjf
 * changed a const to not const :)
 *
 * Revision 1.5  1996/04/02  12:40:00  sjf
 * Since the number of edges on a mesh face can now only number 3 or 4, optimised
 * the packing routines accordingly.
 *
 * Revision 1.4  1996/03/25  16:43:33  jop
 * Fixed PackMeshParams
 *
 * Revision 1.3  1996/02/20  16:07:19  jrc
 * Now looks for SGL header files in the correct place (parent directory).
 *
 * Revision 1.2  1995/12/20  17:29:55  jop
 * Added pvr1lims.h include
 *
 * Revision 1.1  1995/12/18  18:59:37  sjf
 * Initial revision
 *
 ************************************************************************
 ******* PKISP was put into 2 directories for PVR1 and PVR2 versions ****
 ************************************************************************
 *
 * Revision 1.48  1995/11/09  18:20:23  jop
 * Moved over to transformed structures
 *
 * Revision 1.47  1995/11/05  15:23:05  sjf
 * Simplified an "overflow" test in pack mesh.
 *
 * Revision 1.46  1995/11/04  12:15:51  sjf
 * Unwound loop in pack mesh ordered... Now uses a switch statement.
 * This appeared to give a 1-2% overall improvment on a mesh demo.
 *
 * Revision 1.45  1995/10/19  14:18:33  sjf
 * Changed name of variable for parameter space pointer.
 *
 * Revision 1.44  1995/10/12  12:35:17  jrc
 * Fixed unterminated list of perpendicular shadow planes.
 *
 * Revision 1.43  1995/10/04  11:00:20  sjf
 * Fixed bug in pack opaque dummy - it wasnt using a first plane instruction.
 * (Tut tut tut)
 *
 * Revision 1.42  1995/10/04  10:37:29  jop
 * Added PackMeshParamsOrdered
 *
 * Revision 1.41  1995/09/19  14:22:34  sjf
 * Changed most for loops to count down instead of up (wherever the index wasn't
 * actually referenced) as this often generates faster loop testing.
 *
 * Revision 1.40  1995/09/19  09:01:18  sjf
 * 1) Minor tidy-up of pack opaque
 * 2) replaced pack shadow and pack light volumes with one
 *    unified routine (based on ther old ones and the optimised pack
 *    pack opaque). The difference between packing light vols and
 *    shadow vols was so minor that it didn't make sence to maintain
 *    two routines.
 *
 * Revision 1.39  1995/09/11  15:47:40  sm
 * removed PacktranslucentParams function since it is no longer needed.
 *
 * Revision 1.38  1995/09/08  15:54:58  sm
 * added dummy object pack routines
 *
 * Revision 1.37  1995/09/06  09:34:36  sjf
 * Optimised pack opaque ... the same could/should be done for the
 * other pack routines.
 *
 * Revision 1.36  1995/09/04  15:33:15  sm
 * added pack translucent dummy routine
 *
 * Revision 1.35  1995/09/04  15:31:09  sjf
 * Did further optimisations on pack to 20 bit.
 *
 * Revision 1.34  1995/08/27  15:22:35  sjf
 * Fixed pack shadow planes, and deleted unneeded clip parameter... This
 * still needs optimising!
 *
 * Revision 1.33  1995/08/24  12:23:55  pdl
 * added a const.
 *
 * Revision 1.32  1995/08/23  14:17:33  sm
 * fixed minor trans instr bug!
 *
 * Revision 1.31  1995/08/21  15:50:55  sm
 * modified pack trans plaesn.
 *
 * Revision 1.30  1995/08/21  11:51:08  jop
 * Added some (unnecessary) ASSERTs
 *
 * Revision 1.29  1995/08/14  12:54:17  sjf
 * Fixed range checking in pack routine.
 *
 * Revision 1.28  1995/08/14  12:20:08  sjf
 * removed uneeded debug message.
 *
 * Revision 1.27  1995/08/14  11:36:00  sjf
 * Added faster IEEE float to 20 bit packing routine
 * 2) Changed the plane structure so that it contained the packed values instead of
 * 	floats.
 *
 * Revision 1.26  1995/08/08  12:38:13  sjf
 * OOOPS : Fixed a bug I sort of introduced, to so with packing very small
 * negative numbers in the a and b parameters.
 *
 * Revision 1.25  1995/08/08  12:17:26  jop
 * Made PackMeshParams plane list based
 *
 * Revision 1.24  1995/08/02  15:16:41  sm
 * minor speedups
 *
 * Revision 1.23  1995/08/01  09:52:09  sm
 * minor optim if pack routine
 *
 * Revision 1.22  1995/07/31  13:46:33  sjf
 * Fixed problem in pack opaque when there were no forward planes, and
 * a clipping plane was requested. Also tidied up the Tags that were put
 * in since some are not defined externally, so would be random. Put in
 * fixed constants instead, which would be quicker anyway.
 *
 * Revision 1.21  1995/07/31  12:23:42  jop
 * Put edge direction reversal in order (PackMeshParams)
 *
 * Revision 1.20  1995/07/30  21:59:47  sjf
 * "Reverted" to version 1.18 because 1.19 did soem very strange things.
 *
 * Revision 1.18  1995/07/28  11:18:18  sm
 * fixed cut and paste bug !
 *
 * Revision 1.17  1995/07/28  10:08:53  sm
 * fixed bug with packing reverse_replace_if
 *
 * Revision 1.16  1995/07/27  21:32:05  sm
 * *** empty log message ***
 *
 * Revision 1.15  1995/07/27  21:11:13  sm
 * fixed bug wrt numPlanes
 *
 * Revision 1.14  1995/07/27  18:41:10  sm
 * *** empty log message ***
 *
 * Revision 1.13  1995/07/27  18:35:29  sm
 * changed first entry of rev visible to rev invis
 *
 * Revision 1.12  1995/07/27  18:33:36  jop
 * Better nNumPlanes calculation in packmeshparams
 * ,.
 *
 * Revision 1.11  1995/07/27  14:39:52  sm
 * added inc for clip and perp planes
 *
 * Revision 1.10  1995/07/27  11:53:21  sjf
 * Added a Pack background plane.
 *
 * Revision 1.9  1995/07/26  13:01:08  sm
 * removed syscon.h
 *
 * Revision 1.8  1995/07/26  12:28:12  sm
 * changed to return num planes
 *
 * Revision 1.7  1995/07/26  08:57:44  sjf
 * Removed uneeded #include.
 *
 * Revision 1.6  1995/07/25  17:51:25  sm
 * fixed mistake with mem limit
 *
 * Revision 1.5  1995/07/25  17:01:12  sjf
 * Changed "see inside" to "repeat reverse", and "changed" names of the global
 * param buffer management variables (by doing MACROS which is really nasty).
 *
 * Revision 1.4  1995/07/25  10:15:31  sm
 * changed forw_invis to forw_perp in PackMeshParams
 *
 * Revision 1.3  1995/07/25  10:07:53  jop
 * Added mesh parameter packer
 *
 * Revision 1.2  1995/07/24  15:13:42  sm
 * split up into four pack param routines
 *
 * Revision 1.1  1995/07/24  10:27:55  sm
 * Initial revision
 *
 *
 *************************************************************************/

#define MODULE_ID MODID_PKISP

#include "sgl_defs.h"
#include "pvrlims.h"
#include "sgl_math.h"
#include "nm_intf.h"
#include "dlnodes.h" 
#include "dvdevice.h"
#include "dregion.h"
#include "rnglobal.h"
#include "rnstate.h"
#include "rnconvst.h"
#include "hwinterf.h"
#include "pkisp.h"
#include "pktsp.h"	/* for PackTexasTransparent */
#include "rnmesh.h"

#include "debug.h"
#include "metrics.h"

#include "pmsabre.h"

#include "pvrosapi.h"
#include "parmbuff.h"

SGL_EXTERN_TIME_REF /* if we are timing code */

/*
// Define some dummy Tags for the planes which don't use them, but
// Would be useful for debugging. Note in the non debug, use small
// 16 bit constants as these should be loaded faster.
*/
#if DEBUG

#define DEBUG_CLIP_PLANE_TAG 0xAAAAAAUL
#define DEBUG_INVIS_FP_TAG	 0xBBBBBBUL
#define DEBUG_INVIS_RP_TAG	 0xCCCCCCUL
#define DEBUG_PERP_TAG		 0xDDDDDDUL
#define DEBUG_DUMMY_F_TAG	 0xEEEEEEUL
#define DEBUG_DUMMY_R_TAG	 0xFFFFFFUL

#else

#define DEBUG_CLIP_PLANE_TAG 0x000000UL
#define DEBUG_INVIS_FP_TAG	 0x000000UL
#define DEBUG_INVIS_RP_TAG	 0x000000UL
#define DEBUG_PERP_TAG		 0x000000UL
#define DEBUG_DUMMY_F_TAG	 0x000000UL
#define DEBUG_DUMMY_R_TAG	 0x000000UL

#endif

#define UPPER_6_OF_TAG		0x3F000UL

/*
// INCLUDE Pack to 20 Bit routines, such that it isn't inlined
*/
#define INLINE_P20
#include "pto20b.h"
#undef  INLINE_P20

/**************************************************************************
 * Function Name  : PackPlane   (LOCAL ONLY)
 * Inputs         : Plane pointer, result pointer, instruction, tag
 * Outputs        : None
 * Input/Output	  : 
 * Returns        :
 * Global Used    : 
 * Description    : Packs the plane into the required format for Sabre/ISP
 *					It sort of assumes that this will be inlined (for efficiency)
 *	
 *					Packs to the 3 word per plane format for PCX1
 **************************************************************************/

static INLINE void PackPlane(sgl_uint32 * pPlaneMem,
			  const TRANSFORMED_PLANE_STRUCT  * pPlane,
			  const sgl_uint32 Instr,
			  const	sgl_uint32 Tag)
{
	/*
	// Temporary variables in a (possibly vain) hope to get better
	// overlap of memory operations
	*/
#if PCX2 || PCX2_003
	float	tmp0, tmp1;
#else
	sgl_uint32	tmp0, tmp1;
#endif

	SGL_TIME_START(PACK_PLANE_TIME)

	#if ISPTSP
		/*
		// Use temp variables here so the compiler "knows" that
		// the assignments can be done in any order. (The compiler thinks they
		//  might overlap in memory otherwise.)
		*/
		tmp0 = pPlane->n32A; 
		tmp1 = pPlane->n32B;
		pPlaneMem[0] = tmp0;
		pPlaneMem[1] = tmp1;
		pPlaneMem[2] = pPlane->n32C;
		pPlaneMem[3] = (Instr | (Tag << 4));

	#elif PCX1  || ((PCX2 || PCX2_003) && FORCE_NO_FPU)

		/*
		// Word Zero has Instruction, Tag Upper Bits, and A
		*/
		tmp0 = Instr | 
					   ((UPPER_6_OF_TAG & Tag) << (20 - 12)) | 
					   pPlane->n32A;

		/*
		// Word 1 has Lower Bits of Tag, and B
		// (note we assume that the upper 6 bits of the tag are shifted
		// off the top)
		*/
		tmp1 = (Tag << 20) | pPlane->n32B;

		pPlaneMem[0] = tmp0;
		pPlaneMem[1] = tmp1;

		/*
		// While word 2 is just C
		*/
		pPlaneMem[2] = pPlane->n32C;

	#elif PCX2 || PCX2_003
		/*
		// Use temp variables here so the compiler "knows" that
		// the assignments can be done in any order. (The compiler thinks they
		//  might overlap in memory otherwise.)
		*/
		tmp0 = pPlane->f32A; 
		tmp1 = pPlane->f32B;
		FW( pPlaneMem, 0, tmp0);
		FW( pPlaneMem, 1, tmp1);

		/* Check C value. PCX2 hardware has problems converting a number in
		 * the range -9.313e-10 to 0. If C is in this range set to 0.
		 */
	#if PCX2		
		if (sfabs(pPlane->f32C) <= MAGIC_FLOAT_FIX)
		{	/* If problem set C to 0. */
			IW( pPlaneMem, 2, 0);
		}
		else /* continue as normal */
	#endif /* PCX2 */
		{
			FW( pPlaneMem, 2, (pPlane->f32C));
		}
		IW( pPlaneMem, 3, (Instr | (Tag << 4)));

	#else
	#pragma message ("ISPTSP, PCX1, PCX2 or PCX2_003 must be defined");
	#endif
	SGL_TIME_STOP(PACK_PLANE_TIME)
}


/**************************************************************************
 * Function Name  : PackFlatOnPlane   (LOCAL ONLY)
 * Inputs         : result pointer, distance, instruction, distance, tag,
 * Outputs        : None
 * Input/Output	  :
 * Returns        :
 * Global Used    :
 * Description    : Packs the flat on plane into the required format for Sabre/ISP
 *					It sort of assumes that this will be inlined (for efficiency)
 *	
 *					Packs to the 3 word per plane format for PCX1
 **************************************************************************/
#if (PCX2 || PCX2_003) && !FORCE_NO_FPU
static INLINE void PackFlatOnPlane(sgl_uint32 * pPlaneMem,
			  float distance,
			  const sgl_uint32 Instr,
			  const	sgl_uint32 Tag)
#else
static INLINE void PackFlatOnPlane(sgl_uint32 * pPlaneMem,
			  sgl_int32 distance,
			  const sgl_uint32 Instr,
			  const	sgl_uint32 Tag)
#endif

{
	#if ISPTSP

   		pPlaneMem[0] = 0;
   		pPlaneMem[1] = 0;
  	   	pPlaneMem[2] = distance;
   	  	pPlaneMem[3] = (Instr | (Tag << 4));

	#elif PCX1  || ((PCX2 || PCX2_003) && FORCE_NO_FPU)

		/*
		// Word Zero has Instruction, Tag Upper Bits, and A = 0
		*/
		pPlaneMem[0] = (Instr) |
					   ((UPPER_6_OF_TAG & Tag) << (20 - 12));

		/*
		// Word 1 has Lower Bits of Tag, and B = 0
		// (note we assume that the upper 6 bits of the tag are shifted
		// off the top)
		*/
		pPlaneMem[1] = (Tag << 20);

		/*
		// While word 2 is just distance
		*/
		pPlaneMem[2] = distance;

	#elif PCX2 || PCX2_003

   		IW( pPlaneMem, 0, 0); 
   		IW( pPlaneMem, 1, 0);

		/* Check C value. PCX2 hardware has problems converting a number in
		 * the range -9.313e-10 to 0. If C is in this range set to 0.
		 */
	#if PCX2
		if (sfabs(distance) <= MAGIC_FLOAT_FIX)
		{	/* If problem set C to 0. */
			IW( pPlaneMem, 2, 0);
		}
		else /* continue as normal */
	#endif /* PCX2 */
		{
			FW( pPlaneMem, 2, distance);
		}

   	  	IW( pPlaneMem, 3, (Instr | (Tag << 4)));

	#else
	#pragma message ("ISPTSP, PCX1, PCX2 or PCX2_003 must be defined");
	#endif
}


/**************************************************************************
 * Function Name  : PackEdge   (LOCAL ONLY)
 * Inputs         : Edge pointer, result pointer, instruction, tag
 * Outputs        : None
 * Input/Output	  :
 * Returns        :
 * Global Used    :
 * Description    : Similar to pack plane, but takes the "edge" structure
 *					format of the meshes
 **************************************************************************/
static INLINE void PackEdge(sgl_uint32 * pPlaneMem,  const PTRANSEDGE pEdge)
{
	#if ISPTSP

		pPlaneMem[0] = pEdge->n32A;
		pPlaneMem[1] = pEdge->n32B;
		pPlaneMem[2] = pEdge->n32C;
		pPlaneMem[3] = forw_perp | (DEBUG_PERP_TAG << 4);

	#elif PCX1  || ((PCX2 || PCX2_003) && FORCE_NO_FPU)

		/*
		// Word Zero has Instruction, Tag Upper Bits, and A
		*/
		pPlaneMem[0] = (forw_perp) |
					   ((UPPER_6_OF_TAG & DEBUG_PERP_TAG) << (20 - 12)) |
					   pEdge->n32A;

		/*
		// Word 1 has Lower Bits of Tag, and B
		// (note we assume that the upper 6 bits of the tag are shifted
		// off the top)
		*/
		pPlaneMem[1] = (DEBUG_PERP_TAG << 20) | pEdge->n32B;

		/*
		// While word 2 is just C
		*/
		pPlaneMem[2] = pEdge->n32C;

	#elif PCX2 || PCX2_003

		FW( pPlaneMem, 0, (pEdge->f32A));
		FW( pPlaneMem, 1, (pEdge->f32B));

		/* Check C value. PCX2 hardware has problems converting a number in
		 * the range -9.313e-10 to 0. If C is in this range set to 0.
		 */
	#if PCX2
		if (sfabs(pEdge->f32C) <= MAGIC_FLOAT_FIX)
		{	/* If problem set C to 0. */
			IW( pPlaneMem, 2, 0);
		}
		else /* continue as normal */
	#endif /* PCX2 */
		{
			FW( pPlaneMem, 2, (pEdge->f32C));
		}

		IW( pPlaneMem, 3, (forw_perp | (DEBUG_PERP_TAG << 4)));

	#else
	#pragma message ("ISPTSP, PCX1, PCX2 or PCX2_003 must be defined");
	#endif
}


/**************************************************************************
 * Function Name  : PackNegdatedEdge   (LOCAL ONLY)
 * Inputs         : Edge pointer, result pointer, instruction, tag
 * Outputs        : None
 * Input/Output	  :
 * Returns        :
 * Global Used    :
 * Description    : Similar to pack edge, but swaps the orientation...
 **************************************************************************/
static INLINE void PackNegatedEdge(sgl_uint32 * pPlaneMem,  const PTRANSEDGE pEdge)
{
	#if ISPTSP

		pPlaneMem[0] = NegatePacked20Bit(pEdge->n32A);
		pPlaneMem[1] = NegatePacked20Bit(pEdge->n32B);
		pPlaneMem[2] = -pEdge->n32C;
		pPlaneMem[3] = forw_perp | (DEBUG_PERP_TAG << 4);

	#elif PCX1  || ((PCX2 || PCX2_003) && FORCE_NO_FPU)

		/*
		// Word Zero has Instruction, Tag Upper Bits, and A
		*/
		pPlaneMem[0] = (forw_perp) |
					   ((UPPER_6_OF_TAG & DEBUG_PERP_TAG) << (20 - 12)) |
					   NegatePacked20Bit(pEdge->n32A);

		/*
		// Word 1 has Lower Bits of Tag, and B
		// (note we assume that the upper 6 bits of the tag are shifted
		// off the top)
		*/
		pPlaneMem[1] = (DEBUG_PERP_TAG << 20) | NegatePacked20Bit(pEdge->n32B);

		/*
		// While word 2 is just C
		*/
		pPlaneMem[2] = -pEdge->n32C;
		
	#elif PCX2 || PCX2_003
		/* Negate parameters.
		 */
		{
		float	tmp0, tmp1;
		
		
		tmp0 = -pEdge->f32A;
		tmp1 = -pEdge->f32B;
		FW( pPlaneMem, 0, tmp0);
		FW( pPlaneMem, 1, tmp1);
		}
		
		/* Check C value. PCX2 hardware has problems converting a number in
		 * the range -9.313e-10 to 0. If C is in this range set to 0.
		 */
		#if PCX2
			if (sfabs(pEdge->f32C) <= MAGIC_FLOAT_FIX)
			{	/* If problem set C to 0. */
				IW( pPlaneMem, 2, 0);
			}
			else /* continue as normal */
		#endif /* PCX2 */
			{
				float	tmp0;
				
				
				tmp0 = -pEdge->f32C;
				FW( pPlaneMem, 2, tmp0);
			}

		IW( pPlaneMem, 3, (forw_perp | (DEBUG_PERP_TAG << 4)));

	#else
	#pragma message ("ISPTSP, PCX1, PCX2 or PCX2_003 must be defined");
	#endif
}

/**************************************************************************
 * Function Name  : PackOpaqueParams
 * Inputs         :
 * Outputs        :
 * Input/Output	  :
 * Returns        :
 * Global Used    :
 *
 * Description    : Takes in a group of planes for a single conviex primitive
 *					and packs data for the planes into memory data format for
 *					the ISP.
 *				
 **************************************************************************/

int	PackOpaqueParams( const PLANE_CATEGORIES_STRUCT * pPlaneLists,
					  const sgl_bool AddClipPlane,
					  const sgl_bool RepeatReverse)
{
	sgl_bool	AddDummyForward = FALSE;

	int	numForward, numReverse, numPlanes;
	int i;
	sgl_uint32 * pPlaneMem;
#if (PCX2 || PCX2_003) && !FORCE_NO_FPU
	float f32FixedClipDist = RnGlobalGetFixedClipDist();
#else
	sgl_uint32 n32FixedClipDist	= RnGlobalGetFixedClipDist();
#endif

	/*
	// Keep a pointer into the various plane pointer lists,
	// as well as a pointer to the plane data itself
	*/
	TRANSFORMED_PLANE_STRUCT  * const * ppPlane;
	const TRANSFORMED_PLANE_STRUCT  *  pPlane;
	sgl_uint32 CurrentLimit;

	SGL_TIME_START(PACK_OPAQUE_TIME)

	numForward = pPlaneLists->NumFV + pPlaneLists->NumFI;
	numReverse = pPlaneLists->NumRV + pPlaneLists->NumRI;

	/*
	// To make life simple, we always have a forward plane. This
	// is set to be the foreground clipping plane, and is invisible.
	// Note that we can then ignore the "add clip plane" as this is
	// already covered by the dummy forward.
	*/
	if (numForward == 0)
	{
		AddDummyForward = TRUE;	
		numForward = 1;	
	}
	/*
	// Do we need a clipping plane?
	*/
	else if (AddClipPlane)
	{
		numForward++;
	}


	/* need to check to see if we have enough space in the parameter memory */

	if (RepeatReverse)
		numReverse<<=1;

	numPlanes = numForward + numReverse + pPlaneLists->NumPE;


#define CHECK_LIMITS 1
#if CHECK_LIMITS
	CurrentLimit = GetSabreLimit (PVRParamBuffs[PVR_PARAM_TYPE_ISP].uBufferPos);
	
	if (CurrentLimit == 0)
	{
 		/* this isnt enough space */
		DPFDEV ((DBG_WARNING, "PackOpaqueParams: Out of sabre memory pages !"));
		
		SGL_TIME_STOP(PACK_OPAQUE_TIME)
		return(0);
	}

	if ((PVRParamBuffs[PVR_PARAM_TYPE_ISP].uBufferPos + 
		 (numPlanes * WORDS_PER_PLANE )) > CurrentLimit)
	{
		CurrentLimit = GetSabreLimit (CurrentLimit++);
		
		if (CurrentLimit == 0)
		{
			/* this isnt enough space */
			DPFDEV ((DBG_WARNING, "PackOpaqueParams: Out of sabre memory pages !"));
	
			SGL_TIME_STOP(PACK_OPAQUE_TIME)
			return 0;
		}

		PVRParamBuffs[PVR_PARAM_TYPE_ISP].uBufferPos = 
			GetStartOfObject (PVRParamBuffs[PVR_PARAM_TYPE_ISP].uBufferPos, 
							  (numPlanes * WORDS_PER_PLANE ));
		
		if ((PVRParamBuffs[PVR_PARAM_TYPE_ISP].uBufferPos + 
			 (numPlanes * WORDS_PER_PLANE )) > CurrentLimit)
		{
			/* this isnt enough space */
			DPFDEV ((DBG_WARNING, "PackOpaqueParams: Out of sabre memory! %d planes in object ", numPlanes));
				
			SGL_TIME_STOP(PACK_OPAQUE_TIME)
			return 0;
		}

	}
#endif

	/* Get where we Add the object to the plane parameter memory */
   	pPlaneMem = &PVRParamBuffs[PVR_PARAM_TYPE_ISP].pBuffer[PVRParamBuffs[PVR_PARAM_TYPE_ISP].uBufferPos];

	/*
	// Up the index by the number of planes weve added, "times" the
	// size of each plane in sgl_uint32.
	*/
	PVRParamBuffs[PVR_PARAM_TYPE_ISP].uBufferPos += numPlanes * WORDS_PER_PLANE;


	/*************************************************************/
	/******************* Add the forward planes ******************/
	/*************************************************************/
	if(!AddDummyForward)
  	{
		/*/////////////////////
		// Determine if we have any visible forward planes.
		// If we do, then the first one has a special instruction.
		// (otherwise, put the special instruction on the invisible planes)
		//
		// Note this does the First Invisible forward plane.
		///////////////////// */
		if(pPlaneLists->NumFV !=0)
		{
			/*
			// get pointer to the first forward Vis entry in the list
			*/
			ppPlane = pPlaneLists->FVPlanes;

			/*
			// Do the first forward plane
			*/
			pPlane = *ppPlane;

			PackPlane(pPlaneMem, pPlane, forw_visib_fp, pPlane->u32TexasTag);

			pPlaneMem+=WORDS_PER_PLANE;
			ppPlane++;

			/*
			// Do the remaining forward planes
			*/
			for (i =(pPlaneLists->NumFV - 1); i > 0; i--)			
			{
				/*
				// Get pointer to this plane
				*/
				pPlane = *ppPlane;

				PackPlane(pPlaneMem, pPlane, forw_visib, pPlane->u32TexasTag);

				pPlaneMem+=WORDS_PER_PLANE;
				ppPlane++;

	   		}/*end for forward visibles*/

			/*
			// Do the very first Invisible Forward (if there is one)
			*/
			if(pPlaneLists->NumFI != 0)
			{
				/*
				// get pointer to the first entry in the list,
				// and a pointer to the first plane
				*/
				ppPlane = pPlaneLists->FIPlanes;
				pPlane  = *ppPlane;

				/*
				// Store the plane values. The instruction is for
				// a standard invisible plane
				*/
				PackPlane(pPlaneMem, pPlane, forw_invis, DEBUG_INVIS_FP_TAG);

				/*
				// Move on to the next plane
				*/
				pPlaneMem+=WORDS_PER_PLANE;
				ppPlane++;

			}/*end if FI planes*/
		}/*end if FV planes*/

		/*/////////////////////
		// ELSE we must therefore have a forward Invisible plane
		// which must be the first forward plane
		///////////////////// */
		else
		{
			ASSERT(pPlaneLists->NumFI != 0);

			/*
			// get pointer to the first entry in the list,
			// and a pointer to the first plane
			*/
			ppPlane = pPlaneLists->FIPlanes;
			pPlane  = *ppPlane;

			/*
			// Store the plane values - the instruction is for
			// the FIRST invisible plane
			*/
			PackPlane(pPlaneMem, pPlane, forw_invis_fp, DEBUG_INVIS_FP_TAG);

			/*
			// Move on to the next plane
			*/
			pPlaneMem+=WORDS_PER_PLANE;
			ppPlane++;

		}/*end if then else*/

		/*/////////////////////
		// Do the remaining forward invisible planes.
		// NOTE that ppPlane is already pointing to the second
		// invisible plane (if any)
		///////////////////// */
		for (i = (pPlaneLists->NumFI - 1); i > 0; i--)			
		{
			pPlane = *ppPlane;
				
			PackPlane(pPlaneMem, pPlane, forw_invis, DEBUG_INVIS_FP_TAG);

			/*
			// Move on to the next plane
			*/
			pPlaneMem+=WORDS_PER_PLANE;
			ppPlane++;
		}/*end for forward invisible*/


		/* /////////////////////
		// Do we need to add a clipping plane?
		///////////////////// */
		if (AddClipPlane)
		{
   			/* add a  forward clip plane */
#if (PCX2 || PCX2_003) && !FORCE_NO_FPU
			PackFlatOnPlane(pPlaneMem, f32FixedClipDist,
							forw_invis, DEBUG_CLIP_PLANE_TAG);
#else
			PackFlatOnPlane(pPlaneMem, n32FixedClipDist,
							forw_invis, DEBUG_CLIP_PLANE_TAG);
#endif
			pPlaneMem+=WORDS_PER_PLANE;
 	
  		}/*end if add clip plane*/


	}  /* no dummy plane*/

	/*
	// Else add a dummy forward plane
	*/
   	else
   	{
   	   /* add a single dummy forward plane */
#if (PCX2 || PCX2_003) && !FORCE_NO_FPU
		PackFlatOnPlane(pPlaneMem, f32FixedClipDist,
						forw_invis_fp, DEBUG_DUMMY_F_TAG);
#else
		PackFlatOnPlane(pPlaneMem, n32FixedClipDist,
						forw_invis_fp, DEBUG_DUMMY_F_TAG);
#endif

   		pPlaneMem+=WORDS_PER_PLANE;

   	}/*end if then else*/


	/*************************************************************/
	/************* Add the perpendicular planes ******************/
	/*************************************************************/

	ppPlane = pPlaneLists->PEPlanes;

	for (i = pPlaneLists->NumPE; i > 0; i--)			
	{
		/*
		// Get easy access to this plane
		*/
		pPlane = *ppPlane;

	   	/* add the perps */

		PackPlane(pPlaneMem, pPlane, forw_perp, DEBUG_PERP_TAG);

		pPlaneMem+=WORDS_PER_PLANE;
		ppPlane++;
   	}



	/*************************************************************/
	/******************* Add the reverse planes ******************/
	/*************************************************************/

	ppPlane = pPlaneLists->RVPlanes;

	/*
	// Do ALL but the last reverse visible plane. It MAY need a different
	// instruction (NOTE these aren't to be seen yet, so actually use
	// an invisible instruction)
	*/
   	for (i = (pPlaneLists->NumRV - 1); i > 0; i--)			
	{
		/*
		// Get easy access to this plane
		*/
		pPlane = *ppPlane;

	 	/* add the Reverse visible */
		PackPlane(pPlaneMem, pPlane, rev_invis, DEBUG_INVIS_RP_TAG);

		pPlaneMem+=WORDS_PER_PLANE;
		ppPlane++;
   	}

	/*
	// Do the last visible reverse
	*/
   	if(pPlaneLists->NumRV != 0)			
	{
		/*
		// Get easy access to this plane
		*/
		pPlane = *ppPlane;

		 /* last reverse must have special instruction */
	  	if (RepeatReverse && (pPlaneLists->NumRI==0))
		{
			PackPlane(pPlaneMem, pPlane, rev_replace_if, DEBUG_INVIS_RP_TAG);
		}
	   	else
		{
			PackPlane(pPlaneMem, pPlane, rev_invis, DEBUG_INVIS_RP_TAG);
		}

		pPlaneMem+=WORDS_PER_PLANE;
   	}

	/*
	// Do the Rev INVIS planes.
	//
	// Again, do all but the last one
	*/
	ppPlane = pPlaneLists->RIPlanes;

   	for (i= (pPlaneLists->NumRI -1); i > 0; i--)			
   	{
		/*
		// Get easy access to this plane
		*/
		pPlane = *ppPlane;

   		/* add the Reverse invisible */

		PackPlane(pPlaneMem, pPlane, rev_invis, DEBUG_INVIS_RP_TAG);

		pPlaneMem+=WORDS_PER_PLANE;
		ppPlane ++;
	}

	/*
	// Do the last one
	*/
	if(pPlaneLists->NumRI != 0)
	{
		/*
		// Get easy access to this plane
		*/
		pPlane = *ppPlane;

   		/* add the Reverse invisible */
		/*
		// last reverse must have special instruction for
		// objects with invisible plane
		*/
   		if (RepeatReverse)
		{
			PackPlane(pPlaneMem, pPlane, rev_replace_if, DEBUG_INVIS_RP_TAG);
		}
   		else
		{
			PackPlane(pPlaneMem, pPlane, rev_invis, DEBUG_INVIS_RP_TAG);
		}
		pPlaneMem+=WORDS_PER_PLANE;
	}


	/*************************************************************/
	/****** Add the reverse planes AGAIN (if needed )*************/
	/*************************************************************/	
   	if (RepeatReverse)
   	{
		ppPlane = pPlaneLists->RVPlanes;

   	   	for (i = pPlaneLists->NumRV; i>0; i--)			
   	   	{
			/*
			// Get easy access to this plane
			*/
			pPlane = *ppPlane;

	 		/* add the Reverse visible */

			PackPlane(pPlaneMem, pPlane, rev_visib, pPlane->u32TexasTag);

			pPlaneMem+=WORDS_PER_PLANE;
			ppPlane ++;
   	   	}

		ppPlane = pPlaneLists->RIPlanes;
		
	   	for (i = pPlaneLists->NumRI; i>0; i--)			
	  	{
			/*
			// Get easy access to this plane
			*/
			pPlane = *ppPlane;

    	  	/* add the Reverse invisible */
			PackPlane(pPlaneMem, pPlane, rev_invis, DEBUG_INVIS_RP_TAG);

			pPlaneMem+=WORDS_PER_PLANE;
			ppPlane ++;
	  	}
   	}/*end if repeat reverse*/


	SGL_TIME_STOP(PACK_OPAQUE_TIME)
	return	numPlanes;

}

/**************************************************************************
 * Function Name  : PackLightShadVolParams
 * Inputs         :
 * Outputs        :
 * Input/Output	  :
 * Returns        :
 * Global Used    :
 *
 * Description    : Takes in a group of planes for a single convex primitive
 *					that is either a shadow or light volume, and packs it
 *					accordingly, for the ISP/Sabre. Returns TRUE
 *				
 **************************************************************************/

int	PackLightShadVolParams( const PLANE_CATEGORIES_STRUCT * pPlaneLists,
							const sgl_bool IsShadowVol)
{
	sgl_bool	AddDummyForward;
	sgl_bool	AddDummyReverse;

	int	numForward, numReverse, numPlanes;
	int i;

	sgl_uint32 * pPlaneMem;

#if (PCX2 || PCX2_003) && !FORCE_NO_FPU
	float f32FixedClipDist = RnGlobalGetFixedClipDist();
#else
	sgl_uint32 n32FixedClipDist	= RnGlobalGetFixedClipDist();
#endif
	/*
	// Keep a pointer into the various plane pointer lists,
	// as well as a pointer to the plane data itself
	*/
	TRANSFORMED_PLANE_STRUCT  * const * ppPlane;
	const TRANSFORMED_PLANE_STRUCT  *  pPlane;
	sgl_uint32 CurrentLimit;

	ASSERT(pPlaneLists->NumFI == 0);
	ASSERT(pPlaneLists->NumRI == 0);

	SGL_TIME_START(PACK_LIGHTSHADVOL_TIME)

	numForward = pPlaneLists->NumFV;
	numReverse = pPlaneLists->NumRV;

	/*
	// The hardware ALWAYS requires a forward (or perp plane) and a reverse
	// plane. To make life simple always have a forward plane.
	*/
	if (numForward == 0)
	{
		AddDummyForward = TRUE;	
		numForward = 1;	
	}
	else
	{
		AddDummyForward = FALSE;	
	}
		

	if (numReverse == 0)
	{
		AddDummyReverse = TRUE;	
		numReverse = 1;	
	}
	else
	{
		AddDummyReverse = FALSE;	
	}

	/*
	// how many planes are going in the hardware? and is there room?
	*/
	numPlanes = numForward + numReverse + pPlaneLists->NumPE;

#define CHECK_LIMITS 1
#if CHECK_LIMITS
	CurrentLimit = GetSabreLimit (PVRParamBuffs[PVR_PARAM_TYPE_ISP].uBufferPos);
	
	if (CurrentLimit == 0)
	{
 		/* this isnt enough space */
		DPFDEV ((DBG_WARNING, "PackLightShadVolParams: Out of sabre memory pages !"));
			
		SGL_TIME_STOP(PACK_OPAQUE_TIME)
		return(0);
	}

	if ((PVRParamBuffs[PVR_PARAM_TYPE_ISP].uBufferPos + 
		 (numPlanes * WORDS_PER_PLANE )) > CurrentLimit)
	{
		CurrentLimit = GetSabreLimit (CurrentLimit++);
		
		if (CurrentLimit == 0)
		{
			/* this isnt enough space */
			DPFDEV ((DBG_WARNING, "PackLightShadVolParams: Out of sabre memory pages !"));
			
			SGL_TIME_STOP(PACK_OPAQUE_TIME)
			return 0;
		}

		PVRParamBuffs[PVR_PARAM_TYPE_ISP].uBufferPos = 
			GetStartOfObject (PVRParamBuffs[PVR_PARAM_TYPE_ISP].uBufferPos, 
							  (numPlanes * WORDS_PER_PLANE ));
		
		if ((PVRParamBuffs[PVR_PARAM_TYPE_ISP].uBufferPos + 
			 (numPlanes * WORDS_PER_PLANE )) > CurrentLimit)
		{
			/* this isnt enough space */
			DPFDEV ((DBG_WARNING, "PackLightShadVolParams: Out of sabre memory! %d planes in object ", numPlanes));
			
			SGL_TIME_STOP(PACK_OPAQUE_TIME)
			return 0;
		}

	}
#endif


	/* Get where we Add the object to the plane parameter memory */
   	pPlaneMem = &PVRParamBuffs[PVR_PARAM_TYPE_ISP].pBuffer[PVRParamBuffs[PVR_PARAM_TYPE_ISP].uBufferPos];

	/*
	// Up the index by the number of planes we've added, times the
	// size of each plane in sgl_uint32.
	*/
	PVRParamBuffs[PVR_PARAM_TYPE_ISP].uBufferPos += WORDS_PER_PLANE * numPlanes;

	/*************************************************************/
	/******************* Add the forward planes ******************/
	/*************************************************************/

	/*///////////////////////
	// If there weren't any forward planes
	/////////////////////// */
	if(AddDummyForward)
  	{
		/*
		// If there are no perps, then use First and Test
		*/
		if(pPlaneLists->NumPE == 0)
		{
#if (PCX2 || PCX2_003) && !FORCE_NO_FPU
			PackFlatOnPlane(pPlaneMem, f32FixedClipDist,
							test_shad_forw_fp, DEBUG_DUMMY_F_TAG);
#else
			PackFlatOnPlane(pPlaneMem, n32FixedClipDist,
							test_shad_forw_fp, DEBUG_DUMMY_F_TAG);
#endif
		}
		/*
		// Else use First plane - the test will be done in the perps
		*/
		else
		{
#if (PCX2 || PCX2_003) && !FORCE_NO_FPU
			PackFlatOnPlane(pPlaneMem, f32FixedClipDist,
							forw_visib_fp, DEBUG_DUMMY_F_TAG);
#else
			PackFlatOnPlane(pPlaneMem, n32FixedClipDist,
							forw_visib_fp, DEBUG_DUMMY_F_TAG);
#endif
		}

   		pPlaneMem+=WORDS_PER_PLANE;
	}
	/* ///////////////////////
	// ELSE If there is only ONE (original) forward plane
	/////////////////////// */
	else if(numForward == 1)
	{
		/*
		// get pointer to the first forward plane
		*/
		pPlane = *(pPlaneLists->FVPlanes);

		/*
		// If there are no perps, then use First and Test
		*/
		if(pPlaneLists->NumPE == 0)
		{
			PackPlane(pPlaneMem, pPlane, test_shad_forw_fp, DEBUG_DUMMY_F_TAG);
		}
		/*
		// Else use First plane - the test will be done in the perps
		*/
		else
		{
			PackPlane(pPlaneMem, pPlane, forw_visib_fp, DEBUG_DUMMY_F_TAG);
		}

		pPlaneMem+=WORDS_PER_PLANE;
	}
	/*///////////////////////
	// Else there is more than one forward plane..
	/////////////////////// */
	else
	{
		/*
		// get pointer to the first forward Vis entry in the list
		*/
		ppPlane = pPlaneLists->FVPlanes;

		/*
		// Do the first forward plane. NOTE we KNOW there is more
		// than 1 forward plane
		*/
		pPlane = *ppPlane;

		PackPlane(pPlaneMem, pPlane, forw_visib_fp, DEBUG_DUMMY_F_TAG);
		
		pPlaneMem+=WORDS_PER_PLANE;
		ppPlane++;

		/*
		// do all the remaining BUT  the last forward plane
		*/
		for (i = (numForward - 2); i > 0; i--)			
		{
			/*
			// Get pointer to this plane
			*/
			pPlane = *ppPlane;

			PackPlane(pPlaneMem, pPlane, forw_visib, DEBUG_DUMMY_F_TAG);

			pPlaneMem+=WORDS_PER_PLANE;
			ppPlane++;

	   	}/*end for forward visibles*/

		/*
		// Do the last forward visible. If there aren't any perp
		// planes, then use the test fp instruction, else the test
		// will be done in the perps
		*/
		pPlane = *ppPlane;

		if(pPlaneLists->NumPE == 0)
		{
			PackPlane(pPlaneMem, pPlane, test_shad_forw, DEBUG_DUMMY_F_TAG);
		}
		else
		{
			PackPlane(pPlaneMem, pPlane, forw_visib, DEBUG_DUMMY_F_TAG);
		}

		pPlaneMem+=WORDS_PER_PLANE;

   	}/*end if then else*/


	/*************************************************************/
	/************* Add the perpendicular planes ******************/
	/*************************************************************/
	if(pPlaneLists->NumPE > 0)
	{
		/*
		// Get access to the first pointer in the array
		*/
		ppPlane = pPlaneLists->PEPlanes;
		
		/*
		// Do all but the last one.. the last one gets the test
		// instruction
		*/
		for (i = (pPlaneLists->NumPE - 1); i > 0; i --)
		{
			/*
			// Get easy access to this plane
			*/
			pPlane = *ppPlane;

			/* add the perp */
			PackPlane(pPlaneMem, pPlane, forw_perp, DEBUG_PERP_TAG);

			pPlaneMem+=WORDS_PER_PLANE;
			ppPlane++;

		} /*end for all but last perp*/
		
		/*
		// The last one gets the test instruction (the lucky
		// devil)
		*/
		pPlane = *ppPlane;

		/* add the perp */
		PackPlane(pPlaneMem, pPlane, test_shad_perp, DEBUG_PERP_TAG);

		pPlaneMem+=WORDS_PER_PLANE;

	}/*end if some perp planes*/

	/*************************************************************/
	/******************* Add the reverse planes ******************/
	/*************************************************************/

	/*///////////////////////
	// If there weren't any Reverse planes
	/////////////////////// */
	if(AddDummyReverse)
	{
		/*
		// The command we store depends on whether we are doing a
		// light volome or a shadow volume
		*/
		if(IsShadowVol)
		{
#if (PCX2 || PCX2_003) && !FORCE_NO_FPU
			PackFlatOnPlane(pPlaneMem, 0.0f,
							test_shadow_rev, DEBUG_DUMMY_F_TAG);
#else
			PackFlatOnPlane(pPlaneMem, 0,
							test_shadow_rev, DEBUG_DUMMY_F_TAG);
#endif

		}
		else
		{
#if (PCX2 || PCX2_003) && !FORCE_NO_FPU
			PackFlatOnPlane(pPlaneMem, 0.0f,
							test_light_rev, DEBUG_DUMMY_F_TAG);
#else
			PackFlatOnPlane(pPlaneMem, 0,
							test_light_rev, DEBUG_DUMMY_F_TAG);
#endif

		}
	}
	/*///////////////////////
	// Else there are more than one forward plane..
	/////////////////////// */
	else
	{
		/*
		// get easy access to the list of pointers
		*/
		ppPlane = pPlaneLists->RVPlanes;
		
		/*
		// Do all but the last one
		*/
		for (i = (pPlaneLists->NumRV - 1); i > 0; i-- )
		{
			/*
			// Get easy access to this plane
			*/
			pPlane = *ppPlane;

			/* add the Reverse */
			PackPlane(pPlaneMem, pPlane, rev_visib, DEBUG_DUMMY_R_TAG);

			pPlaneMem+=WORDS_PER_PLANE;
			ppPlane++;
		}/*end for all but last one*/


		/*
		// on the very last, but the test command - the command depends on
		// what we are packing
		*/
		pPlane = *ppPlane;

		if(IsShadowVol)
		{
			PackPlane(pPlaneMem, pPlane, test_shadow_rev, DEBUG_DUMMY_R_TAG);
		}
		else
		{
			PackPlane(pPlaneMem, pPlane, test_light_rev, DEBUG_DUMMY_R_TAG);
		}

	}/*end if else on reverse planes*/

	SGL_TIME_STOP(PACK_LIGHTSHADVOL_TIME)

	return	numPlanes;

}





/**************************************************************************
 * Function Name  : PackMeshParamsOrdered
 * Inputs         :
 * Outputs        :
 * Input/Output	  :
 * Returns        :
 * Global Used    :
					
 * Description    : Packs a mesh polygon into sabre memory
 **************************************************************************/

int PackMeshParamsOrdered ( const TRANSFORMED_PLANE_STRUCT Planes[],
						  XMESHEXTRA Extras[],
						  sgl_int32 *Order,
						  const int nNumPlanes)
{
	int	  nPlane,  nEdges, nSabrePlanes, nTotalPlanes = 0;
	const TRANSFORMED_PLANE_STRUCT 	*pPlane;
	PTRANSEDGE					pEdge;
	PXMESHEXTRA					pExtra;
   	sgl_uint32						*pPlaneMem = &PVRParamBuffs[PVR_PARAM_TYPE_ISP].pBuffer[PVRParamBuffs[PVR_PARAM_TYPE_ISP].uBufferPos];
#if (PCX2 || PCX2_003) && !FORCE_NO_FPU
	float f32FixedClipDist = RnGlobalGetFixedClipDist();
#else
	sgl_uint32 n32FixedClipDist	= RnGlobalGetFixedClipDist();
#endif
	
	/* need to check to see if we have enough space in the parameter memory */
	SGL_TIME_START(PACK_MESH_ORDERED_TIME)

	if ((PVRParamBuffs[PVR_PARAM_TYPE_ISP].uBufferPos + ((6*WORDS_PER_PLANE) * nNumPlanes)) > PVRParamBuffs[PVR_PARAM_TYPE_ISP].uBufferLimit)
	{
		/* this isnt enough space */
		DPFDEV ((DBG_WARNING, "PackMeshParamsOrdered: Out of sabre memory!"));

	    SGL_TIME_STOP(PACK_MESH_ORDERED_TIME)
		return (0);
	}

	for (nPlane = 0; nPlane < nNumPlanes; ++nPlane)
	{
		int nOrderedPlane = Order[nPlane];
		pPlane = Planes + nOrderedPlane;
		pExtra = Extras + nOrderedPlane;
		nEdges = pExtra->nEdges;
		
		/* add the visible polygon plane */
	
		PackPlane(pPlaneMem, pPlane, forw_visib_fp, pPlane->u32TexasTag);
		pPlaneMem += WORDS_PER_PLANE;
		
		nSabrePlanes = 1;

				
		/*
		// add the edges
		//
		// We have either 3 or 4 edges. Note that it doesnt matter what
		// order we are doing things in.
		*/
		ASSERT((nEdges == 4) || (nEdges == 3));

		if(nEdges == 4)
		{
			#define EDGE_OFF 3

			pEdge = pExtra->pE[EDGE_OFF];
				
			if (pExtra->u32EdgeFlags & (1 << EDGE_OFF))
			{
				/* we need to reverse the sense of the edge */

				PackEdge(pPlaneMem, pEdge);
			}
			else
			{
				/* edge is oriented correctly */

				PackNegatedEdge(pPlaneMem, pEdge);
			}
	
			pPlaneMem += WORDS_PER_PLANE;
			++nSabrePlanes;

			#undef EDGE_OFF
		}/*end if 4 */

		/*
		// Do the remaining edges
		*/
		#define EDGE_OFF 2

		pEdge = pExtra->pE[EDGE_OFF];

		if (pExtra->u32EdgeFlags & (1 << EDGE_OFF))
		{
			/* we need to reverse the sense of the edge */

			PackNegatedEdge(pPlaneMem, pEdge);
		}
		else
		{
			/* edge is oriented correctly */

			PackEdge(pPlaneMem, pEdge);
		}
	
		pPlaneMem += WORDS_PER_PLANE;
		++nSabrePlanes;

		#undef EDGE_OFF



		#define EDGE_OFF 1

		pEdge = pExtra->pE[EDGE_OFF];
		if (pExtra->u32EdgeFlags & (1 << EDGE_OFF))
		{
			/* we need to reverse the sense of the edge */

			PackNegatedEdge(pPlaneMem, pEdge);
		}
		else
		{
			/* edge is oriented correctly */

			PackEdge(pPlaneMem, pEdge);
		}
	
		pPlaneMem += WORDS_PER_PLANE;
		++nSabrePlanes;

		#undef EDGE_OFF

		#define EDGE_OFF 2

		pEdge = pExtra->pE[EDGE_OFF];

		if (pExtra->u32EdgeFlags & (1 << EDGE_OFF))
		{
			/* we need to reverse the sense of the edge */

			PackNegatedEdge(pPlaneMem, pEdge);
		}
		else
		{
			/* edge is oriented correctly */

			PackEdge(pPlaneMem, pEdge);
		}
	
		pPlaneMem += WORDS_PER_PLANE;
		++nSabrePlanes;

		#undef EDGE_OFF


		#define EDGE_OFF 1

		pEdge = pExtra->pE[EDGE_OFF];
		if (pExtra->u32EdgeFlags & (1 << EDGE_OFF))
		{
			/* we need to reverse the sense of the edge */

			PackNegatedEdge(pPlaneMem, pEdge);
		}
		else
		{
			/* edge is oriented correctly */

			PackEdge(pPlaneMem, pEdge);
		}
	
		pPlaneMem += WORDS_PER_PLANE;
		++nSabrePlanes;

		#undef EDGE_OFF

		#define EDGE_OFF 0

		pEdge = pExtra->pE[EDGE_OFF];
		if (pExtra->u32EdgeFlags & (1 << EDGE_OFF))
		{
			/* we need to reverse the sense of the edge */

			PackNegatedEdge(pPlaneMem, pEdge);
		}
		else
		{
			/* edge is oriented correctly */

			PackEdge(pPlaneMem, pEdge);
		}
	
		pPlaneMem += WORDS_PER_PLANE;
		++nSabrePlanes;

		#undef EDGE_OFF




		/* add clip plane if necessary */
		
		if (pExtra->ZClipped)
		{
			/* add a  forward clip plane */
#if (PCX2 || PCX2_003) && !FORCE_NO_FPU
			PackFlatOnPlane(pPlaneMem, f32FixedClipDist,
							forw_invis, DEBUG_CLIP_PLANE_TAG);
#else
			PackFlatOnPlane(pPlaneMem, n32FixedClipDist,
							forw_invis, DEBUG_CLIP_PLANE_TAG);
#endif

			pPlaneMem += WORDS_PER_PLANE;
			++nSabrePlanes;
		}

		pExtra->u32SabreIndex = PVRParamBuffs[PVR_PARAM_TYPE_ISP].uBufferPos;
		pExtra->nSabrePlanes =  nSabrePlanes;
		PVRParamBuffs[PVR_PARAM_TYPE_ISP].uBufferPos += WORDS_PER_PLANE * nSabrePlanes;
		nTotalPlanes += nSabrePlanes;
	}	
	
	SGL_TIME_STOP(PACK_MESH_ORDERED_TIME)
	return (nTotalPlanes);
}


/**************************************************************************
 * Function Name  : PackMeshParams
 * Inputs         :
 * Outputs        :
 * Input/Output	  :
 * Returns        :
 * Global Used    :
					
 * Description    : Packs a mesh polygon into sabre memory
 **************************************************************************/

int PackMeshParams( const TRANSFORMED_PLANE_STRUCT *pPlane,
					XMESHEXTRA *pExtra,
					const int nNumPlanes )
{
	int	  nEdges, nCounter, nSabrePlanes, nTotalPlanes = 0;
	PTRANSEDGE					pEdge;
   	sgl_uint32						*pPlaneMem = &PVRParamBuffs[PVR_PARAM_TYPE_ISP].pBuffer[PVRParamBuffs[PVR_PARAM_TYPE_ISP].uBufferPos];
	sgl_uint32						CurrentLimit;
#if (PCX2 || PCX2_003) && !FORCE_NO_FPU
	float f32FixedClipDist = RnGlobalGetFixedClipDist();
#else
	sgl_uint32 n32FixedClipDist	= RnGlobalGetFixedClipDist();
#endif

	/* need to check to see if we have enough space in the parameter memory */

	SGL_TIME_START(PACK_MESH_TIME)

	CurrentLimit = GetSabreLimit (PVRParamBuffs[PVR_PARAM_TYPE_ISP].uBufferPos);

	if (CurrentLimit == 0)
	{
		return(0); /* nTotalPlanes */
	}

	for ( nCounter = nNumPlanes; nCounter!=0 ;nCounter--, pPlane++, pExtra++)
	{
		nEdges = pExtra->nEdges;
		
		if (PVRParamBuffs[PVR_PARAM_TYPE_ISP].uBufferPos + (6 * WORDS_PER_PLANE) >= CurrentLimit)
		{
			CurrentLimit = GetSabreLimit (CurrentLimit);
			
			if (CurrentLimit == 0)
			{
				return(nTotalPlanes);
			}

			PVRParamBuffs[PVR_PARAM_TYPE_ISP].uBufferPos = GetStartOfObject (PVRParamBuffs[PVR_PARAM_TYPE_ISP].uBufferPos, (6 * WORDS_PER_PLANE));

			if (PVRParamBuffs[PVR_PARAM_TYPE_ISP].uBufferPos + (6 * WORDS_PER_PLANE) >= CurrentLimit)
			{
				DPFDEV ((DBG_WARNING, "PackMeshParams: Out of ISP buffer space"));
			
				break;
			}
			else
			{
				pPlaneMem = &PVRParamBuffs[PVR_PARAM_TYPE_ISP].pBuffer[PVRParamBuffs[PVR_PARAM_TYPE_ISP].uBufferPos];
			}
		}

		/* add the visible polygon plane */
	
		PackPlane(pPlaneMem, pPlane, forw_visib_fp, pPlane->u32TexasTag);
		pPlaneMem += WORDS_PER_PLANE;

		/*
		// add the edges
		//
		// We have at most 4 edges, so rather than do a loop, do a switch.
		// which drops into the right place and then through the remaining
		// ones. Note that it doesn't matter which order we do the edges.
		*/
		ASSERT((nEdges == 4)|| (nEdges == 3));

		if(nEdges == 4)
		{
			nSabrePlanes = 5;
			
			#define EDGE_OFF 3

			pEdge = pExtra->pE[EDGE_OFF];
				
			if (pExtra->u32EdgeFlags & (1 << EDGE_OFF))
			{
				/* we need to reverse the sense of the edge */
	
				PackNegatedEdge(pPlaneMem, pEdge);
			}
			else
			{
				/* edge is oriented correctly */

				PackEdge(pPlaneMem, pEdge);
			}
	
			pPlaneMem += WORDS_PER_PLANE;

			#undef EDGE_OFF
		}
		/*
		// Else only 3 edges
		*/
		else
		{
			nSabrePlanes = 4;
		}

		/*
		// the other edges
		*/
		#define EDGE_OFF 2

		pEdge = pExtra->pE[EDGE_OFF];
		if (pExtra->u32EdgeFlags & (1 << EDGE_OFF))
		{
			/* we need to reverse the sense of the edge */

			PackNegatedEdge(pPlaneMem, pEdge);
		}
		else
		{
			/* edge is oriented correctly */
			PackEdge(pPlaneMem, pEdge);
		}
	
		#undef EDGE_OFF


		#define EDGE_OFF 1

		pEdge = pExtra->pE[EDGE_OFF];
		if (pExtra->u32EdgeFlags & (1 << EDGE_OFF))
		{
			/* we need to reverse the sense of the edge */

			PackNegatedEdge(pPlaneMem + WORDS_PER_PLANE, pEdge);
		}
		else
		{
			/* edge is oriented correctly */
			PackEdge(pPlaneMem + WORDS_PER_PLANE, pEdge);
		}
	
		#undef EDGE_OFF



		#define EDGE_OFF 0

		pEdge = pExtra->pE[EDGE_OFF];
		if (pExtra->u32EdgeFlags & (1 << EDGE_OFF))
		{
			/* we need to reverse the sense of the edge */

			PackNegatedEdge(pPlaneMem + (2*WORDS_PER_PLANE), pEdge);
		}
		else
		{
			/* edge is oriented correctly */
			PackEdge(pPlaneMem + (2*WORDS_PER_PLANE), pEdge);
		}
	
		pPlaneMem += 3 * WORDS_PER_PLANE;

		#undef EDGE_OFF


		

		/* add clip plane if necessary */
		
		if (pExtra->ZClipped)
		{
			/* add a  forward clip plane */
#if (PCX2 || PCX2_003) && !FORCE_NO_FPU
			PackFlatOnPlane(pPlaneMem, f32FixedClipDist,
							forw_invis, DEBUG_CLIP_PLANE_TAG);
#else
			PackFlatOnPlane(pPlaneMem, n32FixedClipDist,
							forw_invis, DEBUG_CLIP_PLANE_TAG);
#endif
			pPlaneMem += WORDS_PER_PLANE;
			++nSabrePlanes;
		}

		pExtra->u32SabreIndex = PVRParamBuffs[PVR_PARAM_TYPE_ISP].uBufferPos;
		pExtra->nSabrePlanes =  nSabrePlanes;
		PVRParamBuffs[PVR_PARAM_TYPE_ISP].uBufferPos += WORDS_PER_PLANE * nSabrePlanes;
		nTotalPlanes += nSabrePlanes;
	}	
	
	SGL_TIME_STOP(PACK_MESH_TIME)
	return (nTotalPlanes);
}


/**************************************************************************
 * Function Name  : PackBackgroundPlane
 * Inputs         : Texas tag for the colour
 * Outputs        :
 * Input/Output	  :
 * Returns        : "Location" of the background object
 * Global Used    :
					
 * Description    : Puts a background plane into the sabre parameter space.
 **************************************************************************/

#if (PCX2 || PCX2_003) && !FORCE_NO_FPU
void PackBackgroundPlane (const sgl_uint32 u32TexasTag,
						 const float  BackgroundDistance)
#else
void PackBackgroundPlane (const sgl_uint32 u32TexasTag,
						 const sgl_int32  BackgroundDistance)
#endif
{
	sgl_uint32 * pPlaneMem;

	if ((PVRParamBuffs[PVR_PARAM_TYPE_ISP].uBufferPos+(1*WORDS_PER_PLANE))>PVRParamBuffs[PVR_PARAM_TYPE_ISP].uBufferLimit)
	{
		/* this isnt enough space */
		DPFDEV ((DBG_FATAL, "PackBackgroundPlane: Out of sabre memory for background!"));
	
		DPF ((DBG_FATAL, "This shouldnt happen!"));
		ASSERT(FALSE);
		return ;
	}
	/*
	// Get a pointer to it
	*/
   	pPlaneMem = &PVRParamBuffs[PVR_PARAM_TYPE_ISP].pBuffer[PVRParamBuffs[PVR_PARAM_TYPE_ISP].uBufferPos];

	PackFlatOnPlane(pPlaneMem, BackgroundDistance,
					forw_visib_fp, u32TexasTag);

	PVRParamBuffs[PVR_PARAM_TYPE_ISP].uBufferPos +=WORDS_PER_PLANE;
	
}


/**************************************************************************
 * Function Name  : PackOpaqueDummy
 * Inputs         : NONE
 * Outputs        : NONE
 * Input/Output	  :
 * Returns        :
 * Global Used    :
					
 * Description    : Puts a dummy translucent object into the sabre parameter space.
 **************************************************************************/

void PackOpaqueDummy()
{
	sgl_uint32 * pPlaneMem;
	int Plane;

	if ((PVRParamBuffs[PVR_PARAM_TYPE_ISP].uBufferPos+(NUM_DUMMY_OBJECT_PLANES*WORDS_PER_PLANE))>PVRParamBuffs[PVR_PARAM_TYPE_ISP].uBufferLimit)
	{
		/* this isnt enough space */
		DPFDEV ((DBG_FATAL, "PackOpaqueDummy: Out of sabre memory for opaque dummy!"));
		
		DPF ((DBG_FATAL, "This shouldnt happen!"));
		ASSERT(FALSE);
		return ;
	}
	/*
	// Get a pointer to it
	*/
   	pPlaneMem = &PVRParamBuffs[PVR_PARAM_TYPE_ISP].pBuffer[PVRParamBuffs[PVR_PARAM_TYPE_ISP].uBufferPos];

#if (PCX2 || PCX2_003) && !FORCE_NO_FPU
	PackFlatOnPlane(pPlaneMem, -1.0f,
					forw_visib_fp, 0xBBFFFAB0UL>>4);
#else
	PackFlatOnPlane(pPlaneMem, -128,
					forw_visib_fp, 0xBBFFFAB0UL>>4);
#endif

  	pPlaneMem+=WORDS_PER_PLANE;
	PVRParamBuffs[PVR_PARAM_TYPE_ISP].uBufferPos +=WORDS_PER_PLANE;

   	for (Plane=0; Plane < NUM_DUMMY_OBJECT_PLANES-1 ; Plane++)			
   	{
#if (PCX2 || PCX2_003) && !FORCE_NO_FPU
		PackFlatOnPlane(pPlaneMem, -1.0f,
						rev_invis, 0xBBFFFBB0UL>>4);
#else
		PackFlatOnPlane(pPlaneMem, -128,
						rev_invis, 0xBBFFFBB0UL>>4);
#endif

   		pPlaneMem+=WORDS_PER_PLANE;
	   	PVRParamBuffs[PVR_PARAM_TYPE_ISP].uBufferPos +=WORDS_PER_PLANE;

   	}/*end for forward visibles*/
	
}


#if VIGNETTE_FIX
/**************************************************************************
 * Function Name  : PackOpaqueDummyLarge
 * Inputs         : NONE
 * Outputs        : NONE
 * Input/Output	  :
 * Returns        :
 * Global Used    :
					
 * Description    : Puts a large dummy opaque object into the sabre parameter
 *					space. May be required for the vignetting fix.
 **************************************************************************/

void PackOpaqueDummyLarge()
{
	sgl_uint32 * pPlaneMem;
	int Plane;

	if ((PVRParamBuffs[PVR_PARAM_TYPE_ISP].uBufferPos+(NUM_DUMMY_OBJECT_PLANES_LARGE*WORDS_PER_PLANE))>PVRParamBuffs[PVR_PARAM_TYPE_ISP].uBufferLimit)
	{
		/* this isnt enough space */
		DPFDEV ((DBG_FATAL, " PackOpaqueDummyLarge: Out of sabre memory for opaque dummy!"));
	
		DPF ((DBG_FATAL, "This shouldnt happen!"));
		ASSERT(FALSE);
		return ;
	}
	/*
	// Get a pointer to it
	*/
   	pPlaneMem = &PVRParamBuffs[PVR_PARAM_TYPE_ISP].pBuffer[PVRParamBuffs[PVR_PARAM_TYPE_ISP].uBufferPos];

#if (PCX2 || PCX2_003) && !FORCE_NO_FPU
	PackFlatOnPlane(pPlaneMem, -1.0f,
					forw_visib_fp, 0xBBFFFAB0UL>>4);
#else
	PackFlatOnPlane(pPlaneMem, -128,
					forw_visib_fp, 0xBBFFFAB0UL>>4);
#endif

  	pPlaneMem+=WORDS_PER_PLANE;
	PVRParamBuffs[PVR_PARAM_TYPE_ISP].uBufferPos +=WORDS_PER_PLANE;

   	for (Plane=0; Plane < NUM_DUMMY_OBJECT_PLANES_LARGE-1 ; Plane++)
   	{
#if (PCX2 || PCX2_003) && !FORCE_NO_FPU
		PackFlatOnPlane(pPlaneMem, -1.0f,
						rev_invis, 0xBBFFFBB0UL>>4);
#else
		PackFlatOnPlane(pPlaneMem, -128,
						rev_invis, 0xBBFFFBB0UL>>4);
#endif

   		pPlaneMem+=WORDS_PER_PLANE;
	   	PVRParamBuffs[PVR_PARAM_TYPE_ISP].uBufferPos +=WORDS_PER_PLANE;

   	}/*end for forward visibles*/
}
#endif


/**************************************************************************
 * Function Name  : PackCompleteShadow
 * Inputs         : NONE
 * Outputs        : NONE
 * Input/Output	  :
 * Returns        :
 * Global Used    :
					
 * Description    : Puts in an shadow volume that shadows EVERYTHING. This
 *					is needed so light volumes can work.
 **************************************************************************/

void PackCompleteShadow()
{
	sgl_uint32 * pPlaneMem;
	const int PlanesNeeded = 2;

	if ((PVRParamBuffs[PVR_PARAM_TYPE_ISP].uBufferPos+(PlanesNeeded*WORDS_PER_PLANE))>PVRParamBuffs[PVR_PARAM_TYPE_ISP].uBufferLimit)
	{
		/* this isnt enough space */
		DPFDEV ((DBG_FATAL, "PackCompleteShadow: Out of sabre memory for shadow!"));
		DPF ((DBG_FATAL, "This shouldnt happen!"));
		ASSERT(FALSE);
		return ;
	}
	/*
	// Get a pointer to it
	*/
   	pPlaneMem = &PVRParamBuffs[PVR_PARAM_TYPE_ISP].pBuffer[PVRParamBuffs[PVR_PARAM_TYPE_ISP].uBufferPos];


	/*
	// Put in the Forward plane
	*/
#if (PCX2 || PCX2_003) && !FORCE_NO_FPU
	PackFlatOnPlane(pPlaneMem, RnGlobalGetFixedClipDist(),
					test_shad_forw_fp, 0xBBFFFAB0UL>>4);
#else
	PackFlatOnPlane(pPlaneMem, RnGlobalGetFixedClipDist(),
					test_shad_forw_fp, 0xBBFFFAB0UL>>4);
#endif

  	pPlaneMem+=WORDS_PER_PLANE;
	PVRParamBuffs[PVR_PARAM_TYPE_ISP].uBufferPos +=WORDS_PER_PLANE;

	/*
	// Put in the Reverse plane
	*/
#if (PCX2 || PCX2_003) && !FORCE_NO_FPU
	PackFlatOnPlane(pPlaneMem, 0.0f,
					test_shadow_rev, 0xBBFFFBB0UL>>4);
#else
	PackFlatOnPlane(pPlaneMem, 0,
					test_shadow_rev, 0xBBFFFBB0UL>>4);
#endif

   	PVRParamBuffs[PVR_PARAM_TYPE_ISP].uBufferPos +=WORDS_PER_PLANE;

}


/**************************************************************************
 * Function Name  : PackTranslucentBeginDummy
 * Inputs         : NONE
 * Outputs        : NONE
 * Input/Output	  :
 * Returns        :
 * Global Used    :
					
 * Description    : Puts a dummy translucent object into the sabre parameter space.
 *					Used to indicate the start of a translucent pass.
 **************************************************************************/

void PackTranslucentBeginDummy(sgl_uint32 u32TSPTag)
{
	sgl_uint32 * pPlaneMem;
	int Plane;

	if ((PVRParamBuffs[PVR_PARAM_TYPE_ISP].uBufferPos + (NUM_TRANS_PASS_START_PLANES * WORDS_PER_PLANE))>PVRParamBuffs[PVR_PARAM_TYPE_ISP].uBufferLimit)
	{
		/* this isnt enough space */
		DPFDEV ((DBG_FATAL, "Out of sabre memory for translucent dummy!"));
	
		DPF ((DBG_FATAL, "This shouldnt happen!"));
		ASSERT(FALSE);
		return ;
	}
	/*
	// Get a pointer to it
	*/
   	pPlaneMem = &PVRParamBuffs[PVR_PARAM_TYPE_ISP].pBuffer[PVRParamBuffs[PVR_PARAM_TYPE_ISP].uBufferPos];

#if (PCX2 || PCX2_003) && !FORCE_NO_FPU
	PackFlatOnPlane(pPlaneMem, -1.0f,
					begin_trans, u32TSPTag);
#else
	PackFlatOnPlane(pPlaneMem, -128,
					begin_trans, 0xAAFFFAA0UL>>4);
#endif

  	pPlaneMem+=WORDS_PER_PLANE;
	PVRParamBuffs[PVR_PARAM_TYPE_ISP].uBufferPos +=WORDS_PER_PLANE;

   	for (Plane=0; Plane<NUM_TRANS_PASS_START_PLANES-1; Plane++)			
   	{
#if (PCX2 || PCX2_003) && !FORCE_NO_FPU
		PackFlatOnPlane(pPlaneMem, -1.0f,
						rev_invis, u32TSPTag);
#else
		PackFlatOnPlane(pPlaneMem, -128,
						rev_invis, u32TSPTag);
#endif

   		pPlaneMem+=WORDS_PER_PLANE;
	   	PVRParamBuffs[PVR_PARAM_TYPE_ISP].uBufferPos +=WORDS_PER_PLANE;

   	}/*end for forward visibles*/

	/*
		place in a transparent background plane for
		trans/shadow/smoothshading fix
	*/
#if PCX1 || ISPTSP
	PackFlatOnPlane (pPlaneMem, 0x7FFFFFFF, forw_visib_fp, u32TSPTag);

	pPlaneMem+=WORDS_PER_PLANE;
   	PVRParamBuffs[PVR_PARAM_TYPE_ISP].uBufferPos +=WORDS_PER_PLANE;
#endif

}

/**************************************************************************
 * Function Name  : PackTranslucentDummy
 * Inputs         : NONE
 * Outputs        : NONE
 * Input/Output	  :
 * Returns        :
 * Global Used    :
					
 * Description    : Puts a dummy translucent object into the sabre parameter space.
 **************************************************************************/

void PackTranslucentDummy(sgl_uint32 u32TSPTag)
{
	sgl_uint32 * pPlaneMem;
	int Plane;

	if ((PVRParamBuffs[PVR_PARAM_TYPE_ISP].uBufferPos + (NUM_TRANS_PASS_START_PLANES * WORDS_PER_PLANE))>PVRParamBuffs[PVR_PARAM_TYPE_ISP].uBufferLimit)
	{
		/* this isnt enough space */
		DPFDEV ((DBG_FATAL, "Out of sabre memory for translucent dummy!"));
		
		DPF ((DBG_FATAL, "This shouldnt happen!"));
		ASSERT(FALSE);
		return ;
	}
	/*
	// Get a pointer to it
	*/
   	pPlaneMem = &PVRParamBuffs[PVR_PARAM_TYPE_ISP].pBuffer[PVRParamBuffs[PVR_PARAM_TYPE_ISP].uBufferPos];

   	for (Plane=0; Plane<NUM_DUMMY_OBJECT_PLANES; Plane++)			
   	{
#if (PCX2 || PCX2_003) && !FORCE_NO_FPU
		PackFlatOnPlane(pPlaneMem, -1.0f,
						rev_invis, u32TSPTag);
#else
		PackFlatOnPlane(pPlaneMem, -128,
						rev_invis, u32TSPTag);
#endif

   		pPlaneMem+=WORDS_PER_PLANE;
	   	PVRParamBuffs[PVR_PARAM_TYPE_ISP].uBufferPos +=WORDS_PER_PLANE;

   	}/*end for forward visibles*/

}


/**************************************************************************
 * Function Name  : AddDummyPlanesL (internal only)
 * Inputs         : 
 * Outputs        : 
 * Returns        : 
 * Global Used    : 
 * Description    : Adds the various dummy objects which are needed to
 *					to guarantee minimum plane conditions in the hardware
 *					and to do other clever things...
 *	
 *					Ideally, these should be removed and put into
 *					a "once only" setup bit of code.
 *
 **************************************************************************/

void AddDummyPlanesL(sgl_bool fShadowsOn)
{
	sgl_int32 DummyStart;
	
	/* Add begin translucent dummy plane */ 
	DummyStart = (PVRParamBuffs[PVR_PARAM_TYPE_ISP]).uBufferPos;
	PackTranslucentBeginDummy(PackTexasTransparent (fShadowsOn));
	AddRegionBeginTransDummyL(DummyStart);


	/* Add NUM_DUMMY_OBJECT_PLANES plane translucent dummy objects to plane data */ 
	DummyStart = PVRParamBuffs[PVR_PARAM_TYPE_ISP].uBufferPos;
	PackTranslucentDummy(PackTexasTransparent (FALSE));
	AddRegionTransDummyL(DummyStart);

	/* Same with opaque */
	DummyStart = PVRParamBuffs[PVR_PARAM_TYPE_ISP].uBufferPos;
	PackOpaqueDummy();
	AddRegionOpaqueDummyL(DummyStart);

#if VIGNETTE_FIX
	/* Create large dummy object which may be required for the
	 * vignette fix. Contains NUM_DUMMY_OBJECT_PLANES planes.
	 */
	DummyStart = PVRParamBuffs[PVR_PARAM_TYPE_ISP].uBufferPos;
	PackOpaqueDummyLarge();
	AddRegionOpaqueDummyLargeL(DummyStart);
#endif

	/* And again with complete shadow object */
	DummyStart = PVRParamBuffs[PVR_PARAM_TYPE_ISP].uBufferPos;
	PackCompleteShadow();
	AddCompleteShadowDummyL(DummyStart);
}

/*------------------------------ End of File --------------------------------*/

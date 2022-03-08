/**************************************************************************
 * Name         : pvrlims.h
 * Title        : Constants for PVR1/PVR2 instruction format
 * Author       : Simon Fenney
 * Created      : 24th Nov 1995
 *
 * Copyright : 1995-2022 Imagination Technologies (c)
 * License	 : MIT
 *
 * Description  : 
 *
 * Platform     : ANSI
 *
 * Modifications:-
 * $Log: pvrlims.h,v $
 * Revision 1.22  1997/09/15  16:10:29  mjg
 * Added the DAG_TRANS_SORTING define.
 *
 * Revision 1.21  1997/07/14  19:31:10  mjg
 * Added the PCX2_003 compile switch.
 *
 * Revision 1.20  1997/07/11  09:37:32  gdc
 * protected against multiple inclusion
 *
 * Revision 1.19  1997/06/17  10:23:47  mjg
 * Turned off FORCE_NO_FPU.
 *
 * Revision 1.18  1997/06/17  09:23:26  mjg
 * Added the FORCE_NO_FPU option.
 *
 * Revision 1.17  1997/03/03  17:09:30  gdc
 * put PCX1 min plane lim back up to 35 to stop dropped spans on
 * 3d-flip cube and wipeout
 *
 * Revision 1.16  1997/02/20  15:12:39  erf
 * PCX2 change. Need to increase size of begin trans to two.
 * This is to generate an extra plane for the translucent pass
 * which ensures that a translucent pass has at least 4 planes.
 *
 * Revision 1.15  1997/02/20  13:19:42  erf
 * Bad news about vignetting and PCX1. The PCX2 solution doesn't
 * work. More investigation is needed.
 * Reducing the number of planes per pass doesn't seem to have
 * an adverse affect. Leave at 3 for the time being for PCX1.
 *
 * Revision 1.14  1997/02/20  10:55:11  erf
 * PCX1 vignetting fix.
 * 1. Implemented PCX2 vignetting fix for PCX1. See 1.13 message.
 * 2. Reduced minimun plane requirement for PCX1 from 35 to 3.
 *
 * Revision 1.13  1997/02/19  14:53:24  erf
 * PCX2 vignetting fix.
 * 1. Defined VIGNETTE_FIX.
 * 2. Defined IN_PLANE_CACHE_BOUNDARY as the 1024 cache boundary.
 * 3. Defined NUM_DUMMY_OBJECT_PLANES_LARGE as 32.
 *
 * Revision 1.12  1996/12/10  23:32:35  erf
 * Changed compile switches:
 * 1. PVR1 to ISPTSP.
 * 2. PVR2 to PCX1.
 * 3. PVR3 to PCX2.
 *
 * Revision 1.11  1996/11/21  16:15:24  erf
 * Added PCX2 hardware specific definitions using compile switch PVR3.
 *
 * Revision 1.10  1996/11/14  15:43:21  erf
 * Added PCX2 hardware settings using compile switch PVR3.
 *
 * Revision 1.9  1996/10/23  16:13:37  jop
 *  Added debug stuff (ifdefed out) for sm/shad/trans bug
 * y
 *
 * Revision 1.8  1996/09/27  13:24:32  gdc
 * fixed last log message
 *
 * Revision 1.7  1996/09/27  13:21:18  gdc
 * upped min plane number on PVR2 to 35 to avoid span dropping
 * on d3d flip cube
 *
 * Revision 1.6  1996/09/04  11:16:23  sjf
 * Hmm I think we can't count the begin trans plane as part of the
 * first object in PVR1: We still need 8 planes, so I've made it 9
 * allowing for the "begin trans" plane/instruction.
 *
 * Revision 1.5  1996/09/04  10:39:59  sjf
 * Set the Region limit values to be correct (Moved from pmsabre.c)
 * and made one constant more like English :)
 *
 * Revision 1.4  1996/07/10  11:49:51  sm
 * upped pvr2 plane min
 *
 * Revision 1.3  1996/07/08  17:08:20  sm
 * wopped pcx1 dummy object to 8 planes
 *
 * Revision 1.2  1996/06/10  19:29:23  jop
 * fixed defines for dummy objects
 *
 * Revision 1.1  1996/04/30  18:01:50  jop
 * Initial revision
 *
 * Revision 1.1  1995/12/18  18:59:02  sjf
 * Initial revision
 *
 *************************************************************************/

#ifndef __PVRLIMS_H__
#define __PVRLIMS_H__

#if ISPTSP
	/*
	// The Min and maximum number of planes we can handle in a region
	//
	// The min is also used to build a dummy opaque object which we can
	// add when we need to go over this limit.
	//
	// NOTE: We set REGION_PLANE_LIM to Zero if there ISNT a limit.
	*/
	#define REGION_PLANE_MIN (33)
	#define REGION_PLANE_LIM (1022)

	/*
	// Define the number of planes/instructions needed to start a
	// translucent pass. I.E the number im the TransDummy Object
	// PLUS the size of dummy objects used to pad out stuff.
	//
	// MIDAS3s first object must also have a min of OBJECT_DUMMY planes,
	*/
	#define NUM_TRANS_PASS_START_PLANES (9)
	#define NUM_DUMMY_OBJECT_PLANES (8)

	/*
	// Define the number of words (32 bit) per plane data
	*/
	#define WORDS_PER_PLANE (4)

	/*
	// Values used to define the format of the region(or tile) header word.
	//
	// Note: The masks are used to get the correct number of bits - They have 
	// to be used in conjuection with a shift.
	*/
	/* Ysize - Bits 14..5 */ 
	#define REG_YSIZE_MASK  0x3FFL
	#define REG_YSIZE_SHIFT 5

	/* Xsize - Bits 4..0 */ 
	#define REG_XSIZE_MASK  0x1FL

	/* Y position  29..20 */
	#define REG_YPOS_MASK  0x3FFL
	#define REG_YPOS_SHIFT 20

	/* X pos  19..15 */
	#define REG_XPOS_MASK  0x1FL
	#define REG_XPOS_SHIFT 15

	/*
	// Flag Bits for bits 30 and 31 to identify a region
	*/
	#define REG_COMPULSARY_BITS 0x40000000L

	/*
	// object pointer consts
	//
	// When adding objects to region lists, define the "max" num bits allowed
	// for number of planes and address pointers.
	//  NOTE that these MIGHT  only checked in debug versions
	*/
	#define OBJ_ADDRESS_MASK (0x0007FFFFL)
	#define OBJ_PCOUNT_MASK  (0x3FFL)
	#define OBJ_PCOUNT_SHIFT  (19)

	#define OBJ_VERY_LAST_PTR (0x80000000UL)

	#define DAG_TRANS_SORTING 0

#elif PCX1

	/*
	// The Min and maximum number of planes we can handle in a region
	//
	// The min is also used to build a dummy opaque object which we can
	// add when we need to go over this limit.
	*/
	#define REGION_PLANE_MIN (35)
	#define REGION_PLANE_LIM (3730)

	/*
	// Define the number of planes/instructions needed to start a
	// translucent pass. I.E the number im the TransDummy Object
	// PLUS the size of dummy objects used to pad out stuff.
	*/
	#define NUM_TRANS_PASS_START_PLANES (2)
	#define NUM_DUMMY_OBJECT_PLANES		(8)

	/* Define for vignetting fix algorithm. This simply count the number of
	 * translucent passes and if these and the current tile plane count exceed
	 * the 1024 in-cache boundary then the translucent pass must be started out
	 * of cache.
	 */
	#define VIGNETTE_FIX 0

	#if VIGNETTE_FIX
		/* Define dummy object of 32 planes in size. Used to fill opaque pass
		 * to 1024 cache boundary.
		 */
		#define NUM_DUMMY_OBJECT_PLANES_LARGE (32)

		/* This is plane cache size for a tile. If the plane count exceeds
		 * this then the planes taken from out of cache.
		 */
		#define	IN_PLANE_CACHE_BOUNDARY	(1024)
	#endif

	/*
	// Define the number of words (32 bit) per plane data
	*/
	#define WORDS_PER_PLANE (3)

	/*
	// Values used to define the format of the region(or tile) header word.
	//
	// Note: The masks are used to get the correct number of bits - They have 
	// to be used in conjuection with a shift.
	*/
	/* Ysize - Bits 14..5 */ 
	#define REG_YSIZE_MASK  0x3FFL
	#define REG_YSIZE_SHIFT 5

	/* Xsize - Bits 4..0 */ 
	#define REG_XSIZE_MASK  0x1FL

	/* Y position  29..20 */
	#define REG_YPOS_MASK  0x3FFL
	#define REG_YPOS_SHIFT 20

	/* X pos  19..15 */
	#define REG_XPOS_MASK  0x1FL
	#define REG_XPOS_SHIFT 15

	/*
	// Flag Bits for bits 30 and 31 to identify a region
	*/
	#define REG_COMPULSARY_BITS 0x40000000L

	/*
	// object pointer consts
	//
	// When adding objects to region lists, define the "max" num bits allowed
	// for number of planes and address pointers.
	//  NOTE that these MIGHT  only checked in debug versions
	*/
	#define OBJ_ADDRESS_MASK (0x0007FFFFL)
	#define OBJ_PCOUNT_MASK  (0x3FFL)
	#define OBJ_PCOUNT_SHIFT  (19)

	#define OBJ_VERY_LAST_PTR (0x80000000UL)

	#define DAG_TRANS_SORTING 0

#elif PCX2 || PCX2_003

	/*
	// The Min and maximum number of planes we can handle in a region
	//
	// The min is also used to build a dummy opaque object which we can
	// add when we need to go over this limit.
	*/
	#define REGION_PLANE_MIN (3)
	#define REGION_PLANE_LIM (11000)

	/*
	// Define the number of planes/instructions needed to start a
	// translucent pass. I.E the number im the TransDummy Object
	// PLUS the size of dummy objects used to pad out stuff.
	*/
	#define NUM_TRANS_PASS_START_PLANES (2)
	#define NUM_DUMMY_OBJECT_PLANES (8)

	/* Define for vignetting fix algorithm. This simply count the number of
	 * translucent passes and if these and the current tile plane count exceed
	 * the 1024 in-cache boundary then the translucent pass must be started out
	 * of cache.
	 */
	#define VIGNETTE_FIX 1

	#if VIGNETTE_FIX
		/* Define dummy object of 32 planes in size. Used to fill opaque pass
		 * to 1024 cache boundary.
		 */
		#define NUM_DUMMY_OBJECT_PLANES_LARGE (32)

		/* This is plane cache size for a tile. If the plane count exceeds
		 * this then the planes taken from out of cache.
		 */
		#define	IN_PLANE_CACHE_BOUNDARY	(1024)
	#endif

	/*
	// Define the number of words (32 bit) per plane data
	*/
	#define FORCE_NO_FPU 0
	#if FORCE_NO_FPU
		#define WORDS_PER_PLANE (3)
	#else
		#define WORDS_PER_PLANE (4)
	#endif

	/*
	// Values used to define the format of the region(or tile) header word.
	//
	// Note: The masks are used to get the correct number of bits - They have 
	// to be used in conjuection with a shift.
	*/
	/* Ysize - Bits 14..5 */ 
	#define REG_YSIZE_MASK  0x3FFL
	#define REG_YSIZE_SHIFT 5

	/* Xsize - Bits 4..0 */ 
	#define REG_XSIZE_MASK  0x1FL

	/* Y position  29..20 */
	#define REG_YPOS_MASK  0x3FFL
	#define REG_YPOS_SHIFT 20

	/* X pos  19..15 */
	#define REG_XPOS_MASK  0x1FL
	#define REG_XPOS_SHIFT 15

	/*
	// Flag Bits for bits 30 and 31 to identify a region
	*/
	#define REG_COMPULSARY_BITS 0x40000000L

	/*
	// Flag Bit for bits 29 to identify as link list pointer
	*/
	#define LINK_LIST_BIT		0x20000000UL

	/*
	// object pointer consts
	//
	// When adding objects to region lists, define the "max" num bits allowed
	// for number of planes and address pointers.
	//  NOTE that these MIGHT  only checked in debug versions
	*/
	#define OBJ_ADDRESS_MASK (0x0007FFFFL)
	#define OBJ_PCOUNT_MASK  (0x3FFL)
	#define OBJ_PCOUNT_SHIFT  (19)

	#define OBJ_VERY_LAST_PTR (0x80000000UL)

	#define DAG_TRANS_SORTING 1

#else
	COMPILE ERROR!!!!!!!!
#endif

#endif

/*
// End of file
*/

/*****************************************************************************
 *;++
 * Name           : $RCSfile: rnmesh.c,v $
 * Title          : RNMESH.C
 * C Author       : Jim Page
 * Created        : 21/04/95
 *
 * Copyright	: 1995-2022 Imagination Technologies (c)
 * License		: MIT
 *
 * Description	  : Mesh node rendering control  
 *                   
 * Program Type   : C module (.dll)
 *
 * RCS info:
 *
 * $Date: 1997/10/07 13:12:04 $/user/rcs/revfiles/sabre/sgl/RCS/
 * $Revision: 1.214 $
 * $Locker:  $
 * $Log: rnmesh.c,v $
 * Revision 1.214  1997/10/07  13:12:04  mjg
 * Smallest bug fix of all time? Added the character 'r'. The inline
 * version of PackTo20Bit in ProcessEdgesAllVisble (PCX1 only) was altered
 * by the previous compiler to do a reverse subtraction, even though the
 * hand coded line was for a normal subtraction. The new compiler didn't
 * do this and so any SGL mesh that was supposed to be entirely on screen,
 * wasn't.
 *
 * Revision 1.213  1997/09/11  10:24:23  erf
 * Read dithering quality setting.
 *
 * Revision 1.212  1997/09/03  10:30:28  dm
 * asm changed to __asm for new proton compiler
 *
 * Revision 1.211  1997/07/18  13:56:47  gdc
 * PC'ified
 *
 * Revision 1.210  1997/07/14  19:32:28  mjg
 * Added PCX2_003 compile switch.
 *
 * Revision 1.209  1997/06/23  16:15:43  gdc
 * downgraded some warnings to messages and a assert to a warning
 *
 * Revision 1.208  1997/06/17  10:24:55  mjg
 *  Added FORCE_NO_FPU option.
 *
 * Revision 1.207  1997/06/04  11:14:44  gdc
 * removed some compiler warnings
 *
 * Revision 1.206  1997/05/16  10:33:03  gdc
 * downgraded another one
 *
 * Revision 1.205  1997/05/15  21:12:38  gdc
 * downgraded an assert to a warning
 *
 * Revision 1.204  1997/05/15  15:03:35  gdc
 * downgraded a warning to a message
 *
 * Revision 1.203  1997/05/13  17:55:42  mjg
 * Replaced PVROS memory management calls with SGL.
 *
 * Revision 1.202  1997/05/12  12:46:30  mjg
 * Changed an ASSERT to a DBG_WARNING.
 *
 * Revision 1.201  1997/05/02  07:42:44  erf
 * Removed UINT32's etc and replaced with sgl_uint32's etc.
 *
 * Revision 1.200  1997/04/30  19:20:47  jop
 * Put Ed's changes back to how they were
 *
 * Revision 1.199  1997/04/29  14:48:48  erf
 * Added new PVROS API function call mechanism.
 *
 * Revision 1.198  1997/04/25  20:36:20  mjg
 * Removed a few warnings - had problems with the (float const (*)[4]) array
 *
 * Revision 1.197  1997/04/21  20:01:29  mjg
 * Implemented new parameter buffer structures.
 *
 * Revision 1.196  1997/04/21  15:28:56  mjg
 * Replaced ParamBufferInfo with PVRParamBuffs.
 *
 * Revision 1.195  1997/04/10  10:09:11  mjg
 * Fixed an ASSERT replaced a division.
 *
 * Revision 1.194  1997/04/09  10:57:38  gdc
 * removed all references to projection_mat global form this file
 * all values now got via functions
 *
 * Revision 1.193  1997/04/08  14:39:28  gdc
 * local proj mat is now accessed thru functions not as a global
 *
 * Revision 1.192  1997/04/07  13:34:23  gdc
 * added a trans set ID as a funtion parameter rather than a
 * global variable
 *
 * Revision 1.191  1997/04/04  17:41:58  gdc
 * removed some redundant code and made fRegionSclae be got from
 * projection matrix struct
 *
 * Revision 1.190  1997/03/31  22:09:15  jop
 * Sorted out WIN32/DOS32 issues
 *
 * Revision 1.189  1997/03/27  12:36:08  erf
 * Operating system independent memory allocation/deallocation macros.
 * 1. Added #include "pvrosapi.h" to include new macros.
 * 2. Changed malloc, calloc, realloc and free to PVROSMalloc, PVROSCalloc,
 *    PVROSRealloc and PVROSFree.
 *
 * Revision 1.188  1997/03/25  10:24:44  erf
 * When processing meshes read texture filtering quality setting to
 * set the bilinear register on PCX2.
 *
 * Revision 1.187  1997/03/11  12:48:44  sxy
 * Disabled the function call CreatePointData() for texture wraping,
 * the job is done in dlmesh.c.
 *
 * Revision 1.186  1997/03/10  16:26:14  erf
 * Fix assembler in ProcessEdgesAllVisible() which I broke in the doing
 * version 1.185. Sorry !!!
 *
 * Revision 1.185  1997/03/05  17:34:11  erf
 * 1. Removed changes made by SXY in 1.183 to check the C value.
 *    This is already done in pkisp.c.
 * 2. Removed 'current_trans_set_id++' since it was compiled out
 *    anyway.
 *
 * Revision 1.184  1997/02/27  15:33:02  gdc
 * got previous change to compile
 *
 * Revision 1.183  1997/02/27  15:08:26  sxy
 * Added check fC values for PCX2.
 *
 * Revision 1.182  1997/02/27  10:12:56  sxy
 * Added flat non_textured translucent mesh, fixed bug neche-flipsgl.
 *
 * Revision 1.181  1997/02/25  12:47:37  gdc
 * changed mesh non-textured shading to use non-branching texas packing
 * routines, which might get a little speed up.
 *
 * Revision 1.180  1997/02/12  10:21:18  mjg
 * Reverted to a slightly more optimal version of the chage below.
 *
 * Revision 1.179  1997/02/11  18:49:16  mjg
 * Support for negative scaling.
 *
 * Revision 1.178  1997/02/04  10:32:46  gdc
 * Changed defintion of gfRegion[X-Y]Scale in conjunction with change to
 * dvdevice.c of this date
 *
 * Revision 1.177  1997/02/03  10:55:50  gdc
 * removed D3DOPTIM_SGL conditionals as this is now the only option
 *
 * Revision 1.176  1997/01/29  18:17:21  mjg
 * Put in a check to stop solid regions running out of sabre space.
 *
 * Revision 1.175  1997/01/10  19:51:23  erf
 * Fixed PCX2 disappearing planes at edge of screen.
 *
 * Revision 1.174  1996/12/18  18:24:03  erf
 * PCX2 needs to scale A, B & C parameters to prevent overflow.
 * ie 32 bit to 20 bit conversion overflow prevention.
 * Forget what I said in Rev 1.167.
 *
 * Revision 1.173  1996/12/11  16:47:15  erf
 * Changed #if defined(ICL) to #if ICL.
 *
 * Revision 1.172  1996/12/10  23:17:38  erf
 * Changed compile switches:
 * 1. PVR1 to ISPTSP.
 * 2. PVR2 to PCX1.
 * 3. PVR3 to PCX2.
 *
 * Revision 1.171  1996/12/03  17:00:19  erf
 * Using D3DOPTIM_SGL compile switch for D3D potimisation with SGL do:
 * 1. No need to correct current_trans_set_id with translucent planes.
 *
 * Revision 1.170  1996/12/02  13:03:13  erf
 * D3D optimisation enabled for SGL using D3DOPTIM_SGL compile switch.
 *
 * Revision 1.169  1996/11/30  00:14:10  erf
 * In D3DOPTIM for translucency do:
 * 1. Ensure current_trans_set_id multiple of 2 and > 1.
 * 2. Depths are passed as are. ie remove negate.
 *
 * Revision 1.168  1996/11/29  12:25:59  erf
 * Using D3DOPTIM compile switch for optimisation do:
 * 1. Remove division of RegSize in the calculation of gfRegionYScale.
 *    Absolute Y co-ordinates are calculated.
 * 2. Use FirstYRegionExact and LastYRegionExact to calculate absolute Y co-ordinates.
 * 3. Use new routines AddRegionSolid() and AddRegionSeeThru() to add objects.
 * 4. Change ASSERTs to check absolute Y co-ordinates.
 *
 * Revision 1.167  1996/11/14  15:59:45  erf
 * Use PVR3 compile switch for PCX2 to:
 * 1. Don't scale A, B & C parameters for perpendicular edges.
 * 2. No need to scale A, B & C parameters to prevent overflow.
 *
 * Revision 1.166  1996/11/08  15:43:49  gdc
 * changed size of malloc for setting up gpXEdges to be correct size
 * previously was bigger than required
 *
 * Revision 1.165  1996/11/07  12:10:47  erf
 * Added PVR3 compile switch for PCX2 to:
 * 1. Change n32A, B & C to f32A, B & C.
 * 2. Remove need for FLOAT_TO_FIXED.
 *
 * Revision 1.164  1996/11/07  09:52:22  erf
 * Added PVR3 compile switch to:
 * 1.Remove calls to PackTo20Bit() since not needed in PCX2.
 *
 * Revision 1.163  1996/11/06  15:03:46  gdc
 * moved a malloc from MeshAtListTrav to MaeshOnSglInit to avoid potential
 * memory leak if many devices are created (and possibly destroyed)
 *
 * Revision 1.162  1996/11/01  15:15:09  gdc
 * changed z-depth for translucent pass sorting to be face average rather
 * than a single vertex
 *
 * Revision 1.161  1996/10/30  15:41:06  gdc
 * hopefully fixed the NEC mesh plate clip rejection - vertices infront
 * of the front clipping plane had their Y-direction outsideness wrong
 * way round
 *
 * Revision 1.160  1996/10/29  18:20:16  gdc
 * did a couple of changes to make n32C value calculation more consistent
 * and removed a multiply in the process
 *
 * Revision 1.159  1996/10/11  12:08:19  msh
 * "Small number" checks modified for Midas Arcade
 *
 * Revision 1.158  1996/10/11  11:52:42  mal
 * Added some more metric macros.
 *
 * Revision 1.157  1996/10/10  11:11:55  mal
 * Modified some of the metric macros.
 *
 * Revision 1.156  1996/10/04  17:13:26  mal
 * Added some metric macros.
 *
 * Revision 1.155  1996/10/03  16:05:41  ncj
 * Fixed macro reference
 *
 * Revision 1.154  1996/10/03  12:42:57  mal
 * Added some metric macros for profiling code.
 *
 * Revision 1.153  1996/10/01  17:41:24  mjg
 * Moved stuff that was done at the start to being done at the beginning
 * of each display list traversal - helps BestFitRegions.
 *
 * Revision 1.152  1996/09/12  15:09:08  mal
 * Modified SGL_MAX_PLANES limit to SGL_MAX_PLANES.
 * Shadows may now have more than SGL_MAX_PLANES.
 * Set to twice SGL_MAX_PLANES value. re: NHE 044a, 049A.
 *
 * Revision 1.151  1996/09/04  14:35:52  sjf
 * "improved" sorting of translucent passes with large meshes by
 * basing ordering on individual faces, rather than the overal
 * bounding box.
 *
 * Revision 1.150  1996/09/02  14:34:29  sjf
 * Hmm Just realised that the small object rejection would work
 * better for Quads if I used the diagonal AC instead of the eged
 * AB. This change won't really affect Triangles.
 *
 * Revision 1.149  1996/08/30  15:49:10  sjf
 * Added in small object rejection for PVR1.
 *
 * Revision 1.148  1996/08/29  13:12:17  sjf
 * Added new param to AddRegionTranslucent.
 *
 * Revision 1.147  1996/08/28  10:19:04  sjf
 * Have redone the smooth shading using a much better method: this merely
 * comes up with new pseudo positions for clipped vertices. There is still
 * a minor problem with the way it chooses how to clip (it currently clips
 * the line between the z clipped vertex and the furthest Z vertex), but this
 * is minor compared to the way it was.
 *
 * Revision 1.146  1996/08/27  14:17:38  sjf
 * Have done a "partial" fix to the smooth shading/quad clipping code
 * so that now, at least, it doesn't flash! Unfortunately the code
 * interpolates the normals, rather than the intensity values, which
 * means that as points are clipped, the shading changes. I'm going to
 * try to fix this in the next version, but it will also require changing
 * the smooth shading code as well.
 *
 * Revision 1.145  1996/08/21  11:55:52  sjf
 * Improved the marking of the clipped flags on vertices. If the vertex was
 * Z clipped, it wasn't bothering to check the clipping VS the other clipping planes.
 * It now does this, which should allow us to eliminate a few more invisible
 * faces + improve the possibility of rejecting whole meshes outright.
 *
 * Revision 1.144  1996/08/20  10:11:22  sjf
 * Added ifdef to comment out/use mesh code that does its own region handling.
 *
 * Revision 1.143  1996/07/24  14:01:45  gdc
 * In processedgesallvisible cast 'cmp' instuct on [esp] resgiter to DWORD PTR
 * for compatability with intel compiler upgrade - no effect on previous compiler version
 *
 * Revision 1.142  1996/07/15  16:52:44  jrc
 * Restored the RCS revision logging.
 *
 * Revision 1.137  1996/07/08  17:31:03  jop
 * Added new smooth shading mode
 *
 * Revision 1.135  1996/07/06  01:42:33  sjf
 * Fixed another (hopefully only remaining) case of non textured
 * translucency, which was causing "cat scratch" render lock up.
 *
 * Revision 1.134  1996/07/05  15:15:13  jop
 * Added a temporary fix for the smooth shading clipping bug
 *
 * Revision 1.133  1996/07/05  07:57:36  sjf
 * 1) Added some debug only code
 * 2) Fixed bug with texture pre-process crashing if the first faces
 *   used the global material.
 *
 * Revision 1.132  1996/06/24  16:21:04  sjf
 * 1) Forced off translucency if for any local material there is
 *    a nontextured face.
 * 2) Fixed a bug from last version which incorrectly chose shdaing
 * umm shading options.
 *
 * Revision 1.131  1996/06/21  14:40:56  jrc
 * Surrounded OutputDebugString calls by #if WIN32.
 *
 * Revision 1.130  1996/06/20  20:45:14  jop
 * (Combined SJF and JOP editing).....
 *  HOPEFULLY ...
 * 1) Fixed the texture wrapping so that it only comes on when S and
 *    O maps are actually defined
 * 2) (Now this may be dubious) turned off smooth shading when translucency
 *    is on. I know there IS a problem with the hardware with PVR1 but I've
 *    also disabled it on PCX1 as well. If this is OK in PCX1 then take the
 *    guard out!.
 *
 * Revision 1.128  1996/06/06  23:06:03  sjf
 * Added material instancing,.
 *
 * Revision 1.127  1996/06/05  12:24:04  sjf
 * Found a very rare bug where if a horizontal (or vertical) edge lay JUST
 * above (or to the left) of a tile, a one pixel gap would appear between
 * abutting faces. This seems to occur because the ABC parameters for the edge
 * DRAW the edge in the NEXT tile (due to slight error/rounding differences).
 * .. but the vertices "tile" number get TRUNCATED into the top (or left) region
 * This causes the break. What I've done is added a small "slop" value which should
 * push the region calculation into the next region. There IS a VERY small chance that
 * it may get pushed too far and we'll get the same effect in reverse, but I haven't
 * seen this happen.
 *
 * Revision 1.126  1996/06/04  16:12:43  sjf
 * In texture caching, forgot to check the quality flags. Fixed now.
 *
 * Revision 1.125  1996/05/30  20:32:49  sjf
 *  Added texture caching.
 *
 * Revision 1.124  1996/05/23  16:53:16  jop
 * Some stuff to make the b4 compiler happy(ier)
 *
 * Revision 1.123  1996/05/20  19:30:29  jop
 * Fixed MechWarrior texture bug - a naughty ! where there shouldn't
 * have been
 *
 * Revision 1.122  1996/05/17  18:57:06  jop
 * edge asm code now checks for no edges in list
 *
 * Revision 1.121  1996/05/15  12:14:31  sjf
 * Hopefully, (I not confirmed) fixed the problem with incorrect
 * Z values on near-perpendicular mesh faces.
 *
 * Revision 1.120  1996/05/11  18:45:07  sjf
 * Hopefully fixed some rather nasty bugs when there is a mixture of shading etc
 * types - the flat shading was wrong!
 *
 * Revision 1.119  1996/05/10  20:51:05  jop
 * Fixed bug causing the wrong asm code to be built for pvr1
 *
 * Revision 1.118  1996/05/09  17:33:10  jop
 * Fixed PVR2 negation in edge calc
 *
 * Revision 1.117  1996/05/08  12:36:23  sjf
 * Fixed a problem if an empty mesh is given to the process routine. It now
 * aborts early, rather than crashing :)
 *
 * Revision 1.116  1996/05/03  16:27:56  sjf
 * Got rid of unneeded parameter in a pack routine.
 *
 * Revision 1.115  1996/05/02  12:34:12  jop
 * Fixed plane size bug, and Edges asm
 *
 * Revision 1.114  1996/05/01  14:01:26  sjf
 * 1) Eliminated texel sampling
 * 2) Fixed bug I introduced when deleting unused local materials - it would
 *    sometimes have a local material affect the global settings. Not a bad bug
 *    but nice to have fixed.
 *
 * Revision 1.113  1996/05/01  12:02:22  jrc
 * Removed some static variables that were changed but not used.
 *
 * Revision 1.112  1996/04/30  20:34:17  sjf
 * Fixed really horrible bug with tranlucent meshes. Basically
 * it would fill ISP parameter space with a load of crap when
 * it encountered one.
 *
 * Revision 1.111  1996/04/30  18:02:29  jop
 * Removed some excess includes
 *
 * Revision 1.110  1996/04/29  15:07:16  sjf
 * Fixed a bug which could cause part of a mesh, which ahd several
 * local materials, to disappear.
 *
 * Revision 1.109  1996/04/26  14:47:52  jop
 * double barrel p20bit now removed temporarily
 *
 * Revision 1.108  1996/04/25  14:33:13  jrc
 * Oops, now changed the previous log message to avoid nested comments in the
 * header!
 *
 * Revision 1.107  1996/04/25  14:28:08  jrc
 * Changed a // comment to a C style comment on a #if 0 line.  This is
 * required for the dependency file creation by the unix makefile.
 *
 * Revision 1.106  1996/04/25  13:18:45  jop
 * Put double barelled 20bit routine into edge code
 *
 * Revision 1.105  1996/04/25  11:19:59  jop
 * Added assembly fot 20 bit packing
 *
 * Revision 1.104  1996/04/25  09:58:25  jop
 * ProcessEdgesAllVisible now in assembler for ICL builds
 *
 * Revision 1.103  1996/04/24  18:46:16  sjf
 * 1) Used a local variable to store intermediate flag values in the process partly vis
 * verts routine
 * 2) Used some local variables in the transform of normal in face processing, so it
 *    should allow better FP overlap with Intels ICL compiler.
 *
 * Revision 1.102  1996/04/24  13:49:31  sjf
 * To get better overlap of FP instructions (well with the ICL compiler) added
 * in some temporary variables, which convinces the compiler that source and
 * destination dont overlap in memory.
 *
 * Revision 1.101  1996/04/24  01:04:54  jop
 * Fixed bug where even if PackmeshParams ran out of space, entries
 * would still be placed in region data for the planes that didn't
 * exist. This could be tidied, at the moment the entirety of
 * what is given to DoplaneStuff is rejected - could work out
 * exactly when to stop....? Yeah, maybe not.
 *
 * Revision 1.100  1996/04/23  18:58:02  sjf
 * 1) Tidied up some long lines
 * 2) Made sure when "pre_cull" was off, it didnt allocate any extra mem
 * 3) Allowed use of "int" compares to do FP sign tests when on Pentiac.
 * 4) Hopefully sped up the test for texture wrapping by moving it out
 *    of loops.
 *
 * Revision 1.99  1996/04/23  16:51:29  sjf
 * dlmesh now deletes "unused" postions of anonymous meshes. Had to access
 * some alternative fields to cope with this.
 *
 * Revision 1.98  1996/04/22  16:10:57  sjf
 *  added call to non textured pack routine
 *
 * Revision 1.97  1996/04/20  16:46:19  sjf
 *  Added a smooth textured packing routine.
 *
 * Revision 1.96  1996/04/20  16:17:31  jop
 * Refined edge and vertex structures further
 *
 * Revision 1.95  1996/04/20  13:17:53  sjf
 * Got rid of redundant f1OverZ from point structure.
 *
 * Revision 1.94  1996/04/19  19:54:29  sjf
 *  Added a call to the specialised flat textured pack routine.
 *
 * Revision 1.93  1996/04/19  16:53:45  jop
 * Added asm pre-cull preprocessor
 *
 * Revision 1.92  1996/04/18  14:05:13  jop
 * Added pre-culling mode
 *
 * Revision 1.91  1996/04/18  12:52:31  sjf
 * Some more optimisations (I think I added the "fast" edge calculation method
 * to the "partly visible" edge routine, when it detected that the edge was, in fact,
 * completely visible.
 * 2) Also tried to get integer/FPDiv overlap by rearanging a bit of code. This
 * should speed up pentium performance, and shouldn't harm other, better, processors.
 *
 * Revision 1.90  1996/04/17  15:47:52  sjf
 * Re-arranged the order of rejection of offscreen meshes + allowed
 * the partly visible vertex processing to report if the mesh
 * is actually completely onscreen (or competely offscreen).
 *
 * Revision 1.89  1996/04/16  19:09:03  sjf
 *  1) Removed Vertex flags copy from original data (no longer needed)
 *  2) Added new Wizzo! (tm) edge calculation for on screen edges.
 *
 * Revision 1.88  1996/04/16  16:35:38  jop
 * Added out clamping bug in dodgy rep point code
 *
 * Revision 1.87  1996/04/12  15:22:47  jop
 * Fixed special case smooth shading rep point bugs
 *
 * Revision 1.86  1996/04/11  19:00:33  jop
 * Sorted out // comment
 *
 * Revision 1.85  1996/04/11  14:20:16  jop
 * Added points data array to mesh structure
 *
 * Revision 1.84  1996/04/04  18:16:16  jop
 * Working file for beta4
 *
 * Revision 1.83  1996/04/04  12:59:09  jop
 * All of ProcessFacesVisible optionally in asm now
 *
 * Revision 1.82  1996/04/04  11:23:19  sjf
 * Re-put in my code - and removed jims changes to cope with only SGL_MAX_PLANES per
 * material. Basically, this code is now in DLMESH so that we do fewer tests when
 * rendering.
 * 2) fixed a bug that was stopping texture wrapping.
 *
 * Revision 1.81  1996/04/03  18:11:42  jop
 * A lot of assembler, and extended simon's changes
 * for compatability with big meshes (such as nefertiti)
 *
 * Revision 1.80  1996/04/02  17:44:49  sjf
 * Changed compares for culling mode to precalculated values.
 *
 * Revision 1.79  1996/04/02  12:34:49  sjf
 * Optimised like mad in rnmesh
 *
 * Revision 1.78  1996/03/26  17:01:53  jop
 * More optimising
 *
 * Revision 1.77  1996/03/25  19:36:25  sjf
 * Changed region Min & Maxs to Region Rect structure
 * 2) Changed to use standard bounding box struct.
 *
 * Revision 1.76  1996/03/25  15:07:28  jop
 * Optimisations
 *
 * Revision 1.75  1996/03/19  16:47:26  jop
 * Sorted out double slash error
 *
 * Revision 1.74  1996/03/05  16:14:54  jop
 * added tranlucency back again
 *
 * Revision 1.73  1996/03/05  13:30:02  jop
 * Fixed edge bug in new code.
 *
 * Revision 1.71  1996/02/12  23:03:04  jop
 * Decluttered partly visible mesh processing
 *
 * Revision 1.70  1996/02/09  16:32:42  jop
 * Went to new list method
 *
 * Revision 1.69  1996/01/31  14:25:48  jop
 * Changed shading/points structs
 *
 * Revision 1.68  1995/12/18  18:54:58  sjf
 * disabled REPORT_FRAME_RATE ..
 *
 * Revision 1.67  1995/12/18  18:20:02  jop
 * Added some stuff which noone wants
 *
 * Revision 1.66  1995/11/28  11:06:52  pdl
 * removed two messages.
 *
 * Revision 1.65  1995/11/15  14:31:58  jop
 * Added a performance analyser
 *
 * Revision 1.64  1995/11/09  18:22:12  jop
 * new structures adopted
 *
 * Revision 1.63  1995/11/05  15:27:10  sjf
 * Rewrote the code that handles the mesh region packing - seems a lot faster
 * now (about 2 fps!!).
 *
 * Revision 1.62  1995/11/02  20:54:54  jop
 * Fixed dropout bug (INT8s wrapping round)
 *
 * Revision 1.61  1995/11/02  19:50:04  sjf
 * 1) Added user defineable culling on an object by object basis
 * 2) Added fog as a quality setting.
 *
 * Revision 1.60  1995/11/02  14:31:52  jop
 * Fixed typo
 *
 * Revision 1.59  1995/11/02  14:23:39  jop
 * Clamping replaced on visible faces
 *
 * Revision 1.58  1995/11/02  12:39:35  jop
 * Fixed texturing rep point algorithm
 *
 * Revision 1.57  1995/11/01  19:32:10  jop
 * Added to get rid of extra mesh bits
 *
 * Revision 1.56  1995/10/31  16:26:03  jop
 * Added back (probably unnecessarily) perpendicular planes and
 * fixed the nefertiti disappearing bug (back & front face culling??)
 *
 * Revision 1.55  1995/10/30  16:56:55  jop
 * Back face culling enabled
 *
 * Revision 1.54  1995/10/29  12:01:55  jop
 * Fixed bug in edge calculation - edges with 1 vertex behind the camera
 * weren't calculated properly - the vertex's screen space coords were
 * random
 *
 * Revision 1.53  1995/10/28  23:17:46  jop
 * Downgraded a DPF to VERBOSE
 *
 * Revision 1.52  1995/10/27  19:16:48  jop
 * Killed a few namco course bugs - going outside region arrays
 *
 * Revision 1.51  1995/10/27  17:39:00  jop
 * Now throws away perp planes
 *
 * Revision 1.50  1995/10/27  14:25:04  jop
 * Made region sizes non-rnmesh dependant!
 *
 * Revision 1.49  1995/10/27  13:17:43  pdl
 * Fixed batch shading (pf_reversed) bug
 *
 * Revision 1.48  1995/10/26  16:52:05  jop
 * Ifdef'd vsgl.h
 *
 * Revision 1.47  1995/10/26  15:54:24  jop
 * RnGetRegionsFromBBox now used to reject offscreen faces. Cut&Paste
 * bug removed from texture wrapping code.
 *
 * Revision 1.46  1995/10/22  18:39:34  sjf
 * Removed simulator window stuff.
 *
 * Revision 1.45  1995/10/20  20:08:03  jop
 * killed spurious include
 *
 * Revision 1.44  1995/10/20  20:03:44  jop
 * Added simulator window
 *
 * Revision 1.43  1995/10/18  13:50:18  jop
 * Fixed region bbox flushing problem
 *
 * Revision 1.42  1995/10/17  12:41:25  jrc
 * Now passing rectangle of regions as a struct to RnDetermineRegionsFromBBox.
 *
 * Revision 1.41  1995/10/13  12:20:34  sjf
 * There is a bug in Put Planes In regions (or there abouts) which makes it fail
 * when it runs out of Mesh Region Blocks. a fix, I just increased the
 * number and size of the blocks, but this is not a good solution (i.e. it may
 * re-appear). It would be better to fix the whole way the system worked.
 *
 * Revision 1.40  1995/10/11  10:29:24  pdl
 * corrected a cut'n'paste bug in the call to texas packer.
 *
 * Revision 1.39  1995/10/08  17:27:23  jop
 * Improved region bbox calculations and unprojectable rep points
 * now properly detected. CAVEAT - texturing may go weird if all vertices
 * are offscreen bur polygon is visible
 *
 * Revision 1.38  1995/10/06  18:08:45  jop
 * Better bounding box rejection
 *
 * Revision 1.37  1995/10/06  16:07:04  jop
 * Fixed typos
 *
 * Revision 1.36  1995/10/06  16:00:31  jop
 * Installed new texture scaling
 *
 * Revision 1.35  1995/10/06  14:19:47  jop
 * Divided up RnProcessMeshNode and adjust region optimiser to pack
 * reverse regions befor forward regions
 *
 * Revision 1.34  1995/10/05  14:11:00  jop
 * Fixed non-portable code in region optimiser
 *
 * Revision 1.33  1995/10/04  18:53:57  jop
 * Removed old regio-ising code
 *
 * Revision 1.32  1995/10/04  10:42:19  jop
 * Added support for region sorted sabre data
 *
 * Revision 1.31  1995/10/02  13:44:26  jop
 * Improved bounding box calculation (it works now) and fiddled with
 * Wireframe mode stuff
 *
 * Revision 1.30  1995/09/28  10:29:58  pdl
 * RnProcessMaterialNode has changed.
 *
 * Revision 1.29  1995/09/26  08:50:59  sjf
 * 1) This version was previously checked in by JOP but accidently
 *    got full of DOS ^M chars... I obsoleted that version and
 *    have rechecked it in in Unix format. (it wouldn't compile
 *    under Linux gcc)
 * 2) The original changes were the removal of dud parameters from
 *    the flat shading routines.
 *
 * 
 * Revision 1.28  1995/09/13  18:41:20  jop
 * Removed buggy highlight variable from DoPlaneStuff
 *
 * Revision 1.27  1995/09/13  18:25:51  jop
 * Changed to different smooth shading call
 * ,
 *
 * Revision 1.26  1995/09/12  18:39:53  jop
 * Several optimisations
 *
 * Revision 1.25  1995/09/05  12:07:38  jop
 * Tidied up sabre region stuff
 *
 * Revision 1.21  1995/08/24  17:23:46  jop
 * removed export keyword
 *
 * Revision 1.19  1995/08/24  15:35:36  jop
 * Implemented texture wrapping
 * ,
 *
 * Revision 1.18  1995/08/23  15:56:25  jop
 * pf_reversed flag now used to indicate flipped polygons. Also VecNegate used for
 * negating vectors.
 *
 * Revision 1.17  1995/08/21  21:59:47  jop
 * Fixed various bugs
 *
 * Revision 1.11  1995/08/04  17:17:47  jop
 * Added ASSERT to warn if polygon's normal is 0,0,0#
 *
 * Revision 1.10  1995/08/03  16:01:26  jop
 * Changed to Stel's dual shading commands
 *
 * Revision 1.8  1995/07/27  18:43:08  jop
 * Added extra params 
 *
 * Revision 1.7  1995/07/27  13:14:44  jop
 * Didi it properly this time
 *
 * Revision 1.6  1995/07/27  13:12:29  jop
 * removed some non-UNIX bugs
 *
 * Revision 1.5  1995/07/27  12:59:47  jop
 * put test routine in ifdef WIN32 thingys
 *
 * Revision 1.4  1995/07/27  11:01:15  jop
 * Added more stuff
 *
 * Revision 1.2  1995/07/25  16:40:01  jop
 * Loads of new stuff
 *
 * Revision 1.1  1995/07/23  17:19:23  jop
 * Initial revision
 *
 *
 *;--
 *****************************************************************************/

/*
// define Debug Module IDs
*/
#include <math.h>
#include <stdio.h>

#define MODULE_ID	MODID_RNMESH

#include "sgl.h"
#include "sgl_defs.h"
#include "sgl_math.h"

#include "dlnodes.h" 

#include "rnglobal.h"
#include "rnstate.h"
#include "list.h"
#include "pmsabre.h"
#include "rnconvst.h"
#include "rnfshade.h"
#include "rnmater.h"
#include "rntex.h"
#include "pktsp.h"
#include "pkisp.h"
#include "dvdevice.h"
#include "rncamera.h"
#include "rnmesh.h"
#include "rnshade.h"
#include "rnreject.h"
#include "profile.h"
#include "pvrosapi.h"
#include "sglmem.h"
#include "txmops.h"
#include "rnmater.h"
#include "metrics.h"
#include "parmbuff.h"

/* Needed for optimised object packing routines in dregion.c. */
#include "dregion.h"

#define PRE_CULL	0

SGL_EXTERN_TIME_REF /* if we are timing code !!! */

#define DEBUG_REGIONS		FALSE
/*
// Cull mode for the current mesh
*/
static sgl_bool keepAntiClock;
static sgl_bool keepClockwise;

#if PRE_CULL
static sgl_uint32	CullMode;
#endif


#if DEBUG_REGIONS
	
	static int gnRegionHistogram[32][24];
	static int gnRegionSections;
	
#endif

/*
// These only define the INITIAL Max vertices and edges. They are automatically
// expanded for larger meshes.
*/
#define MAX_XVERTICES	100
#define MAX_XEDGES		300

/*
// These are the transformed vertices and edge, PLUS how big the structures are.
// As stated, they are automatically expanded as required
*/
static	int							gnXVertices = 0;
static	PTRANSVERTEX 				gpXVertices = NULL;
static	int							gnXEdges = 0;
static	PTRANSEDGE	 				gpXEdges = NULL;

#if PRE_CULL
static	sgl_uint8		 				*gpXEdgeIndex = NULL;
static	sgl_uint8		 				*gpXVertexIndex = NULL;
#endif

static	TRANSFORMED_PLANE_STRUCT	*gXPlaneList[SGL_MAX_INTERNAL_PLANES];
static	CONV_POINTS_STRUCT			gPointsData[SGL_MAX_INTERNAL_PLANES];
static	XMESHEXTRA 					gXExtras[SGL_MAX_INTERNAL_PLANES];
static	SMOOTHPARAMS				gSP;

/*
// for translucent mesh handling. Ideally this SHOULD be a local, passed,
// variable, but What the hey.
*/
static	BBOX_MINMAX_STRUCT BBoxMinmax;

/********************************************************************/
/********************************************************************/
/*
// Mesh Region Handling
*/
/********************************************************************/
/********************************************************************/
#define USE_INTERNAL_REGION_HANDLING 0

#if USE_INTERNAL_REGION_HANDLING

#define X_REGIONS	32
#define Y_REGIONS	24
#define PLANES_IN_BLOCK	40

typedef struct
{
	int NumForPlanes;
	int NumRevPlanes;
	sgl_int32 ForPlaneIndices[PLANES_IN_BLOCK];
	sgl_int32 RevPlaneIndices[PLANES_IN_BLOCK];

}MESH_REGION_BLOCK_STRUCT;

typedef struct
{
	int	MinX, MinY, MaxX, MaxY;
} REGIONS_IN_USE;

/*
// Declare the array of Mesh regions
*/
static MESH_REGION_BLOCK_STRUCT	MeshRegion[Y_REGIONS][X_REGIONS];
static REGIONS_IN_USE			RegionsInUse;

#define VERTEX_VISIBLE	0x80000000UL
#define EDGE_VISIBLE	0x80000000UL
#define FACE_VISIBLE	0x80

static void InitRegionBuffers ()
{
	MESH_REGION_BLOCK_STRUCT *pRegion;

	int Count;

	/*
	// get pointer to the first region
	*/
	pRegion = &MeshRegion[0][0];

	/*
	// Step through all the regions (in no particular order)
	// and intialise the number of planes to 0
	*/
	for (Count = X_REGIONS * Y_REGIONS; Count != 0; Count --, pRegion++)
	{
		pRegion->NumForPlanes =0;
		pRegion->NumRevPlanes =0;
	}

	RegionsInUse.MaxX = -1;
	RegionsInUse.MaxY = -1;
	RegionsInUse.MinX = X_REGIONS + 1;
	RegionsInUse.MinY = Y_REGIONS + 1;

}


static int OutputRegionPlanes (TRANSFORMED_PLANE_STRUCT *Planes, XMESHEXTRA *Extras)
{
	int nYr, nXr;
	int YLimit, XLimit; /*so the compiler KNOWS these won't vary*/
	MESH_REGION_BLOCK_STRUCT *pRegion;

	int nTotalPlanes = 0;

	/*
	// Step through the used Mesh Regions
	*/
	YLimit = RegionsInUse.MaxY;
	XLimit = RegionsInUse.MaxX;

	for (nYr = RegionsInUse.MinY; nYr <= YLimit; nYr ++)
	{
		/*
		// Get pointer to the first X region in this bunch of y's
		*/
		pRegion = &MeshRegion[nYr][RegionsInUse.MinX];

		/*
		// Step through the X regions
		*/
		for (nXr = RegionsInUse.MinX; nXr <= XLimit; nXr ++, pRegion ++)
		{
			sgl_uint32 u32StartOfSection;
			int nSabrePlanes;

			/*
			// Do the "reverse" planes first
			*/
			if(pRegion->NumRevPlanes)
			{
				/*
				// Add it to the hardware param buffer etc
				*/
				u32StartOfSection = PVRParamBuffs[PVR_PARAM_TYPE_ISP].uBufferPos;

				nSabrePlanes = PackMeshParamsOrdered (Planes, Extras, 
					 pRegion->RevPlaneIndices, 
					 pRegion->NumRevPlanes);

				AddRegionOpaqueSingle (nXr, nYr, u32StartOfSection, nSabrePlanes);

				nTotalPlanes += nSabrePlanes;

				/*
				// Clear the count out so we don't have to do this later
				*/
				pRegion->NumRevPlanes = 0;
			}/*end some reverse planes*/


			/*
			// Do the forward planes next so they might end up in front
			// of the "reverse" ones when pixels jave the same z depth
			*/
			if(pRegion->NumForPlanes)
			{
				/*
				// Add it to the hardware param buffer etc
				*/
				u32StartOfSection = PVRParamBuffs[PVR_PARAM_TYPE_ISP].uBufferPos;
				nSabrePlanes = PackMeshParamsOrdered (Planes, Extras, 
					 pRegion->ForPlaneIndices, 
					 pRegion->NumForPlanes);

				AddRegionOpaqueSingle (nXr, nYr, 
						  u32StartOfSection, nSabrePlanes);

				nTotalPlanes += nSabrePlanes;

				/*
				// Clear the count out so we don't have to do this later
				*/
				pRegion->NumForPlanes = 0;

			}/*end do forward planes*/

		}/*end for x regions*/
	}/*end for y*/


	/* re-init the in use regions */
	RegionsInUse.MaxX = -1;
	RegionsInUse.MaxY = -1;
	RegionsInUse.MinX = X_REGIONS + 1;
	RegionsInUse.MinY = Y_REGIONS + 1;

	return (nTotalPlanes);
}



static int OutputRegionTranPlanes (TRANSFORMED_PLANE_STRUCT *Planes, XMESHEXTRA *Extras, float NearZ)
{
	int nYr, nXr;
	int YLimit, XLimit; /*so the compiler KNOWS these won't vary*/
	MESH_REGION_BLOCK_STRUCT *pRegion;

	int nTotalPlanes = 0;

	/*
	// Step through the used Mesh Regions
	*/
	YLimit = RegionsInUse.MaxY;
	XLimit = RegionsInUse.MaxX;

	for (nYr = RegionsInUse.MinY; nYr <= YLimit; nYr ++)
	{
		/*
		// Get pointer to the first X region in this bunch of y's
		*/
		pRegion = &MeshRegion[nYr][RegionsInUse.MinX];

		/*
		// Step through the X regions
		*/
		for (nXr = RegionsInUse.MinX; nXr <= XLimit; nXr ++, pRegion ++)
		{
			sgl_uint32 u32StartOfSection;
			int nSabrePlanes;

			/*
			// Do the "reverse" planes first
			*/
			if(pRegion->NumRevPlanes)
			{
				/*
				// Add it to the hardware param buffer etc
				*/
				u32StartOfSection = PVRParamBuffs[PVR_PARAM_TYPE_ISP].uBufferPos;

				nSabrePlanes = PackMeshParamsOrdered (Planes, Extras, 
					 pRegion->RevPlaneIndices, 
					 pRegion->NumRevPlanes);

				AddRegionTranslucentSingle (nXr, nYr, u32StartOfSection, nSabrePlanes,NearZ);

				nTotalPlanes += nSabrePlanes;

				/*
				// Clear the count out so we don't have to do this later
				*/
				pRegion->NumRevPlanes = 0;
			}/*end some reverse planes*/

			/*
			// Do the forward planes next so they might end up in front
			// of the "reverse" ones when pixels jave the same z depth
			*/
			if(pRegion->NumForPlanes)
			{
				/*
				// Add it to the hardware param buffer etc
				*/
				u32StartOfSection = PVRParamBuffs[PVR_PARAM_TYPE_ISP].uBufferPos;
				nSabrePlanes = PackMeshParamsOrdered (Planes, Extras, 
					 pRegion->ForPlaneIndices, 
					 pRegion->NumForPlanes);

				AddRegionTranslucentSingle (nXr, nYr, 
						  u32StartOfSection, nSabrePlanes,NearZ);

				nTotalPlanes += nSabrePlanes;

				/*
				// Clear the count out so we don't have to do this later
				*/
				pRegion->NumForPlanes = 0;

			}/*end do forward planes*/
		}/*end for x regions*/
	}/*end for y*/


	/* re-init the in use regions */
	RegionsInUse.MaxX = -1;
	RegionsInUse.MaxY = -1;
	RegionsInUse.MinX = X_REGIONS + 1;
	RegionsInUse.MinY = Y_REGIONS + 1;

	return (nTotalPlanes);
}

#endif USE_INTERNAL_REGION_HANDLING


int PutPlanesInRegions( TRANSFORMED_PLANE_STRUCT *Planes, 
						XMESHEXTRA *Extras, 
						sgl_uint32 nPlanes)
{
	#if ! USE_INTERNAL_REGION_HANDLING

	sgl_uint32 nTotalPlanes, k;
	PXMESHEXTRA   pExtra = Extras;
	
	nTotalPlanes = PackMeshParams (Planes, Extras, nPlanes);

	if (nTotalPlanes > 0) 
	{

		/* Add objects, while we've still got enough ISP space */
		for (k = nPlanes; (k !=0) && (pExtra->u32SabreIndex < 
			 ((PVRParamBuffs[PVR_PARAM_TYPE_ISP].uBufferPos)-pExtra->nSabrePlanes)); 
			 k--, ++pExtra)
		{ 
			sgl_uint32 XYData;
			/* Formulate region data and type of object. */
			XYData =	ENCODE_OBJXYDATA(	PACKED_TYPE_OPAQUE,
											pExtra->RegionsRect.FirstXRegion,
											pExtra->RegionsRect.FirstYRegion,
											pExtra->RegionsRect.LastXRegion,
											pExtra->RegionsRect.LastYRegion );

			/* Use the add single opaque object function
			 */
			AddRegionSolid( XYData, pExtra->nSabrePlanes, pExtra->u32SabreIndex);
		}
	}

	return (nTotalPlanes);
	
	#else

	sgl_uint32 nPlane, nTotalPlanes = 0;
	PXMESHEXTRA   pExtra;

	ASSERT (nPlanes > 0);

    SGL_TIME_START(PUT_PLANES_REGION_TIME)

	pExtra = gXExtras;
	for (nPlane = 0; nPlane < nPlanes; nPlane++, pExtra++)
	{
		sgl_int32 nXMin = pExtra->n32MinX;
		sgl_int32 nXMax = pExtra->n32MaxX;
		sgl_int32 nYMin = pExtra->n32MinY;
		sgl_int32 nYMax = pExtra->n32MaxY;
		
		/* sanity check ... */
	
		ASSERT ((nXMin >= 0) && (nXMin < X_REGIONS));
		ASSERT ((nXMax >= 0) && (nXMax < X_REGIONS));
		ASSERT ((nYMin >= 0) && (nYMin < Y_REGIONS));
		ASSERT ((nYMax >= 0) && (nYMax < Y_REGIONS));
		
		ASSERT (nXMin <= nXMax);
		ASSERT (nYMin <= nYMax);

		/*//////////////
		// If this poly is puny, then lump it with other
		// tiny faces, otherwise add it explicitly to the
		// parameter lists.
		// NOTE: Use bitwise AND  as this will be cheaper than
		// the additional branch.
		///////////// */
		if((nXMin == nXMax) & (nYMin == nYMax))
		{
			MESH_REGION_BLOCK_STRUCT *pRegion;


			/*
			// Update the bounds on the used regions
			*/
			CHOOSE_MIN (RegionsInUse.MinX, nXMin);
			CHOOSE_MIN (RegionsInUse.MinY, nYMin);
			CHOOSE_MAX (RegionsInUse.MaxX, nXMax);
			CHOOSE_MAX (RegionsInUse.MaxY, nYMax);

			pRegion = &MeshRegion[nYMin][nXMin];

			/*
			// if a reverse polygon plane
			*/
			if (GlobalTransformedPlanes[nPlane].pOriginalData->flags & pf_reversed)
			{
				/*
				// if this region is full, then flush it
				*/
				if(pRegion->NumRevPlanes == PLANES_IN_BLOCK)
				{
					sgl_uint32 u32StartOfSection = PVRParamBuffs[PVR_PARAM_TYPE_ISP].uBufferPos;
					int nSabrePlanes;
					/*
					// Add it to the hardware param buffer etc
					*/
    			    SGL_TIME_SUSPEND(PUT_PLANES_REGION_TIME)
					nSabrePlanes = PackMeshParamsOrdered (Planes, Extras, 
								 pRegion->RevPlaneIndices, 
								 pRegion->NumRevPlanes);
    			    SGL_TIME_RESUME(PUT_PLANES_REGION_TIME)

					AddRegionOpaqueSingle (nXMin, nYMin, 
										  u32StartOfSection, nSabrePlanes);

					nTotalPlanes += nSabrePlanes;

					/*
					// Add the new plane to the start
					*/
					pRegion->RevPlaneIndices[0] = nPlane;
					pRegion->NumRevPlanes = 1;
				}
				/*
				// Else just add it
				*/
				else
				{
					pRegion->RevPlaneIndices[pRegion->NumRevPlanes] = nPlane;
					pRegion->NumRevPlanes ++;
				}
			}
			/*
			// else is a forward one
			*/
			else
			{
				/*
				// if this region is full, then flush it
				*/
				if(pRegion->NumForPlanes == PLANES_IN_BLOCK)
				{
					sgl_uint32 u32StartOfSection = PVRParamBuffs[PVR_PARAM_TYPE_ISP].uBufferPos;
					int nSabrePlanes;
					/*
					// Add it to the hardware param buffer etc
					*/
					u32StartOfSection = PVRParamBuffs[PVR_PARAM_TYPE_ISP].uBufferPos;
    			    SGL_TIME_SUSPEND(PUT_PLANES_REGION_TIME)
					nSabrePlanes = PackMeshParamsOrdered (Planes, Extras, 
								 pRegion->ForPlaneIndices, 
								 pRegion->NumForPlanes);
    			    SGL_TIME_RESUME(PUT_PLANES_REGION_TIME)

					AddRegionOpaqueSingle (nXMin, nYMin, 
										  u32StartOfSection, nSabrePlanes);

					nTotalPlanes += nSabrePlanes;

					/*
					// Add the new plane to the start
					*/
					pRegion->ForPlaneIndices[0] = nPlane;
					pRegion->NumForPlanes = 1;
				}
				/*
				// Else just add it
				*/
				else
				{
					pRegion->ForPlaneIndices[pRegion->NumForPlanes] = nPlane;
					pRegion->NumForPlanes ++;
				}
			}/*end if forward or reverse*/

		}
		/*//////////////
		// Else this face should be added as a complete object
		///////////// */
		else
		{
			sgl_int32 index;
			
			REGIONS_RECT_STRUCT TheRegions;
			sgl_uint32 nSabrePlanes;
			sgl_uint32 u32StartOfSection = PVRParamBuffs[PVR_PARAM_TYPE_ISP].uBufferPos;			

			TheRegions.FirstXRegion = nXMin;
			TheRegions.LastXRegion  = nXMax;
			TheRegions.FirstYRegion = nYMin;
			TheRegions.LastYRegion  = nYMax;


			index = nPlane;
			SGL_TIME_SUSPEND(PUT_PLANES_REGION_TIME)
			nSabrePlanes = PackMeshParamsOrdered (Planes, Extras, 
								 &index,  1);

			AddRegionOpaque(&TheRegions, u32StartOfSection, nSabrePlanes);
			SGL_TIME_RESUME(PUT_PLANES_REGION_TIME)

			nTotalPlanes+= nSabrePlanes;

		}/*end else*/
	}/*end for plane*/

	nTotalPlanes += OutputRegionPlanes (Planes, Extras);

	SGL_TIME_STOP(PUT_PLANES_REGION_TIME)

	return (nTotalPlanes);

	#endif
}

/********************************************************************/

static int PutTranPlanesInRegions( TRANSFORMED_PLANE_STRUCT *Planes, 
								   XMESHEXTRA *Extras, 
								   sgl_uint32 nPlanes,
								   int nCurrTransSetID)
{
	#if ! USE_INTERNAL_REGION_HANDLING

	sgl_uint32 nTotalPlanes, k;
	PXMESHEXTRA   pExtra = Extras;
	
	nTotalPlanes = PackMeshParams (Planes, Extras, nPlanes);

	if (nTotalPlanes > 0)
	{
		sgl_uint32 XYData;
		TRANS_REGION_DEPTHS_STRUCT	Trans;
		const TRANS_REGION_DEPTHS_STRUCT *pTrans;

		for (k = nPlanes; k !=0 ; k--, ++pExtra)
		{
			/* Optimisation uses AddRegionSeeThru() routine.
			 * Pack description for routine.
			 */
			XYData = ENCODE_OBJXYDATA(	PACKED_TYPE_TRANSLUCENT,
										pExtra->RegionsRect.FirstXRegion,
										pExtra->RegionsRect.FirstYRegion,
										pExtra->RegionsRect.LastXRegion,
										pExtra->RegionsRect.LastYRegion);

			/* Use the NearZ direct, allow for inversion
			 */
			Trans.BaseDepth = pExtra->NearZ;
			Trans.DepthDx	= 0.0f;
			Trans.DepthDy	= 0.0f;
			pTrans = (const TRANS_REGION_DEPTHS_STRUCT *) &Trans;

			/* Use the add single translucent object function
			 */
			AddRegionSeeThru( XYData, nCurrTransSetID, 
							  pExtra->nSabrePlanes, pExtra->u32SabreIndex, pTrans);
		}
	}

	return (nTotalPlanes);
	
	#else
	
	sgl_uint32 nPlane, nTotalPlanes = 0;
	PXMESHEXTRA   pExtra;

	ASSERT (nPlanes > 0);

	pExtra = gXExtras;
	for (nPlane = 0; nPlane < nPlanes; nPlane++, pExtra++)
	{
		sgl_int32 nXMin = pExtra->n32MinX;
		sgl_int32 nXMax = pExtra->n32MaxX;
		sgl_int32 nYMin = pExtra->n32MinY;
		sgl_int32 nYMax = pExtra->n32MaxY;
		
		/* sanity check ... */
	
		ASSERT ((nXMin >= 0) && (nXMin < X_REGIONS));
		ASSERT ((nXMax >= 0) && (nXMax < X_REGIONS));
		ASSERT ((nYMin >= 0) && (nYMin < Y_REGIONS));
		ASSERT ((nYMax >= 0) && (nYMax < Y_REGIONS));
		
		ASSERT (nXMin <= nXMax);
		ASSERT (nYMin <= nYMax);


		/*//////////////
		// If this poly is puny, then lump it with other
		// tiny faces, otherwise add it explicitly to the
		// parameter lists.
		// NOTE: Use bitwise AND  as this will be cheaper than
		// the additional branch.
		///////////// */
		if((nXMin == nXMax) & (nYMin == nYMax))
		{
			MESH_REGION_BLOCK_STRUCT *pRegion;


			/*
			// Update the bounds on the used regions
			*/
			CHOOSE_MIN (RegionsInUse.MinX, nXMin);
			CHOOSE_MIN (RegionsInUse.MinY, nYMin);
			CHOOSE_MAX (RegionsInUse.MaxX, nXMax);
			CHOOSE_MAX (RegionsInUse.MaxY, nYMax);

			pRegion = &MeshRegion[nYMin][nXMin];

			/*
			// if a reverse polygon plane
			*/
			if (GlobalTransformedPlanes[nPlane].pOriginalData->flags & pf_reversed)
			{
				/*
				// if this region is full, then flush it
				*/
				if(pRegion->NumRevPlanes == PLANES_IN_BLOCK)
				{
					sgl_uint32 u32StartOfSection = PVRParamBuffs[PVR_PARAM_TYPE_ISP].uBufferPos;
					int nSabrePlanes;
					/*
					// Add it to the hardware param buffer etc
					*/
					nSabrePlanes = PackMeshParamsOrdered (Planes, Extras, 
								 pRegion->RevPlaneIndices, 
								 pRegion->NumRevPlanes);

					AddRegionTranslucentSingle (nXMin, nYMin, 
										  u32StartOfSection, nSabrePlanes,NearZ);

					nTotalPlanes += nSabrePlanes;

					/*
					// Add the new plane to the start
					*/
					pRegion->RevPlaneIndices[0] = nPlane;
					pRegion->NumRevPlanes = 1;
				}
				/*
				// Else just add it
				*/
				else
				{
					pRegion->RevPlaneIndices[pRegion->NumRevPlanes] = nPlane;
					pRegion->NumRevPlanes ++;
				}
			}
			/*
			// else is a forward one
			*/
			else
			{
				/*
				// if this region is full, then flush it
				*/
				if(pRegion->NumForPlanes == PLANES_IN_BLOCK)
				{
					sgl_uint32 u32StartOfSection = PVRParamBuffs[PVR_PARAM_TYPE_ISP].uBufferPos;
					int nSabrePlanes;
					/*
					// Add it to the hardware param buffer etc
					*/
					u32StartOfSection = PVRParamBuffs[PVR_PARAM_TYPE_ISP].uBufferPos;
					nSabrePlanes = PackMeshParamsOrdered (Planes, Extras, 
								 pRegion->ForPlaneIndices, 
								 pRegion->NumForPlanes);

					AddRegionTranslucentSingle (nXMin, nYMin, 
										  u32StartOfSection, nSabrePlanes,NearZ);

					nTotalPlanes += nSabrePlanes;

					/*
					// Add the new plane to the start
					*/
					pRegion->ForPlaneIndices[0] = nPlane;
					pRegion->NumForPlanes = 1;
				}
				/*
				// Else just add it
				*/
				else
				{
					pRegion->ForPlaneIndices[pRegion->NumForPlanes] = nPlane;
					pRegion->NumForPlanes ++;
				}
			}/*end if forward or reverse*/

		}
		/*//////////////
		// Else this face should be added as a complete object
		///////////// */
		else
		{
			sgl_int32 index;
			
			REGIONS_RECT_STRUCT TheRegions;
			sgl_uint32 nSabrePlanes;
			sgl_uint32 u32StartOfSection = PVRParamBuffs[PVR_PARAM_TYPE_ISP].uBufferPos;

			TheRegions.FirstXRegion = nXMin;
			TheRegions.LastXRegion  = nXMax;
			TheRegions.FirstYRegion = nYMin;
			TheRegions.LastYRegion  = nYMax;


			index = nPlane;
			nSabrePlanes = PackMeshParamsOrdered (Planes, Extras, 
								 &index,  1);

			AddRegionTranslucent(&TheRegions, u32StartOfSection, nSabrePlanes,NearZ);

			nTotalPlanes+= nSabrePlanes;

		}/*end else*/
	}/*end for plane*/

	nTotalPlanes += OutputRegionTranPlanes (Planes, Extras, NearZ);

	return (nTotalPlanes);
	#endif
}

/********************************************************************/

void RnMeshOnSglInitialise ()
{
	int k;
																		 
	for (k = 0; k < SGL_MAX_INTERNAL_PLANES; ++k)
	{
		gXPlaneList[k] = &GlobalTransformedPlanes[k];
	}
#if USE_INTERNAL_REGION_HANDLING
	InitRegionBuffers ();
#endif
	
	gnXEdges = SglReadPrivateProfileInt ("Mesh", "MaxEdges", MAX_XEDGES, "sgl.ini");

	gpXEdges = SGLMalloc (sizeof (TRANSEDGE) * gnXEdges);

	ASSERT (gpXEdges);

	gnXVertices = SglReadPrivateProfileInt ("Mesh", "MaxVertices", MAX_XVERTICES,
													 "sgl.ini");
	
	gpXVertices = SGLMalloc (sizeof (TRANSVERTEX) * gnXVertices);

	ASSERT (gpXVertices);

#if PRE_CULL

	gpXVertexIndex = SGLMalloc (sizeof (sgl_uint8) * gnXVertices);
	gpXEdgeIndex = SGLMalloc (sizeof (sgl_uint8) * gnXEdges);

	ASSERT (gpXVertexIndex);
	ASSERT (gpXEdgeIndex);

#endif

}

/**************************************************************************
 * Function Name  : DoPlaneStuff  (LOCAL FUNCTION)
 *	
 **************************************************************************/


static int DoPlaneStuff (const MESH_NODE_STRUCT *pMesh,
						 int nPlanes,
						 MASTER_STATE_STRUCT *pState,
						 MATERIAL_STATE_STRUCT *pMaterial,
						 int nCurrTransSetID)
{
	int	     k;
	sgl_bool bShadowLights;
	sgl_bool bFogOn;
    sgl_bool HasTranslucent;

	int ORedFlags, ANDedFlags, FlagsMask;


	SGL_TIME_START(PLANE_STUFF_TIME)

	bShadowLights = pState->pLightsState->flags & lsf_shadows;

	ORedFlags  = pMesh->ORedPlaneFlags;
	ANDedFlags = pMesh->ANDedPlaneFlags;
	FlagsMask  = ~(0);

	HasTranslucent = (pMaterial->texas_precomp.LowWord & MASK_TRANS);
	/* 
	//check quality state 
	//
	// If there is no smooth shading, then disable it
	*/
#if 0 
/*
//if we have both translucency and smooth shading, turn the smooth
//shading off instead of the translucency 
*/
	if (!(pState->pQualityState->flags & qf_smooth_shading)||
	      HasTranslucent)
#else
	if (!(pState->pQualityState->flags & qf_smooth_shading))
#endif
	{
		/* 
		// switch off smooth shading 
		*/
		ORedFlags  &= ~pf_smooth_shad;
		ANDedFlags &= ~pf_smooth_shad;

		/*
		// This is only needed when we are doing a mixture of shading types
		*/
		FlagsMask  &= ~pf_smooth_shad;
	}


	/*
	// if texturing is turned off OR there is no texture in the current material
	*/
	if (!(pState->pQualityState->flags & qf_textures) || 
		!(pMaterial->texture_flags & MASK_TEXTURE))
	{
		/*
		// switch off texturing
		*/
		ORedFlags  &= ~pf_textured;
		ANDedFlags &= ~pf_textured;

		/*
		// This is only needed when we are doing a mixture of shading types
		*/
		FlagsMask  &= ~pf_textured;
		
		/*HasTranslucent = FALSE;*/
	}
	/*
	// Else we are either texture wrapping or direct texturing, so set up
	// the texture projection matrix accordingly
	*/
	else 		
	{
		int ThisTextureSize = TexasGetTextureDimension (&pMaterial->texas_precomp);

		/*
		// Do we need change the texture projection bit of the
		// local projection matrix for a different sized texture?
		*/
		if(!RnGlobalLocalProjMatTextureIsValid(ThisTextureSize))
		{
			RnRescaleLocalProjectionTexture(ThisTextureSize, 
											pState->pTransformState);
		}

		/*
		// Since we have texturing, make sure we turn it on
		*/
		ORedFlags  |= pf_textured;
		ANDedFlags |= pf_textured;
		
		/*HasTranslucent = (pMaterial->texas_precomp.LowWord & MASK_TRANS);*/

	}/*if textured */

#if 0  /*fix by simon, shoes by cardin, hair by ...*/
if(HasTranslucent)
{
	return;
}
#endif

	/*
	// OK Smooth shading and translucency is OUT! on Midas3, possibly PCX1 and 
	// possibly PCX2 as well.
	// (we'll need to check the latter case)
	*/

	/* And, I'm guessing PCX2_003 doesn't do it either ! - MJG */

#if ISPTSP || PCX1 || PCX2 || PCX2_003
	if(HasTranslucent)
	{
		/* 
		// switch off smooth shading 
		*/
		ORedFlags  &= ~pf_smooth_shad;
		ANDedFlags &= ~pf_smooth_shad;

		/*
		// This is only needed when we are doing a mixture of shading types
		*/
		FlagsMask  &= ~pf_smooth_shad;

		#if PCX1 || PCX2 || PCX2_003
			DPF((DBG_MESSAGE, "Disabling smooth translucent. IS this Needed?"));
		#endif
	}
#endif


	/*
	// if all the surfaces in the objects are to be textured/shaded in the same
	// manner ...
	*/
	if(ORedFlags == ANDedFlags)
	{
		
		bFogOn = pState->pQualityState->flags & qf_fog;
		
		/*
		// turn off texture wrapping if it is not enabled in the material
		// (IE texture on amd premapped off)
		*/
		if((ANDedFlags & (pf_pre_mapped|pf_textured))== pf_textured)
		{
			/*
			// If wrapping impossible, switch it off
			*/
			if(! (pMaterial->texture_flags & wrapping_on))
			{
				ANDedFlags &= ~pf_textured;
			}
		}

		SGL_TIME_SUSPEND(PLANE_STUFF_TIME)

		switch (ANDedFlags & ~(pf_pre_mapped))
		{
			case pf_textured:
			{
				if (ANDedFlags & pf_pre_mapped)
				{
					DoTextureMappingFast (nPlanes, 
						(const TRANSFORMED_PLANE_STRUCT *const*) gXPlaneList, 
						pState->pTransformState, pMaterial, 
						GlobalTextureResults);

				}
				/*
				// Else texture wrap it
				*/
				else
				{
					DoTextureWrapping (nPlanes, gXPlaneList, 
									  pState->pTransformState, pMaterial,
									  GlobalTextureResults);
				}


				DoFlatTextureShading (FALSE, nPlanes, gXPlaneList, 
									 pState->pTransformState, 
									 pMaterial, pState->pLightsState,
									 GlobalShadingResults);

				PackTexasParamsFlatTextured(nPlanes, GlobalTextureResults, 
								GlobalShadingResults, bShadowLights,
								bFogOn, gXPlaneList, pMaterial);

				break;
			}


			case pf_textured | pf_smooth_shad:
			{
				if (ANDedFlags & pf_pre_mapped)
				{
					DoTextureMappingFast (nPlanes, 
						(const TRANSFORMED_PLANE_STRUCT *const*) gXPlaneList, 
						pState->pTransformState, pMaterial,
						GlobalTextureResults);
				}
				else
				{
					DoTextureWrapping (nPlanes, gXPlaneList, 
									  pState->pTransformState, pMaterial,
									   GlobalTextureResults);
				}
				GetSmoothShadingParametersAdjoint (nPlanes, gXPlaneList, 
										   pState->pTransformState, 
										   pMaterial, 
										   pState->pLightsState, 
										   GlobalShadingResults, &gSP);

				PackTexasParamsSmoothTextured(nPlanes, GlobalTextureResults, 
										  GlobalShadingResults,  
										  bShadowLights, bFogOn, 
										  gXPlaneList, pMaterial);

				break;
			}

			case pf_smooth_shad:
			{
				/*
				// Also turn off translucency if no texture - this is not a
				// good way of working around the hardware, but will do for
				// now at least.
				*/

				#if 0 
					/*
					//if we have both translucency and smooth shading, turn the smooth
					//shading off instead of the translucency 
					*/
					/*HasTranslucent = FALSE;*/
				#else
					HasTranslucent = FALSE;
				#endif

				GetSmoothShadingParametersAdjoint (nPlanes, gXPlaneList, 
										   pState->pTransformState, 
										   pMaterial, pState->pLightsState, 
										   GlobalShadingResults, &gSP);

				if(bShadowLights)
				{
					PackTexasParamsSmoothNonTexturedShad (nPlanes, 
													  GlobalShadingResults, 
													  bFogOn, 
													  gXPlaneList);
				}
				else
				{
					PackTexasParamsSmoothNonTextured (nPlanes, 
													  GlobalShadingResults, 
													  bFogOn, 
													  gXPlaneList);
				}
				break;
			}

 			default:
			{
				/*
				// Also turn off translucency if no texture - this is not a
				// good way of working around the hardware, but will do for
				// now at least.
				*/
				/*HasTranslucent = FALSE;*/

				DoFlatShading (FALSE, nPlanes, gXPlaneList, 
							  pState->pTransformState, pMaterial,
							  pState->pLightsState, GlobalShadingResults);

				if(HasTranslucent)
				{
					PackTexasParamsFlatNonTexturedTrans(nPlanes, 
												   GlobalShadingResults,
												   bShadowLights,
												   bFogOn,
												   pMaterial->translucent_int,
												   gXPlaneList);
				}
				else
				{
					PackTexasParamsFlatNonTextured(nPlanes, 
												   GlobalShadingResults,
												   bShadowLights,
												   bFogOn,
												   gXPlaneList);
				}												   
			   
				break;
			}
		}

		SGL_TIME_RESUME(PLANE_STUFF_TIME)

	}
	/*
	// Else we have a variety of shading operations to apply
	*/
	else
	{
		static	TRANSFORMED_PLANE_STRUCT	*gXVisible[SGL_MAX_INTERNAL_PLANES];
		static	TRANSFORMED_PLANE_STRUCT	*gXTextured[SGL_MAX_INTERNAL_PLANES];
		static	TRANSFORMED_PLANE_STRUCT	*gXTexWrap[SGL_MAX_INTERNAL_PLANES];
		static	TRANSFORMED_PLANE_STRUCT	*gXShaded[SGL_MAX_INTERNAL_PLANES];
		static	TRANSFORMED_PLANE_STRUCT	*gXTexShad[SGL_MAX_INTERNAL_PLANES];
		static	TRANSFORMED_PLANE_STRUCT	*gXTexWrapShad[SGL_MAX_INTERNAL_PLANES];
		int nVisible = 0;
		int nTextured = 0;
		int nTexWrap = 0;
		int nShaded = 0;
		int nTexShad = 0;
		int nTexWrapShad = 0;
		int FaceFlag;


		/*
		// The following code could be made a LOT faster...
		*/

		/*
		// Clear off unneeded flags
		*/
		FlagsMask  &= ~(pf_pre_mapped | pf_reversed);

		/* faces have different material properties */

		for (k = 0; k < nPlanes; ++k)
		{
			FaceFlag = GlobalTransformedPlanes[k].flags;


			/*
			// turn off texture wrapping if it is not enabled in the material
			*/
			if((FaceFlag & (pf_pre_mapped | pf_textured))== pf_textured)
			{
				/*
				// If wrapping impossible, switch it off
				*/
				if(! (pMaterial->texture_flags & wrapping_on))
				{
					FaceFlag &= ~pf_textured;
				}
			}


			/*
			// Also turn off translucency. THIS IS NOT a proper fix - but
			// we cant have non-textured translucency! What we should do
			// is individually put planes in with either translucent or
			// non translucent routines (see below) but I haven't time to
			// sort out that sort of nonsense now.    SJF
			*/
			if(!(FaceFlag & pf_textured))
			{
				HasTranslucent = FALSE;
			}

			switch (FaceFlag & FlagsMask)
			{
				case pf_textured:
				{
					if (FaceFlag & pf_pre_mapped)
					{
						gXTextured[nTextured++] = &GlobalTransformedPlanes[k];
					}
					else
					{
						gXTexWrap[nTexWrap++] = &GlobalTransformedPlanes[k];
					}
					break;
				}
	
				case pf_textured | pf_smooth_shad:
				{
					if (FaceFlag & pf_pre_mapped)
					{
						gXTexShad[nTexShad++] = &GlobalTransformedPlanes[k];
					}
					else
					{
						gXTexWrapShad[nTexWrapShad++] = &GlobalTransformedPlanes[k];
					}
					break;
				}
	
				case pf_smooth_shad:
				{
					gXShaded[nShaded++] = &GlobalTransformedPlanes[k];
					break;
				}
	
				default:
				{
					gXVisible[nVisible++] = &GlobalTransformedPlanes[k];
					break;
				}
			}/*end switch*/
		}


		bFogOn = pState->pQualityState->flags & qf_fog;

	    SGL_TIME_SUSPEND(PLANE_STUFF_TIME)

		if (nTexShad)
		{
			GetSmoothShadingParametersAdjoint (nTexShad, 
				(TRANSFORMED_PLANE_STRUCT **) gXTexShad, 
			    pState->pTransformState, pMaterial,
			    pState->pLightsState, GlobalShadingResults, &gSP);

			DoTextureMappingFast (nTexShad, 
				(const TRANSFORMED_PLANE_STRUCT * const*) gXTexShad, 
				pState->pTransformState, pMaterial, GlobalTextureResults);

			PackTexasParamsSmoothTextured (nTexShad, GlobalTextureResults, 
									GlobalShadingResults, 	
									bShadowLights,
									   bFogOn, gXTexShad, pMaterial);

		}

		if (nTexWrapShad)
		{
			GetSmoothShadingParametersAdjoint (nTexWrapShad, gXTexWrapShad,
									   pState->pTransformState, 
									   pMaterial, 
									   pState->pLightsState, GlobalShadingResults, 
									   &gSP);

			DoTextureWrapping (nTexWrapShad, gXTexWrapShad, 
							pState->pTransformState, pMaterial,
							GlobalTextureResults);

			PackTexasParamsSmoothTextured (nTexWrapShad, GlobalTextureResults, 
										   GlobalShadingResults, 
										   bShadowLights, bFogOn, 
										   gXTexWrapShad, pMaterial);
		}

		if (nTextured)
		{
			DoTextureMappingFast (nTextured, 
				(const TRANSFORMED_PLANE_STRUCT *const*) gXTextured, 
				pState->pTransformState, pMaterial, GlobalTextureResults);

			DoFlatTextureShading (FALSE, nTextured, gXTextured,  
								 pState->pTransformState,
								 pMaterial, pState->pLightsState,
								 GlobalShadingResults);

			PackTexasParamsFlatTextured (nTextured, GlobalTextureResults, 
							GlobalShadingResults,bShadowLights, bFogOn, 
							 gXTextured, pMaterial);
		}

		if (nTexWrap)
		{
			DoTextureWrapping (nTexWrap, gXTexWrap, pState->pTransformState, 
							  pMaterial, GlobalTextureResults);

			DoFlatTextureShading (FALSE, nTexWrap, gXTexWrap,
								 pState->pTransformState, 
								 pMaterial,pState->pLightsState,
								 GlobalShadingResults);

			PackTexasParamsFlatTextured(nTexWrap, GlobalTextureResults, 
							GlobalShadingResults, bShadowLights,
							 bFogOn, gXTexWrap, pMaterial);
		}

		if (nShaded)
		{
			GetSmoothShadingParametersAdjoint (nShaded, gXShaded, 
									   pState->pTransformState, pMaterial,
									   pState->pLightsState, 
									   GlobalShadingResults, &gSP);

			PackTexasParamsNonTextured(nShaded,GlobalShadingResults, 
							TRUE, bShadowLights, bFogOn, gXShaded);
		}

		if (nVisible)
		{
			DoFlatShading (FALSE, nVisible, gXVisible, pState->pTransformState, 
						  pMaterial, pState->pLightsState, GlobalShadingResults);

			PackTexasParamsNonTextured(nVisible, GlobalShadingResults, 
							FALSE, bShadowLights, bFogOn, gXVisible);
		}

	    SGL_TIME_RESUME(PLANE_STUFF_TIME)

	}/*end else*/

	SGL_TIME_STOP(PLANE_STUFF_TIME)

	if (HasTranslucent)
	{
		return (PutTranPlanesInRegions (GlobalTransformedPlanes, gXExtras, 
										nPlanes, nCurrTransSetID));
	}	
	else
	{
		return (PutPlanesInRegions (GlobalTransformedPlanes, gXExtras, nPlanes));
	}
}



/**************************************************************************
 * Function Name  : ProcessVerticesAllVisible  (LOCAL FUNCTION)
 * Inputs         : pVertices     - pointer to start of an array of vertex data
 *					nVertices	  - the number of vertices to process
 *	
 *
 * NOTE: There is a problem I (sjf) had to work around concerning the region
 *		 calculation of vertices and the Edge ABC parameter generation. IF a
 *		 vertex was JUST above (or to the left) of a region boundary, it will
 *		 naturally be placed in that region. Unfornately, with the rounding that
 *		 is somehow done with the edge ABC calculation, the edge ends up JUST
 *		 in the region below. Thus if you have a horizontal (or vertical) join
 *		 which lies a TINY fraction above (or to the left) between two faces,
 *		 you get a one pixel gap appearing - the chances of this are slim, but
 *		 it is very annoying when it does appear. To "fix" this, I've added a
 *		 small round up value which will push such vertices into the next region.
 *
 *		 I guess we could potentially have the reverse situation, (because of
 *		 this "rounding" up) but the value  seems to be small enough that
 *		 I haven't seen it occur. 
 **************************************************************************/

#define FIX_ROUNDING_PROBLEM (0.001f)

static void ProcessVerticesAllVisible( 	 PVERTEX pVertex, 
											 int nVertices, 
						const TRANSFORM_STRUCT	 *pTransform)
{
	PTRANSVERTEX pXVertex;

	PROJECTION_MATRIX_STRUCT  * const pProjMat = RnGlobalGetProjMat ();

	float	fRegionXScale = pProjMat->fRegionXScale;
	float	SxDash = pProjMat->SxDash;
	float	SyDash = pProjMat->SyDash;
	float	OxDash = pProjMat->OxDash;
	float	OyDash = pProjMat->OyDash;
	register float f1OverZ;
	float	fXws, fYws, fZws;
	float	fXss, fYss;
#if PRE_CULL
	sgl_uint8	*p = gpXVertexIndex;
#endif

	SGL_TIME_START(TRANSFORM_VERTICES_ALL_TIME)
	
	pXVertex = gpXVertices;

	for (/*None*/; nVertices != 0; --nVertices, ++pVertex, ++pXVertex)
	{
#if PRE_CULL
		if (*p)
		{
#endif
			pXVertex->pvVertex = pVertex->vVertex;
			
			/*
			// For brevity define v, and m as shortcuts into the vector and
			// matrix
			*/
			#define v pVertex->vVertex
			#define m pTransform->mat

			fXws = v[0] * m[0][0] + v[1] * m[0][1] + v[2] * m[0][2] + m[0][3];
			fYws = v[0] * m[1][0] + v[1] * m[1][1] + v[2] * m[1][2] + m[1][3];
			fZws = v[0] * m[2][0] + v[1] * m[2][1] + v[2] * m[2][2] + m[2][3];

			pXVertex->vWorldSpace[0] = fXws;
			pXVertex->vWorldSpace[1] = fYws;
			pXVertex->vWorldSpace[2] = fZws;
			#undef v
			#undef m


			f1OverZ = 1.0f / fZws;

			fXss = (SxDash * fXws * f1OverZ) + OxDash;
			fYss = (SyDash * fYws * f1OverZ) + OyDash;
			pXVertex->vScreenSpace[0] = fXss;
			pXVertex->vScreenSpace[1] = fYss;
			
			/* Work out which region it falls in */
			pXVertex->nRegion[0] = (sgl_int32) (fXss * fRegionXScale + 
												   FIX_ROUNDING_PROBLEM);

			/* For optimisation exact Y co-ordinates required.
			 */
			pXVertex->nRegion[1] = (sgl_int32) fYss;

			ASSERT(RnGlobalProjMatRegionOnScreen(pXVertex->nRegion[0], 
												 pXVertex->nRegion[1]));

#if PRE_CULL
		}
		++p;
#endif
	}

	SGL_TIME_STOP(TRANSFORM_VERTICES_ALL_TIME)
}

/**************************************************************************
 * Function Name  : ProcessVerticesPartlyVisible  (LOCAL FUNCTION)
 * Inputs         : pVertices     - pointer to start of an array of vertex data
 *					nVertices	  - the number of vertices to process
 *	
 **************************************************************************/

static TEST_BOX_ENUM ProcessVerticesPartlyVisible (PVERTEX pVertex, 
												   int	  nVertices, 
											  const TRANSFORM_STRUCT *pTransform)
{
	PTRANSVERTEX pXVertex;

	PROJECTION_MATRIX_STRUCT  * const pProjMat = RnGlobalGetProjMat ();

	float	SxDash = pProjMat->SxDash;
	float	SyDash = pProjMat->SyDash;
	float	OxDash = pProjMat->OxDash;
	float	OyDash = pProjMat->OyDash;
	register float f1OverZ;
	float	fXws, fYws, fZws;
	float	fXss, fYss;
	float FirstYRegionExact, LastYRegionExact;
	float foregroundDistance, fRegionXScale, FirstXRegion, LastXRegion;
	float fViewportMinX, fViewportMaxX, fViewportMinY, fViewportMaxY;
	/*
	// These are used to determine if the mesh is entirely
	// on screen, or possibly entirely offscreen, or partly on screen.
	*/
	sgl_bool AnyClipping;
	int		 AndFlags;

	SGL_TIME_START(TRANSFORM_VERTICES_PARTLY_TIME)

	FirstYRegionExact = (float)pProjMat->FirstYRegionExact;
	LastYRegionExact = (float)pProjMat->LastYRegionExact;
	foregroundDistance = pProjMat->foregroundDistance;
	fRegionXScale = pProjMat->fRegionXScale;
	FirstXRegion = (float)pProjMat->FirstXRegion;
	LastXRegion = (float)pProjMat->LastXRegion;
	fViewportMinX = pProjMat->fViewportMinX;
	fViewportMaxX = pProjMat->fViewportMaxX;
	fViewportMinY = pProjMat->fViewportMinY;
	fViewportMaxY = pProjMat->fViewportMaxY;

#if PRE_CULL
	sgl_uint8	*p = gpXVertexIndex;
#endif

	AnyClipping  = FALSE;
	AndFlags =~0;

	pXVertex = gpXVertices;

	for (/*None*/; nVertices != 0; --nVertices, ++pVertex, ++pXVertex)
	{
#if PRE_CULL
		if (*p)
		{
#endif
			pXVertex->pvVertex = pVertex->vVertex;
			
			/*
			// For brevity define v, and m as shortcuts into the vector and
			// matrix
			*/
			#define v pVertex->vVertex
			#define m pTransform->mat
			
			fXws = v[0] * m[0][0] + v[1] * m[0][1] + v[2] * m[0][2] + m[0][3];
			fYws = v[0] * m[1][0] + v[1] * m[1][1] + v[2] * m[1][2] + m[1][3];
			fZws = v[0] * m[2][0] + v[1] * m[2][1] + v[2] * m[2][2] + m[2][3];


			pXVertex->vWorldSpace[0] = fXws;
			pXVertex->vWorldSpace[1] = fYws;
			pXVertex->vWorldSpace[2] = fZws;
			#undef v
			#undef m

			/*
			// Check first if it is on the wrong side of the foreground
			// clipping plane.
			*/
			if (fZws < foregroundDistance)
			{
				int   Flags;
				AnyClipping = TRUE;

				Flags =  CLIPPED_Z;
				/*
				// Test the other viewing fustrum boundaries as well
				//
				// Test against Min X
				*/
				if(fXws < fViewportMinX * fZws)
				{
					Flags |= CLIPPED_MINUS_X;
				}

				/*
				// Test against Max X. Note that min and max x clipping
				// is not mutually exclusive with a negative Z.
				*/
				if(fXws > fViewportMaxX * fZws)
				{
					Flags |= CLIPPED_PLUS_X;
				}
				/*
				// Test against Min Y.
				*/
				if(fYws < fViewportMinY * fZws)
				{
					Flags |= CLIPPED_MINUS_Y;
				}

				/*
				// Test against Max Y.
				*/
				if(fYws > fViewportMaxY * fZws)
				{
					Flags |= CLIPPED_PLUS_Y;
				}
				pXVertex->Flags = Flags;
 				AndFlags &= Flags;
			}
			else
			{
				sgl_int32 Region;
				int   Flags;

				f1OverZ = 1.0f / fZws;

				fXss = (SxDash * fXws * f1OverZ) + OxDash;
				fYss = (SyDash * fYws * f1OverZ) + OyDash;
				pXVertex->vScreenSpace[0] = fXss;
				pXVertex->vScreenSpace[1] = fYss;
				/* 
				// Work out which region it falls in.
				// NOTE: There is a REALLY unlikely bug which could occur if the
				// mesh face is absolutely enormous and the projected region is
				// larger than can be represented by an sgl_int32.... but who could
				// be bothered testing for it....
				*/
				Region = (sgl_int32) (fXss * fRegionXScale + 
								  FIX_ROUNDING_PROBLEM);
				if(Region < FirstXRegion)
				{
					pXVertex->nRegion[0] = (sgl_int32)FirstXRegion;
					Flags = CLIPPED_MINUS_X;
					AnyClipping = TRUE;
				}
				else if(Region > LastXRegion)
				{
					pXVertex->nRegion[0] = (sgl_int32)LastXRegion;
					Flags = CLIPPED_PLUS_X;			
					AnyClipping = TRUE;
				}
				else 
				{
					pXVertex->nRegion[0] = Region;
					Flags = 0;
				}


				/* Correction needed with Y region.	Absolute Y co-ordinates required.
				 */
				Region = (sgl_int32) fYss;

				/* first Y region is the top of the viewport - max Y */
				if(Region < FirstYRegionExact)
				{
					pXVertex->nRegion[1] = (sgl_int32)FirstYRegionExact;
					Flags |= CLIPPED_PLUS_Y;
					AnyClipping = TRUE;
				}
				else if(Region > LastYRegionExact)
				{
					pXVertex->nRegion[1] = (sgl_int32)LastYRegionExact;
					Flags |= CLIPPED_MINUS_Y;
					AnyClipping = TRUE;
				}
				else 
				{
					pXVertex->nRegion[1] = Region;
				}

				/*
				// Update whether all the vertices are to one side of the
				// view pyramid, and the flag values for this vertex.
				*/
				AndFlags &= Flags;
				pXVertex->Flags= Flags;
			}/*end else NOT Z clipped*/

#if PRE_CULL
		}
		++p;
#endif
	}/*end for */

	/*
	//Decide whether the mesh is entirely on screen etc
	*/
	if(!AnyClipping)
	{
	    SGL_TIME_STOP(TRANSFORM_VERTICES_PARTLY_TIME)
		return(TB_BOX_ALL_ONSCREEN);
	}
	else if(AndFlags)
	{
	    SGL_TIME_STOP(TRANSFORM_VERTICES_PARTLY_TIME)
		return(TB_BOX_OFFSCREEN);
	}
	else
	{
	    SGL_TIME_STOP(TRANSFORM_VERTICES_PARTLY_TIME)
		return(TB_BOX_PART_ONSCREEN);
	}
}

/**************************************************************************
 * Function Name  : ProcessEdgesAllVisible  (LOCAL FUNCTION)
 * Inputs         : pEdges     - pointer to start of an array of edge data
 *					nEdges	   - the number of edges to process
 *	
 **************************************************************************/

/*sgl_int32 PackTo20BitDoubleBarrel (const float	F1, const float F2, int *pV1, int *pV2);*/

void ProcessEdgesAllVisible (PEDGE pEdge, int nEdges)
{
	#if ICL && !(PCX2 || PCX2_003)

		/*
		// Define the largest exponent the SABRE hardware can handle
		*/
		#define MAX_LEGAL_20_EXPONENT (14)


		/*
		// Define whether we should have an extra 1/2 bits accuracy
		// when changing from IEEE 24 bit mantissa to the 16 bit 
		// mantissa of the Sabre Float
		*/
		#define USE_ROUNDING 0

		/*///////////////////////////////////////////////////////////////////
		///////////////////////////////////////////////////////////////////// 
		// IEEE version 
		///////////////////////////////////////////////////////////////////// 
		////////////////////////////////////////////////////////////////// */


		#define IEEE_SIGN_BIT (0x80000000L)

		/*
		// Mantissa is in lower 23 bits
		*/
		#define IEEE_MANTISSA_MASK		  0x007FFFFFL	/*	((1L<<23)-1) */
		#define IEEE_ASSUMED_MANTISSA_BIT 0x00800000L	/* (1L<<23L) */

		/*
		// Values for getting the IEEE exponent
		*/
		#define IEEE_E_MASK  (0xFF)
		#define IEEE_E_SHIFT (23)

		/*
		// Define how much we have to shift down the IEEE mantissa
		// to get the right number of bits for the 20bit float
		// NOTE this INCLUDES the assumed Most sig bit.
		*/
		#define IEEE_TO_20_SHIFT 9	/* (24 - 15) */

		/*
		// Define how to round from the IEEE float to the 20 bit float.
		// the rounding bit is the bit JUST after the where the
		// 20 bit mantissa runs out
		*/
		#define MANTISSA_ROUND 0x00000100L /*(1L<<(IEEE_TO_20_SHIFT - 1))*/

		/*
		// Define the maximum mantissa we can hold, so we can reset a value
		// if we get an overflow in rounding.
		//
		// Also define which bit will get set if we get an overflow
		*/
		#define MAX_MANTISSA      0x00FFFFFFL		/*((0x8000L << (IEEE_TO_20_SHIFT)) - 1)*/
		#define MANTISSA_OVERFLOW 0x01000000L		/*(0x8000L << (IEEE_TO_20_SHIFT))*/

		/*
		// A mask so we can negate JUST the mantissa
		*/
		#define MASK_20_MANTISSA (0xFFFFL)  /* 16 bits */

		/*
		// Define the IEEE exponent which is effectively the same
		// as a "zero" exponent in our 20bit "sabre floats"
		*/
		#define IEEE_E_SHIFT_DOWN (102)

		/*
		// Define range of exponents we can handle
		*/
		#define IEEE_E_SMALLEST  87 /*(IEEE_E_SHIFT_DOWN - 15)*/
		#define IEEE_E_LARGEST	 116 /*(IEEE_E_SHIFT_DOWN + MAX_LEGAL_20_EXPONENT )*/

		/*
		// Where to put the exponent in the 20 bit number
		*/
		#define SHIFT_20_EXP (16)

		/*
		// Where the sign bit is for 20 bit S float
		*/
		#define SIGN_BIT_FOR_20BIT 0x00008000L	/*(1<<15)*/

		static float	fFloatToFixed = FLOAT_TO_FIXED;
		static float	fMaxCentre = 1024.0f;
		static float	fHalfOffset = 1024.0f;

		SGL_TIME_START(TRANSFORM_EDGES_ALL_TIME)
		
		__asm
		{
			push	ebx
			push	esi
			push	edi

			mov		ecx, DWORD PTR nEdges
			mov		eax, DWORD PTR pEdge
			
			/* make sure the parameters are half useful */

			or		ecx, ecx
			jz		FEAV_BombOut
			or		eax, eax
			jz		FEAV_BombOut

			/* 
				create a new stack frame 
			*/
			
			sub		esp, 12
			mov		[esp + 4], ecx
			
			/* 
				esp:		parameter for PackTo20Bit
				esp + 4:	counter
				esp + 8:	fMaxValueTmpStore
			*/
			
			mov		esi, eax
			mov		edi, gpXEdges
			mov		ebx, DWORD PTR gpXVertices

			/*
				registers
				
				eax:	-
				ebx:	gpXVertices
				ecx:	-
			edx:	-
				esi:	pEdge
				edi:	pXEdge
			*/

FEAV_loop:			

;			int 3
			
			/* 
				edx:		pV1
				ecx:		pV2

				sizeof (TRANSVERTEX) = 36 = 4*9
			*/
			
			mov		edx, [esi].nIDV1
			mov		ecx, [esi].nIDV2
			lea		edx, [edx+edx*8]
			lea		ecx, [ecx+ecx*8]
			lea		edx, [ebx+edx*4]
			lea		ecx, [ebx+ecx*4]
			
			/*
			// Compute initial A,B,&C values before scaling, using the
			// projected vertex positions

			fA = pV1->vScreenSpace[1] - pV2->vScreenSpace[1];
			fB = pV2->vScreenSpace[0] - pV1->vScreenSpace[0];
			fC = (pV1->vScreenSpace[0] * pV2->vScreenSpace[1]) - 
				 (pV2->vScreenSpace[0] * pV1->vScreenSpace[1]);

			*/

			fld		DWORD PTR 4[edx].vScreenSpace
			fsub	DWORD PTR 4[ecx].vScreenSpace
			fld		DWORD PTR 0[ecx].vScreenSpace
			fsub	DWORD PTR 0[edx].vScreenSpace
			fld		DWORD PTR 0[edx].vScreenSpace
			fmul	DWORD PTR 4[ecx].vScreenSpace
			fld		DWORD PTR 0[ecx].vScreenSpace
			fmul	DWORD PTR 4[edx].vScreenSpace
			fxch	ST(1)
			fsubrp	ST(1), ST

			/*
				MaxVal = sfabs(MAX_CENTRE * (fA+fB) + fC) + HALF_OFFSET * (sfabs(fA) + sfabs(fB));
			*/

			fld		ST(2)					; fA fC fB fA
			fadd	ST, ST(2)				; fA+fB fC fB fA
			fld		ST(2)					; fB fA+fB fC fB fA
			fabs							; fabs(fB) fA+fB fC fB fA
			fld		ST(4)					; fA fabs(fB) fA+fB fC fB fA
			fabs							; fabs(fA) fabs(fB) fA+fB fC fB fA
			fxch	ST(2)					; fA+fB fabs(fB) fabs(fA) fC fB fA
			fmul	DWORD PTR fMaxCentre	; M*(fA+fB) fabs(fB) fabs(fA) fC fB fA
			fxch	ST(1)					; fabs(fB) M*(fA+fB) fabs(fA) fC fB fA
			faddp	ST(2), ST				; M*(fA+fB) fabs(fA)+fabs(fB) fC fB fA
			fadd	ST, ST(2)				; (M*(fA+fB))+fC fabs(fA)+fabs(fB) fC fB fA
			fxch	ST(1)					; fabs(fA)+fabs(fB) (M*(fA+fB))+fC fC fB fA
			fmul	DWORD PTR fHalfOffset	; H*(fabs(fA)+fabs(fB)) (M*(fA+fB))+fC fC fB fA
			fxch	ST(2)					; fC (M*(fA+fB))+fC H*(fabs(fA)+fabs(fB)) fB fA
			fmul	DWORD PTR fFloatToFixed	; fC*f2F (M*(fA+fB))+fC H*(fabs(fB)+fabs(fA)) fB fA
			fxch	ST(1)					; (M*(fA+fB))+fC fC*f2F H*(fabs(fB)+fabs(fA)) fB fA
			fabs							; fabs((M*(fA+fB))+fC) fC*f2F H*(fabs(fA)+fabs(fB)) fB fA
			faddp	ST(2), ST				; fC*f2F H*(fabs(fB)+fabs(fA))+fabs((M*(fA+fB))+fC) fB fA
			fxch	ST(1)
			
			/*
				fMaxVal fC	fB	fA
			*/

			fstp	DWORD PTR 8[esp]			/* C  B  A  -  -  -  -  -  -  */               

			mov		ecx, 8[esp]
			mov		edx, ecx
			and		ecx, 0x7F800000
			neg		ecx
			and		edx, 0x007FFFFF+0x7FFFFFFF+1
			add		ecx, 0x7E800000
			xor		edx, 0x007FFFFF
			and		ecx, 0x7F800000
			or		ecx, edx
			mov		8[esp], ecx

			fmul	DWORD PTR 8[esp]			/* C  B  A  -  -  -  -  */               
			fxch	ST(2)						/* A  B  c  -  -  -  -  */               
			add		esi, SIZE EDGE
			fmul	DWORD PTR 8[esp]
			fxch	ST(2)						/* c  B  a  -  -  -  -  */               
			fistp	DWORD PTR 8[edi]			/* B  a  -  -  -  -  */               
			fmul	DWORD PTR 8[esp]
			fxch	ST(1)						/* a  b  -  -  -  -  */               
			fstp	DWORD PTR 0[esp]			/* b  -  -  -  -  */               

			#if 0

		        call    PackTo20Bit
				mov		DWORD PTR 0[edi], eax

			#else

				/* 
					Pack to 20 bit format

					ecx: exponent
					edx: mantissa
				*/
				mov		ecx, 0[esp]
				mov		edx, ecx
				
				; exp = (intVal >> IEEE_E_SHIFT) & IEEE_E_MASK
				
				shr		ecx, IEEE_E_SHIFT
				and		ecx, IEEE_E_MASK

				; mantissa = (intVal & IEEE_MANTISSA_MASK) | IEEE_ASSUMED_MANTISSA_BIT;

				and		edx, IEEE_MANTISSA_MASK
				or		edx, IEEE_ASSUMED_MANTISSA_BIT

				cmp		ecx, IEEE_E_SMALLEST
				jge		p20_1_1
				xor		edx, edx					; exponent too small, clip to 0
				jmp		p20_1exit

	p20_1_1:	; exp = exp - IEEE_E_SHIFT_DOWN;

				sub		ecx, IEEE_E_SHIFT_DOWN
				cmp		ecx, 0
				jge		p20_1_2

				neg		ecx							; this number is denormalised in 20 bit
				shr		edx, cl
				add		edx, MANTISSA_ROUND
				shr		edx, IEEE_TO_20_SHIFT
				cmp		DWORD PTR 0[esp], 0
				jge		p20_1exit
			
				#if PCX1
			
					or		edx, SIGN_BIT_FOR_20BIT
				
				#else
			
					neg		edx
					and		edx, MASK_20_MANTISSA
				
				#endif

				jmp		p20_1exit

	p20_1_2:	shr		edx, IEEE_TO_20_SHIFT
				shl		ecx, SHIFT_20_EXP
				cmp		DWORD PTR 0[esp], 0
				jge		p20_1_3
			
				#if PCX1
			
					or		edx, SIGN_BIT_FOR_20BIT
				
				#else
			
					neg		edx
					and		edx, MASK_20_MANTISSA
				
				#endif

	p20_1_3:	or		edx, ecx

	p20_1exit:	mov		DWORD PTR 0[edi], edx

			#endif

			fstp	DWORD PTR 0[esp] 			/* b  -  -  -  -  */               

			#if 0

		        call    PackTo20Bit
				mov		DWORD PTR 4[edi], eax

			#else

				/* 
					Pack to 20 bit format

					ecx: exponent
					edx: mantissa
				*/
				mov		ecx, 0[esp]
				mov		edx, ecx
				
				; exp = (intVal >> IEEE_E_SHIFT) & IEEE_E_MASK
				
				shr		ecx, IEEE_E_SHIFT
				and		ecx, IEEE_E_MASK

				; mantissa = (intVal & IEEE_MANTISSA_MASK) | IEEE_ASSUMED_MANTISSA_BIT;

				and		edx, IEEE_MANTISSA_MASK
				or		edx, IEEE_ASSUMED_MANTISSA_BIT

				cmp		ecx, IEEE_E_SMALLEST
				jge		p20_2_1
				xor		edx, edx					; exponent too small, clip to 0
				jmp		p20_2exit

	p20_2_1:	; exp = exp - IEEE_E_SHIFT_DOWN;

				sub		ecx, IEEE_E_SHIFT_DOWN
				cmp		ecx, 0
				jge		p20_2_2

				neg		ecx							; this number is denormalised in 20 bit
				shr		edx, cl
				add		edx, MANTISSA_ROUND
				shr		edx, IEEE_TO_20_SHIFT
				cmp		DWORD PTR 0[esp], 0
				jge		p20_2exit
			
				#if PCX1
			
					or		edx, SIGN_BIT_FOR_20BIT
				
				#else
			
					neg		edx
					and		edx, MASK_20_MANTISSA
				
				#endif

				jmp		p20_2exit

	p20_2_2:	shr		edx, IEEE_TO_20_SHIFT
				shl		ecx, SHIFT_20_EXP
				cmp		DWORD PTR 0[esp], 0
				jge		p20_2_3
			
				#if PCX1
			
					or		edx, SIGN_BIT_FOR_20BIT
				
				#else
			
					neg		edx
					and		edx, MASK_20_MANTISSA
				
				#endif

	p20_2_3:	or		edx, ecx

	p20_2exit:	mov		DWORD PTR 4[edi], edx

			#endif

			add		edi, SIZE TRANSEDGE
			dec		DWORD PTR 4[esp]
			jnz		FEAV_loop

			add		esp, 12

	FEAV_BombOut:

			pop		edi
			pop		esi
			pop		ebx
		}

		SGL_TIME_STOP(TRANSFORM_EDGES_ALL_TIME)

	#else
	
		PTRANSEDGE		pXEdge;
#if PRE_CULL
		sgl_uint8			*p = gpXEdgeIndex;
#endif
		
		SGL_TIME_START(TRANSFORM_EDGES_ALL_TIME)

		pXEdge = gpXEdges;

		for (/*NOTHING*/; nEdges != 0; --nEdges, ++pEdge, ++pXEdge)
		{
#if PRE_CULL
			if (*p)
			{
#endif
				/*
				// New wizzo faster edge computation that uses screen
				// coordinates directly. This can be done because we know
				// the end points are on screen.
				//						 			Simon
				*/
				float			fA, fB, fC;
				float			MaxVal;
				PTRANSVERTEX	pV1, pV2;
				
				ASSERT (pEdge->nIDV1 != pEdge->nIDV2);

				/*
				// Get pointers to the vertices at each end of the edge
				*/
				pV1 = gpXVertices + pEdge->nIDV1;
				pV2 = gpXVertices + pEdge->nIDV2;

				/*
				// Compute initial A,B,&C values before scaling, using the
				// projected vertex positions
				*/				
				fA = pV1->vScreenSpace[1] - pV2->vScreenSpace[1];
				fB = pV2->vScreenSpace[0] - pV1->vScreenSpace[0];
				fC = (pV1->vScreenSpace[0] * pV2->vScreenSpace[1]) - 
					 (pV2->vScreenSpace[0] * pV1->vScreenSpace[1]);

				/*
				// to prevent the 20Bit SFLOATS from overflowing (currently
				// they are 2x larger than I originally thought necessary)
				*/
				#define MAX_CENTRE  (1024.0f)
				#define HALF_OFFSET (1024.0f)

				MaxVal = sfabs(MAX_CENTRE * (fA+fB) + fC) +
						 HALF_OFFSET * (sfabs(fA) + sfabs(fB));

				#undef MAX_CENTRE
				#undef HALF_OFFSET

				MaxVal = ApproxRecip (MaxVal);

#if (PCX2 || PCX2_003) && !FORCE_NO_FPU
				/* PCX2 has IEEE floating point support. No need for rescaling.
				 */
				pXEdge->f32A = fA * MaxVal;
				pXEdge->f32B = fB * MaxVal;
				pXEdge->f32C = fC * MaxVal;
#else
		        SGL_TIME_SUSPEND(TRANSFORM_EDGES_ALL_TIME)
				#if 1
				pXEdge->n32A = PackTo20Bit (fA * MaxVal);
				pXEdge->n32B = PackTo20Bit (fB * MaxVal);
				#else
				PackTo20BitDoubleBarrel (fA * MaxVal, fB * MaxVal, &pXEdge->n32A, &pXEdge->n32B);
				#endif
		        SGL_TIME_RESUME(TRANSFORM_EDGES_ALL_TIME)

				pXEdge->n32C = (sgl_int32)(fC * MaxVal * FLOAT_TO_FIXED);
#endif


#if PRE_CULL
			}
			++p;
#endif
		}/*end for*/

		SGL_TIME_STOP(TRANSFORM_EDGES_ALL_TIME)

	#endif

}


/**************************************************************************
 * Function Name  : ProcessEdgesPartlyVisible  (LOCAL FUNCTION)
 * Inputs         : pEdges     - pointer to start of an array of edge data
 *					nEdges	   - the number of edges to process
 *	
 **************************************************************************/

static void ProcessEdgesPartlyVisible (PEDGE pEdge, int nEdges)
{
	PTRANSVERTEX	pV1, pV2;
	PTRANSEDGE		pXEdge;
#if PRE_CULL
	sgl_uint8			*p = gpXEdgeIndex;
#endif
	float fOverflowRescale;
	float yToCorner, xToCorner, xPerPixel, yPerPixel;
	PROJECTION_MATRIX_STRUCT  * const pProjMat = RnGlobalGetProjMat ();
	sgl_vector RCentre;

	SGL_TIME_START(TRANSFORM_EDGES_PARTLY_TIME)

	RCentre[0] = pProjMat->RCentre[0];
	RCentre[1] = pProjMat->RCentre[1];
	RCentre[2] = pProjMat->RCentre[2];
	yToCorner = pProjMat->yToCorner;
	xToCorner = pProjMat->xToCorner;
	fOverflowRescale = pProjMat->fOverflowRescale;
	xPerPixel = pProjMat->xPerPixel;
	yPerPixel = pProjMat->yPerPixel;
	

	pXEdge = gpXEdges;

	for (/*Nothing*/ ; nEdges != 0; --nEdges, ++pEdge, ++pXEdge)
	{
	#if PRE_CULL
		if (*p)
		{
	#endif

			ASSERT (pEdge->nIDV1 != pEdge->nIDV2);

			pV1 = gpXVertices + pEdge->nIDV1;
			pV2 = gpXVertices + pEdge->nIDV2;


			/*
			// If the end points are on screen, then use the faster method.
			*/
			if((pV1->Flags | pV2->Flags) == 0)
			{
				float	fA, fB, fC;
				float	MaxVal;

				/*
				// Compute initial A,B,&C values before scaling, using the
				// projected vertex positions
				*/				
				fA = pV1->vScreenSpace[1] - pV2->vScreenSpace[1];
				fB = pV2->vScreenSpace[0] - pV1->vScreenSpace[0];
				fC = (pV1->vScreenSpace[0] * pV2->vScreenSpace[1]) - 
					 (pV2->vScreenSpace[0] * pV1->vScreenSpace[1]);

				/*
				// These values are a bit odd .. they are desgined
				// to prevent the 20Bit SFLOATS from overflowing
				*/
				#define MAX_CENTRE  (1024.0f)
				#define HALF_OFFSET (1024.0f)

				MaxVal = sfabs(MAX_CENTRE * (fA+fB) + fC) +
						 HALF_OFFSET * (sfabs(fA) + sfabs(fB));

				#undef MAX_CENTRE
				#undef HALF_OFFSET

				MaxVal = ApproxRecip (MaxVal);

#if (PCX2 || PCX2_003) && !FORCE_NO_FPU
				/* PCX2 has IEEE floating point support. No need for rescaling.
				 */
				pXEdge->f32A = fA * MaxVal;
				pXEdge->f32B = fB * MaxVal;
				pXEdge->f32C = fC * MaxVal;
#else
			    SGL_TIME_SUSPEND(TRANSFORM_EDGES_PARTLY_TIME)
				pXEdge->n32A = PackTo20Bit (fA * MaxVal);
				pXEdge->n32B = PackTo20Bit (fB * MaxVal);
			    SGL_TIME_RESUME(TRANSFORM_EDGES_PARTLY_TIME)

				pXEdge->n32C = (sgl_int32)(fC * MaxVal * FLOAT_TO_FIXED);
#endif
			}
			/*
			// Else use the world space coords to compute the edge parameters
			*/
			else
			{
				sgl_vector 		vN;
				float			fCentreDotNormal;
				float			fXBorder;	
				float			fYBorder;
				float			fMaxValue;
				float			fA, fB;

				/* 
				//Screen space coords already calculated - calculate edge plane 
				// equation 
				*/
				CrossProd (pV1->vWorldSpace, pV2->vWorldSpace, vN);

				ASSERT ((vN[0] != 0) || (vN[1] != 0) || (vN[2] != 0));
			
				/* calculate sabre parameters for edge plane
				 */
				fCentreDotNormal = DotProd (vN, RCentre);
				fXBorder = vN[0] * xToCorner;
				fYBorder = vN[1] * yToCorner;

				fMaxValue = sfabs (fCentreDotNormal) + sfabs (fXBorder) + 
													   sfabs (fYBorder);

				fMaxValue = - fOverflowRescale * ApproxRecip (fMaxValue);
				fA = vN[0] * xPerPixel * fMaxValue;
				fB = vN[1] * yPerPixel * fMaxValue;

#if (PCX2 || PCX2_003) && !FORCE_NO_FPU
				/* PCX2 has IEEE floating point support. No need for rescaling.
				 * Negative all parameters.
				 */
				pXEdge->f32A = fA;
				pXEdge->f32B = fB;
				pXEdge->f32C = ((fCentreDotNormal-(fXBorder+fYBorder))*fMaxValue);
#else
			    SGL_TIME_SUSPEND(TRANSFORM_EDGES_PARTLY_TIME)
				pXEdge->n32A = PackTo20Bit (fA);
				pXEdge->n32B = PackTo20Bit (fB);
				SGL_TIME_RESUME(TRANSFORM_EDGES_PARTLY_TIME)

				pXEdge->n32C = 
				  (sgl_int32)((fCentreDotNormal-(fXBorder+fYBorder))*fMaxValue*FLOAT_TO_FIXED);
#endif
			}

	#if PRE_CULL
		}
		++p;
	#endif
	}

	SGL_TIME_STOP(TRANSFORM_EDGES_PARTLY_TIME)
}

/**************************************************************************
 * Function Name  : TransformAndComputeSabreParamsVisible  (LOCAL FUNCTION)
 * Inputs         : pFace     - pointer to start of an array of face data
 *					numFaces  - the number of faces to process
 *					pTransform - pointer to the transformation
 *	
 * Outputs        : pTransformedPlanes - the planes/faces after transformation
 *				    pExtras			   - the rest of the data needed for
 *
 * Returns		  : The number of planes/polys NOT rejected. (IE Not
 *					back-face culled) 
 *					This version does no clipping rejection - ie it
 *					should be called by the "entirely onscreen" code.
 **************************************************************************/
static int TransformAndComputeSabreParamsVisible(const FACE  **ppExtFace, 
												int	  numFaces,
							const TRANSFORM_STRUCT	  *pTransform,
										PXMESHEXTRA	  pXExtra,
							 TRANSFORMED_PLANE_STRUCT *pXPlane,
						const CONV_SHADING_STRUCT	  **ppExtShadingData,
						const CONV_TEXTURE_UNION	  **ppExtTextureData,
						const CONV_POINTS_STRUCT	  **ppExtPointsData)
{
	int NumProcessed;
	
	/*
	// To make the code simpler, rather than have two different
	// loops for processing normals: IE either using normal matrix
	// (ie either uniform or no scaling) or transpose of inverse
	// (when non uniform). Basically, since the non-uniform is
	// fairly rare, we will copy the inverse and transpose it, 
	// which means we can use the same code
	*/
	float const (*m)[4];
	float matcopy[3][4];
	PTRANSVERTEX pVert;

	const FACE	 				*pFace;
	const CONV_SHADING_STRUCT	*pShadingData;
	const CONV_TEXTURE_UNION	*pTextureData;
	const CONV_POINTS_STRUCT	*pPointsData;

	float	fCentreDotNormal;
	float	fXBorder;	
	float	fYBorder;	

	float fOverflowRescale;
	float yToCorner, xToCorner, xPerPixel, yPerPixel;
	PROJECTION_MATRIX_STRUCT  * const pProjMat = RnGlobalGetProjMat ();
	sgl_vector RCentre;

	SGL_TIME_START(TRANSFORM_COMPUTE_SABRE_ALL_TIME)

	RCentre[0] = pProjMat->RCentre[0];
	RCentre[1] = pProjMat->RCentre[1];
	RCentre[2] = pProjMat->RCentre[2];
	yToCorner = pProjMat->yToCorner;
	xToCorner = pProjMat->xToCorner;
	fOverflowRescale = pProjMat->fOverflowRescale;
	xPerPixel = pProjMat->xPerPixel;
	yPerPixel = pProjMat->yPerPixel;

	/*
	// Initialise the number processed and kept, to 0 
	*/
	NumProcessed = 0;


	/*
	// De-reference the Face, shading etc pointers to get local
	// copies that we increment. At the end of the routine, we
	// store these incremented values back again
	//
	// It SHOULD be much quicker to manipulate the local copies, as
	// this should result in less indirection.
	*/
	pFace 		= *ppExtFace;
	pShadingData= *ppExtShadingData;
	pTextureData= *ppExtTextureData;
	pPointsData = *ppExtPointsData;
	

	/*
	// If the transform matrix is non uniform, then copy the inverse,
	// transposing it at the same time
	// This is not very common (hopefully :) )
	*/
	
	if ( (pTransform->scale_flag == arbitrary_scale) ||
	     (pTransform->has_neg_scaling))
	{
		int i,j;

		if (pTransform->has_neg_scaling)
		{
			float fScale = -1.0f;
		
			for(i= 0; i < 3; i ++)
			{
				for(j= 0; j < 3; j ++)
				{
					matcopy[j][i] = pTransform->inv[i][j] * (fScale);
				}
			}
		}
		else
		{
			for(i= 0; i < 3; i ++)
			{
				for(j= 0; j < 3; j ++)
				{
					matcopy[j][i] = pTransform->inv[i][j];
				}
			}
		}
		/*
		// Set up pointer to the copy
		*/
		m = (void*) matcopy;
	}
	/*
	// else just set the pointer at the usual matrix
	*/
	else
	{
		m = pTransform->mat;
	}

	/*
	// Step through the planes
	*/
	for(/*Nil*/; numFaces != 0;  numFaces--, pFace++, 
							   pShadingData++, pTextureData++ , pPointsData++)
	{
	#if PRE_CULL
		if (pFace->EdgeFlags & FACE_VISIBLE)
		{
	#endif

			/*
			// Define some temporary float values so that the compiler
			// can assume that when we write to memory, we wont destroy
			// the source... as in the following matrix operation.
			*/
			float tmp1, tmp2, tmp3;
			
			/*
			// Transform the normal
			*/
			#define Norm pFace->PlaneData.normal
			tmp1 = Norm[0]*m[0][0] + Norm[1]*m[0][1] + Norm[2]*m[0][2];

			tmp2 = Norm[0]*m[1][0] + Norm[1]*m[1][1] + Norm[2]*m[1][2];

			tmp3 = Norm[0]*m[2][0] + Norm[1]*m[2][1] + Norm[2]*m[2][2];
			#undef Norm

			pXPlane->normal[0] = tmp1;
			pXPlane->normal[1] = tmp2;
			pXPlane->normal[2] = tmp3;
			/*
			// Get a pointer to a transformed point on this face
			*/
			pVert = gpXVertices + pFace->pnVertexIDs[0];

			/*
			// Get the "D" value of the plane
			*/
			pXPlane->d = DotProd(pXPlane->normal, pVert->vWorldSpace);

			#if PRE_CULL

				/*
				// Decide which way it faces, and whether we can cull it or not
				*/
			#if SLOW_FCMP && !MULTI_FP_REG
				if(FLOAT_TO_LONG(pXPlane->d) > 0)
			#else
				if(pXPlane->d > 0.0f)
			#endif
				{
					/*
					// This is a FORWARD PLANE (Jim has put something back-the-front)
					// reverse the normal to get it the right way round.
					*/
					VecNegate (pXPlane->normal);
					pXPlane->d = -pXPlane->d;
					pXExtra->u32EdgeFlags = pFace->EdgeFlags ^ 0xFF;
					pXPlane->flags = (int) pFace->PlaneData.flags | pf_reversed;
				}
				/*
				// Else is a reverse (anti clockwise) plane 
				// (Something else is back the front)
				*/
				else
				{
					pXExtra->u32EdgeFlags = pFace->EdgeFlags;
					pXPlane->flags = (int) pFace->PlaneData.flags;
				}
			
			#else

				/*
				// Decide which way it faces, and whether we can cull it or not
				*/
			#if SLOW_FCMP && !MULTI_FP_REG
				if(FLOAT_TO_LONG(pXPlane->d) > 0)
			#else
				if(pXPlane->d > 0.0f)
			#endif
				{
					/*
					// Read the "static global" variable that was set up when we
					// first decided to process this mesh
					//
					// This is a FORWARD PLANE (Jim has put something backwards)
					// reverse the normal to get it the right way round.
					*/
					if (keepAntiClock)
					{
						VecNegate (pXPlane->normal);
						pXPlane->flags = (int) pFace->PlaneData.flags | pf_reversed;
						pXPlane->d = -pXPlane->d;
						pXExtra->u32EdgeFlags = pFace->EdgeFlags ^ 0xFF;
					}
					else
					{
						/* 
						// cull back faces : Go to the next one
						*/
						continue;
					}
				}
				/*
				// Else is a reverse (anti clockwise) plane 
				// (Something else is back the front)
				*/
				else
				{
					if (keepClockwise)
					{
						pXExtra->u32EdgeFlags = pFace->EdgeFlags;
						pXPlane->flags = (int) pFace->PlaneData.flags;
					}
					else
					{
						/* cull back faces */
						continue;
					}
				}
			
			#endif

			/*
			// save the pointer to the original plane data... we will need
			// to access it again later, plus in the Transformed plane data as well.
			*/
			pXExtra->pOrigFace = pFace;
			pXPlane->pOriginalData = &pFace->PlaneData;

			/*
			// Compute the sabre params
			*/
			fCentreDotNormal = DotProd (pXPlane->normal, RCentre);
			fXBorder = pXPlane->normal[0] * xToCorner;
			fYBorder = pXPlane->normal[1] * yToCorner;

			if (sfabs (pXPlane->d) < 
				  (sfabs (fCentreDotNormal) + sfabs (fXBorder) + sfabs (fYBorder)))
			{
				DPF((DBG_VERBOSE, 
					"Perpendicular polygon - bodging up flat plane ..."));


				ASSERT(pVert->vWorldSpace[2] >= pProjMat->foregroundDistance);

#if (PCX2 || PCX2_003) && !FORCE_NO_FPU
				/* PCX2 has IEEE floating point support.
				 */
				pXPlane->f32C = (RCentre[2] / pVert->vWorldSpace[2]);
				pXPlane->f32A = 0.0f;
				pXPlane->f32B = 0.0f;
#else
				pXPlane->n32C =(sgl_int32)(RCentre[2] * FLOAT_TO_FIXED
									    / pVert->vWorldSpace[2]);

				SGL_TIME_SUSPEND(TRANSFORM_COMPUTE_SABRE_ALL_TIME)
				pXPlane->n32A = SFLOAT_20BIT_ZERO;
				pXPlane->n32B = SFLOAT_20BIT_ZERO;
				SGL_TIME_RESUME(TRANSFORM_COMPUTE_SABRE_ALL_TIME)
#endif

				/*
				// Save pointers to the texturing and shading data
				// (On a pentium this makes use of 
				// some of the cycles that are chewed up by the divide)
				// These 5 pointer assignment lines were originally AFTER
				// the if statement
				*/
				pXPlane->pShadingData= pShadingData;
				pXPlane->pTextureData= pTextureData;
				pXPlane->pPointsData= pPointsData;
				pXExtra++;
				NumProcessed++;

			}
			/*
			// Else this plane is well behaved
			*/
			else
			{
				float	fInvD;
				float	fA, fB;
			
				fInvD = fOverflowRescale / pXPlane->d;

				/*
				// Save pointers to the texturing and shading data
				// (On a pentium  this makes use of 
				// some of the cycles that are chewed up by the divide)
				// These 5 pointer assignment lines were originally AFTER
				// the if statement
				*/
				pXPlane->pShadingData= pShadingData;
				pXPlane->pTextureData= pTextureData;
				pXPlane->pPointsData= pPointsData;
				pXExtra++;
				NumProcessed++;

				fA = pXPlane->normal[0] * xPerPixel * fInvD;
				fB = pXPlane->normal[1] * yPerPixel * fInvD;

#if (PCX2 || PCX2_003) && !FORCE_NO_FPU
				pXPlane->f32A = fA;
				pXPlane->f32B = fB;
				pXPlane->f32C = (fCentreDotNormal - (fXBorder + fYBorder)) * fInvD;
#else
				SGL_TIME_SUSPEND(TRANSFORM_COMPUTE_SABRE_ALL_TIME)
				pXPlane->n32A = PackTo20Bit (fA);
				pXPlane->n32B = PackTo20Bit (fB);
				SGL_TIME_RESUME(TRANSFORM_COMPUTE_SABRE_ALL_TIME)

				pXPlane->n32C = (sgl_int32) ((fCentreDotNormal - (fXBorder + fYBorder)) 
										* (fInvD * FLOAT_TO_FIXED));
#endif
			}/* end else choose projection method*/


			/*
			// increment the pointers to the results
			*/
			pXPlane++;
#if PRE_CULL
		}
#endif
	}/*end for k*/


	/*
	// Save the incremented pointers back again
	*/
	*ppExtFace 		  = pFace;
	*ppExtShadingData = pShadingData;
	*ppExtTextureData = pTextureData;
	*ppExtPointsData  = pPointsData;
	

	/*
	// Return the number we are left with
	*/
	SGL_TIME_STOP(TRANSFORM_COMPUTE_SABRE_ALL_TIME)
	return NumProcessed;

}

/**************************************************************************
 * Function Name  : TransformAndComputeSabreParamsPartlyVisible  (LOCAL FUNCTION)
 * Inputs         : pFace     - pointer to start of an array of face data
 *					numFaces  - the number of faces to process
 *					pTransform - pointer to the transformation
 *	
 * Outputs        : pTransformedPlanes - the planes/faces after transformation
 *				    pExtras			   - the rest of the data needed for
 *
 * Returns		  : The number of planes/polys NOT rejected. (IE Not
 *					back-face culled) OR in front of the foreground planw
 *
 * Description	  : Like the TransformAndComputeSabreParams routine except
 *					it also checks if the faces are entirely in front of the
 *					foreground Z plane, or to one side etc,
 *					and rejects those that are.
 **************************************************************************/
static int TransformAndComputeSabreParamsPartlyVisible(const FACE **ppExtFace, 
												int	   numFaces,
							const TRANSFORM_STRUCT	  *pTransform,
										PXMESHEXTRA	   pXExtra,
							 TRANSFORMED_PLANE_STRUCT *pXPlane,
						const CONV_SHADING_STRUCT	  **ppExtShadingData,
						const CONV_TEXTURE_UNION	  **ppExtTextureData,
						const CONV_POINTS_STRUCT	  **ppExtPointsData)

{
	int NumProcessed;
	
	/*
	// To make the code simpler, rather than have two different
	// loops for processing normals: IE either using normal matrix
	// (ie either uniform or no scaling) or transpose of inverse
	// (when non uniform). Basically, since the non-uniform is
	// fairly rare, we will copy the inverse and transpose it, 
	// which means we can use the same code
	*/
	float const (*m)[4];
	float matcopy[3][4];
	PTRANSVERTEX pVert;

	const FACE	 				*pFace;
	const CONV_SHADING_STRUCT	*pShadingData;
	const CONV_TEXTURE_UNION	*pTextureData;
	const CONV_POINTS_STRUCT	*pPointsData;
	float foregroundDistance, invForegroundDistance, fOverflowRescale;
	float yToCorner, xToCorner, xPerPixel, yPerPixel;
	PROJECTION_MATRIX_STRUCT  * const pProjMat = RnGlobalGetProjMat ();
	sgl_vector RCentre;

	foregroundDistance = pProjMat->foregroundDistance;
	invForegroundDistance = pProjMat->invForegroundDistance;
	RCentre[0] = pProjMat->RCentre[0];
	RCentre[1] = pProjMat->RCentre[1];
	RCentre[2] = pProjMat->RCentre[2];
	yToCorner = pProjMat->yToCorner;
	xToCorner = pProjMat->xToCorner;
	fOverflowRescale = pProjMat->fOverflowRescale;
	xPerPixel = pProjMat->xPerPixel;
	yPerPixel = pProjMat->yPerPixel;

	SGL_TIME_START(TRANSFORM_COMPUTE_SABRE_PARTLY_TIME)

	/*
	// Initialise the number processed and kept, to 0 
	*/
	NumProcessed = 0;


	/*
	// De-reference the Face, shading etc pointers to get local
	// copies that we increment. At the end of the routine, we
	// store these incremented values back again
	//
	// It SHOULD be much quicker to manipulate the local copies, as
	// this should result in less indirection.
	*/
	pFace 		= *ppExtFace;
	pShadingData= *ppExtShadingData;
	pTextureData= *ppExtTextureData;
	pPointsData = *ppExtPointsData;
	

	/*
	// If the transform matrix is non uniform, then copy the inverse,
	// transposing it at the same time
	// This is not very common (hopefully :) )
	*/
	
	if ( (pTransform->scale_flag == arbitrary_scale) ||
	     (pTransform->has_neg_scaling))
	{
		int i,j;

		if (pTransform->has_neg_scaling)
		{
			float fScale = -1.0f;
		
			for(i= 0; i < 3; i ++)
			{
				for(j= 0; j < 3; j ++)
				{
					matcopy[j][i] = pTransform->inv[i][j] * (fScale);
				}
			}
		}
		else
	  	{
			for(i= 0; i < 3; i ++)
			{
				for(j= 0; j < 3; j ++)
				{
					matcopy[j][i] = pTransform->inv[i][j];
				}
			}
	  	}
		/*
		// Set up pointer to the copy
		*/
		m = (void*) matcopy;
	}
	/*
	// else just set the pointer at the usual matrix
	*/
	else
	{
		m = pTransform->mat;
	}

	/*
	// Step through the planes
	*/
	for(/*Nil*/;  numFaces != 0;  numFaces--, pFace++, 
								  pShadingData++, pTextureData++, pPointsData++)
	{
#if PRE_CULL
		if (pFace->EdgeFlags & FACE_VISIBLE)
		{
#endif
			PTRANSVERTEX pVa, pVb, pVc;
			int			 Offscreen;

			float	fCentreDotNormal;
			float	fXBorder;
			float	fYBorder;
			float tmp1, tmp2, tmp3;

			/*
			// First see if we can reject the face as being in front of the
			// front Z clipping plane, or to one side etc. Basically see if
			//  ALL of the vertices have a particular clip flag set
			//
			// This bit of code is likely to be dog slow because of all
			// the array indexing.
			*/
			pVa = gpXVertices + pFace->pnVertexIDs[0];
			pVb = gpXVertices + pFace->pnVertexIDs[1];
			pVc = gpXVertices + pFace->pnVertexIDs[2];

			Offscreen = pVa->Flags & pVb->Flags & pVc->Flags;

			if(pFace->nVertices == 4)
			{
				PTRANSVERTEX pVd;
				pVd = gpXVertices + pFace->pnVertexIDs[3];
				Offscreen &= pVd->Flags;
			}
			/*
			// Ok if they are all clipped the same way, get the next face
			*/
			if(Offscreen)
			{
				continue;
			}

			
			/*
			// Now go on with exactly the same processing as in the non Z clipped
			// routine.....
			//
			// Transform the normal
			*/
			#define Norm pFace->PlaneData.normal
			tmp1 = Norm[0]*m[0][0] + Norm[1]*m[0][1] + Norm[2]*m[0][2];

			tmp2 = Norm[0]*m[1][0] + Norm[1]*m[1][1] + Norm[2]*m[1][2];

			tmp3 = Norm[0]*m[2][0] + Norm[1]*m[2][1] + Norm[2]*m[2][2];

			#undef Norm
			pXPlane->normal[0] = tmp1;
			pXPlane->normal[1] = tmp2;
			pXPlane->normal[2] = tmp3;


			/*
			// Get a pointer to a transformed point on this face
			*/
			pVert = gpXVertices + pFace->pnVertexIDs[0];

			/*
			// Get the "D" value of the plane
			*/
			pXPlane->d = DotProd(pXPlane->normal, pVert->vWorldSpace);

			#if PRE_CULL

				/*
				// Decide which way it faces, and whether we can cull it or not
				*/
			#if SLOW_FCMP && !MULTI_FP_REG
				if(FLOAT_TO_LONG(pXPlane->d) > 0)
			#else
				if(pXPlane->d > 0.0f)
			#endif
				{
					/*
					// This is a FORWARD PLANE (something is back-the-front)
					// reverse the normal to get it the right way round.
					*/
					VecNegate (pXPlane->normal);
					pXPlane->d = -pXPlane->d;
					/*
					// Reverse the edges, by flipping the flag
					*/
					pXExtra->u32EdgeFlags = pFace->EdgeFlags ^ 0xFF;
					pXPlane->flags = (int) pFace->PlaneData.flags | pf_reversed;
				}
				/*
				// Else is a reverse (anti clockwise) plane 
				// (Something else is back the front)
				*/
				else
				{
					pXExtra->u32EdgeFlags = pFace->EdgeFlags;
					pXPlane->flags = (int) pFace->PlaneData.flags;
				}

			#else

				/*
				// Decide which way it faces, and whether we can cull it or not
				*/
			#if SLOW_FCMP && !MULTI_FP_REG
				if(FLOAT_TO_LONG(pXPlane->d) > 0)
			#else
				if(pXPlane->d > 0.0f)
			#endif
				{
					/*
					// Read the "static global" variable that was set up when we
					// first decided to process this mesh
					//
					// This is a FORWARD PLANE 
					// reverse the normal to get it the right way round.
					*/
					if (keepAntiClock)
					{
						VecNegate (pXPlane->normal);

						pXPlane->flags = (int) pFace->PlaneData.flags | pf_reversed;
						pXPlane->d = -pXPlane->d;
						/*
						// Reverse the edges, by flipping the flag
						*/
						pXExtra->u32EdgeFlags = pFace->EdgeFlags ^ 0xFF;
					}
					else
					{
						/* 
						// cull back faces : Go to the next one
						*/
						continue;
					}
				}
				/*
				// Else is a reverse (anti clockwise) plane 
				// (Something else is back the front)
				*/
				else
				{
					if (keepClockwise)
					{
						pXExtra->u32EdgeFlags = pFace->EdgeFlags;
						pXPlane->flags = (int) pFace->PlaneData.flags;
					}
					else
					{
						/* cull back faces */
						continue;
					}
				}
			
			#endif

			/*
			// save the pointer to the original plane data... we will need
			// to access it again later, plus in the Transformed plane data as well.
			*/
			pXExtra->pOrigFace = pFace;
			pXPlane->pOriginalData = &pFace->PlaneData;

			/*
			// Compute the sabre params
			*/
			fCentreDotNormal = DotProd (pXPlane->normal, RCentre);
			fXBorder = pXPlane->normal[0] * xToCorner;
			fYBorder = pXPlane->normal[1] * yToCorner;

			if (sfabs (pXPlane->d) < 
				  (sfabs (fCentreDotNormal) + sfabs (fXBorder) + sfabs (fYBorder)))
			{
				float	fInvZdist;

				DPF((DBG_VERBOSE,
					 "Perpendicular polygon - bodging up flat plane ..."));


				if(pVert->vWorldSpace[2] < foregroundDistance)
				{
					fInvZdist = invForegroundDistance;
				}
				else
				{
					fInvZdist = 1.0f/pVert->vWorldSpace[2];
				}
#if (PCX2 || PCX2_003) && !FORCE_NO_FPU
				/* PCX2 has IEEE floating point support.
				 */
				pXPlane->f32C = (RCentre[2] * fInvZdist);
				pXPlane->f32A = 0.0f;
				pXPlane->f32B = 0.0f;
#else
				pXPlane->n32C 
				  =(sgl_int32)(RCentre[2] * FLOAT_TO_FIXED * fInvZdist);

				pXPlane->n32A = SFLOAT_20BIT_ZERO;
				pXPlane->n32B = SFLOAT_20BIT_ZERO;
#endif

				/*
				// Save pointers to the texturing and shading data
				// (On a pentium this makes use of 
				// some of the cycles that are chewed up by the divide)
				// These 5 pointer assignment lines were originally AFTER
				// the if statement
				*/
				pXPlane->pShadingData= pShadingData;
				pXPlane->pTextureData= pTextureData;
				pXPlane->pPointsData= pPointsData;
				pXExtra++;
				NumProcessed++;
			}
			/*
			// Else this plane is well behaved
			*/
			else
			{
				float	fInvD;
				float	fA, fB;

				fInvD = fOverflowRescale / pXPlane->d;

				/*
				// Save pointers to the texturing and shading data
				// (On a pentium this makes use of 
				// some of the cycles that are chewed up by the divide)
				// These 5 pointer assignment lines were originally AFTER
				// the if statement
				*/
				pXPlane->pShadingData= pShadingData;
				pXPlane->pTextureData= pTextureData;
				pXPlane->pPointsData= pPointsData;
				pXExtra++;
				NumProcessed++;

				fA = pXPlane->normal[0] * xPerPixel * fInvD;
				fB = pXPlane->normal[1] * yPerPixel * fInvD;

#if (PCX2 || PCX2_003) && !FORCE_NO_FPU
				pXPlane->f32A = fA;
				pXPlane->f32B = fB;
				pXPlane->f32C = ((fCentreDotNormal - (fXBorder + fYBorder)) * fInvD);
#else
				SGL_TIME_SUSPEND(TRANSFORM_COMPUTE_SABRE_PARTLY_TIME)
				pXPlane->n32A = PackTo20Bit (fA);
				pXPlane->n32B = PackTo20Bit (fB);
				SGL_TIME_RESUME(TRANSFORM_COMPUTE_SABRE_PARTLY_TIME)

				pXPlane->n32C = (sgl_int32) ((fCentreDotNormal - (fXBorder + fYBorder)) 
										* fInvD * FLOAT_TO_FIXED);

#endif

			}/* end else choose projection method*/

			/*
			// increment the pointers to the results
			*/
			pXPlane++;
#if PRE_CULL
		}
#endif
	}/*end for k*/
													

	/*
	// Save the incremented pointers back again
	*/
	*ppExtFace 		  = pFace;
	*ppExtShadingData = pShadingData;
	*ppExtTextureData = pTextureData;
	*ppExtPointsData  = pPointsData;


	/*
	// Return the number we are left with
	*/
	SGL_TIME_STOP(TRANSFORM_COMPUTE_SABRE_PARTLY_TIME)

	return NumProcessed;

}

#if PRE_CULL

static int PreprocessFacesVisible (int numVertices, 
								   int numEdges, 
								   int numFaces, 
								   VERTEX *pVertices, 
								   FACE   *pFace, 
								   const TRANSFORM_STRUCT	*pTransform)
{
	#if ICL

	static float 	DminusEdotN;
	static sgl_uint32	iptr;

	__asm
	{
			push	esi
			push	edi
			push	ebx

			mov		esi, gpXVertexIndex
			mov		edi, gpXEdgeIndex

			mov		ebx, eax
			and		ebx, 0x7FFFFFFF

			/* init vertex indices */

			mov		ecx, numVertices
			shr		ecx, 2
V0:			dec		ecx
			mov		DWORD PTR [esi+ecx*4], ebx
			jnz		V0

			/* init edge indices */

			mov		ecx, numEdges
			shr		ecx, 2
E0:			dec		ecx
			mov		DWORD PTR [edi+ecx*4], ebx
			jnz		E0

			/* if no culling is happening, exit */

			cmp		eax, 0xFFFFFFFF
			je		PFVExit

			mov		edx, numFaces
			mov		ebx, pFace
			mov		ecx, pTransform
			lea		ecx, [ecx].inv
			mov		iptr, ecx

PFVLoop:	/* load EyePointInObjectSpace */

			mov		ecx, iptr
			fld		DWORD PTR 12[ecx]
			fld		DWORD PTR 12[ecx+16]
			fld		DWORD PTR 12[ecx+32]
						
			/* get object space coordinates for vertex 0 */

			mov		ecx, 0[ebx].pnVertexIDs
			lea		ecx, [ecx*4]
			lea		ecx, [ecx+ecx*2]
			add		ecx, pVertices
			fld		DWORD PTR 0[ecx].vVertex
			fld		DWORD PTR 4[ecx].vVertex
			fld		DWORD PTR 8[ecx].vVertex
			fxch	ST(2)

			/* get vertex dot normal (d) */

			lea		ecx, [ebx].PlaneData.normal
			fmul	DWORD PTR 0[ecx]
			fxch	ST(1)
			fmul	DWORD PTR 4[ecx]
			fxch	ST(2)
			fmul	DWORD PTR 8[ecx]
			fxch	ST(2)
			faddp	ST(1), ST
			faddp	ST(1), ST

			fxch	ST(3)

			/* get eye dot normal */

			fmul	DWORD PTR 0[ecx]
			fxch	ST(2)
			fmul	DWORD PTR 4[ecx]
			fxch	ST(1)
			fmul	DWORD PTR 8[ecx]
			fxch	ST(2)
			faddp	ST(1), ST
			faddp	ST(1), ST

			/* get d - e.n */

			fsubrp	ST(1), ST
			fstp	DWORD PTR [DminusEdotN]

			/* see if face is culled */

			mov		ecx, DWORD PTR [DminusEdotN]
			and		ecx, 0x01000000UL + 0x7F000000UL
			xor		ecx, CullMode
			jz		NotVisible							
			or		BYTE PTR [ebx].EdgeFlags, 0x80

			/* set the vertex and edge indices to 1 for visible */

			mov		eax, 0[ebx].pnVertexIDs
			mov		ecx, 0[ebx].pnEdgeIDs
			mov		BYTE PTR [esi+eax], 1
			mov		BYTE PTR [edi+ecx], 1
			mov		eax, 4[ebx].pnVertexIDs
			mov		ecx, 4[ebx].pnEdgeIDs
			mov		BYTE PTR [esi+eax], 1
			mov		BYTE PTR [edi+ecx], 1
			mov		eax, 8[ebx].pnVertexIDs
			mov		ecx, 8[ebx].pnEdgeIDs
			mov		BYTE PTR [esi+eax], 1
			mov		BYTE PTR [edi+ecx], 1

			cmp		DWORD PTR [ebx].nVertices, 4
			jne		NotFourV
			
			mov		eax, 12[ebx].pnVertexIDs
			mov		ecx, 12[ebx].pnEdgeIDs
			mov		BYTE PTR [esi+eax], 1
			mov		BYTE PTR [edi+ecx], 1

NotFourV:	add		ebx, SIZE FACE
			dec		edx
			jnz		PFVLoop
			jmp		PFVExit

NotVisible:	and		BYTE PTR [ebx].EdgeFlags, 0x7F
			add		ebx, SIZE FACE
			dec		edx
			jnz		PFVLoop		

PFVExit:	pop		ebx
			pop		edi
			pop		esi
	}

	#else

		sgl_vector 	EyePointInObjectSpace;
		float		d, EyeDotNormal;
		sgl_uint8		*p;
		sgl_uint32		*p32;
		
		EyePointInObjectSpace[0] = pTransform->inv[0][3];
		EyePointInObjectSpace[1] = pTransform->inv[1][3];
		EyePointInObjectSpace[2] = pTransform->inv[2][3];
		
		p = (sgl_uint8 *) gpXVertexIndex;
		
		while (numVertices--)
		{
			*(p++) = 0;
		}

		p = (sgl_uint8 *) gpXEdgeIndex;
		
		while (numEdges--)
		{
			*(p++) = 0;
		}

		for (/* nothing */; numFaces; numFaces--, pFace++)
		{
			d = 			DotProd (pFace->PlaneData.normal, pVertices[pFace->pnVertexIDs[0]].vVertex);
			EyeDotNormal = 	DotProd (pFace->PlaneData.normal, EyePointInObjectSpace);
			
			/* use bitwise ops to reduce branching */
			
			if (((d > EyeDotNormal) & (keepAntiClock != 0)) |
			    ((d < EyeDotNormal) & (keepClockwise != 0)))
			{
				pFace->EdgeFlags |= FACE_VISIBLE;

				if (pFace->nVertices == 3)
				{
					gpXVertexIndex[pFace->pnVertexIDs[0]] = 1;
					gpXVertexIndex[pFace->pnVertexIDs[1]] = 1;
					gpXVertexIndex[pFace->pnVertexIDs[2]] = 1;

					gpXEdgeIndex[pFace->pnEdgeIDs[0]] = 1;
					gpXEdgeIndex[pFace->pnEdgeIDs[1]] = 1;
					gpXEdgeIndex[pFace->pnEdgeIDs[2]] = 1;
				}
				else
				{
					gpXVertexIndex[pFace->pnVertexIDs[0]] = 1;
					gpXVertexIndex[pFace->pnVertexIDs[1]] = 1;
					gpXVertexIndex[pFace->pnVertexIDs[2]] = 1;
					gpXVertexIndex[pFace->pnVertexIDs[3]] = 1;

					gpXEdgeIndex[pFace->pnEdgeIDs[0]] = 1;
					gpXEdgeIndex[pFace->pnEdgeIDs[1]] = 1;
					gpXEdgeIndex[pFace->pnEdgeIDs[2]] = 1;
					gpXEdgeIndex[pFace->pnEdgeIDs[3]] = 1;
				}
			}
			else
			{
				pFace->EdgeFlags &= ~FACE_VISIBLE;
			}
		}

	#endif
}

#endif

/**************************************************************************
 * Function Name  : ProcessFacesVisible   (LOCAL FUNCTION)
 * Inputs         : pFace     - pointer to start of an array of input face data
 *					numFaces  - the number of faces to process
 *					pTransform - pointer to the transformation
 *	
 * Outputs        : pTransformedPlanes - the planes/faces after transformation
 *				    pExtras			   - the rest of the data needed for
 *
 * Returns		  : The number of planes/polys NOT rejected. (IE Not
 *					back-face culled)
 *
 *					This routine call the Transform and Compute Sabre
 *					parameters routine, and then determines the regions
 *					each of the  faces are in.
 *
 *					IT ASSUMES all the faces are COMPLETELY ON SCREEN.
 **************************************************************************/

#define CHOOSE_MINMAX(a,b,c) if(a<b)b=a;else if(a>c)c=a;

static int ProcessFacesVisible (const FACE **ppFace, 
									  int	 numFaces,
					const TRANSFORM_STRUCT	*pTransform,
						  PXMESHEXTRA		 pXExtra,
				TRANSFORMED_PLANE_STRUCT	*pXPlane,
				const CONV_SHADING_STRUCT  **ppShadingData,
				const CONV_TEXTURE_UNION   **ppTextureData,
				const CONV_POINTS_STRUCT   **ppPointsData)
									  
{
	int k;
	#if 0 /*defined (ICL)*/
		/* 
			Otherwise NumRemaining is optimised into a reg - we want it
			in RAM!
		*/
		static int NumRemaining;
	#else
		int NumRemaining;
	#endif
	sgl_int32 n32MinX, n32MinY, n32MaxX, n32MaxY;
	PTRANSVERTEX   pVa, pVb, pVc, pVd;
	
	/*
	// First of all, transform, cull, and calculate sabre parameters for
	// the faces.
	*/
	NumRemaining = TransformAndComputeSabreParamsVisible(ppFace, numFaces, 
				   pTransform, pXExtra, pXPlane,
				   ppShadingData, ppTextureData, ppPointsData);


	ASSERT((NumRemaining >= 0) && (NumRemaining <= numFaces));

	SGL_TIME_START(TRANSFORM_FACES_ALL_TIME)

	#if 0 /*defined (ICL)*/
		
		__asm
		{
			cmp		DWORD PTR [NumRemaining], 0
			jle		PFV_exit

			push	ebp
			push	esi
			push	edi
			push	ebx

//			int 3
			
			// esi: pXExtra
			// edi: pXPlane
			// ebp: pFace
			// eax: -
			// ebx: -
			// ecx: -
			// edx: -
			
			mov		esi, pXExtra
			mov		edi, pXPlane

PFV_loop:

			mov		ebp, DWORD PTR [esi].pOrigFace
			
			mov		ecx, DWORD PTR 0[ebp].pnVertexIDs
			mov		edx, DWORD PTR [gpXVertices]
			lea		ecx, DWORD PTR [ecx+ecx*4]
			lea		ecx, DWORD PTR [edx+ecx*8]

			// pXPlane->projRepPoint[0] = pVa->vScreenSpace[0];
			// pXPlane->projRepPoint[1] = pVa->vScreenSpace[1];

			mov		ebx, DWORD PTR 0[ecx].vScreenSpace
			mov		edx, DWORD PTR 4[ecx].vScreenSpace
			mov		DWORD PTR 0[edi + 48], ebx
			mov		DWORD PTR 4[edi + 48], edx
			
			// VecCopy (pVa->vWorldSpace, pXPlane->repPnt);

			mov		eax, DWORD PTR 0[ecx].vWorldSpace
			mov		ebx, DWORD PTR 4[ecx].vWorldSpace
			mov		edx, DWORD PTR 8[ecx].vWorldSpace
			mov		DWORD PTR 0[edi].repPnt, eax
			mov		DWORD PTR 4[edi].repPnt, ebx
			mov		DWORD PTR 8[edi].repPnt, edx

			// VecCopy (pVa->pvVertex,    pFace->PlaneData.rep_point);

			mov		ebx, DWORD PTR [ecx].pvVertex
			fld		DWORD PTR 0[ebx]
			fld		DWORD PTR 4[ebx]
			fld		DWORD PTR 8[ebx]
			fxch	ST(2)
			fstp	DWORD PTR 0[ebp].PlaneData.rep_point
			fstp	DWORD PTR 4[ebp].PlaneData.rep_point
			fstp	DWORD PTR 8[ebp].PlaneData.rep_point
			
			mov		al, BYTE PTR 0[ecx].nRegion
			mov		bl, BYTE PTR 4[ecx].nRegion

			mov		ecx, DWORD PTR 4[ebp].pnVertexIDs
			mov		edx, DWORD PTR [gpXVertices]
			lea		ecx, DWORD PTR [ecx+ecx*4]
			lea		ecx, DWORD PTR [edx+ecx*8]
			mov		ah, BYTE PTR 0[ecx].nRegion
			mov		bh, BYTE PTR 4[ecx].nRegion

			cmp		al, ah
			jl		_1_1
;			xchg	al, ah
			mov		dl, al
			mov		al, ah
			mov		ah, dl
_1_1:		cmp		bl, bh
			jl		_1_2
;			xchg	bl, bh
			mov		dl, bl
			mov		bl, bh
			mov		bh, dl
_1_2:
			mov		ecx, DWORD PTR 8[ebp].pnVertexIDs
			mov		edx, DWORD PTR [gpXVertices]
			lea		ecx, DWORD PTR [ecx+ecx*4]
			lea		ecx, DWORD PTR [edx+ecx*8]
			mov		dl, BYTE PTR 0[ecx].nRegion
			mov		dh, BYTE PTR 4[ecx].nRegion

			cmp		al, dl
			jl		_2_1
			mov		al, dl
			jmp		_2_2
_2_1:		cmp		ah, dl
			jg		_2_2
			mov		ah, dl			
_2_2:		cmp		bl, dh
			jl		_3_1
			mov		bl, dh
			jmp		_3_2
_3_1:		cmp		bh, dh
			jg		_3_2
			mov		bh, dh			
_3_2:
			// pXExtra->pE[0] = gpXEdges + pFace->pnEdgeIDs[0];
			// pXExtra->pE[1] = gpXEdges + pFace->pnEdgeIDs[1];
			// pXExtra->pE[2] = gpXEdges + pFace->pnEdgeIDs[2];

			mov		edx, DWORD PTR gpXEdges
			mov		ecx, DWORD PTR 0[ebp].pnEdgeIDs
			shl		ecx, 4
			add		ecx, edx
			mov		DWORD PTR 0[esi].pE, ecx
			mov		ecx, DWORD PTR 4[ebp].pnEdgeIDs
			shl		ecx, 4
			add		ecx, edx
			mov		DWORD PTR 4[esi].pE, ecx
			mov		ecx, DWORD PTR 8[ebp].pnEdgeIDs
			shl		ecx, 4
			add		ecx, edx
			mov		DWORD PTR 8[esi].pE, ecx

			// pXExtra->nEdges = pFace->nVertices;
			
			mov		ecx, DWORD PTR [ebp].nVertices
			mov		DWORD PTR [esi].nEdges, ecx

			// 4 sides?

			cmp		ecx, 4
			jne		_5_2

			// pXExtra->pE[3] = gpXEdges + pFace->pnEdgeIDs[3];

			mov		ecx, DWORD PTR 12[ebp].pnEdgeIDs
			shl		ecx, 4
			add		ecx, edx
			mov		DWORD PTR 12[esi].pE, ecx

			mov		ecx, DWORD PTR 12[ebp].pnVertexIDs
			mov		edx, DWORD PTR [gpXVertices]
			lea		ecx, DWORD PTR [ecx+ecx*4]
			lea		ecx, DWORD PTR [edx+ecx*8]
			mov		dl, BYTE PTR 0[ecx].nRegion
			mov		dh, BYTE PTR 4[ecx].nRegion

			cmp		al, dl
			jl		_4_1
			mov		al, dl
			jmp		_4_2
_4_1:		cmp		ah, dl
			jg		_4_2
			mov		ah, dl			
_4_2:		cmp		bl, dh
			jl		_5_1
			mov		bl, dh
			jmp		_5_2
_5_1:		cmp		bh, dh
			jg		_5_2
			mov		bh, dh			
_5_2:		

			// pXExtra->ZClipped = FALSE;

			xor		ecx, ecx
			mov		DWORD PTR [esi].ZClipped, ecx

			// pXExtra->RegionsRect.FirstXRegion = n32MinX;
			// pXExtra->RegionsRect.FirstYRegion = n32MinY;
			// pXExtra->RegionsRect.LastXRegion  = n32MaxX;
			// pXExtra->RegionsRect.LastYRegion  = n32MaxY;

			mov		BYTE PTR 4[esi + 0], al
			mov		BYTE PTR 4[esi + 4], ah
			mov		BYTE PTR 4[esi + 8], bl
			mov		BYTE PTR 4[esi + 12], bh

			// increment pointers and loop

			add		esi, SIZE XMESHEXTRA
			add		edi, SIZE TRANSFORMED_PLANE_STRUCT
			dec		DWORD PTR [NumRemaining]
			jnz		PFV_loop
			
			pop		ebx
			pop		edi
			pop		esi
			pop		ebp

PFV_exit:

		}

	#else

		/*
		// Step through the remaining faces determining the regions of the screen
		// they lie in
		*/
		for(k = NumRemaining; k != 0; k--, pXPlane++, pXExtra++)
		{
			const FACE *pFace;

			/*
			// Get access to the ORIGINAL face
			*/
			pFace = pXExtra->pOrigFace;

			pXExtra->nEdges = pFace->nVertices;

			/*
			// Check that we have the correct number of vertices
			*/
			ASSERT ((pFace->nVertices == 3) || (pFace->nVertices == 4));

			/*
			// Work out the regions the face covers
			//
			// First get pointers to the first three vertices
			*/
			pVa = gpXVertices + pFace->pnVertexIDs[0];
			pVb = gpXVertices + pFace->pnVertexIDs[1];
			pVc = gpXVertices + pFace->pnVertexIDs[2];

			/* 
			// copy projected rep point coords. All points are visible,
			// so the first vertex will do nicely.
			*/
			{
				/*
				// Use temporary variables because this will convince
				// a compiler that pVa->vScreenSpace and pXPlane->projRepPoint
				// dont overlap, and hence can be done in parallel
				*/
				float tmp1, tmp2;

				tmp1 = pVa->vScreenSpace[0];
				tmp2 = pVa->vScreenSpace[1];
				pXPlane->projRepPoint[0] = tmp1; 
				pXPlane->projRepPoint[1] = tmp2;
			}

			/*
			// Set up a world space rep point (which would be used
			// by the shading routine
			//
			// OPTIMISATION. The pFace->PlaneData.rep_point should
			// be set when the mesh face is created/modified. SJF
			*/
			VecCopy (pVa->pvVertex, (float*) pFace->PlaneData.rep_point);
			VecCopy (pVa->vWorldSpace, pXPlane->repPnt);


			/*
			// Get the extremes of the first 2 points
			*/
			if(pVa->nRegion[0] < pVb->nRegion[0])
			{
				n32MinX	= pVa->nRegion[0];
				n32MaxX	= pVb->nRegion[0];
			}
			else
			{
				n32MinX	= pVb->nRegion[0];
				n32MaxX	= pVa->nRegion[0];
			}

			if(pVa->nRegion[1] < pVb->nRegion[1])
			{
				n32MinY = pVa->nRegion[1];
				n32MaxY = pVb->nRegion[1];
			}
			else
			{
				n32MinY = pVb->nRegion[1];
				n32MaxY = pVa->nRegion[1];
			}

			/*
			// Look at the third point
			*/
			CHOOSE_MINMAX (pVc->nRegion[0], n32MinX, n32MaxX);
			CHOOSE_MINMAX (pVc->nRegion[1], n32MinY, n32MaxY);

			/*
			// Save the pointers to the first 3 transformed edges
			*/
			pXExtra->pE[0] = gpXEdges + pFace->pnEdgeIDs[0];
			pXExtra->pE[1] = gpXEdges + pFace->pnEdgeIDs[1];
			pXExtra->pE[2] = gpXEdges + pFace->pnEdgeIDs[2];

			/*
			// Is there a 4th point
			*/		
			if (pFace->nVertices == 4)
			{
				pVd = gpXVertices + pFace->pnVertexIDs[3];

				CHOOSE_MINMAX (pVd->nRegion[0], n32MinX, n32MaxX);
				CHOOSE_MINMAX (pVd->nRegion[1], n32MinY, n32MaxY);

				/*
				// At the same time store the 4th edge
				*/
				pXExtra->pE[3] = gpXEdges + pFace->pnEdgeIDs[3];

			}/*end if 4th pointers */
			
			/*
			// Store the results
			*/		
			pXExtra->RegionsRect.FirstXRegion = n32MinX;
			pXExtra->RegionsRect.FirstYRegion = n32MinY;
			pXExtra->RegionsRect.LastXRegion  = n32MaxX;
			pXExtra->RegionsRect.LastYRegion  = n32MaxY;
			

			/* check they aren't back to front */
			ASSERT ( n32MinX <= n32MaxX);
			ASSERT ( n32MinY <= n32MaxY);

			/* check face is on screen */
			DPF((DBG_MESSAGE,"RnGlobalProjMatRegionBoxOnScreen = %d",
				 RnGlobalProjMatRegionBoxOnScreen(&(pXExtra->RegionsRect))));

			/*
			// Because of the crap plane limit on Midas3, we need
			// sort sort of heuristic for rejecting some faces.
			// Look at the "length" of the edge AB
			*/
			#if ISPTSP
				pXExtra->ApproxSize = 
					sfabs(pVa->vScreenSpace[0] - pVc->vScreenSpace[0]) +
					sfabs(pVa->vScreenSpace[1] - pVc->vScreenSpace[1]);
			#endif

			/*
			// For the cases when this is translucent, give a 1/Z depth
			// Note that this is a waste WRT divisions... but
			// this stuff will all be re-written for version 3.1 of SGL
			*/
			if(pFace->nVertices == 4)
			{
			  pXExtra->NearZ = 4.0f /
				(pVa->vWorldSpace[2]+pVb->vWorldSpace[2]+
				 pVc->vWorldSpace[2]+pVd->vWorldSpace[2]);
			}
			else
			{
			  pXExtra->NearZ = 3.0f /
				(pVa->vWorldSpace[2]+pVb->vWorldSpace[2]+
				 pVc->vWorldSpace[2]);
			}


			/*
			// It's not Z clipped.
			*/
			pXExtra->ZClipped = FALSE;

		}/*end for k*/
	

	#endif

	SGL_TIME_STOP(TRANSFORM_FACES_ALL_TIME)

	return NumRemaining;
}


/**************************************************************************
 * Function Name  : ProcessFacesPartlyVisible (LOCAL FUNCTION)
 * Inputs         : pFace     - pointer to start of an array of input face data
 *					numFaces  - the number of faces to process
 *					pTransform - pointer to the transformation
 *	
 * Outputs        : pTransformedPlanes - the planes/faces after transformation
 *				    pExtras			   - the rest of the data needed for
 *
 * Returns		  : The number of planes/polys NOT rejected. (IE Not
 *					back-face culled)
 *
 *					As for ProcessFacesVisible, except the faces do NOT
 *					have to be onscreen, and may go through the foreground
 *					clipping plane
 **************************************************************************/


static int ProcessFacesPartlyVisible (const FACE **ppFace,
									  int	 numFaces,
					const TRANSFORM_STRUCT	*pTransform,
						  PXMESHEXTRA	     pXExtra,
				TRANSFORMED_PLANE_STRUCT	*pXPlane,
				const CONV_SHADING_STRUCT	**ppShadingData,
				const CONV_TEXTURE_UNION	**ppTextureData,
				const CONV_POINTS_STRUCT	**ppPointsData)
									  
{
	int k;
	int AnyClipped;
	int NumRemaining;
	sgl_int32 n32MinX, n32MinY, n32MaxX, n32MaxY;
	PTRANSVERTEX   pVa, pVb, pVc, pVd;
	float invForegroundDistance;
	PROJECTION_MATRIX_STRUCT  * const pProjMat = RnGlobalGetProjMat ();

	invForegroundDistance = pProjMat->invForegroundDistance;
	
	/*
	// First of all, transform, cull, and calculate sabre parameters for
	// the faces.
	*/
	NumRemaining = TransformAndComputeSabreParamsPartlyVisible(ppFace, numFaces, 
								pTransform,	pXExtra, pXPlane,
								ppShadingData, ppTextureData, ppPointsData);

	ASSERT((NumRemaining >= 0) && (NumRemaining <= numFaces));

	SGL_TIME_START(TRANSFORM_FACES_PARTLY_TIME)

	/*
	// Step through the remaining faces determining the regions of the screen
	// they lie in
	*/
	for(k = NumRemaining; k != 0; k--, pXPlane++, pXExtra++)
	{
		const FACE *pFace;

		/*
		// Get access to the ORIGINAL face
		*/
		pFace = pXExtra->pOrigFace;

		pXExtra->nEdges = pFace->nVertices;

		/*
		// Check that we have the correct number of vertices
		*/
		ASSERT ((pFace->nVertices == 3) || (pFace->nVertices == 4));


		/*
		// First get pointers to the first three vertices, and the
		// 4th if this is a quad.
		*/
		pVa = gpXVertices + pFace->pnVertexIDs[0];
		pVb = gpXVertices + pFace->pnVertexIDs[1];
		pVc = gpXVertices + pFace->pnVertexIDs[2];

		/*
		// Store the pointers to the edges 
		*/
		pXExtra->pE[0] = gpXEdges + pFace->pnEdgeIDs[0];
		pXExtra->pE[1] = gpXEdges + pFace->pnEdgeIDs[1];
		pXExtra->pE[2] = gpXEdges + pFace->pnEdgeIDs[2];

		/*
		// Also see if any of the vertices were clipped.
		*/

		AnyClipped = pVa->Flags | pVb->Flags | pVc->Flags;

		if(pFace->nVertices == 4)
		{
			pVd = gpXVertices + pFace->pnVertexIDs[3];
			AnyClipped |=  pVd->Flags;
			
			/*
			// save pointer to the 4th edge
			*/
			pXExtra->pE[3] = gpXEdges + pFace->pnEdgeIDs[3];
		}

		/*///////////////////////
		// If the face is entirely on screen, handle it in
		// exactly the same way as the All visible code
		/////////////////////// */
		if((AnyClipped & CLIPPED_FLAGS)==0)
		{
			/* 
			// copy projected rep point coords. Any vertex is valid,	
			// so choose the first.
			*/
			pXPlane->projRepPoint[0] = pVa->vScreenSpace[0];
			pXPlane->projRepPoint[1] = pVa->vScreenSpace[1];


			/*
			// Set up a world space rep point (which would be used
			// by the shading routine
			//
			// OPTIMISATION. The pFace->PlaneData.rep_point should
			// be set when the mesh face is created/modified. SJF
			*/
			VecCopy (pVa->pvVertex, (float*) pFace->PlaneData.rep_point);
			VecCopy (pVa->vWorldSpace,	pXPlane->repPnt);
			
			/*
			// Work out the regions the face covers
			//
			// Get the extremes of the first 2 points
			*/
			if(pVa->nRegion[0] < pVb->nRegion[0])
			{
				n32MinX	= pVa->nRegion[0];
				n32MaxX	= pVb->nRegion[0];
			}
			else
			{
				n32MinX	= pVb->nRegion[0];
				n32MaxX	= pVa->nRegion[0];
			}

			if(pVa->nRegion[1] < pVb->nRegion[1])
			{
				n32MinY = pVa->nRegion[1];
				n32MaxY = pVb->nRegion[1];
			}
			else
			{
				n32MinY = pVb->nRegion[1];
				n32MaxY = pVa->nRegion[1];
			}

			/*
			// Look at the third point
			*/
			CHOOSE_MINMAX (pVc->nRegion[0], n32MinX, n32MaxX);
			CHOOSE_MINMAX (pVc->nRegion[1], n32MinY, n32MaxY);
	
			/*
			// Is there a 4th point
			*/		
			if (pFace->nVertices == 4)
			{
				CHOOSE_MINMAX (pVd->nRegion[0], n32MinX, n32MaxX);
				CHOOSE_MINMAX (pVd->nRegion[1], n32MinY, n32MaxY);
			}/*end if 4th pointers */
		
			/*
			// Store the results
			*/		
			pXExtra->RegionsRect.FirstXRegion = n32MinX;
			pXExtra->RegionsRect.FirstYRegion = n32MinY;
			pXExtra->RegionsRect.LastXRegion  = n32MaxX;
			pXExtra->RegionsRect.LastYRegion  = n32MaxY;
		

			/* check they aren't back to front */
			ASSERT ( n32MinX <= n32MaxX);
			ASSERT ( n32MinY <= n32MaxY);

			/* check face is on screen */
#if DEBUG
			if(!RnGlobalProjMatRegionBoxOnScreen(&(pXExtra->RegionsRect)))
			{
				DPF ((DBG_MESSAGE, "RnGlobalProjMatRegionBoxOnScreen Failed\n"));
			}
#endif
			/*
			// Because of the crap plane limit on Midas3, we need
			// sort sort of heuristic for rejecting some faces.
			// Look at the "length" of the edge AB
			*/
			#if ISPTSP
				pXExtra->ApproxSize = 
					sfabs(pVa->vScreenSpace[0] - pVc->vScreenSpace[0]) +
					sfabs(pVa->vScreenSpace[1] - pVc->vScreenSpace[1]);
			#endif

			/*
			// For the cases when this is translucent, give a 1/Z depth
			// Note that this is a waste WRT divisions... but
			// this stuff will all be re-written for version 3.1 of SGL
			*/
			if(pFace->nVertices == 4)
			{
			  pXExtra->NearZ = 4.0f /
				(pVa->vWorldSpace[2]+pVb->vWorldSpace[2]+
				 pVc->vWorldSpace[2]+pVd->vWorldSpace[2]);
			}
			else
			{
			  pXExtra->NearZ = 3.0f /
				(pVa->vWorldSpace[2]+pVb->vWorldSpace[2]+
				 pVc->vWorldSpace[2]);
			}

			/*
			// It's not Z clipped.
			*/
			pXExtra->ZClipped = FALSE;
		}

		/*///////////////////////
		// ELSE if any of the points were on the wrong side of 
		// the law ....
		/////////////////////// */
		else if(AnyClipped & CLIPPED_Z)
		{
			/*
			// OK, face goes through the foreground plane, we will use
			// the bounding box routine to find out which regions the
			// face goes in. This is not blindingly efficient, but
			// hey, this should crop up too often.
			*/
			BBOX_MINMAX_STRUCT BBox;

			DPF ((DBG_VERBOSE, "Using behind the eye region bbox method ..."));
			
			VecCopy (pVa->vWorldSpace, BBox.boxMin);
			VecCopy (pVa->vWorldSpace, BBox.boxMax);
			
			CHOOSE_MINMAX (pVb->vWorldSpace[0], BBox.boxMin[0], BBox.boxMax[0]);
			CHOOSE_MINMAX (pVb->vWorldSpace[1], BBox.boxMin[1], BBox.boxMax[1]);
			CHOOSE_MINMAX (pVb->vWorldSpace[2], BBox.boxMin[2], BBox.boxMax[2]);

			CHOOSE_MINMAX (pVc->vWorldSpace[0], BBox.boxMin[0], BBox.boxMax[0]);
			CHOOSE_MINMAX (pVc->vWorldSpace[1], BBox.boxMin[1], BBox.boxMax[1]);
			CHOOSE_MINMAX (pVc->vWorldSpace[2], BBox.boxMin[2], BBox.boxMax[2]);

			if (pFace->nVertices == 4)
			{
				CHOOSE_MINMAX(pVd->vWorldSpace[0],BBox.boxMin[0],BBox.boxMax[0]);
				CHOOSE_MINMAX(pVd->vWorldSpace[1],BBox.boxMin[1],BBox.boxMax[1]);
				CHOOSE_MINMAX(pVd->vWorldSpace[2],BBox.boxMin[2],BBox.boxMax[2]);
			}

			/*
			// Is this off screen?
			//
			// if so compact up the lists (this is really nasty!) BUT
			// I think this will be a reasonably rare case.
			//
			// Guess we'll just have to see when we profile...
			*/
			if (!RnDetermineRegionsFromBBox (&BBox, &pXExtra->RegionsRect))
			{
			  	PXMESHEXTRA					pNewExtras;
				TRANSFORMED_PLANE_STRUCT	*pNewXPlane;
				int j;

				DPF ((DBG_MESSAGE,"Part ZCLIPPED OFFscreen:Shift down:%d",k-1));

				pNewExtras = pXExtra;
				pNewXPlane = pXPlane;
				/*
				// Step through the number of planes we haven't yet looked at
				*/
				for(j = k - 1; j != 0; j--, pNewExtras++, pNewXPlane++)
				{
					/*
					// Copy the next one up down to this point
					*/
					*pNewExtras = *(pNewExtras+1);
					*pNewXPlane = *(pNewXPlane+1);
				}/*end for*/

				/*
				// Move the pointers down one spot to compensate for the
				// add that will happen at the end of the loops :)
				*/
				pXExtra --;
				pXPlane --;
				NumRemaining --;

				ASSERT(NumRemaining >= 0);
			}
			/*
			// Else this  is on screen (well probably 8) )
			*/
			else
			{
				PTRANSVERTEX pBestVert;
				float 	fRepZ, fTotalZ;
				
				/* check they aren't back to front */
				ASSERT (pXExtra->RegionsRect.FirstXRegion <= 
									pXExtra->RegionsRect.LastXRegion);
				ASSERT (pXExtra->RegionsRect.FirstYRegion <= 
									pXExtra->RegionsRect.LastYRegion);

				/* 
				// look for furthest away rep point 
				*/
				fRepZ	  = pVa->vWorldSpace[2];
				pBestVert = pVa;

				if(pVb->vWorldSpace[2] > fRepZ)
				{
					fRepZ	  = pVb->vWorldSpace[2];
					pBestVert = pVb;
				}

				if(pVc->vWorldSpace[2] > fRepZ)
				{
					fRepZ	  = pVc->vWorldSpace[2];
					pBestVert = pVc;
				}

				if((pFace->nVertices == 4) &&( pVd->vWorldSpace[2] > fRepZ))
				{
					fRepZ	  = pVd->vWorldSpace[2];
					pBestVert = pVd;
				}

				/*
				// NOTE We MUST have found one in front of the clipping plane
				*/
#if DEBUG
				if(!(fRepZ >= (pProjMat->invForegroundDistance) ))
				{
					DPF ((DBG_WARNING, "Rep Z infront of Foreground\n"));
				}
#endif
				
				/* 
				// copy the rep point's data 
				*/
				VecCopy (pBestVert->pvVertex, (float*) pFace->PlaneData.rep_point);
				VecCopy (pBestVert->vWorldSpace, pXPlane->repPnt);
				pXPlane->projRepPoint[0] = pBestVert->vScreenSpace[0];
				pXPlane->projRepPoint[1] = pBestVert->vScreenSpace[1];

				/*
				// Because of the crap plane limit on Midas3, we need
				// sort sort of heuristic for rejecting some faces.
				// Just make it large for this case
				*/
				#if ISPTSP
					pXExtra->ApproxSize = 1000;
				#endif

				/*
				// For the cases when this is translucent, give a 1/Z depth
				// Note that this is a waste WRT divisions... but
				// this stuff will all be re-written for version 3.1 of SGL
				*/
				 
				if(pFace->nVertices == 4)
				{
				  fTotalZ = pVa->vWorldSpace[2]+pVb->vWorldSpace[2]+
					pVc->vWorldSpace[2]+pVd->vWorldSpace[2];
				  if(fTotalZ !=0.0f)
				  {
					pXExtra->NearZ = 4.0f /fTotalZ;
				  }
				  else
				  {
					pXExtra->NearZ = invForegroundDistance;
				  }  
				}
				else
				{
				  fTotalZ = pVa->vWorldSpace[2]+pVb->vWorldSpace[2]+
					pVc->vWorldSpace[2];
				  if(fTotalZ !=0.0f)
				  {
					pXExtra->NearZ = 3.0f /fTotalZ;
				  }
				  else
				  {
					pXExtra->NearZ = invForegroundDistance;
				  }  
				}

				/*
				// Set a flag on the face to indicate that it is Z clipped.
				// This indicates if we need to add a clipping plane
				*/
				pXExtra->ZClipped = TRUE;

			}/*end else the funny poly is on screen*/

		}
		/*///////////////////////
		// Else SOME of the points are off screen. BUT NONE are Z clipped
	 	/////////////////////// */
		else
		{
			/*static c = 0;*/
			/*
			// Work out the regions the face covers
			//
			// Get the extremes of the first 2 points
			*/
			if(pVa->nRegion[0] < pVb->nRegion[0])
			{
				n32MinX	= pVa->nRegion[0];
				n32MaxX	= pVb->nRegion[0];
			}
			else
			{
				n32MinX	= pVb->nRegion[0];
				n32MaxX	= pVa->nRegion[0];
			}

			if(pVa->nRegion[1] < pVb->nRegion[1])
			{
				n32MinY = pVa->nRegion[1];
				n32MaxY = pVb->nRegion[1];
			}
			else
			{
				n32MinY = pVb->nRegion[1];
				n32MaxY = pVa->nRegion[1];
			}

			/*
			// Look at the third point
			*/
			CHOOSE_MINMAX (pVc->nRegion[0], n32MinX, n32MaxX);
			CHOOSE_MINMAX (pVc->nRegion[1], n32MinY, n32MaxY);
	
			/*
			// Is there a 4th point
			*/		
			if (pFace->nVertices == 4)
			{
				CHOOSE_MINMAX (pVd->nRegion[0], n32MinX, n32MaxX);
				CHOOSE_MINMAX (pVd->nRegion[1], n32MinY, n32MaxY);
			}/*end if 4th pointers */
		
			/*
			// Store the results
			*/		
			pXExtra->RegionsRect.FirstXRegion = n32MinX;
			pXExtra->RegionsRect.FirstYRegion = n32MinY;
			pXExtra->RegionsRect.LastXRegion  = n32MaxX;
			pXExtra->RegionsRect.LastYRegion  = n32MaxY;

			/* check they aren't back to front */
			ASSERT ( n32MinX <= n32MaxX);
			ASSERT ( n32MinY <= n32MaxY);

			/* check face is on screen */
#if DEBUG
			if(!RnGlobalProjMatRegionBoxOnScreen(&(pXExtra->RegionsRect)))
			{
				DPF ((DBG_MESSAGE, "RnGlobalProjMatRegionBoxOnScreen Failed\n"));
			}
#endif
			/*
			//Now look for a Rep Point, basically first see if there
			// is one that is on screen
			*/
			if(/*(++c%2) ||*/ ((pVa->Flags & CLIPPED_FLAGS) == 0))
			{
				DPF((DBG_VERBOSE, "Choosing Vert A as rep"));
				pXPlane->projRepPoint[0] = pVa->vScreenSpace[0];
				pXPlane->projRepPoint[1] = pVa->vScreenSpace[1];

				VecCopy (pVa->pvVertex, (float*) pFace->PlaneData.rep_point);
				VecCopy (pVa->vWorldSpace,	pXPlane->repPnt);
			}
			else if((pVb->Flags & CLIPPED_FLAGS) == 0)
			{
				DPF((DBG_VERBOSE, "Choosing Vert B as rep"));
				pXPlane->projRepPoint[0] = pVb->vScreenSpace[0];
				pXPlane->projRepPoint[1] = pVb->vScreenSpace[1];

				VecCopy (pVb->pvVertex, (float*) pFace->PlaneData.rep_point);
				VecCopy (pVb->vWorldSpace,	pXPlane->repPnt);
			}
			else if((pVc->Flags & CLIPPED_FLAGS) == 0)
			{
				DPF((DBG_VERBOSE, "Choosing Vert C as rep"));
				pXPlane->projRepPoint[0] = pVc->vScreenSpace[0];
				pXPlane->projRepPoint[1] = pVc->vScreenSpace[1];

				VecCopy (pVc->pvVertex, (float*) pFace->PlaneData.rep_point);
				VecCopy (pVc->vWorldSpace,	pXPlane->repPnt);
			}
			else if((pFace->nVertices == 4)&&(pVd->Flags & CLIPPED_FLAGS) == 0)
			{
				DPF((DBG_VERBOSE, "Choosing Vert D as rep"));
				pXPlane->projRepPoint[0] = pVd->vScreenSpace[0];
				pXPlane->projRepPoint[1] = pVd->vScreenSpace[1];

				VecCopy (pVd->pvVertex, (float*) pFace->PlaneData.rep_point);
				VecCopy (pVd->vWorldSpace,	pXPlane->repPnt);
			}
			/*
			// They are all off screen.
			*/
			else
			{
				PTRANSVERTEX pV;
								
				if (pVa->vWorldSpace[2] > pVb->vWorldSpace[2])
				{
					if (pVa->vWorldSpace[2] > pVc->vWorldSpace[2])
					{
						pV = pVa;
					}
					else
					{
						pV = pVc;
					}
				}
				else
				{
					if (pVb->vWorldSpace[2] > pVc->vWorldSpace[2])
					{
						pV = pVb;
					}
					else 
					{
						pV = pVc;
					}
				}
				
				if ((pFace->nVertices == 4) && 
									  (pVd->vWorldSpace[2] > pV->vWorldSpace[2]))
				{
					pV = pVd;
				}
				
				pXPlane->projRepPoint[0] = pV->vScreenSpace[0];
				pXPlane->projRepPoint[1] = pV->vScreenSpace[1];
				VecCopy (pV->pvVertex, (float*) pFace->PlaneData.rep_point);
				VecCopy (pV->vWorldSpace,	pXPlane->repPnt);
			}

			/*
			// Because of the crap plane limit on Midas3, we need
			// sort sort of heuristic for rejecting some faces.
			// Look at the "length" of the edge AB
			*/
			#if ISPTSP
				pXExtra->ApproxSize = 
					sfabs(pVa->vScreenSpace[0] - pVb->vScreenSpace[0]) +
					sfabs(pVa->vScreenSpace[1] - pVb->vScreenSpace[1]);
			#endif

			/*
			// For the cases when this is translucent, give a 1/Z depth
			// Note that this is a waste WRT divisions... but
			// this stuff will all be re-written for version 3.1 of SGL
			*/
			if(pFace->nVertices == 4)
			{
			  pXExtra->NearZ = 4.0f /
				(pVa->vWorldSpace[2]+pVb->vWorldSpace[2]+
				 pVc->vWorldSpace[2]+pVd->vWorldSpace[2]);
			}
			else
			{
			  pXExtra->NearZ = 3.0f /
				(pVa->vWorldSpace[2]+pVb->vWorldSpace[2]+
				 pVc->vWorldSpace[2]);
			}

			/*
			// It's not Z clipped.
			*/
			pXExtra->ZClipped = FALSE;

		}/* End Else all corners off screen */			

	}/*end for k*/
	
	SGL_TIME_STOP(TRANSFORM_FACES_PARTLY_TIME)
	return NumRemaining;
}

/**************************************************************************
 * Function Name  : CreatePointData (LOCAL FUNCTION)
 *					pXExtra   - pointer to the first of the Extra Data for faces
 *					numFaces  - the number of faces to process
 *	
 * Outputs        : pPointsData - an array of point data
 *					pXPlane		- array of transformed (INPUT/OUTPUT)
 * Returns		  : NOTHING
 *
 *	NOTE NOTE NOTE: THIS ROUTINE IS A STOP GAP MEASURE., THE SET UP OF THIS
 *					DATA SHOULD BE DONE IN DLMESH!!!!!!!!!				
 **************************************************************************/
#if 0
static void CreatePointData(PXMESHEXTRA	     pXExtra,
								int			 numFaces,
					  CONV_POINTS_STRUCT	 *pPointsData,
					TRANSFORMED_PLANE_STRUCT *pXPlane)
{

	const FACE * pFace;
	PTRANSVERTEX   pVa, pVb, pVc;

	for(/*NOTHING*/;  numFaces != 0; numFaces --, pXExtra++, pPointsData++, pXPlane++)
	{
		/*
		// Get the pointer to the face data
		*/
		pFace = pXExtra->pOrigFace;

		/*
		// Get the pointer to the face data
		*/
		pXPlane->pPointsData = pPointsData;

		/*
		// Get pointers to the vertices on this face
		*/
		pVa = gpXVertices + pFace->pnVertexIDs[0];
		pVb = gpXVertices + pFace->pnVertexIDs[1];
		pVc = gpXVertices + pFace->pnVertexIDs[2];

		/*
		// Compute the stuff which should have been done in dlmesh
		*/
		VecCopy(pVa->pvVertex, pPointsData->pt1);
		VecSub (pVb->pvVertex, pVa->pvVertex, pPointsData->pt2_delta);
		VecSub (pVc->pvVertex, pVa->pvVertex, pPointsData->pt3_delta);

	}/*end for*/
}

#endif
/**************************************************************************
 * Function Name  : CreateSmoothShadingDataVisible(LOCAL FUNCTION)
 *					
 *	
 * Outputs        : 
 *
 *	
 **************************************************************************/

static void CreateSmoothShadingDataVisible (PXMESHEXTRA	 pXExtra,
											int			 numFaces,
											TRANSFORMED_PLANE_STRUCT *pXPlane)
{
	const FACE * pFace;
	PTRANSVERTEX   pVa, pVb, pVc;
	CONV_SHADING_STRUCT	 *pS;

	for(/*NOTHING*/;  numFaces != 0; numFaces --, pXExtra++, pXPlane++)
	{
		float fDet;

		/*
		// Get the pointer to the face data
		*/
		pFace = pXExtra->pOrigFace;

		if (pFace->PlaneData.flags & pf_smooth_shad)
		{
			/*
			// Get the pointer to the shading data
			*/
			pS = (CONV_SHADING_STRUCT*) pXPlane->pShadingData;

			/*
			// Get pointers to the vertices on this face
			*/
			pVa = gpXVertices + pFace->pnVertexIDs[0];
			pVb = gpXVertices + pFace->pnVertexIDs[1];
			pVc = gpXVertices + pFace->pnVertexIDs[2];

			#define fX0 pVa->vScreenSpace[0]
			#define fX1 pVb->vScreenSpace[0]
			#define fX2 pVc->vScreenSpace[0]

			#define fY0 pVa->vScreenSpace[1]
			#define fY1 pVb->vScreenSpace[1]
			#define fY2 pVc->vScreenSpace[1]

			fDet = fX0*fY1 - fX0*fY2 - fX1*fY0 + fX1*fY2 + fX2*fY0 - fX2*fY1;

			/*
			// Calculate 1/determinant:
			*/
		  #if defined(MIDAS_ARCADE)

			/* MIDAS Arcade gets FP execeptions if we use 1.0e-20f.  1.0e-3f seems to be okay */
			if (sfabs (fDet) > 1.0e-3f)

		  #else

			if (sfabs (fDet) > 1.0e-20f)

		  #endif
			{
				pS->f1OverDet = 1.0f / fDet;

				/*
				// pPre->fAdjoint is a constant zero offset
				*/
				pS->fAdjoint[0][0] = fY1-fY2;
				pS->fAdjoint[0][1] = fY2-fY0;
				pS->fAdjoint[0][2] = fY0-fY1;

				pS->fAdjoint[1][0] = fX2-fX1;
				pS->fAdjoint[1][1] = fX0-fX2;
				pS->fAdjoint[1][2] = fX1-fX0;

				pS->fRepDeltaX = 0.0f;
				pS->fRepDeltaY = 0.0f;
			}

			#undef fX0
			#undef fX1
			#undef fX2

			#undef fY0
			#undef fY1
			#undef fY2
		}
	}/*end for*/
}



/**************************************************************************
 * Function Name  : ExtrapolateScreenPos(LOCAL FUNCTION)
 *					
 * Inputs		  : pVClipped    A vertex that is Z clipped
 *				    pVOK		 A vertex which is NOT
 *	
 * Outputs        : PretendProj	 A "screen" position for the clipped
 *								 vertex. This is obtained by clipping the
 *								 line joing the two vertices, getting its
 *								 Screen position.  By THEN extrapolating the line
 *								 between the projected screen coord of the OK
 *								 vertex and clipped point, we should get a 
 *								 pseudo pos for our original bad vertex.
 *	
 **************************************************************************/
static void  ExtrapolateScreenPos( const PTRANSVERTEX pVClipped,
								   const PTRANSVERTEX pVOk,
								   sgl_2d_vec PretendProj,
								   float foregroundDistance,
								   float invForegroundDistance,
								   float SxDash, float SyDash,
								   float OxDash, float OyDash)
{
	float fBehind, fZdelta, fScale;
	float fX, fY;
	/*
	// First intersect the line between the vertices in world space
	// with clipping plane
	*/
	fZdelta = pVOk->vWorldSpace[2] - pVClipped->vWorldSpace[2];
	fBehind = foregroundDistance - pVClipped->vWorldSpace[2];
	ASSERT(fZdelta >= 0.0f);
	ASSERT(fBehind >= 0.0f);

  #if defined(MIDAS_ARCADE)

	/* MIDAS Arcade gets FP execeptions if we use 1.0e-20f.  1.0e-3f seems to be okay */
	if(fZdelta > 1.0E-3f)

  #else

	if(fZdelta > 1.0E-20f)

  #endif
	{
		fScale = fBehind / (fZdelta);
	}
	else
	{
		fScale = fBehind * 1.0E20f;
	}

	/*
	// Get the World space pos of the clipped value
	*/
	fX = pVClipped->vWorldSpace[0] + 
			fScale * (pVOk->vWorldSpace[0] - pVClipped->vWorldSpace[0]);
	fY = pVClipped->vWorldSpace[1] + 
			fScale * (pVOk->vWorldSpace[1] - pVClipped->vWorldSpace[1]);
	
	/*
	// Get the screen space pos of the clipped value
	*/
	fX = (SxDash * fX * invForegroundDistance) + OxDash;
	fY = (SyDash * fY * invForegroundDistance) + OyDash;

	/*
	// Now interpolate BACK to where the pseudo bad vertex will end up
	*/
	fBehind = (1.0f - fScale);

  #if defined(MIDAS_ARCADE)

	/* MIDAS Arcade gets FP execeptions if we use 1.0e-20f.  1.0e-3f seems to be okay */
	if(fBehind > 1.0E-3f)
	{
		fBehind = 1.0f / fBehind;
	}
	else
	{
		fBehind = 1.0E-3f;
	}

  #else

	if(fBehind > 1.0E-20f)
	{
		fBehind = 1.0f / fBehind;
	}
	else
	{
		fBehind = 1.0E-20f;
	}

  #endif

	PretendProj[0] = (fX - fScale * pVOk->vScreenSpace[0]) * fBehind;
	PretendProj[1] = (fY - fScale * pVOk->vScreenSpace[1]) * fBehind;

}

/**************************************************************************
 * Function Name  : CreateSmoothShadingDataPartlyVisible(LOCAL FUNCTION)
 *					
 *	
 * Outputs        : 
 * 
 * 					Generates the input to the smooth shading function, when
 *					there is a chance of the faces being Z clipped.
 *	
 **************************************************************************/

#define Vec2dCopy(x,y)	y[0]=x[0];y[1]=x[1];

static void CreateSmoothShadingDataPartlyVisible (PXMESHEXTRA	 pXExtra,
											int			 numFaces,
											TRANSFORMED_PLANE_STRUCT *pXPlane,
											sgl_bool bIsTextureWrapping,
											const TRANSFORM_STRUCT	 *pTransform)
{
	const FACE * pFace;
	PTRANSVERTEX   pVa, pVb, pVc;
	CONV_SHADING_STRUCT	*pS;
	float foregroundDistance, invForegroundDistance, SxDash, SyDash, OxDash, OyDash;
	PROJECTION_MATRIX_STRUCT  * const pProjMat = RnGlobalGetProjMat ();

	foregroundDistance = pProjMat->foregroundDistance;
	invForegroundDistance = pProjMat->invForegroundDistance;
	SxDash = pProjMat->SxDash;
	SyDash = pProjMat->SyDash;
	OxDash = pProjMat->OxDash;
	OyDash = pProjMat->OyDash;

	for(/*NOTHING*/;  numFaces != 0; numFaces --, pXExtra++, pXPlane++)
	{
		float fDet;

		/*
		// Get the pointer to the face data
		*/
		pFace = pXExtra->pOrigFace;

		if (pFace->PlaneData.flags & pf_smooth_shad)
		{
			/*
			// Get the pointer to the shading data
			*/
			pS = (CONV_SHADING_STRUCT*) pXPlane->pShadingData;

			/*
			// Get pointers to the vertices on this face
			*/
			pVa = gpXVertices + pFace->pnVertexIDs[0];
			pVb = gpXVertices + pFace->pnVertexIDs[1];
			pVc = gpXVertices + pFace->pnVertexIDs[2];


			/*
			// If any of vertices A, B, or C are Z clipped, then can't
			// use the usual Non-clipped code.
			*/
			if ((pVa->Flags | pVb->Flags | pVc->Flags) & CLIPPED_Z)
			{
				/*
				// This holds the projected position of the vertices AND OR
				// clipped positions
				*/
				sgl_2d_vec proj[3];
				float dist;
				PTRANSVERTEX  pVfurthest;
				

				/*
				// This code needs to come up with "new" screen positions
				// for any of A,B, or C that are clipped. To do this,
				// first choose the vertex that is furthest away
				*/
				if(pVa->vWorldSpace[2] > pVb->vWorldSpace[2])
				{
					dist = pVa->vWorldSpace[2];
					pVfurthest = pVa;
				}
				else
				{
					dist = pVb->vWorldSpace[2];
					pVfurthest = pVb;
				}

				if(dist <  pVc->vWorldSpace[2])
				{
					dist = pVc->vWorldSpace[2];
					pVfurthest = pVc;
				}

				if(pFace->nVertices == 4)
				{
					PTRANSVERTEX  pVd;
					pVd = gpXVertices + pFace->pnVertexIDs[3];

					
					if(dist <  pVd->vWorldSpace[2])
					{
						dist = pVd->vWorldSpace[2];
						pVfurthest = pVd;
					}
				}

				/*
				// The furthest vertex CANT be zclipped. We wouldnt be here
				// otherwise.
				*/
				ASSERT(!(pVfurthest->Flags& CLIPPED_Z))

				/*
				// For each vertex that is Z clipped, come up with an
				// "extrapolated" screen position for it, otherwise just use
				// the screen position it's current got
				*/
				if(pVa->Flags & CLIPPED_Z)
				{
					ExtrapolateScreenPos(pVa, pVfurthest, proj[0], 
										 foregroundDistance, 
										 invForegroundDistance, 
										 SxDash, SyDash, OxDash, OyDash);
				}
				else
				{
					Vec2dCopy (pVa->vScreenSpace, proj[0]);
				}


				if(pVb->Flags & CLIPPED_Z)
				{
					ExtrapolateScreenPos(pVb, pVfurthest, proj[1],
										 foregroundDistance, 
										 invForegroundDistance, 
										 SxDash, SyDash, OxDash, OyDash);
				}
				else
				{
					Vec2dCopy (pVb->vScreenSpace, proj[1]);
				}

				if(pVc->Flags & CLIPPED_Z)
				{
					ExtrapolateScreenPos(pVc, pVfurthest, proj[2],
										 foregroundDistance, 
										 invForegroundDistance, 
										 SxDash, SyDash, OxDash, OyDash);
				}
				else
				{
					Vec2dCopy (pVc->vScreenSpace, proj[2]);
				}


				/*
				// Come up with the adjoint matrix of the screen positions
				*/
				#define fX0 proj[0][0]
				#define fX1 proj[1][0]
				#define fX2 proj[2][0]

				#define fY0 proj[0][1]
				#define fY1 proj[1][1]
				#define fY2 proj[2][1]

				fDet = fX0*fY1 - fX0*fY2 - fX1*fY0 + fX1*fY2 + fX2*fY0 - fX2*fY1;

				/*
				// Calculate 1/determinant:
				*/
			  #if defined(MIDAS_ARCADE)

				/* MIDAS Arcade gets FP execeptions if we use 1.0e-20f.  1.0e-3f seems to be okay */
				if (sfabs (fDet) > 1.0e-3f)
				{
					pS->f1OverDet = 1.0f / fDet;
				}
				else if(fDet > 0.0f)
				{
					pS->f1OverDet = 1.0e-3f;
				}
				else
				{
					pS->f1OverDet = -1.0e-3f;
				}

			  #else

				if (sfabs (fDet) > 1.0e-20f)
				{
					pS->f1OverDet = 1.0f / fDet;
				}
				else if(fDet > 0.0f)
				{
					pS->f1OverDet = 1.0e-20f;
				}
				else
				{
					pS->f1OverDet = -1.0e-20f;
				}

			  #endif
				/*
				// pPre->fAdjoint is a constant zero offset
				*/
				pS->fAdjoint[0][0] = fY1-fY2;
				pS->fAdjoint[0][1] = fY2-fY0;
				pS->fAdjoint[0][2] = fY0-fY1;

				pS->fAdjoint[1][0] = fX2-fX1;
				pS->fAdjoint[1][1] = fX0-fX2;
				pS->fAdjoint[1][2] = fX1-fX0;

				pS->fRepDeltaX = pXPlane->projRepPoint[0] - proj[0][0];
				pS->fRepDeltaY = pXPlane->projRepPoint[1] - proj[0][1];

				#undef fX0
				#undef fX1
				#undef fX2

				#undef fY0
				#undef fY1
				#undef fY2
			}
			/*
			// Else No Z clipping to worry us. Just use A, B, & C directly
			*/
			else
			{
				#define fX0 pVa->vScreenSpace[0]
				#define fX1 pVb->vScreenSpace[0]
				#define fX2 pVc->vScreenSpace[0]

				#define fY0 pVa->vScreenSpace[1]
				#define fY1 pVb->vScreenSpace[1]
				#define fY2 pVc->vScreenSpace[1]

				fDet = fX0*fY1 - fX0*fY2 - fX1*fY0 + fX1*fY2 + fX2*fY0 - fX2*fY1;

				/*
				// Calculate 1/determinant:
				*/
			  #if defined(MIDAS_ARCADE)

				/* MIDAS Arcade gets FP execeptions if we use 1.0e-20f.  1.0e-3f seems to be okay */
				if (sfabs (fDet) > 1.0e-3f)
				{
					pS->f1OverDet = 1.0f / fDet;
				}
				else if(fDet > 0.0f)
				{
					pS->f1OverDet = 1.0e-3f;
				}
				else
				{
					pS->f1OverDet = -1.0e-3f;
				}

			  #else

				if (sfabs (fDet) > 1.0e-20f)
				{
					pS->f1OverDet = 1.0f / fDet;
				}
				else if(fDet > 0.0f)
				{
					pS->f1OverDet = 1.0e-20f;
				}
				else
				{
					pS->f1OverDet = -1.0e-20f;
				}

			  #endif


				/*
				// pPre->fAdjoint is a constant zero offset
				*/
				pS->fAdjoint[0][0] = fY1-fY2;
				pS->fAdjoint[0][1] = fY2-fY0;
				pS->fAdjoint[0][2] = fY0-fY1;

				pS->fAdjoint[1][0] = fX2-fX1;
				pS->fAdjoint[1][1] = fX0-fX2;
				pS->fAdjoint[1][2] = fX1-fX0;

				pS->fRepDeltaX = pXPlane->projRepPoint[0] - pVa->vScreenSpace[0];
				pS->fRepDeltaY = pXPlane->projRepPoint[1] - pVa->vScreenSpace[1];

				#undef fX0
				#undef fX1
				#undef fX2

				#undef fY0
				#undef fY1
				#undef fY2
			}
		}
	}/*end for*/
}

/**************************************************************************
 * Function Name  : RnProcessMeshNode
 * Inputs         : pMesh- pointer to the convex primitive node
 *					
 * Outputs        : None
 * Input/Output	  : pState- pointer to a master state stack "frame"
 * Returns        : None
 * Global Used    : See description
 *
 * Description    : This does (or at least organises) all the processing
 *					of a mesh node  - from rejecting/accepting, transformation
 *					projection, shading, and giving to the hardware management
 *					side of things.
 *
 **************************************************************************/

void RnProcessMeshNode (const MESH_NODE_STRUCT *pMesh, 
						MASTER_STATE_STRUCT *pState,
						int nCurrTransSetID)
{
	int			nEdges, nVertices;
	int 		m;
	const FACE	*pFace;

#if DEBUG
	int FaceNumber = 0;
#endif
#if DEBUG || PRE_CULL
	int nFaces;
#endif

	const CONV_SHADING_STRUCT  *pShadingData;
	const CONV_TEXTURE_UNION   *pTextureData;
	const CONV_POINTS_STRUCT   *pPointsData;

	PMESHMATERIAL pLocMatEnt;
	
	/*
	// If we have REAL local materials, then we must take a copy
	// of the global material state.
	*/
	MATERIAL_STATE_STRUCT LocalMaterialState, *pMaterialState;
	
	sgl_bool				 bZClipped;
	sgl_bool				 bMustTextWrap;	
	sgl_bool				 bIsSmoothShaded;
	TEST_BOX_ENUM BoxCase;
	PROJECTION_MATRIX_STRUCT  * const pProjMat = RnGlobalGetProjMat ();

	SGL_TIME_START(TRIVIAL_REJECTION_TIME)
	/*
	// First test if the mesh is likely to be on screen. If not, exit.
	//
	// transform the bounding box into world space, and compare it against
	// the viewing pyramid
	*/
	TransformBBox(pState->pTransformState, &pMesh->CentBBox, &BBoxMinmax);

	BoxCase = RnTestBoxWithCamera (&BBoxMinmax, TRUE, &bZClipped);

	SGL_TIME_STOP(TRIVIAL_REJECTION_TIME)

	if(BoxCase ==TB_BOX_OFFSCREEN)
	{
		DPF ((DBG_VERBOSE, "Mesh invisible"));
		/* if only we could do this for all objects */
		return;
	}

	/*
	// Set up local variables etc
	*/

	ASSERT (gpXVertices);
	
	ASSERT (pMesh);
	ASSERT (pState);

	SGL_TIME_START(MESH_NODE_TIME)

	/*
	// if there are no faces, then get out of here!
	*/
	if(pMesh->nEdges <= 0)
	{
		return;
	}

	/* Set bilinear filtering mode.
	 * This is the best place to do this. The relevent bilinear
	 * filtering setting will be set for all subsequent mesh objects.
	 */
  	if (pState->pQualityState->flags & qf_texture_filtering)
	{
		pProjMat->eFilterType = pState->pQualityState->eFilterType;
	}

	/* Set dithering mode.
	 */
  	if (pState->pQualityState->flags & qf_dithering)
	{
		pProjMat->bDithering = TRUE;
	}
	else
	{
		pProjMat->bDithering = FALSE;
	}

	pShadingData = pMesh->pShadingData;
	pTextureData = pMesh->pTextureData;
	pPointsData	 = pMesh->pPointsData;

	ASSERT (pMesh->pVertexData);
	ASSERT (pMesh->pEdgeData);
	ASSERT (pMesh->Faces);

	/*
	// Set up the global face culling mode for this mesh
	*/
	
	if(! pState->pTransformState->has_neg_scaling)
	{
		keepAntiClock = (pMesh->CullMode != sgl_cull_anticlock);
		keepClockwise = (pMesh->CullMode != sgl_cull_clockwise);
	}
	else
	{
		keepClockwise = (pMesh->CullMode != sgl_cull_anticlock);
		keepAntiClock = (pMesh->CullMode != sgl_cull_clockwise);
 	}

#if PRE_CULL
	switch (pMesh->CullMode)
	{
		case sgl_cull_anticlock: 
		{
			CullMode = 0x00000000; 
			break;
		}

		case sgl_cull_clockwise: 
		{
			CullMode = 0x80000000; 
			break;
		}

		default: 
		{
			CullMode = 0xFFFFFFFF; 
			break;
		}
	}
#endif

	nVertices = pMesh->nVertices;
	nEdges	  = pMesh->nEdges;

#if DEBUG || PRE_CULL
	nFaces	  = ListGetItemsInList (pMesh->Faces);
#endif

	if (nVertices > gnXVertices)
	{
		PTRANSVERTEX pNew;
	#if PRE_CULL
		sgl_uint8		 *pNewIndex;
	#endif

		pNew = SGLMalloc (sizeof (TRANSVERTEX) * nVertices);
		if (pNew)
		{
			DPF ((DBG_WARNING, "Bumping up static vertex array to %d", nVertices));

			SGLFree (gpXVertices);
			
			gpXVertices = pNew;
			gnXVertices = nVertices;
		}
		else
		{
			DPF ((DBG_ERROR,
			 "Unable to bump up static vertex array to %d - not rendering mesh", 
							 nVertices));
			SGL_TIME_STOP(MESH_NODE_TIME)
			return ;
		}

		#if PRE_CULL
		pNewIndex = SGLMalloc (sizeof (sgl_uint8) * nVertices);
		if (pNewIndex)
		{
		    DPF ((DBG_WARNING, "Bumping up static vertex index array to %d", nVertices));
			
			SGLFree (gpXVertexIndex);
			
			gpXVertexIndex = pNewIndex;
		}
		else
		{
		    DPF ((DBG_ERROR,
				  "Unable to bump up static vertex index array to %d - not rendering mesh", 
				  nVertices));
			SGL_TIME_STOP(MESH_NODE_TIME)
			  return ;
		}

		#endif
	}

	if (nEdges > gnXEdges)
	{
	    PTRANSEDGE pNew;
#if PRE_CULL
		sgl_uint8	   *pNewIndex;
#endif
		pNew = SGLMalloc ( sizeof (TRANSEDGE) * nEdges);
		if (pNew)
		{
			DPF ((DBG_WARNING, "Bumping up static edge array to %d", nEdges));
			SGLFree(gpXEdges);

			gpXEdges = pNew;
			gnXEdges = nEdges;
		}
		else
		{
			DPF ((DBG_ERROR,
				  "Unable to bump up static edge array to %d - not rendering mesh",
				  nEdges));
			SGL_TIME_STOP(MESH_NODE_TIME)
			return ;
		}

	 #if PRE_CULL
		pNewIndex = SGLMalloc ( sizeof (sgl_uint8) * nEdges);		
		if (pNewIndex)
		{
			DPF ((DBG_WARNING, "Bumping up static edge index array to %d", nEdges));
			SGLFree(gpXEdgeIndex);
			gpXEdgeIndex = pNewIndex;
		}
		else
		{
			DPF((DBG_ERROR,
			"Unable to bump up static edge index array to %d - not rendering mesh", 
				 nEdges));
			SGL_TIME_STOP(MESH_NODE_TIME)
			return ;
		}

	#endif
	}


	/*
	// We must have a valid mesh
	*/
	#if DEBUG
		if (!nVertices || !nFaces || !nEdges)
		{
			DPF ((DBG_ERROR, 
				"RnProcessMeshNode: Bad mesh: %d vertices, %d faces, %d edges", 
				nVertices, nFaces, nEdges));
		}
	#endif

	
	/*
	  // Do we need to update the local projection matrix
	*/
	if(!RnGlobalLocalProjMatIsValid())
	{
		DPF((DBG_MESSAGE, "Update LT mat"));
		RnUpdateLocalProjectionMatrix(pState->pTransformState);
	}

	/*
	// Do we have any "real" local materials? If we do, make a local
	// copy of the global one, and set up a pointer to it
	*/
	ASSERT(ListGetItemsInList(pMesh->Materials) > 0);

	if(pMesh->HasLocalMaterials)
	{
		LocalMaterialState =  *pState->pMaterialState;
		pMaterialState     =  &LocalMaterialState;
	}
	/*
	// Else we arent going to modify the global materials
	// structure, so just use it (ie get a pointer to it)
	*/
	else
	{
		pMaterialState = pState->pMaterialState;

	}/*end if/else local materials*/


	#if PRE_CULL

	    SGL_TIME_SUSPEND(MESH_NODE_TIME)
		PreprocessFacesVisible (nVertices, nEdges, nFaces, 
								ListFindItemFast (pMesh->Vertices, 0), 
								ListFindItemFast (pMesh->Faces, 0), 
								pState->pTransformState);
	    SGL_TIME_RESUME(MESH_NODE_TIME)

	#endif

	/*
	// Determine if we have to do any texture wrapping
	*/

	bMustTextWrap =(pMaterialState->texture_flags & MASK_TEXTURE) &&
				   (pMaterialState->texture_flags & SMAP_BITS) &&
				   (pMaterialState->texture_flags & OMAP_BITS) &&
					!(pMesh->ORedPlaneFlags & pf_pre_mapped);

	/* 
	// is any part of the mesh smooth shaded?  and is smooth shading enabled
	*/

	bIsSmoothShaded = (pMesh->ORedPlaneFlags & pf_smooth_shad) &&
					  (pState->pQualityState->flags & qf_smooth_shading);

	/*
	// Process the vertices of the mesh. Determine which routine to
	// use by the flag returned from the bounding box routine
	*/
	if(BoxCase == TB_BOX_ALL_ONSCREEN)
	{
		DPF ((DBG_VERBOSE, "Mesh completely visible"));
		/* 
		// transform all vertices to world space and project it to
		//  screen space 
		*/
		SGL_TIME_SUSPEND(MESH_NODE_TIME)
		ProcessVerticesAllVisible (pMesh->pVertexData, nVertices, 
									  pState->pTransformState);
		SGL_TIME_RESUME(MESH_NODE_TIME)
	}
	else
	{
		DPF ((DBG_VERBOSE, "Mesh partly visible"));

		ASSERT(BoxCase == TB_BOX_PART_ONSCREEN);
		
		/* 
		// transform all vertices to world space and clip. If a vertex is
		// visible, transform it to screen space 
		//
		// NOTE if all the vertices are visible, then we can upgrade to
		// the completely on-screen case, which is faster.
		*/
		SGL_TIME_SUSPEND(MESH_NODE_TIME)
		BoxCase = ProcessVerticesPartlyVisible (pMesh->pVertexData, nVertices, 
										 pState->pTransformState);
		SGL_TIME_RESUME(MESH_NODE_TIME)

		/*
		// If the mesh IS offscreen... then get out of here
		*/
		if(BoxCase ==TB_BOX_OFFSCREEN)
		{
			DPF ((DBG_VERBOSE, "Mesh invisible"));
			/* if only we could do this for all objects */
		    SGL_TIME_STOP(MESH_NODE_TIME)
			return;
		}
	}

	/*
	// Process the Edges and faces of the mesh. NOTE. I am hoping the
	// compiler is smart enough to notice that in the previous case where
	// "BoxCase == TB_BOX_ALL_ONSCREEN" ie for the vertices, that it will
	// notice the value has not changed and short circuit the test. GCC will
	// do this, so I hope other compilers will as well. SJF
	*/
	if(BoxCase == TB_BOX_ALL_ONSCREEN)
	{

		DPF ((DBG_VERBOSE, "Mesh completely visible AGAIN"));

		/*
		// The next thing to do is process all the edges working out their
		// sabre parameters. Direction will have to be decided later, should
		// be a matter of negating if the opposite sense is desired.
		// Should be able to tell according to order the vertices are
		// specified on the edge.
		*/
	    SGL_TIME_SUSPEND(MESH_NODE_TIME)
		ProcessEdgesAllVisible (pMesh->pEdgeData, nEdges);
	    SGL_TIME_RESUME(MESH_NODE_TIME)
		/*
		// Step through all the "local" materials. Note even if there
		// aren't any real local materials, the first one always refers
		// to the number of planes affected by the global material.
		//
		// Furthermore, each material can refer to a maximum of 
		// SGL_MAX_PLANES surfaces. This reduces the amount of testing we
		// have to do during this routine.
		//
		*/
		pFace = ListFindItemFast (pMesh->Faces, 0);
		pLocMatEnt = ListFindItemFast (pMesh->Materials, 0);

		for(m = ListGetItemsInList(pMesh->Materials); m!=0; m--, pLocMatEnt++)
		{
			int numProjected;

			/*
			// Update the material with this new material if necessary
			//
			// If the pointer is NULL then no changes are to be made.
			*/
			if(pLocMatEnt->pMaterial != NULL)
			{
				RnProcessMaterialNode( pLocMatEnt->pMaterial, pMaterialState,
															  pState);
			}

			ASSERT(pLocMatEnt->nMaterialUsage >= 0);
			ASSERT(pLocMatEnt->nMaterialUsage <= SGL_MAX_INTERNAL_PLANES);

			/*
			// Do the faces affected by the material
			//
			// First cull/project/etc the faces, and return the number
			// that remain.
			*/
	        SGL_TIME_SUSPEND(MESH_NODE_TIME)
			numProjected = ProcessFacesVisible (&pFace, 
						   pLocMatEnt->nMaterialUsage,
						   pState->pTransformState,
						   gXExtras,
						   GlobalTransformedPlanes,
						   &pShadingData,
						   &pTextureData,
						   &pPointsData);
	        SGL_TIME_RESUME(MESH_NODE_TIME)
		#if DEBUG
			FaceNumber += pLocMatEnt->nMaterialUsage;
		#endif


			/*
			// if all the faces were rejected, go to the next group
			*/
			if(numProjected == 0)
			{
				continue;
			}

		#if 0
			/*
			// if we are doing texture wrapping then create point data
			*/
			if (bMustTextWrap)
			{
				CreatePointData (gXExtras, numProjected, gPointsData, GlobalTransformedPlanes);
			}
		#endif
			#if 1
			/*
			// if we are doing smooth shading then create shading data
			*/
			SGL_TIME_SUSPEND(MESH_NODE_TIME)
			if (bIsSmoothShaded)
			{
				CreateSmoothShadingDataVisible (gXExtras, 
												numProjected, 
												GlobalTransformedPlanes);
			}
			SGL_TIME_RESUME(MESH_NODE_TIME)
			#endif

			/*
			// Now shade the remaining planes
			*/
			SGL_TIME_SUSPEND(MESH_NODE_TIME)
			if (!DoPlaneStuff (pMesh, numProjected, pState, 
							   pMaterialState, nCurrTransSetID))
			{
				break;
			}
			SGL_TIME_RESUME(MESH_NODE_TIME)
		}/*end for step through the materials*/
	}
	/*
	// ELSE PARTLY OnScreen
	*/
	else
	{
		DPF ((DBG_VERBOSE, "Mesh partly visible AGAIN"));

		ASSERT(BoxCase == TB_BOX_PART_ONSCREEN);

  		/*
		// If we are here then we should have a partially transformed list 
		// of vertices. If the vertex is visible, we should have screen coords,
		// and region info calculated. If invisible, we should have world
		// coordinates, a reasonable halfway house should we wish to work out
		// where the point is in screen coords later (such as working out data
		// for partially offscreen edge). The next thing to do is process all 
		// the edges to work out their sabre parameters. Direction will have
		// to be decided later, should be a matter of negating if the
		// opposite sense is desired. Should be able to tell according to
		//  the direction the face walks the edge.
		*/
		SGL_TIME_SUSPEND(MESH_NODE_TIME)
		ProcessEdgesPartlyVisible (pMesh->pEdgeData, nEdges);

		SGL_TIME_RESUME(MESH_NODE_TIME)
		/*
		// Step through all the "local" materials. Note even if there
		// aren't any real local materials, the first one always refers
		// to the number of planes affected by the global material.
		//
		// Furthermore, each material can refer to a maximum of 
		// SGL_MAX_PLANES surfaces. This reduces the amount of testing we
		// have to do during this routine.
		//
		*/
		pFace = ListFindItemFast (pMesh->Faces, 0);
		pLocMatEnt = ListFindItemFast (pMesh->Materials, 0);

		for(m = ListGetItemsInList(pMesh->Materials); m!=0; m--, pLocMatEnt++)
		{
			int numProjected;

			/*
			// Update the material with this new material if necessary
			//
			// If the pointer is NULL then no changes are to be made.
			*/
			if(pLocMatEnt->pMaterial != NULL)
			{
				RnProcessMaterialNode( pLocMatEnt->pMaterial, pMaterialState,
															  pState);
			}

			ASSERT(pLocMatEnt->nMaterialUsage >= 0);
			ASSERT(pLocMatEnt->nMaterialUsage <= SGL_MAX_INTERNAL_PLANES);

			/*
			// Do the faces affected by the material
			//
			// First cull/project/etc the faces, and return the number
			// that remain.
			*/
			numProjected = ProcessFacesPartlyVisible (&pFace, 
						   pLocMatEnt->nMaterialUsage,
						   pState->pTransformState,
						   gXExtras,
						   GlobalTransformedPlanes,
						   &pShadingData,
						   &pTextureData,
						   &pPointsData);

			/*
			// if all the faces were rejected, go to the next group
			*/
			if(numProjected == 0)
			{
				continue;
			}
		
		#if 0
			/*
			// if we are doing texture wrapping then create point data
			*/
			if (bMustTextWrap)
			{
				CreatePointData (gXExtras, numProjected, gPointsData, GlobalTransformedPlanes);
			}
		#endif
			#if 1
			/*
			// if we are doing smooth shading then create shading data
			*/
			if (bIsSmoothShaded)
			{
				CreateSmoothShadingDataPartlyVisible (gXExtras, 
												numProjected, 
												GlobalTransformedPlanes,
												bMustTextWrap,
												pState->pTransformState);
			}
			#endif

			/*
			// Now shade the remaining planes
			*/
			SGL_TIME_SUSPEND(MESH_NODE_TIME)
			if (!DoPlaneStuff (pMesh, numProjected, pState, 
							   pMaterialState, nCurrTransSetID))
			{
				break;
			}
		    SGL_TIME_RESUME(MESH_NODE_TIME)

		}/*end for step through the materials*/
	}/*end else partly on screen */
	
	SGL_TIME_STOP(MESH_NODE_TIME)
}



/**************************************************************************
 * Function Name  : RnCTPreProcessMeshNode
 * Inputs         : pMesh- pointer to the convex primitive node
 *					pState- pointer to a master state stack "frame"
 *					ppCachedTexture - pointer to cached texture struct.
 * Outputs        : None
 * Input/Output	  : 
 * Returns        : None
 * Global Used    : See description
 *
 * Description    : This tries to work out if any cached textures are used
 *					by the mesh, and if so, marks their useage/size etc.
 *
 **************************************************************************/

void RnCTPreProcessMeshNode(const MESH_NODE_STRUCT * pMesh, 
								  MASTER_STATE_STRUCT *pState,
								  void *pCachedTexture)
{
	int size,YSize, i;
	PMESHMATERIAL pLocMatEnt;
	PROJECTION_MATRIX_STRUCT  * const pProjMat = RnGlobalGetProjMat ();
	
	/*
	// Transformed Bounding box (if any) in world Coordinates
	*/
	BBOX_MINMAX_STRUCT bboxInWC;

	/*
	// The regions the box spans
	*/	
	REGIONS_RECT_STRUCT Regions;

	int BoxCase;
	sgl_bool bZClipped;


	/*
	// First, if there are definitely NO cached textures affecting this
	// mesh, get out of here
	*/
	if((pCachedTexture== NULL) && !pMesh->HasLocalMaterials)
	{
		return;
	}

	/*
	// If the quality flags say there is no texture - get out as well
	*/
	if(! pState->pQualityState->flags & qf_textures)
	{
		return;
	}


	/*
	// Ok decide if the  mesh is onscreen, and how much of the screen
	// it covers.
	//
	// First transform into world space
	*/

	TransformBBox(pState->pTransformState, &pMesh->CentBBox, &bboxInWC);

	BoxCase = RnTestBoxWithCamera (&bboxInWC, TRUE, &bZClipped);

	if (BoxCase == TB_BOX_OFFSCREEN)
	{
		/*
		// Get out of here
		*/
		DPF((DBG_VERBOSE, "BBox Offscreen:"));
		return;
	}
	else
	{
		int Res = RnDetermineRegionsFromBBox(&bboxInWC, &Regions);

		ASSERT (Res != 0);
	}

	/*
	// Ok map these into sizes
	*/
	size= (Regions.LastXRegion - Regions.FirstXRegion +1) * 
									 pProjMat->RegionInfo.XSize;
	YSize= (Regions.LastYRegion - Regions.FirstYRegion +1) * 
									 pProjMat->RegionInfo.YSize;

	if(YSize > size)
	{
		size = YSize;
	}


	/*
	// Now step through the materials used, (including the first) and
	// set up any cacheable textures. NOTE this doesnt check to see if
	// the texture IS actually used, but who cares - it's close enough
	*/
	if(pCachedTexture != NULL)
	{
		MarkCachedTextureUsed(pCachedTexture, size);
	}
	pLocMatEnt = ListFindItemFast (pMesh->Materials, 0);


	for(i = ListGetItemsInList(pMesh->Materials); i != 0;   i--, pLocMatEnt++)
	{
		void * pPreviousCText;

		pPreviousCText = pCachedTexture;
		/*
		// Update using this material
		// NOTE: Ignore null material entries.
		*/
		if(pLocMatEnt->pMaterial!=NULL)
		{
			RnCTPreprocessMaterialNode(pLocMatEnt->pMaterial, pState, &pCachedTexture);
		}
		/*
		// if the texture has changed, and we still have a cached texture,
		// mark it as used
		*/
		if((pPreviousCText != pCachedTexture) &&(pCachedTexture != NULL))
		{
			MarkCachedTextureUsed(pCachedTexture, size);
		}
		
	}/*end for*/

}


/*
// END OF FILE
*/


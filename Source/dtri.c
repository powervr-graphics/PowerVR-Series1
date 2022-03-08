/******************************************************************************
 * Name         : dtri.c
 * Title        : Low level API top level code for PowerVR
 * Author       : Jim Page
 * Created      : 14/10/1996
 *
 * Copyright	: 1995-2022 Imagination Technologies (c)
 * License		: MIT
 *
 * Description  : 
 *
 * Platform     : ANSI
 *
 * Modifications:
 * $Log: dtri.c,v $
 * Revision 1.143  1997/11/24  15:15:57  gdc
 * removed reference to SGLTT_TEXAPI
 *
 * Revision 1.142  1997/11/11  10:58:00  gdc
 * took range limiting of z-bias value
 *
 * Revision 1.141  1997/11/07  16:00:55  gdc
 * stupid typo
 *
 * Revision 1.140  1997/11/07  15:58:55  gdc
 * added depth bias stuff - which is untested and will need tweaking
 *
 * Revision 1.139  1997/10/28  10:31:52  erf
 * Removed ExtendTSPBuffer() routine and changed calls to
 * PVROSExtendTSPBuffer().
 *
 * Revision 1.138  1997/10/22  17:14:32  sxy
 * added timing metrics for developer's driver.
 *
 * Revision 1.137  1997/10/16  12:09:57  sxy
 * deliberatly over wrote previous version, this is why there is a version number missing.
 *
 * Revision 1.135  1997/10/13  11:12:04  erf
 * Pass in 0 when calling ProcessTriCoreLite() for capatibility sake.
 *
 * Revision 1.134  1997/09/24  16:05:42  sxy
 * changed some DPF to DPFDEV.
 *
 * Revision 1.133  1997/09/10  13:42:34  erf
 * 1. Removed process routines to d3dcore.h and dcore.h, or to the
 *    assembler files d3dtriml.asm and dtriml.asm .
 *
 * Revision 1.132  1997/09/05  13:02:12  sxy
 * Added a large Epsilon to cull more small triangles.
 *
 * Revision 1.131  1997/09/01  15:25:08  dm
 * For nowm  made a couple of fns non-static, so they
 * can be used by D3D fdhgifdshid[3~
 *
 * Revision 1.130  1997/08/26  16:27:14  erf
 * Fixed flat shaded triangles with highlights. PCX1 abd PCX2 require the
 * same fix.
 * Got compiling for PCX1.
 *
 * Revision 1.129  1997/08/26  13:33:36  erf
 * Added fix for flat shaded triangles with specular highlights.
 * This assures that the start TSP address is properly aligned
 * for a bunch of triangles.
 *
 * Revision 1.128  1997/08/22  09:10:57  sxy
 * added code for vertex fog in quads routines.
 *
 * Revision 1.127  1997/08/19  15:15:09  gdc
 * removed stuff I'd removed earlier
 *
 * Revision 1.126  1997/08/19  14:52:27  gdc
 * typo
 *
 * Revision 1.125  1997/08/19  14:31:10  gdc
 * added average colour and vertex fog
 *
 * Revision 1.124  1997/08/19  08:51:03  gdc
 * added texture bit to Vfog control word
 *
 * Revision 1.123  1997/08/18  16:25:27  gdc
 * added more stuff for vertex fog/ smooth highlights
 *
 * Revision 1.122  1997/08/13  16:19:02  gdc
 * started adding stuff for vertex fogging
 *
 * Revision 1.121  1997/08/13  10:32:16  gdc
 * removed process and pack functions to include files - its ugly but
 * this module is getting so bloated its hard to tell what going on
 *
 * Revision 1.120  1997/08/08  19:45:45  mjg
 * Added PCX2_003 enhancements -
 *  Bilinear no longer global, it's per surface (flag in context)
 *  Differenciate between translucent texture formats (4444 and 1555)
 *  Alpha blending defined by flag and blend modes
 *
 * Revision 1.119  1997/08/05  16:37:01  erf
 * Removed D3D triangle processing code which isn't needed.
 *
 * Revision 1.118  1997/07/24  18:15:11  mjg
 * Changed an ASSERT to a DBG_MESSAGE, for when a triangle had two vertices
 * the same (technically a 'line'), as we weren't doing anything about it.
 *
 * Revision 1.117  1997/07/14  19:28:35  mjg
 * Added the PCX2_003 compile switch and MODULE_ID.
 *
 * Revision 1.116  1997/06/27  11:14:56  erf
 * 1. Fixed ProcessD3DStripCore() so that D3D performance is up again.
 * 2. Re-organised code in ProcessD3DStripTransCore() to pre 1.114.
 *
 * Revision 1.115  1997/06/24  17:44:02  erf
 * Added new pass per translucent triangle\quad code into processing
 * routines. For changes just search for SGLTT_NEWPASSPERTRI.
 *
 * Revision 1.114  1997/06/24  15:00:04  sxy
 * fixed a bug in ProcessD3DStripTransCore.
 *
 * Revision 1.113  1997/06/23  21:58:44  jop
 * Added only-extend parameter to SetHighWaterMark
 *
 * Revision 1.112  1997/06/23  17:25:54  sxy
 * moved the code of advance the face pointer in ProcessD3DStripCore, fixed the missing triangles in Hyperblade.
 *
 * Revision 1.111  1997/06/23  16:10:08  jop
 * Fixed bugs in adaptive TSP buffering
 *
 * Revision 1.110  1997/06/22  17:35:29  jop
 * Added adaptive TSP parameter stuff
 *
 * Revision 1.109  1997/06/20  14:44:38  erf
 * The D3D and SGLDirect non-perspective texturing routines are the same
 * so they should call the same routine in dtexnp.c.
 *
 * Revision 1.108  1997/06/18  14:37:17  sxy
 * removed a UINT32.
 *
 * Revision 1.107  1997/06/09  08:35:29  erf
 * Changed name of bogus inverse Z increment to BOGUSINV_INCREMENT.
 *
 * Revision 1.106  1997/06/04  13:51:08  erf
 * Added facility for user to set the bogus inverse Z value. New flag
 * and variable (SGLTT_BOGUSINVZPROVIDED and fBogusInvZ) defined in
 * the context structure.
 *
 * Revision 1.105  1997/05/29  13:18:41  jrc
 * Implemented SGLTT_NEWPASSPERTRI **for D3D format n-polys only**.
 *
 * Revision 1.104  1997/05/27  17:57:35  jop
 * Fixed small bufg in end of ISP buffer detection code
 *
 * Revision 1.103  1997/05/16  18:19:11  erf
 * Copied bogus stuff from normal processing to D3D processing.
 *
 * Revision 1.102  1997/05/16  17:19:27  erf
 * Changed gfBogusZ and fMinZ to gfBogusInvZ and fMinInvZ.
 *
 * Revision 1.101  1997/05/13  17:47:49  erf
 * 1. Added non perspective correction texturing control.
 * 2. Created function arrays which hold the texture functions.
 *    Now any index is calculated to select the required function.
 *
 * Revision 1.100  1997/05/09  14:33:01  jrc
 * 1. Added fix for D3D quad clipping bug.
 * 2. Added small quads mode (flag 0x80000000).
 *
 * Revision 1.99  1997/05/08  10:09:37  erf
 * 1. Removed static from DirectPolygons so that is can be used by the
 *    PVRDirect driver.
 * 2. Added GetTextureHandle() call instead of GetNamedItemType() and
 *    GetNamedItem() etc crap, to reduce the files required by the
 *    PVRDirect driver.
 *
 * Revision 1.98  1997/05/06  16:06:07  jop
 * Added support for both texture handle types
 *
 * Revision 1.97  1997/05/02  07:42:44  erf
 * Removed UINT32's etc and replaced with sgl_uint32's etc.
 *
 * Revision 1.96  1997/04/30  19:33:34  mjg
 * Updated TEXTURE_MAP to HTEXTURE and removed the use of the
 * SGLTT_TEXAPI flag.
 *
 * Revision 1.95  1997/04/24  20:58:34  gdc
 * sgl now initialsed form DllMain in windows builds
 *
 * Revision 1.94  1997/04/23  17:08:38  erf
 * Changed operation of texture filtering for PCX2-002. The filter selection
 * is now made at sgltri_startofframe().
 *
 * Revision 1.93  1997/04/23  13:24:03  gdc
 * added a pragma message
 *
 * Revision 1.92  1997/04/23  09:06:10  gdc
 * made TSPDoubleBuffering static and off temporarily
 * (it slower with it on anyway)
 *
 * Revision 1.91  1997/04/21  18:47:27  mjg
 * Put parmbuff.h back.
 *
 * Revision 1.90  1997/04/21  18:41:35  sxy
 * Added a flag SGLTT_TEXAPI for using texapi functions.
 *
 * Revision 1.89  1997/04/21  15:45:13  mjg
 * Removed 'parambuff.h' include.
 *
 * Revision 1.88  1997/04/21  15:25:56  mjg
 * Replaced ParamBufferInfo with PVRParamBuffs.
 *
 * Revision 1.87  1997/04/17  08:33:54  erf
 * Move the calculation of fHalfTexel for the texel offset fix since the
 * value ->invTexSize must first be calculated. Sorry ho !!!
 *
 * Revision 1.86  1997/04/02  21:28:12  mjg
 * Updated calling conventions.
 *
 * Revision 1.85  1997/03/31  23:39:39  mlj
 * Fixes to get DOS32 driver to build
 *
 * Revision 1.84  1997/03/31  22:06:36  jop
 * rationalised WIN32s, #include "windows.h", and stuck in some DOS32s
 *
 * Revision 1.83  1997/03/26  06:49:42  erf
 * Removed stuff which provides PCX2 bilinear functionality for sglmid4.dll
 * running on PCX2.
 *
 * Revision 1.82  1997/03/25  10:13:06  erf
 * 1. Put in opaque translucent stuff for Graham.
 * 2. Put in control which sets the half texel offset if bilinear filtering
 *    on.
 *
 * Revision 1.81  1997/03/20  19:20:51  jrc
 * Added bilinear function calls (currently always set to bilinear mode).
 *
 * Revision 1.80  1997/03/20  18:10:40  jop
 * Changed name of asm versions of funcs to stop link errors
 *
 * Revision 1.79  1997/03/14  18:09:23  jop
 *  Added pvrosapi.h include!
 *
 * Revision 1.78  1997/03/14  15:56:32  jwf
 * Added HWIO macros for macintosh compatability
 *
 * Revision 1.77  1997/03/14  11:05:01  jwf
 * Made some changes to make it compile on the macintosh
 *
 * Revision 1.76  1997/03/07  21:05:06  jop
 * Added single pass trans stuff
 *
 * Revision 1.75  1997/03/05  12:21:08  gdc
 * coded new entry into gPDC for TexSize
 *
 * Revision 1.74  1997/03/04  20:40:00  erf
 * Pixel offset bug fixed. fAddToXY added to vertices when MODIFYXY defined
 * (defined in dtri.h). Also the reqion calculation if corrected. Now only
 * puts plane data in regions where the plane actually appear (as part or
 * whole). Previously regions were given invalid plane data.
 *
 * Revision 1.73  1997/02/24  11:38:59  gdc
 * changed D3D poly/tri rejection to use the LESS_THAN_EPSILON
 * macro - so it is consistent with SGLLite stuff. Bit redundant
 * as this is all assemblerised anyway.
 *
 * Revision 1.72  1997/02/24  10:37:10  erf
 * Add D3D offset fix to ProcessD3DTriCore() routine.
 *
 * Revision 1.71  1997/02/07  09:23:24  gdc
 * changed flat texture packing back to calling thru a function
 * pointer as it seemed no make no discernable speed improvement
 * but left code (disabled) in just in case...
 *
 * Revision 1.70  1997/02/03  17:54:34  sjf
 * Added code to detect if our exponential fogging is used anywhere.
 *
 * Revision 1.69  1997/02/03  16:12:55  rgi
 * Added support for D3DTriangle assembler by adding support
 * for D3D face format to the existing ProcessTriCoreLite
 * function in dtriml.asm . Also added support to route
 * ProcessD3DStrip[Trans]Core calls to an assembler
 * equivalent.
 *
 * Revision 1.68  1997/01/30  16:48:26  gdc
 * more re-arrangment of Texture packing stuff - TSP double buffer writes
 * are moved into the top level pack functions.
 * Also reduces basic nPolys (IBUFFERSIZE) to 16 as this seems to give
 * better D3DTest performance - I assume 32 nPolys might have been going slightly
 * out of primary cache
 *
 * Revision 1.67  1997/01/28  14:47:58  gdc
 * added the sizeof the material struct to the tex function parameters
 *
 * Revision 1.66  1997/01/27  18:26:05  gdc
 * reworked texture packing variants to support large PCI writes on a nPoly
 * basis if required
 *
 * Revision 1.65  1997/01/27  09:12:48  gdc
 * changed to use storage functions when packing involves a true texture
 * at the momemnt this must be compiled with C texture functions,
 * as texture function now writes out all Texas data
 *
 * Revision 1.64  1997/01/24  11:48:14  jrc
 * Commented out native shading code for the time being.
 *
 * Revision 1.63  1997/01/24  09:22:05  rgi
 * In-lined the texture per polygon code to allow it to be optimised
 * into the top level process routines. Sadly the assembler version
 * of ProcessD3DTriCore has to be disabled at this point. It will
 * return.
 *
 * Revision 1.62  1997/01/20  16:09:04  sjf
 * Urk. John C found a bug in the "clipping" code that resulted from
 * the floating point compares. If ALL the Y values, or ALL the X vals
 * for the vertices were negative, the floating point compares would
 * get the max and min around the wrong way which messes up the clip
 * code.  Have now put a test in to reverse these. Clipping is probably
 * not that common so the extra test isn't going to cost too much.
 *
 * Revision 1.61  1997/01/18  01:01:41  jrc
 * First go at native shading API (not finished).
 *
 * Revision 1.60  1997/01/17  09:45:46  gdc
 * fixed an assert that wouldn't compile
 *
 * Revision 1.59  1997/01/16  16:50:13  jrc
 * Put AddToXY fix back in for Agile Warrior.  Only done for translucent D3D
 * triangles.
 * Also improved an assert slightly.
 *
 * Revision 1.58  1997/01/16  13:06:48  rgi
 * Added support for assembler version of ProcessTriCoreLite
 * and tidied up some unused stuff.
 *
 * Revision 1.57  1997/01/15  13:20:48  rgi
 * Passed the size of the IMATERIAL structure explicitly into
 * the PackTSP routines that may be implemented in assembler.
 * .#
 *
 * Revision 1.56  1997/01/13  17:01:49  sjf
 * Put the flat shade fix into the C version of ProcessD3DtriCore
 * as well, buit as this is currently using an assembler version, it's
 * not going to achieve much. Note that the Strip routines may still be
 * wrong.
 *
 * Revision 1.55  1997/01/13  12:41:17  sjf
 * Added some very minor optimisations to my 3 fav' routines, to do
 * with the translucent cases.
 *
 * Revision 1.54  1997/01/13  12:14:25  sjf
 * 1) Fixed bug with flat shading colour being chosen from the wrong vertex
 *    when a back-facing polygon was flipped BUT only in TriCoreTrans thru to
 *    QuadCore. STILL NEEDS TO BE FIXED in 
 *							D3DTriCore
 *							D3DStripCore
 *					  and	D3DStripTransCore
 *			   
 * 2) Got the floating point divide to overlap with loads of integer in those
 *    routines as well.
 *
 * Revision 1.53  1997/01/10  19:52:42  jrc
 * Simon's fix for his performance stuff.
 * Initial fix (switched off for now) for testing pixel bounding box thing.
 *
 * Revision 1.52  1997/01/10  14:10:57  sjf
 * Hopefully improved the bounding box calc for the 3 routines
 * xxxTriCoreTrans through to QuadCore, by doing the comparisons
 * in "float" and then only at the end converting them to integer.
 *
 * Revision 1.51  1997/01/10  01:35:03  sjf
 * Moved one of the colour averaging macros out to sglmacro.h as another
 * file needed it.
 *
 * Revision 1.50  1997/01/09  17:08:03  rgi
 * Changed the TEXTURE handling code to actually do
 * something useful during the first pass. Also split
 * out at this level PackTSPTex for MipMapped and none
 * MipMapped cases. All per poly process functions that
 * involve texture now call the appropriate ProcessFlatTex
 * handler at the end and hence don't increment gpCurrentMat
 *
 * Revision 1.49  1997/01/08  16:28:07  sjf
 * Fixed bugs in the optimisation code with flipping of back faces.
 *
 * Revision 1.48  1997/01/08  16:19:37  sjf
 * Colour mask for new highlight averaging was off 4 bits for green.
 *
 * Revision 1.47  1997/01/08  15:53:26  sjf
 * Fixed bug with Process..TriCoreTrans through to QuadCore  to do
 * with the optimisations. I'd mixed up gpPDC (which is R/W) with
 * gPDC which is a read-only version and the compiler was optimising
 * out stuff which was actually changing, but it was effectively
 * being told it wasn't (because of the const).
 *
 * Revision 1.46  1997/01/07  19:39:35  sjf
 * WARNING Untested Code!
 * 1) OPtimised the colour and translucency averaging code
 * 2) Some optimisation of the non-assemlerised triangle/quad routines
 *
 * Revision 1.45  1997/01/03  14:32:38  rgi
 * Added fake timer slots to record the precise number of
 * objects input via the SGLLite API.
 *
 * Revision 1.44  1996/12/19  02:59:33  jrc
 * Fixed average colour highlight macro.
 *
 * Revision 1.43  1996/12/19  01:50:56  jrc
 * Added a branch to handle average colour case
 *
 * Revision 1.42  1996/12/19  01:41:29  jop
 * Replaced all the material implementations with macros
 *
 * Revision 1.41  1996/12/19  00:15:40  jop
 * Some little fixes plus rockem
 *
 * Revision 1.40  1996/12/17  23:48:31  jop
 * Slightly improved John's reversed poly algorithm
 *
 * Revision 1.39  1996/12/13  19:06:57  jrc
 * Added n sided poly support
 *
 * Revision 1.38  1996/12/11  03:42:29  jop
 * Disabled chunking for now
 *
 * Revision 1.37  1996/12/11  01:01:23  mjg
 * Included 'dregion.h' as it made the sun simulator happy.
 *
 * Revision 1.36  1996/12/10  23:45:12  erf
 * Changed compile switches:
 * 1. PVR1 to ISPTSP.
 * 2. PVR2 to PCX1.
 * 3. PVR3 to PCX2.
 *
 * Revision 1.35  1996/12/10  20:16:43  jrc
 * Started converting pollygon code to new edge format.
 *
 * Revision 1.34  1996/12/10  18:29:51  jop
 * Added bogus z stuff
 *
 * Revision 1.33  1996/12/09  14:06:58  gdc
 * changed names of DirectTriangles and DirectQuads to sgltri_triangles
 * and sgltri_quads, so that API entry points are no longer thru sgltri.c
 *
 * Revision 1.32  1996/12/05  20:55:47  jop
 * Removed asm compile flags to dtri.h
 *
 * Revision 1.31  1996/12/05  18:04:14  jop
 * Made fMinZ and fAddToXY extern
 *
 * Revision 1.30  1996/12/05  11:59:13  erf
 * Changed '.' to '->' in line 4890.
 *
 * Revision 1.29  1996/12/03  15:54:17  rgi
 * Assembler version of ProcessD3DStripClippingCore introduced.
 * The only benifit in the assembler is currently that the
 * divide is scheduled better.
 *
 * Revision 1.28  1996/12/03  01:35:41  mjg
 * Turned CHUNKING off for Midas3 and for the mean time, left it enabled for
 * Midas4 (and 5).
 *
 * Revision 1.27  1996/12/03  00:17:57  jrc
 * Fixed intersection test bug.
 *
 * Revision 1.26  1996/12/02  20:11:49  jop
 * Tidied up Lite/D3D format support
 *
 * Revision 1.25  1996/12/02  16:47:10  jop
 * Added support for sgl lite quads
 *
 * Revision 1.24  1996/11/30  04:54:48  jop
 * Went through poly code like an old-testament prophet. It now
 * looks simpler, but probably isn't. Took out COMBINE code as
 * Hossein wants it out, but we will probably have to merge it back in
 * again. Sorry John. Anyway, tidied the interface and made it all
 * a bit more efficient.
 *
 * Revision 1.23  1996/11/29  22:37:50  jop
 * Fixed chunking
 *
 * Revision 1.22  1996/11/29  19:41:34  jop
 * Lots of things
 *
 * Revision 1.21  1996/11/28  12:35:32  rgi
 * Added a mega-bodge to allow the compiler to optimise the code
 * better. Essentially only the parts that need to setup gPDC view
 * it as a modifiable structure (via the pointer gpPDC) while all
 * the rest of the processing code which do not modify it view it
 * as a const structure. This allows the compiler to presume that
 * fields in gPDC do not change because it assigns a value to ptr->field in a code
 * block.
 *
 * Revision 1.20  1996/11/28  10:31:52  gdc
 * translucent textures now use function pointers as if global translucent
 * is on - this avoids trans-shad-smooth which causes render abort
 *
 * Revision 1.19  1996/11/27  17:53:29  gdc
 * added support for non-textured smooth shaded translucency
 *
 * Revision 1.18  1996/11/26  00:20:53  jop
 * Updated strip code to handle trans passes better
 *
 * Revision 1.17  1996/11/25  23:57:48  jop
 * *** empty log message ***
 *
 * Revision 1.16  1996/11/25  18:15:28  gdc
 * added even more process options in a very linear way - just ripe
 * for a clever (and bored) #define writer
 *
 * Revision 1.15  1996/11/25  13:33:27  gdc
 * WIN32'd assembler call to ProcessD3DTriCore and put back a C
 * version for arcader's
 *
 * Revision 1.14  1996/11/23  05:01:38  jop
 * Added proper start of trans pass stuff
 *
 * Revision 1.13  1996/11/23  04:56:12  jop
 * Merged Rob's branch and added some elementary translucency processing
 *
 * Revision 1.8.1.1  1996/11/22  18:31:30  rgi
 * Updated to work with the latest dispml.asm
 *
 * Revision 1.12  1996/11/23  04:15:51  jop
 * Made some structure changes
 *
 * Revision 1.11  1996/11/23  02:33:37  jop
 * Changed texturing code a bit and twiddled the alpha stuff
 * to use integers instead of floats. Also references dfunc.h
 * to get material-appropriate functions and data
 *
 * Revision 1.10  1996/11/23  02:26:56  jrc
 * First attempt at polygon combining.
 *
 * Revision 1.9  1996/11/22  02:27:15  jrc
 * First attempt at polygons.
 *
 * Revision 1.8  1996/11/21  17:25:12  gdc
 * added remainder of texture and translucency stuff
 *
 * Revision 1.7  1996/11/21  16:59:54  gdc
 * added in stuff for non textured translucency and started on translucency
 * with textures
 *
 * Revision 1.6  1996/11/21  16:24:39  jrc
 * 1. Renamed nPolys in global context to nInputTriangles.
 * 2. Backfacing triangles are now turned around by swapping vertices 0 and 2
 *    instead of 1 and 2 (so polygons always use first three vertices for
 *    texturing and shading).
 * 3. Removed D3DFACE structure because we should be using the identical
 *    SGLD3DFACE structure in sgl.h instead.
 * 4. Separate versions of the ProcessD3DTriCore function are now used.
 *
 * Revision 1.5  1996/11/20  18:34:41  gdc
 * added support for shadows and light volumes
 *
 * Revision 1.4  1996/11/19  19:11:30  gdc
 * started adding shadow and light volume options
 *
 * Revision 1.3  1996/11/19  16:47:00  rgi
 * Performance Update, split off assember equivalent
 * of ProcessD3DTriCore at this point.
 *
 * Revision 1.2  1996/11/19  00:57:47  jop
 * Loads of stuff including general tidy up of i/f and added
 * smooth shading
 *
 * Revision 1.1  1996/11/18  18:30:06  jop
 * Initial revision
 *
 *
 *****************************************************************************/

#define MODULE_ID MODID_DTRI

#include "sgl_defs.h"
#include "sgl_init.h"
#include "pvrosapi.h"
#include "metrics.h"
#include "hwinterf.h"
#include "sgl_math.h"

#include "nm_intf.h"
#include "getnamtb.h"
#include "dlntypes.h"

#include "dtri.h"
#include "dshade.h"
#include "dtex.h"
#include "disp.h"
#include "dtsp.h"
#include "pmsabre.h"
#include "dregion.h"
#include "parmbuff.h"
#include "pvrlims.h"
#include "texapi.h"

SGL_EXTERN_TIME_REF /* if we are timing code */

#if WIN32 || DOS32

	#define DO_FPU_PRECISION TRUE

	void __cdecl SetupFPU (void);
	void __cdecl RestoreFPU (void);

#else

	#define DO_FPU_PRECISION FALSE

#endif

/*
// Maximum number of vertices in a convex flat fan that can form a polyhedron
// sent to the hardware as a single object.  Direct3D supports 32 vertex flat
// fans, but the current code splits these up into polygons with smaller
// numbers of vertices.
*/
#define MAX_POLYGON_VERTICES  8

/* Group adjacent, translucent objects into the same translucent pass */
/* Note:the chunking version does not check for the SGLTT_NEWPASSPERTRI flag */
#if ISPTSP
	#define CHUNKING FALSE
#elif PCX1 || PCX2 || PCX2_003
	  #define CHUNKING FALSE /*TRUE*/
#endif

/*
// ============================================================================
// 								STATIC VARIABLES:
// ============================================================================
*/
const PROCESSDATACONTEXT gPDC;
PROCESSDATACONTEXT *const gpPDC = (PROCESSDATACONTEXT*) &gPDC;

ITRI		gpTri[IBUFFERSIZE];
IEDGE		gpEdge[IEDGEBUFFERSIZE];
IMATERIAL 	gpMat[IBUFFERSIZE];
PIMATERIAL	gpMatCurrent = gpMat;

float gfDepthBias;

extern HLDEVICE gHLogicalDev;
extern float fMinInvZ;
extern float gfBogusInvZ;
extern sgl_bool FogUsed;

static int TSPDoubleBuffer = 0; /* disable TSP double buffing till PVROS sorted */ 
#pragma message ("Another extern to sort out")
#pragma message ("TSP Double buffer hard coded to OFF")

/* Variable to fix the pixel offset problem. The value is added to
 * all vertices in D3D and SGL-Lite.
 */
extern float 	fAddToXY;

/* typedef for the pack TSP parameter functions.
 */
typedef	int (*PACKTSPFUNCTION) (PIMATERIAL, int, int, sgl_uint32, sgl_uint32 *, int);

PACKTSPFUNCTION PackTSPTexFn;

/* Initialise the 4 pack functions. These are indexed as required.
 */
static PACKTSPFUNCTION PackTSPFunctions[4] = {	PackTSPTex,
												PackTSPMipMapTex,
									   			PackTSPTexNonPer,
								   				PackTSPMipMapTexNonPer
									   			};

/* typedef for the process TSP parameter functions.
 */
typedef	void (*PROCESSTSPFUNCTION)(PITRI);

PROCESSTSPFUNCTION ProcessFlatTexFn;

/* Initialise the 8 D3D pack functions. These are indexed as required.
 * The SGLDirect and D3D non-perspective correct flat textured cases are
 * the same.
 */
static PROCESSTSPFUNCTION ProcessD3DTSPFunctions[8] = {	ProcessD3DFlatTex,
														ProcessD3DFlatTexWrapU,
														ProcessD3DFlatTexWrapV,
														ProcessD3DFlatTexWrapUV,
														ProcessFlatTexNonPer,
														ProcessD3DFlatTexWrapUNonPer,
														ProcessD3DFlatTexWrapVNonPer,
														ProcessD3DFlatTexWrapUVNonPer
														};

/* Initialise the 2 pack functions. These are indexed as required.
 */
static PROCESSTSPFUNCTION ProcessTSPFunctions[2] =	   {ProcessFlatTex,
														ProcessFlatTexNonPer
														};


#define FLATTEX 0x00000000
#define SMOOTHTEX 0x00000001
#define HIGHTEX 0x00000002
#define SMOOTHSHADTEX 0x00000004

#include "dpack.h"
#include "dprocess.h"
#include "dcore.h"

/**********************************************************************
**
**
**
**
**********************************************************************/

static void ProcessD3DPolys (PVRHANDLE TexHeap, PPIR pPerPolyfn, PBPR pPerBuffn, sgl_uint32 TSPWords)
{
	sgl_uint32	TSPAddr, TSPInc;
	sgl_uint32 	TSPSpaceAvailable;
	sgl_uint32	*pTSP;
	sgl_uint32	NewObject = TRUE;
	
	while ( gpPDC->nInputTriangles != 0 )
	{
		int nBurst;
		
		SGL_TIME_SUSPEND(SGLTRI_TRIANGLES_TIME)
		SGL_TIME_START(SGLTRI_PROCESS_TIME)

		/* Process as many as possible or all the remainder        */
		gpMatCurrent = gpMat;			/* pPerPolyFn updates this */
		
		if (gpPDC->TSPControlWord & MASK_TRANS)
		{
			nBurst = ProcessD3DStripTransCore ( pPerPolyfn, NewObject );

			NewObject = FALSE;
		}
		else
		{
			nBurst = ProcessD3DStripCore ( pPerPolyfn );
		}
		
		SGL_TIME_STOP(SGLTRI_PROCESS_TIME)
		SGL_TIME_RESUME(SGLTRI_TRIANGLES_TIME)

		/* Process the content of the buffer */
		SGL_TIME_SUSPEND(SGLTRI_TRIANGLES_TIME)
		SGL_TIME_START(SGLTRI_PACKTRI_TIME)
		
		/*
			Work out how many triangle's worth of data we can
			actually put in there ...
		*/
		TSPSpaceAvailable = PVRParamBuffs[PVR_PARAM_TYPE_TSP].uBufferLimit -
							PVRParamBuffs[PVR_PARAM_TYPE_TSP].uBufferPos;
		
#if PCX2 || PCX2_003
		if((gpPDC->Context.u32Flags & SGLTT_VERTEXFOG) && 
		   (gpPDC->Context.u32Flags & (SGLTT_HIGHLIGHT | SGLTT_GOURAUD)))
		{
			/* calculate maximum possible TSP usage - haven't a clue
			** what the actual usage is yet
			*/
			if (TSPSpaceAvailable < (nBurst * (TSPWords + 16)))
			{
				TSPSpaceAvailable = PVROSExtendTSPBuffer (gHLogicalDev);
				
				if (TSPSpaceAvailable < (nBurst * (TSPWords + 16)))
				{
					/* buffer full so break after this pass */
					gpPDC->nInputTriangles = 0;
					
					/*
					  This division should only take place if the buffer
					  is nearly full
					  */
					TSPSpaceAvailable /= (TSPWords + 16);
					nBurst = TSPSpaceAvailable;
				}
			}
		}
		else if((gpPDC->Context.u32Flags & SGLTT_VERTEXFOG) || 
				(gpPDC->Context.u32Flags & (SGLTT_HIGHLIGHT | SGLTT_GOURAUD)))
		{
			/* calculate maximum possible TSP usage - haven't a clue
			** what the actual usage is yet
			*/
			if (TSPSpaceAvailable < (nBurst * (TSPWords + 8)))
			{
				TSPSpaceAvailable = PVROSExtendTSPBuffer (gHLogicalDev);
				
				if (TSPSpaceAvailable < (nBurst * (TSPWords + 8)))
				{
					/* buffer full so break after this pass */
					gpPDC->nInputTriangles = 0;
					
					/*
					  This division should only take place if the buffer
					  is nearly full
					  */
					TSPSpaceAvailable /= (TSPWords + 8);
					nBurst = TSPSpaceAvailable;
				}
			}
		}
		else
#endif
		{

			if (TSPSpaceAvailable < (nBurst * TSPWords))
			{
				TSPSpaceAvailable = PVROSExtendTSPBuffer (gHLogicalDev);

				if (TSPSpaceAvailable < (nBurst * TSPWords))
				{
					/* buffer full so break after this pass */
					gpPDC->nInputTriangles = 0;
				
					/*
					   This division should only take place if the buffer
				   	is nearly full
					*/
					TSPSpaceAvailable /= TSPWords;
					nBurst = TSPSpaceAvailable;
				}
			}
		}

		/* Index of start of TSP parms */
		TSPAddr = PVRParamBuffs[PVR_PARAM_TYPE_TSP].uBufferPos;

		/* Get address of buffer in host-land */
		pTSP = PVRParamBuffs[PVR_PARAM_TYPE_TSP].pBuffer + TSPAddr;

#if PCX2 || PCX2_003
		if((gpPDC->Context.u32Flags & SGLTT_VERTEXFOG) || 
		   (gpPDC->Context.u32Flags & (SGLTT_HIGHLIGHT | SGLTT_GOURAUD)))
		{
			/*
			** Note: all the TSP data for the extra planes is stored 
			** after the data for the normal planes - thererfore 
			** a TSP offset needs be calculted for the planes STP pointer
			*/

			TSPInc = PackISPPolygonExtra(gpTri, gpMat, gpEdge, nBurst, 
									 TSPAddr >> 1, TSPWords >> 1);
			/* don't know how much tsp space used till return */
			PVRParamBuffs[PVR_PARAM_TYPE_TSP].uBufferPos += (TSPInc<<1);

			if ( pPerBuffn != NULL )
			{
				/* Call additonal type specific handler */
				pPerBuffn (pTSP, nBurst);
			}
			else
			{
				/* Call flat shading packer */
				PackTSPFlat (gpTri, gpMat, nBurst, 2, pTSP);
			}
			/* I'm not sure if the TSP offset is correct */
			PackExtra(gpTri, gpMat, nBurst, 8, pTSP+nBurst*TSPWords);
		}
		else
#endif
		{
	  		PackISPPolygon (gpTri, gpMat, gpEdge, nBurst, TSPAddr >> 1, TSPWords >> 1);
			/* all objects are of known TSP size */
			PVRParamBuffs[PVR_PARAM_TYPE_TSP].uBufferPos += TSPWords * nBurst;
		
			if ( pPerBuffn != NULL )
			{
				/* Call additonal type specific handler */
				pPerBuffn (pTSP, nBurst);
			}
			else
			{
				/* Call flat shading packer */
				PackTSPFlat (gpTri, gpMat, nBurst, 2, pTSP);
			}
		}
		SGL_TIME_STOP(SGLTRI_PACKTRI_TIME)
		SGL_TIME_RESUME(SGLTRI_TRIANGLES_TIME)
	}
}

/**********************************************************************
**
**
**
**
**********************************************************************/

static void ProcessTris (PVRHANDLE TexHeap, 
						 PPIR pPerPolyfn, 
						 PBPR pPerBuffn, 
						 sgl_uint32 TSPWords)
{
	sgl_uint32	TSPAddr, TSPInc;
	sgl_uint32 	TSPSpaceAvailable;
	sgl_uint32	*pTSP;
	sgl_uint32	NewObject = TRUE;
	
	while ( gpPDC->nInputTriangles != 0 )
	{
		int nBurst;
		
		SGL_TIME_SUSPEND(SGLTRI_TRIANGLES_TIME)
		SGL_TIME_START(SGLTRI_PROCESS_TIME)

		/* Process as many as possible or all the remainder        */
		gpMatCurrent = gpMat;			/* pPerPolyFn updates this */

		nBurst = ProcessTriCoreLite ( pPerPolyfn, NewObject, 0);
		NewObject = FALSE;
		
		SGL_TIME_STOP(SGLTRI_PROCESS_TIME)
		SGL_TIME_RESUME(SGLTRI_TRIANGLES_TIME)

		/* Process the content of the buffer */
		SGL_TIME_SUSPEND(SGLTRI_TRIANGLES_TIME)
		SGL_TIME_START(SGLTRI_PACKTRI_TIME)
		
		/*
			Work out how many triangle's worth of data we can
			actually put in there ...
		*/
		TSPSpaceAvailable = PVRParamBuffs[PVR_PARAM_TYPE_TSP].uBufferLimit -
							PVRParamBuffs[PVR_PARAM_TYPE_TSP].uBufferPos;
		
#if PCX2 || PCX2_003
		if((gpPDC->Context.u32Flags & SGLTT_VERTEXFOG) && 
		   (gpPDC->Context.u32Flags & (SGLTT_HIGHLIGHT | SGLTT_GOURAUD)))
		{
			/* calculate maximum possible TSP usage - haven't a clue
			** what the actual usage is yet
			*/
			if (TSPSpaceAvailable < (nBurst * (TSPWords + 16)))
			{
				TSPSpaceAvailable = PVROSExtendTSPBuffer (gHLogicalDev);
				
				if (TSPSpaceAvailable < (nBurst * (TSPWords + 16)))
				{
					/* buffer full so break after this pass */
					gpPDC->nInputTriangles = 0;
					
					/*
					  This division should only take place if the buffer
					  is nearly full
					  */
					TSPSpaceAvailable /= (TSPWords + 16);
					nBurst = TSPSpaceAvailable;
				}
			}
		}
		else if((gpPDC->Context.u32Flags & SGLTT_VERTEXFOG) || 
				(gpPDC->Context.u32Flags & (SGLTT_HIGHLIGHT | SGLTT_GOURAUD)))
		{
			/* calculate maximum possible TSP usage - haven't a clue
			** what the actual usage is yet
			*/
			if (TSPSpaceAvailable < (nBurst * (TSPWords + 8)))
			{
				TSPSpaceAvailable = PVROSExtendTSPBuffer (gHLogicalDev);
				
				if (TSPSpaceAvailable < (nBurst * (TSPWords + 8)))
				{
					/* buffer full so break after this pass */
					gpPDC->nInputTriangles = 0;
					
					/*
					  This division should only take place if the buffer
					  is nearly full
					  */
					TSPSpaceAvailable /= (TSPWords + 8);
					nBurst = TSPSpaceAvailable;
				}
			}
		}
		else
#endif
		{
			if (TSPSpaceAvailable < (nBurst * TSPWords))
			{
				TSPSpaceAvailable = PVROSExtendTSPBuffer (gHLogicalDev);
				
				if (TSPSpaceAvailable < (nBurst * TSPWords))
				{
					/* buffer full so break after this pass */
					gpPDC->nInputTriangles = 0;
					
					/*
					  This division should only take place if the buffer
					  is nearly full
					  */
					TSPSpaceAvailable /= TSPWords;
					nBurst = TSPSpaceAvailable;
				}
			}
		}
		/* Index of start of TSP parms */
		TSPAddr = PVRParamBuffs[PVR_PARAM_TYPE_TSP].uBufferPos;

		/* Get address of buffer in host-land */
		pTSP = PVRParamBuffs[PVR_PARAM_TYPE_TSP].pBuffer + TSPAddr;

#if PCX2 || PCX2_003
		if((gpPDC->Context.u32Flags & SGLTT_VERTEXFOG) || 
		   (gpPDC->Context.u32Flags & (SGLTT_HIGHLIGHT | SGLTT_GOURAUD)))
		{
			/*
			** Note: all the TSP data for the extra planes is stored 
			** after the data for the normal planes - thererfore 
			** a TSP offset needs be calculted for the planes STP pointer
			*/

			TSPInc = PackISPTriExtra(gpTri, gpMat, nBurst, 
									 TSPAddr >> 1, TSPWords >> 1);
			/* don't know how much tsp space used till return */
			PVRParamBuffs[PVR_PARAM_TYPE_TSP].uBufferPos += (TSPInc<<1);

			if ( pPerBuffn != NULL )
			{
				/* Call additonal type specific handler */
				pPerBuffn (pTSP, nBurst);
			}
			else
			{
				/* Call flat shading packer */
				PackTSPFlat (gpTri, gpMat, nBurst, 2, pTSP);
			}
			/* I'm not sure if the TSP offset is correct */
			PackExtra(gpTri, gpMat, nBurst, 8, pTSP+nBurst*TSPWords);
		}
		else
#endif
		{
			PackISPTri(gpTri, nBurst, TSPAddr >> 1, TSPWords >> 1);
			/* all objects are of known TSP size */
			PVRParamBuffs[PVR_PARAM_TYPE_TSP].uBufferPos += TSPWords * nBurst;

			if ( pPerBuffn != NULL )
			{
				/* Call additonal type specific handler */
				pPerBuffn (pTSP, nBurst);
			}
			else
			{
				/* Call flat shading packer */
				PackTSPFlat (gpTri, gpMat, nBurst, 2, pTSP);
			}
		}

		SGL_TIME_STOP(SGLTRI_PACKTRI_TIME)
		SGL_TIME_RESUME(SGLTRI_TRIANGLES_TIME)
	}
}

/**********************************************************************
**
**
**
**
**********************************************************************/

static void ProcessQuads (PVRHANDLE TexHeap, PPIR pPerPolyfn, PBPR pPerBuffn, sgl_uint32 TSPWords)
{
	sgl_uint32	TSPAddr, TSPInc;
	sgl_uint32 	TSPSpaceAvailable;
	sgl_uint32	*pTSP;
	sgl_uint32	NewObject = TRUE;
	
	while ( gpPDC->nInputTriangles != 0 )
	{
		int nBurst;
		
		SGL_TIME_SUSPEND(SGLTRI_TRIANGLES_TIME)
		SGL_TIME_START(SGLTRI_PROCESS_TIME)

		/* Process as many as possible or all the remainder        */
		gpMatCurrent = gpMat;			/* pPerPolyFn updates this */
		
		nBurst = ProcessQuadCore ( pPerPolyfn, NewObject );
		NewObject = FALSE;
		
		SGL_TIME_STOP(SGLTRI_PROCESS_TIME)
		SGL_TIME_RESUME(SGLTRI_TRIANGLES_TIME)

		/* Process the content of the buffer */
		SGL_TIME_SUSPEND(SGLTRI_TRIANGLES_TIME)
		SGL_TIME_START(SGLTRI_PACKTRI_TIME)
		
		/*
			Work out how many triangle's worth of data we can
			actually put in there ...
		*/
		TSPSpaceAvailable = PVRParamBuffs[PVR_PARAM_TYPE_TSP].uBufferLimit -
							PVRParamBuffs[PVR_PARAM_TYPE_TSP].uBufferPos;
		
		
#if PCX2 || PCX2_003
		if((gpPDC->Context.u32Flags & SGLTT_VERTEXFOG) && 
		   (gpPDC->Context.u32Flags & (SGLTT_HIGHLIGHT | SGLTT_GOURAUD)))
		{
			/* calculate maximum possible TSP usage - haven't a clue
			** what the actual usage is yet
			*/
			if (TSPSpaceAvailable < (nBurst * (TSPWords + 16)))
			{
				TSPSpaceAvailable = PVROSExtendTSPBuffer (gHLogicalDev);
				
				if (TSPSpaceAvailable < (nBurst * (TSPWords + 16)))
				{
					/* buffer full so break after this pass */
					gpPDC->nInputTriangles = 0;
					
					/*
					  This division should only take place if the buffer
					  is nearly full
					  */
					TSPSpaceAvailable /= (TSPWords + 16);
					nBurst = TSPSpaceAvailable;
				}
			}
		}
		else if((gpPDC->Context.u32Flags & SGLTT_VERTEXFOG) || 
				(gpPDC->Context.u32Flags & (SGLTT_HIGHLIGHT | SGLTT_GOURAUD)))
		{
			/* calculate maximum possible TSP usage - haven't a clue
			** what the actual usage is yet
			*/
			if (TSPSpaceAvailable < (nBurst * (TSPWords + 8)))
			{
				TSPSpaceAvailable = PVROSExtendTSPBuffer (gHLogicalDev);
				
				if (TSPSpaceAvailable < (nBurst * (TSPWords + 8)))
				{
					/* buffer full so break after this pass */
					gpPDC->nInputTriangles = 0;
					
					/*
					  This division should only take place if the buffer
					  is nearly full
					  */
					TSPSpaceAvailable /= (TSPWords + 8);
					nBurst = TSPSpaceAvailable;
				}
			}
		}
		else
#endif
		{
			if (TSPSpaceAvailable < (nBurst * TSPWords))
			{
				TSPSpaceAvailable = PVROSExtendTSPBuffer (gHLogicalDev);

				if (TSPSpaceAvailable < (nBurst * TSPWords))
				{
					/* buffer full so break after this pass */
					gpPDC->nInputTriangles = 0;
				
					/*
					   This division should only take place if the buffer
				  	 is nearly full
					*/
					TSPSpaceAvailable /= TSPWords;
					nBurst = TSPSpaceAvailable;
				}
			}
		}

		/* Index of start of TSP parms */
		TSPAddr = PVRParamBuffs[PVR_PARAM_TYPE_TSP].uBufferPos;

		/* Get address of buffer in host-land */
		pTSP = PVRParamBuffs[PVR_PARAM_TYPE_TSP].pBuffer + TSPAddr;

#if PCX2 || PCX2_003
		if((gpPDC->Context.u32Flags & SGLTT_VERTEXFOG) || 
		   (gpPDC->Context.u32Flags & (SGLTT_HIGHLIGHT | SGLTT_GOURAUD)))
		{
			/*
			** Note: all the TSP data for the extra planes is stored 
			** after the data for the normal planes - thererfore 
			** a TSP offset needs be calculted for the planes STP pointer
			*/

			TSPInc = PackISPPolygonExtra (gpTri, gpMat, gpEdge, nBurst,
									 TSPAddr >> 1, TSPWords >> 1);

			/* don't know how much tsp space used till return */
			PVRParamBuffs[PVR_PARAM_TYPE_TSP].uBufferPos += (TSPInc<<1);

			if ( pPerBuffn != NULL )
			{
				/* Call additonal type specific handler */
				pPerBuffn (pTSP, nBurst);
			}
			else
			{
				/* Call flat shading packer */
				PackTSPFlat (gpTri, gpMat, nBurst, 2, pTSP);
			}
			/* I'm not sure if the TSP offset is correct */
			PackExtra(gpTri, gpMat, nBurst, 8, pTSP+nBurst*TSPWords);
		}
		else
#endif
		{
			PackISPPolygon (gpTri, gpMat, gpEdge, nBurst, TSPAddr >> 1, TSPWords >> 1);
			/* all objects are of known TSP size */
			PVRParamBuffs[PVR_PARAM_TYPE_TSP].uBufferPos += TSPWords * nBurst;
		
			if ( pPerBuffn != NULL )
			{
				/* Call additonal type specific handler */
				pPerBuffn (pTSP, nBurst);
			}
			else
			{
				/* Call flat shading packer */
				PackTSPFlat (gpTri, gpMat, nBurst, 2, pTSP);
			}
		}

		SGL_TIME_STOP(SGLTRI_PACKTRI_TIME)
		SGL_TIME_RESUME(SGLTRI_TRIANGLES_TIME)
	}
}

/*
// ============================================================================
// 							  SGL EXTERNAL ROUTINES:
// ============================================================================
*/

#include "dfunc.h"

#if PCX2_003
/* In order to insert textures into the correct pass, 
 * we have to examine the alphamask to identify the type.
 */
#include "tmalloc.h"
typedef struct
{
  PTEXTURESPEC pTextureSpec;
  MNODE_BLOCK MemBlock;

} TPRIVATEDATA;	
#endif /* PCX2_003 */

#ifdef DLL_METRIC
extern sgl_uint32 nTransPolygonsInFrame, nOpaquePolygonsInFrame;
#endif

extern DEVICE_REGION_INFO_STRUCT RegionInfo; /* In dregion.c */

void DirectPolygons ( PVRHANDLE TexHeap,
					  PSGLCONTEXT  pContext,
						int  nNumFaces,
					    int  pFaces[][3],
						PSGLVERTEX  pVertices,
						sgl_bool bQuads )
{
	TEXAS_PRECOMP_STRUCT  TPS;
	PIFUNCBLOCK			  pFuncBlock;
	PPIR				  fnPerPoly;
	sgl_uint32			  uFuncBlockIndex;
	sgl_int32 			  ISPSpaceAvailable;
	sgl_uint32            nExtraPlanes = 0;  

	ISPSpaceAvailable = ( (PVRParamBuffs[PVR_PARAM_TYPE_ISP].uBufferLimit) -
						  (PVRParamBuffs[PVR_PARAM_TYPE_ISP].uBufferPos) );


	/* Calculate extra planes needed for pervertex for and gouraud highlights */
	/* this will be the maximum - we might not need all this as if we detect
	** that a poly has zero fog  we will not add the extra plane used for
	** fogging
	*/
	if(pContext->u32Flags & SGLTT_VERTEXFOG)
	{
		nExtraPlanes++;
	}

	if(pContext->u32Flags & (SGLTT_HIGHLIGHT | SGLTT_GOURAUD))
	{
		nExtraPlanes++;
	}

	if(pContext->u32Flags & SGLTT_DEPTHBIAS)
	{
		gfDepthBias = BOGUSINVZ_INCREMENT * (float)(pContext->uDepthBias);
	}
	else
	{
		gfDepthBias = 0.0f;
	}

	if ( ISPSpaceAvailable < (nNumFaces * WORDS_PER_PLANE * 
							  (4 + bQuads + nExtraPlanes)) )
	{
		if (ISPSpaceAvailable > 0)
		{
			/* only once per frame! */

			if (bQuads && !nExtraPlanes)
			{
				nNumFaces = ISPSpaceAvailable / (WORDS_PER_PLANE * 5);
			}
			else if(bQuads && nExtraPlanes)
			{
				nNumFaces = ISPSpaceAvailable / (WORDS_PER_PLANE * (5+nExtraPlanes));
			}
			else if(nExtraPlanes)
			{
				nNumFaces = ISPSpaceAvailable / (WORDS_PER_PLANE * (4+nExtraPlanes));
			}
			else
			{
				nNumFaces = ISPSpaceAvailable / (WORDS_PER_PLANE * 4);
			}
		}
		else
		{
			DPFDEV ((DBG_ERROR, "DirectPolygons: ISP space overflowing"));
			return;
		}
	}

	/* ini PDC */
	gpPDC->Context = *pContext; 
	gpPDC->nInputTriangles = nNumFaces;
	gpPDC->pVertices = pVertices; 
	gpPDC->pFace = pFaces;

	/*
	// init TSP control word to 0 or not fogged
	*/

	if (!gpPDC->Context.bFogOn)
	{
		gpPDC->TSPControlWord = MASK_DISABLE_FOG;
	}
	else
	{
		gpPDC->TSPControlWord = 0;
		FogUsed = 1;
	}

	/* Need to set the Boguz Inv Z value if it is provided by the user.
	 * This is to be used for the ISP in the depth calculation.
	 */
	if (gpPDC->Context.u32Flags & SGLTT_BOGUSINVZPROVIDED)
	{
		/* Read the bogus inverse Z provided.
		 * Scale the value correctly.
		 */
		gfBogusInvZ = (gpPDC->Context.fBogusInvZ * fMinInvZ);
	}

	/*
	// work out which function block we are using
	*/
	uFuncBlockIndex = gpPDC->Context.u32Flags & (SGLTT_GOURAUD | SGLTT_TEXTURE | SGLTT_HIGHLIGHT);

	if (gpPDC->Context.u32Flags & SGLTT_AVERAGECOLOUR)
	{
		if (gpPDC->Context.eShadowLightVolMode == NO_SHADOWS_OR_LIGHTVOLS)
		{
			if(gpPDC->Context.u32Flags & SGLTT_VERTEXFOG)
			{
				sgl_uint32 uFogColour;

				uFogColour = 
					((sgl_uint32)(gpPDC->Context.fFogR*255)<<16) |
					((sgl_uint32)(gpPDC->Context.fFogG*255)<<8) |
					((sgl_uint32)(gpPDC->Context.fFogB*255));

				gpPDC->VFogTSPControlWord[0] = ((uFogColour >> 16) & 0x000000FF);
				gpPDC->VFogTSPControlWord[0] |= (MASK_TEXTURE | MASK_TRANS | MASK_DISABLE_FOG);
				gpPDC->VFogTSPControlWord[1] = (uFogColour << 16);
				pFuncBlock = NoVolAverageVFogFuncs;
			}
			else
#if 0
			if (gpPDC->Context.u32Flags & SGLTT_NATIVESHADING)
			{
				pFuncBlock = NoVolAverageNativeFuncs;
			}
			else
#endif
			{
				pFuncBlock = NoVolAverageFuncs;
			}
		}
		else
		{
			if (gpPDC->Context.eShadowLightVolMode == ENABLE_SHADOWS)
			{
				pFuncBlock = ShadowAverageFuncs;
			}
			else
			{
				pFuncBlock = LightVolAverageFuncs;
			}
		}
	}
	else
	{
		if (gpPDC->Context.eShadowLightVolMode == NO_SHADOWS_OR_LIGHTVOLS)
		{
			if(gpPDC->Context.u32Flags & SGLTT_VERTEXFOG)
			{
				sgl_uint32 uFogColour;

				uFogColour = 
					((sgl_uint32)(gpPDC->Context.fFogR*255)<<16) |
					((sgl_uint32)(gpPDC->Context.fFogG*255)<<8) |
					((sgl_uint32)(gpPDC->Context.fFogB*255));

				gpPDC->VFogTSPControlWord[0] = ((uFogColour >> 16) & 0x000000FF);
				gpPDC->VFogTSPControlWord[0] |= (MASK_TEXTURE | MASK_TRANS | MASK_DISABLE_FOG);
				gpPDC->VFogTSPControlWord[1] = (uFogColour << 16);
				pFuncBlock = NoVolVFogFuncs;
			}
			else
#if 0
			if (gpPDC->Context.u32Flags & SGLTT_NATIVESHADING)
			{
				pFuncBlock = NoVolNativeFuncs;
			}
			else
#endif
			{
				pFuncBlock = NoVolFuncs;
			}
		}
		else
		{
			if (gpPDC->Context.eShadowLightVolMode == ENABLE_SHADOWS)
			{
				pFuncBlock = ShadowFuncs;
			}
			else
			{
				pFuncBlock = LightVolFuncs;
			}
		}

	} /* if average colour ... else */


	/* set up for global translucency if necessary */

	if ((gpPDC->Context.u32Flags & SGLTT_GLOBALTRANS) 
		&& (gpPDC->Context.u32GlobalTrans < 0xF0))
	{
		uFuncBlockIndex |= 0x08;		

		if (!(gpPDC->Context.u32Flags & SGLTT_VERTEXTRANS))
		{
			sgl_uint32 Alpha = 0xF - (gpPDC->Context.u32GlobalTrans >> 4);
			gpPDC->TSPControlWord |= Alpha << 13;
		}
	}
	
	/* 
	// set up for vertex trans 
	// To speed up the Vertex translucency averaging calc,
	// pre-compute 85 * the global trans value / 128.
	*/
	if (gpPDC->Context.u32Flags & SGLTT_VERTEXTRANS)
	{
		uFuncBlockIndex |= 0x10;
		gpPDC->u32GlobalTransBy85div128 = (85 * gpPDC->Context.u32GlobalTrans) >> 7;
	}
	
#if PCX2_003

	/* Set the (few) blending modes that we have */
	if (!(gPDC.Context.u32Flags & SGLTT_BLENDDISABLE))
	{
		/* Source */
		switch (gpPDC->Context.uBlendModes & 0xFF)
		{
			case sgl_blend_zero: /* Ignore */
				gpPDC->TSPControlWord &= ~MASK_BLEND_ALPHA;
				break; 
			case sgl_blend_one: /* Set alpha bit */
				gpPDC->TSPControlWord |= MASK_BLEND_ALPHA | MASK_TRANS;
				break;
			case sgl_blend_srcalpha: /* Clear alpha bit */
				gpPDC->TSPControlWord &= ~MASK_BLEND_ALPHA;
				break;
		}
		/* Destination */
		switch ((gpPDC->Context.uBlendModes >> 8) & 0xFF)
		{
			case sgl_blend_zero: /* Ignore */
				gpPDC->TSPControlWord &= ~MASK_BLEND_ONE_OVER_ALPHA;
				break; 
			case sgl_blend_one: /* Set 1/alpha bit */
				gpPDC->TSPControlWord |= MASK_BLEND_ONE_OVER_ALPHA | MASK_TRANS | MASK_GLOBAL_TRANS;
				break;
			case sgl_blend_invsrcalpha: /* Clear 1/alpha bit */
				gpPDC->TSPControlWord &= ~MASK_BLEND_ONE_OVER_ALPHA;
				break;
		}
	}

#endif /* PCX2_003 */
	
	/* Y coordinate is in LINES NOT normal REGIONS */
	gpPDC->Context.FirstYRegion *= RegionInfo.YSize;
	
	if ( ( (gpPDC->Context.LastYRegion+1) <= RegionInfo.NumYRegions ) ||
		 ( !RegionInfo.HasLeftOver ) )
	{
		/* Calculate accurate end of the Y Region */
		gpPDC->Context.LastYRegion = ( ( gpPDC->Context.LastYRegion + 1 ) *
									 RegionInfo.YSize ) - 1;
	}
	else
	{
		/* Last region was not full size, go to last line on screen */
		gpPDC->Context.LastYRegion = ( ( RegionInfo.NumYRegions - 1 ) *
									 RegionInfo.YSize ) + RegionInfo.LeftOverY;
	}
	
	/* XRegion scaling can be achieved with a shift */
	gpPDC->ShiftRegX = 5;		/* Start at 32 */
	
	if ( RegionInfo.XSize > (1<<gpPDC->ShiftRegX) )
	{
		do
		{
			/* Larger shifts, for 64 perhaps */
			gpPDC->ShiftRegX++;
		}
		while ( RegionInfo.XSize > (1<<gpPDC->ShiftRegX) );
	}
	else
	{
		while ( RegionInfo.XSize < (1<<gpPDC->ShiftRegX) )
		{
			/* Smaller Shifts ??? lets be future proof */
			gpPDC->ShiftRegX--;
		}
	}
	
	ASSERT( ( RegionInfo.XSize == (1<<gpPDC->ShiftRegX) ) );
	
	/* validate the texture if it's there */
	ProcessFlatTexFn = NULL;	

	if (gpPDC->Context.u32Flags & SGLTT_TEXTURE)
	{
		sgl_bool	fTexIsOK = TRUE;
		HTEXTURE hTexture;
		int			nIndex = 0;
#if PCX2_003
		TPRIVATEDATA *pTPD;
		sgl_bool	HasTranslucent = FALSE;
#endif

		if (hTexture = GetTextureHandle (gpPDC->Context.nTextureName))
		{			
			TPS.TexAddress = hTexture->TSPTextureControlWord;
			gpPDC->invTexSize = TexasGetInvTextureDimension (&TPS);
			gpPDC->TexSize = 1.0f/gpPDC->invTexSize;
#if PCX2_003
			/*  SGL set translucent flag from the value of AlphaMask  */
			pTPD = hTexture->pPrivateData;
			
			if(!(gpPDC->Context.u32Flags & SGLTT_BLENDDISABLE))
			{
				if ((pTPD->pTextureSpec->pFormat->AlphaMask == 0x0000F000) ||
					(pTPD->pTextureSpec->pFormat->AlphaMask == 0x00008000))
				{
					HasTranslucent =  TRUE;
				}
			}
#endif
		}
		else
		{
			fTexIsOK = FALSE;
		}
		
		if (fTexIsOK)
		{
			gpPDC->TexAddress = TPS.TexAddress;
			TPS.LowWord = MASK_TEXTURE;
			
#if PCX2_003			
			/* Need to set flag to indicate if bilinear filtering enabled.
			 * Has to be done here since it requires the result of ->invTexSize.
			 */
	  		if (gpPDC->Context.u32Flags & SGLTT_BILINEAR)
			{
				gpPDC->fHalfTexel = gpPDC->invTexSize * 0.5f;						
				gpPDC->TSPControlWord |= MASK_BILINEAR;
			}
			else
			{
				gpPDC->fHalfTexel = 0.0f;
			}
#elif PCX2
			/* We have overriden the application settings so fix texel if
			 * bilinear.
			 */
			if (bBilinearEnabled)
			{
		  		gpPDC->fHalfTexel = gpPDC->invTexSize * 0.5f;
			}
			else
			{
				gpPDC->fHalfTexel = 0.0f;
			}
#else
			/* Any card other than PCX2 family is assumed to be non-Bilinear.*/
			gpPDC->fHalfTexel = 0.0f;
#endif

			if (gpPDC->Context.bFlipU)
			{
				gpPDC->TSPControlWord |= MASK_FLIP_U;
			}

			if (gpPDC->Context.bFlipV)
			{
				gpPDC->TSPControlWord |= MASK_FLIP_V;
			}
		
#if PCX2_003
	  	   	if (HasTranslucent)
#else		
			if (TPS.TexAddress & MASK_4444_555)
#endif
			{
				gpPDC->TSPControlWord |= MASK_TRANS;
				/* if it is a translucent texture we should make sure and
				** flatten off smooth shading if there are shadows or L-Vs
				** i.e. treat it as if global translucency is set 
				*/
				#define SMFLATTENMASK	(SGLTT_GOURAUD | SGLTT_GLOBALTRANS | SGLTT_VERTEXTRANS)
				#define SMFLATTENRESULT	(SGLTT_GOURAUD)
				
				if (((gpPDC->Context.u32Flags & SMFLATTENMASK) == SMFLATTENRESULT)
					&& (gpPDC->Context.eShadowLightVolMode != NO_SHADOWS_OR_LIGHTVOLS))
				{
					/* 
					   no need to worry about the actual global trans
					   value in TSPControlWord, it's 0 by default
					*/
					uFuncBlockIndex |= 0x08;
				}
			}

			if (gpPDC->Context.u32Flags & SGLTT_MIPMAPOFFSET)
			{
				gpPDC->n32MipmapOffset = gpPDC->Context.n32MipmapOffset;
			}

			/* One version does all. Bilinear and Non bilinear.
			 */
			if (gpPDC->Context.bDoUVTimesInvW)
			{
				/* Setup correct process texture handler. 8 choices:
				 *
				 *		0. D3D flat texture, perspective correct.
				 *		1. D3D flat texture, wrap U, perspective correct.
				 *		2. D3D flat texture, wrap V, perspective correct.
				 *		3. D3D flat texture, wrap U, wrap V, perspective correct.
				 *		4. D3D flat texture, non-perspective correct.
				 *		5. D3D flat texture, wrap U, non-perspective correct.
				 *		6. D3D flat texture, wrap V, non-perspective correct.
				 *		7. D3D flat texture, wrap U, wrap V, non-perspective correct.
				 */
				/* Determine which function to call.
				 */
				nIndex = 0;

				if (gpPDC->Context.u32Flags & SGLTT_WRAPU)
					nIndex += 0x1;

				if (gpPDC->Context.u32Flags & SGLTT_WRAPV)
					nIndex += 0x2;

				if (gpPDC->Context.u32Flags & SGLTT_NONPERSPTEX)
					nIndex += 0x4;

				ProcessFlatTexFn = ProcessD3DTSPFunctions[nIndex];
			}
			else
			{
				/* Setup correct process texture handler. 2 choices:
				 *
				 *		0. Flat texture, perspective correct.
				 *		1. D3D flat texture, non-perspective correct.
				 */
				/* Determine which function to call.
				 */
				nIndex = 0;

				if (gpPDC->Context.u32Flags & SGLTT_NONPERSPTEX)
					nIndex += 0x1;

				ProcessFlatTexFn = ProcessTSPFunctions[nIndex];
			}
		}
		else
		{
			/* not a valid texture handle */
			
			uFuncBlockIndex &= ~MASK_TEXTURE;
		}

		/* Setup correct pack texture handler. 4 choices:
		 *
		 *		0. Non mip-mapped, perspective correct.
		 *		1. Mip-mapped, perspective correct.
		 *		2. Non mip-mapped, non-perspective correct.
		 *		3. Mip mapped, non-perspective correct.
		 */
		/* Determine which function to call.
		 */
		nIndex = 0;

		if (gpPDC->TexAddress & MASK_MIP_MAPPED)
			nIndex += 0x1;

		if (gpPDC->Context.u32Flags & SGLTT_NONPERSPTEX)
			nIndex += 0x2;

		PackTSPTexFn = PackTSPFunctions[nIndex];
	}
	else
	{
		/* Check for the allignment of the TSP parameters.
		 * No problem with textured triangles.
		 * Potential wastage but only when different triangles passed in single
		 * calls.
		 */
		PVRParamBuffs[PVR_PARAM_TYPE_TSP].uBufferPos += 0x2;
		PVRParamBuffs[PVR_PARAM_TYPE_TSP].uBufferPos &= 0xFFFFFFFC;
	}

	#if DO_FPU_PRECISION

		SetupFPU ();
		
	#endif

	ASSERT ((uFuncBlockIndex & 0xFFFFFFE0) == 0);
	
	pFuncBlock += uFuncBlockIndex;

	gpPDC->TSPControlWord |= pFuncBlock->TSPControlWord;

	fnPerPoly = pFuncBlock->fnPerPoly;

	if ( ( fnPerPoly == ProcessFlatTex ) && ( ProcessFlatTexFn != NULL ) )
	{
		/* Process loops do this for free if ProcessFlatTexFn != NULL */
		fnPerPoly = NULL;
	}

#ifdef DLL_METRIC
	if (gpPDC->TSPControlWord & MASK_TRANS)
	{
		nTransPolygonsInFrame += nNumFaces;
	}
	else
	{
		nOpaquePolygonsInFrame += nNumFaces;
	}
#endif
	
	if (gpPDC->Context.u32Flags & SGLTT_FACESIND3DFORMAT)
	{
		if (bQuads)
		{
			DPFDEV ((DBG_ERROR, "sgltri_quads called with D3dFORMAT flag set"));
		}
		else if (gpPDC->Context.u32Flags & SGLTT_USED3DSTRIPFLAGS)
		{
#ifdef METRIC
			Times[INPUT_D3DPOLY_COUNT].Count += 1;
			Times[INPUT_D3DPOLY_COUNT].Total += nNumFaces;
#endif
			ProcessD3DPolys (TexHeap, fnPerPoly, pFuncBlock->fnPerBuffer, pFuncBlock->uSize);
		}		
		else
		{
#ifdef METRIC
			Times[INPUT_D3DTRI_COUNT].Count += 1;
			Times[INPUT_D3DTRI_COUNT].Total += nNumFaces;
#endif
			ProcessTris (TexHeap, fnPerPoly, pFuncBlock->fnPerBuffer, pFuncBlock->uSize);
		}
	}
	else
	{
		if (bQuads)
		{
#ifdef METRIC
			Times[INPUT_LITEQUAD_COUNT].Count += 1;
			Times[INPUT_LITEQUAD_COUNT].Total += nNumFaces;
#endif
			ProcessQuads (TexHeap, fnPerPoly, pFuncBlock->fnPerBuffer, pFuncBlock->uSize);
		}
		else
		{
#ifdef METRIC
			Times[INPUT_LITETRI_COUNT].Count += 1;
			Times[INPUT_LITETRI_COUNT].Total += nNumFaces;
#endif
			ProcessTris (TexHeap, fnPerPoly, pFuncBlock->fnPerBuffer, pFuncBlock->uSize);
		}
	}
	
	#if DO_FPU_PRECISION

		RestoreFPU ();

	#endif
}

/*------------------------------- End of File -------------------------------*/

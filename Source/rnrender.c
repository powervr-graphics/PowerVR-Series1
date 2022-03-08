/**************************************************************************
 * Name         : rnrender.c
 * Title        : SGL render
 * Author       : Simon Fenney
 * Created      : 26 July 1995
 *
 * Copyright : 1995-2022 Imagination Technologies (c)
 * License	 : MIT
 *
 * Description  : Renders the given scene, traversing the database etc.
 *
 * Platform     : ANSI
 *
 * Modifications:-
 * $Log: rnrender.c,v $
 * Revision 1.213  1997/09/11  10:26:23  erf
 * Allow control of dithering in SGL for quality setting.
 *
 * Revision 1.212  1997/09/10  17:17:48  erf
 * Added TRUE to HWStartRender() to enable dither in SGL.
 *
 * Revision 1.211  1997/07/24  17:07:25  gdc
 * removed windows.h include, changed call to HWStartRender so that
 * 2d callback is done there and hardware waited for there - and got
 * rid of duplicate WaitForHardware
 *
 * Revision 1.210  1997/07/14  19:33:04  mjg
 * Added PCX2_003 compile switch.
 *
 * Revision 1.209  1997/07/14  09:56:18  gdc
 * made some debug messages appear better
 *
 * Revision 1.208  1997/07/11  10:09:04  gdc
 * added a module id
 *
 * Revision 1.207  1997/06/25  15:56:23  gdc
 * typo
 *
 * Revision 1.206  1997/06/25  15:53:19  gdc
 * added a couple of debug messages
 *
 * Revision 1.205  1997/06/11  16:33:25  mjg
 * Moved PreRenderCallback and register initialisation to HWStartRender.
 *
 * Revision 1.204  1997/06/11  12:34:01  jop
 * Organised rendering
 *
 * Revision 1.203  1997/06/04  15:32:48  gdc
 * stopped compiler warnings
 *
 * Revision 1.202  1997/06/04  11:16:30  gdc
 * removed some compiler warnings
 *
 * Revision 1.201  1997/05/30  16:34:43  mjg
 * Passed the logicaldevice handle to PVROSCallback, rather than NULL and
 * added a couple of '#if !WIN32' lines.
 *
 * Revision 1.200  1997/05/22  23:26:57  jop
 * Needed to get rid of syntax error
 *
 * Revision 1.199  1997/05/21  14:01:30  gdc
 * put is a line to cause app to exit if we cant get buffer
 *
 * Revision 1.198  1997/05/20  13:59:44  gdc
 * removed thread priority code
 *
 * Revision 1.197  1997/05/15  20:14:23  jop
 * Removed synchronisation stuff
 *
 * Revision 1.196  1997/05/14  17:14:34  gdc
 * don't need set sabre ptr register in windows
 *
 * Revision 1.195  1997/05/13  14:48:37  gdc
 * put in call to schedule render rather than copying params and
 * hwstart render
 *
 * Revision 1.194  1997/05/13  13:40:01  gdc
 * typo
 *
 * Revision 1.193  1997/05/13  10:44:55  gdc
 * added logical device to get virtual buffer call
 * (which replaced get parameter space)
 *
 * Revision 1.192  1997/05/06  14:31:07  erf
 * Texture filtering is set to point sampled as default.
 *
 * Revision 1.191  1997/05/02  07:42:44  erf
 * Removed UINT32's etc and replaced with sgl_uint32's etc.
 *
 * Revision 1.190  1997/04/30  19:21:05  jop
 * Put Ed's changes back to how they were
 *
 * Revision 1.189  1997/04/29  14:49:36  erf
 * Added new PVROS API function call mechanism.
 *
 * Revision 1.188  1997/04/25  16:32:44  mjg
 * Replaced an instance of pSCBusBurstSpace.
 *
 * Revision 1.187  1997/04/24  21:00:56  gdc
 * sgl now initialsed form DllMain in windows builds
 *
 * Revision 1.186  1997/04/22  17:40:20  mjg
 * Tidied up includes
 *
 * Revision 1.185  1997/04/21  20:01:43  mjg
 * Implemented new parameter buffer structures.
 *
 * Revision 1.184  1997/04/21  19:08:13  gdc
 * added a parmbuff.h include for PVRParamBuffs
 *
 * Revision 1.183  1997/04/21  15:29:22  mjg
 * Replaced ParamBufferInfo with PVRParamBuffs.
 *
 * Revision 1.182  1997/04/15  13:11:23  mjg
 * Put in, but commented out, the call to PVROSUploadParameters.
 *
 * Revision 1.181  1997/04/10  19:54:31  mjg
 * Removed a little redundant code.
 *
 * Revision 1.180  1997/04/09  16:47:29  gdc
 * removed reference to projection_mat global
 *
 * Revision 1.179  1997/04/08  09:10:41  gdc
 * added windows.h for WIN32 builds for GetCurrentThread etc.
 *
 * Revision 1.178  1997/04/07  14:57:50  gdc
 * default camera for rendering is now got via a function call
 * rather than a global variable
 *
 * Revision 1.177  1997/04/07  13:21:58  gdc
 * removed current_trans_set_id global from this module
 *
 * Revision 1.176  1997/04/04  11:05:17  gdc
 * temporarily moved definition of current_trans_set_id here from sgllite
 * as re-work external variables
 *
 * Revision 1.175  1997/03/31  22:09:40  jop
 * Sorted out WIN32/DOS32 issues
 *
 * Revision 1.174  1997/03/29  22:03:47  mjg
 * Inserted PVROSDelay.
 *
 * Revision 1.173  1997/03/26  06:51:01  erf
 * Removed stuff which allows sglmid4.dll to use PCX2 functionality when
 * running on a PCX2.
 *
 * Revision 1.172  1997/03/25  12:23:56  erf
 * Added texture filtering selection for SGL. ie point sampled, bilinear etc.
 *
 * Revision 1.171  1997/03/25  10:16:07  jop
 * Updated to use generic callbacks instead of special direct draw nastythings
 *
 * Revision 1.170  1997/03/07  10:52:59  erf
 * Change strcat(...) to strcpy(...) in DumpSabreAndTexas().
 *
 * Revision 1.169  1997/03/04  18:18:46  erf
 * Made changes to DumpSabreAndTexas() routine.
 *
 * Revision 1.168  1997/02/12  12:35:19  erf
 * In sgl_render() create new translucent pass flushing object.
 *
 * Revision 1.167  1997/02/06  14:16:30  mjg
 * Altered the call to ResetRegionDataL.
 *
 * Revision 1.166  1997/02/04  12:22:56  gdc
 * moved sgltri_ functions to a new module called sgllite.c
 * moved some of the param buffer management functions into rnglobal.c
 * where they can be conviently shared by sgllite.c and rnrender.c
 * rnrender.c is now SGL ONLY
 *
 * Revision 1.165  1997/02/03  17:55:56  sjf
 * Added code to handle the switching of fog on and off correctly.
 *
 * Revision 1.164  1997/02/03  16:11:43  erf
 * Replaced masking plane colour to background colour.
 *
 * Revision 1.163  1997/02/03  12:49:24  erf
 * 1. Removed all D3DOPTIM_SGL compile switch references.
 * 2. Removed call to PackTexasFlatAtAddr() in sgltri_render(). Doesn't
 *    seem to be needed.
 *
 * Revision 1.162  1997/01/28  12:51:36  erf
 * PCX2 changes:
 * 1. Removed divide by 255 for fast fog colour.
 * 2. Removed LINK_LIST conditions to call special link list routines.
 *
 * Revision 1.161  1997/01/23  18:59:30  mjg
 * Freed buffers if there is a error during Direct Draw preparations. I've
 * included the call for all render functions, but the problem was only ever
 * apparent with 'Revolte' on a Midas 3 with ISPDoubleBuffer=0 in the ini file.
 *
 * Revision 1.160  1997/01/22  18:48:47  sjf
 * Changed Grahams #if 0  on the single buffering of TSP params
 * to #if ISPTSP.   It'll still go down in a screaming heap
 * but I feel better about it.
 *
 * Revision 1.159  1997/01/22  18:03:48  gdc
 * removed an extra parameter copy for single buffered TSP params
 *
 * Revision 1.158  1997/01/21  10:35:02  erf
 * Enable HWSetXClip for PCX2.
 *
 * Revision 1.157  1997/01/20  21:22:53  jrc
 * Added masking for PCX2 in the SGLMID4.DLL build.
 *
 * Revision 1.156  1997/01/20  21:08:26  erf
 * PCX2 hardware changes:
 * 1. Add new interface to PCX2 and Link list routines.
 *
 * Revision 1.155  1997/01/17  12:31:43  erf
 * Various changes for PCX1 & PCX2:
 * 1. Allocate 100% of ISP space for plane data. ie no TLB for
 *    ISP object pointers.
 * 2. Disabled masking support for PCX1.
 * 3. Fix PCX2 fast fogging. Divide colours by 255.0f (nasty).
 *
 * Revision 1.154  1997/01/10  19:40:29  mjg
 * *** empty log message ***
 *
 * Revision 1.153  1997/01/10  16:03:33  erf
 * Fixed PCX2 hardware issues:
 * 1. Fog colour.
 * 2. Masking support.
 *
 * Revision 1.152  1997/01/07  12:30:51  mjg
 * Dynamic Regions in SGL for Midas3 required the number of regions rendered.
 *
 * Revision 1.151  1996/12/20  16:05:02  erf
 * Added pixel masking (in PCX1 & PCX2) and fast fogging (only PCX2).
 * Align ISP object pointer index to 16 pointer boundary.
 *
 * Revision 1.150  1996/12/18  22:03:46  jrc
 * Added sgltri_rerender.
 *
 * Revision 1.149  1996/12/18  22:01:47  jop
 * Bloody rockem fix
 *
 * Revision 1.148  1996/12/18  18:19:24  erf
 * Align start address of ISP Object Pointers with 16 pointer boundary.
 * This prevents block of 16 object pointers crossing a page break. Needs
 * to be enabled for PCX2.
 * Removed any calls by SGL Lite to old pmsabre.c routines. These are
 * in the compile switch D3DOPTIM. This allows me to banish these from
 * pmsabre.c.
 *
 * Revision 1.147  1996/12/17  23:49:41  jop
 * Fixed light volumes
 *
 * Revision 1.146  1996/12/12  15:14:34  mjg
 * Changed strip rendering call for ISPTSP.
 *
 * Revision 1.145  1996/12/11  02:22:10  jop
 * Changed some defines from ISPTSP to WIN32
 *
 * Revision 1.144  1996/12/10  23:24:47  erf
 * Changed compile switches:
 * 1. PCX1 to PCX1 || PCX2.
 * 2. MIDASPC1 to ISPTSP.
 * 3. PVR1 to ISPTSP.
 * 4. PVR2 to PCX1.
 * 5. PVR3 to PCX2.
 *
 * Revision 1.143  1996/12/10  19:04:46  gdc
 * moved defintion of current_trans_set_id to this module from pmsabre.c
 *
 * Revision 1.142  1996/12/10  15:29:23  jop
 * Added 2d compositing support
 *
 * Revision 1.141  1996/12/09  14:43:40  gdc
 * removed reference to sgltri.h, and relocated GetAppHints to this file
 *
 * Revision 1.140  1996/12/05  21:26:35  jrc
 * Added code for freezing the rendered image and dumping parameters after a
 * certain number of frames.
 *
 * Revision 1.139  1996/12/05  17:02:07  jrc
 * Renamed DUMP_PARAM_FILES to DUMP_PARAMS so as not to clash with simulator
 * version.
 *
 * Revision 1.138  1996/12/03  18:17:32  erf
 * Routine AddDummyPlanes() compiled out when D3DOPTIM used for
 * both SGL and SGL-Lite.
 *
 * Revision 1.137  1996/12/03  02:07:37  erf
 * Seperate SGL and SGL-Lite in terms of D3DOPTIM and D3DOPTIM_SGL.
 *
 * Revision 1.136  1996/12/02  13:04:35  erf
 * D3D optimisation enabled for SGL using D3DOPTIM_SGL compile switch.
 *
 * Revision 1.135  1996/11/29  11:44:20  erf
 * Using D3DOPTIM compile switch for optimisation do:
 * 1. In sgl_render() use ResetRegionDataL() to initialise new system.
 * 2. In sgl_render() use GenerateObjectPtrLite() to copy object pointers.
 *
 * Revision 1.134  1996/11/25  21:14:25  jrc
 * Added Rowland's new sabre and texas parameter dumping.
 *
 * Revision 1.133  1996/11/22  19:18:48  jop
 * Conditionally changed the place where AllowLightVolAddition
 * happens - will change when Rob updates dregion.c
 *
 * Revision 1.132  1996/11/15  09:32:43  rgi
 * Added D3DOPTIM compile case for SGLLite and D3DHAL. The
 * similar but different pmsabre calls are in pmsabrel.
 *
 * Revision 1.131  1996/11/08  18:15:57  erf
 * Added PVR3 compile switch for PCX2 to implement IEEE floating point
 * support. Changes made:
 * 1. Changed calls to PackBackgroundPLane().
 * 2. Changed n32FixedProjBackDist to f32FixedProjBackDist.
 *
 * Revision 1.130  1996/11/07  18:17:01  jrc
 * Added 1 to the X clipping so we err on it being too wide, not too narrow
 * (which often caused a missing pixel line down the right hand side).
 *
 * Revision 1.129  1996/11/07  16:03:26  jop
 * Returns region bounding box in reserved word in context
 *
 * Revision 1.128  1996/11/04  16:44:59  erf
 * Removed #define DUMP_PARAM_FILES 0 definition.
 *
 * Revision 1.127  1996/11/04  11:26:30  rgi
 * Added monitoring of GENERATE_TIME within TOTAL_RENDER_TIME.
 * Also monitored STARTOFFRAME_TIME in a way compatible with
 * the DLL_METRIC option. See dvdevice.c for more.
 *
 * Revision 1.126  1996/10/30  16:08:20  jrc
 * Added X clipping for strips and changed the clip value for the other case.
 *
 * Revision 1.125  1996/10/30  14:09:41  jop
 * Removed NETRACE 
 *
 * Revision 1.124  1996/10/29  18:04:03  jrc
 * Added device X clipping for D3D tunnel numbers bug fix.
 *
 * Revision 1.123  1996/10/24  16:07:02  jrc
 * An effort at working around smooth shading/translucency/shadows problem.
 *
 * Revision 1.122  1996/10/23  16:13:59  jop
 *  Added debug stuff (ifdefed out) for sm/shad/trans bug
 * y
 *
 * Revision 1.121  1996/10/18  13:04:28  jop
 * Removed a notprintf and tidied up renderstrip
 *
 * Revision 1.120  1996/10/18  12:23:53  jop
 * moved copyparamsifrequired() to after PackTexasFlatAtAddress()
 *
 * Revision 1.119  1996/10/16  21:16:24  jop
 * Took out PCX WaitForHardware()s before GenObj..Mem
 *
 * Revision 1.118  1996/10/11  17:10:01  jop
 * Fixed extents
 *
 * Revision 1.117  1996/10/09  18:30:51  jrc
 * Moved mesh region size setup from each display list traversal to within
 * sgl_create_screen_device call.  The previous scenario caused pauses and
 * crashes and slowdowns on some machines.
 *
 * Revision 1.116  1996/10/09  18:10:10  jop
 * Added strip functionality
 *
 * Revision 1.115  1996/10/04  16:24:10  sjf
 * Killed off PMSabreOnRender.
 *
 * Revision 1.114  1996/10/04  14:39:57  mal
 * Added some timing macros.
 *
 * Revision 1.113  1996/10/03  17:10:20  mal
 * Added some metric macros for profiling code.
 *
 * Revision 1.112  1996/10/03  14:21:38  jop
 * Added calls to PMSabreOnRender
 *
 * Revision 1.111  1996/10/02  16:50:30  jop
 * Fixed adapt bug
 *
 * Revision 1.110  1996/10/01  17:42:59  mjg
 * Added a call to help BestFitRegions work properly
 *
 * Revision 1.109  1996/09/30  11:40:28  msh
 * Moved where CopyParamsIfRequired is invoked in MIDAS
 * Arcade to immediately before the render is started (and
 * more importantly after some ISP register set-ups).  Avoids
 * system problems apparently to do with DMAing whilst modifying
 * ISP registers.
 *
 * Revision 1.108  1996/09/30  00:22:38  jop
 * Fixed dodgy selective region rendering
 *
 * Revision 1.107  1996/09/27  16:20:52  jop
 * Fixed bogus Z
 *
 * Revision 1.106  1996/09/27  14:28:05  jop
 * Added bogus Z for 'no-z-buffer' situations
 *
 * Revision 1.105  1996/09/19  11:09:21  jrc
 * Put  #if WIN32  around gbD3D usage.
 *
 * Revision 1.104  1996/09/18  17:23:05  jrc
 * Removed some carriage returns.
 *
 * Revision 1.103  1996/09/10  06:02:24  jop
 * Fixed a bug in the lit render routine, the region optimiser had
 * disappeared.
 *
 * Revision 1.102  1996/09/09  17:13:47  mjg
 * Fixed union stuff in sgl lite
 *
 * Revision 1.101  1996/09/08  02:34:43  jop
 * Added GetApplicationHints to sgltri_startofframe
 *
 * Revision 1.100  1996/09/07  23:06:13  jop
 * Added light volumes to triangle api
 *
 * Revision 1.99  1996/09/04  11:52:50  jop
 * added adaptive region stuff
 *
 * Revision 1.98  1996/09/03  18:21:24  jrc
 * Region non-rendering only now done for D3D with fog off.
 * Also, we don't need to deal with double and triple buffering.
 *
 * Revision 1.97  1996/09/03  15:21:22  jrc
 * For the first three frames and the three frames after a background colour
 * change all regions are rendered.
 *
 * Revision 1.96  1996/09/02  21:27:11  jrc
 * Fixed an ASSERT.
 *
 * Revision 1.95  1996/09/02  17:46:04  jrc
 * Added handling for new version of region skipping for SGL Lite/Direct3D.
 *
 * Revision 1.94  1996/09/02  12:39:45  jop
 * Sets current_trans_set_id properly for triangle api
 *
 * Revision 1.93  1996/08/30  15:49:29  sjf
 *  Added in small object rejection for PVR1 - dummy param added to
 * add opaque calls.
 *
 * Revision 1.92  1996/08/28  15:11:20  msh
 * New CopyParamsIfRequired function added for MIDAS Arcade
 * and conditionally compiled stuff for Arcade in original
 * function removed.
 *
 * Revision 1.91  1996/08/02  18:00:54  jop
 * Now bracketing traverse with FPU fiddle
 *
 * Revision 1.90  1996/07/22  15:51:28  msh
 * MIDAS Arcade timing stuff added.
 *
 * Revision 1.89  1996/07/19  14:49:57  sjf
 *  GenerateObjectPtrMem now cleans up after itself so we don't
 * have to call ResetRegionData in sgl_render.
 *
 * Revision 1.88  1996/07/11  16:40:42  sjf
 * Needed to swap order of sgl.h and sgl_defs.h includes.
 *
 * Revision 1.87  1996/07/11  15:57:56  sjf
 * Now gets the number of regions from the viewport itself in sgl_render. This is
 * done to support region masking.
 *
 * Revision 1.86  1996/07/11  11:01:38  sjf
 * Added region masking.
 *
 * Revision 1.85  1996/07/10  13:43:44  jop
 * Added sgltri_isrendercomplete
 *
 * Revision 1.84  1996/07/10  11:17:46  msh
 * Fast copy added for MIDAS Arcade
 *
 * Revision 1.83  1996/07/09  16:31:38  pdl
 * removed a c++ comment.
 *
 * Revision 1.82  1996/07/08  04:52:54  sjf
 * Ha Ha Ha: Put the the background plane back at infinity. It seems this
 * is not really liked by Mech Warrior, and I agree. We need to change
 * the documentation.
 *
 * Revision 1.81  1996/06/28  09:49:59  mal
 * Modified the SetThreadPriority Fns to use the variable
 * HighPrioritySetting and DefaultPrioritySetting global variables.
 *
 * Revision 1.80  1996/06/27  16:57:53  ncj
 * status display now works with pcx1
 *
 * Revision 1.79  1996/06/26  17:27:23  ncj
 * Added display for time in and out of sgl_render
 *
 * Revision 1.78  1996/06/26  15:24:40  jrc
 * Added a SglError(sgl_no_err) to the normal return from sgltri_render.
 *
 * Revision 1.77  1996/06/18  06:02:44  jop
 * Now add background plane before traversing the display list
 *
 * Revision 1.75  1996/06/10  14:11:57  jop
 * added brdsetup.h
 *
 * Revision 1.74  1996/06/06  23:07:06  jop
 * Fixed packtexasflat to set up background plane shadows
 *
 * Revision 1.73  1996/06/05  15:51:52  jop
 * Changed triangle stuff:
 * 1 added PCX1 ifdef to prevent early overwrites of region buffer
 * 2 now adds background plane first
 * 3 fogging enabled
 *
 * Revision 1.72  1996/05/30  10:26:35  ncj
 * Added support for swap_buffers parameter with Direct Draw apps.
 *
 * Revision 1.71  1996/05/16  12:01:22  sjf
 * 1) Added #def for PCX1 on wating to see if the hardware had finished, before
 *   generating the sabre pointers
 * 2) Cleaned up some commented out rubbish.
 *
 * Revision 1.70  1996/05/13  10:41:24  pdl
 * added some WIN32 ifdef's
 *
 * Revision 1.69  1996/05/09  17:34:30  jop
 * Fixed some explicit paths in headers
 *
 * Revision 1.68  1996/05/08  17:36:10  jrc
 * Moved end of file marker to the end of the file, and a few other tidyups.
 *
 * Revision 1.67  1996/05/07  13:02:11  jop
 * Fixed midaspc1 bug in sgltr render
 *
 * Revision 1.66  1996/05/03  18:00:09  jop
 * Added triangle code
 *
 * Revision 1.65  1996/05/03  15:58:21  jop
 * Optimised some stuff and sorted out pcx1 issues
 *
 * Revision 1.64  1996/04/03  18:11:17  jop
 * Removed old REPORT_FRAME_RATE stuff
 *
 * Revision 1.63  1996/04/02  17:46:28  sjf
 * disabled the frame rate reporting when profiling on Unix.
 *
 * Revision 1.62  1996/04/02  12:35:34  sjf
 * Added a def so it can compile on the sun.
 *
 * Revision 1.61  1996/03/28  11:54:45  sm
 * trans vienetting fix
 *
 * Revision 1.60  1996/03/25  19:37:20  sjf
 * removed dud routine.
 *
 * Revision 1.59  1996/03/25  16:40:13  jop
 * Changed stats doodahs
 *
 * Revision 1.58  1996/03/11  17:17:14  sjf
 * Modified the names of the files the parameters are dumped into,
 * and added #def'ble output of hardware registers.
 *
 * Revision 1.57  1996/02/23  15:20:22  ncj
 * Fixed un-initialised variable, caused non ddraw systems to slow down
 *
 * Revision 1.56  1996/02/19  13:35:49  ncj
 * Added direct draw clean-up for single shot mode
 *
 * Revision 1.55  1996/02/12  10:43:41  ncj
 * Added direct draw calls if DDRAW is defined
 *
 * Revision 1.54  1996/01/31  14:26:26  jop
 * Added texas/sabre double buffering
 *
 * Revision 1.53  1996/01/23  16:31:51  sm
 * pvr1 2 also needs a flush plane for shadows
 *
 * Revision 1.52  1996/01/19  14:23:04  jop
 * Added hardware performance measurement support
 *
 * Revision 1.51  1996/01/18  11:06:14  jop
 * Removed raster mode VTRIG twiddle
 *
 * Revision 1.50  1996/01/15  21:27:19  jop
 * Removed double buffering stuff
 *
 * Revision 1.49  1996/01/15  12:36:55  jop
 * Faster Texas buffer copy
 *
 * Revision 1.48  1996/01/10  11:40:14  jop
 * Bridge revision now checked to see if DRAM bug needs to be sorted
 *
 * Revision 1.47  1995/12/18  17:57:50  sjf
 * Just restructured the Hardware debug output.
 *
 * Revision 1.46  1995/12/05  10:44:26  pdl
 * made the sabre/texas param binaries dump out on DUMP_PARAM_FILES==1
 *
 * Revision 1.45  1995/12/01  12:56:29  pdl
 * *** empty log message ***
 *
 * Revision 1.44  1995/12/01  11:23:54  pdl
 * corrected a dpf.
 *
 * Revision 1.43  1995/11/28  11:57:06  pdl
 * enabled more parameter file writing.
 *
 * Revision 1.42  1995/11/27  18:52:28  jop
 * Bodged up for video over PCI
 *
 * Revision 1.41  1995/11/24  08:49:17  sjf
 * Added support for optionally enabling checking of Sabre SDRAM parameters in MidasPC1.
 *
 * Revision 1.40  1995/11/15  14:34:21  jop
 * Added mesh performance analyser
 *
 * Revision 1.39  1995/11/05  15:24:29  sjf
 * Fixed bug - first frame now uses SW buffer and I didn't check for this (
 * actually it wasn't a bug but it upset an ASSERT).
 *
 * Revision 1.38  1995/11/04  12:16:55  sjf
 * Added the ability to printout the framerate.
 *
 * Revision 1.37  1995/11/03  17:45:53  sjf
 * Added a new copy method for Midaspc1 that uses Jim's VXD copy...
 * the error checking was taking an extraordinary amount of time,
 * and copying within the VxD seems much safer.
 *
 * Revision 1.36  1995/10/31  17:59:58  jop
 * Added an ifdef to allow you to see the actual frame you just rendered
 *
 * Revision 1.35  1995/10/31  14:13:07  pdl
 * added an #if MIDASPC1
 *
 * Revision 1.34  1995/10/29  14:19:10  jop
 * sgl_render now conducts time-consuming tasks at big thread priority
 *
 * Revision 1.33  1995/10/28  14:48:07  sjf
 * Removed some uneeded debug code
 * 2) Put some extra checking in -  we were getting a complete lock up of the
 *    SCBurst memory. There appears to be a bug in the Accessing of Texture RAM (i.e.
 *   reading.
 *
 * Revision 1.32  1995/10/27  10:03:27  sjf
 * Added some optional debug code - not enabled at the mo.
 *
 * Revision 1.31  1995/10/24  19:36:43  sjf
 * Added code to reliably copy params to the buggy Sabre SDRAM by checking it after
 * the copy.
 *
 * Revision 1.30  1995/10/24  11:47:47  sjf
 * Added stuff to test sabre SDRAM ... it doesn't seem to be working very
 *  well at all.
 *
 * Revision 1.29  1995/10/22  18:40:07  sjf
 * Added stuff while trying to get it running on PC.....
 *
 * NEED TO REMOVE THE CLEARING OF SABRE PARAM STORE !!!!!!!
 *
 * Revision 1.28  1995/10/19  14:22:41  sjf
 * Now supports independant single/double buffering of sabre and texas
 * parameters.				  
 *
 * Revision 1.27  1995/10/17  12:45:14  jrc
 * Now passing the rectangles of regions as a single struct instead of four
 * ints to AddRegionOpaque.
 *
 * Revision 1.26  1995/10/11  14:20:15  sjf
 * Put parts of sgl_render into their own functions for readibilty
 * 2) Added parameter double buffering.
 *
 * Revision 1.25  1995/10/06  08:36:22  sjf
 * Added dump of parameter memory to file when in debug mode.
 *
 * Revision 1.24  1995/10/05  14:50:13  sjf
 * Mostly cosmetic, but if profiling define "MARK" is set, then it won't actually
 * do a render. Also put in variable to count the number of planes.
 *
 * Revision 1.23  1995/10/04  10:38:33  jop
 * Added hook for speed tests and wireframe rendering
 *
 * Revision 1.22  1995/09/19  09:12:07  sjf
 * Added pack of dummy obj needed for light vols. Really all these dummy
 * objects should be put into the intialisation so that they aren't re-used
 * (err I mean) re-built each render.
 *
 * Revision 1.21  1995/09/18  15:09:48  jrc
 * Removed duff ASSERT.
 *
 * Revision 1.20  1995/09/18  14:54:01  jrc
 * 1. Forgot a semicolon!
 * 2. Removed unused variables i and j in sgl_render.
 *
 * Revision 1.19  1995/09/18  14:50:32  jrc
 * sgl_render now accepts SGL_DEFAULT_LIST (but not SGL_NULL_LIST yet).
 *
 * Revision 1.18  1995/09/08  15:59:09  sm
 * added setting of dummy object
 *
 * Revision 1.17  1995/09/05  14:33:48  sm
 * changed bit that adds dummy planes
 *
 * Revision 1.16  1995/09/04  15:25:28  sm
 * changed for multiple translucency
 * ,
 *
 * Revision 1.15  1995/08/21  15:57:35  sm
 * added flushing plane to the tran pass
 *
 * Revision 1.14  1995/08/21  09:52:44  sjf
 * Background plane set to have no fog.
 *
 * Revision 1.13  1995/08/18  17:05:41  sjf
 * Added correct fogging.
 *
 * Revision 1.12  1995/08/11  10:47:41  sm
 * botched in fog to confirm that it was working
 *
 * Revision 1.11  1995/08/07  12:13:08  jop
 * Added mesh per-render initialisation
 *
 * Revision 1.10  1995/08/01  11:48:41  pdl
 * added TexasSetDim
 *
 * Revision 1.9  1995/07/30  22:45:08  sjf
 * Put code back to using the real background colour.
 *
 * Revision 1.8  1995/07/30  17:48:48  sjf
 * Added tidying up of any display list items.
 *
 * Revision 1.7  1995/07/29  13:36:23  sjf
 * Added debug message.
 *
 * Revision 1.6  1995/07/27  16:07:14  sjf
 * Added a dummy plane to apparently flush out output in the simulator.
 *
 * Revision 1.5  1995/07/27  12:58:41  sjf
 * Oh how embarrassing... fixed a compile error.
 *
 * Revision 1.4  1995/07/27  12:51:37  sjf
 * pCamera is set to be default camera when passed into traverse
 * routine.
 *
 * Revision 1.3  1995/07/27  12:08:18  sjf
 * Added another .h.
 *
 * Revision 1.2  1995/07/27  11:55:31  sjf
 * Added background plane.
 *
 * Revision 1.1  1995/07/27  10:51:58  sjf
 * Initial revision
 *
 *
 **************************************************************************/

#define MODULE_ID MODID_RNRENDER

#include "sgl_defs.h"
#include "sgl.h"
#include "dlnodes.h"
#include "rnglobal.h"
#include "parmbuff.h"
#include "dvdevice.h"
#include "sgl_init.h"
#include "nm_intf.h"
#include "dlglobal.h"
#include "rncamera.h"
#include "pmsabre.h"
#include "rntrav.h"

#include "rnconvst.h" /* ARRHGGGRRRRRRRRRR */
#include "rnstate.h"
#include "rnfshade.h"
#include "rnmesh.h"
#include "pktsp.h"
#include "pkisp.h"

#include "dregion.h"
#include "texas.h"
#include "hwregs.h"

#include "metrics.h"

#include "pvrosapi.h"
#include "parmbuff.h"

#if defined(MIDAS_ARCADE)
#include <time.h>
#endif

#if (WIN32 || DOS32)

	#define DO_FPU_PRECISION TRUE

	void SetupFPU (void);
	void RestoreFPU (void);

#else

	#define DO_FPU_PRECISION FALSE

#endif

SGL_GLOBAL_TIME_DEFN /* if we are timing code */

/*
// An alternative is that if the flag MARK is set, the actual
// render call will NOT be included .... SJF
*/
#define ACTUAL_RENDER_FLAG	TRUE
#if ACTUAL_RENDER_FLAG
int fDoActualRender = TRUE;
#endif

#if WIN32 || DOS32 || MAC

	#include "brdcfg.h"

#endif

#define WAIT_FOR_FRAME	0

static sgl_uint32 TSPBackgroundAddress = 0;

#if defined(MIDAS_ARCADE)
extern sgl_uint32				SWRenderStartTime;
#endif

#if PCX2 || PCX2_003
/* PCX2 Fast fog colour value. Set by SGL or SGL-Lite.
 */
sgl_colour	cFastFogColour = {0.0f, 0.0f, 0.0f};
#endif

#if DUMP_PARAMS

#include <process.h>
sgl_uint32 *curAddr;


/**************************************************************************
 * Function Name  : DumpSabreAndTexas (gloabl)
 * Inputs         : 
 * Input/Outputs  : 
 * Returns        : 
 * Global Used    : 
 * Description    : Dumps the contents of the parameter memory to a file.
 *					Required by sgl_render() and sgltri_render() routines.
 *	
 **************************************************************************/

#define TIMED_CAPTURE			FALSE
#define CAPTURE_RATE			40
#define TEXPARAMS_ONLY			FALSE
#define OUTPUT_LITTLE_END		0
#define COMPRESSOR				FALSE

#if TIMED_CAPTURE
	/* Write parameters to the desktop.
	 */
	#define DUMP_PATH "c:\\Windows\\Desktop\\"
#else
	/* Set up a mapped drive. This is used to initiate the parameter dump
	 * and also the parameters are written here.
	 */
	#define DUMP_PATH "e:\\"
#endif

void DumpSabreAndTexas(long SabreRegionInfoStart,
			PARAM_BUF_MANAGEMENT_STRUCT *pParamBlock, long camera)
{
	FILE *fp;
	FILE *fp2;
	sgl_uint32 Val;
	sgl_uint32 start, i, PlanesLimit, TexasLimit, RegionLimit;
   	char fileEnding[100]="\0", paramFile[100]="\0", InputFile[100]="\0", info[100]="\0";
	char command[30];
	char path[] = DUMP_PATH;
	static int frame;
	static int filenumber=0;
	int captureFrame;
	static sgl_bool  bDoneCapture = FALSE;

	frame++;

	captureFrame=FALSE;

#if TIMED_CAPTURE
	if (frame==CAPTURE_RATE)
		captureFrame=TRUE;
#else
	strcpy(InputFile, DUMP_PATH);
	strcat(InputFile, "capstat.txt");

	if( (fp = fopen(InputFile, "r"))==NULL);
	else if(fgetc(fp)!=0)
	{
		captureFrame=TRUE;
		fclose(fp);
		fp = fopen(InputFile, "w");
		fputc(0, fp);
		fclose(fp);
	}
	else
	{
		fclose(fp);
	}
#endif

	if(captureFrame)
	{
#if COMPRESSOR
		/* wait until the compressor is ready */
		fp = fopen("e:\\compstat.txt", "r");
		while(fgetc(fp)==1)
	  	{
			fclose(fp);
			PVROSDelay(PVR_DELAY_MS, 100);
	  		fp = fopen("e:\\compstat.txt", "r");
	 	}
		fclose(fp);
#endif

		frame=0;
		filenumber++;

		itoa(filenumber, fileEnding, 10);
    	
    	/* set limits */
		TexasLimit = 1024 * 1024;	 	 	
		PlanesLimit = SabreRegionInfoStart;
		RegionLimit = ((sgl_uint32) curAddr ) - ( (sgl_uint32) pISPObjectData);

		/* open info file */	 
		strcat(info, path);
		strcat(info, "info");
		strcat(info, fileEnding);
		strcat(info, ".txt");
		fp2 = fopen(info, "w");

		/* open sabre file */
		strcat(paramFile, path);
		strcat(paramFile, "sab");
		strcat(paramFile, fileEnding);
		//strcat(paramFile, ".Z");
		fp = fopen(paramFile, "wb");
	 
		if( (fp!=NULL) && (fp2!=NULL) )
		{	
			/* dump plane data */

			for(i = 0; i <= PlanesLimit; i++)
			{
				Val = pParamBlock->isp.pParamStore[i];
		
				#if OUTPUT_LITTLE_END
					fputc(Val & 0xff, fp);
					fputc(Val >> 8  & 0xff, fp);
					fputc(Val >> 16 & 0xff, fp);
					fputc(Val >> 24 & 0xff, fp);
					#else
					fputc(Val >> 24 & 0xff, fp);
					fputc(Val >> 16 & 0xff, fp);
					fputc(Val >> 8  & 0xff, fp);
					fputc(Val & 0xff, fp);
				#endif		
			}	  

			/* output address of plane/tile division pointer in 32 bit words */
			fprintf(fp2, "%d %d", i, camera);
	
			/* dump region data (NOTE region data is NOT in pParamBlock for PCX1) */ 
		 
			for(i = 0; i <= RegionLimit; i++)
			{
				Val = pISPObjectData[i];
		
				#if OUTPUT_LITTLE_END
					fputc(Val & 0xff, fp);
					fputc(Val >> 8  & 0xff, fp);
					fputc(Val >> 16 & 0xff, fp);
					fputc(Val >> 24 & 0xff, fp);
				#else
					fputc(Val >> 24 & 0xff, fp);
					fputc(Val >> 16 & 0xff, fp);
					fputc(Val >> 8  & 0xff, fp);
					fputc(Val & 0xff, fp);
				#endif		
			} 

			fclose(fp);
			fclose(fp2);
		}
		else
			fprintf(stderr, "Couldn't write sabre file");
			  
		/* open Texas files */ 
		strcpy(paramFile, path);
		strcat(paramFile, "tex");
		strcat(paramFile, fileEnding);
		//strcat(paramFile, ".Z");
		fp = fopen(paramFile, "wb");
	
		if(fp!=NULL)
		{
		
		/* we may only want the texels on the first frame */
		#if TEXPARAMS_ONLY==TRUE
		if(filenumber!=1)
			TexasLimit= pParamBlock->tsp.IndexLimit - pParamBlock->tsp.StartIndex;	
		#endif
			
			for(i = 0; i<TexasLimit; i++)
			{
		
				Val = ghTexHeap->pTextureMemory[i]; 
  	
			#if OUTPUT_LITTLE_END
				fputc(Val & 0xff, fp);
				fputc(Val >> 8  & 0xff, fp);
				fputc(Val >> 16 & 0xff, fp);
				fputc(Val >> 24 & 0xff, fp);
			#else
				fputc(Val >> 24 & 0xff, fp);
				fputc(Val >> 16 & 0xff, fp);
				fputc(Val >> 8  & 0xff, fp);
				fputc(Val & 0xff, fp);
			#endif
			}

			fclose(fp);
		}
		else
		{
			fprintf(stderr, "Couldn't write texas file");
		}
		fclose(fp);	 

#if COMPRESSOR
		/* indicate to compressor that ready */
		fp = fopen("e:\\compstat.txt", "w");
		fputc(1, fp);
		fclose(fp);

#endif
	  	bDoneCapture = TRUE;
	} /* if (captureFrame) */

} /* DumpSabreAndTexas */

#endif /* #if DUMP_PARAMS */

/**************************************************************************
 * Function Name  : sgl_render
 * Inputs         : 
 * Outputs        : 
 * Returns        : 
 * Global Used    : 
 * Description    : 
 *	
 *
 *
 * NOTE: Still gave to handle double buffering of the output!!!!
 *		 This will probably have to be done with interrupts etc..
 *		 and "shared" variables, which is all a bit of a nightmare.
 **************************************************************************/
/* used when getting parameter buffers */
extern HLDEVICE        gHLogicalDev;

extern void CALL_CONV sgl_render( const  int viewport_or_device, 
						 const  int camera_or_list, 
						 const	sgl_bool swap_buffers)
{
	HDISPLAY hDisplay;
	VIEWPORT_NODE_STRUCT * pViewportOrDevice;
	void * pCameraOrList;
	CAMERA_NODE_STRUCT * pCamera;
	LIST_NODE_STRUCT   * pList;
	PVROSERR err;
	
	int ItemType;
	sgl_int32 BackGroundStart;
	int x_dimension,y_dimension;
	
	static int state = 0;
	
	#if !WIN32
	sgl_uint32 SabreRegionInfoStart;
	#endif 

	PROJECTION_MATRIX_STRUCT  * const pProjMat = RnGlobalGetProjMat ();

	#if ISPTSP
	int nNumRegionsRendered;
	#endif
	
	SGL_TIME_START(TOTAL_RENDER_TIME);
	
	DPF((DBG_MESSAGE,"Entering SGL_RENDER"));

	#if defined(MIDAS_ARCADE)
	SWRenderStartTime = clock();
	#endif
	/*
	// Initialse the system if not already done. 
	// Actually that would be a pretty bad thing to do, since
	// its a bit pointless trying to render rubbish!!!
	*/

#if !WIN32
    if (SglInitialise())
	{
		SglError(sgl_err_failed_init);
 		SGL_TIME_STOP(TOTAL_RENDER_TIME);
		return;
	}
#endif
	/*
	// Tidy up any unfinished business
	*/
	DlCompleteCurrentTransform();
	DlCompleteCurrentConvex();
	DlCompleteCurrentMaterial();
	DlCompleteCurrentMesh();
	DlCompleteCurrentConvex();

	/* Initalise texture filter setting to point sampled as default.
	 */
	pProjMat->eFilterType = sgl_tf_point_sample;

	/*//////////////////////
	// Get the viewport or device to use, and the type
	////////////////////// */
	pViewportOrDevice =(VIEWPORT_NODE_STRUCT *)GetNamedItem(dlUserGlobals.pNamtab, 
									viewport_or_device);

	ItemType =  GetNamedItemType(dlUserGlobals.pNamtab, 
									viewport_or_device);
	/*
	// Check that this Ok
	*/
	if((pViewportOrDevice == NULL) || 
	   ((ItemType != nt_device) && 
	    (ItemType != nt_viewport)) )
	{
		DPF((DBG_WARNING, "SGL_RENDER Not a valid Viewport or device"));
		SglError(sgl_err_bad_name);
		SGL_TIME_STOP(TOTAL_RENDER_TIME);
		return;
	}
	else if(ItemType == nt_device)
	{
		/*
		// Get the  devices equivalent viewport
		*/
		pViewportOrDevice = 
				&((DEVICE_NODE_STRUCT *)pViewportOrDevice)->defaultViewport;
	}
	else
	{
		ASSERT(ItemType == nt_viewport);
	} /*end if else*/

	hDisplay = (HDISPLAY) ((VIEWPORT_NODE_STRUCT *) pViewportOrDevice)->pParentDevice->PhDeviceID;

	/*
	// check if viewport is empty?
	*/
	if( (pViewportOrDevice->Left > pViewportOrDevice->Right) ||
		(pViewportOrDevice->Top	 > pViewportOrDevice->Bottom))
	{
		DPF((DBG_WARNING, "Handling of empty viewports not complete!!"));
	}

	/*//////////////////////
	// Get the Camera Or List to use, and the type
	// AND set up the global projection matrix values
	////////////////////// */
	if (camera_or_list == SGL_DEFAULT_LIST)
	{
		ASSERT(dlUserGlobals.pDefaultList != NULL);
		pCameraOrList = dlUserGlobals.pDefaultList;
		ItemType = nt_list_node;
	}
	else
	{
		pCameraOrList = GetNamedItem(dlUserGlobals.pNamtab, 
									camera_or_list);

		ItemType = GetNamedItemType(dlUserGlobals.pNamtab, 
									camera_or_list);
		/*
		// Check that this Ok
		*/
		if((pCameraOrList == NULL) || 
		   ((ItemType != nt_camera) && 
		    (ItemType != nt_list_node)) )
		{
			DPF((DBG_WARNING, "SGL_RENDER Not a valid list or camera"));
			SglError(sgl_err_bad_name);
		 	SGL_TIME_STOP(TOTAL_RENDER_TIME);
			return;
		}
	}

	if(ItemType == nt_list_node)
	{
		/*
		// Set up the projection matrix using the default camera
		*/
		pCamera = GetDefaultCamera();
		RnSetupProjectionMatrix( pCamera,
								 pViewportOrDevice);
		pList   = pCameraOrList;
	}
	/*
	// Else use this camera
	*/
	else
	{
		ASSERT(ItemType == nt_camera);
		RnSetupProjectionMatrix(pCameraOrList,
								  pViewportOrDevice);

		pCamera = pCameraOrList;
		pList   = NULL;

	} /*end if else*/

	/*
	// For optimisation. Reset the region lists structures to be empty
	*/
	ResetRegionDataL (FALSE);

#define CHECK_TEX_PARAM 0
#if CHECK_TEX_PARAM
	while(! HWFinishedRender());
#endif

	
	/*
	// Get parameter memory, if available...
	*/
	
#if WIN32
	err = PVROSAssignVirtualBuffers(PVRParamBuffs, gHLogicalDev);
	if(err!=PVROS_GROOVY)
	{
		PVROSPrintf("Unable to get buffer - skipping frame\n");
		return;
	}
#else
	GetParameterSpace(PVRParamBuffs);
#endif

	/* //////////////////////////////////////////////////
	/////////////////////////////////////////////////////
	// Add some "special" objects direct to the parameter
	// store. THESE SHOULD BE MOVED OUT AND SET UP ONCE ONLY
	// DURING INTIALISATION (obviously the initial pointers would have
	// to take account of these).
	/////////////////////////////////////////////////////
	////////////////////////////////////////////////// */
	AddDummyPlanesL (FALSE);

#if PCX2 || PCX2_003
	/* Fast fogging. Pack a plane for fogging. Only used by PCX2
	 * Set colour of plane to FOG COLOUR !!!!
	 */
	{
		sgl_uint32		nCurrentTSPAddr;

		/* Save current TSP index.
		 */
		nCurrentTSPAddr = PVRParamBuffs[PVR_PARAM_TYPE_TSP].uBufferPos;

		/* Tag ID of 2 (4/2) used for fogging.
		 */

		PVRParamBuffs[PVR_PARAM_TYPE_TSP].uBufferPos = 4;
		
		/* Pack a flat plane. Need to set colour to fog colour.
		 */
		PackTexasFlat (cFastFogColour, FALSE, FALSE);

		/* Restore the TSP index.
		 */
		PVRParamBuffs[PVR_PARAM_TYPE_TSP].uBufferPos = nCurrentTSPAddr;
	}
#endif


	/* //////////////////////////////////////////////////
	// Add the background plane - disable fogging on it
	// jimp: disable shadows as well
	////////////////////////////////////////////////// */
	BackGroundStart = PVRParamBuffs[PVR_PARAM_TYPE_ISP].uBufferPos;

#if PCX2 || PCX2_003
	PackBackgroundPlane( PackTexasFlat(pCamera->backgroundColour, FALSE, FALSE),
						0.0f);
						/* pProjMat->f32FixedProjBackDist); */ 
#else
	PackBackgroundPlane( PackTexasFlat(pCamera->backgroundColour, FALSE, FALSE),
						0);
						/* pProjMat->n32FixedProjBackDist); */ 
#endif

	/*
	// create a flushing plane ???????
	// Add the actual background opaque plane.
	// This is not a flushing plane !!!
	*/
	AddRegionOpaqueL(&pProjMat->RegionsRect, BackGroundStart, 1);
 	
	BackGroundStart = PVRParamBuffs[PVR_PARAM_TYPE_ISP].uBufferPos;
			   
#if PCX2 || PCX2_003
	PackBackgroundPlane( PackTexasFlat(pCamera->backgroundColour, FALSE, FALSE),
						-1.0f);
#else
	PackBackgroundPlane( PackTexasFlat(pCamera->backgroundColour, FALSE, FALSE),
						-64);
#endif

 	/* !!!! THIS IS ONLY NEEDED FOR THE MIDAS3 (old PVR1) SIMULATOR !!!! */
	/* Well, I'm not too sure about that (SJF)*/
	AddFlushingPlaneL(BackGroundStart);


	/* Add translucent flushing plane.
	 */
	BackGroundStart = PVRParamBuffs[PVR_PARAM_TYPE_ISP].uBufferPos;
						   
#if PCX2 || PCX2_003
	PackBackgroundPlane (PackTexasTransparent (FALSE), -1.0f);
#else
	PackBackgroundPlane (PackTexasTransparent (FALSE), -64);
#endif
	AddTransFlushingPlaneL (BackGroundStart);

	/* //////////////////////////////////////////////////
	/////////////////////////////////////////////////////
	// Traverse the display list
	/////////////////////////////////////////////////////
	////////////////////////////////////////////////// */
	DPF((DBG_MESSAGE, "Calling traverse"));
	
	#if DO_FPU_PRECISION

		SetupFPU ();

	#endif

	RnTraverseDisplayList(pList, pCamera);

	#if DO_FPU_PRECISION

		RestoreFPU ();

	#endif

	/* //////////////////////////////////////////////////
	// Convert the regions lists to ones understood by Sabre
	// Remember where the pointer data begins though.
 	////////////////////////////////////////////////// */

	#if !WIN32
	SabreRegionInfoStart = PVRParamBuffs[PVR_PARAM_TYPE_ISP].uBufferPos;
	#endif

	/*
	// Convert our internal region lists into the hardware ones, and
	// at the same time reset the internal region structures
	*/
	/* Call optimised routine.
	 */
	#if ISPTSP
	nNumRegionsRendered = 
	#endif
	GenerateObjectPtr(&pProjMat->RegionsRect, pViewportOrDevice->regionMask);


	/* //////////////////////////////////////////////////
	// Set up the virtual hardware registers
 	////////////////////////////////////////////////// */

#if !WIN32
	/* Sabre pointer in windows builds set on virtual buffer allocation */
	#if ISPTSP
		HWSetRegionsRegister( nNumRegionsRendered );
	#endif
	HWSetSabPtrRegister(SabreRegionInfoStart, 0);
#endif

/*
// Set the number of regions for Midas3 architecture only
*/

#if PCX2 || PCX2_003
	/* Set the texture filtering register.
	 * Need to wait for the hardware to become available.
	 */
	HWSetBilinearRegister(pProjMat->eFilterType);
#endif

	/*
	// Set the foggy would a wooing go
	*/
	HWSetFogRegister(pProjMat->FogShift);
	TexasSetFogColour(pCamera->FogCol);
	
	/*
	// Set the texture scale flag
	*/
	TexasSetCFRScale(pProjMat->n32CFRValue);
	DPF((DBG_MESSAGE, "CFR Scale is 0x%lX", (long)pProjMat->n32CFRValue));


	/*
	// device hardwired to 0 because i don't understand what is going on
	*/
	DPFOO((DBG_WARNING, "Device in RN render hardwired to 0"));
	HWGetDeviceSize(0,&x_dimension,&y_dimension);
   	TexasSetDim(x_dimension,y_dimension);


	#if ACTUAL_RENDER_FLAG
		/*
			if we are speed testing on the simulator exit here without
			doing an actual render
		*/
	
		if (!fDoActualRender)
		{
			SGL_TIME_STOP(TOTAL_RENDER_TIME); 
			return;
		}
	#endif

#ifndef MARK 
		DPF((DBG_MESSAGE, "Calling HWStartRender"));	

	#if !WIN32
		 /* If we had to use a software buffer for either sabre/texas (or both) then
		   copy them into the correct buffer space. */
		
		PVROSCopyParamsIfRequired(PVRParamBuffs);
	#endif

		/************* RENDER IS STARED HERE ****************/
		HWStartRender( swap_buffers, hDisplay, pProjMat->bDithering );

		DPF((DBG_MESSAGE, "Done HWtSartRender !!!!"));
	#else

		DPF((DBG_WARNING, "Pretending to Call Render......"));	

#endif MARK

	#if DUMP_PARAMS
		/*
		// For Sabre/Texas Debugging, output files of the parameter
		// store contents in little endian format
		*/
		if (PVRParamBuffs[PVR_PARAM_TYPE_ISP].uBufferPos == 0)
		{
			DumpSabreAndTexas(
			  SabreRegionInfoStart, PVRParamBuffs, pProjMat->n32CFRValue);
		}
	#endif

	/*
	// Code to dump out all the hardware regsiters
	*/
	#define DUMP_HW_REGS 0
	#if DUMP_HW_REGS && (WIN32 || DOS32) && DEBUG
		{
			FILE * outfile;

			outfile = fopen("regdump.txt", "w");
			
			HWDumpRegisters(outfile);

			fclose(outfile);
		}
	#endif

		PVROSCallback (gHLogicalDev, CB_POST_RENDER, NULL);


	SGL_TIME_STOP(TOTAL_RENDER_TIME);
	
	/*
	// THAT SHOULD BE ALL FOLKS.....
	*/

	DPF((DBG_MESSAGE,"Exiting SGL_RENDER"));

}/*end of function*/


/*------------------------------- End of File -------------------------------*/

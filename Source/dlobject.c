/*****************************************************************************
 *;++
 * Name           : $RCSfile: dlobject.c,v $
 * Title          : DLOBJECT.C
 * C Author       : Jim Page
 * Created        : 21/04/95
 *
 * Copyright	: 1995-2022 Imagination Technologies (c)
 * License		: MIT
 *
 * Description    :     SGL predefined 3d primitive objects
 *                   
 * Program Type   :     C (SGL cross-platform)
 *
 * RCS info:
 *
 * $Date: 1997/10/16 11:52:37 $
 * $Revision: 1.36 $
 * $Locker:  $
 * $Log: dlobject.c,v $
 * Revision 1.36  1997/10/16  11:52:37  sxy
 * deliberatly over wrote previous version, this is why there is a version number missing.
 *
 * Revision 1.34  1997/09/25  15:30:00  sxy
 * changed some DPFDEVs.
 *
 * Revision 1.33  1997/09/24  15:42:02  sxy
 * changed some DPF to DPFDEV.
 *
 * Revision 1.32  1997/06/06  14:38:47  mjg
 * Added some casts and a couple of 'f's to remove compiler warnings.
 *
 * Revision 1.31  1997/06/04  15:32:32  gdc
 * stopped some compiler warnings - lots more left
 *
 * Revision 1.30  1997/05/13  17:52:43  mjg
 * Replaced PVROS memory management calls with SGL.
 *
 * Revision 1.29  1997/04/30  19:18:47  jop
 * Put Ed's changes back to how they were
 *
 * Revision 1.28  1997/04/29  14:45:35  erf
 * Added new PVROS API function call mechanism.
 *
 * Revision 1.27  1997/04/21  22:45:09  gdc
 * stopped compile warning
 *
 * Revision 1.26  1997/04/02  21:27:29  mjg
 * Updated calling conventions.
 *
 * Revision 1.25  1997/03/27  12:36:08  erf
 * Operating system independent memory allocation/deallocation macros.
 * 1. Added #include "pvrosapi.h" to include new macros.
 * 2. Changed malloc, calloc, realloc and free to PVROSMalloc, PVROSCalloc,
 *    PVROSRealloc and PVROSFree.
 *
 * Revision 1.24  1996/07/11  16:39:32  sjf
 * Needed to swap order of sgl.h and sgl_defs.h includes.
 *
 * Revision 1.23  1996/02/20  10:00:49  jrc
 * Got rid of unused variable warning message
 *
 * Revision 1.22  1996/01/09  12:06:10  jrc
 * 1. Added 'static' to functions that are used only locally.
 * 2. Updated convex object calls to not use the old supply_normals and
 *    supply_uv flags.
 * 3. Changed three /2.0 to *0.5 (faster).
 * 4. Removed unused f1OverRadius variable.
 *
 * Revision 1.21  1995/11/02  16:46:09  pth
 * Got rid of a "printf" statement.
 *
 * Revision 1.20  1995/11/02  15:52:59  pth
 * Added texturing to sgl_create_rectprism
 *
 * Revision 1.19  1995/10/23  11:46:47  pth
 * Got rid of a printf statement
 *
 * Revision 1.18  1995/10/23  10:34:19  pth
 * Fixed 2nd extrude texture bug
 *
 * Revision 1.17  1995/10/23  10:11:31  pth
 * Fixed extrude texture bug.
 *
 * Revision 1.16  1995/10/17  14:44:36  jop
 * Removed final non-constant array init
 *
 * Revision 1.15  1995/10/17  14:34:50  pth
 * Removed explicit declarations that vtune doesn't like.
 *
 * Revision 1.14  1995/10/13  15:19:34  jrc
 * 1. Paul tidied up.
 * 2. John re-enabled the TRUE sphere type.
 *
 * Revision 1.13  1995/10/10  16:41:29  pth
 * Removed obsolete function sgl_add_patch
 *
 * Revision 1.12  1995/10/10  15:01:55  pth
 * *** empty log message ***
 *
 * Revision 1.11  1995/10/10  14:57:52  pth
 * rect_prism texture wrapping is added
 *
 * Revision 1.10  1995/10/10  14:22:19  pth
 * Fixed a few bugs and changed interface to sgl_extrude
 *
 * Revision 1.9  1995/10/09  17:53:58  pth
 * Added texturing/smooth shading to sgl_rectprism/sgl_extrude/sgl_create_prism
 * Fixed some bugs in sgl_revolve
 *
 * Revision 1.8  1995/10/04  10:59:42  pth
 * Buggy version of sgl_revolve_polygon added.
 *
 * Revision 1.7  1995/09/25  11:25:02  jop
 * Tried a new approach on the sphere
 * ,
 *
 * Revision 1.6  1995/09/18  12:01:15  jop
 * Modified smooth shaded sphere routine
 *
 * Revision 1.5  1995/09/13  18:21:47  jop
 * Implemented smooth shaded sphere
 *
 * Revision 1.4  1995/08/03  13:01:33  sm
 * changed functioning of create_sphere routine .
 *
 * Revision 1.3  1995/07/29  15:35:01  sjf
 * In a fit of depravity, renamed *_polyhderon to *_convex.
 *
 * Revision 1.2  1995/07/07  15:47:33  sjf
 * Fixed comment style that GCC doesn't like.
 *
 * Revision 1.1  1995/07/06  16:44:52  jop
 * Initial revision
 *
 *
 *;--
 *****************************************************************************/
/*
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * ++	                           includes                                      ++
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */     

#include <string.h>
#include <limits.h>
#include <math.h>

#define  MODULE_ID      MODID_DLOBJECT

#include "sgl_defs.h"
#include "sgl.h"
#include "sgl_math.h"
#include "pvrosapi.h"
#include "sglmem.h"
#include "dlobject.h"

#define PI				3.141592f
#define TWOPI			(2.0f * PI)
#define ONEOVERTWOPI	(1.0f / TWOPI)

/*
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * ++	                		SGL internal entry points	                     ++
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */	

/* U value is the angle  around the x-y circle */
static float fCalcU(sgl_vector x)
{
	float ans;
	ans=(float)atan2(x[1], x[0])/TWOPI;
	if(ans<0.0f) /* make answer between 0 and 2PI rather than -PI and PI */
	{
		ans+=1.0f;
	}
	return 1.0f-ans; 
}

/*===========================================
 * Function:	sgl_create_rectprism
 *===========================================
 *
 * Scope:		SGL
 * Authors:		Jim Page (initial code)
 *			Paul Hurley (Texturing)
 *
 * Purpose:		Creates a rectangular prism using top-level SGL commands
 *
 * Params:		float x, y, z: dimensions
 *			sgl_bool bName: TRUE if object is to have a name, FALSE if anonymous
 *			sgl_bool bTexture: TRUE then the rectprism will be textured
 *
 * Return:		+ve name or -ve error
 *
 * Globals accessed:   none
 *========================================================================================*/
int CALL_CONV sgl_create_rectprism( float x, float y, float z, 
									sgl_bool bName, sgl_bool bTexture )
{
	int nErr;

	if (x <= 0.0f)
	{
		DPFDEV ((DBG_ERROR, "sgl_create_rectprism: bad x parameter %f", x));
		nErr = sgl_err_bad_parameter;
		SglError (nErr);
	}
	else if (y <= 0.0f)
	{
		DPFDEV ((DBG_ERROR, "sgl_create_rectprism: bad y parameter %f", y));
		nErr = sgl_err_bad_parameter;
		SglError (nErr);
	}
	else if (z <= 0.0f)
	{
		DPFDEV ((DBG_ERROR, "sgl_create_rectprism: bad z parameter %f", z));
		nErr = sgl_err_bad_parameter;
		SglError (nErr);
	}
	else
	{
		sgl_vector rep_pt;

		nErr = sgl_create_convex (bName);
		
		if (nErr < 0)
		{
			DPF ((DBG_ERROR, "sgl_create_rectprism: error creating convex"));
		}
		else if( bTexture )
		{
			sgl_vector v2, v3;
			sgl_2d_vec 	rep_pt_uv={0.5f, 0.5f}, 
					v2_uv={0.0f, 0.0f}, 
					v3_uv={0.0f, 1.0f};

			/* front plane */
			rep_pt[0]=0.0f;
			rep_pt[1]=0.0f;
			rep_pt[2]=-0.5f*z;
			v2[0]=-0.5f*x;
			v2[1]=-0.5f*y;
			v2[2]=-0.5f*z;
			v3[0]=-0.5f*x;
			v3[1]=0.5f*y;
			v3[2]=-0.5f*z;
			sgl_add_plane(rep_pt, v2, v3, FALSE, NULL,NULL,NULL, rep_pt_uv, v2_uv, v3_uv);

			/* back plane */
			rep_pt[2]=0.5f*z;
			v2[2]=0.5f*z;
			v3[2]=0.5f*z;
			v2_uv[0]=1.0f;
			v3_uv[0]=1.0f;
			sgl_add_plane(rep_pt, v3, v2, FALSE, NULL,NULL,NULL, rep_pt_uv, v3_uv, v2_uv);

			/* top plane */
			rep_pt[0]=0.0f; 
			rep_pt[1]=0.5f*y;
			rep_pt[2]=0.0f;
			v2[0]=0.5f*x;
			v2[1]=0.5f*y;
			v2[2]=-0.5f*z;
			v3[0]=-0.5f*x;
			v3[1]=0.5f*y;
			v3[2]=-0.5f*z;
			rep_pt_uv[1]=0.5f;
			v2_uv[1]=0.0f;
			v3_uv[1]=0.0f;
			rep_pt_uv[0]=fCalcU(rep_pt);
			v2_uv[0]=fCalcU(v2);
			v3_uv[0]=fCalcU(v3);
			sgl_add_plane(rep_pt, v2, v3, FALSE, NULL,NULL,NULL, rep_pt_uv, v2_uv, v3_uv);

			/* bottom plane */ 
			rep_pt[1]=-0.5f*y;
			v2[1]=-0.5f*y;
			v3[1]=-0.5f*y;
			rep_pt_uv[0]=fCalcU(rep_pt);
			v2_uv[0]=fCalcU(v2);
			v3_uv[0]=fCalcU(v3);
			sgl_add_plane(rep_pt, v3, v2, FALSE, NULL,NULL,NULL, rep_pt_uv, v3_uv, v2_uv);

			/* left plane */
			rep_pt[0]=-0.5f*x; 
			rep_pt[1]=0.0f;
			rep_pt[2]=0.0f;
			v2[0]=-0.5f*x;
			v2[1]=-0.5f*y;
			v2[2]=0.5f*z;
			v3[0]=-0.5f*x;
			v3[1]=0.5f*y;
			v3[2]=0.5f*z;
			rep_pt_uv[1]=0.5f;
			v2_uv[1]=1.0f;
			v3_uv[1]=1.0f;
			rep_pt_uv[0]=fCalcU(rep_pt);
			v2_uv[0]=fCalcU(v2);
			v3_uv[0]=fCalcU(v3);
			sgl_add_plane(rep_pt, v2, v3, FALSE, NULL,NULL,NULL, rep_pt_uv, v2_uv, v3_uv);

			/* right plane */
			rep_pt[0]=0.5f*x; 
			v2[0]=0.5f*x;
			v3[0]=0.5f*x;
			rep_pt_uv[0]=fCalcU(rep_pt);
			v2_uv[0]=fCalcU(v2);
			v3_uv[0]=fCalcU(v3);
			sgl_add_plane(rep_pt, v3, v2, FALSE, NULL,NULL,NULL, rep_pt_uv, v3_uv, v2_uv);
		}
		else
		{
			/*
			// define the normals for the rectangular prism
			*/
			sgl_vector normals[6] = {{ 1.0f, 0.0f, 0.0f},
								 	{-1.0f, 0.0f, 0.0f},
			
									 { 0.0f, 1.0f, 0.0f},
								 	{ 0.0f,-1.0f, 0.0f},
		
								 	{ 0.0f, 0.0f, 1.0f},
								 	{ 0.0f, 0.0f,-1.0f}};
			int i;
			int surface;
		
			/*
			// The sizes packed into one array, for programming convenience
			*/
			sgl_vector	xyz;
#if 0
			if(bTexture)  shrink wrap object - note this has not been tested!*/
			{
				sgl_set_omap(sgl_omap_inter_normal,0);

				sgl_set_smap(sgl_smap_cylinder,1.0f,1.0f,0.0f,0.0f,0.5f*sqrt(z*z+x*x));
			}
#endif
			/*
			// get half the dimensions
			*/
			xyz[0] = x*0.5f;
			xyz[1] = y*0.5f;
			xyz[2] = z*0.5f;

			/*
			// Process all the surfaces
			*/
			for(surface = 0; surface < 6; surface ++)
			{
				/*
				// Copy the surfaces normals, and calculate the surface rep point
				*/
				for(i = 0; i < 3; i++)
				{
					/*
					// The following is a little inefficient - but who cares..
					*/
					rep_pt[i] = normals[surface][i] * xyz[i];
				}
		
				sgl_add_simple_plane (rep_pt, normals[surface], FALSE);
			}
		}
	}

	return (nErr);
}

/*===========================================
 * Function:	sgl_create_prism
 *===========================================
 *
 * Scope:		SGL
 *
 * Authors:		Jim Page (initial code)
 *				Paul Hurley (smooth shading and texturing)
 *
 * Purpose:		Generates a prism with given radius and height, and with the
 *				given number of sides using top level SGL calls. The axis of
 *				the prism lies on the y axis.
 *
 * Params:		float 		height, radius: dimensions
 * 				int			num_sides: dimensions
 *				sgl_bool 	bName: TRUE if object is to have a name, FALSE if anonymous
 *				sgl_bool bSmoothShade: TRUE if to smooth shade the prism
 *				sgl_bool bTexture: TRUE then the prism is textured.
 *
 * Return:		+ve name or -ve error
 *
 * Globals accessed:   none
 *========================================================================================*/
int CALL_CONV sgl_create_prism ( float height, float radius, int num_sides,
								 sgl_bool bName, sgl_bool bSmoothShade,
								 sgl_bool bTexture )
{
	int nErr;

	if (height <= 0.0f)
	{
		DPFDEV ((DBG_ERROR, "sgl_create_prism: bad height parameter %f", height));
		nErr = sgl_err_bad_parameter;
		SglError (nErr);
	}
	else if (radius <= 0.0f)
	{
		DPFDEV ((DBG_ERROR, "sgl_create_prism: bad radius parameter %f", radius));
		nErr = sgl_err_bad_parameter;
		SglError (nErr);
	}
	else if (num_sides < 3)
	{
		DPFDEV ((DBG_ERROR, "sgl_create_prism: bad num_sides parameter %n", num_sides));
		nErr = sgl_err_bad_parameter;
		SglError (nErr);
	}
	else
	{
		int i;  /* to count through number of sides */
	
		sgl_vector end_normals[2] = {{ 0.0f, 1.0f, 0.0f},
									 { 0.0f,-1.0f, 0.0f}};
	
		/*
		// Arrays of plane data that are passed to the add_planes routine
		*/
		sgl_vector normal;
		sgl_vector rep_pt;
	
		float angle;
	
		nErr = sgl_create_convex (bName);
		
		if (nErr < 0)
		{
			DPF ((DBG_ERROR, "sgl_create_prism: error creating convex"));
		}
		else 
		{
			sgl_vector prev,curr;
			sgl_2d_vec prev_uv, curr_uv, rep_pt_uv;
			/*
			// do the sides - y component of normal is 0.0
			*/
			normal[1] = 0.0f;
			rep_pt[1] = height/2.0f;
			curr[1]=0.0f; 
			prev[1]=0.0f;

			angle = (TWOPI * (num_sides-1) ) / num_sides;
			prev[0]=(float)cos(angle)*radius;
			prev[2]=(float)sin(angle)*radius;

			prev_uv[0]=((float) (num_sides-1))/num_sides;
			prev_uv[1]=0.5f;
			curr_uv[1]=0.5f;
			rep_pt_uv[1]=1.0f;
		
			for(i = 0; i < num_sides; i++)
			{
				angle = (TWOPI * i) / num_sides;
				if (bSmoothShade || bTexture)
				{
					curr[0]=(float)cos(angle)*radius;
					curr[2]=(float)sin(angle)*radius;
					curr_uv[0]=((float) i)/num_sides;

					rep_pt[0]=(curr[0]+prev[0])/2.0f;
					rep_pt[2]=(curr[2]+prev[2])/2.0f;
					rep_pt_uv[0]=(curr_uv[0]+prev_uv[0])/2.0f;

					if (bSmoothShade)
					{
						if (bTexture)
						{
							sgl_add_plane(rep_pt, curr, prev, FALSE,
							  rep_pt,curr,prev, rep_pt_uv,curr_uv,prev_uv);
						}
						else
						{
							sgl_add_plane(rep_pt, curr, prev, FALSE,
							  rep_pt,curr,prev, NULL,NULL,NULL);
						}
					}
					else
					{
						/* bTexture must be TRUE */
						sgl_add_plane(rep_pt, curr, prev, FALSE,
						  NULL,NULL,NULL, rep_pt_uv,curr_uv,prev_uv);
					}

					prev[0]=curr[0];
					prev[2]=curr[2];
					prev_uv[0]=curr_uv[0];
				}
				else
				{
					/*
					// Calculate the normal
					*/
					normal[0] = (float)cos(angle);
					normal[2] = (float)sin(angle);
	
					rep_pt[0] = normal[0] * radius;
					rep_pt[2] = normal[2] * radius;
					sgl_add_simple_plane (rep_pt, normal, FALSE);
				}
			}

			/*
			// Do the top
			*/
			rep_pt[0] = 0.0f;
			rep_pt[1] = height / 2.0f;
			rep_pt[2] = 0.0f;
		
			if(bTexture)
			{
				curr[0]=0.0f;
				curr[1]=height/2.0f; 
				curr[2]=radius;

				prev[0]=radius;
				prev[1]=height/2.0f;
				prev[2]=0.0f;

				rep_pt_uv[0]=0.5f;
				rep_pt_uv[1]=0.5f;

				curr_uv[0]=0.5f;
				curr_uv[1]=1.0f;

				prev_uv[0]=1.0f;
				prev_uv[1]=0.5f;
	
				sgl_add_plane(rep_pt,curr,prev, FALSE, NULL,NULL,NULL,
				  rep_pt_uv,curr_uv,prev_uv);

				/* now do the bottom */

				rep_pt[1]*=-1.0f;
				curr[1]*=-1.0f;
				prev[1]*=-1.0f;
				sgl_add_plane(rep_pt,prev,curr, FALSE, NULL,NULL,NULL,
				  rep_pt_uv,curr_uv,prev_uv);
			}
			else
			{
				sgl_add_simple_plane (rep_pt, end_normals[0], FALSE);
				/*
				// Do the bottom
				*/
				rep_pt[1] *= -1;
		
				sgl_add_simple_plane (rep_pt, end_normals[1], FALSE);
			}			
		}
	}

	return (nErr);
}

/*===========================================
 * Function:	sgl_create_sphere
 *===========================================
 *
 * Scope:		SGL
 * Author:		Jim Page
 *
 *
 * Purpose:		Generates a sphere with given radius, and with the
 *				given number of sides (for quarter of equator).
 *				The axis of the sphere lies on the y axis.
 *
 * Params:		int 		sph_type:	if TRUE generates sphere with decreasing
 *										faces towards sphere poles.
 *				float 		radius,
 *				int 		num_sides
 *				sgl_bool 	bName: TRUE if object is to have a name, FALSE if anonymous
 *				sgl_bool 	bSmoothShade: TRUE to smooth shade, FALSE to flat shade
 *				sgl_bool 	bTexture: TRUE to texture
 *
 * Return:		+ve name or -ve error
 *
 * Globals accessed:   none
 *========================================================================================*/
int CALL_CONV sgl_create_sphere ( int sph_type, float radius, int num_sides,
								  sgl_bool bName, sgl_bool bSmoothShade, 
								  sgl_bool bTexture )
{
	int nErr;

	if (radius <= 0.0f)
	{
		DPFDEV ((DBG_ERROR, "sgl_create_sphere: bad radius parameter %f", radius));
		nErr = sgl_err_bad_parameter;
		SglError (nErr);
	}
	else if (num_sides < 1)
	{
		DPFDEV ((DBG_ERROR, "sgl_create_sphere: bad num_sides parameter %n", num_sides));
		nErr = sgl_err_bad_parameter;
		SglError (nErr);
	}
	else
	{
		#if 1
		
		nErr = sgl_create_convex (bName);
		
		if (nErr < 0)
		{
			DPF ((DBG_ERROR, "sgl_create_sphere: error creating convex"));
		}
		else
		{
			float ang_step, half_step, cur_ang, up_ang, up_ang_step,
				    up_half_step;
			int curside, upside, versides, n_sides;
			
			versides = num_sides;

			n_sides = 4*num_sides;
		
   			up_ang_step = TWOPI / n_sides;
	   		up_half_step = up_ang_step * 0.5f;
		
			for (upside = 0; upside < versides; upside++)
			{
   				up_ang = up_ang_step * upside;

				for (curside = 0; curside < n_sides; curside++)
				{
		   			ang_step = TWOPI / n_sides;
	  		 		half_step = ang_step * 0.5f;
		
					cur_ang = ang_step * curside;
		
					if (bSmoothShade || bTexture)
					{
						sgl_vector s[4];
						sgl_2d_vec u[4];

						/* make up 3 points on the sphere facet */
												
						s[0][0] = radius * (float)cos (cur_ang) * (float)cos (up_ang + up_ang_step);
						s[0][2] = radius * (float)sin (cur_ang) * (float)cos (up_ang + up_ang_step);
						s[0][1] = radius * (float)sin (up_ang + up_ang_step);

						s[1][0] = radius * (float)cos (cur_ang + ang_step) * (float)cos (up_ang + up_ang_step);
						s[1][2] = radius * (float)sin (cur_ang + ang_step) * (float)cos (up_ang + up_ang_step);
						s[1][1] = radius * (float)sin (up_ang + up_ang_step);

						s[2][0] = radius * (float)cos (cur_ang) * (float)cos (up_ang);
						s[2][2] = radius * (float)sin (cur_ang) * (float)cos (up_ang);
						s[2][1] = radius * (float)sin (up_ang);

						s[3][0] = radius * (float)cos (ang_step + cur_ang) * (float)cos (up_ang);
						s[3][2] = radius * (float)sin (ang_step + cur_ang) * (float)cos (up_ang);
						s[3][1] = radius * (float)sin (up_ang);
						
						if(s[0][1]!=s[1][1]) 
						{
							s[1][0] *= 0.95f;
							s[1][1] *= 0.95f;
							s[1][2] *= 0.95f;
							s[2][0] *= 0.95f;
							s[2][1] *= 0.95f;
							s[2][2] *= 0.95f;
						}
						
						u[0][0] = ONEOVERTWOPI * (cur_ang);
						u[0][1] = 0.5f * (1.0f + ((float)sin (up_ang + up_ang_step)));

						u[1][0] = ONEOVERTWOPI * (cur_ang + ang_step);
						u[1][1] = 0.5f * (1.0f + ((float)sin (up_ang + up_ang_step)));

						u[2][0] = ONEOVERTWOPI * (cur_ang);
						u[2][1] = 0.5f * (1.0f + ((float)sin (up_ang)));

						u[3][0] = ONEOVERTWOPI * (cur_ang + ang_step);
						u[3][1] = 0.5f * (1.0f + ((float)sin (up_ang)));

						/* top hemisphere ... */
						
						if (bSmoothShade)
						{
							if (bTexture)
							{
								sgl_add_plane (s[0],s[1],s[2], FALSE,
								  s[0],s[1],s[2], u[0],u[1],u[2]);
							}
							else
							{
								sgl_add_plane (s[0],s[1],s[2], FALSE,
								  s[0],s[1],s[2], NULL,NULL,NULL);
							}
						}
						else
						{
							/* bTexture must be TRUE */
							sgl_add_plane (s[0],s[1],s[2], FALSE,
							  NULL,NULL,NULL, u[0],u[1],u[2]);
						}
			 			
						if (s[0][1] != s[1][1])
						{
							if (bSmoothShade)
							{
								if (bTexture)
								{
				 					sgl_add_plane (s[1],s[3],s[2], FALSE,
									  s[1],s[3],s[2], u[1],u[3],u[2]);
								}
								else
								{
									sgl_add_plane (s[1],s[3],s[2], FALSE,
									  s[1],s[3],s[2], NULL,NULL,NULL);
								}
							}
							else
							{
								/* bTexture must be TRUE */
								sgl_add_plane (s[1],s[3],s[2], FALSE,
								  NULL,NULL,NULL, u[1],u[3],u[2]);
							}
						}

						/* bottom hemisphere. Don't forget to reverse the direction of the face */
						s[0][1] = -s[0][1];
						s[1][1] = -s[1][1];
						s[2][1] = -s[2][1];
						s[3][1] = -s[3][1];

						u[0][1] = - u[0][1];
						u[1][1] = - u[1][1];
						u[2][1] = - u[2][1];
						u[3][1] = - u[3][1];

						if (bSmoothShade)
						{
							if (bTexture)
							{
								sgl_add_plane (s[0],s[2],s[1], FALSE,
								  s[0],s[2],s[1], u[0],u[2],u[1]);
							}
							else
							{
								sgl_add_plane (s[0],s[2],s[1], FALSE,
								  s[0],s[2],s[1], NULL,NULL,NULL);
							}
						}
						else
						{
							/* bTexture must be TRUE */
							sgl_add_plane (s[0],s[2],s[1], FALSE,
							  NULL,NULL,NULL, u[0],u[2],u[1]);
						}

						if (s[0][1]!=s[1][1]) 
						{
							if (bSmoothShade)
							{
								if (bTexture)
								{
				 					sgl_add_plane (s[1],s[2],s[3], FALSE,
									  s[1],s[2],s[3], u[1],u[2],u[3]);
								}
								else
								{
									sgl_add_plane (s[1],s[2],s[3], FALSE,
									  s[1],s[2],s[3], NULL,NULL,NULL);
								}
							}
							else
							{
								/* bTexture must be TRUE */
								sgl_add_plane (s[1],s[2],s[3], FALSE,
								  NULL,NULL,NULL, u[1],u[2],u[3]);
							}
						}
					}
					else
					{
						sgl_vector  domeside;
	
						domeside[0] = radius * (float)cos (half_step + cur_ang) * (float)cos (up_half_step + up_ang);
						domeside[2] = radius * (float)sin (half_step + cur_ang) * (float)cos (up_half_step + up_ang);
						domeside[1] = radius * (float)sin (up_half_step + up_ang);
			
						sgl_add_simple_plane (domeside, domeside, FALSE);
						
						domeside[1] = -domeside[1];                      
			
						sgl_add_simple_plane (domeside, domeside, FALSE);
					}
				}

	     		if (sph_type)
				{
	            	n_sides-=4;
				}
			}
    	}
		#else

		sgl_vector *vVertices;
		int nArraySize = 2 + (((num_sides * 2) - 1) * num_sides * 4);

		vVertices = SGLMalloc (nArraySize * sizeof (sgl_vector));
		
		if (vVertices)
		{
			int j, k;
			int	nF;
			int num_horz_sides = num_sides * 4;
			int num_vert_sides = (num_sides * 2) - 1;
			float ang, up_ang, ang_step = TWOPI / (4.0f * num_sides);
						
			nF = 0;
			
			vVertices[0][0] = 0;
			vVertices[0][1] = -radius;
			vVertices[0][2] = 0;

			vVertices[nArraySize - 1][0] = 0;
			vVertices[nArraySize - 1][1] = radius;
			vVertices[nArraySize - 1][2] = 0;

			nF++;

			up_ang = ang_step - PI;
			
			for (j = 0; j < num_vert_sides; ++j)
			{
				float CosUpAng = (float)cos (up_ang);
				float SinUpAng = (float)sin (up_ang);

				ang = 0.0f;
				
				for (k = 0; k < num_horz_sides; ++k)
				{
					float CosAng = (float)cos (ang);
					float SinAng = (float)sin (ang);

					vVertices[nF][0] = radius * CosAng * CosUpAng;
					vVertices[nF][1] = radius * SinUpAng;
					vVertices[nF][2] = radius * SinAng * CosUpAng;

					nF++;

					ang += ang_step;
				}

				up_ang += ang_step;
			}
			
			ASSERT (nF == nArraySize - 1);
			
			nErr = sgl_create_mesh (bName);

			if (nErr >= sgl_no_err)
			{
				sgl_add_vertices (nArraySize, vVertices, vVertices, NULL);
			}

			for (j = 0; j < num_vert_sides; ++j)
			{
				for (k = 0; k < num_horz_sides; ++k)
				{
					int vFace[4];
					int nRowPos = (j * num_horz_sides) + 1;

					if (j == 0)
					{
						vFace[0] = 0;
						vFace[1] = nRowPos + k;
						vFace[2] = (k == num_horz_sides - 1) ? nRowPos : nRowPos + k + 1;
						sgl_add_face (3, vFace);
					}
					else if (j == num_vert_sides - 1)
					{
						vFace[0] = (k == num_horz_sides - 1) ? nRowPos - num_horz_sides : (nRowPos - num_horz_sides) + k + 1;
						vFace[1] = vFace[0] - 1;
						vFace[2] = nArraySize - 1;
						sgl_add_face (3, vFace);
					}
/*					else
					{
						vFace[0] = nRowPos + k;
						vFace[1] = (k == num_horz_sides - 1) ? nRowPos : vFace[0] + 1;
						vFace[2] = vFace[1] - num_horz_sides;
						vFace[3] = vFace[2] - 1;
						sgl_add_face (4, vFace);

						vFace[0] = nRowPos - num_horz_sides + k;
						vFace[1] = (k == num_horz_sides - 1) ? nRowPos - num_horz_sides : nRowPos - num_horz_sides + k + 1;
						vFace[2] = nRowPos + k;
						sgl_add_face (3, vFace);

						vFace[0] = (k == num_horz_sides - 1) ? nRowPos - num_horz_sides : nRowPos - num_horz_sides + k + 1;
						vFace[1] = (k == num_horz_sides - 1) ? nRowPos : nRowPos + k + 1;
						vFace[2] = nRowPos + k;
						sgl_add_face (3, vFace);
					}*/
				}
			}

			SGLFree (vVertices);
		}
		else
		{
			nErr = sgl_err_no_mem;
		}
		
		#endif
	}
	
	return (nErr);
}   

/*===========================================
 * Function:	vCircumCentre
 *===========================================
 *
 * Scope:		static to this module
 * Author:		Paul Hurley
 *															 
 * Purpose:		calculates the circumcentre of a 2-d triangle describe by the vertices P1P2P3.
 *				The algorithm is obtained from p.22 of Graphic Gems I. The circumcentre is the 
 *				point that is equidistant from all 3 points of the triangle.
 *
 * Params:		sgl_2d_vec centre - the resultant centre is stored here.
 *				sgl_2d_vec P1,P2,P3 - the three points that make up the triangle and will 
 *				thus lie on the circle.		
 *
 * Return:		the radius of the resultant circle.
 *
 * Globals accessed:   none
 *========================================================================================*/
static void vCircumCentre(sgl_2d_vec centre, sgl_2d_vec P1, sgl_2d_vec P2, sgl_2d_vec P3)
{
	float d1,d2,d3,c1,c2,c3,c_2;

	/* d1= (P3-P1).(P2-P1) */
	d1= (P3[0]-P1[0])*(P2[0]-P1[0])+(P3[1]-P1[1])*(P2[1]-P1[1]);
	/* d2= (P3-P2).(P1-P2) */
	d2= (P3[0]-P2[0])*(P1[0]-P2[0])+(P3[1]-P2[1])*(P1[1]-P2[1]);
	/* d3= (P1-P3).(P2-P3) */
	d3= (P1[0]-P3[0])*(P2[0]-P3[0])+(P1[1]-P3[1])*(P2[1]-P3[1]);
	c1=d2*d3; /* c1=d2d3 */
	c2=d3*d1; /* c2=d3d1 */
	c3=d1*d2; /* c3=d1d2 */
	
	c_2=0.5f/(c1+c2+c3); /* 1/(2c) where c=c1+c2+c3; */

	/* centre=1/(2c)*((c2+c3)P1+(c3+c1)P2+(c1+c2)P3) */
	centre[0]=c_2*((c2+c3)*P1[0]+(c3+c1)*P2[0]+(c1+c2)*P3[0]);
	centre[1]=c_2*((c2+c3)*P1[1]+(c3+c1)*P2[1]+(c1+c2)*P3[1]);
} 


/*===========================================
 * Function:	vCircumNormal
 *===========================================
 *
 * Scope:		static to this module
 * Author:		Paul Hurley
 *															 
 * Purpose:		
 *
 * Params:		sgl_vector normal: 
 * 				sgl_vector point
 * 				int i
 *				int nopoints
 *				sgl_2d_vec *circ_cent
 *
 * Return:		
 *
 * Globals accessed:   none
 *========================================================================================*/
static void vCircumNormal(sgl_vector normal, sgl_vector point, int i, int nopoints, 
sgl_2d_vec *circ_cent)
{
 	if(i%2!=0) /* if an odd vertex then it must lie in the centre of one of our triangles */
	{
		/* 
		// normal is vector from circum centre to point 
		*/
		normal[0]=point[0]-circ_cent[(i-1)/2][0];
		normal[1]=point[1]-circ_cent[(i-1)/2][1];
	}
	else
	{
		/* 
		// normal is average of the 2 unit vectors from the two circumcircles that the point is in.
		*/
		sgl_2d_vec v;
		float len[2];

		int nPrev= (i==0) ? (nopoints-1)/2:i/2-1;
		
		v[0]=point[0]-circ_cent[nPrev][0];
		v[1]=point[1]-circ_cent[nPrev][1];
		len[0]=1.0f/((float) sqrt(v[0]*v[0]+v[1]*v[1]));
		v[0]=point[0]-circ_cent[i/2][0];
		v[1]=point[1]-circ_cent[i/2][1];
		len[1]=1.0f/((float) sqrt(v[0]*v[0]+v[1]*v[1]));

		normal[0]=0.5f*((len[0]+len[1])*point[0]-
		  (len[0]*circ_cent[nPrev][0]+len[1]*circ_cent[i/2][0]));
		normal[1]=0.5f*((len[0]+len[1])*point[1]-
		  (len[0]*circ_cent[nPrev][1]+len[1]*circ_cent[i/2][1]));	
	}
}

/*===========================================
 * Function:	fAngle
 *===========================================
 *
 * Scope:		static to this module
 * Author:		Paul Hurley
 *															 
 * Purpose:		Determines the angle PRQ using Cos theta= PR.QR / |PR||QR|. Note that another
 * 				way (not used here) to get the angle is that 
 *				Tan theta= (m1-m2)/(1+m1m2) where m1,m2 are slopes of the lines PR and RQ.
 *
 * Params:		sgl_2d_vec P, R, Q - the 3 2-D points, with the angle at R.
 *
 * Return:		the resultant angle in radians.
 *
 * Globals accessed:   none
 *========================================================================================*/
static float fAngle(sgl_2d_vec P, sgl_2d_vec R, sgl_2d_vec Q)
{
	sgl_2d_vec PR,QR;
	float fLen,ans;

	/* determine vectors PR and PQ */
	PR[0]=R[0]-P[0]; PR[1]=R[1]-P[1];
	QR[0]=R[0]-Q[0]; QR[1]=R[1]-Q[1];
	   
	fLen= (float) sqrt((PR[0]*PR[0]+PR[1]*PR[1])*(QR[0]*QR[0]+QR[1]*QR[1]));

	if( fLen==0 ) /* P=R or R=Q */
	{
		/* angle is 180 degrees */
		return PI;
	}

	/* note no divide by zero since fLen!=0 by above P=R/R=Q check */
	ans=(float) acos((PR[0]*QR[0]+PR[1]*QR[1])/fLen);
	return ans;
}

/*===========================================
 * Function:	fBoundingCircle
 *===========================================
 *
 * Scope:		static to this module
 * Author:		Paul Hurley
 *															 
 * Purpose:		For a given number of 2-d Points, the smallest bounding circle is obtained. 
 *				See Graphics Gems II p.14 for details of the algorithm used. 
 *
 * Params:		sgl_2d_vec centre - the resultant centre of the bounding circle is stored here.
 *				sgl_2d_vec *pt - the 2-D points.
 				int npts - the number of 2-D points
 *
 * Return:		the radius of the resultant circle.
 *
 * Globals accessed:   none
 *========================================================================================*/
static float fBoundingCircle(sgl_2d_vec centre, sgl_2d_vec *pt, int npts)
{
	/* A bounding circle is obtained from two or three points. These store the possible points */
	sgl_2d_vec P,Q,R;

	int i; /* counter */

	float fMinAngle, fCurAngle;
	
	/* determine P - the point with minimum x coordinate */
	P[0]=pt[0][0]; P[1]=pt[0][1]; /* let P=1st point */
	for(i=1;i<npts;i++)
	{
		if(pt[i][0]<P[0]) /* if x-coord is smaller */
		{
			P[0]=pt[i][0]; /* then it become the new 'P' */
			P[1]=pt[i][1];
		}
	}

	/* 
	// determine Q - the point such that the angle of PQ with x-axis is minimal 
	// angle with x axis is given by arctan(m) where m is the slope of the line PQ 
	// being careful not to calculate the slope of 0/0 
	*/
	Q[0]=pt[0][0]; Q[1]=pt[0][1]; /* let Q=1st point */

	if(Q[1]==P[1] && Q[0]==P[0]) /* if Q=P */
	{
		fMinAngle=1000.0f; /* just make it too large */
	}
	else
	{
		fMinAngle=(float) atan2(Q[1]-P[1],Q[0]-P[0]); /* slope of QP */
	}

	for(i=1;i<npts;i++)
	{
		if(pt[i][1]!=P[1] || pt[i][0]!=P[0]) /* only proceed if pt[i]!=P */
		{
			fCurAngle= (float) atan2(pt[i][1]-P[1],pt[i][0]-P[0]); 
			if(fCurAngle<fMinAngle) /* angle is smaller */
			{
				Q[0]=pt[i][0]; /* so pt[i] become the new 'Q' */
				Q[1]=pt[i][1];
				fMinAngle=fCurAngle;
			}
		}
	}

#define SGL_IS_BUGGED	TRUE

	while( SGL_IS_BUGGED ) /* infinite loop */
	{
		/* determine R - the point such that the abs value of the angle PRQ is minimal */
		R[0]=pt[0][0]; R[1]=pt[0][1]; /* let R=1st point */
		fMinAngle= (float) fabs(fAngle(P,R,Q)); /* |angle PRQ| */
		for(i=1;i<npts;i++)
		{
			fCurAngle= (float) fabs(fAngle(P,pt[i],Q));
	
			if(fCurAngle<fMinAngle) /* if |angle| is smaller */
			{
				R[0]=pt[i][0]; /* pt[i] become the new R */
				R[1]=pt[i][1];
				fMinAngle=fCurAngle;
			}
		}

		/* if angle PRQ is obtuse */
		if( (fAngle(P,R,Q)) > PI/2 ) /* being quite lazy by recalculating angle */
		{																	  
			/* bounding circle is given by 2 points P and Q. R isn't involved. */
			centre[0]=(P[0]+Q[0])/2;
			centre[1]=(P[1]+Q[1])/2;
			break; /* we're done */
		}
		/* else if angle PQR is obtuse */
		else if( fAngle(P,Q,R) > PI/2 )
		{
			/* replace Q by R and look for a new R (by repeating loop) */
			Q[0]=R[0]; Q[1]=R[1];
		}
		/* else if angle RPQ is obtuse */
		else if( fAngle(R,P,Q) > PI/2 ) /* being incredibily lazy and not just subtracting the */
		{								/* previous 2 angles from PI */
			/* replace P by R and look for a new R */
			P[0]=R[0]; P[1]=R[1];
		}
		/* else PQR triangle has all acute angles */
		else
		{
			/* now know that triangle PQR forms the bounding circle 
			   so obtain the triangle's circumcentre */
			vCircumCentre(centre,P,Q,R);
			break; /* and we're out of here */
		}
  	} /* end while */

	/* returning the radius of the bounding circle */
	return (float) sqrt( 0.5f*((P[0]-Q[0])*(P[0]-Q[0])+(P[1]-Q[1])*(P[1]-Q[1])) );
}

/* returns the arctan of x/y between 0 and TWOPI */
float fCircAngle(float x, float y)
{
	float at= (float) atan2(x,y);
	if(at<0) at+=TWOPI;
	return at;
}

/*===========================================
 * Function:	sgl_create_extruded_poly
 *===========================================
 *
 * Scope:		SGL
 * Authors:		Jim Page (initial code)
 *		   		Paul Hurley (smooth shading and texturing)
 *
 * Purpose:		Generates a prism  that is the extrusion of the supplied polygon. The
 *				polygon lies in the xy plane and is extruded back in z. The format of
 *				the list of points is simply an array of xy coordinates,
 *				specified as floats. IT IS ASSUMED THAT THE POINTS ARE SUPPLIED IN
 *				CLOCKWISE ORDER. There must be at least 3 points. NOTE: Currently the
 *				polygon MUST BE CONVEX.
 *
 * Params:		int 		numpoints	
 *				float 		point_list[][2]
 *				float		depth
 *				sgl_bool 	bName: TRUE if object is to have a name, FALSE if anonymous
 *				sgl_bool	bSmoothShade: if TRUE generates smooth normals
 *				sgl_bool 	bTexture: TRUE to texture
 *
 * Return:		+ve name or -ve error
 *
 * Globals accessed:   none
 *========================================================================================*/
int CALL_CONV sgl_create_extruded_poly ( int numpoints,
										 float point_list[][2], float depth,
										 sgl_bool bName,
										 sgl_bool bSmoothShade,
										 sgl_bool bTexture )
{
	int nErr;

	if (numpoints < 3)
	{
		DPFDEV ((DBG_ERROR, "sgl_create_extruded_poly: bad numpoints parameter %n", numpoints));
		nErr = sgl_err_bad_parameter;
		SglError (nErr);
	}
	else if (depth <= 0.0f)
	{
		DPFDEV ((DBG_ERROR, "sgl_create_extruded_poly: bad depth parameter %f", depth));
		nErr = sgl_err_bad_parameter;
		SglError (nErr);
	}
	else if (!point_list)
	{
		DPFDEV ((DBG_ERROR, "sgl_create_extruded_poly: null point_list parameter"));
		nErr = sgl_err_bad_parameter;
		SglError (nErr);
	}
	else
	{
		nErr = sgl_create_convex (bName);
		
		if (nErr < 0)
		{
			DPF ((DBG_ERROR, "sgl_create_extruded_poly: error creating convex"));
		}
		else
		{
			sgl_2d_vec *c;
			/*
			// Arrays of plane data that are passed to the add_planes routine
			*/
			sgl_vector normal;
			sgl_vector rep_pt, rep_pt_smoothnormal;	
		
			sgl_vector prev, curr, prev_normal, curr_normal;
			sgl_2d_vec polyC, prev_uv, curr_uv, rep_pt_uv;

			float fPolyRadius;
		
			int i;

			/*
			// get the coords of the last point
			*/
			prev[0] = point_list[numpoints -1][0];    
			prev[1] = point_list[numpoints -1][1];

			/*
			// set the z-coords
			*/
			prev[2] = 0.0f;
			curr[2]	= 0.0f;

			fPolyRadius=fBoundingCircle(polyC, point_list, numpoints);				

			if(bTexture || bSmoothShade)
			{
				/*//// 
				// texturing system is as follows:
				// if it's not a flat plane then u is the angle the vector makes with the horizontal of the
				//	centre of the polygon and v represents the depth into the extruded polygon.
				//	if it is a flat plane (i.e. front and back plane) then (u,v)=(0.5,0.5) at the polygon centre and
				//	all other points are adjusted accordingly.
				/// */
				curr_uv[1]=prev_uv[1]=0.0f;
				rep_pt_uv[1]=0.5f;   
				prev_uv[0]=ONEOVERTWOPI*fCircAngle(prev[0]-polyC[0],prev[1]-polyC[1]);

				/*////
				// set the z smooth normal's to zero
				/// */
				rep_pt_smoothnormal[2]=0.0f;
				prev_normal[2] = 0.0f;
				curr_normal[2] = 0.0f;
			
				/*////
				// there are 2 types of smooth shading available. Spherical smooth shading is the default.
				// 1. 'Circumcentre' smooth shading: Here the circumcentre of every 2 edges is calculated. Then
				// the normal is the distance from this centre. At the corners the normal is averaged. This shading
				// might reflect the polygon shape better than spherical type.
				// 2. 'Spherical' smooth shading: The normals are the vector of the point from the centre of the 
				// polygon.
				// See function vCircumNormal for more details.
				/// */
				if(bSmoothShade==2)
				{
					/* TODO: avoid the need for this memory allocation by having a c_prev and a c_curr! */
					c= (sgl_2d_vec *) SGLCalloc((numpoints+1)/2, sizeof(sgl_2d_vec));
					if(!c)
					{
						return sgl_err_no_mem;
					}
	
					/* calculate the circumcentre for each edge */
					for(i=0;i<numpoints;i+=2)
					{
						vCircumCentre(c[i/2], point_list[i], point_list[(i+1)%numpoints],
					  	point_list[(i+2)%numpoints]);
					}

					/* calculate prev's normal */
					vCircumNormal(prev_normal, prev, numpoints-1, numpoints,c);
				}
				else if( bSmoothShade )
				{
					/* prev's normal is it's vector from the polygon centre (i.e. spherical shading) */
					prev_normal[0]=prev[0]-polyC[0];
					prev_normal[1]=prev[1]-polyC[1];
				}					 
			}
  	
			/*
			// Step through all the points generating
			// the planes that make up the edges
			*/
			for(i = 0; i < numpoints; i++)
			{
				curr[0] = point_list[i][0];
				curr[1] = point_list[i][1];

				/*
				// Choose a representative point
				*/
				rep_pt[0] = (prev[0] + curr[0]) * 0.5f;
				rep_pt[1] = (prev[1] + curr[1]) * 0.5f;
				rep_pt[2] = depth * 0.5f;

				if(bSmoothShade || bTexture)
				{
					curr_uv[0]=ONEOVERTWOPI*fCircAngle(curr[0]-polyC[0],curr[1]-polyC[1]);
					rep_pt_uv[0]=ONEOVERTWOPI*fCircAngle(rep_pt[0]-polyC[0],rep_pt[1]-polyC[1]);
						
					if (bSmoothShade==2)
					{
						vCircumNormal(curr_normal, curr, i, numpoints, c);
						rep_pt_smoothnormal[0]=rep_pt[0]-c[i/2][0];
						rep_pt_smoothnormal[1]=rep_pt[1]-c[i/2][1];
					}
					else if(bSmoothShade)
					{
						curr_normal[0]=curr[0]-polyC[0];
						curr_normal[1]=curr[1]-polyC[1];

						rep_pt_smoothnormal[0]=rep_pt[0]-polyC[0];
						rep_pt_smoothnormal[1]=rep_pt[1]-polyC[1];
					}

					if (bSmoothShade)
					{
						if (bTexture)
						{
							sgl_add_plane(rep_pt, curr, prev, FALSE,
							  rep_pt_smoothnormal, curr_normal, prev_normal,
							  rep_pt_uv, curr_uv, prev_uv);
						}
						else
						{
							sgl_add_plane(rep_pt, curr, prev, FALSE,
							  rep_pt_smoothnormal, curr_normal, prev_normal,
							  NULL,NULL,NULL);
						}
					}
					else
					{
						/* bTexture must be TRUE */
						sgl_add_plane(rep_pt, curr, prev, FALSE,
						  NULL,NULL,NULL,
						  rep_pt_uv, curr_uv, prev_uv);
					}

					/*
					// Update the previous point's smooth shading normal and uv
					*/
					prev_normal[0] = curr_normal[0];
					prev_normal[1] = curr_normal[1];
					prev_uv[0] = curr_uv[0];
				}
				else
				{
					/*
					// calculate the normal
					*/
					normal[0] = prev[1] - curr[1];
					normal[1] = curr[0] - prev[0];
					normal[2] = 0.0f;

					sgl_add_simple_plane (rep_pt, normal, FALSE);
				}
	
				/*
				// Update the previous point
				*/
				prev[0] = curr[0];
				prev[1] = curr[1];
	
			} /*end for*/
		
			/*////
			// the representative point for the ends is the bounding circle centre (=poly centre)
			/// */
			rep_pt[0] = polyC[0];
			rep_pt[1] = polyC[1];
			rep_pt[2] = 0.0f;
	
			/*////
			// Add the front and back planes (different method if no texturing)
			/// */
			if(bTexture)
			{
				/* flat shade but texture */
				curr[0]=point_list[0][0];
				curr[1]=point_list[0][1];
				prev[0]=point_list[1][0];
				prev[1]=point_list[1][1];

				curr_uv[0]=((curr[0]-polyC[0])/fPolyRadius + 1)/2;
				curr_uv[1]=((curr[1]-polyC[1])/fPolyRadius + 1)/2;

				prev_uv[0]=((prev[0]-polyC[0])/fPolyRadius + 1)/2;
				prev_uv[1]=((prev[1]-polyC[1])/fPolyRadius + 1)/2;

				rep_pt_uv[0]=0.5f;
				rep_pt_uv[1]=0.5f;

				/*////
				// Add front plane
				/// */
				sgl_add_plane(rep_pt, curr, prev, FALSE, NULL,NULL,NULL,
				  rep_pt_uv, curr_uv, prev_uv);

				rep_pt[2] = curr[2] = prev[2] = depth;

				/* flip the u- coordinates for the back plane */
				prev_uv[0]=1.0f-prev_uv[0];
				curr_uv[0]=1.0f-curr_uv[0];
				
				/*////
				// Add back plane
				/// */
				sgl_add_plane(rep_pt, prev, curr, FALSE, NULL,NULL,NULL,
				  rep_pt_uv, prev_uv, curr_uv);
			}
			else
			{
				normal[0] =  0.0f;
				normal[1] =  0.0f;
				normal[2] = -1.0f;
				sgl_add_simple_plane (rep_pt, normal, FALSE);

				/*////
				// Add bottom plane
				/// */
				rep_pt[2] = depth;

				normal[0] =  0.0f;
				normal[1] =  0.0f;
				normal[2] =  1.0f;
				sgl_add_simple_plane (rep_pt, normal, FALSE);
			}

			if(bSmoothShade==2) /* used circumcentre shading - return memory */
			{
				SGLFree(c);
			}	
		}	
	}

	return (nErr);
}

/* -----------------------------------------
sgl_revolve_polygon and related functions
------------------------------------------*/

/* used by nClosedRevolve: accesses the yth rotation of the point x */
#define lattice(x,y) 			lat[(x-1)*(nNoSects-1)+(y)-1]
/* used by nOpenRevolve: accesses the yth rotation of the point x */
#define latticeOut(x,y) 		lat[(x)*(nNoSects-1)+(y)-1]

/* if b.(x-y)=0 then the line created by edge xy is perpendicular to the axis a+lambda*b */
#define EdgePerpToAxis(x,y)		(b[0]*((x)[0]-(y)[0])+b[1]*((x)[1]-(y)[1])+b[2]*((x)[2]-(y)[2])==0)

/*////
// returns the vector from a to the point which is perp to v and the axis. i.e. if you consider
// the axis as the 'y-axis' and a as the 'origin', this returns the value of the 'y' coordinate
// of v. lambda=b.(v-a)
/// */
#define lambda(v) 		(b[0]*((v)[0]-a[0])+b[1]*((v)[1]-a[1])+b[2]*((v)[2]-a[2]))

/*////
// static variables used by vAddPlane to enable texture coords to be calculated - local to this
// module.
/// */
static int nCurSects;
static float fObjectHeight, fObjectBottom, fObjectRadius;

/*===========================================
 * Function:	fVecPerpToAxis
 *===========================================
 *
 * Scope:		static to this module
 * Author:		Paul Hurley.
 *
 * Purpose:		Calculates the vector perpendicular to the axis a+lambda*b, which
 *		goes through the point v. The answer is given by the formula, ans = (v-a) - (b.(v-a))b.
 *		If v is on the axis then a vector along the axis is supplied, the direction being 
 * 		positive (negative) if it is above (below) the vector a on the axis. 
 *		i.e. if you consider a as the 'origin' and the axis as the 'y-axis', this calculates
 *		the vector corresponding to v's 'x-coord'.
 *  
 *
 * Params:		ans - the resultant vector.
 *				v - the vector to be processed.
 *				a, b - the axis.
 *
 * Return:		lamda - see #define of lambda above  	
 *
 * Globals accessed:   -
 *========================================================================================*/
static float fVecPerpToAxis(sgl_vector ans, sgl_vector v, sgl_vector a, sgl_vector b)
{
	sgl_vector transv;
	
	
	float dp;
	
	transv[0]=v[0]-a[0];
	transv[1]=v[1]-a[1];
	transv[2]=v[2]-a[2];
	
	dp=b[0]*transv[0]+b[1]*transv[1]+b[2]*transv[2];

	/* see if it lies on the axis - i.e. is x/b1=y/b2=z/b3=lambda?*/
	if( transv[0]*b[1]==transv[1]*b[0] && transv[1]*b[2]==transv[2]*b[1] )
	{
		VecCopy(transv,ans); /* ans=transv */
	}
	else	
	{	/* use formula above */
		sgl_vector sm;

		sm[0] = dp*b[0];
		sm[1] = dp*b[1];
		sm[2] = dp*b[2];

		VecSub(transv, sm, ans);
	}

	return dp;
}

/*===========================================
 * Function:	vRotateAboutAxis
 *===========================================
 *
 * Scope:		static to this module
 * Author:		Paul Hurley.
 *
 * Purpose:		For a given axis a point is rotated a given no of degrees.
 * 			Axis supplied is of the form r=a+lamda*b. a is a positional vector
 *			for the axis while b MUST BE A UNIT VECTOR along the axis. The
 * 			matrix is obtained from p.466 of Graphics Gems I.
 *
 * Params:		ans - 3-d vec storing the rotated point.
 *				a,b - the axis of rotation
 *				theta - the angle of rotation
 *				pt - what's to be rotated
 * Returns:		None 	
 *
 * Globals accessed:   -
 *========================================================================================*/
static void vRotateAboutAxis(sgl_vector ans, sgl_vector a, sgl_vector b, float theta, 
sgl_vector pt)
{
	float s, c, t;

	sgl_vector transpt;

	s=(float) sin(theta);
	c=(float) cos(theta);
	t=1-c;

	/* translate pt by a */
	transpt[0]=pt[0]-a[0];
	transpt[1]=pt[1]-a[1];
	transpt[2]=pt[2]-a[2];

	/* Rotate transpt around axis r=lambda*b and translate back */
	ans[0]=	a[0]+
			(t*b[0]*b[0]+c)*transpt[0]+
			(t*b[0]*b[1]+s*b[2])*transpt[1]+
			(t*b[0]*b[2]-s*b[1])*transpt[2];
	ans[1]= a[1]+
			(t*b[0]*b[1]-s*b[2])*transpt[0]+
			(t*b[1]*b[1]+c)*transpt[1]+
			(t*b[1]*b[2]+s*b[0])*transpt[2];
	ans[2]=	a[2]+
			(t*b[0]*b[2]+s*b[1])*transpt[0]+
			(t*b[1]*b[2]-s*b[0])*transpt[1]+
			(t*b[2]*b[2]+c)*transpt[2];

	ans[2]*=-1; /* Adjust as in sgl, z-axis is negative coming out of page */
}

/*===========================================
 * Function:	nAddInvIfNotOnAxis
 *===========================================
 * if a point doesn't lie on the axis, an invisible plane is drawn towards the axis
*/
static int nAddInvIfNotOnAxis(sgl_vector x, sgl_vector a, sgl_vector b)
{
	if(!((x[0]-a[0])*b[1]==(x[1]-a[1])*b[0] && (x[1]-a[1])*b[2]==(x[2]-a[2])*b[1]))
	{ 
#if 0
		sgl_vector *newpt;
#endif

		/* add an invisible plane */
		sgl_vector v,v2;
		float l;

		l=lambda(x);
		v[0]=a[0]+l*x[0]; v[1]=a[1]+l*x[1]; v[2]=a[2]+l*x[2];
		vRotateAboutAxis(v2, a, b, PI/2, x); 
		sgl_add_plane(v,x,v2,TRUE, NULL,NULL,NULL, NULL,NULL,NULL);

		/* make sure point is drawn */
#if 0
		newpt=(sgl_vector) SGLCalloc(nopoints+1,sizeof(sgl_vector));
		if(!newpt)
		{
			return sgl_err_no_mem;
		}

		memcpy(newpt+bBegin, pt, nopoints*sizeof(sgl_vector));
		
		newpt[(bBegin) ? 0:nopoints+1:]=v;

		SGLFree(pt);
		pt=newpt;
#endif
	}

	return 1; /* everythings fine */
}

/*===========================================
 * Function:	getTopBotOfObject
 *===========================================
 *
 * Scope:		static to this module
 * Author:		Paul Hurley.
 *
 * Purpose:		Calculates the highest and lowest points relative to the axis and
 *				stores them as global variables for use in vAddPlane for texturing.
 *				Also calculates the radius of the object
 *				Also sets the global variable nCurSects for the same reason.
 *
 * Params:		nNoPoints - number of points in the polygon
 *				nNoSects - number of sections in the revolution
 *				pt - vertices of the polygon
 *				a,b - the axis of rotation.
 *				
 * Return:		-	
 *
 * Globals accessed:  fObjectHeight, fObjectBottom, nCurSects
 *========================================================================================*/
static void getTopBotOfObject(int nNoPoints, int nNoSects, float pt[][3], sgl_vector a, sgl_vector b)
{
	int i;
	float l, fObjectTop, fLen;

	sgl_vector v;

	fVecPerpToAxis(v,pt[0],a,b);
	fObjectRadius=SqrVecLength(v); 
	
	fObjectTop=fObjectBottom=lambda(pt[0]); 

	for(i=1;i<nNoPoints;i++)
	{
		if((l=lambda(pt[i]))>fObjectTop)
		{
			fObjectTop=l;
		}
		else if(l<fObjectBottom)
		{
			fObjectBottom=l;
		}

		/*////
		// if the distance from the axis to this point is bigger than the current radius,
		// then the object radius becomes bigger 
		/// */		
		fVecPerpToAxis(v,pt[i],a,b);
		fLen=SqrVecLength(v);
		if(fLen>fObjectRadius)
		{
			fObjectRadius=fLen; 
		}
	}

	fObjectHeight=fObjectTop-fObjectBottom;
	nCurSects=nNoSects;

	fObjectRadius=(float) sqrt(fObjectRadius);
}

/*===========================================
 * Function:	vAddPlane
 *===========================================
 *
 * Scope:		static to this module
 * Author:		Paul Hurley
 * Purpose:		Calculates the vector perpendicular to the axis a+lambda*b, which
 *		goes through the point v. The answer is given by the formula, ans = (v-a) - (b.(v-a))b.
 *		If v is on the axis then a vector along the axis is supplied, the direction being 
 * 		positive (negative) if it is above (below) the vector a on the axis.
 *  
 *
 * Params:		v1,v2,v3 - the 3 vectors from which to compose the plane.
 *				a,b - the axis. 
 *				bSmoothShade- TRUE implies smooth shading. FALSE then flat shading.
 *  		equals 2 then spherical smooth shading. Else cylindrical smooth shading.
 *				bInside - TRUE implies the plane is on the inside of the object.
 *				bTexture - TRUE implies supply calculated UV's.
 *				p1,p2,p3 - used in texturing. These integers represent which rotation segment 
 *			v1,v2 and v3 respectively lie.
 *				
 * Return:		-	
 *
 * Globals accessed:  fObjectHeight, fObjectBottom, nCurSects
 *========================================================================================*/
static void vAddPlane(sgl_vector v1, sgl_vector v2, sgl_vector v3, sgl_vector a, sgl_vector b, 
int bSmoothShade, sgl_bool bInside, int bTexture, int p1, int p2, int p3)
{
	sgl_vector rep_pt;
	
	rep_pt[0]=(v1[0]+v2[0]+v3[0])/3;
	rep_pt[1]=(v1[1]+v2[1]+v3[1])/3;
	rep_pt[2]=(v1[2]+v2[2]+v3[2])/3; 

   	if(bSmoothShade)
	{
		sgl_vector normal[3];
		float lambda[3];
		
		if(bSmoothShade==2) /* Spherical smooth shading */
		{	
			/* normals are difference from centre */
			VecSub(rep_pt,a,normal[0]);
		 	VecSub(v2,a,normal[1]);
			VecSub(v3,a,normal[2]);

			/* calculate 'lamda's' for later use */
			lambda[0]=lambda(rep_pt);
			lambda[1]=lambda(v2);
			lambda[2]=lambda(v3);
		}	
		else 	/* cylindrical smooth shading */
		{	
			/* normals are from axis to point */
			lambda[0]=fVecPerpToAxis(normal[0],rep_pt,a,b);
			lambda[1]=fVecPerpToAxis(normal[1],v2,a,b);
			lambda[2]=fVecPerpToAxis(normal[2],v3,a,b);
		}

		if( bInside ) /* plane is inside the object, so normals should be made point towards axis */
		{
			/* reversing direction of normals */
			VecNegate(normal[0]);
			VecNegate(normal[1]);
			VecNegate(normal[2]);
		}

		if( bTexture ) /* smooth shading plus texturing */
		{
			sgl_2d_vec uv[3];

			/* u is the angle travered/2PI */
			uv[0][0]=((p1+p2+p3)/3.0f)/(float)nCurSects;
			/* v is the distance from bottom of object to point along axis */
			uv[0][1]=(lambda[0]-fObjectBottom)/fObjectHeight;

			uv[1][0]=p2/(float)nCurSects;
			uv[1][1]=(lambda[1]-fObjectBottom)/fObjectHeight;

			uv[2][0]=p3/(float)nCurSects;
			uv[2][1]=(lambda[2]-fObjectBottom)/fObjectHeight;

			sgl_add_plane(rep_pt,v2,v3, FALSE, normal[0],normal[1],normal[2],
			  uv[0],uv[1],uv[2]);
		}
		else /* smooth shading, no texturing */
		{
			sgl_add_plane(rep_pt,v2,v3, FALSE, normal[0],normal[1],normal[2],
			  NULL,NULL,NULL);
		}
	}
	else if( bTexture )
	{
		sgl_2d_vec uv[3];

		if(bInside==2) 
		{
			
			sgl_vector d,e2,e3;

			/*////
			// the plane is perpendicular to the axis of rotation.
			// The UV is adjust st. centre=(1/2,1/2) and u is relative horizontal and v relative 
			// vertical.
			// Currently this is implemented only for y-axis, x-axis or z-axis.
			// calculate 2 orthonormal vectors e2 and e3 to b - only implementing for x/y or z axes.
			// i.e. y axis => (x+z axes) z axis => (x+y axes) etc.
			// u is the value e2 coordinate (=v.e2) and v the value of the e3 coordinate (=v.e3)
			/// */

			e2[0]=b[1]; 
			e2[1]=b[2];
			e2[2]=b[0];

			e3[0]=b[2];
			e3[1]=b[0];
			e3[2]=b[1];

			fVecPerpToAxis(d,rep_pt,a,b); 
			uv[0][0]=(DotProd(d,e2)/fObjectRadius+1)/2.0f; 
			uv[0][1]=(DotProd(d,e3)/fObjectRadius+1)/2.0f;
			fVecPerpToAxis(d,v2,a,b);
			uv[1][0]=(DotProd(d,e2)/fObjectRadius+1)/2.0f; 
			uv[1][1]=(DotProd(d,e3)/fObjectRadius+1)/2.0f;
			fVecPerpToAxis(d,v3,a,b);
			uv[2][0]=(DotProd(d,e2)/fObjectRadius+1)/2.0f; 
			uv[2][1]=(DotProd(d,e3)/fObjectRadius+1)/2.0f;
		}
		else /* u represents angle and v represents distance from bottom of object */
		{
			uv[0][0]=((p1+p2+p3)/3.0f)/(float)nCurSects;
			uv[0][1]=(lambda(rep_pt)-fObjectBottom)/fObjectHeight;

			uv[1][0]=p2/(float)nCurSects;
			uv[1][1]=(lambda(v2)-fObjectBottom)/fObjectHeight;

			uv[2][0]=p3/(float)nCurSects;
			uv[2][1]=(lambda(v3)-fObjectBottom)/fObjectHeight;
		}
		sgl_add_plane(rep_pt,v2,v3, FALSE, NULL,NULL,NULL, uv[0],uv[1],uv[2]);
	}
	else /* flat plane, no texturing */
	{
		sgl_add_plane(rep_pt,v2,v3, FALSE, NULL,NULL,NULL, NULL,NULL,NULL);
	}
}

/*===========================================
 * Function:	nClosedRevolve
 *===========================================
 *
 * Scope:		static to this module
 * Author:		Paul Hurley.
 * Purpose:		Revolves a polygon around an axis, creating one convex object,
 *				so the polygon when revolved is assumed not to introduce any
 *				concavities such as a "hole" in the object.
 *
 * Params:	 	nNoPoints - number of points in the polygon
 * 			   	pt - 3-d polygon data, clockwise oriented, assumed to be planar.
 *			    a,b - the axis of rotation. b is assumed to be normalised.
 *				nNoSects - number of segments for the 360 degree rotation
 *				bSmoothShade - TRUE then smooth shading normals are calculated.
 *				bTexture - TRUE then UV coordinates are calculated.
 *				
 * Return:		-ve error.	
 *
 * Globals accessed:  None
 *========================================================================================*/
static int nClosedRevolve(int nNoPoints, float pt[][3], sgl_vector a, sgl_vector b,
  int nNoSects, sgl_bool bSmoothShade, sgl_bool bTexture)
{
	/*////
	// If axis is null, then the edge of 1st to last points in the polygon
	// is the axis. If the axis is specified but two points don't lie on
	// the axis then they are extended (invisibly) to lie on the axis. 
	/// */

	int i,j;
	int nErr = 0;
	sgl_bool bAxisCalculated=0; /* flag if memory was allocated for axis */
	sgl_vector *lat; /* storage for rotation points */
	float rotateby=TWOPI/nNoSects; 
	
	/*////
	// NOTE: this only works for the case of x-axis/y-axis - this check is done 
	// because clockwise around the x-axis means going into z +ve while for y-axis it is z -ve.  
	// Without this the normals end up pointing the wrong direction.
	/// */
	if(b[1]==0.0f && b[2]==0.0f)
	{
		/* if rotating around the x-axis go anti-clockwise */
		 rotateby*=-1;
	}

	if(a==0)
	{
		/* no axis supplied , so assume the axis is from 1st and last data points. */
	   	bAxisCalculated=TRUE; /* need to deallocate when finished */
		a=(float *) SGLMalloc(sizeof(sgl_vector));
		b=(float *) SGLMalloc(sizeof(sgl_vector));
		if(a==NULL || b==NULL)
		{
			return sgl_err_no_mem;
		}

		/* let a be the centre of the object */
		a[0]=(pt[nNoPoints-1][0]+pt[0][0])/2.0f; 
		a[1]=(pt[nNoPoints-1][1]+pt[0][1])/2.0f; 
		a[2]=(pt[nNoPoints-1][2]+pt[0][2])/2.0f; 
		VecSub(pt[0], pt[nNoPoints-1], b);
	}
	
	/* test if the end points lie on the axis and if not insert invisible edges */
	nAddInvIfNotOnAxis(pt[0],a,b);
	nAddInvIfNotOnAxis(pt[nNoPoints-1],a,b);

	if(bTexture)
	{
		getTopBotOfObject(nNoPoints,nNoSects, pt, a,b);
	}

	/* Create lattice of rotation points. */
	lat= SGLCalloc((nNoPoints-2)*(nNoSects-1),sizeof(sgl_vector));
	if(!lat)
	{
		return sgl_err_no_mem;
	}
	for(i=1;i<nNoPoints-1;i++)
	{
		for(j=1;j<nNoSects;j++)
		{
			vRotateAboutAxis(lattice(i,j), a, b, rotateby*j, pt[i]);
		}
	}

	/* deal with 1st edge */
	if( EdgePerpToAxis(pt[0],pt[1]) )
	{
		/* add one flat plane */
		vAddPlane(pt[0],pt[1],lattice(1,1),a,b,FALSE,2,bTexture,0,1,1);
	}
	else
	{
		vAddPlane(pt[0],pt[1],lattice(1,1),a,b,bSmoothShade,0,bTexture,0,0,1);
		vAddPlane(pt[0],lattice(1,nNoSects-1),pt[1],a,b,bSmoothShade,0,bTexture,0,nNoSects-1,0);
		for(j=1;j<nNoSects-1;j++)
		{
			vAddPlane(pt[0],lattice(1,j),lattice(1,j+1),a,b,bSmoothShade,0,bTexture,0,j,j+1);
		}
	}

	/* deal with intermediate edges */
	for(i=2;i<nNoPoints-1;i++)
	{
		if( EdgePerpToAxis(pt[i],pt[i-1]) )
		{
			/* draw a flat plane - note this implies a concavity */
			vAddPlane(pt[i],lattice(i-1,1),pt[i-1],a,b,FALSE,2,bTexture,0,1,0);
		}
		else
		{
			vAddPlane(pt[i],lattice(i-1,1),pt[i-1],a,b,bSmoothShade,0,bTexture,0,1,0);
			vAddPlane(pt[i],pt[i-1],lattice(i-1,nNoSects-1),a,b,bSmoothShade,0,bTexture,0,0,
			   nNoSects-1);
			for(j=1;j<nNoSects-1;j++)
			{
				vAddPlane(lattice(i,j),lattice(i-1,j+1),lattice(i-1,j),a,b,bSmoothShade,0,
				bTexture,j,j+1,j);
			}
		}
	}

	/* deal with last edge */
	if( EdgePerpToAxis(pt[i],pt[i-1]) )
	{
		/* draw a flat plane */
		vAddPlane(pt[nNoPoints-1],lattice(nNoPoints-2,1),pt[nNoPoints-2],a,b,FALSE,2,bTexture,
		0,1,0);
	}
	else
	{
		vAddPlane(pt[nNoPoints-1],lattice(nNoPoints-2,1),pt[nNoPoints-2],a,b,bSmoothShade,0, bTexture, 0,1,0);
		vAddPlane(pt[nNoPoints-1],pt[nNoPoints-2],lattice(nNoPoints-2,nNoSects-1),a,b,
		  bSmoothShade,0,bTexture,0,0,nNoSects-1);
		for(j=1;j<nNoSects-1;j++)
		{
			vAddPlane(pt[nNoPoints-1],lattice(nNoPoints-2,j+1), lattice(nNoPoints-2,j),a,b,
			bSmoothShade,0,bTexture,0,j+1,j);
		}
	}
		   
	if(bAxisCalculated) /* axis of rotation was calculated */
	{
		SGLFree(a);
		SGLFree(b);
	}

	SGLFree(lat);	/* return lattice mem to heap */

	return nErr;
}

/*===========================================
 * Function:	nOpenRevolve
 *===========================================
 *
 * Scope:		static to this module
 * Author:		Paul Hurley.
 *
 * Purpose:		Rotates a polygon around an axis, creating a convex object with
 *				closing planes for each section revolved. This allows for a hole
 *				to occur between the object and the axis.
 *
 * Params:		nNoPoints- no of points in the polygon
 *				pt - 3-D polygon - must be COPLANAR
 *				a,b - the axis of rotation - b must be a unit vector.
 * 				nNoSects - number of sections to create.
 * 				bSmoothShade - TRUE implies smooth shade
 *				bTexture - TRUE implies generate UV texture coordinates
 *				
 * Return:		-ve error.	
 *
 * Globals accessed:  -
 *========================================================================================*/
static int nOpenRevolve( int		nNoPoints,
						 float		pt[][3],
						 sgl_vector a,
						 sgl_vector b,
						 int		nNoSects,
						 sgl_bool	bSmoothShade,
						 sgl_bool	bTexture )
{
	sgl_vector v, *lat;
	sgl_bool *pbInside;

	int i, j, nSetPrevInside, nErr=1;
	float dp;

	float rotateby=TWOPI/nNoSects;

	/*
	// this only works for the case of x-axis/y-axis - see nClosedRevolve for explanation of why.
	*/
	if(b[1]==0.0f && b[2]==0.0f)
	{
		/* if rotating around the x-axis go anti-clockwise */
		 rotateby*=-1;
	}


	/*////
	// determine inside or outside - currently only works if polygon is on z=constant axis 
	// which is all sgl is supporting since sgl_revolve_polygon only takes 2-D polygons (=> z=0).
	// Also calculate the object's radius.
	/// */
	pbInside= (sgl_bool *) SGLCalloc(nNoPoints,sizeof(int));
	if(!pbInside)
	{
		return sgl_err_no_mem;
	}

	nSetPrevInside=FALSE;
	for(i=0;i<nNoPoints;i++) /* for each edge */
	{	
		/*
		// determining whether edge is inside or out.
		// This is done by seeing whether the normal coming
		// out of the edge is going towards the axis or away
		// from it.
		*/
		VecSub(pt[(i+1)%nNoPoints],pt[i],v);
		dp=b[1]*(-v[1])+b[0]*v[0];

		if(dp<0)
		{
			pbInside[i]=1; /* edge inside */
		}
		else if(dp>0)
		{
			pbInside[i]=0; /* edge outside */
		}

		if(nSetPrevInside)
		{	/* previous edge is dependent on orientation of this edge */
			nSetPrevInside=FALSE;
			pbInside[i-1]=pbInside[i]; 
		}

		if(dp==0)
		{
			/* inside/outside is determined by what the next edge is */
			nSetPrevInside=TRUE;
		}
	}
	fObjectRadius=(float) sqrt(fObjectRadius); /* want actual radius not radius squared */

	if(nSetPrevInside) /* determine last points orientation by 1st points */
	{
		pbInside[nNoPoints-1]=pbInside[0];
	}
	
	if(bTexture) /* need to calculate top of object for texturing later */
	{
		getTopBotOfObject(nNoPoints,nNoSects,pt, a,b);
	}

	
	/*//// 
	// Create lattice of rotation points.
	// Doing this avoids repeditive rotation calculations, and makes it easier 
	// to program. It is not the most efficient way (not all points are needed and
	// only certain points are needed at certain sections) and it requires extra memory. 
	// However, it'll do. If memory is a problem, then this algorithm can easily be modified 
	// to do these rotations on the fly. Also, only current plus previous lattice line is used - 
	// need only keep those at any one time. 
	// TODO: use symmetry to get lattice points rather than rotation if possible - i.e.
	// if the number of sections is divisible by a power of 2 
	/// */
	lat= (sgl_vector *) SGLCalloc(nNoPoints*(nNoSects-1),sizeof(sgl_vector));
	if(!lat)
	{
		SGLFree(pbInside);
		return sgl_err_no_mem;
	}	

	for(i=0;i<nNoPoints;i++)
	{
		if(EdgePerpToAxis(pt[i],pt[(i+1)%nNoPoints]))
		{
			pbInside[i]=2; /* avoid smooth shading on planes perp to axis */
		}

		for(j=1;j<nNoSects;j++)
		{
			vRotateAboutAxis(latticeOut(i,j), a, b, rotateby*j, pt[i]);
		}
	}
	
	/*
	// add 1st convex section 
	*/
	nErr=sgl_create_convex(0);
	if (nErr < 0)
	{
		DPF ((DBG_ERROR, "sgl_revolve_polygon: error creating convex"));
		SGLFree(pbInside);
		SGLFree(lat);
		return nErr;
	}

	for(i=0;i<nNoPoints;i++)
	{
		vAddPlane(pt[(i+1)%nNoPoints],latticeOut(i,1),pt[i],a,b,
		  (pbInside[i]==2) ? FALSE:bSmoothShade,pbInside[i],bTexture,0,1,0); /* inter edge */
	}

	/* closing off planes of 1st convex section (left + rhs) */
	vAddPlane(pt[0],pt[2],pt[1],a,b,FALSE,FALSE,FALSE,0,0,0);
	vAddPlane(latticeOut(0,1),latticeOut(1,1),latticeOut(2,1),a,b,FALSE,FALSE,FALSE,0,0,0); 

	/*
	// add last convex section 
	*/
	nErr=sgl_create_convex(0);
	if (nErr < 0)
	{
		DPF ((DBG_ERROR, "sgl_revolve_polygon: error creating convex"));
		SGLFree(pbInside);
		SGLFree(lat);
		return nErr;
	}

	for(i=0;i<nNoPoints;i++)
	{
		vAddPlane(latticeOut((i+1)%nNoPoints,nNoSects-1),pt[i],latticeOut(i,nNoSects-1),a,b,
		  (pbInside[i]==2) ? FALSE:bSmoothShade,pbInside[i],bTexture,nNoSects-1,0,nNoSects-1); 
	}

	/* closing planes of last convex section (left + rhs) */
	vAddPlane(latticeOut(0,nNoSects-1),latticeOut(2,nNoSects-1),latticeOut(1,nNoSects-1),a,b,
	  FALSE,FALSE,FALSE,0,0,0); 
	vAddPlane(pt[0],pt[1],pt[2],a,b,FALSE,FALSE,FALSE,0,0,0);

	/*
	// add intermediate convex sections 
	*/
	for(j=1;j<nNoSects-1;j++)
	{	
		nErr=sgl_create_convex(0);
		if (nErr < 0)
		{
			DPF ((DBG_ERROR, "sgl_revolve_polygon: error creating convex"));
			SGLFree(pbInside);
			SGLFree(lat);
			return nErr;
		}

		/* NOTE: currently have 0/2PI -> (0 or 1 -which?) texture wrap problem - not fixing! */
		for(i=0;i<nNoPoints;i++)
		{							  
			vAddPlane(latticeOut((i+1)%nNoPoints,j),latticeOut(i,j+1),latticeOut(i,j),a,b,
			  (pbInside[i]==2) ? FALSE:bSmoothShade,pbInside[i],bTexture,j,j+1,j); /* inter edge */
		}

		/* closing flat planes (left and rhs) */
		vAddPlane(latticeOut(0,j),latticeOut(2,j),latticeOut(1,j),a,b,FALSE,FALSE,FALSE,0,0,0);
		vAddPlane(latticeOut(0,j+1),latticeOut(1,j+1),latticeOut(2,j+1),a,b,FALSE,FALSE,FALSE,0,0,0); 
	}
		   
	SGLFree(lat);	/* return lattice mem to heap */
	SGLFree(pbInside); /* return inside information to heap */
				 
	return nErr;
}

/*===========================================
 * Function:	sgl_revolve_polygon
 *===========================================
 *
 * Author:		Paul Hurley.
 * Scope:		SGL
 *
 * Purpose:		For a given polygon, it is rotated a set number of times
 *			around the axis to produce a convex or series of
 * 			convex object(s). Assuming the points are COPLANAR and the
 * 			polygon must be CONVEX. 
 *
 * Params:		nNoPoints - the number of points in the object
 *				pt	- 2-D array of polygon data. Note the polygon
 *			must be supplied clockwise.
 *				nNoSects  - how many sectors are wanted in the rotation
 *				bSmoothShade - if TRUE then smooth shading normals
 *			are calculated. (If =2 then the object is treated as 
 *			a sphere).
 *				bTexture - if TRUE then UV coords are calculated.
 *				bClosed - if TRUE then the object is assumed to be
 *			close around the axis, else open.
 *				nYAxis - if TRUE then the y-axis is choosen as the axis,
 *			otherwise it defaults to the x-axis.
 *
 * Globals accessed:   None
 *========================================================================================*/
int CALL_CONV sgl_revolve_polygon( int nNoPoints, float pt[][2],
								   int nNoSects, sgl_bool bClosed,
								   sgl_bool bYAxis, int bSmoothShade,
								   sgl_bool bTexture )
{	
	sgl_vector a={0.0f,0.0f,0.0f},b={1.0f,0.0f,0.0f}; /* default is x-axis */
	float (*newpt)[3]; /* array to store 3-d version of polygon */
	int nErr;
	int i;	/* counter */

	if(nNoPoints<2)
	{
		DPFDEV ((DBG_ERROR, "sgl_revolve_polygon: bad nNoPoints parameter %n", nNoPoints));
		nErr = sgl_err_bad_parameter;
		SglError (nErr);
		return nErr;
	}
	else if(nNoSects<3)
	{
	 	DPFDEV ((DBG_ERROR, "sgl_revolve_polygon: bad nNoSects parameter %n", nNoSects));
		nErr = sgl_err_bad_parameter;
		SglError (nErr);
		return nErr;
	}

	newpt=SGLCalloc(nNoPoints, sizeof(sgl_vector));
	if(!newpt)
	{
		return sgl_err_no_mem;
	}
		
	/* add a z=0 coordinate to each point in the polygon */	
	for(i=0;i<nNoPoints;i++)
	{
		newpt[i][0]=pt[i][0];
		newpt[i][1]=pt[i][1];
		newpt[i][2]=0.0f;
	}

	if(bYAxis)
	{
		/* change to y-axis */
		b[0]=0.0f;
		b[1]=1.0f;
	}

	/* call the correct procedure */
	if(bClosed)
	{
		nErr=nClosedRevolve(nNoPoints, newpt, a, b, nNoSects, bSmoothShade, bTexture);
	}
	else
	{
		nErr=nOpenRevolve(nNoPoints, newpt, a, b, nNoSects, bSmoothShade, bTexture);
	}

	SGLFree(newpt);  /* don't need 3-D polygon anymore */

	return nErr;
}

/* end of $RCSfile: dlobject.c,v $ */

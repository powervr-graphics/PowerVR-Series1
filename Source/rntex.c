/*****************************************************************************
 * Name         : rntex.c
 * Title        : Performs all the texture parameter calculation.
 * Author       : Peter Leaback
 * Created      : 17/07/1995
 *
 * Copyright	: 1995-2022 Imagination Technologies (c)
 * License		: MIT
 *
 * Description  :
 * 
 * Platform     : ANSI
 *
 * Modifications:
 * $Log: rntex.c,v $
 * Revision 1.65  1997/06/04  11:14:28  gdc
 * removed some compiler warnings
 *
 * Revision 1.64  1997/05/02  07:42:44  erf
 * Removed UINT32's etc and replaced with sgl_uint32's etc.
 *
 * Revision 1.63  1997/04/23  09:39:44  mjg
 * Removed compiler warnings.
 *
 * Revision 1.62  1997/04/09  16:43:13  gdc
 * removed reference to global project_mat struct
 *
 * Revision 1.61  1997/04/08  14:40:12  gdc
 * local proj mat is now accessed thru functions not as a global
 *
 * Revision 1.60  1997/04/07  16:21:35  gdc
 * made WrappingTransform for texture wrapping a local variable whose
 * value is obtained via a function call
 *
 * Revision 1.59  1997/04/03  08:33:26  rgi
 * Added ZEUS_ARCADE support
 *
 * Revision 1.58  1997/01/20  13:18:31  erf
 * Fixed bilinear bug where the top bits were not set to zero.
 *
 * Revision 1.57  1996/12/09  13:40:12  gdc
 * removed all references to Triangle API -assume D3DOPTIM stuff has
 * now taken over
 *
 * Revision 1.56  1996/11/18  18:32:22  jop
 * Removed PowerSGL direct stuff
 *
 * Revision 1.55  1996/11/09  04:23:06  jrc
 * Hack for reusing texturing and shading on successive faces (currently
 * disabled).
 *
 * Revision 1.54  1996/10/16  21:16:58  jop
 * Added new pack routines for new lite stuff
 *
 * Revision 1.53  1996/10/11  12:09:02  msh
 * "Small number" checks modified for Midas Arcade
 *
 * Revision 1.52  1996/10/10  11:10:15  mal
 * Modified some of the metrics macros.
 *
 * Revision 1.51  1996/10/09  15:36:05  mal
 * Added some new timers to the code.
 *
 * Revision 1.50  1996/10/03  12:33:37  mal
 * Added some metric macros for code profiling.
 *
 * Revision 1.49  1996/09/23  17:40:50  jrc
 * Added mipmap offset value for SGL Lite.
 *
 * Revision 1.48  1996/09/18  16:00:44  pdl
 * added a new mip-map calculation that does not need
 * a rep point. (ohh err mrs!)
 *
 * Revision 1.47  1996/09/17  09:55:08  jrc
 * Clipped rep pt for mipmap case of DoTextureMappingTri.
 *
 * Revision 1.46  1996/09/05  17:02:25  jop
 * Went back to adjoint calc method (sparklies fix)
 *
 * Revision 1.45  1996/09/04  00:01:40  jop
 * Moved over to using inverse matrix in triangle code
 *
 * Revision 1.44  1996/08/30  11:46:44  jop
 * Removed a non-existent function
 *
 * Revision 1.43  1996/08/29  13:12:30  sjf
 * Fixed x%$*'ing C++ comment, which isn't ansi C.
 *
 * Revision 1.42  1996/08/29  11:15:23  jop
 * Added WRAPU and WRAPV D3D functionality
 *
 * Revision 1.41  1996/08/12  11:17:14  pdl
 * reverted to the old origin moving in the accurate
 * texturing routine.
 *
 * Revision 1.40  1996/08/07  13:28:59  pdl
 * added the more accurate rescaling to the tringle code.
 *
 * Revision 1.39  1996/08/07  10:58:55  pdl
 * made the fast texture mapping use the accurate rescaling.
 *
 * Revision 1.38  1996/07/31  21:41:29  jop
 * Cut out unnecessary compares in triangle code
 *
 * Revision 1.37  1996/07/26  13:34:52  mal
 * Added Performance Improvements to the Non MipMapped Case
 * in Function DotextureMapping Fast.
 *
 * Revision 1.36  1996/07/10  14:35:25  pdl
 * added the accurate/fast texturing.
 *
 * Revision 1.35  1996/07/10  13:37:25  jop
 * Modified tri texture code to to invw multiply if necessary
 *
 * Revision 1.34  1996/07/06  01:44:12  sjf
 * Added the test for negative exponents to the "full on" versions
 * that are in the accurate version. We were getting  bad UV data
 * that caused negative exponents. The negative value's upper bits
 * then overflowed into the hardware control word causing chaos and
 * a lock up (in MechWarrior). Hopefully this extra test won't slow
 * the software significantly (compared to the other rubbish I've added
 * today).
 *
 * Revision 1.32  1996/06/27  11:45:07  jrc
 * Added warning parameter to call of MapExternalToInternal.
 *
 * Revision 1.31  1996/05/23  16:13:19  jop
 * 'fixed' bordeaux bug (compiler related).
 *
 * Revision 1.30  1996/05/23  16:04:53  pdl
 * added a FULL_ON version of the triangle stuff
 *
 * Revision 1.29  1996/05/21  15:38:58  jop
 * Made triangle vertex ptrs indirect
 *
 * Revision 1.28  1996/05/15  09:54:17  pdl
 * changed the origin moving in the NON FULL_ON texturing.
 *
 * Revision 1.27  1996/05/10  14:46:20  jop
 * Unified triangle support
 *
 * Revision 1.26  1996/05/09  17:00:31  pdl
 * added some triangle support.
 *
 * Revision 1.25  1996/04/23  23:07:56  sjf
 * Split main tex map for loop into 2 -one that does MIP maps and the
 * other not. Removes one measly branch from the loop. (but every bit
 * helps).
 *
 * Revision 1.24  1996/04/23  19:00:20  sjf
 * Moved some Pentium macros to sglmacro.h
 *
 * Revision 1.23  1996/04/23  16:52:46  sjf
 * More fun doing some fiddling with ints to manipulate floats as part
 * of the optimisation for pentium.
 *
 * Revision 1.22  1996/04/19  16:16:25  sjf
 * Some optimisation - probably shaved about 7% off for pentium -
 * less for other CPUs.
 *
 * Revision 1.21  1996/03/27  15:33:48  pdl
 * added a few consts
 * inproved the speed of non-mip mapped FULL_ON
 *
 * Revision 1.20  1996/03/26  12:43:07  pdl
 * fiddled with the sfabs's.
 *
 * Revision 1.19  1996/03/12  11:59:17  sm
 * fixed bug in testing if smooth shaded or not
 *
 * Revision 1.18  1996/01/31  16:33:27  pdl
 * convex objects can be wrapped.
 *
 * Revision 1.17  1996/01/31  15:02:58  jop
 * Added shading/points struct changes
 *
 * Revision 1.16  1995/12/05  14:44:16  pdl
 * added the origin moving code to the non FULL_ON version
 * of DoTextureMapping so textures do not wobble when you
 * are far away from the texture origin.
 *
 * Revision 1.15  1995/11/13  15:41:07  pdl
 * fixed a mip mapping bug.
 * completed the FULL_ON dotexturemapping with gradient calculation
 *
 * Revision 1.14  1995/11/09  17:10:08  pdl
 * reverted to my version
 * added Simon's modification
 * added a partially finished FULL_ON DoTextureMapping.
 * changed DoTextureWrapping rep_point to jim's new point.
 *
 * Revision 1.11  1995/10/04  08:37:31  sjf
 *   Added code to support texture space which is independant of the
 * texture map size.
 *
 * Revision 1.10  1995/09/28  10:27:48  pdl
 * added the wrap space stuff.
 * added the spherial wrapping.
 *
 * Revision 1.9  1995/09/08  15:36:39  sjf
 * Didi some very minor optimisations (eg where getting max of a set of
 * values) which propable save only a few cycles per plane!
 *
 * Revision 1.8  1995/09/01  16:50:44  pdl
 * made DoTextureMapping a little safer.
 * chanhed the wrapping to use the smooth shading normals.
 *
 * Revision 1.7  1995/08/23  14:59:15  pdl
 * added part of the spherical mapping.
 *
 * Revision 1.6  1995/08/18  11:00:52  pdl
 * Added the texture wrapping functions.
 *
 * Revision 1.5  1995/08/07  11:09:55  pdl
 * added the non-mip mapped case.
 *
 * Revision 1.4  1995/08/03  17:25:12  pdl
 * Make coersion of negative floats to sgl_uint32 work.
 *
 * Revision 1.3  1995/08/02  11:56:03  pdl
 * added sfabs()
 *
 * Revision 1.2  1995/07/26  16:13:29  pdl
 * The compression was incorrectly calculated.
 *
 * Revision 1.1  1995/07/21  15:19:34  pdl
 * Initial revision
 *
 *
 *****************************************************************************/

#include "sgl.h"

#include "sgl_defs.h"
#include "metrics.h"
#include "dlnodes.h"
#include "rnconvst.h"
#include "rnstate.h"
#include "rnglobal.h"
#include "sgl_math.h"

#include "metrics.h"

SGL_EXTERN_TIME_REF  /* if we are timing code !!! */ 

/* this is used for storing the data from the texture wrapping, and is
   input to the texture mapper*/

CONV_TEXTURE_UNION TempTexResults[SGL_MAX_PLANES];


/*
** flong allows manipulation of floats as integers
*/

typedef union {
	float f;
	unsigned long l;
} flong;

/* this is the number of times the texture repeats before we translate
   it back to the origin */

#define	RESCALE_MAGIC_NO	16.0f   


/*
**	CFRreciprocal is used by DoTextureMapping to 
**	rescale 'c','f', and 'r' to keep the coefficients
**  in a similar range to a,b,d,e,p and q.
**  Texas will scale them back to the right values.
*/

/******************************************************************************
 * Function Name: DoTextureMappingFast
 *
 * Inputs       : NumberOfPlanes,
 *				  Planes,an array of pointers to the planes.
 *				  ObjToEye,transformation matrix
 *				  MState,the material state.
 * Outputs      : Results,an array of coefficients in TEXAS format.
 * Returns      : -
 * Globals Used : projection_mat.CFRreciprocal,the scale for 'C','F', and 'R'
 *
 * Description  : This transforms some texture axes into screen space,
 *				  calculates the inverse, and packs the coefficients
 *				  into TEXAS format. It also packs the TexAddress which includes
 *				  some of the texture flags.
 * Comments		: 
 *				  THIS IS THE 'FULL_ON' VERSION OF THIS ROUTINE. IE the fastest
 *				  way i can think of doing the routine.The slower (but more 
 *				  accurate routine is below this one). 
 *****************************************************************************/

void DoTextureMappingFast(int							   NumberOfPlanes,
						  const TRANSFORMED_PLANE_STRUCT   ** PlanesArray,
						  const TRANSFORM_STRUCT		   *ObjToEye,
						  const MATERIAL_STATE_STRUCT	   *MState,
						  TEXTURING_RESULT_STRUCT  		   *pResults)
{

	float 	largestT,largestB;
	float	compression,pqr,approx1;
	float 	dudx,dvdx,dudy,dvdy;
	float	eye_a,eye_b,eye_c,eye_d,eye_e,eye_f,eye_p,eye_q,eye_r;
	float	adj_a,adj_b,adj_c,adj_d,adj_e,adj_f,adj_p,adj_q,adj_r;

	float	x,y;
	flong 	topExponent,MipMap;
	long	exp;
	long	MipMant,MipExp;

	sgl_bool	MipMapped;

	const TRANSFORMED_PLANE_STRUCT   * pPlane;
	const CONV_TEXTURE_UNION		 * pPremapped;
	const LOCAL_PROJECTION_STRUCT  * const pLocalProjMat = RnGlobalGetLocalProjMat();
	PROJECTION_MATRIX_STRUCT  * const pProjMat = RnGlobalGetProjMat ();
	float CFRreciprocal = pProjMat->CFRreciprocal;

	#if MULTI_FP_REG
		/*
		// If we have floating point registers, make a local "copy" of
		// the projection matrix values. This should put it in registers.
		*/
		float	Sr00,Sr01,Sr02,Sr03,Sr10,Sr11,Sr12,Sr13,Sr20,Sr21,Sr22,Sr23;
	#else
		/*
		// For the case where we DONT have many FP registers,
		// define aliases for the projection matrix entries.
		*/
		#define Sr00 (pLocalProjMat->sr[0][0])
		#define Sr01 (pLocalProjMat->sr[0][1])
		#define Sr02 (pLocalProjMat->sr[0][2])
		#define Sr03 (pLocalProjMat->sr[0][3])
		#define Sr10 (pLocalProjMat->sr[1][0])
		#define Sr11 (pLocalProjMat->sr[1][1])
		#define Sr12 (pLocalProjMat->sr[1][2])
		#define Sr13 (pLocalProjMat->sr[1][3])

		#define Sr20 (ObjToEye->mat[2][0])
		#define Sr21 (ObjToEye->mat[2][1])
		#define Sr22 (ObjToEye->mat[2][2])
		#define Sr23 (ObjToEye->mat[2][3])

	#endif

	ASSERT(pLocalProjMat->valid); /*stop if the matrix is not valid*/

	SGL_TIME_START(TEXTURE_TIME)

	/*
	// load up the Object to Screen matrix (if we have fp registers)
	*/
	#if MULTI_FP_REG
		Sr00=pLocalProjMat->sr[0][0];
		Sr01=pLocalProjMat->sr[0][1];
		Sr02=pLocalProjMat->sr[0][2];
		Sr03=pLocalProjMat->sr[0][3];
		Sr10=pLocalProjMat->sr[1][0];
		Sr11=pLocalProjMat->sr[1][1];
		Sr12=pLocalProjMat->sr[1][2];
		Sr13=pLocalProjMat->sr[1][3];
		Sr20=ObjToEye->mat[2][0];
		Sr21=ObjToEye->mat[2][1];
		Sr22=ObjToEye->mat[2][2];
		Sr23=ObjToEye->mat[2][3];
	#endif


	/*
	// also define convenient access to the "scaled" texture projection
	// as well
	*/
	#define TSr00 pLocalProjMat->textureSR[0][0]
	#define TSr01 pLocalProjMat->textureSR[0][1]
	#define TSr02 pLocalProjMat->textureSR[0][2]

	#define TSr10 pLocalProjMat->textureSR[1][0]
	#define TSr11 pLocalProjMat->textureSR[1][1]
	#define TSr12 pLocalProjMat->textureSR[1][2]

	#define TSr20 pLocalProjMat->textureSR[2][0]
	#define TSr21 pLocalProjMat->textureSR[2][1]
	#define TSr22 pLocalProjMat->textureSR[2][2]

	MipMapped = MState->texas_precomp.TexAddress & MASK_MIP_MAPPED;

	/*
	// Are all these planes MIP mapped? Use either the MIP map loop
	// or the non MIP'd.
	*/
	for(/*Nil*/; NumberOfPlanes!=0; NumberOfPlanes--, pResults++, PlanesArray++)
	{
		/*
		// get pointer to this plane
		*/
		pPlane = *PlanesArray;

		/*
		// Get a pointer to the UV Info
		*/
		pPremapped = pPlane->pTextureData;

		/*
		// For typing convenience, define aliases to the various
		// bits in the premapped stuff
		*/
		#define UPoint pPremapped->pre_mapped.u_vect
		#define VPoint pPremapped->pre_mapped.v_vect
		#define OPoint pPremapped->pre_mapped.o_vect

		/*
		**	transform U,V and the origin from object space to eye space.
		*/
		eye_a=UPoint[0]*TSr00 + UPoint[1]*TSr01 + UPoint[2]*TSr02;
		eye_b=VPoint[0]*TSr00 + VPoint[1]*TSr01 + VPoint[2]*TSr02;
		eye_c=OPoint[0]*Sr00  + OPoint[1]*Sr01  + OPoint[2]*Sr02 + Sr03;

		eye_d=UPoint[0]*TSr10 + UPoint[1]*TSr11 + UPoint[2]*TSr12;
		eye_e=VPoint[0]*TSr10 + VPoint[1]*TSr11 + VPoint[2]*TSr12;
		eye_f=OPoint[0]*Sr10  + OPoint[1]*Sr11  + OPoint[2]*Sr12 + Sr13;

		eye_p=UPoint[0]*TSr20 + UPoint[1]*TSr21 + UPoint[2]*TSr22;
		eye_q=VPoint[0]*TSr20 + VPoint[1]*TSr21 + VPoint[2]*TSr22;
		eye_r=OPoint[0]*Sr20  + OPoint[1]*Sr21  + OPoint[2]*Sr22 + Sr23;

		#undef UPoint
		#undef VPoint
		#undef OPoint


		/*
		**	calculate the adjoint.
		*/
		adj_a=eye_e*eye_r - eye_q*eye_f;
		adj_b=eye_q*eye_c - eye_b*eye_r;
		adj_c=eye_b*eye_f - eye_e*eye_c; 

		adj_d=eye_p*eye_f - eye_d*eye_r;
		adj_e=eye_a*eye_r - eye_p*eye_c;
		adj_f=eye_d*eye_c - eye_a*eye_f; 

		adj_p=eye_d*eye_q - eye_p*eye_e; 
		adj_q=eye_p*eye_b - eye_a*eye_q; 
		adj_r=eye_a*eye_e - eye_d*eye_b; 

		/* check that pqr is not negative at the representative point */

		x=pPlane->projRepPoint[0];
		y=pPlane->projRepPoint[1];

		pqr=adj_p*x + adj_q*y + adj_r;

		if(MipMapped)
		{
			/*do the mip mapped calculation*/

			dudx= sfabs(adj_a*adj_r - adj_p*adj_c);
			dvdx= sfabs(adj_d*adj_r - adj_p*adj_f);

			/* 
			//approximate the pythagorean distance 
			//
			// IE dist(a,b) = |a| + |b| - 0.585786f*min(|a|,|b|)
			// I think this is supposed to be about 7% out worst case...
			*/
		#if SLOW_FCMP && !MULTI_FP_REG
			if(FLOAT_TO_LONG(dudx) < FLOAT_TO_LONG(dvdx))
		#else
			if(dudx < dvdx)
		#endif
			{
				approx1= (1.0f-0.585786f) * dudx + dvdx;
			}
			else
			{
				approx1=  dudx + (1.0f-0.585786f) * dvdx;
			}
	
			dudy= sfabs(adj_b*adj_r - adj_q*adj_c);
			dvdy= sfabs(adj_e*adj_r - adj_q*adj_f);
	
			/*
			// And again
			*/
		#if SLOW_FCMP && !MULTI_FP_REG
			if(FLOAT_TO_LONG(dudy) < FLOAT_TO_LONG(dvdy))
		#else
			if( dudy < dvdy )
		#endif
			{
				compression= (1.0f-0.585786f) * dudy + dvdy;
			}
			else
			{
				compression= dudy + (1.0f-0.585786f) * dvdy;
			}
			
			/* 
			// pick the larger of the distances and store in
			// compression. Originally there was a variable
			// called approx2, but we re-use compression instead.
			*/
			ASSERT(approx1>= 0.0f);
			ASSERT(compression>= 0.0f);

		#if SLOW_FCMP && !MULTI_FP_REG
			if (FLOAT_TO_LONG(approx1) > FLOAT_TO_LONG(compression))
		#else
			if (approx1>compression)
		#endif
			{
			  	compression=approx1;
			}

			/*
			**	rescale 'c','f', and 'r' to keep the coefficients
			**  in a similar range to a,b,d,e,p and q.
			**  Texas will scale them back to the right values.
			*/

			adj_c*=CFRreciprocal;
			adj_f*=CFRreciprocal;
			adj_r*=CFRreciprocal;

			/*
			**	find the largest (magnitude) of a,b,c,d,e,f 
			** 
			** On some slower processors floating point compare
			** etc is very slow. Since these values are all positive, we
			** can just as validly "assume" they are integer as the IEEE
			** fp format will work.
			** Note also that fabs can be replaced with a clearing of the
			** TOP bit.
			** This bit of code also assumes that there are B'all FP registers
			** and therefore most FP values are sitting out in memory (not
			** registers)
			*/

			SGL_TIME_SUSPEND(TEXTURE_TIME)
			SGL_TIME_START(TEXTURE_PARAMETER_SETUP_TIME)

		#if SLOW_FCMP && !MULTI_FP_REG
			{
				long  LargestIntEquiv;
				long  temp;

				/*
				// Get the abs of a, and "convert" it to int.
				*/
				LargestIntEquiv = FLOAT_TO_LONG(adj_a) & FABS_MASK;


				/*
				// Get "fabs" of b and compare it with that of a
				// etc etc etc
				*/
				temp = FLOAT_TO_LONG(adj_b) & FABS_MASK;
				if(temp > LargestIntEquiv)
				{
					LargestIntEquiv = temp;
				}

				temp = FLOAT_TO_LONG(adj_c) & FABS_MASK;
				if(temp > LargestIntEquiv)
				{
					LargestIntEquiv = temp;
				}

				temp = FLOAT_TO_LONG(adj_d) & FABS_MASK;
				if(temp > LargestIntEquiv)
				{
					LargestIntEquiv = temp;
				}


				temp = FLOAT_TO_LONG(adj_e) & FABS_MASK;
				if(temp > LargestIntEquiv)
				{
					LargestIntEquiv = temp;
				}

				temp = FLOAT_TO_LONG(adj_f) & FABS_MASK;
				if(temp > LargestIntEquiv)
				{
					LargestIntEquiv = temp;
				}

				largestT = LONG_TO_FLOAT(LargestIntEquiv);

				/*
				** find the largest of p,q,r 
				*/
				/*
				// Get the abs of p, and "convert" it to int.
				*/
				LargestIntEquiv = FLOAT_TO_LONG(adj_p) & FABS_MASK;

				temp = FLOAT_TO_LONG(adj_q) & FABS_MASK;
				if(temp > LargestIntEquiv)
				{
					LargestIntEquiv = temp;
				}

				temp = FLOAT_TO_LONG(adj_r) & FABS_MASK;
				if(temp > LargestIntEquiv)
				{
					LargestIntEquiv = temp;
				}

				largestB = LONG_TO_FLOAT(LargestIntEquiv);
			}
			/*
			// Use FP compares to get the largest
			*/
		#else
			largestT= sfabs(adj_a);

			/* 
			// make a temp copy of the result of the sfabs() because
			// this may be faster on compilers that call an sfabs
			// function
			*/
			FabsT1=sfabs(adj_b);
			if(FabsT1>largestT)
				largestT=FabsT1;

			FabsT2=sfabs(adj_c);
			if(FabsT2>largestT)
				largestT=FabsT2;

			FabsT1=sfabs(adj_d);
			if(FabsT1>largestT)
				largestT=FabsT1;

			FabsT2=sfabs(adj_e);
			if(FabsT2>largestT)
				largestT=FabsT2;

			FabsT1=sfabs(adj_f);
			if(FabsT1>largestT)
				largestT=FabsT1;

			/*
			** find the largest of p,q,r 
			*/
			largestB= sfabs(adj_p);

			FabsT1=sfabs(adj_q);
			if (FabsT1>largestB)
				  largestB=FabsT1;

			FabsT2=sfabs(adj_r);
			if (FabsT2>largestB)
				  largestB=FabsT2;
		#endif




			/*
			** calculate a fast floor(log2(largest/largest_bot))
			*/

			largestT=1.0f/largestT;

			topExponent.f=largestB*largestT;

			/* extract the offset exponent from the IEEE fp number */

			exp=(topExponent.l>>23) & 0xff; 	

			/* calculate the reciprocal (ignore post normalising) */

			exp=126-exp;

			/*
			** rescale the texturing coefficients to 16 bits.
			*/

			largestT=largestT * 32767.0f;

			if(pqr<0.0f)
			{
				largestT=-largestT;
			}

			adj_a=adj_a * largestT;
			adj_b=adj_b * largestT;
			adj_c=adj_c * largestT;
			adj_d=adj_d * largestT;
			adj_e=adj_e * largestT;
			adj_f=adj_f * largestT;

			/* calculate a fast pow(2.0,floor())*largestT */

			topExponent.l=(exp+127)<<23;

			largestT=largestT * topExponent.f;

			adj_p=adj_p * largestT;
			adj_q=adj_q * largestT;
			adj_r=adj_r * largestT;

			/* solve the compression for 'n'.Compression is
			   pre-multiplied by pqr*pqr, so we only have to multiply by the
			   square of the rescale value */

			MipMap.f=compression*largestT*largestT;

			/* convert from IEEE to the TEXAS floating point format*/

			MipMant =(MipMap.l>>16)&0x7f;
			MipMant+=128; /*add in the implied 0.5*/
			/*126 because of the different decimal point*/
			MipExp=(MipMap.l>>23)-126;


			/*
			** Texas can't handle an exponent greater than 15,so we will
   			** reduce the resolution of 'p', 'q', and 'r'.
			** THIS SHOULD HAPPEN **VERY** RARELY.The Level Of Detail
			** should have removed the texture by now.
			*/

			if(exp>15)
			{
				adj_p=(float)((sgl_int32)adj_p >>(exp-15));
				adj_q=(float)((sgl_int32)adj_q >>(exp-15));
				adj_r=(float)((sgl_int32)adj_r >>(exp-15));
				MipExp-=(exp-15)<<1;

				exp=15;
			}
			/*
			** Texas can't handle a negative exponent,so we will
   			** reduce the resolution of a,b,c,d,e and f.
			** This condition only happens if the texture is VERY zoomed.
			** It may not be worth the expense of testing for this.
			*/
			else if(exp<0)
			{
				adj_a=(float)((sgl_int32)adj_a >>(0-exp));
				adj_b=(float)((sgl_int32)adj_b >>(0-exp));
				adj_c=(float)((sgl_int32)adj_c >>(0-exp));
				adj_d=(float)((sgl_int32)adj_d >>(0-exp));
				adj_e=(float)((sgl_int32)adj_e >>(0-exp));
				adj_f=(float)((sgl_int32)adj_f >>(0-exp));

				exp=0;
			}




			/*
			**  now pack up the data in TEXAS format
			*/
			/*first instruction word*/

			pResults->Control1=((sgl_uint32)exp << SHIFT_EXPONENT);

			/*texture parameters*/ 

			pResults->TexCoeff1 =((sgl_uint32)MipMant << SHIFT_PMIP_M);
			pResults->TexCoeff1|=((sgl_uint32)MipExp  << SHIFT_PMIP_E);
			pResults->TexCoeff1|=((sgl_int32)adj_r) & 0xffffUL;

	        pResults->TexCoeff2 =((sgl_int32)adj_q << 16);
	        pResults->TexCoeff2|=((sgl_int32)adj_p) & 0xffffUL;

	        pResults->TexCoeff3 =(MState->texas_precomp.TexAddress) << 16;
	        pResults->TexCoeff3|=((sgl_int32)adj_c) & 0xffffUL;

	        pResults->TexCoeff4 =((sgl_int32)adj_b << 16);
	        pResults->TexCoeff4|=((sgl_int32)adj_a) & 0xffffUL;

	        pResults->TexCoeff5 =(MState->texas_precomp.TexAddress) & 0xffff0000UL;
	        pResults->TexCoeff5|=((sgl_int32)adj_f) & 0xffffUL;

	        pResults->TexCoeff6 =((sgl_int32)adj_e << 16);
	        pResults->TexCoeff6|=((sgl_int32)adj_d) & 0xffffUL;
	
			SGL_TIME_STOP(TEXTURE_PARAMETER_SETUP_TIME)
			SGL_TIME_RESUME(TEXTURE_TIME)

		}
		/*
		// Else it's not MIP mapped.
		*/
		else
		{
			/*
			//do the non-mip map calculation 
			*/

			/*
			** TEXAS does not divide by a negative number correctly,
			** so we need to negate everything
			*/

			SGL_TIME_SUSPEND(TEXTURE_TIME)
			SGL_TIME_START(TEXTURE_PARAMETER_SETUP_TIME)

			if(pqr<0.0f)
			{
				adj_a=-adj_a;
				adj_b=-adj_b;
				adj_c=-adj_c;
				adj_d=-adj_d;
				adj_e=-adj_e;
				adj_f=-adj_f;
				adj_p=-adj_p;
				adj_q=-adj_q;
				adj_r=-adj_r;
			}
	
			/*
			**	rescale 'c','f', and 'r' to keep the coefficients
			**  in a similar range to a,b,d,e,p and q.
			**  Texas will scale them back to the right values.
			*/

			adj_c*=CFRreciprocal;
			adj_f*=CFRreciprocal;
			adj_r*=CFRreciprocal;

			/*
			**	find the largest (magnitude) of a,b,c,d,e,f 
			** 
			** On some slower processors floating point compare
			** etc is very slow. Since these values are all positive, we
			** can just as validly "assume" they are integer as the IEEE
			** fp format will work.
			** Note also that fabs can be replaced with a clearing of the
			** TOP bit.
			** This bit of code also assumes that there are B'all FP registers
			** and therefore most FP values are sitting out in memory (not
			** registers)
			*/


		#if SLOW_FCMP && !MULTI_FP_REG
			{
				long  LargestIntEquiv;
				long  temp;

				/*
				// Get the abs of a, and "convert" it to int.
				*/
				LargestIntEquiv = FLOAT_TO_LONG(adj_a) & FABS_MASK;


				/*
				// Get "fabs" of b and compare it with that of a
				// etc etc etc
				*/
				temp = FLOAT_TO_LONG(adj_b) & FABS_MASK;
				if(temp > LargestIntEquiv)
				{
					LargestIntEquiv = temp;
				}

				temp = FLOAT_TO_LONG(adj_c) & FABS_MASK;
				if(temp > LargestIntEquiv)
				{
					LargestIntEquiv = temp;
				}

				temp = FLOAT_TO_LONG(adj_d) & FABS_MASK;
				if(temp > LargestIntEquiv)
				{
					LargestIntEquiv = temp;
				}


				temp = FLOAT_TO_LONG(adj_e) & FABS_MASK;
				if(temp > LargestIntEquiv)
				{
					LargestIntEquiv = temp;
				}

				temp = FLOAT_TO_LONG(adj_f) & FABS_MASK;
				if(temp > LargestIntEquiv)
				{
					LargestIntEquiv = temp;
				}

				largestT = LONG_TO_FLOAT(LargestIntEquiv);


				/*
				** find the largest of p,q,r 
				*/
				/*
				// Get the abs of p, and "convert" it to int.
				*/
				LargestIntEquiv = FLOAT_TO_LONG(adj_p) & FABS_MASK;

				temp = FLOAT_TO_LONG(adj_q) & FABS_MASK;
				if(temp > LargestIntEquiv)
				{
					LargestIntEquiv = temp;
				}

				temp = FLOAT_TO_LONG(adj_r) & FABS_MASK;
				if(temp > LargestIntEquiv)
				{
					LargestIntEquiv = temp;
				}

				largestB = LONG_TO_FLOAT(LargestIntEquiv);
			}
			/*
			// Use FP compares to get the largest
			*/
		#else
			largestT= sfabs(adj_a);

			/* 
			// make a temp copy of the result of the sfabs() because
			// this may be faster on compilers that call an sfabs
			// function
			*/
			FabsT1=sfabs(adj_b);
			if(FabsT1>largestT)
				largestT=FabsT1;

			FabsT2=sfabs(adj_c);
			if(FabsT2>largestT)
				largestT=FabsT2;

			FabsT1=sfabs(adj_d);
			if(FabsT1>largestT)
				largestT=FabsT1;

			FabsT2=sfabs(adj_e);
			if(FabsT2>largestT)
				largestT=FabsT2;

			FabsT1=sfabs(adj_f);
			if(FabsT1>largestT)
				largestT=FabsT1;

			/*
			** find the largest of p,q,r 
			*/
			largestB= sfabs(adj_p);

			FabsT1=sfabs(adj_q);
			if (FabsT1>largestB)
				  largestB=FabsT1;

			FabsT2=sfabs(adj_r);
			if (FabsT2>largestB)
				  largestB=FabsT2;
		#endif

			/*
			** calculate a fast floor(log2(largest/largest_bot))
			*/

			largestT=1.0f/largestT;

			topExponent.f=largestB*largestT;

			/* extract the offset exponent from the IEEE fp number */

			exp=(topExponent.l>>23) & 0xff; 	

			/* calculate the reciprocal (ignore post normalising) */

			exp=126-exp;

			/*
			** rescale the texturing coefficients to 16 bits.
			*/

			largestT=largestT * 32767.0f;

			adj_a=adj_a * largestT;
			adj_b=adj_b * largestT;
			adj_c=adj_c * largestT;
			adj_d=adj_d * largestT;
			adj_e=adj_e * largestT;
			adj_f=adj_f * largestT;

			/* calculate a fast pow(2.0,floor())*largestT */

			topExponent.l=(exp+127)<<23;

			largestT=largestT * topExponent.f;

			adj_p=adj_p * largestT;
			adj_q=adj_q * largestT;
			adj_r=adj_r * largestT;

			/*
			** Texas can't handle an exponent greater than 15,so we will
   			** reduce the resolution of 'p', 'q', and 'r'.
			** THIS SHOULD HAPPEN **VERY** RARELY.The Level Of Detail
			** should have removed the texture by now.
			*/

			if(exp>15)
			{
				adj_p=(float)((sgl_int32)adj_p >>(exp-15));
				adj_q=(float)((sgl_int32)adj_q >>(exp-15));
				adj_r=(float)((sgl_int32)adj_r >>(exp-15));

				exp=15;
			}
			/*
			** Texas can't handle a negative exponent,so we will
   			** reduce the resolution of a,b,c,d,e and f.
			** This condition only happens if the texture is VERY zoomed.
			** It may not be worth the expense of testing for this.
			*/
			else if(exp<0)
			{
				adj_a=(float)((sgl_int32)adj_a >>(0-exp));
				adj_b=(float)((sgl_int32)adj_b >>(0-exp));
				adj_c=(float)((sgl_int32)adj_c >>(0-exp));
				adj_d=(float)((sgl_int32)adj_d >>(0-exp));
				adj_e=(float)((sgl_int32)adj_e >>(0-exp));
				adj_f=(float)((sgl_int32)adj_f >>(0-exp));

				exp=0;
			}



			/*
			**  now pack up the data in TEXAS format
			*/
	
			/*first instruction word*/

			pResults->Control1=((sgl_uint32)exp << SHIFT_EXPONENT);

			/*texture parameters*/ 

			pResults->TexCoeff1 =((sgl_int32)adj_r) & 0xffffUL;

	        pResults->TexCoeff2 =((sgl_int32)adj_q << 16);
	        pResults->TexCoeff2|=((sgl_int32)adj_p) & 0xffffUL;

	        pResults->TexCoeff3 =(MState->texas_precomp.TexAddress) << 16;
	        pResults->TexCoeff3|=((sgl_int32)adj_c) & 0xffffUL;

	        pResults->TexCoeff4 =((sgl_int32)adj_b << 16);
	        pResults->TexCoeff4|=((sgl_int32)adj_a) & 0xffffUL;

	        pResults->TexCoeff5 =(MState->texas_precomp.TexAddress) & 0xffff0000;
	        pResults->TexCoeff5|=((sgl_int32)adj_f) & 0xffffUL;

	        pResults->TexCoeff6 =((sgl_int32)adj_e << 16);
	        pResults->TexCoeff6|=((sgl_int32)adj_d) & 0xffffUL;

			SGL_TIME_STOP(TEXTURE_PARAMETER_SETUP_TIME)
			SGL_TIME_RESUME(TEXTURE_TIME)

		}/*end if MIP mapped or not*/
	
	} /*end for loop*/

	SGL_TIME_STOP(TEXTURE_TIME)

	/*
	// Undo our defines
	*/
	#undef Sr00
	#undef Sr01
	#undef Sr02
	#undef Sr03
	#undef Sr10
	#undef Sr11
	#undef Sr12
	#undef Sr13
	#undef Sr20
	#undef Sr21
	#undef Sr22
	#undef Sr23

}

/******************************************************************************
 * Function Name: DoTextureMappingAccurate
 *
 * Inputs       : NumberOfPlanes,
 *				  Planes,an array of pointers to the planes.
 *				  ObjToEye,transformation matrix
 *				  MState,the material state.
 * Outputs      : Results,an array of coefficients in TEXAS format.
 * Returns      : -
 * Globals Used : projection_mat.CFRreciprocal,the scale for 'C','F', and 'R'
 *
 * Description  : This transforms some texture axes into screen space,
 *				  calculates the inverse, and packs the coefficients
 *				  into TEXAS format. It also packs the TexAddress which includes
 *				  some of the texture flags.
 * Comments		: 
 *				  I deal with negative exponents, it may not be worth the 
 *				  expense.
 *****************************************************************************/

void DoTextureMappingAccurate(int							   NumberOfPlanes,
							  const TRANSFORMED_PLANE_STRUCT   * const Planes[],
							  const TRANSFORM_STRUCT		   *ObjToEye,
							  const MATERIAL_STATE_STRUCT	   *MState,
							  TEXTURING_RESULT_STRUCT  		   *Results)
{

	float 	largestT,largestB;
	float	compression,reciprocal,u,v,abc,def,pqr,min,approx1,approx2;
	float 	dudx,dvdx,dudy,dvdy;
	float	eye_a,eye_b,eye_c,eye_d,eye_e,eye_f,eye_p,eye_q,eye_r;
	float	adj_a,adj_b,adj_c,adj_d,adj_e,adj_f,adj_p,adj_q,adj_r;
	float	Sr00,Sr01,Sr02,Sr03,Sr10,Sr11,Sr12,Sr13,Sr20,Sr21,Sr22,Sr23;
	float	*UPoint,*VPoint,*OPoint,OPointD[3];
	float	x,y;
	flong 	topExponent,MipMap;
	long	exp;
	long	MipMant,MipExp;
	int		i;
	const LOCAL_PROJECTION_STRUCT  *pLocalProjMat = RnGlobalGetLocalProjMat();
	PROJECTION_MATRIX_STRUCT  * const pProjMat = RnGlobalGetProjMat ();
	float CFRreciprocal = pProjMat->CFRreciprocal;
	/*load up the Object to Screen matrix*/

	ASSERT(pLocalProjMat->valid); /*stop if the matrix is not valid*/

	SGL_TIME_START(TEXTURE_TIME)

	Sr00=pLocalProjMat->sr[0][0];
	Sr01=pLocalProjMat->sr[0][1];
	Sr02=pLocalProjMat->sr[0][2];
	Sr03=pLocalProjMat->sr[0][3];
	Sr10=pLocalProjMat->sr[1][0];
	Sr11=pLocalProjMat->sr[1][1];
	Sr12=pLocalProjMat->sr[1][2];
	Sr13=pLocalProjMat->sr[1][3];
	Sr20=ObjToEye->mat[2][0];
	Sr21=ObjToEye->mat[2][1];
	Sr22=ObjToEye->mat[2][2];
	Sr23=ObjToEye->mat[2][3];

	/*
	// also define convenient access to the "scaled" texture projection
	// as well
	*/
	#define TSr00 pLocalProjMat->textureSR[0][0]
	#define TSr01 pLocalProjMat->textureSR[0][1]
	#define TSr02 pLocalProjMat->textureSR[0][2]

	#define TSr10 pLocalProjMat->textureSR[1][0]
	#define TSr11 pLocalProjMat->textureSR[1][1]
	#define TSr12 pLocalProjMat->textureSR[1][2]

	#define TSr20 pLocalProjMat->textureSR[2][0]
	#define TSr21 pLocalProjMat->textureSR[2][1]
	#define TSr22 pLocalProjMat->textureSR[2][2]

	for(i=0;i<NumberOfPlanes;i++)
	{
		/*get a pointer to the U,V and O vector*/

		UPoint=(float*)(Planes[i]->pTextureData->pre_mapped.u_vect);
		VPoint=(float*)(Planes[i]->pTextureData->pre_mapped.v_vect);
		OPoint=(float*)(Planes[i]->pTextureData->pre_mapped.o_vect);

		/*
		** As the origin of the texture moves a long way from
		** the viewer, the texture coefficients get less accurate.
		** So the following code brings the origin back to
		** the closest approach of the texture plane and the viewer.
		*/


		/* translate the texture origin into eye space*/

		OPointD[0]=OPoint[0]-(ObjToEye->inv[0][3]);
		OPointD[1]=OPoint[1]-(ObjToEye->inv[1][3]);
		OPointD[2]=OPoint[2]-(ObjToEye->inv[2][3]);

		/* find the closest approach with 0 */

		u=-(OPointD[0]*UPoint[0] + OPointD[1]*UPoint[1] + OPointD[2]*UPoint[2]) /
			(UPoint[0]*UPoint[0] + UPoint[1]*UPoint[1] + UPoint[2]*UPoint[2]);

		v=-(OPointD[0]*VPoint[0] + OPointD[1]*VPoint[1] + OPointD[2]*VPoint[2]) /
			(VPoint[0]*VPoint[0] + VPoint[1]*VPoint[1] + VPoint[2]*VPoint[2]);
		
		u=(float)floor(u*0.5f)*2.0f; /*as textures repeat after 1 or 2 units*/
		v=(float)floor(v*0.5f)*2.0f;

		/* move the texture origin to the closest approach */

		OPointD[0]=OPoint[0] + u*UPoint[0] + v*VPoint[0];
		OPointD[1]=OPoint[1] + u*UPoint[1] + v*VPoint[1];
		OPointD[2]=OPoint[2] + u*UPoint[2] + v*VPoint[2];


		/*
		**	transform U,V and the origin from object space to eye space.
		*/
		eye_a= UPoint[0]*TSr00 +  UPoint[1]*TSr01 +  UPoint[2]*TSr02;
		eye_b= VPoint[0]*TSr00 +  VPoint[1]*TSr01 +  VPoint[2]*TSr02;
		eye_c=OPointD[0]*Sr00  + OPointD[1]*Sr01  + OPointD[2]*Sr02 + Sr03;

		eye_d= UPoint[0]*TSr10 +  UPoint[1]*TSr11 +  UPoint[2]*TSr12;
		eye_e= VPoint[0]*TSr10 +  VPoint[1]*TSr11 +  VPoint[2]*TSr12;
		eye_f=OPointD[0]*Sr10  + OPointD[1]*Sr11  + OPointD[2]*Sr12 + Sr13;

		eye_p= UPoint[0]*TSr20 +  UPoint[1]*TSr21 +  UPoint[2]*TSr22;
		eye_q= VPoint[0]*TSr20 +  VPoint[1]*TSr21 +  VPoint[2]*TSr22;
		eye_r=OPointD[0]*Sr20  + OPointD[1]*Sr21  + OPointD[2]*Sr22 + Sr23;

		/*
		**	calculate the adjoint.
		*/

		adj_a=eye_e*eye_r - eye_q*eye_f;
 		adj_b=eye_q*eye_c - eye_b*eye_r;
 		adj_c=eye_b*eye_f - eye_e*eye_c; 

	 	adj_d=eye_p*eye_f - eye_d*eye_r;
 		adj_e=eye_a*eye_r - eye_p*eye_c;
 		adj_f=eye_d*eye_c - eye_a*eye_f; 

	 	adj_p=eye_d*eye_q - eye_p*eye_e; 
	 	adj_q=eye_p*eye_b - eye_a*eye_q; 
	 	adj_r=eye_a*eye_e - eye_d*eye_b; 

		/*
		**	calculate the MIP-map coefficient.
		*/

		/* calculate u and v at the representative point */

		x=Planes[i]->projRepPoint[0];
		y=Planes[i]->projRepPoint[1];

		pqr=adj_p*x + adj_q*y + adj_r;

		if(MState->texas_precomp.TexAddress & MASK_MIP_MAPPED)
		{   /*do the mip mapped calculation*/

			abc=adj_a*x + adj_b*y + adj_c;
			def=adj_d*x + adj_e*y + adj_f;


			/*pqr is only zero if the routine has been given bad data*/
			ASSERT(pqr!=0)
	
			reciprocal=1.0f/pqr;

			u=abc*reciprocal;
			v=def*reciprocal;

			/*
			// calculate the changes in u,v when x is incremented, and
			// get abs value
			//
			// (Re-arranged this from original, as gcc was putting in some
			// double maths!
			*/
			reciprocal=1.0f/(pqr+adj_p);
		
			dudx= sfabs((abc+adj_a)*reciprocal - u);
			dvdx= sfabs((def+adj_d)*reciprocal - v);

			/* approximate the pythagorean distance */
			min=(dudx < dvdx) ? dudx : dvdx;
			approx1= dudx + dvdx - (0.585786f*min);

	
			/* calculate the changes in u,v when y is incremented */
			reciprocal=1.0f/(pqr+adj_q);
	
			dudy=sfabs((abc+adj_b)*reciprocal - u);
			dvdy=sfabs((def+adj_e)*reciprocal - v);

			min=( dudy < dvdy ) ? dudy : dvdy;
			approx2= dudy + dvdy - (0.585786f*min);

			/* pick the larger of the distances */
#define ORIGINAL 0
#if ORIGINAL
			if (approx1>approx2)
			  	compression=approx1;
	   		else
				compression=approx2; 
#else
			/*
			// Pick a combination of the two...
			*/
			if (approx1>approx2)
			{
				if((approx1 * 0.25f) > approx2)
				{
			  		compression=approx1 * 0.25f;
				}
				else if((approx1 * 0.5f) > approx2)
				{
			  		compression=approx1 * 0.5f;
				}
				else
				{
			  		compression=approx1;
				}
			}
	   		else
			{	
				if((approx2 * 0.25f) > approx1)
				{
			  		compression=approx2 * 0.25f;
				}
				else if((approx2 * 0.5f) > approx1)
				{
			  		compression=approx2 * 0.5f;
				}
				else
				{
			  		compression=approx2;
				}
			}

#endif

			/*
			** TEXAS does not divide by a negative number correctly,
			** so we need to negate everything
			*/

			SGL_TIME_SUSPEND(TEXTURE_TIME)
			SGL_TIME_START(TEXTURE_PARAMETER_SETUP_TIME)
			if(pqr<0.0f)
			{
				adj_a=-adj_a;
				adj_b=-adj_b;
				adj_c=-adj_c;
				adj_d=-adj_d;
				adj_e=-adj_e;
				adj_f=-adj_f;
				adj_p=-adj_p;
				adj_q=-adj_q;
				adj_r=-adj_r;
			}


			/*
			**	rescale 'c','f', and 'r' to keep the coefficients
			**  in a similar range to a,b,d,e,p and q.
			**  Texas will scale them back to the right values.
			*/

			adj_c*=CFRreciprocal;
			adj_f*=CFRreciprocal;
			adj_r*=CFRreciprocal;

			/*
			**	find the largest of a,b,c,d,e,f 
			*/
			largestT= sfabs(adj_a);

			if (sfabs(adj_b)>largestT)
				  largestT=sfabs(adj_b);

			if (sfabs(adj_c)>largestT)
				  largestT=sfabs(adj_c);

			if (sfabs(adj_d)>largestT)
				  largestT=sfabs(adj_d);

			if (sfabs(adj_e)>largestT)
				  largestT=sfabs(adj_e);

	  		if (sfabs(adj_f)>largestT)
				  largestT=sfabs(adj_f);

			/*
			** find the largest of p,q,r 
			*/
			largestB= sfabs(adj_p);

			if (sfabs(adj_q)>largestB)
				  largestB=sfabs(adj_q);

			if (sfabs(adj_r)>largestB)
				  largestB=sfabs(adj_r);


			/*
			** calculate a fast floor(log2(largest/largest_bot))
			*/

			largestT=1.0f/largestT;

			topExponent.f=largestB*largestT;

			/* extract the offset exponent from the IEEE fp number */

			exp=(topExponent.l>>23) & 0xff; 	

			/* calculate the reciprocal (ignore post normalising) */

			exp=126-exp;


			/*
			** rescale the texturing coefficients to 16 bits.
			*/

			largestT=largestT * 32767.0f;

			adj_a=adj_a * largestT;
			adj_b=adj_b * largestT;
			adj_c=adj_c * largestT;
			adj_d=adj_d * largestT;
			adj_e=adj_e * largestT;
			adj_f=adj_f * largestT;

			/* calculate a fast pow(2.0,floor()) */

			topExponent.l=(exp+127)<<23;

			largestT=largestT * topExponent.f;

			pqr=pqr * largestT;
			adj_p=adj_p * largestT;
			adj_q=adj_q * largestT;
			adj_r=adj_r * largestT;

			/* solve the compression for 'n' */

			MipMap.f=compression*pqr*pqr;

			/* convert from IEEE to the TEXAS floating point format*/

			MipMant =(MipMap.l>>16)&0x7f;
			MipMant+=128; /*add in the implied 0.5*/

			MipExp=(MipMap.l>>23)-126; /*126 because of the different decimal point*/


			/*
			** Texas can't handle an exponent greater than 15,so we will
   			** reduce the resolution of 'p', 'q', and 'r'.
			** THIS SHOULD HAPPEN **VERY** RARELY.The Level Of Detail
			** should have removed the texture by now.
			*/

			if(exp>15)
			{
				adj_p=(float)((sgl_int32)adj_p >>(exp-15));
				adj_q=(float)((sgl_int32)adj_q >>(exp-15));
				adj_r=(float)((sgl_int32)adj_r >>(exp-15));
				MipExp-=(exp-15)<<1;

				exp=15;
			}

			/*
			** Texas can't handle a negative exponent,so we will
   			** reduce the resolution of a,b,c,d,e and f.
			** This condition only happens if the texture is VERY zoomed.
			** It may not be worth the expense of testing for this.
			*/
			else if(exp<0)
			{
				adj_a=(float)((sgl_int32)adj_a >>(0-exp));
				adj_b=(float)((sgl_int32)adj_b >>(0-exp));
				adj_c=(float)((sgl_int32)adj_c >>(0-exp));
				adj_d=(float)((sgl_int32)adj_d >>(0-exp));
				adj_e=(float)((sgl_int32)adj_e >>(0-exp));
				adj_f=(float)((sgl_int32)adj_f >>(0-exp));

				exp=0;
			}

			/*
			**  now pack up the data in TEXAS format
			**  I will remove some of the masks when the
			**  routine works.
			*/
			/*first instruction word*/

			Results[i].Control1=((sgl_uint32)exp << SHIFT_EXPONENT) & MASK_EXPONENT;

			/*texture parameters*/ 

			Results[i].TexCoeff1 =((sgl_uint32)MipMant << SHIFT_PMIP_M) & MASK_PMIP_M;
			Results[i].TexCoeff1|=((sgl_uint32)MipExp  << SHIFT_PMIP_E) & MASK_PMIP_E;
			Results[i].TexCoeff1|=((sgl_int32)adj_r) & 0xffffUL;

	        Results[i].TexCoeff2 =((sgl_int32)adj_q << 16) & 0xffff0000;
	        Results[i].TexCoeff2|=((sgl_int32)adj_p) & 0xffffUL;

	        Results[i].TexCoeff3 =(MState->texas_precomp.TexAddress) << 16;
	        Results[i].TexCoeff3|=((sgl_int32)adj_c) & 0xffffUL;

	        Results[i].TexCoeff4 =((sgl_int32)adj_b << 16) & 0xffff0000;
	        Results[i].TexCoeff4|=((sgl_int32)adj_a) & 0xffffUL;

	        Results[i].TexCoeff5 =(MState->texas_precomp.TexAddress) & 0xffff0000;
	        Results[i].TexCoeff5|=((sgl_int32)adj_f) & 0xffffUL;

	        Results[i].TexCoeff6 =((sgl_int32)adj_e << 16) & 0xffff0000;
	        Results[i].TexCoeff6|=((sgl_int32)adj_d) & 0xffffUL;

			SGL_TIME_STOP(TEXTURE_PARAMETER_SETUP_TIME)
			SGL_TIME_RESUME(TEXTURE_TIME)

		} 
		else    /*do the non-mip map calculation */
		{
			/*
			** TEXAS does not divide by a negative number correctly,
			** so we need to negate everything
			*/
			SGL_TIME_SUSPEND(TEXTURE_TIME)
			SGL_TIME_START(TEXTURE_PARAMETER_SETUP_TIME)
			if(pqr<0.0f)
			{
				adj_a=-adj_a;
				adj_b=-adj_b;
				adj_c=-adj_c;
				adj_d=-adj_d;
				adj_e=-adj_e;
				adj_f=-adj_f;
				adj_p=-adj_p;
				adj_q=-adj_q;
				adj_r=-adj_r;
			}
	
			/*
			**	rescale 'c','f', and 'r' to keep the coefficients
			**  in a similar range to a,b,d,e,p and q.
			**  Texas will scale them back to the right values.
			*/

			adj_c*=CFRreciprocal;
			adj_f*=CFRreciprocal;
			adj_r*=CFRreciprocal;

			/*
			**	find the largest of a,b,c,d,e,f 
			*/

			largestT= sfabs(adj_a);

			if (sfabs(adj_b)>largestT)
				  largestT=sfabs(adj_b);

			if (sfabs(adj_c)>largestT)
				  largestT=sfabs(adj_c);

			if (sfabs(adj_d)>largestT)
				  largestT=sfabs(adj_d);

			if (sfabs(adj_e)>largestT)
				  largestT=sfabs(adj_e);

	  		if (sfabs(adj_f)>largestT)
				  largestT=sfabs(adj_f);

			/*
			** find the largest of p,q,r 
			*/

			largestB= sfabs(adj_p);

			if (sfabs(adj_q)>largestB)
				  largestB=sfabs(adj_q);

			if (sfabs(adj_r)>largestB)
				  largestB=sfabs(adj_r);

			/*
			** calculate a fast floor(log2(largest/largest_bot))
			*/

			largestT=1.0f/largestT;

			topExponent.f=largestB*largestT;

			/* extract the offset exponent from the IEEE fp number */

			exp=(topExponent.l>>23) & 0xff; 	

			/* calculate the reciprocal (ignore post normalising) */

			exp=126-exp;

			/*
			** rescale the texturing coefficients to 16 bits.
			*/

			largestT=largestT * 32767.0f;

			adj_a=adj_a * largestT;
			adj_b=adj_b * largestT;
			adj_c=adj_c * largestT;
			adj_d=adj_d * largestT;
			adj_e=adj_e * largestT;
			adj_f=adj_f * largestT;

			/* calculate a fast pow(2.0,floor()) */

			topExponent.l=(exp+127)<<23;

			largestT=largestT * topExponent.f;

			adj_p=adj_p * largestT;
			adj_q=adj_q * largestT;
			adj_r=adj_r * largestT;

			/*
			** Texas can't handle an exponent greater than 15,so we will
   			** reduce the resolution of 'p', 'q', and 'r'.
			** THIS SHOULD HAPPEN **VERY** RARELY.The Level Of Detail
			** should have removed the texture by now.
			*/

			if(exp>15)
			{
				adj_p=(float)((sgl_int32)adj_p >>(exp-15));
				adj_q=(float)((sgl_int32)adj_q >>(exp-15));
				adj_r=(float)((sgl_int32)adj_r >>(exp-15));

				exp=15;
			}
			/*
			** Texas can't handle a negative exponent,so we will
   			** reduce the resolution of a,b,c,d,e and f.
			** This condition only happens if the texture is VERY zoomed.
			** It may not be worth the expense of testing for this.
			*/
			else if(exp<0)
			{
				adj_a=(float)((sgl_int32)adj_a >>(0-exp));
				adj_b=(float)((sgl_int32)adj_b >>(0-exp));
				adj_c=(float)((sgl_int32)adj_c >>(0-exp));
				adj_d=(float)((sgl_int32)adj_d >>(0-exp));
				adj_e=(float)((sgl_int32)adj_e >>(0-exp));
				adj_f=(float)((sgl_int32)adj_f >>(0-exp));

				exp=0;
			}



			/*
			**  now pack up the data in TEXAS format
			**  I will remove some of the masks when the
			**  routine works.
			*/
	
			/*first instruction word*/

			Results[i].Control1=((sgl_uint32)exp << SHIFT_EXPONENT) & MASK_EXPONENT;

			/*texture parameters*/ 

			Results[i].TexCoeff1 =((sgl_int32)adj_r) & 0xffffUL;

	        Results[i].TexCoeff2 =((sgl_int32)adj_q << 16) & 0xffff0000;
	        Results[i].TexCoeff2|=((sgl_int32)adj_p) & 0xffffUL;

	        Results[i].TexCoeff3 =(MState->texas_precomp.TexAddress) << 16;
	        Results[i].TexCoeff3|=((sgl_int32)adj_c) & 0xffffUL;

	        Results[i].TexCoeff4 =((sgl_int32)adj_b << 16) & 0xffff0000;
	        Results[i].TexCoeff4|=((sgl_int32)adj_a) & 0xffffUL;

	        Results[i].TexCoeff5 =(MState->texas_precomp.TexAddress) & 0xffff0000;
	        Results[i].TexCoeff5|=((sgl_int32)adj_f) & 0xffffUL;

	        Results[i].TexCoeff6 =((sgl_int32)adj_e << 16) & 0xffff0000;
	        Results[i].TexCoeff6|=((sgl_int32)adj_d) & 0xffffUL;

			SGL_TIME_STOP(TEXTURE_PARAMETER_SETUP_TIME)
			SGL_TIME_RESUME(TEXTURE_TIME)

		}
	} /*end for loop*/

	SGL_TIME_STOP(TEXTURE_TIME)
}


/******************************************************************************
 * Function Name: IntersectSphere
 *
 * Inputs       : O,the origin of the line.
 *				  V,the vector along the line (does not have to be normal)
 *				  Radius, of the sphere.
 * Outputs      : Result,the intersection point.
 * Returns      : Valid, if the origin is within the sphere.
 * Globals Used : -
 *
 * Description  : Intersects a line and a sphere. The origin of the line must be
 *				: inside the sphere. The intersection point returned is in the
 *				: direction of the vector V.
 * Comments		: Taken from An Introduction to Ray Tracing by Glassner pp91
 *****************************************************************************/
sgl_bool IntersectSphere(sgl_vector O,sgl_vector V,float Radius,sgl_vector Result)
{
	float A,B,C,X,Part;

	Radius=Radius*Radius;
	C=O[0]*O[0] + O[1]*O[1] + O[2]*O[2];
	C-=Radius;

	if(C<0.0f)
	{
		/*
		** inside the sphere
		*/

		B=(O[0]*V[0] + O[1]*V[1] + O[2]*V[2]) *2.0f;
		A=V[0]*V[0] + V[1]*V[1] + V[2]*V[2];

		/*find the roots of the quadratic*/

		Part=ssqrt(B*B - 4.0f*A*C); /*This should never be complex*/

		X=-B + Part;

		if(X>0.0f)
		{
			/*we have the forward direction root*/

			X/=2.0f*A;
		}
		else
		{
			/*pick the other root*/
			
			X=-B - Part;
			X/=2.0f*A;

		}

		/*calculate the intersection point*/
		
		Result[0]=O[0] + X*V[0];
		Result[1]=O[1] + X*V[1];
		Result[2]=O[2] + X*V[2];

		return(TRUE);
	}
	else
	{
		/*
		** outside the sphere
		*/

		return(FALSE);
	}

}

/******************************************************************************
 * Function Name: IntersectCylinder
 *
 * Inputs       : O,the origin of the line.
 *				  V,the vector along the line (does not have to be normal)
 *				  Radius, of the cylinder.
 * Outputs      : Result,the intersection point.
 * Returns      : Valid, if the origin is within the cylinder and the vector is
 *				  not pointing along the Y axis.
 * Globals Used : -
 *
 * Description  : Intersects a line and a cylinder. The origin of the line must be
 *				: inside the cylinder. The intersection point returned is in the
 *				: direction of the vector V.
 * Comments		: Taken from An Introduction to Ray Tracing by Glassner pp91
 *				: The cylinder is aligned along the Y axis. 
 *****************************************************************************/
sgl_bool IntersectCylinder(sgl_vector O,sgl_vector V,float Radius,sgl_vector Result)
{
	float A,B,C,X,Part;

	Radius=Radius*Radius;
	C=O[0]*O[0] + O[2]*O[2];
	C-=Radius;

	if(C<0.0f)
	{
		/*
		** inside the cylinder
		*/

		B=(O[0]*V[0] + O[2]*V[2]) *2.0f;
		A=V[0]*V[0] + V[2]*V[2];

	  #if defined (MIDAS_ARCADE)  || defined (ZEUS_ARCADE)

		/* MIDAS Arcade gets FP execeptions if we use 1.0e-20f.  1.0e-3f seems to be okay */
		if(A>1.0e-3f) /* is it not pointing along the Y axis */

	  #else

		if(A>1.0e-20f) /* is it not pointing along the Y axis */

	  #endif
		{
			/*find the roots of the quadratic*/

			ASSERT((B*B - 4.0f*A*C)>=0.0f)
			Part=ssqrt(B*B - 4.0f*A*C); /*This should never be complex*/

			X=-B + Part;

			if(X>0.0f)
			{
				/*we have the forward direction root*/

				X/=2.0f*A;
			}
			else
			{
				/*pick the other root*/
				
				X=-B - Part;
				X/=2.0f*A;
			}

			/*calculate the intersection point*/
		
			Result[0]=O[0] + X*V[0];
			Result[1]=O[1] + X*V[1];
			Result[2]=O[2] + X*V[2];

			return(TRUE);
		}
		else
		{	
			/*we are pointing along the Y axis */
	
			return(FALSE);
		}
	}
	else
	{
		/*
		** outside the cylinder
		*/

		return(FALSE);
	}

}

/******************************************************************************
 * Function Name: IntersectPlane
 *
 * Inputs       : O,the origin of the line.
 *				  V,the vector along the line (does not have to be normal)
 * Outputs      : Result,the intersection point.
 * Returns      : Valid, if the line is not parallel to the plane.
 * Globals Used : -
 *
 * Description  : Intersects a line and the z=0 plane.
 *****************************************************************************/
sgl_bool IntersectPlane(sgl_vector O,sgl_vector V,sgl_vector Result)
{
	float X;
	
  #if defined (MIDAS_ARCADE) || defined (ZEUS_ARCADE)

	/* MIDAS Arcade gets FP execeptions if we use 1.0e-20f.  1.0e-3f seems to be okay */
	if(sfabs(V[2])>1.0e-3f)

  #else

	if(sfabs(V[2])>1.0e-20f)

  #endif
	{
		X=-O[2] / V[2];

   		/*calculate the intersection point*/
		
   		Result[0]=O[0] + X*V[0];
   		Result[1]=O[1] + X*V[1];
   		Result[2]=0.0f;

		return(TRUE);
	}
	else
	{
		return(FALSE);
	}


}

/******************************************************************************
 * Function Name: MapPlane
 *
 * Inputs       : P0, a position on the plane.
 *				  P1, ""
 *				  P2, ""
 *				  su,sv, the scale of U and V.
 *				  ou,ov, offset values for the origin.
 * Outputs      : UV0, the mapped P0
 *				  UV1, the mapped P1
 *				  UV2, the mapped P2
 * Returns      : -
 * Globals Used : -
 *
 * Description  : maps from three points on the z=0 plane to U,V coordinates.
 *****************************************************************************/
void MapPlane(sgl_vector P0,sgl_vector P1,sgl_vector P2,
   				float su,float sv,float ou,float ov,
   				sgl_2d_vec UV0,sgl_2d_vec UV1,sgl_2d_vec UV2)
{

	UV0[0]=(P0[0]-ou)/su;
	UV0[1]=(P0[1]-ov)/sv;

	UV1[0]=(P1[0]-ou)/su;
	UV1[1]=(P1[1]-ov)/sv;

	UV2[0]=(P2[0]-ou)/su;
	UV2[1]=(P2[1]-ov)/sv;

}

/******************************************************************************
 * Function Name: MapCylinder
 *
 * Inputs       : P0, a position on the cylinder.
 *				  P1, ""
 *				  P2, ""
 *				  su,sv, the scale of U and V.
 *				  ou,ov, offset values for the origin.
 * Outputs      : UV0, the mapped P0
 *				  UV1, the mapped P1
 *				  UV2, the mapped P2
 * Returns      : -
 * Globals Used : -
 *
 * Description  : maps from three points on a cylinder to U,V coordinates.
 *****************************************************************************/
void MapCylinder(sgl_vector P0,sgl_vector P1,sgl_vector P2,
   				float su,float sv,float ou,float ov,
   				sgl_2d_vec UV0,sgl_2d_vec UV1,sgl_2d_vec UV2)
{
	float ang0,ang1,ang2;

	/*
	** how far round the cylinder are the points ?
	*/

	/* atan2 is an expensive operation. It could be replaced by a single precision
	** ACOS and a little fiddling.
    */

	ang0=(float)atan2(P0[2],P0[0]); 
	ang1=(float)atan2(P1[2],P1[0]);
	ang2=(float)atan2(P2[2],P2[0]);

	/*
	** now, deal with the cross over from PI to -PI.
	*/

	if(P0[0]<0.0f && P1[0]<0.0f && P2[0]<0.0f) /*probably not a good idea*/
	{

		if(ang0>0.0f)
		{
			if(ang1<0.0f)
			    ang1+=SGL_2_PI;
			if(ang2<0.0f)
			    ang2+=SGL_2_PI;
		}
		else
		{
			if(ang1>0.0f || ang2>0.0f)
			{
				ang0+=SGL_2_PI;
				if(ang1<0.0f)
				    ang1+=SGL_2_PI;
				if(ang2<0.0f)
				    ang2+=SGL_2_PI;

			}
		}
	}

	UV0[0]=( (ang0/(float)SGL_2_PI)-ou )/su;    /*set the U coordinates*/
	UV1[0]=( (ang1/(float)SGL_2_PI)-ou )/su;
	UV2[0]=( (ang2/(float)SGL_2_PI)-ou )/su;

	UV0[1]=(P0[1]-ov)/sv;    /*set the V coordinates*/
	UV1[1]=(P1[1]-ov)/sv;    
	UV2[1]=(P2[1]-ov)/sv;    

}

/******************************************************************************
 * Function Name: MapSphere
 *
 * Inputs       : P0, a position on the sphere.
 *				  P1, ""
 *				  P2, ""
 *				  su,sv, the scale of U and V.
 *				  ou,ov, offset values for the origin.
 *				  Radius, the radius of the sphere.
 * Outputs      : UV0, the mapped P0
 *				  UV1, the mapped P1
 *				  UV2, the mapped P2
 * Returns      : -
 * Globals Used : -
 *
 * Description  : maps from three points on a sphere to U,V coordinates.
 *****************************************************************************/
void MapSphere(sgl_vector P0,sgl_vector P1,sgl_vector P2,
   				float su,float sv,float ou,float ov,float Radius,
   				sgl_2d_vec UV0,sgl_2d_vec UV1,sgl_2d_vec UV2)
{
	float ang0,ang1,ang2;
	float Part,Part1;

	/* atan2 is an expensive operation. It could be replaced by a single precision
	** ACOS and a little fiddling.
    */

	ang0=(float)atan2(P0[2],P0[0]); 
	ang1=(float)atan2(P1[2],P1[0]);
	ang2=(float)atan2(P2[2],P2[0]);

	/*
	** now, deal with the cross over from PI to -PI.
	*/

	if(P0[0]<0.0f && P1[0]<0.0f && P2[0]<0.0f) /*probably not a good idea*/
	{

		if(ang0>0.0f)
		{
			if(ang1<0.0f)
			    ang1+=SGL_2_PI;
			if(ang2<0.0f)
			    ang2+=SGL_2_PI;
		}
		else
		{
			if(ang1>0.0f || ang2>0.0f)
			{
				ang0+=SGL_2_PI;
				if(ang1<0.0f)
				    ang1+=SGL_2_PI;
				if(ang2<0.0f)
				    ang2+=SGL_2_PI;

			}
		}
	}

	Part=Radius*sv; /*partial result*/
	Part1=1.0f/(2.0f*Part);

	UV0[1]=(P0[1]- ov + Part)*Part1;    /*calculate the V coordinates*/
	UV1[1]=(P1[1]- ov + Part)*Part1;    
	UV2[1]=(P2[1]- ov + Part)*Part1;    

	UV0[0]=( (ang0/(float)SGL_2_PI)-ou )/su;    /*calculate part of the U coordinates*/
	UV1[0]=( (ang1/(float)SGL_2_PI)-ou )/su;
	UV2[0]=( (ang2/(float)SGL_2_PI)-ou )/su;


}

/******************************************************************************
 * Function Name: ConstructISurfaceNormalRay
 *
 * Inputs       : SmapType,
 *				  Point,the object point
 * Outputs      : O, the vector to the ray
 *				  V, the vector along the ray
 * Returns      : -
 * Globals Used : -
 *
 * Description  : Given a point on the object, a ray is generated that is
 *				  perpendicular to the intermediate surface normal and 
 *				  intersects the object point.
 *****************************************************************************/
void ConstructISurfaceNormalRay(sgl_uint32 SmapType,sgl_vector Point,
								sgl_vector O,sgl_vector V)
{

	switch (SmapType)
	{
		case sgl_smap_plane:
			O[0]=Point[0];
			O[1]=Point[1];
			O[2]=Point[2];

			V[0]=0.0f;
			V[1]=0.0f;
			V[2]=-1.0f;
			break;
		case sgl_smap_cylinder:
			O[0]=0.0f;
			O[1]=Point[1];
			O[2]=0.0f;

			V[0]=Point[0];
			V[1]=0.0f;
			V[2]=Point[2];

			break;
		case sgl_smap_sphere:
			O[0]=0.0f;
			O[1]=0.0f;
			O[2]=0.0f;

			V[0]=Point[0];
			V[1]=Point[1];
			V[2]=Point[2];
		
			break;

	}
}

/******************************************************************************
 * Function Name: ConstructReflectedRay
 *
 * Inputs       : ViewPoint,
 *				  Point,the object point
 *				  Normal,the normal to the surface
 * Outputs      : V, the vector along the ray
 * Returns      : -
 * Globals Used : -
 *
 * Description  : Given a point on the object, the normal to the surface and the
 *				  viewpoint, a ray is generated that is a reflection off the 
 *				  surface. 
 * Comments		: The equation is taken from Foley & VanDam pp730
 *****************************************************************************/
void ConstructReflectedRay(sgl_vector ViewPoint,
						   sgl_vector Point,
						   sgl_vector Normal,
						   sgl_vector V)
{
	sgl_vector I;
	float Dot;

	/* I=ViewPoint-Point */

	VecSub(ViewPoint,Point,I);

	VecNormalise(I);

	Dot= 2.0f*DotProd(Normal,I);

	V[0]=Dot*Normal[0] - I[0];
	V[1]=Dot*Normal[1] - I[1];
	V[2]=Dot*Normal[2] - I[2];

}

/******************************************************************************
 * Function Name: ConstructRefractedRay
 *
 * Inputs       : ViewPoint,
 *				  Point,the object point
 *				  Normal,the normal to the surface
 *				  RefracIndex, the refractive index of the solid
 * Outputs      : V, the vector along the ray
 * Returns      : -
 * Globals Used : -
 *
 * Description  : Given a point on the object, the normal to the surface,the 
 *				  refractive index and the viewpoint, a ray is generated that 
 * 				  is refracted through the surface. 
 * Comments		: The equation is taken from Foley & VanDam pp758
 *****************************************************************************/
void ConstructRefractedRay(sgl_vector ViewPoint,
						   sgl_vector Point,
						   sgl_vector Normal,
						   float RefracIndex,
						   sgl_vector V)
{
	sgl_vector I;
	float Dot,Part;

	/* I=ViewPoint-Point */

	VecSub(ViewPoint,Point,I);

	VecNormalise(I);

	Dot= DotProd(Normal,I);

	Part=RefracIndex*Dot - ssqrt(1.0f - RefracIndex*RefracIndex*(1.0f-Dot*Dot));

	V[0]=Normal[0]*Part - RefracIndex*I[0];
	V[1]=Normal[1]*Part - RefracIndex*I[1];
	V[2]=Normal[2]*Part - RefracIndex*I[2];



}

/******************************************************************************
 * Function Name: DoTextureWrapping
 *
 * Inputs       : NumberOfPlanes,
 *				  Planes,an array of pointers to the planes.
 *				  ObjToEye,transformation matrix
 *				  MState,the material state.
 * Outputs      : Results,an array of coefficients in TEXAS format.
 * Returns      : -
 * Globals Used : TempTexResults,
 *
 * Description  : This performs the mapping from points in object space to
 *				  UV coordinates. These are then transformed to U,V, and O 
 *				  vectors in object space. The texture pointer in the transformed
 *				  plane structure is redirected to this temp data, and 
 *				  finally DoTextureMapping is called.
 *
 * Comments		: map optimise the transformState==WrappingTransform case
 *****************************************************************************/
void DoTextureWrapping(int						NumberOfPlanes,
					TRANSFORMED_PLANE_STRUCT   * Planes[],
					  TRANSFORM_STRUCT		   *ObjToEye,
					  MATERIAL_STATE_STRUCT	   *MState,
					  TEXTURING_RESULT_STRUCT  *Results)
{

	sgl_vector V1,V2,V3,O1,O2,O3;
	sgl_vector IPoint1,IPoint2,IPoint3;
	sgl_vector SourceO1,SourceO2,SourceO3;
	sgl_vector SourceN1,SourceN2,SourceN3;
	sgl_vector ViewPoint;
	sgl_2d_vec UV0,UV1,UV2;
	int	i;
	TRANSFORM_STRUCT WrapSpace;
	TRANSFORM_STRUCT WrappingTransform;
	sgl_vector pt2, pt3;


	/* get a copy of the camera position in object space */

	ViewPoint[0]=ObjToEye->inv[0][3];
	ViewPoint[1]=ObjToEye->inv[1][3];
	ViewPoint[2]=ObjToEye->inv[2][3];

	/* We need to calculate the matrix that moves the wrapped object
    ** in relation to the intermediate surface.
    ** When the smap was placed in the display list, a copy of the 
    ** current transformation was made in 'WrappingTransform'.
    ** Multiplying the current transform by the inverse of the 
    ** wrapping transform should be the object to wrap space matrix.
    */

	/* get a local copy of the wrapping transform */
	RnGlobalCopyWrappingTransform(&WrappingTransform);

	for(i=0;i<3;i++)
	{

		WrapSpace.mat[i][0]= ObjToEye->mat[0][0] * WrappingTransform.inv[i][0] +
						     ObjToEye->mat[1][0] * WrappingTransform.inv[i][1] +
						     ObjToEye->mat[2][0] * WrappingTransform.inv[i][2] ;
		WrapSpace.mat[i][1]= ObjToEye->mat[0][1] * WrappingTransform.inv[i][0] +
						     ObjToEye->mat[1][1] * WrappingTransform.inv[i][1] +
						     ObjToEye->mat[2][1] * WrappingTransform.inv[i][2] ;
		WrapSpace.mat[i][2]= ObjToEye->mat[0][2] * WrappingTransform.inv[i][0] +
						     ObjToEye->mat[1][2] * WrappingTransform.inv[i][1] +
						     ObjToEye->mat[2][2] * WrappingTransform.inv[i][2] ;
		WrapSpace.mat[i][3]= ObjToEye->mat[0][3] * WrappingTransform.inv[i][0] +
						     ObjToEye->mat[1][3] * WrappingTransform.inv[i][1] +
						     ObjToEye->mat[2][3] * WrappingTransform.inv[i][2] +
							 WrappingTransform.inv[i][3];
	}


	for(i=0;i<NumberOfPlanes;i++)
	{

		/*transform the points into wrapping space */


		/*we don't allow wrapping on sgl_add_simple_planes*/

		ASSERT(Planes[i]->pPointsData);

		VecAdd (Planes[i]->pPointsData->pt1, Planes[i]->pPointsData->pt2_delta, pt2);
		VecAdd (Planes[i]->pPointsData->pt1, Planes[i]->pPointsData->pt3_delta, pt3);

		TransformVector(&WrapSpace,Planes[i]->pPointsData->pt1,SourceO1);
		TransformVector(&WrapSpace,pt2,						   SourceO2);
		TransformVector(&WrapSpace,pt3,						   SourceO3);

		/*rotate the normals into wrapping space */
		/* NOTE there better not be any scale */
		/* NOTE i may optimise this out for the I-surface normal */

		if (Planes[i]->flags & pf_smooth_shad)
		{
		
		
			TransformDirVector(&WrapSpace,Planes[i]->pShadingData->norm1,SourceN1); 
			TransformDirVector(&WrapSpace,Planes[i]->pShadingData->norm2,SourceN2); 
			TransformDirVector(&WrapSpace,Planes[i]->pShadingData->norm3,SourceN3); 
		}
		else
		{
			/* we have to flat wrap */

			TransformDirVector(&WrapSpace,Planes[i]->pOriginalData->normal,SourceN1); 

			/*the other two normals are identical*/

			SourceN2[0]=SourceN1[0];
			SourceN2[1]=SourceN1[1];
			SourceN2[2]=SourceN1[2];

			SourceN3[0]=SourceN1[0];
			SourceN3[1]=SourceN1[1];
			SourceN3[2]=SourceN1[2];

		}

		/* NOTE I may rewrite this copy*/

		O1[0]=SourceO1[0];
		O1[1]=SourceO1[1];
		O1[2]=SourceO1[2];
		O2[0]=SourceO2[0];
		O2[1]=SourceO2[1];
		O2[2]=SourceO2[2];
		O3[0]=SourceO3[0];
		O3[1]=SourceO3[1];
		O3[2]=SourceO3[2];

		switch(MState->texture_flags & 0x1C) /* sgl_omap_types*/
		{
			case sgl_omap_obj_normal:
				/*just copy the data over*/
			
				V1[0]=SourceN1[0];
				V1[1]=SourceN1[1];
				V1[2]=SourceN1[2];
			
				V2[0]=SourceN2[0];
				V2[1]=SourceN2[1];
				V2[2]=SourceN2[2];

				V3[0]=SourceN3[0];
				V3[1]=SourceN3[1];
				V3[2]=SourceN3[2]; 

				break;

			case sgl_omap_inter_normal:
				ConstructISurfaceNormalRay(MState->texture_flags & 0x3,
										   SourceO1,O1,V1);
				ConstructISurfaceNormalRay(MState->texture_flags & 0x3,
										   SourceO2,O2,V2);
				ConstructISurfaceNormalRay(MState->texture_flags & 0x3,
										   SourceO3,O3,V3);

				break;
			case sgl_omap_reflection:

				ConstructReflectedRay(ViewPoint,SourceO1,
									  SourceN1,V1);
				ConstructReflectedRay(ViewPoint,SourceO2,
									  SourceN2,V2);
				ConstructReflectedRay(ViewPoint,SourceO3,
									  SourceN3,V3);

				break;
			case sgl_omap_transmission:

				ConstructRefractedRay(ViewPoint,
									  SourceO1,
									  SourceN1,
									  MState->refrac_index,
									  V1);
				ConstructRefractedRay(ViewPoint,
									  SourceO2,
									  SourceN2,
									  MState->refrac_index,
									  V2);
				ConstructRefractedRay(ViewPoint,
									  SourceO3,
									  SourceN3,
									  MState->refrac_index,
									  V3);

				break;
			default:

				DPF ((DBG_ERROR, "Bad OMAP"));

				break;
		}

		/*
		** The bounced rays have been generated.
		** Next, the intersection of the rays and the intermediate surface is
		** performed, and then mapped into UV space.
		*/


		switch(MState->texture_flags & 0x3) /* sgl_smap_types */
		{
			case sgl_smap_plane:
				if( IntersectPlane(O1,V1,IPoint1) &&
					IntersectPlane(O2,V2,IPoint2) &&
					IntersectPlane(O3,V3,IPoint3) )
				{
					MapPlane(IPoint1,IPoint2,IPoint3,
   						 	 MState->su,MState->sv,MState->ou,MState->ov,
   							 UV0,UV1,UV2);
				}
				else
				{
					/* Construct dummy parameters*/
					DPF((DBG_WARNING, "Invalid texture wrapping ray"));

					UV0[0]=0.0f;
					UV0[1]=0.0f;
					UV1[0]=1.0f;
					UV1[1]=0.0f;
					UV2[0]=0.0f;
					UV2[1]=1.0f;
				}			
				break;

			case sgl_smap_cylinder:
				if( IntersectCylinder(O1,V1,MState->radius,IPoint1) &&
					IntersectCylinder(O2,V2,MState->radius,IPoint2) &&
					IntersectCylinder(O3,V3,MState->radius,IPoint3) )
				{
					MapCylinder(IPoint1,IPoint2,IPoint3,
   						 	 	MState->su,MState->sv,MState->ou,MState->ov,
   							 	UV0,UV1,UV2);
				}
				else
				{
					/* Construct dummy parameters*/
					DPF((DBG_WARNING, "Invalid texture wrapping ray"));

					UV0[0]=0.0f;
					UV0[1]=0.0f;
					UV1[0]=1.0f;
					UV1[1]=0.0f;
					UV2[0]=0.0f;
					UV2[1]=1.0f;
				}
				break;

			case sgl_smap_sphere:

				if( IntersectSphere(O1,V1,MState->radius,IPoint1) && 
					IntersectSphere(O2,V2,MState->radius,IPoint2) &&
					IntersectSphere(O3,V3,MState->radius,IPoint3) )
				{
					MapSphere(IPoint1,IPoint2,IPoint3,
   						      MState->su,MState->sv,MState->ou,MState->ov,MState->radius,
   							  UV0,UV1,UV2);
				}
				else
				{
					/* Construct dummy parameters*/
					DPF((DBG_WARNING, "Invalid texture wrapping ray"));

					UV0[0]=0.0f;
					UV0[1]=0.0f;
					UV1[0]=1.0f;
					UV1[1]=0.0f;
					UV2[0]=0.0f;
					UV2[1]=1.0f;
				}
				break;
			default:

				DPF ((DBG_ERROR, "Bad IMAP"));

				break;
	   	}

		/* convert the three points and three UV's to the internal format of
		   U,V, and O */ 

		MapExternalToInternal((float*)(Planes[i]->pPointsData->pt1),
   							  pt2,
   							  pt3,
  							  UV0, UV1, UV2,
							  TempTexResults[i].pre_mapped.u_vect,
							  TempTexResults[i].pre_mapped.v_vect,
							  TempTexResults[i].pre_mapped.o_vect);

		/*redirect the projected plane texture pointer to this data*/

		Planes[i]->pTextureData=&(TempTexResults[i]);
	}

	/*now calculate the TEXAS coefficients */

	DoTextureMappingFast( NumberOfPlanes, 
						  (const TRANSFORMED_PLANE_STRUCT **)Planes,
						  ObjToEye, MState, Results );


};

/*
// End of file
*/

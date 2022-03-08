/******************************************************************************
 * Name         : rnshade.h
 * Title        : SGL API
 * Author       : Unknown
 * Created      : Unknown
 *
 * Copyright	: 1995-2022 Imagination Technologies (c)
 * License		: MIT
 *
 * Description  : Header file for Smooth Shading module 
 * 
 * Platform     : ANSI
 *
 * Modifications:
 * $Log: rnshade.h,v $
 * Revision 1.9  1996/12/09  13:38:47  gdc
 * removed all references to TRIANGLE API and added a proper file header
 * assume that D3DOPTIM stuff has fully takken over
 *
 *
 *****************************************************************************/

typedef struct tagSMOOTHPARAMS
{
	sgl_vector	vOrigin;
	sgl_vector	vVPOrigin;
	sgl_vector	vVPdX;
	sgl_vector	vVPdY;
	float		f1OverVPZ;
	
} SMOOTHPARAMS, *PSMOOTHPARAMS;

void SmoothObjectPrecalc (const MASTER_STATE_STRUCT *pState, PSMOOTHPARAMS pSM);

void GetSmoothShadingParameters (int numPlanes,
								 TRANSFORMED_PLANE_STRUCT *ptransPlanes[],
								 TRANSFORM_STRUCT *t,
								 MATERIAL_STATE_STRUCT *stateMaterial,
								 LIGHTS_STATE_STRUCT *lightState,
								 SHADING_RESULT_STRUCT *pShadingResults,
								 PSMOOTHPARAMS pSP);



void GetSmoothShadingParametersAdjoint (int numPlanes,
								 TRANSFORMED_PLANE_STRUCT *ptransPlanes[],
								 TRANSFORM_STRUCT *t,
								 MATERIAL_STATE_STRUCT *stateMaterial,
								 LIGHTS_STATE_STRUCT *lightState,
								 SHADING_RESULT_STRUCT *pShadingResults,
								 PSMOOTHPARAMS pSP);

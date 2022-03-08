/******************************************************************************
 * Name : dltransf.c
 * Title : Transformation Display List Routine,  List nodes
 * Author : Stel Michael
 * Created : 1/05/1995
 *
 * Copyright : 1995-2022 Imagination Technologies (c)
 * License	 : MIT
 *
 * Description : SGL Routines for creating and editing transformation nodes + utility routines
 *
 *				Most of this is covered in TRDD027x.doc
 *
 * Platform : ANSI compatible
 *
 * Modifications:-
 * $Log: dltransf.c,v $
 * Revision 1.31  1997/05/13  17:53:44  mjg
 * Replaced PVROS memory management call with an SGL one.
 *
 * Revision 1.30  1997/05/02  07:42:44  erf
 * Removed UINT32's etc and replaced with sgl_uint32's etc.
 *
 * Revision 1.29  1997/04/30  19:19:06  jop
 * Put Ed's changes back to how they were
 *
 * Revision 1.28  1997/04/29  14:46:13  erf
 * Added new PVROS API function call mechanism.
 *
 * Revision 1.27  1997/04/24  20:56:36  gdc
 * sgl now initialsed form DllMain in windows builds
 *
 * Revision 1.26  1997/04/02  21:27:54  mjg
 * Updated calling conventions.
 *
 * Revision 1.25  1997/03/27  12:36:08  erf
 * Operating system independent memory allocation/deallocation macros.
 * 1. Added #include "pvrosapi.h" to include new macros.
 * 2. Changed malloc, calloc, realloc and free to PVROSMalloc, PVROSCalloc,
 *    PVROSRealloc and PVROSFree.
 *
 * Revision 1.24  1997/02/11  16:49:40  mjg
 * A bit more negative scaling things.
 *
 * Revision 1.23  1997/02/10  13:30:37  sjf
 * Added "negative scaling" flag to transformation matrices.
 *
 * Revision 1.22  1996/09/30  15:05:28  msh
 * Scale function modified to use sfabs rather than fabs.
 * This stops single<->double conversions on the MIPS R5000
 * and probably is better for an Intel processor too.
 *
 * Revision 1.21  1996/06/12  11:48:51  jop
 * Commented out the DLLEXPORT stuff
 *
 * Revision 1.20  1996/06/10  14:10:30  jop
 * now exports axes in windows build
 *
 * Revision 1.19  1995/10/22  18:38:57  sjf
 * Had to comment out the predefined axis defs for Windows version...
 *
 * Revision 1.18  1995/08/07  15:10:11  sm
 * *** empty log message ***
 *
 * Revision 1.17  1995/08/04  14:39:16  sjf
 * 1) Added inverse rescale to transformation routines
 * 2) Fixed bug in scale when given invalid scaling factors
 * 3) Made a number of routines static, as they were not used elsewhere.
 *
 * Revision 1.17  1995/08/04  14:39:16  sjf
 * 1) Added inverse rescale to transformation routines
 * 2) Fixed bug in scale when given invalid scaling factors
 * 3) Made a number of routines static, as they were not used elsewhere.
 *
 * Revision 1.16  1995/08/02  11:37:46  pdl
 * added sfabs()
 *
 * Revision 1.15  1995/08/01  17:48:54  pdl
 * changed the doubles to floats.
 *
 * Revision 1.14  1995/07/26  15:07:44  sjf
 * Got rid of warnings with initialisation of static struct.
 *
 * Revision 1.13  1995/07/14  10:38:07  sm
 * added x, y and x axis definitions
 *
 * Revision 1.12  1995/07/06  12:06:35  sm
 * couple of fixes
 *
 * Revision 1.11  1995/07/06  10:47:25  sm
 * added check matrix
 *
 * Revision 1.10  1995/06/15  10:19:24  sm
 * changed scale to inverse
 *
 * Revision 1.9  1995/06/14  10:04:52  sm
 * used single precision sincos function
 *
 * Revision 1.8  1995/06/13  18:27:28  sm
 * modified scale flag stuff
 *
 * Revision 1.7  1995/06/12  17:55:21  sm
 * removed Mag !
 *
 * Revision 1.6  1995/06/08  13:06:28  sm
 * Added module id
 *
 * Revision 1.5  1995/06/07  11:44:59  sm
 * Minor change to Rotate()
 *
 * Revision 1.4  1995/06/06  11:47:30  sm
 * Added rotation routines which
 * include the quaternion stuff.
 *
 * Revision 1.3  1995/06/05  12:39:23  sm
 * Added scaling functions
 *
 * Revision 1.2  1995/06/02  17:09:09  sm
 * Added routines for sgl_modify_transform and sgl_translate
 *
 * Revision 1.1  1995/06/02  12:43:06  sm
 * Initial revision
 *
 *
 *****************************************************************************/

#define MODULE_ID MODID_DLTRANSF
#define DECLARING_AXES			/* we need this to make sgl.h work properly */

#include "sgl_defs.h"
#include "dlnodes.h"
#include "sgl_math.h"
#include "nm_intf.h"
#include "dlglobal.h"
#include "pvrosapi.h"
#include "sglmem.h"
#include "sgl_init.h"

/* 
// globally define the x, y and z axis
*/ 

#if 0

	#ifdef WIN32
	#define DLLExport extern __declspec (dllexport)
	#else
	#define DLLExport
	#endif

	DLLExport sgl_vector sgl_x_axis = {(float) 1.0, (float) 0.0, (float) 0.0};
	DLLExport sgl_vector sgl_y_axis = {(float) 0.0, (float) 1.0, (float) 0.0};
	DLLExport sgl_vector sgl_z_axis = {(float) 0.0, (float) 0.0, (float) 1.0};

#else

	#if WIN32
	#else

		sgl_vector sgl_x_axis = {(float) 1.0, (float) 0.0, (float) 0.0};
		sgl_vector sgl_y_axis = {(float) 0.0, (float) 1.0, (float) 0.0};
		sgl_vector sgl_z_axis = {(float) 0.0, (float) 0.0, (float) 1.0};

	#endif

#endif

/**************************************************************************
 * Function Name  : CheckMatrix
 * Inputs         :  
 * Outputs        : 
 * Input/Output	  : Mat		- transformation struct to be checked
 * Returns        : TRUE if matrix is OK otherwise FALSE
 * Global Used    : None
 * Description    : Check to see if a matrix and its inverse when multiplied
					result in an identity matrix.
 *				   
 **************************************************************************/
#if DEBUG
int		CheckMatrix(TRANSFORM_STRUCT * Mat)
{
	int Result;

	int i, j, k;

	float sum;

	Result = TRUE;

	/*
	   Check the rotation part of the
	   matrix
	*/
	for(i = 0; i < 3; i ++)
		for(j = 0; j < 3; j ++)
		{
			sum = 0.0f;

			for(k = 0; k < 3; k ++)
			 	sum +=  Mat->mat[i][k] * Mat->inv[k][j];

			if(i==j)
				Result = Result && (sfabs(sum - 1.0f) < 0.001f); /* have multiplied error by ten because we now use only floats */
			else
				Result = Result && (sfabs(sum) < 0.001f); /* same here */

		}

	if (!Result)
		sum =0.0f; /* dummy*/

	/*
	   check the scaling part
	*/

	for(i = 0; i < 3; i ++)
	{
			sum = 0.0f;

			for(k = 0; k < 3; k ++)
				sum += Mat->mat[i][k]* Mat->inv[k][3];

			sum += Mat->mat[i][3];

			Result = Result && (sfabs(sum) < 0.001f); /* and here !! */

	}

	if (!Result)
		sum =0.0f; /* dummy*/

	return Result;
}

#endif


/**************************************************************************
 * Function Name  : DlCompleteCurrentTransform
 * Inputs         : None
 * Outputs        : None
 * Input/Output	  : None
 * Returns        : None
 * Global Used    : Current Transform Node pointer.
 * Description    : Does any tidying up to complete the current transform
 *					At present, this just means setting the current transform
 *					node pointer to NULL, since each transform operation is self
 *					contained, it currently doesn't need any tidying up.
 *
 **************************************************************************/

void DlCompleteCurrentTransform(void)
{
	dlUserGlobals.pCurrentTransform = NULL;
}



/**************************************************************************
 * Function Name  : SetIdentityMatrix
 * Inputs         :  
 * Outputs        : 
 * Input/Output	  : Mat		- transformation struct to be initialised
 * Returns        : None
 * Global Used    : None
 * Description    : Initialises given transform to an identity marix
 *					
 *				   
 **************************************************************************/

void SetIdentityMatrix(TRANSFORM_STRUCT * Mat)
{

	static TRANSFORM_STRUCT  Identity =   
	{
		/* set scale flag to no_scale */ 	
		no_scale,
		/* set rescale values to Unity*/ 
		1.0f, 1.0f,

		/* hasn't had any negative scaling applied ...*/
		0,

		/*
		// Define the matrices
		*/
		{ { 1.0f, 0.0f, 0.0f, 0.0f},
		  { 0.0f, 1.0f, 0.0f, 0.0f},
		  {0.0f, 0.0f, 1.0f, 0.0f}},

		/* inverse of matrix */
		{ { 1.0f, 0.0f, 0.0f, 0.0f}, 
		  { 0.0f, 1.0f, 0.0f, 0.0f},
		  { 0.0f, 0.0f, 1.0f, 0.0f}}

	};
	
	*Mat = Identity;
}



/**************************************************************************
 * Function Name  : Translate  LOCAL ROUTINE ONLY
 * Inputs         : x,y,z : Translation in x, y and z  
 * Outputs        : 
 * Input/Output	  : Mat - transformation struct to which translation is
						  is being added.
 * Returns        : None
 * Global Used    : None
 * Description    : Adds translation to a given transformation matrix.
 *					
 *				   
 **************************************************************************/

static void Translate(float x, float y, float z, TRANSFORM_STRUCT * Mat)
{

	Mat->mat[0][3] += Mat->mat[0][0] * x + Mat->mat[0][1] * y + Mat->mat[0][2] * z;	
	Mat->mat[1][3] += Mat->mat[1][0] * x + Mat->mat[1][1] * y + Mat->mat[1][2] * z;	
	Mat->mat[2][3] += Mat->mat[2][0] * x + Mat->mat[2][1] * y + Mat->mat[2][2] * z;	


	Mat->inv[0][3] -= x;	
	Mat->inv[1][3] -= y;	
	Mat->inv[2][3] -= z;	

}





/**************************************************************************
 * Function Name  : RangeCheck
 * Inputs         : x: given scale value  
 * Outputs        : 
 * Input/Output	  :
						  
 * Returns        : clipped scale value
 * Global Used    : None
 * Description    : Clips scale parameters if too large to
					prevent silly values in matrices
 *					
 *				   
 **************************************************************************/

#define  MaxScaleVals (1.0E9f)
#define  MinScaleVals (1.0E-9f)


static float RangeCheck(float x)
{

	if(x > MaxScaleVals)
	{
		/*
		 check the ranges of the values
		 if too large
		*/

		x =  MaxScaleVals;
	}
	else if(x < - MaxScaleVals)
	{
		/*
		  if too large and negative
		*/

		x = - MaxScaleVals;
	}
	else if(( x < MinScaleVals) && (x > -MinScaleVals))
	{
		/*
		   if too close to zero
		*/

		if(x < 0.0f)
			x = - MinScaleVals;
		else
			x = MinScaleVals;
	}

	return (float) x;
}




/**************************************************************************
 * Function Name  : Scale   LOCAL ROUTINE ONLY
 * Inputs         : x,y,z : Scaling in x, y and z  
 * Outputs        : 
 * Input/Output	  : Mat - transformation struct to which scaling is
						  is being added.
 * Returns        : None
 * Global Used    : None
 * Description    : Scale matrix by the given values in x,y, and z	directions
 					but try to make sure we will still have an inverse matrix.

 					Note that this produces a matrix which is equivalent to applying
 					the scaling to the vector then applying existing matrix.
 *					
 *				   
 **************************************************************************/

static void Scale(float x, float y,  float z,TRANSFORM_STRUCT * Mat)
{

	int i;
	float invX, invY, invZ;
	float	xfab,yfab,zfab;
	sgl_bool has_neg_scaling;

	/* make sure scale values aren't ridiculously large */

	x = RangeCheck(x);
	y = RangeCheck(y);
	z = RangeCheck(z);

	/*
	// Determine if the scaling flips one of the axes. Note we are
	// only interested if there are an ODD number of negative
	// scales.
	*/
	has_neg_scaling =  (x < 0.0f);
	has_neg_scaling ^= (y < 0.0f);
	has_neg_scaling ^= (z < 0.0f);

	Mat->has_neg_scaling ^= has_neg_scaling;

	/*
	   	optimising division, when it has to be done
       	by an inverse and a multiply anyway....
	*/

	invX = 1.0f / x;
	invY = 1.0f / y;
	invZ = 1.0f / z;

	/*
	   	step through the columns, scaling the scale/rotation part of the
		matrix, by the relevant amounts
	*/

	for(i = 0; i < 3; i ++)
	{
	 	Mat->mat[i][0] *= x;
	 	Mat->mat[i][1] *= y;
	 	Mat->mat[i][2] *= z;

	 	Mat->inv[0][i] *= invX;
	 	Mat->inv[1][i] *= invY;
	 	Mat->inv[2][i] *= invZ;
	}

	/* translation part if inverse matrix */

	Mat->inv[0][3] *= invX;
	Mat->inv[1][3] *= invY;
	Mat->inv[2][3] *= invZ;



	/*
		add scaling information to current martix
	*/
	
	switch (Mat->scale_flag)
	{
		case arbitrary_scale: 	 
			/*
				matrix already contains non-uniform scaling 
			*/
			break;

		case uniform_scale:

			xfab=sfabs(x);	
			yfab=sfabs(y);	
			zfab=sfabs(z);	

			if ( ((xfab==yfab) && (xfab==zfab)) &&
				!( ((sgl_uint32) x | (sgl_uint32) y | (sgl_uint32) z) & 0x80000000))
			{
				/* uniform, non-negative, scaling values have been given */
				
				/* 
				// rescale is inverse of scaling value 
				// We are not intereseted in sign
				*/
	   			Mat->rescale/=xfab;
	   			Mat->invrescale*=xfab;
			}
			else
			{
				/* non-uniform scaling values have been given */		

				Mat->scale_flag = arbitrary_scale;
			}

			break;

 		case no_scale:

			xfab = sfabs(x);				
			yfab = sfabs(y);	
			zfab = sfabs(z);	

			if ((xfab==yfab) && (xfab==zfab))
			{
			/* uniform scaling, non-negative, values have been given */
									
				if (xfab != 1.0f)
				{				
	   				Mat->scale_flag = uniform_scale;					 
	   				Mat->rescale	= 1.0f / xfab;
	   				Mat->invrescale	= xfab;
				} 

			/*
			// otherwise all values have a magnitude if 1.0 and we do nothing 
			*/
			}
			else
			{
				/* non-uniform scaling vaues have been given */		

				Mat->scale_flag = arbitrary_scale;
			}

			break;

	}


}





/**************************************************************************
 * Function Name  : InternalMultiply LOCAL ROUTINE ONLY
 * Inputs         : rot - 	rotation to be added to matrix
					irot -	inverse of rotation  
 * Outputs        :  
 * Input/Output	  : Source - Current matrix
					Result - resulting matrix
						  
 * Returns        : 
 * Global Used    : None
 * Description    : Internal rotation multiply routine 
 *					
 *				   
 **************************************************************************/


static void InternalMultiply(TRANSFORM_STRUCT * Result, /*result*/

						 float rot[3][3],
						 float irot[3][3],

						 TRANSFORM_STRUCT * Source)   /*source*/
{
	int Row, Col;
	/*
	   	local values used to prevent aliasing problems - eg
		if the result is also one of the operands. Thus we
		put the result in here, and then copy it to the real
		result matrix.
 	*/

	TRANSFORM_STRUCT  Local;

	for(Row = 0; Row < 3; Row ++)
	{
		for(Col = 0; Col < 3; Col ++)
		{
			/*
				unroll the innermost loop to do sr bit
			*/

			Local.mat[Row][Col]=
				Source->mat[Row][0] * rot[0][Col] +
				Source->mat[Row][1] * rot[1][Col] +
				Source->mat[Row][2] * rot[2][Col];


			/*
				and also for inverse as well
			*/
			Local.inv[Row][Col] =
				irot[Row][0] * Source->inv[0][Col] +
				irot[Row][1] * Source->inv[1][Col] +
				irot[Row][2] * Source->inv[2][Col];

		};

		/*
			Do the translation part. The rotation doesn't affect
			the translation.
		*/

		Local.mat[Row][3] = Source->mat[Row][3];

		/*
			And the inverse translation
		*/

		Local.inv[Row][3] = 
   			irot[Row][0] * Source->inv[0][3] +
			irot[Row][1] * Source->inv[1][3] +
			irot[Row][2] * Source->inv[2][3];


	}


	Local.rescale 	= Source->rescale;
	Local.invrescale= Source->invrescale;
	Local.has_neg_scaling = Source->has_neg_scaling;
	Local.scale_flag = Source->scale_flag;


	/*
		now copy these back to the original params
		(we can do this because it is a structure)
	*/

	*Result = Local;

}




/**************************************************************************
 * Function Name  : Rotate   Local Routine ONLY
 * Inputs         : axis -  axis about which rotation is to take place
					theta - angle to rotate by
 * Outputs        :  
 * Input/Output	  : Mat - transformation struct to which rotation
						  is being added.
 * Returns        : 
 * Global Used    : None
 * Description    : Add rotation information to given rotation matrix
					by first creating a quaternian, converting this to
					matrix form then added this to the given matrix
					structure using a matrix multiply. 
 *					
 *				   
 **************************************************************************/


static void Rotate(sgl_vector axis, float theta, TRANSFORM_STRUCT * Mat)
{

	float halfTheta;

 	float  sinHalfTheta;
 	float  cosHalfTheta;

	float rot[3][3]; 
	float irot[3][3];

	
	float W,X,Y,Z;

	float Tm1,Tm2,Tm3,Tm4,Tm5,TmNorm;

	/* convert to quaternian */

	halfTheta=theta*0.5f;



	/*	Using double sin and cos  */ 
/*	cosHalfTheta = (float) cos(halfTheta);
	sinHalfTheta = (float) sin(halfTheta);
*/


	/* using simons groovy sincos function */ 

	s_sincos(halfTheta,&sinHalfTheta,&cosHalfTheta);


	W = cosHalfTheta;
	X = sinHalfTheta*axis[0];
	Y = sinHalfTheta*axis[1];
	Z = sinHalfTheta*axis[2];
	

	/* convert quaternian into Matrix form */ 

	Tm1 = X*X;
	Tm2 = Y*Y;
	Tm3 = Z*Z;
	Tm4 = Tm2+Tm3;

	TmNorm=W*W + Tm1 + Tm4;  /* Normalising term */

	if (TmNorm!=0.0f)
		Tm5 = 2.0f / TmNorm; 
	else
		Tm5 = 0.0f;	


	rot[0][0] = 1.0f - Tm5*Tm4;
	rot[1][1] = 1.0f - Tm5*(Tm1+Tm3);    
	rot[2][2] = 1.0f - Tm5*(Tm1+Tm2);

	Tm1 = Tm5*X;
	Tm2 = Tm5*Y;
	Tm4 = Tm5*Z*W;
	Tm5 = Tm1*Y; 

	rot[0][1] = Tm5 - Tm4;
	rot[1][0] = Tm5 + Tm4;

	Tm4 = Tm2*W;
	Tm5 = Tm1*Z;

	rot[0][2] = Tm5 + Tm4;
	rot[2][0] = Tm5 - Tm4;


	Tm4 = Tm1*W; 
	Tm5 = Tm2*Z;

	rot[1][2] = Tm5 - Tm4;
	rot[2][1] = Tm5 + Tm4;


	/* the inverse of a rotation matrix is its transpose */

	irot[0][0] = rot[0][0];
	irot[1][0] = rot[0][1];
	irot[2][0] = rot[0][2];

	irot[0][1] = rot[1][0];
	irot[1][1] = rot[1][1];
	irot[2][1] = rot[1][2];

	irot[0][2] = rot[2][0];
	irot[1][2] = rot[2][1];
	irot[2][2] = rot[2][2];


	InternalMultiply (Mat,  rot, irot, Mat);				 
	
}




/**************************************************************************
 * Function Name  : InitTransformNode
 * Inputs         : name  			- transform name 
 * Outputs        : 
 * Input/Output	  : pTranNode		- transformation node being initialised
 * Returns        : None
 * Global Used    : None
 * Description    : Sets the node header fields and the transform field to
 *					an identity matrix  
 *				   
 **************************************************************************/



void InitTransformNode(TRANSFORM_NODE_STRUCT * pTranNode,int name)
{

	pTranNode->node_hdr.n16_node_type = (sgl_int16) nt_transform;
	pTranNode->node_hdr.n16_name	  = (sgl_int16) name;
	pTranNode->node_hdr.next_node	  = NULL;

	SetIdentityMatrix(&(pTranNode->transform));

}





/**************************************************************************
 * Function Name  : sgl_create_transform
 * Inputs         : generate_name 
 * Outputs        : 
 * Input/Output	  : 
 * Returns        : if generate_name is TRUE and function is successful
					transform name is returned otherwise err is returned
					if generate_name is FALSE return value has no meaning
 * Global Used    : dlUserGlobals structure
 * Description    : adds a transformation node to the display list
					and initialised it to an identity matrix.
 *
 **************************************************************************/


int	CALL_CONV sgl_create_transform( sgl_bool generate_name )
{
	int error = sgl_no_err;
	int name  = NM_INVALID_NAME;

	TRANSFORM_NODE_STRUCT * pNode;

	/*	
		Initialise sgl if this hasn't yet been done		
	*/
#if !WIN32
	if(SglInitialise() != 0)
	{
		/*
			We failed to initialise sgl
		*/

		error = sgl_err_failed_init;
		SglError(error);

		return error;
	}
#endif
	/*
	   Tidy up current transforms etc...
	*/
/*	DlCompleteCurrentMaterial() */

	DlCompleteCurrentConvex();

	/* 
		Create transform node 
	*/


	pNode = NEW(TRANSFORM_NODE_STRUCT);
	if(pNode == NULL)
	{
		/*
			Abort	   
		*/
		error = sgl_err_no_mem;
		SglError(error);

		return error;
	} 


	/*
		If we need a name, generate one, adding the item to
		the name table at the same time.
	*/

	if (generate_name)
	{
		name = AddNamedItem(dlUserGlobals.pNamtab,
			   				pNode,
							nt_transform);
		/*
		 	If there were no free spaces, then the name will
		 	contain an error value (i.e.negative)
		
			In this situation TIDY UP and ABORT.
		*/
		if(name < 0)
		{
			error = name;
			name = NM_INVALID_NAME;

			SGLFree(pNode);
			SglError(error);
			return error;
		}
	} /*end if generate name */



	InitTransformNode(pNode, name);

	AppendNodeToList(dlUserGlobals.pCurrentList, pNode);


	/* Newly created transform node becomes the current transform */

	dlUserGlobals.pCurrentTransform = pNode;


	SglError(error);

	return name;
}



/**************************************************************************
 * Function Name  : sgl_modify_transform
 * Inputs         : name   			- name of transform
					clear_transform - resets transform to an identity matrix
									  + resets scale stuff
 * Outputs        : NONE
 * Input/Output	  : NONE
 * Returns        : NONE
 * Global Used    : dlUserGlobals structure
 * Description    : Sets the current transform to which following transform
					operations are applid to. 
 *
 **************************************************************************/
 
void CALL_CONV sgl_modify_transform( int name, sgl_bool clear_transform )
{

	TRANSFORM_NODE_STRUCT * pNode;

  	/*	
		Initialise sgl if this hasn't yet been done		
	*/
#if !WIN32
	if(SglInitialise() != 0)
	{
		/*
			We failed to initialise sgl
		*/
		SglError(sgl_err_failed_init);
		return;
	}
#endif

	/* Make sure that given name is the name of a transform */

    if ( GetNamedItemType(dlUserGlobals.pNamtab, name) != nt_transform )
	{
		/* the given name is invalid */

    	SglError(sgl_err_bad_name);
		return; 
	}


	/*
	   Tidy up current Convex primitive
	*/

	DlCompleteCurrentConvex();


	/*	get reference to transform from the name table */

    pNode = GetNamedItem(dlUserGlobals.pNamtab, name);


	/*	reset the transformation if requested */

	if (clear_transform)
		SetIdentityMatrix(&(pNode->transform));


	/* the named transform becomes the current transform */

	dlUserGlobals.pCurrentTransform = pNode;


	SglError(sgl_no_err);

}





/**************************************************************************
 * Function Name  : sgl_translate
 * Inputs         : x , y , z 	- Translation in x, y and z.
 * Outputs        : NONE
 * Input/Output	  : NONE
 * Returns        : NONE
 * Global Used    : dlUserGlobals structure
 * Description    : Adds translation x, y and z to the current transform
					If there is no current transform, one is created
					and added to the current list.
 *
 **************************************************************************/

void CALL_CONV sgl_translate( float x, float y, float z )
{
	TRANSFORM_NODE_STRUCT * pNode;

	/*	
		Initialise sgl if this hasn't yet been done		
	*/
#if !WIN32
	if(SglInitialise() != 0)
	{
		/*
			We failed to initialise sgl
		*/
		SglError(sgl_err_failed_init);

		return;
	}
#endif


	/* Complete current convex primitive */

	DlCompleteCurrentConvex();


	if (dlUserGlobals.pCurrentTransform == NULL) /* there is no current transformation */
	{
		/* Create a new transformation node (unnamed) */
		pNode = NEW(TRANSFORM_NODE_STRUCT);

   		if(pNode == NULL)
		{
			/*
				Abort	   
			*/
			SglError(sgl_err_no_mem);
			return;
		} 

		InitTransformNode(pNode, NM_INVALID_NAME);

		AppendNodeToList(dlUserGlobals.pCurrentList, pNode);


		/* Newly created transform node becomes the current transform */

		dlUserGlobals.pCurrentTransform = pNode;
	}


	Translate(x, y, z, &(dlUserGlobals.pCurrentTransform->transform));

	SglError(sgl_no_err);

}




void CALL_CONV sgl_scale( float x, float y, float z )
{

	TRANSFORM_NODE_STRUCT * pNode;

	/*	
		Initialise sgl if this hasn't yet been done		
	*/

#if !WIN32
	if(SglInitialise() != 0)
	{
		/*
			We failed to initialise sgl
		*/
		SglError(sgl_err_failed_init);

		return;
	}
#endif

	/* Complete current convex primitive */

	DlCompleteCurrentConvex();


	if (dlUserGlobals.pCurrentTransform == NULL) /* there is no current transformation */
	{
		/* Create a new transformation node (unnamed) */
		pNode = NEW(TRANSFORM_NODE_STRUCT);

   		if(pNode == NULL)
		{
			/*
				Abort	   
			*/
			SglError(sgl_err_no_mem);
			return;
		} 

		InitTransformNode(pNode, NM_INVALID_NAME);

		AppendNodeToList(dlUserGlobals.pCurrentList, pNode);


		/* Newly created transform node becomes the current transform */

		dlUserGlobals.pCurrentTransform = pNode;
	}
	else
		pNode = dlUserGlobals.pCurrentTransform;


   	Scale(x, y, z, &(pNode->transform));

	SglError(sgl_no_err);

}



void CALL_CONV sgl_rotate( sgl_vector axis, float angle )
{
	TRANSFORM_NODE_STRUCT * pNode;

	/*	
		Initialise sgl if this hasn't yet been done		
	*/

#if !WIN32
	if(SglInitialise() != 0)
	{
		/*
			We failed to initialise sgl
		*/
		SglError(sgl_err_failed_init);

		return;
	}
#endif

	/* Complete current convex primitive */

	DlCompleteCurrentConvex();


	if (dlUserGlobals.pCurrentTransform == NULL) /* there is no current transformation */
	{
		/* Create a new transformation node (unnamed) */
		pNode = NEW(TRANSFORM_NODE_STRUCT);

   		if(pNode == NULL)
		{
			/*
				Abort	   
			*/
			SglError(sgl_err_no_mem);
			return;
		} 

		InitTransformNode(pNode, NM_INVALID_NAME);

		AppendNodeToList(dlUserGlobals.pCurrentList, pNode);


		/* Newly created transform node becomes the current transform */

		dlUserGlobals.pCurrentTransform = pNode;
	}
	else
		pNode = dlUserGlobals.pCurrentTransform;

	Rotate(axis,angle,&(pNode->transform));

	SglError(sgl_no_err);
}


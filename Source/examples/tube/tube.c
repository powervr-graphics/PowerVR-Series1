/******************************************************************************
 * Name : tube.c
 * Title : simple tube example
 * Author : Stel Michael
 * Created : 26/07/1995
 *
 * Copyright : 1995-2022 Imagination Technologies (c)
 * License	 : MIT
 *
 * Description : first ever historic demo for SGL
 *
 * Platform : ANSI compatible
 *
 * Modifications:-
 * $Log $
 *
 *	
 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "math.h"
#include "../../sgl.h"


#define NAMED_ITEM			TRUE
#define UNAMED_ITEM			FALSE

#define COLOUR				TRUE
#define GREY				FALSE

#define SHADOW_LIGHT		TRUE
#define NO_SHADOWS			FALSE

#define INVISIBLE			TRUE
#define	VISIBLE				FALSE

  
#define	LENS_50MM			4.0f

#define PI	3.142f


int	main()
{
	int frame;
	int tubeName;
	int logicalDevice;
	int viewport1;
	int transformName;
	int materialName;
	int camera1;
	
	sgl_colour	lightGreen	=	{0.3f,1.0f,0.3f};
	sgl_colour	White		=	{1.0f,1.0f,1.0f};
	sgl_colour	darkGrey	=	{0.25f,0.25f,0.25f};
	sgl_colour	mediumGrey	=	{0.5f,0.5f,0.5f};
	sgl_vector	lightDir	=	{1.0f,-1.0f,1.0f};
	sgl_vector	lightPos	=	{0.0f,0.0f,0.0f};
	int		lightCon  = 32;	

	int PntLight1;
	int lightTrans;

	sgl_vector	topPnt		=	{0.0f,35.0f,0.0f};
	sgl_vector	topNorm		=	{0.0f,1.0f,0.0f};
	sgl_vector	botPnt		=	{0.0f,-35.0f,0.0f};
	sgl_vector	botNorm		=	{0.0f,-1.0f,0.0f};

	sgl_vector	sidePnt;
	sgl_vector	sideNorm;
	int 	tubeSides;
	float	tubeRadius,tAngle,angleStep;

	float	Angle,Inc;


	/* 
	// create screen device 0 with 640x480 screen resolution 
	// and 24 bit colour and no double buffering 
	*/ 

	logicalDevice = sgl_create_screen_device(0,640,480,sgl_device_24bit,FALSE);

	if (logicalDevice<0)
	{
		fprintf(stderr,"ERROR %01d: Failed to Open Screen Device\n",logicalDevice);
		exit(1);	
	}

	
	/*
	// Set up a viewport smaller than the device , that is set so the image 
	// is scaled to it
	*/
	viewport1 = sgl_create_viewport(logicalDevice,20,0,403,383,
												20, 0, 183, 183);
	/*
	// Because viewport sizes may have hardware, check what we were
	// given, and reset the camera rectangle accordingly
	*/
	if(viewport1 >= 0)
	{
		int left, top, right, bottom;
		float cleft, ctop, cright, cbottom;
		float ysize, xmiddle;

		sgl_get_viewport(viewport1, &left, &top, &right, &bottom,
								&cleft, &ctop, &cright, & cbottom);

		/*
		// Compute the largest centred square for the camera
		*/
		ysize = (bottom - top) / 2.0f;
		xmiddle = ((float) right + left) / 2.0f;

		sgl_set_viewport(viewport1, left, top, right, bottom,
								xmiddle - ysize, top, xmiddle + ysize, bottom);
		
	}
	else
	{
		fprintf(stderr,"ERROR %01d: Failed to Create Viewport\n",viewport1);
		exit(1);	
	}


	/*************************************************/
	/*************************************************/
	/*************************************************/


	/* 	
	// Put camera in a list so that its positioning transformation does not affect 
	// the tubes position 
	*/

	sgl_create_list(UNAMED_ITEM,TRUE,FALSE);

		sgl_create_transform(UNAMED_ITEM);   /* this could be omitted */
		sgl_translate(0.0f,0.0f,-200.0f);

		/*	Add Camera with the zoom factor of a 50mm lens, an aspect 
			ratio of 1, a front clipping 
			plane at 10.0, and no background clipping plane */


		camera1 = sgl_create_camera(LENS_50MM,10.0f,0.0f);

	/* go back up the list */
	
	sgl_to_parent();
	
	/*************************************************/
	/*************************************************/
	/*************************************************/
	/*************************************************/
		
	/*	
	// Ambient colour is set to a grey slightly
	// lighter than default. Note that only the red component is relevent
	// for grey lights .This is also set as an absolute level
	*/

	sgl_create_ambient_light(UNAMED_ITEM,darkGrey,GREY,FALSE);


	/*	
	// A single spotlight is added with a concentration of 32 and is positioned to 
	// shine
	// down from the top left onto the tube 
	*/

	PntLight1 = sgl_create_point_light(NAMED_ITEM,White, 
							FALSE, 
							lightDir,lightPos, 0,
							!SHADOW_LIGHT, FALSE);
  	

	/*************************************************/
	/*************************************************/
	/*************************************************/
	/*************************************************/
	
 	/*
	// Position Info for the tube...
	*/

	sgl_create_list(UNAMED_ITEM, TRUE, FALSE);
	
	/* 
	// create an unnamed transform then set it to translate and rotate 
	// about the x-axis 
	*/

	sgl_create_transform(UNAMED_ITEM);   /* this could be omitted */
	sgl_translate(0.0f,5.0f,0.0f);
	sgl_rotate(sgl_x_axis,(PI/5.0f));
	sgl_rotate(sgl_y_axis,(PI/5.0f));

	/* 
	// create a named transform which COULD be used later to rotate the tube
	*/	
	transformName = sgl_create_transform(NAMED_ITEM); 

	/* 
	// set tubes material properties 
	*/
	materialName = sgl_create_material(NAMED_ITEM, FALSE); /* not local */
	sgl_set_diffuse(lightGreen);
	sgl_set_specular(lightGreen,0); /* turn specular reflectivity off */

  
	/*	
	// build up tube by adding two invisible planes for the top and bottom and
	// 32 visible planes for the actual tube 
	*/ 

	tubeName = sgl_create_convex(NAMED_ITEM);
	
	sgl_add_simple_plane(topPnt,topNorm,INVISIBLE); 
	sgl_add_simple_plane(botPnt,botNorm,INVISIBLE); 


	/* Add the sides of the tube using 32 calls to sgl_add_simple_plane */

#define NUMSIDES 12
	tubeRadius=30.0f;
	tAngle=0.0f;
	angleStep=(2.0f*PI)/ ((float) NUMSIDES);

	for (tubeSides=0;tubeSides<NUMSIDES;tubeSides++)
	{	
		tAngle+=angleStep;

		sideNorm[0]=cos(tAngle);
		sideNorm[1]=0.0f;
		sideNorm[2]=sin(tAngle);
		
		sidePnt[0]=sideNorm[0]*tubeRadius;		
		sidePnt[1]=sideNorm[1]*tubeRadius;		
		sidePnt[2]=sideNorm[2]*tubeRadius;		
	
		sgl_add_simple_plane(sidePnt, sideNorm, VISIBLE);	
	}

	/*
	// FINISHED WITH TUBE
	*/
	sgl_to_parent();

	/*************************************************/
	/*************************************************/
	/*  Move the light round a bit					*/
	/*************************************************/
	/*************************************************/

	sgl_create_list(UNAMED_ITEM, TRUE, FALSE);
		/*
		// Move the light up, then allow for rotations
		*/
		sgl_translate(0.0f, 10.0f, 0.0f);

		lightTrans = sgl_create_transform(NAMED_ITEM); 
		
		sgl_position_light(	PntLight1 );

	sgl_to_parent();


	/*////////////////////////////
	// Animate the tube by rotating it about the global y-axis 
	//////////////////////////// */

#define MULTI_FRAMES 0
#if MULTI_FRAMES
#define NUM_FRAMES 10
#else
#define NUM_FRAMES 1
#endif

    for (frame=0; frame < NUM_FRAMES; frame++)
	{

		/*
		// Move the object
		*/
		sgl_modify_transform(transformName, TRUE);
		sgl_rotate(sgl_z_axis, frame * PI / 200.0f);


		/*
		// Move the light
		*/
		sgl_modify_transform(lightTrans, TRUE);
		sgl_rotate(sgl_y_axis, - frame * PI / 5.0f );
		sgl_translate(-10.0f, 0.0f, -30.0f);


		/* 	
		// Render a single image using the defined camera1 into
		// viewport1 
		*/
   		sgl_render(viewport1, camera1, FALSE);


		/*
		// Output the Simulator results
		*/
#if MULTI_FRAMES
		{
			char fname[30];
	
			sprintf(fname, "result%02d.bmp", frame);
			TexasWriteBMP(fname);
		}
#else
		TexasWriteBMP("result.bmp");
#endif
    }


	/* Close down the display device in an orderly fashion */
	

	sgl_delete_device(logicalDevice);	

	exit(0);

 }	  



/*--------------------------- End of File --------------------------------*/

/******************************************************************************
 * Name : fogtest.c
 * Title : Example showing different amounts of Fog
 * Author : Simon Fenney
 * Created : 17/08/1995
 *
 * Copyright : 1995-2022 Imagination Technologies (c)
 * License	 : MIT
 *
 * Description :
 *
 * Platform : ANSI compatible
 *
 * Modifications:-
 * $Log: fogtest.c,v $
 * Revision 1.2  1995/10/04  11:43:21  sjf
 * Changed UV coordinates to use 0..1
 *
 * Revision 1.1  1995/08/18  17:50:05  sjf
 * Initial revision
 *
 *	
 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include "math.h"
#include "../../sgl.h"


#define NAMED_ITEM			TRUE
#define UNAMED_ITEM			FALSE

#define COLOUR				TRUE
#define GREY				FALSE

#define SHADOW_LIGHT		TRUE
#define NO_SHADOWS			FALSE

#define SMOOTH_SHADING		TRUE
#define NO_SMOOTH_SHADING	FALSE

#define INVISIBLE			TRUE
#define	VISIBLE				FALSE

  
#define	LENS_50MM			4.0f

#define PI	3.142f


int	main()
{
	int logicalDevice;
	int viewport1;
	int camera1;

	int groundTex,skyTex;	
	sgl_intermediate_map First,Second;

	
	sgl_vector	lightDir	=	{1.0f,-1.0f,1.0f};

	int next_colour;

	sgl_colour	Grey	=	{0.5f,0.5f,0.5f};
	sgl_colour	DullRed	=	{0.7f,0.2f,0.2f};
	sgl_colour	White	=	{1.0f,1.0f,1.0f};

	sgl_2d_vec uv1,uv2,uv3;
	sgl_vector	groundPnt	= {0.0f,   -100.0f, 2000.0f };	
	sgl_vector	point2		= {-10.0f, -100.0f, 2000.0f };	
	sgl_vector	point3		= {0.0f,   -100.0f, 2010.0f };	

	sgl_vector	skyPnt		= {0.0f, 200.0f ,3000.0f};
	sgl_vector	skyPnt2		= {0.0f, 200.0f, 3010.0f };	
	sgl_vector	skyPnt3		= {-10.0f,200.0f, 3000.0f };	

	int i, j;

	float fog_density;



	#define NUM_COLOURS 6
	sgl_colour colours[NUM_COLOURS] = {{1.0f, 0.0f, 0.3f},
									   {0.2f, 0.6f, 0.9f},
									   {0.7f, 0.9f, 0.1f},
									   {0.9f, 0.5f, 0.0f},
									   {0.4f, 0.4f, 0.9f},
									   {0.0f, 0.7f, 0.9f}};

	/*
	// create screen device 0 with 640x480 screen resolution and 
	// 24 bit colour and no double buffering 
	*/ 

	logicalDevice = sgl_create_screen_device(0,640,480,sgl_device_24bit,FALSE);

	if (logicalDevice<0)
	{
		fprintf(stderr,"ERROR %01d: Failed to Open Screen Device\n",logicalDevice);
		exit(1);	
	}

	/*
	//	Define an initial viewport
	*/ 
	viewport1 = sgl_create_viewport(logicalDevice,0,0,640,480,
									80.0,0.0f,560.0f,480.0f);

	
	/* 	Put camera in a list so that its 
	//	positioning transformation does not affect 
	//	the cubes position 
	*/

	sgl_create_list(UNAMED_ITEM,TRUE,FALSE);
	sgl_translate(0.0f,0.0f,-200.0f);

	/*	Add Camera with the zoom factor of a 50mm lens, 
		an aspect ratio of 1, a front clipping 
		plane at 10.0, and no background clipping plane */

	camera1 = sgl_create_camera(LENS_50MM,
			  					10.0f,
								0.0f);



	/* go back up the list */
	
	sgl_to_parent();
	
		
	/*	
	// Add a light
	*/
	sgl_create_parallel_light (UNAMED_ITEM, Grey, 
								TRUE,
								lightDir,
								FALSE,
								FALSE);

	/*
	// Add a bunch of objects in a grid
	*/
	next_colour = 0;
	for(i = -1; i <= 1; i++)
	{
		for(j = -1; j <= 1; j++)
		{
			sgl_create_list(UNAMED_ITEM,TRUE,FALSE);

			/*
			// Position the object around the origin
			*/
			sgl_translate(i * 60.0f, j * 35.0f, 350.0f + j * 300.0f);

			/*
			// choose the next colour
			*/

			sgl_set_diffuse(colours[(next_colour++) % NUM_COLOURS]);

			/*
			// and create an object
			*/
			sgl_create_sphere (0, 10.0f, 3,FALSE, FALSE, FALSE);

			/* go back up the list */
			sgl_to_parent();

		}

	}


	/* load up and assign names to textures */

	First=ConvertBMPtoSGL("../textures/rivets.bmp",FALSE);
	groundTex=sgl_create_texture(sgl_map_16bit_mm,
								sgl_map_128x128,
								TRUE,TRUE,&First,NULL);


	Second=ConvertBMPtoSGL("../textures/cloudsda.bmp",FALSE);
	skyTex=sgl_create_texture(sgl_map_16bit_mm,
							sgl_map_128x128,
							TRUE,TRUE,&Second,NULL);


	/* Add the sky plane */

	sgl_create_list(UNAMED_ITEM,TRUE,FALSE);

		uv1[0]=0.0;
		uv1[1]=0.0;

		uv2[0]=1.0E-3f;
		uv2[1]=0.0;

		uv3[0]=0.0;
		uv3[1]=1.0E-3f;

		sgl_set_glow(White); 

		sgl_set_texture_effect(FALSE,FALSE,FALSE,TRUE);

		sgl_set_texture_map(skyTex,FALSE,FALSE);

		sgl_add_plane(skyPnt,skyPnt2,skyPnt3,
			VISIBLE,FALSE,NULL,NULL,NULL,TRUE,uv1,uv2,uv3); 

	sgl_to_parent();



	/* Add the ground plane */

	sgl_create_list(UNAMED_ITEM,TRUE,FALSE);

		uv1[0]=0.0;
		uv1[1]=0.0;

		uv2[0]=0.04;
		uv2[1]=0.0;

		uv3[0]=0.0;
		uv3[1]=0.04;

		sgl_set_glow(White); 

		sgl_set_texture_effect(FALSE,FALSE,FALSE,TRUE);

		sgl_set_texture_map(groundTex,FALSE,FALSE);

		sgl_add_plane(groundPnt,point2,point3,
			VISIBLE,FALSE, NULL, NULL,NULL,TRUE,uv1,uv2,uv3); 

	sgl_to_parent();


	/*
	// Go into a loop doing 10 viewports with different fog densities
	*/

	/*
	// make the initial fog absorb 4% of light  per unit distance
	*/
	fog_density = 0.04f; 
	for(i=0; i < 10; i++)
	{
		/*
		// Reset the camera fog density, decreasing the fog by a factor
		// each time
		*/
		sgl_set_fog(camera1, DullRed, fog_density);

		/*
		// Reduce the fog density to 55% of its original value each step
		*/
		fog_density *= 0.55f;


		/*
		// Set up the next viewport going across the screen to the right
		*/
		sgl_set_viewport(viewport1, 
				i * (640/10),  0, 
				(i+1)*(640/10) -1 ,480,
				80.0,0.0f,560.0f,480.0f);

		/*
		// And render it
		*/
		sgl_render(viewport1, camera1, FALSE);

	}

	/*write the image to disc*/

	TexasWriteBMP("result.bmp");

	/* Close down the display device in an orderly fashion */
	
	sgl_delete_device(logicalDevice);	

	exit(0);

 }	  



/*--------------------------- End of File --------------------------------*/

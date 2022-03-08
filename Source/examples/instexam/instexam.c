/******************************************************************************
 * Name : instexam.c
 * Title : Example showing use of instancing, and instance parameterisation
 * Author : Simon Fenney
 * Created : 16/09/1995
 *
 * Copyright : 1995-2022 Imagination Technologies (c)
 * License	 : MIT
 *
 * Description :
 *
 * Platform : ANSI compatible
 *
 * Modifications:-
 * $Log: instexam.c,v $
 * Revision 1.3  1995/10/13  12:43:40  sjf
 * Added new parameter to rectprism call.
 *
 * Revision 1.2  1995/10/04  11:53:22  sjf
 * Changed UV texture mapping coords to 0..1 (plus now uses newer
 * sphere routine).
 *
 * Revision 1.1  1995/09/16  19:42:04  sjf
 * Initial revision
 *
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

#define SEPARATE_LIST		TRUE

#define DONT_PRESERVE		FALSE
#define	PRESERVE			TRUE
  
#define	LENS_50MM			4.0f

#define PI	3.142f



void main(void)
{
	int logicalDevice;
	int camera1;

	int groundTex,skyTex;	
	sgl_intermediate_map First,Second;

	
	sgl_vector	lightDir	=	{0.5f,-1.0f,2.0f};
	int light1;

	sgl_colour	White	=	{1.0f,1.0f,1.0f};


	sgl_colour	Red		=	{0.8f,0.1f,0.1f};
	sgl_colour	Blue	=	{0.1f,0.1f,0.8f};

	/*
	// Instance names
	*/
	int RedInst, BlueInst;
	int BoxInst, SphereInst;

	int ExampleInstance;


	sgl_2d_vec uv1,uv2,uv3;
	sgl_vector	groundPnt	= {0.0f,   -50.0f, 2000.0f };	
	sgl_vector	point2		= {-10.0f, -50.0f, 2000.0f };	
	sgl_vector	point3		= {0.0f,   -50.0f, 2010.0f };	

	sgl_vector	skyPnt		= {0.0f, 200.0f ,3000.0f};
	sgl_vector	skyPnt2		= {0.0f, 200.0f, 3010.0f };	
	sgl_vector	skyPnt3		= {-10.0f,200.0f, 3000.0f };	

	int InstSubs[5][2];

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
	// Put camera in a list so that its 
	// positioning transformation does not affect 
	// the objects' positions
	*/

	sgl_create_list(UNAMED_ITEM,TRUE,FALSE);
		sgl_translate(0.0f,0.0f,-100.0f);

		/*	
		// Add Camera with the zoom factor of a 50mm lens, 
		// an aspect ratio of 1, a front clipping 
		// plane at 10.0, and no background clipping plane 
		*/

		camera1 = sgl_create_camera(LENS_50MM,
			  					10.0f,
								0.0f);

	/* go back up the list */
	
	sgl_to_parent();
	
		
	/*	
	// Add a light
	*/

	light1 = sgl_create_parallel_light (NAMED_ITEM,
							   White, 
								TRUE,
								lightDir,
								TRUE,
								FALSE);



	/* load up and assign names to textures */

	First=ConvertBMPtoSGL("../textures/rivets.bmp",FALSE);
	Second=ConvertBMPtoSGL("../textures/cloudsda.bmp",FALSE);

	groundTex=sgl_create_texture(sgl_map_16bit_mm,
								sgl_map_128x128,
								TRUE,TRUE,&First,NULL);
	skyTex=sgl_create_texture(sgl_map_16bit_mm,
							sgl_map_128x128,
							TRUE,TRUE,&Second,NULL);

	/* Add the sky plane */

	sgl_create_list(UNAMED_ITEM,TRUE,FALSE);

		/*
		// The sky shouldn't cast shadows, so switch them off for our light.
		// In fact, turn it off completely (the state will be restored when
		// we exit the local list)
		*/
		sgl_switch_light(light1, FALSE, FALSE, FALSE);

		uv1[0]=0.0;
		uv1[1]=0.0;

		uv2[0]=1.0E-3;
		uv2[1]=0.0;

		uv3[0]=0.0;
		uv3[1]=1.0E-3;

		sgl_set_glow(White); 

		sgl_set_texture_effect(FALSE,FALSE,FALSE,TRUE);

		sgl_set_texture_map(skyTex,FALSE,FALSE);

		sgl_add_plane(skyPnt,skyPnt2,skyPnt3,
			VISIBLE,FALSE,NULL,NULL, NULL, TRUE,uv1,uv2,uv3); 

	sgl_to_parent();



	/* Add the ground plane */

	sgl_create_list(UNAMED_ITEM,TRUE,FALSE);

		uv1[0]=0.0;
		uv1[1]=0.0;

		uv2[0]=0.04;
		uv2[1]=0.0;

		uv3[0]=0.0;
		uv3[1]=0.04;

		sgl_set_diffuse(White); 

		sgl_set_texture_effect(TRUE,TRUE,TRUE,TRUE);

		sgl_set_texture_map(groundTex,FALSE,FALSE);

		sgl_add_plane(groundPnt,point2,point3,
			VISIBLE,FALSE,NULL,NULL,NULL, TRUE,uv1,uv2,uv3); 

	sgl_to_parent();




	/* /////////////////////////////////////////////////////////
	// Create 2 instances.....
	///////////////////////////////////////////////////////// */

	/*
	// Create an "instance" that makes a red material. This is simply a 
	// separate list that we will reference later.
	//
	// NOTE: Dont preserve state as we want the colour to affect
	// objects after the instance is used.
	*/
	RedInst = sgl_create_list(NAMED_ITEM, DONT_PRESERVE, SEPARATE_LIST);
		sgl_set_diffuse(Red);

	/*
	// Create an "instance" that defines a blue material
	// NOTE: Dont preserve state as we want the colour to affect
	// objects after the instance is used.
	*/
	BlueInst = sgl_create_list(NAMED_ITEM, DONT_PRESERVE, SEPARATE_LIST);
		sgl_set_diffuse(Blue);
	

	/*
	// Create a "box" instance
	//
	// NOTE Preserve is not important here.
	*/
	BoxInst = sgl_create_list(NAMED_ITEM, DONT_PRESERVE, SEPARATE_LIST);
		sgl_create_rectprism (10.0f, 10.0f, 10.0f, FALSE, FALSE);

	/*
	// Create a "sphere" instance
	*/
	SphereInst = sgl_create_list(NAMED_ITEM, DONT_PRESERVE, SEPARATE_LIST);
		 sgl_create_sphere (0, 10.0f, 5, FALSE, FALSE, FALSE);


	/*
	// Create a Basic model instance of a Red Box
	//
	// With this instance, dont let it affect anything else by setting
	// the PRESERVE flag.
	*/
	ExampleInstance = sgl_create_list(NAMED_ITEM, PRESERVE, SEPARATE_LIST);
		sgl_use_instance(RedInst);
		sgl_use_instance(BoxInst);


	/* ////////////////////////////////////////
	// GO BACK TO THE END OF THE DEFAULT LIST (AND DONT CLEAR IT!)
	//////////////////////////////////////// */
	sgl_modify_list(SGL_DEFAULT_LIST, FALSE);



	/* ////////////////////////////////////////
	// Add a bunch of objects in a grid using instances and instance
	// substitutions
	//////////////////////////////////////// */
	sgl_create_list(UNAMED_ITEM,TRUE,FALSE);

		/*
		// position the first back a bit and to the "left"
		*/
		sgl_translate(0, 0, 70.0f);
		sgl_translate(-30.0, 0.0, 0.0f);

		/*
		// Use the default instance (Red Box)
		*/
		sgl_use_instance(ExampleInstance);

	
		/*
		// For demonstration purposes, save the current state (including
		// instance substitutions
		//
		// POINT A
		*/
		sgl_create_list(UNAMED_ITEM,TRUE,FALSE);
		
			/*
			// Set the instance replacements so that any reference to Red
			//  gets replaced with a reference to blue.
			*/
			InstSubs[0][0] = RedInst;
			InstSubs[0][1] = BlueInst;

			sgl_instance_substitutions(1, &InstSubs[0][0]);
			
			/*
			// Move a bit to the right
			*/
			sgl_translate(30.0f, 0.0f, 0.0f);

			/*
			// Now re-use the example instance, and the red will be
			// replaced by Blue
			*/
			sgl_use_instance(ExampleInstance);


			/*
			// Now ALSO specify that the cube is replaced by a sphere
			*/
			InstSubs[0][0] = BoxInst;
			InstSubs[0][1] = SphereInst;

			sgl_instance_substitutions(1, &InstSubs[0][0]);
			sgl_translate(30.0f, 0.0f, 0.0f);

			sgl_use_instance(ExampleInstance);
		
		/*
		// return to the parent which should restore the substitutions (AND
		// also the position )
		*/
		sgl_to_parent();


		/*
		// DO the instance again, which should be back to the way it
		// was originally.
		// Move a Down a bit (position beneath the original)
		*/
		sgl_translate(0.0f, -30.0f, 0.0f);
		sgl_use_instance(ExampleInstance);

	
	sgl_to_parent();



	/*
	// render the picture
	*/
	sgl_render(logicalDevice, camera1, FALSE);

	/*write the image to disc*/

	TexasWriteBMP("result.bmp");

	/* Close down the display device in an orderly fashion */
	
	sgl_delete_device(logicalDevice);	

	exit(0);

 }	  



/*--------------------------- End of File --------------------------------*/

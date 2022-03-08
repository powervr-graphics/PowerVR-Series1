/******************************************************************************
 * Name : texcube.c
 * Title : textured cube example
 * Author : Peter Leaback
 * Created : 26/07/1995
 *
 * Copyright : 1995-2022 Imagination Technologies (c)
 * License	 : MIT
 *
 * Description :
 *
 * Platform : ANSI compatible
 *
 * Modifications:-
 * $Log: hwsimtest.c,v $
 * Revision 1.1  1995/12/18  17:33:39  sjf
 * Initial revision
 *
 * Revision 1.3  1995/10/04  12:29:10  sjf
 * Changed UV coordinate system to 0..1
 * 2) newer version of add_plane with 3 smooth normals.
 *
 * Revision 1.2  1995/08/22  20:07:52  sjf
 * changed texture path.
 *
 * Revision 1.1  1995/07/30  22:47:39  sjf
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

  
#define	LENS_50MM			4.0f

#define PI	3.142f



/**************************************************************************
 * Function Name  : CreateTetrahedron (internal only)
 * Inputs         : 
 * Outputs        : 
 * Returns        : 
 * Global Used    : 
 * Description    : Generates a simple tetraheron (not a very good one!)
 *
 **************************************************************************/
static void CreateTetrahedron()
{
	#define THE_SIZE 90.0f
	sgl_vector BasePts[3]={ {-THE_SIZE, 0.0f, -THE_SIZE},
							{ THE_SIZE, 0.0f, -THE_SIZE},
							{ 0.0f, 0.0f,	  THE_SIZE/ 3.0f}};


	sgl_2d_vec uv1 = {0.0f, 0.0f};
	sgl_2d_vec uv2 = {0.0f, 1.0f};
	sgl_2d_vec uv3 = {1.0f, 0.5f};

	int i;

	sgl_vector TopPnt ={0.0f, THE_SIZE, 0.0f};

	/*
	// create a new convex object
	*/
	sgl_create_convex(FALSE);

	/*
	// Do the base
	*/
	sgl_add_plane(BasePts[0], BasePts[1], BasePts[2],
				  FALSE, 
				  FALSE, NULL, NULL, NULL,
				  TRUE,  uv1, uv2, uv3);

	/*
	// Do the sides
	*/
	for(i = 0; i < 3; i++)
	{
		sgl_add_plane(BasePts[i], TopPnt, BasePts[(i + 1)%3],
						FALSE, 
						FALSE, NULL, NULL, NULL,
						TRUE,  uv1, uv2, uv3);

	}

}



/**************************************************************************
 * Function Name  : CreateNGon (internal only)
 * Inputs         : 
 * Outputs        : 
 * Returns        : 
 * Global Used    : 
 * Description    : Generates a simple N Gon (not a very good one!)
 *
 **************************************************************************/
static void CreateNGon(int numSides)
{
	#define THE_SIZE 90.0f
	#define MAX_SIDES 10

	int i;

	sgl_vector BasePts[MAX_SIDES];


	sgl_2d_vec uv1 = {0.0f, 0.0f};
	sgl_2d_vec uv2 = {0.0f, 1.0f};
	sgl_2d_vec uv3 = {1.0f, 0.5f};

	sgl_vector TopPnt ={0.0f, THE_SIZE, 0.0f};


	/*
	// Set up the base pnts
	*/
	if(numSides < 3)
	{
		numSides = 3;
	}
	else if(numSides > MAX_SIDES)
	{
		numSides = MAX_SIDES;
	}

	/*
	// create the base points
	*/
	for(i = 0; i < numSides; i++)
	{
		float angle = i * 2.0f * 3.1415 / ((float) numSides);

		BasePts[i][0] = THE_SIZE * cos(angle);
		BasePts[i][1] = 0.0f;
		BasePts[i][2] = THE_SIZE * sin(angle);
	}


	/*
	// create a new convex object
	*/
	sgl_create_convex(FALSE);

	/*
	// Do the base
	*/
	sgl_add_plane(BasePts[0], BasePts[1], BasePts[2],
				  FALSE, 
				  FALSE, NULL, NULL, NULL,
				  TRUE,  uv1, uv2, uv3);

	/*
	// Do the sides
	*/
	for(i = 0; i < numSides; i++)
	{
		sgl_add_plane(BasePts[i], TopPnt, BasePts[(i + 1)%numSides],
						FALSE, 
						FALSE, NULL, NULL, NULL,
						TRUE,  uv1, uv2, uv3);

	}

}









int	main()
{
	int logicalDevice;
	int viewport1;
	int transformName;
	int materialName;
	int texName[4];
	int camera1;
	
	sgl_colour	lightGreen	=	{0.3f,1.0f,0.3f};
	sgl_colour	Blue		=	{0.2f,0.1f,0.95f};
	sgl_colour	Red			=	{0.9f,0.1f,0.1f};
	sgl_colour	White		=	{1.0f,1.0f,1.0f};
	sgl_colour	darkGrey	=	{0.25f,0.25f,0.25f};
	sgl_colour	mediumGrey	=	{0.5f,0.0f,0.0f};

	sgl_vector	lightDir	=	{1.0f,-1.0f,4.0f};

	sgl_vector	lightPos	=	{-100.0f,100.0f,-100.0f};
	int		lightCon  = 32;	

	sgl_vector	topPnt		=	{0.0f,25.0f,0.0f};
	sgl_vector	topNorm		=	{0.0f,1.0f,0.0f};
	sgl_vector	botPnt		=	{0.0f,-25.0f,0.0f};
	sgl_vector	botNorm		=	{0.0f,-1.0f,0.0f};

	sgl_vector	sidePnt;
	sgl_vector	sideNorm;

	sgl_intermediate_map Imap;
	sgl_2d_vec uv1,uv2,uv3;
	sgl_vector point2,point3;


	int i;

	/* create screen device 0 with 640x480 screen resolution and 24 bit colour and no double buffering */ 

	logicalDevice = sgl_create_screen_device(0,640,480,sgl_device_24bit,FALSE);

	if (logicalDevice<0)
	{
		fprintf(stderr,"ERROR %01d: Failed to Open Screen Device\n",logicalDevice);
		exit(1);	
	}

	
	/*	set the viewport for the opened device so that there is a 20 pixel border
		around the rendered image */ 

	viewport1 = sgl_create_viewport(logicalDevice,0,0,127 ,127 ,0.0f,0.0f,128.0f,128.0f);


	/* 	Put camera in a list so that its positioning transformation does not affect 
		the cubes position */

	sgl_create_list(UNAMED_ITEM,TRUE,FALSE);

	sgl_create_transform(UNAMED_ITEM);   /* this could be omitted */
	sgl_translate(0.0f,0.0f,-68.0f);

	/*	Add Camera with the zoom factor of a 50mm lens, an aspect ratio of 1, a front clipping 
		plane at 10.0, and no background clipping plane */

	camera1 = sgl_create_camera(LENS_50MM,10.0f,0.0f);


	/* go back up the list */
	
	sgl_to_parent();


	/*
	// Load some textures
	*/	
	Imap=ConvertBMPtoSGL("../textures/pan2.bmp",FALSE);
	texName[0]=sgl_create_texture(sgl_map_16bit_mm,sgl_map_128x128,
								TRUE,TRUE,&Imap,NULL);
	free(Imap.pixels);


	Imap=ConvertBMPtoSGL("../textures/rivets.bmp",FALSE);
	texName[1]=sgl_create_texture(sgl_map_16bit_mm,sgl_map_128x128,
								TRUE,TRUE,&Imap,NULL);
	free(Imap.pixels);


	Imap=ConvertBMPtoSGL("../textures/RD3.bmp",FALSE);
	texName[2]=sgl_create_texture(sgl_map_16bit_mm,sgl_map_256x256,
								TRUE,TRUE,&Imap,NULL);
	free(Imap.pixels);

	Imap=ConvertBMPtoSGL("../textures/glowy.bmp",FALSE);
	texName[3]=sgl_create_texture(sgl_map_16bit_mm,sgl_map_128x128,
								TRUE,TRUE,&Imap,NULL);
	free(Imap.pixels);


	/*	Ambient colour is set to a grey slightly lighter than default. 
		Note that only the red component is relevent
		for grey lights .This is also set as an absolute level*/

	/*sgl_create_ambient_light(NAMED_ITEM, White, GREY,FALSE);*/

	/*	
	// A single spotlight is added with a concentration of 32 and is 
	// positioned to shine down from the top left onto the cube 
	*/

	sgl_create_parallel_light(UNAMED_ITEM, White, GREY,lightDir,
						!SHADOW_LIGHT,NO_SMOOTH_SHADING);
  	

 
	sgl_create_transform(UNAMED_ITEM);   /* this could be omitted */


	/* set the material properties */
	
	sgl_set_diffuse(White);
	sgl_set_diffuse(Blue);
	sgl_set_ambient(Blue);
	sgl_set_specular(White,0); /* turn specular reflectivity off */

	
#define LOTS_OF_OBJECTS_TEST  0


#if LOTS_OF_OBJECTS_TEST 
#define TRANSLUCENT_LOTS 1

	sgl_set_background_colour(camera1,Red);

	for(i=0; i < 120; i ++)
	{
		#define OFFSET_SCALE 90.0f
		sgl_vector extraTrans;
		float scale;
		int j;

		/*
		// create random object
		*/
		for(j = 0; j < 3; j++)
		{
			extraTrans[j] = ((float)(sgl_rand() % 100)) / 100.0f;
		}
		sgl_set_diffuse(extraTrans);
		


	#if TRANSLUCENT_LOTS
		j = sgl_rand()%4;
		sgl_set_texture_map(texName[j],FALSE,FALSE);

		if(i % 30 == 0)
		{
			sgl_new_translucent();
		}

		j = sgl_rand()%1000;
		if(j > 990)
		{
			sgl_set_opacity( (float)(sgl_rand()%1000) / 1000.0f);
			sgl_set_opacity(0.50f);
		}
	#else	
		j = sgl_rand()%5;
		if(j != 4)
		{
			sgl_set_texture_map(texName[j],FALSE,FALSE);
		}

	#endif

		sgl_create_list(FALSE, TRUE, FALSE);

		for(j = 0; j < 3; j++)
		{
			extraTrans[j] = ((float)(sgl_rand() % 10000)) / 10000.0f * 
												  OFFSET_SCALE *2.0f;
		}

		sgl_translate(-OFFSET_SCALE + extraTrans[0] -40.0f,  
					-OFFSET_SCALE + extraTrans[1] + 40.0f,
					500.0f+ extraTrans[2]);


		sgl_rotate(sgl_x_axis,((float)(sgl_rand() % 10000)) / 100.0f);

		sgl_rotate(sgl_y_axis,((float)(sgl_rand() % 10000)) / 100.0f);

		scale = 0.5f + ((float)(sgl_rand() % 100))/ 200.0f;
		sgl_scale(scale, scale, scale);

		CreateNGon(sgl_rand()%5 + 3);

		sgl_to_parent();
	}

#else
	sgl_set_background_colour(camera1,lightGreen);

	sgl_set_texture_map(texName[0],FALSE,FALSE);

	/*
	// create 4 tetrahedrons
	//
	// 2 opaque, 2 translucent
	*/
	sgl_create_list(FALSE, TRUE, FALSE);
		sgl_translate(-50.0f,  0.0f,520.0f+64.0);
		sgl_translate(-50.0f,  120.0f,0.0f);
		/*sgl_translate( 50.0f,  0.0f,-600.0f);*/

		/*sgl_rotate(sgl_x_axis,(PI/2.5f));*/
		sgl_rotate(sgl_y_axis,(PI/8.0f));

		CreateTetrahedron();

	sgl_to_parent();
#if 1

	sgl_create_list(FALSE, TRUE, FALSE);
		sgl_translate(+30.0f,  40.0f, 500.0f+64.0);

		sgl_rotate(sgl_x_axis,(PI/2.5f));
		sgl_rotate(sgl_z_axis,(PI/8.0f));

		CreateTetrahedron();

	sgl_to_parent();

#endif

#if 1

	/*
	// Set opacity to 1/2
	*/
	sgl_set_opacity(0.75f);
	sgl_set_diffuse(Red);
	sgl_set_ambient(Red);

	sgl_create_list(FALSE, TRUE, FALSE);
		sgl_translate(+10.0f,  10.0f, 480.0f+64.0);

		/*sgl_rotate(sgl_x_axis,(PI/2.5f));*/
		/*sgl_rotate(sgl_z_axis,(PI/8.0f));*/

		CreateTetrahedron();

	sgl_to_parent();
#endif

#if 1

	sgl_create_list(FALSE, TRUE, FALSE);
		sgl_translate(-50.0f,  -40.0f, 480.0f+64.0);

		sgl_rotate(sgl_x_axis,(PI/2.5f));
		/*sgl_rotate(sgl_z_axis,(PI/8.0f));*/

		CreateTetrahedron();

	sgl_to_parent();
#endif
#endif




	sgl_render(viewport1, camera1, FALSE);

	/*write the image to disc*/

	TexasWriteBMP("result.bmp");

	/* Close down the display device in an orderly fashion */
	

	sgl_delete_device(logicalDevice);	

	exit(0);

}	  



/*--------------------------- End of File --------------------------------*/

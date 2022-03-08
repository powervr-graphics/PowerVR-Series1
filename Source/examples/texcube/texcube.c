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
 * $Log: texcube.c,v $
 * Revision 1.4  1996/03/23  08:07:24  sjf
 * Changed to new API
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


int	main()
{
	int logicalDevice;
	int viewport1;
	int transformName;
	int materialName;
	int texName,texName1;
	int camera1;
	
	sgl_colour	lightGreen	=	{0.3f,1.0f,0.3f};
	sgl_colour	White		=	{1.0f,1.0f,1.0f};
	sgl_colour	darkGrey	=	{0.25f,0.25f,0.25f};
	sgl_colour	mediumGrey	=	{0.5f,0.0f,0.0f};
	sgl_vector	lightDir	=	{1.0f,-1.0f,1.0f};
	sgl_vector	lightPos	=	{-100.0f,100.0f,-100.0f};
	int		lightCon  = 32;	

	sgl_vector	topPnt		=	{0.0f,25.0f,0.0f};
	sgl_vector	topNorm		=	{0.0f,1.0f,0.0f};
	sgl_vector	botPnt		=	{0.0f,-25.0f,0.0f};
	sgl_vector	botNorm		=	{0.0f,-1.0f,0.0f};

	sgl_vector	sidePnt;
	sgl_vector	sideNorm;

	sgl_intermediate_map First,Second;
	sgl_2d_vec uv1,uv2,uv3;
	sgl_vector point2,point3;


	/* create screen device 0 with 640x480 screen resolution and 24 bit colour and no double buffering */ 

	logicalDevice = sgl_create_screen_device(0,640,480,sgl_device_24bit,FALSE);

	if (logicalDevice<0)
	{
		fprintf(stderr,"ERROR %01d: Failed to Open Screen Device\n",logicalDevice);
		exit(1);	
	}

	
	/*	set the viewport for the opened device so that there is a 20 pixel border
		around the rendered image */ 

	viewport1 = sgl_create_viewport(logicalDevice,0,0,640,480,80.0,0.0f,560.0f,480.0f);


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
	

	First=ConvertBMPtoSGL("../textures/rtoucan.bmp",FALSE);
	Second=ConvertBMPtoSGL("../textures/rivets.bmp",FALSE);
	
	texName=sgl_create_texture(sgl_map_16bit_mm,sgl_map_128x128,
								TRUE,TRUE,&First,NULL);
	texName1=sgl_create_texture(sgl_map_16bit_mm,sgl_map_128x128,
								TRUE,TRUE,&Second,NULL);

		
	/*	Ambient colour is set to a grey slightly lighter than default. 
		Note that only the red component is relevent
		for grey lights .This is also set as an absolute level*/

	sgl_create_ambient_light(NAMED_ITEM,mediumGrey,FALSE);

	sgl_set_ambient ( White);
	/*	A single spotlight is added with a concentration of 32 and is positioned to shine
		down from the top left onto the cube */

	sgl_create_point_light(UNAMED_ITEM,mediumGrey, lightDir,lightPos,
						lightCon,
						!SHADOW_LIGHT,NO_SMOOTH_SHADING);
  	

 
	sgl_create_transform(UNAMED_ITEM);   /* this could be omitted */
	sgl_translate(0.0f,0.0f,560.0f+64.0);
    sgl_rotate(sgl_x_axis,(PI/2.5f));
    sgl_rotate(sgl_z_axis,(PI/8.0f));

	/* set the cube material properties */
	
	sgl_set_diffuse(White);
	sgl_set_specular(White,0); /* turn specular reflectivity off */
	sgl_set_texture_map(texName,FALSE,FALSE);
  
	/*	build up a cube with 4 simple planes, and two textured planes*/

	sideNorm[0]=1.0;
	sideNorm[1]=0.0;
	sideNorm[2]=0.0;
    sidePnt[0]=sideNorm[0]*64.0;
    sidePnt[1]=sideNorm[1]*64.0;
    sidePnt[2]=sideNorm[2]*64.0;
	sgl_add_simple_plane(sidePnt, sideNorm, VISIBLE);	

	sideNorm[0]=0.0;
	sideNorm[1]=0.0;
	sideNorm[2]=1.0;
    sidePnt[0]=sideNorm[0]*64.0;
    sidePnt[1]=sideNorm[1]*64.0;
    sidePnt[2]=sideNorm[2]*64.0;
	sgl_add_simple_plane(sidePnt, sideNorm, VISIBLE);	

	sideNorm[0]=-1.0;
	sideNorm[1]=0.0;
	sideNorm[2]=0.0;
    sidePnt[0]=sideNorm[0]*64.0;
    sidePnt[1]=sideNorm[1]*64.0;
    sidePnt[2]=sideNorm[2]*64.0;
	sgl_add_simple_plane(sidePnt, sideNorm, VISIBLE);	

	sideNorm[0]=0.0;
	sideNorm[1]=0.0;
	sideNorm[2]=-1.0;

    sidePnt[0]=-64.0;
    sidePnt[1]=-64.0;
    sidePnt[2]=-64.0;

	uv1[0]=0.0;
	uv1[1]=0.0;

	point2[0]=-64.0;
	point2[1]=64.0;
	point2[2]=-64.0;

	uv2[0]=0.0;
	uv2[1]=1.0;

	point3[0]=64.0;
	point3[1]=-64.0;
	point3[2]=-64.0;

	uv3[0]=1.0;
	uv3[1]=0.0;

	sgl_add_plane(sidePnt,point2,point3,VISIBLE,
				NULL,NULL,NULL,uv1,uv2,uv3); 

	sideNorm[0]=0.0;
	sideNorm[1]=1.0;
	sideNorm[2]=0.0;
    sidePnt[0]=sideNorm[0]*64.0;
    sidePnt[1]=sideNorm[1]*64.0;
    sidePnt[2]=sideNorm[2]*64.0;
	sgl_add_simple_plane(sidePnt, sideNorm, VISIBLE);	

	/*re-define the material, without starting a new object*/

	sgl_create_material(FALSE,TRUE);
	sgl_set_texture_map(texName1,FALSE,FALSE);

	sideNorm[0]=0.0;
	sideNorm[1]=-1.0;
	sideNorm[2]=0.0;

    sidePnt[0]=-64.0;
    sidePnt[1]=-64.0;
    sidePnt[2]=-64.0;

	uv1[0]=0.0;
	uv1[1]=0.0;

	point2[0]=64.0;
	point2[1]=-64.0;
	point2[2]=-64.0;

	uv2[0]=0.0;
	uv2[1]=1.0;

	point3[0]=-64.0;
	point3[1]=-64.0;
	point3[2]=64.0;

	uv3[0]=1.0;
	uv3[1]=0.0;

	sgl_add_plane(sidePnt,point2,point3,VISIBLE,
								NULL,NULL,NULL,uv1,uv2,uv3); 


	sgl_render(viewport1, camera1, FALSE);

	/*write the image to disc*/

	TexasWriteBMP("result.bmp");

	/* Close down the display device in an orderly fashion */
	
	free(First.pixels);
	free(Second.pixels);
	sgl_delete_device(logicalDevice);	

	exit(0);

 }	  



/*--------------------------- End of File --------------------------------*/

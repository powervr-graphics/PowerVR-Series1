/******************************************************************************
 * Name : tower.c
 * Title : tower example
 * Author : Stel Michael
 * Created : 26/07/1995
 *
 * Copyright : 1995-2022 Imagination Technologies (c)
 * License	 : MIT
 *
 * Description : tower example using SGL
 *
 * Platform : ANSI compatible
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

#define SMOOTH_SHADING		TRUE
#define NO_SMOOTH_SHADING	FALSE

#define INVISIBLE			TRUE
#define	VISIBLE				FALSE

#define	ON					TRUE
#define OFF					FALSE

#define	LENS_200MM			16.0f
#define	LENS_50MM			4.0f
#define	LENS_35MM			3.0f
#define	LENS_24MM			2.0f



#define PI	3.142f
#define TWOPI 	(2.0f * PI)


void    AddTower();



int	main()
{
	int logicalDevice;
	int viewport1,viewport2;
	int camera1,camera2,light1,light2;

	int frame;
	int groundTex,skyTex;	
	sgl_intermediate_map First,Second;
	
	sgl_colour	White	=	{1.0f,1.0f,1.0f};
	sgl_colour	green	=	{0.0f,1.0f,0.0f};
	sgl_colour SkyBlue= {0.5f, 0.6f, 0.9f};  

	sgl_colour	red	=	{1.0f,0.0f,0.0f};
	sgl_colour	blue	=	{0.0f,0.0f,1.0f};
	sgl_colour	yellow	=	{1.0f,1.0f,0.0f};
	sgl_colour	darkgrey	=	{0.25f,0.25f,0.25f};

	sgl_vector	ParLightDir = {1.0f,-1.0f,1.0f};

	sgl_vector	ParLight2Dir = {-1.0f,-1.0f,1.0f};

	sgl_vector	ParLight3Dir = {0.0f,-1.0f,1.0f};

	sgl_vector	groundPnt	= {0.0f,0.0f, 2000.0f };	
	sgl_vector	point2		= {-10.0f,0.0f, 2000.0f };	
	sgl_vector	point3		= {0.0f,0.0f, 2010.0f };	

	sgl_vector	skyPnt		= {0.0f, 2000.0f ,3000.0f};
	sgl_vector	skyPnt2		= {0.0f,2000.0f, 3010.0f };	
	sgl_vector	skyPnt3		= {-10.0f,2000.0f, 3000.0f };	
	
	sgl_2d_vec uv1,uv2,uv3;


	/* create screen device 0 with 640x480 screen resolution and 24 bit colour and no double buffering */ 

	logicalDevice = sgl_create_screen_device(0,640,680,sgl_device_24bit,FALSE);

	if (logicalDevice<0)
	{
		fprintf(stderr,"ERROR %01d: Failed to Open Screen Device\n",logicalDevice);
		exit(1);	
	}
	
	/*	set the viewport for the opened device so that there is a 20 pixel border
		around the rendered image */ 

	viewport1 = sgl_create_viewport(logicalDevice,0,0,640,480,80.0,0.0f,560.0f,480.0f);

	viewport2 = sgl_create_viewport(logicalDevice,64,64,191,319,64.0,128.0f,192.0f,256.0f);


	camera1 = sgl_create_camera(LENS_24MM,10.0f,0.0f);

	camera2 = sgl_create_camera(LENS_35MM,10.0f,0.0f);

    sgl_create_ambient_light (UNAMED_ITEM,darkgrey,GREY, FALSE);


	light1 = sgl_create_parallel_light (NAMED_ITEM,White,COLOUR, ParLightDir,NO_SHADOWS,FALSE);
	light2 = sgl_create_parallel_light (NAMED_ITEM,White,COLOUR, ParLight2Dir,NO_SHADOWS,FALSE);

	sgl_set_parallel_light (light2,blue,COLOUR, ParLight2Dir,NO_SHADOWS,FALSE);

	/* since camera cant be position yet everything is being translated then rotated about x axis */	

	sgl_create_transform(UNAMED_ITEM);   /* this could be omitted */
	sgl_translate(0.0f,-500.0f,1200.0f);
	sgl_rotate(sgl_x_axis,-0.14f);
	sgl_rotate(sgl_y_axis,1.0f);


	/* load up and assign names to textures */

	First=ConvertBMPtoSGL("../textures/rivets.bmp",FALSE);
	Second=ConvertBMPtoSGL("../textures/cloudsda.bmp",FALSE);

	groundTex=sgl_create_texture(sgl_map_16bit_mm,sgl_map_128x128,TRUE,TRUE,&First,NULL);
	skyTex=sgl_create_texture(sgl_map_16bit_mm,sgl_map_128x128,TRUE,TRUE,&Second,NULL);


	/* Add the sky plane */

	sgl_create_list(UNAMED_ITEM,TRUE,FALSE);

		uv1[0]=0.0;
		uv1[1]=0.0;

		uv2[0]=1.2E-3f;
		uv2[1]=0.0;

		uv3[0]=0.0;
		uv3[1]=1.2E-3f;

		sgl_set_glow(White); 

		sgl_set_texture_effect(FALSE,FALSE,FALSE,TRUE);

		sgl_set_texture_map(skyTex,FALSE,FALSE);

		sgl_add_plane(skyPnt,skyPnt2,skyPnt3,VISIBLE,FALSE,
				NULL,NULL,NULL,TRUE,uv1,uv2,uv3); 

	sgl_to_parent();



	/* Add the ground plane */

	sgl_create_list(UNAMED_ITEM,TRUE,FALSE);

		uv1[0]=0.0;
		uv1[1]=0.0;

		uv2[0]=0.04f;
		uv2[1]=0.0;

		uv3[0]=0.0;
		uv3[1]=0.04f;

		sgl_set_glow(White); 

		sgl_set_texture_effect(FALSE,FALSE,FALSE,TRUE);

		sgl_set_texture_map(groundTex,FALSE,FALSE);

		sgl_add_plane(groundPnt,point2,point3,VISIBLE,FALSE,
					NULL,NULL,NULL,TRUE,uv1,uv2,uv3); 

	sgl_to_parent();


	/* Add the tower model */

	AddTower();


   	sgl_set_diffuse(green);
   	sgl_set_specular(SkyBlue,4);  

	sgl_create_sphere (0, 200.0f, 6,FALSE, FALSE, FALSE);

	sgl_translate(10.0f,0.0f,400.0f);

	sgl_switch_light(light1, OFF, NO_SHADOWS,FALSE); /* switch off white light */

   	sgl_set_diffuse(red);
   	sgl_set_specular(SkyBlue,4);  

	sgl_create_sphere (0, 200.0f, 6,FALSE, FALSE, FALSE);

   /* 	Render a single image using the defined camera1 into viewport1 */

	for (frame = 0 ; frame<1 ; frame++)
	   	sgl_render(viewport1, camera1, FALSE);


	sgl_set_parallel_light (light2,White,COLOUR, ParLight2Dir,NO_SHADOWS,FALSE);

	sgl_set_parallel_light (light1,yellow,COLOUR, ParLight2Dir,NO_SHADOWS,FALSE);

	sgl_render(viewport2, camera2, FALSE);

	TexasWriteBMP("result.bmp");


	/* Close down the display device in an orderly fashion */
	
	free(First.pixels);

	sgl_delete_device(logicalDevice);	

	exit(0);

 }	  





/**************************************************************************
 * Function Name  : sgl_cross_prod
 * Inputs         : Vec1 ,Vec2 
 * Outputs        : 
 * Input/Output	  : Vec3
 * Returns        : 
 * Global Used    : None
 * Description    : returns cross product of two given vectors in Vec3
 *
 *					NOTE: The result MUST NOT be the same as either of the
 *					input vectors
 *
 **************************************************************************/


void sgl_cross_prod(const sgl_vector Vec1, const sgl_vector Vec2, sgl_vector Vec3)
{
	Vec3[0] = (Vec1[1] * Vec2[2]) - (Vec1[2] * Vec2[1]);
	Vec3[1] = (Vec1[2] * Vec2[0]) - (Vec1[0] * Vec2[2]);
	Vec3[2] = (Vec1[0] * Vec2[1]) - (Vec1[1] * Vec2[0]);
}



/**************************************************************************
 * Function Name  : sgl_vec_sub
 * Inputs         : Vec1 ,Vec2 
 * Outputs        : 
 * Input/Output	  : Vec3
 * Returns        : 
 * Global Used    : None
 * Description    : Does Vec1 - Vec2 putting result in Vec3
 *
 **************************************************************************/
void	sgl_vec_sub(const sgl_vector Vec1, const sgl_vector Vec2, sgl_vector Result)
{
	Result[0] = Vec1[0] - Vec2[0];
	Result[1] = Vec1[1] - Vec2[1];
	Result[2] = Vec1[2] - Vec2[2];
}





/**************************************************************************
 * Function Name  : sgl_simple_plane2
 * Inputs         : a, b, c - plane vertices 
					visible - plane visibility
 * Outputs        : None
 * Input/Output	  : None
 * Returns        : None
 * Global Used    : 
 * Description    : Adds a simple plane, specified as three vertices
 *					
 **************************************************************************/



void	sgl_simple_plane2(sgl_vector a,sgl_vector b,sgl_vector c,sgl_bool visible)
{                                       
	sgl_vector ab,ac;

	float norm[3];

	sgl_vec_sub(a,b,ab);  
	sgl_vec_sub(a,c,ac);
	                      
	sgl_cross_prod(ab,ac,norm);
	               
	sgl_add_simple_plane(a,norm,visible);    
} 





/**************************************************************************
 * Function Name  : RevolveSection
 * Inputs         : Vertices - array of 2d vertices, these must form a closed convex polygon
					NumVertices - number of vertices
					NumSections - number of elements in full object
					SectionStep - Number of section steps per iteration.
								  If this is one all section will have a convex primitive,
								  otherwise every n sections will have one.
 * Outputs        : None
 * Input/Output	  : None
 * Returns        : None
 * Global Used    : 
 * Description    : Rotates a closed convex polygon about the y axis forming a 3d object.
 *					
 **************************************************************************/

void	RevolveSection(sgl_2d_vec Vertices[],int NumVertices,int NumSections,int SectionStep)
{
	float AngleStep,CurAngle,SecAngle;
	int	CurVertice;
    int CurSection;
    sgl_vector  p1,p2,p3,p4,p5,p6;

	
	AngleStep=TWOPI/((float) NumSections);
    
	CurAngle=0.0f;
    
	for(CurSection = 0; CurSection < NumSections; CurSection++)
	{  
		/* CurAngle and SecAngle define the two boundaries of the section */
	
   		SecAngle = CurAngle+AngleStep;

		/* Step through the vertices, adding bounding planes as we go */

		for (CurVertice=0; CurVertice < (NumVertices - 1); CurVertice++)
		{	

			p1[0] = Vertices[CurVertice][0]*cos(CurAngle);
			p1[1] = Vertices[CurVertice][1];
			p1[2] = Vertices[CurVertice][0]*sin(CurAngle);
		    
			p2[0] = Vertices[CurVertice][0]*cos(SecAngle);
			p2[1] = Vertices[CurVertice][1];
			p2[2] = Vertices[CurVertice][0]*sin(SecAngle);
	    
			p3[0] = Vertices[CurVertice+1][0]*cos(CurAngle);
			p3[1] = Vertices[CurVertice+1][1];
			p3[2] = Vertices[CurVertice+1][0]*sin(CurAngle);
	    
			p4[0] = Vertices[CurVertice+1][0]*cos(SecAngle);
			p4[1] = Vertices[CurVertice+1][1];
			p4[2] = Vertices[CurVertice+1][0]*sin(SecAngle);
        
			/* add the two bounding planes at first iteration */

	        if (CurVertice==0)
   	    	{   
	        	p5[0] = Vertices[NumVertices-2][0]*cos(CurAngle);
				p5[1] = Vertices[NumVertices-2][1];
				p5[2] = Vertices[NumVertices-2][0]*sin(CurAngle);
	    
				p6[0] = Vertices[NumVertices-2][0]*cos(SecAngle);
				p6[1] = Vertices[NumVertices-2][1];
				p6[2] = Vertices[NumVertices-2][0]*sin(SecAngle);
				
				/* must create a new polyhedron */

				sgl_create_convex(UNAMED_ITEM);

				/* sgl_simple_plane2 define a plane using three point as
				   opposed to a point and a vector as in sgl_simple_plane */ 

				sgl_simple_plane2(p1,p5,p3,VISIBLE);
        	    sgl_simple_plane2(p2,p4,p6,VISIBLE);
	        }                                
        
			/* add outline plane */
	
			sgl_simple_plane2(p1,p3,p2,VISIBLE);

		}   

		CurAngle+=(AngleStep*((float) SectionStep));
    }

}



/**************************************************************************
 * Function Name  : Tube
 * Inputs         : Vertices -  two 2d vertices 
					NumVertices - number of vertices
					NumSides - tubes number of sides 
 * Outputs        : None
 * Input/Output	  : None
 * Returns        : None
 * Global Used    : 
 * Description    : Forms a very simple surface of revolution using two 
					given 2d vertices 
 *					
 **************************************************************************/

void	Tube(sgl_2d_vec Vertices[], int NumVertices, int NumSides)
{

	float 	AngleStep, SecAngle;
	float 	CurAngle = 0.0f;

	int	CurSide	= 0;

	sgl_vector	Base;
	sgl_vector  Top;
	sgl_vector 	p1, p2, p3, p4;

	sgl_bool	First;
	
	/* AngleStep is the angle of rotation per iteration */

	AngleStep = TWOPI / NumSides;
	
 	SecAngle = CurAngle+AngleStep;

    First=TRUE;

	/*
		loop throught the sides of the object 
	*/
	
	for (CurSide=0; CurSide<NumSides; CurSide++) 
	{   
		p1[0] = Vertices[0][0]*cos(CurAngle);
		p1[1] = Vertices[0][1];
		p1[2] = Vertices[0][0]*sin(CurAngle);
	     
		p2[0] = Vertices[0][0]*cos(SecAngle);
		p2[1] = Vertices[0][1];
		p2[2] = Vertices[0][0]*sin(SecAngle);

   		p3[0] = Vertices[1][0]*cos(CurAngle);
	   	p3[1] = Vertices[1][1];
		p3[2] = Vertices[1][0]*sin(CurAngle);

		/* for first iteration check to see if we need to add bounding planes
		   at top and bottom */

		if (CurSide == 0)
		{
			p4[0] = Vertices[1][0]*cos(SecAngle);
			p4[1] = Vertices[1][1];
			p4[2] = Vertices[1][0]*sin(SecAngle);
		
			Base[0] = 0.0f;
			Base[1] = Vertices[0][1];	
			Base[2] = 0.0f;       

			Top[0] = 0.0f;
			Top[1] = Vertices[1][1];
			Top[2] = 0.0f;
    
			/* 
			   if the x component for either vertex isnt zero, then add
			   a bounding plane 
			*/   

			if (Vertices[1][0] != 0.0f)  
			{
			    sgl_create_convex(UNAMED_ITEM);
				First = FALSE;
		     	sgl_simple_plane2(p3,Top,p4,VISIBLE);
		    }

			if (Vertices[0][0] != 0.0f)     
			{
				if (First)
				{
					sgl_create_convex(UNAMED_ITEM);
					First = FALSE;
				}

		     	sgl_simple_plane2(Base,p1,p2,VISIBLE);
			}
		}

   		if (First)
   		{
   			sgl_create_convex(UNAMED_ITEM);
   			First = FALSE;
   		}

     	sgl_simple_plane2(p1,p3,p2,VISIBLE);
		
		CurAngle = SecAngle;
	 	SecAngle += AngleStep;

	}
}
                                                



/**************************************************************************
 * Function Name  : AddTower
 * Inputs         : None
 * Outputs        : None
 * Input/Output	  : None
 * Returns        : None
 * Global Used    : TowerRotation 'sgl_modify_transform' can be used to rotate tower
 * Description    : Constructs Model of Tower
 *					
 **************************************************************************/

void    AddTower()
{

	sgl_colour White  = {1.0f,1.0f,1.0f};
	sgl_colour black  = {0.4f,0.4f,0.4f};

	sgl_colour Yellow = {1.0f,1.0f,0.0f};
	sgl_colour SkyBlue= {0.5f, 0.6f, 0.9f};  

 
	/* Data for the tower */

	sgl_2d_vec VoutlineTop[6]={ {75.0f,0.0f},
							    {72.0f,2.0f},
							    {60.0f,5.0f},
							    {55.0f,5.0f},
							    {50.0f,0.0f},
							    {75.0f,0.0f}};

   	sgl_2d_vec VoutlineCen[5]={ {73.0f,0.0f},
							    {73.0f,5.0f},
					            {50.0f,5.0f}, 
							    {50.0f,0.0f},
							    {73.0f,0.0f}};  
					
					
	sgl_2d_vec Frame1[5]={{65.0f,0.0f},
					   	  {50.0f,10.0f},
					   	  {45.0f,5.0f},
					   	  {50.0f,0.0f},
					   	  {65.0f,0.0f}};

	sgl_2d_vec Frame2[5]={{5.0f,25.0f},
	                      {5.0f,20.0f},
	                      {45.0f,5.0f},
	                      {50.0f,10.0f},
	                      {5.0f,25.0f}};
    
    
    sgl_2d_vec Tow1[2] = {{30.0f,0.0f},
    		     	   {25.0f,50.0f}};
    						
    sgl_2d_vec Tow2[2] = {{25.0f,50.0f},					
    					 {15.0f,250.0f}};
    						
    sgl_2d_vec Tow3[2] = {{15.0f,250.0f},
                        {12.5f,300.0f}};
                            
    sgl_2d_vec Tow4[2] = {{12.5f,300.0f},
						{5.0f,360.0f}};

    
	/* Add the tower */
    
	sgl_create_list(UNAMED_ITEM,TRUE,FALSE);

		sgl_set_diffuse(black);
   		sgl_set_specular(SkyBlue,1);  

		sgl_scale(2.75f,2.75f,2.75f);	

		/* Add main central part of the tower */
		
		/* NOTE: it would be more plane efficient with a routine
				 that took a convex arc of points in this particular case
		*/

        Tube(Tow1, 2, 24);
        Tube(Tow2, 2, 24);
        Tube(Tow3, 2, 24);
        Tube(Tow4, 2, 24);         

		/* translate to rotating part of tower */

		sgl_translate(0.0f,280.0f,0.0f);

        sgl_create_transform(UNAMED_ITEM);

		/* 
			NOTE: TRUE for 'preserve state' stores a copy of the current state which is
				  which becomes the state after sgl_to_parent				  
		*/
	    
		sgl_create_list(UNAMED_ITEM,TRUE,FALSE);

			/* sgl_y_axis is a predefined axis type - 0.0f,1.0f,0.0f */
			
	        sgl_rotate(sgl_y_axis,PI/32.0f);     
            
			/* Add the top struts */

	        RevolveSection(Frame1,5,32,4);
	        RevolveSection(Frame2,5,32,4);

			/* Add central dougnut shaped observation area */

	        sgl_rotate(sgl_y_axis,-PI/32.0f);

			RevolveSection(VoutlineTop,6,16,1);

		    sgl_translate(0.0f,-5.0f,0.0f);
		    sgl_set_diffuse(Yellow);

			RevolveSection(VoutlineCen,5,16,1);

			sgl_scale(1.0f,-1.0f,1.0f);	 
		    sgl_set_diffuse(White);

			RevolveSection(VoutlineTop,6,16,1);
      
		   /* Add the bottom strutts */

	        sgl_rotate(sgl_y_axis,PI/32.0f);

	        RevolveSection(Frame1,5,32,4);
	        RevolveSection(Frame2,5,32,4);

		sgl_to_parent();
        
    sgl_to_parent();

}



/*--------------------------- End of File --------------------------------*/

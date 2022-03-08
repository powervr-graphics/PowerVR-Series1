/******************************************************************************
 * Name : textower.c
 * Title : tower example
 * Author : Stel Michael
 * Created : 22/08/1995
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
void    AddGenerator();
void	AddHead();


int	groundTex, skyTex, pan1Tex, pan2Tex, pan3Tex, barbTex,glowyTex,badTex;	
sgl_intermediate_map First,Second,Third,Fourth,Fifth,Sixth,Seventh,Ninth;

int	camTran;


sgl_2d_vec uv1,uv2,uv3;

static sgl_colour White  = {0.8f,0.8f,0.8f};
static sgl_colour black  = {0.0f,0.0f,0.0f};

static sgl_colour Yellow = {1.0f,1.0f,0.0f};
static sgl_colour SkyBlue= {0.5f, 0.6f, 0.9f};  

static sgl_colour	fogWhite	=	{1.0f,0.8f,0.8f};

static sgl_colour	blue	=	{0.2f,0.2f,8.0f};
static sgl_colour	darkgrey	=	{0.55f,0.55f,0.55f};




void main(int argc, char * argv[])
{
	int logicalDevice;
	int viewport1;
	int camera1,light1,light2;

	int frame;
	int firstFrame, lastFrame;
	
	float	camAngle;

	sgl_vector	ParLightDir = {1.0f,-1.0f,1.0f};
	sgl_vector	ParLight2Dir = {-1.0f,-1.0f,1.0f};

	sgl_vector	groundPnt	= {0.0f,0.0f, 2000.0f };	
	sgl_vector	point2		= {-10.0f,0.0f, 2000.0f };	
	sgl_vector	point3		= {0.0f,0.0f, 2010.0f };	

	sgl_vector	skyPnt		= {0.0f, 1000.0f ,3000.0f};
	sgl_vector	skyPnt2		= {0.0f,1000.0f, 3010.0f };	
	sgl_vector	skyPnt3		= {-10.0f,1000.0f, 3000.0f };	
	


	/*
	// Determine how many frames to do
	*/
	if(argc == 1)
	{
		fprintf(stderr, "No parameter supplied.. assuming 1 frame\n");
		fprintf(stderr, "Usage is\n");
		fprintf(stderr, "textower N       Renders frame 0 to N \n");
		fprintf(stderr, "textower N N2    Renders frame N to N2 \n");

		firstFrame = 0;
		lastFrame = 0;
	}
	else if(argc == 2)
	{
		firstFrame = 0;

		sscanf(argv[1], "%d", &lastFrame);

		if(lastFrame < firstFrame)
		{
			fprintf(stderr, 
				"Bad last frame parameter \"%s\", doing frame 0 only\n", argv[1]);

			lastFrame = firstFrame;
		}
		
	}
	else
	{
		sscanf(argv[1], "%d", &firstFrame);
		sscanf(argv[2], "%d", &lastFrame);

		if(lastFrame < firstFrame)
		{
			fprintf(stderr, 
				"Bad last frame parameter \"%s\", doing frame 0 only\n", argv[1]);

			lastFrame = firstFrame = 0;
		}

	}


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
	
	/*	set the viewport for the opened device  */ 

 	viewport1 = sgl_create_viewport( logicalDevice, 
									 0,0,640,480,
									 0.0f, 0.0f, 560.0f, 480.0f);



    sgl_create_ambient_light (UNAMED_ITEM,darkgrey,GREY, FALSE); 


	light1 = sgl_create_parallel_light( NAMED_ITEM, White, COLOUR, 
									   ParLightDir,NO_SHADOWS,FALSE);

	/*	
	// light2 = sgl_create_parallel_light( NAMED_ITEM, White, COLOUR, 
	//									   ParLight2Dir,NO_SHADOWS,FALSE);
	//
	// sgl_set_parallel_light (light2,blue, COLOUR, ParLight2Dir,
	// NO_SHADOWS,FALSE);
	*/


	/*
	// POSITION THE CAMERA
	*/
	sgl_create_list(UNAMED_ITEM,TRUE,FALSE);

		camTran = sgl_create_transform(TRUE);

		sgl_create_transform(FALSE);

		/* position camera */

		sgl_rotate(sgl_y_axis,0.5f);

		sgl_rotate(sgl_x_axis,-0.05f);

		sgl_translate(0.0f,150.0f,-600.0f);

		/* orient camera */

		sgl_rotate(sgl_y_axis,-0.05f);

		sgl_rotate(sgl_x_axis,0.012f);

		camera1 = sgl_create_camera(LENS_35MM,10.0f,0.0f);

	sgl_to_parent();

	/* load up and assign names to textures */

	First=ConvertBMPtoSGL("../textures/grass2.bmp",FALSE);
	Second=ConvertBMPtoSGL("../textures/cloudsda.bmp",FALSE);
	Third=ConvertBMPtoSGL("../textures/pan1.bmp",FALSE);
	Fourth=ConvertBMPtoSGL("../textures/pan2.bmp",FALSE);
	Fifth=ConvertBMPtoSGL("../textures/pan3.bmp",FALSE);

	Sixth=ConvertBMPtoSGL("../textures/barbie.bmp",TRUE);
	Seventh=ConvertBMPtoSGL("../textures/glowy.bmp",FALSE);

	Ninth=ConvertBMPtoSGL("../textures/badblk2.bmp",TRUE);

	groundTex= sgl_create_texture( sgl_map_16bit_mm, sgl_map_128x128, 
								   TRUE,TRUE,&First,NULL);

	skyTex= sgl_create_texture( sgl_map_16bit_mm, sgl_map_128x128,
								TRUE,TRUE, &Second, NULL);
	pan1Tex=sgl_create_texture( sgl_map_16bit_mm, sgl_map_128x128,
								TRUE,TRUE,&Third,NULL);
	pan2Tex=sgl_create_texture( sgl_map_16bit_mm, sgl_map_128x128,
								TRUE,TRUE,&Fifth,NULL);
	pan3Tex=sgl_create_texture( sgl_map_16bit_mm, sgl_map_128x128,
								TRUE,TRUE,&Fourth,NULL);

	barbTex=sgl_create_texture( sgl_map_trans16_mm, sgl_map_128x128,
								TRUE,TRUE,&Sixth,NULL);
	glowyTex=sgl_create_texture( sgl_map_16bit_mm, sgl_map_128x128,
								 TRUE,TRUE,&Seventh,NULL);

	badTex=sgl_create_texture( sgl_map_trans16, sgl_map_128x128,
							   FALSE,FALSE,&Ninth,NULL);

	/* Add the sky plane */

	sgl_create_list(UNAMED_ITEM,TRUE,FALSE);

		uv1[0]=0.0;
		uv1[1]=0.0;

		uv2[0]=2.43E-3f;
		uv2[1]=0.0;

		uv3[0]=0.0;
		uv3[1]=2.43E-3f;

		sgl_set_glow(White); 
		sgl_set_diffuse(black); 
		sgl_set_specular(black, 0); 
		sgl_set_texture_effect(TRUE,FALSE,FALSE,TRUE);
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

		sgl_set_diffuse(White); 
		sgl_set_texture_effect(TRUE,TRUE,TRUE,FALSE);
		sgl_set_texture_map(groundTex,FALSE,FALSE);
		sgl_add_plane(groundPnt,point2,point3, VISIBLE, FALSE, 
					 NULL,NULL,NULL,TRUE,uv1,uv2,uv3); 
	sgl_to_parent();


	/* Add the tower model */

 	sgl_set_texture_effect(TRUE,TRUE,TRUE,TRUE);

	AddGenerator();


   	sgl_create_list(UNAMED_ITEM,TRUE,FALSE);

		sgl_translate(-50.0f,0.0f,400.0f);

		AddTower();

	sgl_to_parent();


	/* set the fog */

	sgl_set_fog(camera1, fogWhite, 0.0005);

	sgl_set_background_colour(camera1,fogWhite);


	camAngle=0.0f;
	camAngle+= firstFrame * 0.2f;
	
	for (frame = firstFrame ; frame <= lastFrame ; frame++)
	{

		sgl_modify_transform (camTran, TRUE);	

		sgl_rotate(sgl_y_axis,camAngle);

		camAngle+=0.2f;

	   	sgl_render(viewport1, camera1, FALSE);

		/*
		// Write out the picture to a numbered file
		*/
		{
			char fname[30];
	
			sprintf(fname, "result%02d.bmp", frame);
			TexasWriteBMP(fname);
		}
	
		printf("Frame : %d\n",frame);
	}	

	printf ("Finished !!!!!\n");


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
 * Function Name  : sgl_texture_plane
 * Inputs         : texName
					a, b, c - plane vertices 
					visible - plane visibility
					xScale, yScale - texture scaling
 * Outputs        : None
 * Input/Output	  : None
 * Returns        : None
 * Global Used    : 
 * Description    : Adds a simple plane, specified as three vertices
 *					
 **************************************************************************/


void	sgl_texture_plane(int texName,float a[3],float b[3],float c[3],int visible,float xScale,float yScale,sgl_bool FirstPlane)
{                                       
	static int LastTextureName=-1;
	sgl_2d_vec uv1,uv2,uv3;
    
	uv1[0]=0.0f;
	uv1[1]=0.0f;

	uv2[0]=xScale;
	uv2[1]=0.0f;

  	uv3[0]=0.0f;
   	uv3[1]=yScale;

	/* NOTE: Avoiding the use of lots of local materials as it 
			 it severely slows down sgl */ 

	if (((LastTextureName==-1) || (texName!=LastTextureName)) || FirstPlane)
	{
		sgl_create_material(UNAMED_ITEM,TRUE);	

		sgl_set_texture_map(texName,FALSE,FALSE);
	}

   	sgl_add_plane(a,b,c,visible,FALSE,NULL,NULL,NULL,TRUE,uv1,uv2,uv3); 

	LastTextureName = texName;
	 
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

void	RevolveSection(sgl_2d_vec Vertices[],int NumVertices,int NumSections,int SectionStep,int textureName)
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

				sgl_texture_plane(textureName,p1,p5,p3,VISIBLE, 1.0f, 1.0f,TRUE);
				sgl_texture_plane(textureName,p2,p4,p6,VISIBLE, 1.0f, 1.0f,FALSE);

	        }                                
        
			/* add outline plane */
	
			sgl_texture_plane(textureName,p1,p3,p2,VISIBLE,1.0f,1.0f,FALSE);

		}   

		CurAngle+=(AngleStep*((float) SectionStep));
    }

}



/**************************************************************************
 * Function Name  : Tube
 * Inputs         : Vertices -  two 2d vertices 
					NumVertices - number of vertices
					NumSides - tubes number of sides
					Hollow - if hollow is true invisible planes are used for top and bottom 
 * Outputs        : None
 * Input/Output	  : None
 * Returns        : None
 * Global Used    : 
 * Description    : Forms a very simple surface of revolution using two 
					given 2d vertices 
 *					
 **************************************************************************/

void	Tube(sgl_2d_vec Vertices[], int NumVertices, int NumSides, int textureName, sgl_bool Hollow)
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
				sgl_texture_plane(textureName,p3,Top,p4,Hollow, 1.0f, 1.0f, TRUE);
		    }

			if (Vertices[0][0] != 0.0f)     
			{
				if (First)
				{
					sgl_create_convex(UNAMED_ITEM);
					sgl_texture_plane(textureName,Base,p1,p2,Hollow, 1.0f, 1.0f, TRUE);
					First = FALSE;
				}
				else
					sgl_texture_plane(textureName,Base,p1,p2,Hollow, 1.0f, 1.0f, FALSE);

			}
		}

   		if (First)
   		{
   			sgl_create_convex(UNAMED_ITEM);
	   		sgl_texture_plane(textureName,p1,p3,p2,VISIBLE, 1.0f, 1.0f,TRUE);
   			First = FALSE;
   		}
		else
   			sgl_texture_plane(textureName,p1,p3,p2,VISIBLE, 1.0f, 1.0f,FALSE);
		
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
 * Global Used    : 
 * Description    : Constructs Model of Tower
 *					
 **************************************************************************/

void    AddTower()
{


 
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

		sgl_set_diffuse(White); 

/*  	sgl_set_specular(SkyBlue,1);  
*/
		sgl_scale(1.35f,1.35f,1.35f);

		/* Add main central part of the tower */
		
		/* NOTE: it would be more plane efficient with a routine
				 that took a convex arc of points in this particular case
		*/
 
        Tube(Tow1, 2, 12, pan2Tex, FALSE);

        Tube(Tow2, 2, 12, pan2Tex, FALSE);

        Tube(Tow3, 2, 12, pan2Tex, FALSE);

        Tube(Tow4, 2, 12, pan2Tex, FALSE);         


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

	        RevolveSection(Frame1,5,32,4,pan1Tex);
	        RevolveSection(Frame2,5,32,4,pan1Tex);

			/* Add central dougnut shaped observation area */

	        sgl_rotate(sgl_y_axis,-PI/32.0f);

			RevolveSection(VoutlineTop,6,16,1,pan1Tex);

		    sgl_translate(0.0f,-5.0f,0.0f);

		    sgl_set_glow(Yellow);
		    sgl_set_diffuse(black);

			RevolveSection(VoutlineCen,5,16,1,pan1Tex);

			sgl_scale(1.0f,-1.0f,1.0f);	 
		    sgl_set_glow(black);
		    sgl_set_diffuse(White);

			RevolveSection(VoutlineTop,6,16,1,pan1Tex);
    
		   /* Add the bottom strutts */

	        sgl_rotate(sgl_y_axis,PI/32.0f);

	        RevolveSection(Frame1,5,32,4,pan1Tex);
	        RevolveSection(Frame2,5,32,4,pan1Tex);

		sgl_to_parent();
        
    sgl_to_parent();

}





/**************************************************************************
 * Function Name  : AddGenerator
 * Inputs         : None
 * Outputs        : None
 * Input/Output	  : None
 * Returns        : None
 * Global Used    : 
 * Description    : Constructs Model of Generator
 *					
 **************************************************************************/


void    AddGenerator()
{

    sgl_2d_vec Gen1[2] = {{50.0f,0.0f},
    		     	   {50.0f,35.0f}};

    sgl_2d_vec Gen2[2] = {{40.0f,35.0f},
    		     	   	  {40.0f,85.0f}};

    sgl_2d_vec Gen3[2] = {{50.0f,85.0f},
    		     	   	  {50.0f,120.0f}};


    sgl_2d_vec Fence[2] = {{150.0f,5.0f},
    		     	   	  {150.0f,30.0f}};


	sgl_2d_vec Topoutline[6]={
					{152.5f,0.0f},
					{152.5f,5.0f},
					{147.5f,5.0f}, 
					{147.5f,0.0f},
					{152.5f,0.0f}};  


   	sgl_create_list(UNAMED_ITEM,TRUE,FALSE);

		sgl_set_diffuse(White); 
/*   		sgl_set_specular(SkyBlue,1);  
*/
	    Tube(Gen1, 2, 6, pan1Tex,FALSE);	

		/* Set global translucency value */ 

		sgl_set_opacity(0.35f);

	    Tube(Gen2, 2, 12, glowyTex, FALSE);
 
		/* Set global translucency back to fully opaque */

		sgl_set_opacity(1.0f);

	    Tube(Gen3, 2, 6, pan1Tex, FALSE);

		/* Add translucent textured fence */ 

	    Tube(Fence, 2, 36, barbTex, TRUE);

   		RevolveSection(Topoutline,5,18,1,pan1Tex);

		sgl_translate(0.0f,30.0f,0.0f);

   		RevolveSection(Topoutline,5,18,1,pan1Tex);
        
		
	   	sgl_create_list(UNAMED_ITEM,TRUE,FALSE);
	
			sgl_translate(0.0f,160.0f,0.0f);

			sgl_rotate(sgl_x_axis, PI/2.0f);	

			AddHead();	

	    sgl_to_parent();

    sgl_to_parent();

}





/**************************************************************************
 * Function Name  : AddHead
 * Inputs         : None
 * Outputs        : None
 * Input/Output	  : None
 * Returns        : None
 * Global Used    : 
 * Description    : Adds a translucent textured map of a head, bounded by invisible planes.
 *					
 **************************************************************************/

void	AddHead()
{

	sgl_vector	sidePnt;
	sgl_vector	sideNorm;

	sgl_vector	pnt2;	
	sgl_vector	pnt3;	

	/*	build up a cube with 4 simple planes, and two textured planes*/

   	sgl_create_convex(UNAMED_ITEM);


	sideNorm[0]=1.0;
	sideNorm[1]=0.0;
	sideNorm[2]=0.0;
    sidePnt[0]=sideNorm[0]*64.0;
    sidePnt[1]=sideNorm[1]*4.0;
    sidePnt[2]=sideNorm[2]*64.0;

	sgl_add_simple_plane(sidePnt, sideNorm, INVISIBLE);	


	sgl_create_material(FALSE,TRUE);

	sgl_set_texture_map(badTex,FALSE,FALSE);

	sideNorm[0]=0.0;
	sideNorm[1]=-1.0;
	sideNorm[2]=0.0;

    sidePnt[0]=-64.0;
    sidePnt[1]=-4.0;
    sidePnt[2]=-64.0;

	uv1[0]=0.0;
	uv1[1]=0.0;

	pnt2[0]=64.0;
	pnt2[1]=-4.0;
	pnt2[2]=-64.0;

	uv2[0]=0.0;
	uv2[1]=1.0;

	pnt3[0]=-64.0;
	pnt3[1]=-4.0;
	pnt3[2]=64.0;

	uv3[0]=1.0;
	uv3[1]=0.0;

	sgl_add_plane(sidePnt,pnt2,pnt3,VISIBLE,FALSE,NULL,NULL,NULL,TRUE,uv1,uv2,uv3); 


	sideNorm[0]=0.0;
	sideNorm[1]=0.0;
	sideNorm[2]=1.0;
    sidePnt[0]=sideNorm[0]*64.0;
    sidePnt[1]=sideNorm[1]*4.0;
    sidePnt[2]=sideNorm[2]*64.0;

	sgl_add_simple_plane(sidePnt, sideNorm, INVISIBLE);	

	sideNorm[0]=-1.0;
	sideNorm[1]=0.0;
	sideNorm[2]=0.0;

    sidePnt[0]=sideNorm[0]*64.0;
    sidePnt[1]=sideNorm[1]*4.0;
    sidePnt[2]=sideNorm[2]*64.0;

	sgl_add_simple_plane(sidePnt, sideNorm, INVISIBLE);	

	sideNorm[0]=0.0;
	sideNorm[1]=0.0;
	sideNorm[2]=-1.0;

    sidePnt[0]=-64.0;
    sidePnt[1]=-4.0;
    sidePnt[2]=-64.0;

	sgl_add_simple_plane(sidePnt, sideNorm, INVISIBLE);	

	sideNorm[0]=0.0;
	sideNorm[1]=1.0;
	sideNorm[2]=0.0;

    sidePnt[0]=sideNorm[0]*64.0;
    sidePnt[1]=sideNorm[1]*4.0;
    sidePnt[2]=sideNorm[2]*64.0;

	sgl_add_simple_plane(sidePnt, sideNorm, INVISIBLE);	

}



/*--------------------------- End of File --------------------------------*/

#include "sgl.h"

#define RGB(r, g, b) ((sgl_uint32) (((r) << 16) | ((g) << 8) | (b)))



SGLCONTEXT     SGLContext;

SGLVERTEX      Vertices[] =	{

/*                 x      y     z  (1/W)     col       spec.col (u/W) (v/W) */

		    {  0.0f,240.0f,0.0f,0.1f,RGB(255,0,0),RGB(0,0,0),0.0f,0.0f},

		    {320.0f,479.0f,0.0f,0.8f,RGB(255,0,0),RGB(0,0,0),0.0f,0.0f},

		    {639.0f,240.0f,0.0f,0.1f,RGB(255,0,0),RGB(0,0,0),0.0f,0.0f},

		    {320.0f,  0.0f,0.0f,0.8f,RGB(0,255,0),RGB(0,0,0),0.0f,0.0f}

					};

int		   TriFaceData[][3] = {{0,1,2},{3,0,2}};

int            Device;



void main ()

{

	Device = sgl_create_screen_device (0, 640, 480, sgl_device_16bit, TRUE);

	if (Device >= 0)

	{

          SGLContext.bFogOn=TRUE;

	    SGLContext.fFogR=0.5f;

          SGLContext.fFogG=0.5f;

	    SGLContext.fFogB=1.0f;

	    SGLContext.u32FogDensity=16;

					   

	    sgltri_startofframe (&SGLContext);

	    sgltri_triangles	(&SGLContext, 2, TriFaceData, Vertices);

	    sgltri_render       (&SGLContext);

	

	    sgl_delete_device(Device);

	}

}


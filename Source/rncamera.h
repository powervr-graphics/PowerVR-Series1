/**************************************************************************
 * Name         : rncamera.h
 * Title        : Header for rncamera.c
 * Author       : Simon Fenney
 * Created      : 11 May 1995
 *
 * Copyright : 1995-2022 Imagination Technologies (c)
 * License	 : MIT
 *
 * Description  : function prototypes from rncamera.c
 *
 * Platform     : ANSI
 *
 * Modifications:-
 * $Log: rncamera.h,v $
 * Revision 1.5  1997/02/04  11:17:55  gdc
 * removed CAMERA parameter from func RnSetupDefaultCamera as the global
 * that was being sent is now picked up directly
 *
 * Revision 1.4  1995/10/04  08:35:33  sjf
 * Added code to support texture space which is independant of the
 * size of the texture.
 *
 * Revision 1.3  1995/07/28  11:28:53  sjf
 * Added update local proj matrix.
 *
 * Revision 1.2  1995/07/23  16:20:18  sjf
 * Added Default camer setup, and made parameters consts for Projection
 * matrix setup.
 *
 * Revision 1.1  1995/07/20  17:59:57  sjf
 * Initial revision
 *
 *
 *
 *
 **************************************************************************/

#ifndef __rncamera_h__
#define __rncamera_h__



void RnSetupProjectionMatrix(const CAMERA_NODE_STRUCT *pCamera, 
							 const VIEWPORT_NODE_STRUCT *pViewport);

void RnSetupDefaultCamera();

void RnUpdateLocalProjectionMatrix(const TRANSFORM_STRUCT *pTransform);

void RnRescaleLocalProjectionTexture(const int TextureSize,
						const TRANSFORM_STRUCT *pTransform);

#endif /* __rncamera_h__ */

/*--------------------------- End of File --------------------------------*/

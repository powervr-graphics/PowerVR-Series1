 /******************************************************************************
 * Name : sgl_api.h
 * Author : VideoLogic Limited
 * Created : 18/04/1995
 *
 * Copyright : 1995-2022 Imagination Technologies (c)
 * License	 : MIT
 *
 * Description : Enabling header file for API writers 
 *               writing for PowerVR
 *
 * Platform : ANSI compatible
 *
 * (internal only version $Revision: 1.2 $ )
 *
 *****************************************************************************/
/*
// If being compiled in C++, inform it that SGL is C
*/
#ifndef __SGL_API__
#define __SGL_API__

#ifdef __cplusplus
extern "C" {
#endif 

#ifdef WIN32
 #define CALL_CONV __cdecl
#else
 #ifdef __WATCOMC__
  #define CALL_CONV __cdecl
 #else
  #define CALL_CONV
 #endif
#endif

#if _BUILDING_SGL_
#define DllExport __declspec(dllexport) 
#else
#define DllExport
#endif

#define API_DEV_MAGIC_NUM 0x12345678
DllExport void CALL_CONV sgl_register_api(sgl_uint32 uMagicNum);

#ifdef __cplusplus
 }
#endif 

#endif

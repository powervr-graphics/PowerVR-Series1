/******************************************************************************
 * Name         : ordinals.h
 * Author       : Graham Connor
 * Created      : 16/04/1997
 *
 * Copyright	: 1995-2022 Imagination Technologies (c)
 * License		: MIT
 *
 * Description  : SGLMID?
 *
 * Platform     : ANSI
 *
 * Modifications:
 * $Log: ordinals.h,v $
 * Revision 1.8  1997/09/11  10:34:42  erf
 * Added SGL quality function sgl_qual_dither().
 *
 * Revision 1.7  1997/04/24  20:08:01  gdc
 * added a private external entry point to explicitly initialise sgl
 *
 * Revision 1.6  1997/04/24  09:05:36  gdc
 * added a new function class which exports anonomously but doesnt
 * export from the thin layer
 *
 * Revision 1.5  1997/04/22  16:45:25  erf
 * Added stuff to export PVRD private routines.
 *
 * Revision 1.4  1997/04/22  12:17:13  gdc
 * added a pvrd option to but JWH func stuct for PVRHal
 *
 * Revision 1.3  1997/04/22  10:13:06  gdc
 * started adding stuff to allow private function exports - note
 * that PFUNCTION macro has its working version commented waiting
 * for actual functions to appear
 *
 * Revision 1.2  1997/04/22  09:16:42  gdc
 * removed d3disloadingsgl function
 *
 * Revision 1.1  1997/04/21  11:35:39  gdc
 * Initial revision
 *
 *
 *****************************************************************************/

#if DEFFILE==1

#define ZFUNCTION(x,y,type) x @y NONAME
#define YFUNCTION(x,y,type) x @y NONAME
#define PFUNCTION(x,y,type) x @y NONAME
#define XFUNCTION(x,y,type,extra_arg) x @y NONAME
#define FIRST_PRIVATE_FUNCTION
#define LAST_PUBLIC_FUNCTION
#define TOPSTRING EXPORTS
#define BOTSTRING

#endif

#if HFILE==1

#define ZFUNCTION(x,y,type) PVROS##x=y,
#define YFUNCTION(x,y,type) PVROS##x=y,
#define XFUNCTION(x,y,type,extra_arg) PVROS##x=y,
#define PFUNCTION(x,y,type) PVROS##x=y,
#define TOPSTRING enum eSglFuncOrdinals {
#define LAST_PUBLIC_FUNCTION LastPublicFunction,
#define FIRST_PRIVATE_FUNCTION FirstPrivateFunction=1000,
#define BOTSTRING ThisIsAlwaysTheLastEntry };

#endif

#if SGLTHIN
void Int3();
#define DllExport __declspec( dllexport ) __declspec( naked )
#define ZFUNCTION(x,y,type)
#define PFUNCTION(x,y,type)
#define YFUNCTION(x,y,type) DllExport void x(){\
	_asm{ jmp DWORD PTR [func_array +(PVROS##x)*4]   } }
#define XFUNCTION(x,y,type,extra_arg) DllExport type x(){\
	_asm{ \
			  pop  RET_ADDR \
			  push EXTRA_ARG \
			  push RET_ADDR \
			  jmp func_array[PVROS##x]   } }
#define LAST_PUBLIC_FUNCTION
#define FIRST_PRIVATE_FUNCTION
#define TOPSTRING
#define BOTSTRING

#endif

#if PVRD

#define PFUNCTION(x,y,type) PFN_PVRD_##type pfn##x;
#define XFUNCTION(x,y,type,extra_arg)
#define YFUNCTION(x,y,type)
#define TOPSTRING typedef struct _PVR_DIRECT_IF_ {
#define LAST_PUBLIC_FUNCTION 
#define FIRST_PRIVATE_FUNCTION 
#define BOTSTRING  }  PVR_DIRECT_IF, *PPVR_DIRECT_IF;

#endif

TOPSTRING
/*******************************************************
** DO NOT RE-ORDER THESE FUNCTIONS - YOU WILL BREAK SGL
*******************************************************/
	YFUNCTION(sgl_get_versions,1, sgl_versions * )
	YFUNCTION(sgl_create_screen_device,2, int	)
	YFUNCTION(sgl_get_device,3, int	)
	YFUNCTION(sgl_delete_device,4, void )
	YFUNCTION(sgl_create_viewport,5, int  )
	YFUNCTION(sgl_get_viewport,6, int  )
	YFUNCTION(sgl_set_viewport,7, int )
	YFUNCTION(sgl_subtract_viewport,8, void )
	YFUNCTION(sgl_delete_viewport,9, void )
	YFUNCTION(sgl_create_list,10, int 	)
	YFUNCTION(sgl_to_parent,11, void )
	YFUNCTION(sgl_modify_list,12, void )
	YFUNCTION(sgl_delete_list,13, void )
	YFUNCTION(sgl_detach_list,14, void	)
	YFUNCTION(sgl_attach_list,15, void )
	YFUNCTION(sgl_set_ignore_list,16, void )
	YFUNCTION(sgl_use_instance,17, void	)
	YFUNCTION(sgl_instance_substitutions,18, void	)
	YFUNCTION(sgl_dump_list,19, void )
	YFUNCTION(sgl_create_transform,20, int 	)
	YFUNCTION(sgl_modify_transform,21, void )
	YFUNCTION(sgl_translate,22, void )
	YFUNCTION(sgl_rotate,23, void )
	YFUNCTION(sgl_scale,24, void	)
	YFUNCTION(sgl_create_convex,25, int  )
	YFUNCTION(sgl_create_light_volume,26, int	)
	YFUNCTION(sgl_create_hidden_convex,27, int	)
	YFUNCTION(sgl_modify_convex,28, void	)
	YFUNCTION(sgl_add_simple_plane,29, void )
	YFUNCTION(sgl_add_shadow_limit_plane,30, void )
	YFUNCTION(sgl_add_plane,31, void )
	YFUNCTION(sgl_set_simple_plane,32, void )
	YFUNCTION(sgl_set_plane,33, void )
	YFUNCTION(sgl_delete_plane,34, void )
	YFUNCTION(sgl_plane_count,35, int	)
	YFUNCTION(sgl_create_mesh,36, int	)
	YFUNCTION(sgl_delete_mesh,37, void	)
	YFUNCTION(sgl_modify_mesh,38, void )
	YFUNCTION(sgl_add_vertices,39, void )
	YFUNCTION(sgl_add_face,40, void )
	YFUNCTION(sgl_set_cull_mode,41, void	)
	YFUNCTION(sgl_num_vertices,42, int	)
	YFUNCTION(sgl_num_faces,43, int	)
	YFUNCTION(sgl_num_face_vertices,44, int	)
	YFUNCTION(sgl_get_vertex,45, int	)
	YFUNCTION(sgl_set_vertex,46, void	)
	YFUNCTION(sgl_get_face,47, int	)
	YFUNCTION(sgl_set_face,48, void )
	YFUNCTION(sgl_delete_vertex,49, void )
	YFUNCTION(sgl_delete_face,50, void )
	YFUNCTION(sgl_create_material,51, int  )
	YFUNCTION(sgl_use_material_instance,52, void )
	YFUNCTION(sgl_modify_material,53, void )
	YFUNCTION(sgl_set_ambient,54, void	)
	YFUNCTION(sgl_set_diffuse,55, void )
	YFUNCTION(sgl_set_specular,56, void )
	YFUNCTION(sgl_set_glow,57, void )
	YFUNCTION(sgl_set_opacity,58, void )
	YFUNCTION(sgl_set_texture_map,59, void )
	YFUNCTION(sgl_set_texture_effect,60, void	)
	YFUNCTION(sgl_create_texture,61, int	)
	YFUNCTION(sgl_create_cached_texture,62, int	)
	YFUNCTION(sgl_load_cached_texture,63, void	)
	YFUNCTION(sgl_preprocess_texture,64, int	)
	YFUNCTION(sgl_free_pixels,65, void  )
	YFUNCTION(sgl_unload_cached_texture,66, void )
	YFUNCTION(sgl_register_texture_callback,67, void )
	YFUNCTION(sgl_texture_size,68, long	)
	YFUNCTION(sgl_set_texture,69, void )
	YFUNCTION(sgl_set_texture_extended,70, int )
	YFUNCTION(sgl_delete_texture,71, void )
	YFUNCTION(sgl_get_free_texture_mem,72, unsigned long )
	YFUNCTION(sgl_get_free_texture_mem_info,73, void )
	YFUNCTION(sgl_set_omap,74, void )
	YFUNCTION(sgl_create_ambient_light,75, int	)
	YFUNCTION(sgl_create_parallel_light,76, int	)
	YFUNCTION(sgl_create_point_light,77, int  )
	YFUNCTION(sgl_position_light,78, void )
	YFUNCTION(sgl_switch_light,79, void	)
	YFUNCTION(sgl_set_ambient_light,80, void )
	YFUNCTION(sgl_set_parallel_light,81, void	)
	YFUNCTION(sgl_set_point_light,82, void	)
	YFUNCTION(sgl_pseudo_multishadows,83, void	)
	YFUNCTION(sgl_create_camera,84, int 	)
	YFUNCTION(sgl_get_camera,85, int	)
	YFUNCTION(sgl_set_camera,86, void )
	YFUNCTION(sgl_render,87, void )
	YFUNCTION(sgl_use_eor_callback,88, int	)
	YFUNCTION(sgl_create_detail_levels,89, int  )
	YFUNCTION(sgl_get_detail_levels,90, int  )
	YFUNCTION(sgl_set_detail_levels,91, void )
	YFUNCTION(sgl_create_point,92, int	)
	YFUNCTION(sgl_set_point,93, void	)
	YFUNCTION(sgl_switch_point,94, void	)
	YFUNCTION(sgl_position_point,95, void	)
	YFUNCTION(sgl_query_point,96, int	)
	YFUNCTION(sgl_set_fog,97, void	)
	YFUNCTION(sgl_set_background_colour,98, void	)
	YFUNCTION(sgl_create_rectprism,99, int  )
	YFUNCTION(sgl_create_prism,100, int  )
	YFUNCTION(sgl_create_sphere,101, int  )
	YFUNCTION(sgl_create_extruded_poly,102, int  )
	YFUNCTION(sgl_revolve_polygon,103, int  )
	YFUNCTION(sgl_new_translucent,104, void	)
	YFUNCTION(sgl_qual_smooth_shading,105, void )
	YFUNCTION(sgl_qual_texturing,106, void	)
	YFUNCTION(sgl_qual_collision_detection,107, void	)
	YFUNCTION(sgl_qual_generate_shadows,108, void	)
	YFUNCTION(sgl_qual_fog,109, void )
	YFUNCTION(sgl_qual_texture_filter,110, void )
	YFUNCTION(sgl_qual_dither,111, void )
	YFUNCTION(sgl_rand,112, long )
	YFUNCTION(sgl_srand,113, void )
	YFUNCTION(ConvertBMPtoSGL,114,  sgl_intermediate_map )
	YFUNCTION(LoadBMPTexture,115, int )
	YFUNCTION(FreeBMPTexture,116, void )
	YFUNCTION(FreeAllBMPTextures,117, void )
	YFUNCTION(sgl_get_errors,118, void )
	YFUNCTION(sgl_get_win_versions,119, void )
	YFUNCTION(sgl_use_ddraw_mode,120, int )
	YFUNCTION(sgl_use_address_mode,121, void )
	YFUNCTION(sgltri_startofframe,122, void )
	YFUNCTION(sgltri_triangles,123, void )
	YFUNCTION(sgltri_quads,124, void )
	YFUNCTION(sgltri_points,125, void )
	YFUNCTION(sgltri_shadow,126, void )
	YFUNCTION(sgltri_render,127, void )
	YFUNCTION(sgltri_renderstrip,128, sgl_bool )
	YFUNCTION(sgltri_rerender,129, void )
	YFUNCTION(sgltri_isrendercomplete,130, IRC_RESULT )
	YFUNCTION(sgl_set_smap,131, void )
	YFUNCTION(sgl_direct_set_texture_type,132, void )
	YFUNCTION(sgl_direct_set_texture,133, void )
	YFUNCTION(sgl_direct_create_texture,134, void )
	ZFUNCTION(SglSetGlobal, 135, void)
	ZFUNCTION(SglInitialise, 136, int)
	 /*ZFUNCTION(SglGetFuncptrs,137, int )*/
	LAST_PUBLIC_FUNCTION
/*************************************
** Insert private functions after here 	
*************************************/
/* note: The first private function marker is 1000 
**   - so the first private function is 1001
*/
    FIRST_PRIVATE_FUNCTION
    PFUNCTION(PVRDCreateRenderContext,1001,CREATERENDERCONTEXT)
    PFUNCTION(PVRDDestroyRenderContext,1002,DESTROYRENDERCONTEXT)
    PFUNCTION(PVRDStartFrame,1003,STARTFRAME)
    PFUNCTION(PVRDTriangles,1004,TRIANGLES)
    PFUNCTION(PVRDLines,1005,LINES)
    PFUNCTION(PVRDPoints,1006,POINTS)
    PFUNCTION(PVRDPreRender,1007,PRERENDER)
    PFUNCTION(PVRDRender,1008,RENDER)
    PFUNCTION(PVRDRenderStrip,1009,STRIPRENDER)
/*************************************
** Do not put any active code after this line
*************************************/
BOTSTRING


#undef ZFUNCTION
#undef YFUNCTION
#undef XFUNCTION
#undef PFUNCTION
#undef TOPSTRING
#undef BOTSTRING
#undef FIRST_PRIVATE_FUNCTION
#undef LAST_PUBLIC_FUNCTION

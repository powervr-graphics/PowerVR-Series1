#ifndef __METRIC_H__
#define __METRIC_H__

#include "sgl.h"
#include "sgl_defs.h"

#ifdef DLL_METRIC

	/* Define DLL_METRIC to make the SGL DLL output it's own
	   metrics at the end of execution.						 */

	#define METRIC 1
	#define METRIC2 1

	/* This function is called from ResetRegionData in pmsabre.c
	   to allow renders to be counted.							*/

	void InResetRegionData( void );

#endif

#ifdef METRIC

	/*===========================================
	 * Function:	SglTimeNow
	 *===========================================
	 *
	 * Scope:		SGL
	 *
	 * Purpose:		to get a clock tick quickly and effeciently
	 *
	 * Params:		none
	 *
	 * Return:		unsigned int value being the current 32bit clock tick with ref
	 *		        to the CPU Speed.
	 *========================================================================================*/
	sgl_uint32  SglTimeNow(void);

	/*===========================================
	 * Function:	SglCPUFreq
	 *===========================================
	 *
	 * Scope:		SGL
	 *
	 * Purpose:		to return the CPU Frequency
	 *
	 * Params:		none
	 *
	 * Return:		float value being the the CPU frequency.
	 *========================================================================================*/
	float  SglCPUFreq(void);

	#if defined (MIDAS_ARCADE) || defined (ZEUS_ARCADE)

		/*===========================================
		 * Function:	SglMetricInit - ARCADE VERSION
		 *===========================================
		 *
		 * Scope:		SGL
		 *
		 * Purpose:		to initialise metrics stuff
		 *
		 * Params:		none
		 *
		 * Return:		none
		 *========================================================================================*/
		void  SglMetricInit(void);

	#endif

	typedef struct 
	{
		sgl_uint32 Start, Stop, Count, Total;
		#ifdef METRIC2
			sgl_uint32 Max, Stack;		/* VL RGI : Track nested calls dynamically */
		#endif
	} Temporal_Data;
 
	typedef enum 
	{ 
		DUMMY=0,
		TOTAL_APP_TIME, 
		TOTAL_RENDER_TIME, 
		TIMER_TIME,
		RENDER_WAITING_TIME, 
		PLANE_STUFF_TIME,
		MESH_NODE_TIME,	
		DATABASE_TRAVERSAL_TIME, 
		TRIVIAL_REJECTION_TIME, 
		TRANSFORM_FACES_ALL_TIME, 
		TRANSFORM_FACES_PARTLY_TIME, 
		TRANSFORM_EDGES_ALL_TIME, 
		TRANSFORM_EDGES_PARTLY_TIME, 
		TRANSFORM_VERTICES_ALL_TIME, 
		TRANSFORM_VERTICES_PARTLY_TIME, 
		TRANSFORM_COMPUTE_SABRE_ALL_TIME, 
		TRANSFORM_COMPUTE_SABRE_PARTLY_TIME, 
		TRANSFORM_VISIBLE_CONVEX_TIME,
		TRANSFORM_PLANES_TIME,
		DETERMINE_PLANES_REGION_TIME,
		DETERMINE_BBOX_REGION_TIME,
		ADD_REGION_OPAQUE_TIME,
		ADD_REGION_SHADOW_TIME,
		ADD_REGION_LIGHTVOL_TIME,
		ADD_REGION_TRANSLUC_TIME,
		PUT_PLANES_REGION_TIME,
		PROJECTION_TIME, 
		PROJECT_REP_TIME, 
		TEXTURE_TIME, 
		SMOOTH_OBJ_PRECALC_TIME, 
		SMOOTH_DATA_PRECALC_TIME, 
		SMOOTH_TRI_CLIP_PARAM_TIME, 
		SMOOTH_TRI_PARAM_TIME, 
		SMOOTH_ADJ_PARAM_TIME, 
		SMOOTH_PARAM_TIME, 
		FLAT_PARAM_TIME, 
		FLATTEXTURE_PARAM_TIME, 
		SHADOW_VOL_TIME,
		PROCESS_CONVEX_NODE_TIME,
		PROCESS_SHADOW_TIME,
		PACK_OPAQUE_TIME,
		PACK_MESH_TIME,
		PACK_QUADS_TIME,
		PACK_TRIANGLES_TIME,
		PACK_PLANE_TIME,
		PACK_20BIT_TIME,
		PACK_LIGHTSHADVOL_TIME,
		PACK_TEXAS_TIME,
		PACK_MESH_ORDERED_TIME,
		PACK_TEXAS_NT_TIME,
		PACK_TEXAS_FT_TIME,
		PACK_TEXAS_ST_TIME,
		PACK_TEXAS_TRI_TIME,
		PACK_TEXAS_TRINT_TIME,
		PACK_TEXAS_TRISH_TIME,
		PACK_TEXAS_TRILV_TIME,
		PACK_TEXAS_TRINTSH_TIME,
		PACK_TEXAS_TRINTLV_TIME,

		TEXTURE_PARAMETER_SETUP_TIME, 
		PARAMETER_TRANSFER_TIME, 
		SGLTRI_STARTOFFRAME1_TIME,
		SGLTRI_STARTOFFRAME2_TIME,
		GENERATE_TIME,
		SGLTRI_TRIANGLES_TIME,
		SGLTRI_PROCESS_TIME,
		SGLTRI_PACKTRI_TIME,
		PACK_ISPTRI_TIME,
		PACK_ISPCORE_TIME,
		INPUT_D3DTRI_COUNT,
		INPUT_D3DPOLY_COUNT,
		INPUT_LITEQUAD_COUNT,
		INPUT_LITETRI_COUNT,
		PERPOLYPROC_TIME,

		NUM_TIMERS						 
	} TIMER_TYPES;

	#ifdef SGL_APP

		typedef struct 
		{ 
			TIMER_TYPES type;
			char        * name;
		} TIMER_NAMES; 

		TIMER_NAMES timer_names[] = 
		{ 
			{ DUMMY,		  			            "DUMMY                               \0" },
			{ TOTAL_APP_TIME,  			            "TOTAL_APP_TIME                      \0" },
			{ TOTAL_RENDER_TIME,  		            "TOTAL_RENDER_TIME                   \0" },
			{ TIMER_TIME,						    "TIMER_TIME                          \0" },
			{ RENDER_WAITING_TIME,  	            "RENDER_WAITING_TIME                 \0" },
			{ PLANE_STUFF_TIME,						"PLANE_STUFF_TIME                    \0" },
			{ MESH_NODE_TIME,						"MESH_NODE_TIME                      \0" },
			{ DATABASE_TRAVERSAL_TIME,              "DATABASE_TRAVERSAL_TIME             \0" },
			{ TRIVIAL_REJECTION_TIME,               "TRIVIAL_REJECTION_TIME              \0" },
			{ TRANSFORM_FACES_ALL_TIME, 	        "TRANSFORM_FACES_ALL_TIME            \0" },
			{ TRANSFORM_FACES_PARTLY_TIME,	        "TRANSFORM_FACES_PARTLY_TIME         \0" },
			{ TRANSFORM_EDGES_ALL_TIME, 	        "TRANSFORM_EDGES_ALL_TIME            \0" },
			{ TRANSFORM_EDGES_PARTLY_TIME, 	        "TRANSFORM_EDGES_PARTLY_TIME         \0" },
			{ TRANSFORM_VERTICES_ALL_TIME,	        "TRANSFORM_VERTICES_ALL_TIME         \0" },
			{ TRANSFORM_VERTICES_PARTLY_TIME,       "TRANSFORM_VERTICES_PARTLY_TIME      \0" },
			{ TRANSFORM_COMPUTE_SABRE_ALL_TIME,     "TRANSFORM_COMPUTE_SABRE_ALL_TIME    \0" },
			{ TRANSFORM_COMPUTE_SABRE_PARTLY_TIME,  "TRANSFORM_COMPUTE_SABRE_PARTLY_TIME \0" }, 
			{ TRANSFORM_VISIBLE_CONVEX_TIME,        "TRANSFORM_VISIBLE_CONVEX_TIME       \0" }, 
			{ TRANSFORM_PLANES_TIME,				"TRANSFORM_PLANES_TIME               \0" },
			{ DETERMINE_PLANES_REGION_TIME,	        "DETERMINE_PLANES_REGION_TIME        \0" },
			{ DETERMINE_BBOX_REGION_TIME,           "DETERMINE_BBOX_REGION_TIME          \0" },
			{ ADD_REGION_OPAQUE_TIME,				"ADD_REGION_OPAQUE_TIME              \0" },
			{ ADD_REGION_SHADOW_TIME,		        "ADD_REGION_SHADOW_TIME              \0" },
			{ ADD_REGION_LIGHTVOL_TIME,		        "ADD_REGION_LIGHTVOL_TIME            \0" },
			{ ADD_REGION_TRANSLUC_TIME,		        "ADD_REGION_TRANSLUC_TIME            \0" },
			{ PUT_PLANES_REGION_TIME,		        "PUT_PLANES_REGION_TIME              \0" },
			{ PROJECTION_TIME,  			        "PROJECTION_TIME                     \0" },
			{ PROJECT_REP_TIME, 					"PROJECT_REP_TIME                    \0" }, 
			{ TEXTURE_TIME,  				        "TEXTURE_TIME                        \0" },
			{ SMOOTH_OBJ_PRECALC_TIME, 			    "SMOOTH_OBJ_PRECALC_TIME             \0" }, 
			{ SMOOTH_TRI_CLIP_PARAM_TIME, 			"SMOOTH_TRI_CLIP_PARAM_TIME          \0" }, 
			{ SMOOTH_DATA_PRECALC_TIME, 			"SMOOTH_DATA_PRECALC_TIME            \0" }, 
			{ SMOOTH_TRI_PARAM_TIME, 			    "SMOOTH_TRI_PARAM_TIME               \0" }, 
			{ SMOOTH_ADJ_PARAM_TIME, 			    "SMOOTH_ADJ_PARAM_TIME               \0" }, 
			{ SMOOTH_PARAM_TIME, 					"SMOOTH_PARAM_TIME                   \0" }, 
			{ FLAT_PARAM_TIME, 					    "FLAT_PARAM_TIME                     \0" },
			{ FLATTEXTURE_PARAM_TIME, 			    "FLATTEXTURE_PARAM_TIME              \0" },
			{ SHADOW_VOL_TIME,						"SHADOW_VOL_TIME                     \0" },
			{ PROCESS_CONVEX_NODE_TIME,				"PROCESS_CONVEX_NODE_TIME            \0" }, 
			{ PROCESS_SHADOW_TIME,				    "PROCESS_SHADOW_TIME                 \0" }, 
			{ PACK_OPAQUE_TIME,						"PACK_OPAQUE_TIME                    \0" },
			{ PACK_MESH_TIME,						"PACK_MESH_TIME                      \0" },
			{ PACK_QUADS_TIME,						"PACK_QUADS_TIME                     \0" },
			{ PACK_TRIANGLES_TIME,					"PACK_TRIANGLES_TIME                 \0" },
			{ PACK_PLANE_TIME,						"PACK_PLANE_TIME                     \0" },
			{ PACK_20BIT_TIME,						"PACK_20BIT_TIME                     \0" },
			{ PACK_LIGHTSHADVOL_TIME,				"PACK_LIGHTSHADVOL_TIME             \0" },
			{ PACK_TEXAS_TIME, 					    "PACK_TEXAS_TIME                     \0" },
			{ PACK_MESH_ORDERED_TIME,		        "PACK_MESH_ORDERED_TIME              \0" },
			{ PACK_TEXAS_NT_TIME,					"PACK_TEXAS_NT_TIME                  \0" },
			{ PACK_TEXAS_FT_TIME,                   "PACK_TEXAS_FT_TIME                  \0" },
			{ PACK_TEXAS_ST_TIME,					"PACK_TEXAS_ST_TIME                  \0" },
			{ PACK_TEXAS_TRI_TIME,					"PACK_TEXAS_TRI_TIME                 \0" },
			{ PACK_TEXAS_TRINT_TIME,				"PACK_TEXAS_TRINT_TIME               \0" },
			{ PACK_TEXAS_TRISH_TIME,				"PACK_TEXAS_TRISH_TIME               \0" },
			{ PACK_TEXAS_TRILV_TIME,				"PACK_TEXAS_TRILV_TIME               \0" },
			{ PACK_TEXAS_TRINTSH_TIME,			    "PACK_TEXAS_TRINTSH_TIME             \0" },
			{ PACK_TEXAS_TRINTLV_TIME,				"PACK_TEXAS_TRINTLV_TIME             \0" },
			{ TEXTURE_PARAMETER_SETUP_TIME,         "TEXTURE_PARAMETER_SETUP_TIME        \0" },
			{ PARAMETER_TRANSFER_TIME,  	        "PARAMETER_TRANSFER_TIME             \0" },
			{ SGLTRI_STARTOFFRAME1_TIME,			"SGLTRI_STARTOFFRAME1_TIME           \0" },
			{ SGLTRI_STARTOFFRAME2_TIME,			"SGLTRI_STARTOFFRAME2_TIME           \0" },
			{ GENERATE_TIME,                        "GENERATE_TIME                       \0" },
			{ SGLTRI_TRIANGLES_TIME,                "SGLTRI_TRIANGLES_TIME               \0" },
			{ SGLTRI_PROCESS_TIME,					"SGLTRI_PROCESS_TIME                 \0" },
			{ SGLTRI_PACKTRI_TIME,					"SGLTRI_PACKTRI_TIME                 \0" },
			{ PACK_ISPTRI_TIME,                     "PACK_ISPTRI_TIME                    \0" },
			{ PACK_ISPCORE_TIME,					"PACK_ISPCORE_TIME                   \0" },
			{ INPUT_D3DTRI_COUNT,					"INPUT_D3DTRI_COUNT                  \0" },
			{ INPUT_D3DPOLY_COUNT,					"INPUT_D3DPOLY_COUNT                 \0" },
			{ INPUT_LITEQUAD_COUNT,					"INPUT_LITEQUAD_COUNT                \0" },
			{ INPUT_LITETRI_COUNT,					"INPUT_LITETRI_COUNT                 \0" },
			{ PERPOLYPROC_TIME,    					"PERPOLYPROC_TIME                    \0" },
			{ NUM_TIMERS, 				            "NUM_TIMERS                          \0" } 
		}; 

		/*
			if you are building a windows app that wishes to import the Times
			array you need this declaration
		*/

		#if WIN32

			#include <windows.h>
			#define SGL_APP_DLLIMPORT_TIME __declspec dllimport Temporal_Data Times[NUM_TIMERS];

		#endif

	#endif

	#ifdef METRIC2

		#define SGL_TIME_RESET(X)      { Times[##X].Count = 0;  Times[##X].Total = 0; Times[##X].Start = 0;  Times[##X].Stop = 0; Times[##X].Max = 0; Times[##X].Stack = 0; TStack = 0;}
		#define SGL_TIME_START(X)      { 	if ( Times[##X].Stack != 0 ) { Times[##X].Count |= 0xE0000000L; }\
							if ( (Times[##X].Stack = TStack) != 0 ) { Times[TStack].Stop += ( SglTimeNow() - Times[TStack].Start ); } \
							Times[##X].Count += 1; Times[##X].Start = SglTimeNow(); Times[##X].Stop = 0; TStack = ##X; }
		#define SGL_TIME_SUSPEND(X)
		#define SGL_TIME_RESUME(X)
		#define SGL_TIME_STOP(X)       { if ( ##X != TStack ) { Times[TStack].Count |= 0x80000000L; Times[##X].Count |= 0xC0000000L; }\
						   Times[TStack].Stop += (SglTimeNow() - Times[TStack].Start);\
					       Times[TStack].Total += Times[TStack].Stop;\
					       Times[TStack].Max = (( Times[TStack].Max < Times[TStack].Stop ) ? Times[TStack].Stop : Times[TStack].Max);\
						   if ( (TStack = Times[##X].Stack) != 0 ) { Times[TStack].Start = SglTimeNow(); }\
					   Times[##X].Stack = 0; }

		/* In situations where you cannot stand the overheads of even the old
		metrics stuff as it was, use this to group up instances that need timing
		within a loop to register the time only once outside the loop. */
		#define SGL_TIME_FASTINIT(X)   Times[##X].Stop = 0;
		#define SGL_TIME_FASTENTER(X)	 Times[##X].Stop -= SglTimeNow();
		#define SGL_TIME_FASTEXIT(X)	 Times[##X].Stop += SglTimeNow();
		#define SGL_TIME_FASTDONE(X)   { Times[##X].Count += 1;\
					       Times[##X].Total += Times[##X].Stop;\
					       Times[##X].Max = (( Times[##X].Max < Times[##X].Stop ) ? Times[##X].Stop : Times[##X].Max);\
						   if ( TStack != 0 ) Times[TStack].Start += Times[##X].Stop; }

	#else

		#define SGL_TIME_RESET(X)      { Times[##X].Count = 0;  Times[##X].Total = 0; Times[##X].Start = 0;  Times[##X].Stop = 0; }
		#define SGL_TIME_START(X)      { Times[##X].Count += 1; Times[##X].Start = SglTimeNow(); }
		#define SGL_TIME_SUSPEND(X)    { Times[##X].Total += SglTimeNow() - Times[##X].Start; }
		#define SGL_TIME_RESUME(X)     { Times[##X].Start = SglTimeNow(); }
		#define SGL_TIME_STOP(X)       { Times[##X].Stop = SglTimeNow(); Times[##X].Total += Times[##X].Stop - Times[##X].Start; }

	#endif

	#define GET_TICK_FREQ(X)       { X =  SglCPUFreq();  }
	#define TOTAL_DIV_COUNT(X)     (Times[##X].Count == 0) ? 0.0f : (float)(Times[##X].Total / Times[##X].Count);

	#define SGL_TIME_READ_MS(X)    ((float)(Times[##X].Total))
	#define SGL_TIME_READ_COUNT(X) ((long)(Times[##X].Count)) 

	/* Different definitions and references to the time data */
	#ifdef METRIC2

		#define SGL_GLOBAL_TIME_DEFN   Temporal_Data Times[NUM_TIMERS]; int TStack = 0; /* for rnrender.c */
		#define SGL_EXTERN_TIME_REF    extern Temporal_Data Times[NUM_TIMERS]; extern int TStack; /* for sgl dll's */

	#else

		#define SGL_GLOBAL_TIME_DEFN   Temporal_Data Times[NUM_TIMERS];        /* for rnrender.c */
		#define SGL_EXTERN_TIME_REF    extern Temporal_Data Times[NUM_TIMERS]; /* for sgl dll's */

	#endif

	#define GET_TIME_NAME(X)       timer_names[##X].name 

#else /* METRIC */

	/*
	// XXXXX Compile Error: Update metrics.h
	*/

	typedef struct 
	{
		sgl_uint32 Start, Stop, Count, Total;
	}   Temporal_Data;

	#define NUM_TIMERS             1 
	#define SGL_TIME_RESET(X) 
	#define SGL_TIME_START(X) 
	#define SGL_TIME_SUSPEND(X)    
	#define SGL_TIME_RESUME(X)     
	#define SGL_TIME_STOP(X)       
	#define GET_TICK_FREQ(X)        
	#define TOTAL_DIV_COUNT(X)     0.0f 

	#define SGL_TIME_READ_MS(X)    0.0f  
	#define SGL_TIME_READ_COUNT(X) 0L 

	/* Different definitions and references to the time data */
	#define SGL_GLOBAL_TIME_DEFN   Temporal_Data Times[NUM_TIMERS];        /* for rnrender.c */
	#define SGL_EXTERN_TIME_REF    extern Temporal_Data Times[NUM_TIMERS]; /* for sgl dlls */


	#define GET_TIME_NAME(X)       0 

#endif /* __METRIC__ */

#ifndef SGL_TIME_FASTINIT

	#define SGL_TIME_FASTINIT(X)
	#define SGL_TIME_FASTENTER(X)
	#define SGL_TIME_FASTEXIT(X)
	#define SGL_TIME_FASTDONE(X)

#endif

#endif /* __METRIC_H__ */


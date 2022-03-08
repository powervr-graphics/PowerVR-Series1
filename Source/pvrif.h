/* PVRIF.H */

#ifdef API_START
#undef API_START
#endif

#ifdef API_END
#undef API_END
#endif

#ifdef API_FN
#undef API_FN
#endif

#ifndef CHARIZE
#define CHARIZE(x)	#x
#endif

#ifndef ODS
#define ODS(x,s) OutputDebugString(#x); OutputDebugString(s);
#endif 

#define API_GETFN(x)			Get##x##_IF
#define API_GETFNTYPE(x)		PFN_GET_##x##_IF
#define API_STRUCTTYPE(x)		x##_IF
#define API_PSTRUCTTYPE(x)		P##x##_IF
#define API_STRUCT(x)			s##x##_IF

#if defined (API_INSTANTIATE)

	#define API_START(x)	static API_STRUCTTYPE(x) API_STRUCT(x) = {
	#define API_END(x)		}; __declspec(dllexport) int CALL_CONV API_GETFN(x) (API_PSTRUCTTYPE(x) *ppIF)\
							{ \
								*ppIF = &API_STRUCT(x);\
								return (sizeof (API_STRUCTTYPE(x)));\
							}
	#define API_FN(ret,name,parms)		name,

#elif defined (API_JUMPTABLE)
									 
	#define API_START(x) 	typedef struct INSTANCEDATA \
							{\
								unsigned int		hProcess;\
								void *			 	hLib;\
								API_PSTRUCTTYPE(x)	pIf;\
							} INSTANCEDATA;\
							static INSTANCEDATA InstanceData;
	#define API_END(x)
#ifdef EMIT_ENTRY_POINTS
	#define API_FN(ret,name,parms)		__declspec(dllexport naked) void CALL_CONV name (void) \
										{ \
											  DPF((DBG_MESSAGE,"Entering %s",#name)); \
											__asm mov 	eax, DWORD PTR [InstanceData.pIf]\
											__asm jmp 	DWORD PTR [eax.pfn##name]\
										}
#else
	#define API_FN(ret,name,parms)		__declspec(dllexport naked) void CALL_CONV name (void) \
										{ \
											__asm mov 	eax, DWORD PTR [InstanceData.pIf]\
											__asm jmp 	DWORD PTR [eax.pfn##name]\
										}
#endif

#elif defined (API_JUMPTABLE_NOEXPORT)
									 
	#define API_START(x) 	typedef struct INSTANCEDATA \
							{\
								unsigned int		hProcess;\
								void *			 	hLib;\
								API_PSTRUCTTYPE(x)	pIf;\
							} INSTANCEDATA;\
							static INSTANCEDATA InstanceData;
	#define API_END(x)
	#define API_FN(ret,name,parms)		__declspec(naked) void CALL_CONV name (void) \
										{ \
											__asm mov 	eax, DWORD PTR [InstanceData.pIf]\
											__asm jmp 	DWORD PTR [eax.pfn##name]\
										}
#elif defined (API_FNBLOCK)

	#define API_START(x)				typedef struct API_STRUCTTYPE(x) {
	#define API_END(x)					} API_STRUCTTYPE(x), *API_PSTRUCTTYPE(x);\
										typedef int (CALL_CONV *API_GETFNTYPE(x)) (API_PSTRUCTTYPE(x) *ppIF);
	#define API_FN(ret,name,parms)		ret (CALL_CONV *pfn##name) parms;

#elif defined (API_TYPESONLY)

	#define API_START(x)
	#define API_END(x)
	#define API_FN(ret,name,parms)

#else

	#define API_START(x)
	#define API_END(x)
	#define API_FN(ret,name,parms)		ret CALL_CONV name parms;

#endif

/* end of $RCSfile: pvrif.h,v $ */

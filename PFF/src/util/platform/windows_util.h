#pragma once


#ifdef PFF_ENGINE
	#define PFF_API								__declspec(dllexport)
#else
	#define PFF_API								__declspec(dllimport)
#endif

//#ifdef PFF_ENGINE
//	#define PFF_HELPER_API						__declspec(dllexport)
//#elif defined(PFF_HELPER)
//	#define PFF_HELPER_API						__declspec(dllimport)
//#else
//	#define PFF_HELPER_API
//#endif

#ifdef PFF_ENGINE
	#define PFF_API_EDITOR						__declspec(dllexport)
#elif defined(PFF_EDITOR)
	#define PFF_API_EDITOR						__declspec(dllimport)
#else
	#define PFF_API_EDITOR	
#endif

#ifdef PFF_PROJECT
	#define PROJECT_API							extern "C" __declspec(dllexport)
#else
	#define PROJECT_API							__declspec(dllimport)
#endif

#if defined(PFF_ENGINE) || defined(PFF_EDITOR)
	#define USE_IN_EDITOR
#else
	#define USE_IN_EDITOR	
#endif

#define DEBUG_BREAK()							(__debugbreak())

#ifdef CDECL
	#undef CDECL
#endif

// Function type macros.
// Functions with variable arguments
#define VARARGS									__cdecl							
// Standard C function
#define CDECL									__cdecl							
// Standard calling convention
#define STDCALL									__stdcall						
// Force code to be inline
#define FORCEINLINE								__forceinline					
// Force code to NOT be inline
#define FORCENOINLINE							__declspec(noinline)			
// Indicate that the function never returns nullptr.
#define FUNCTION_NON_NULL_RETURN_START			_Ret_notnull_					

// Optimization macros (uses __pragma to enable inside a #define).
#if !defined(__clang__)
	#define PRAGMA_DISABLE_OPTIMIZATION_ACTUAL	__pragma(optimize("",off))
	#define PRAGMA_ENABLE_OPTIMIZATION_ACTUAL	__pragma(optimize("",on))
#elif defined(_MSC_VER)		// Clang only supports __pragma with -fms-extensions
	#define PRAGMA_DISABLE_OPTIMIZATION_ACTUAL	__pragma(clang optimize off)
	#define PRAGMA_ENABLE_OPTIMIZATION_ACTUAL	__pragma(clang optimize on)
#endif

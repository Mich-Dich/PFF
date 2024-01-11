#pragma once


#ifdef PFF_INSIDE_ENGINE
	#define PFF_API __declspec(dllexport)
#else
	#define PFF_API __declspec(dllimport)
#endif // PFF_INSIDE_ENGINE


#define DEBUG_BREAK() (__debugbreak())

#ifdef CDECL
	#undef CDECL
#endif

// Function type macros.
#define VARARGS     __cdecl											// Functions with variable arguments
#define CDECL	    __cdecl											// Standard C function
#define STDCALL		__stdcall										// Standard calling convention
#define FORCEINLINE __forceinline									// Force code to be inline
#define FORCENOINLINE __declspec(noinline)							// Force code to NOT be inline
#define FUNCTION_NON_NULL_RETURN_START _Ret_notnull_				// Indicate that the function never returns nullptr.


// Optimization macros (uses __pragma to enable inside a #define).
#if !defined(__clang__)
	#define PRAGMA_DISABLE_OPTIMIZATION_ACTUAL __pragma(optimize("",off))
	#define PRAGMA_ENABLE_OPTIMIZATION_ACTUAL  __pragma(optimize("",on))
#elif defined(_MSC_VER)		// Clang only supports __pragma with -fms-extensions
	#define PRAGMA_DISABLE_OPTIMIZATION_ACTUAL __pragma(clang optimize off)
	#define PRAGMA_ENABLE_OPTIMIZATION_ACTUAL  __pragma(clang optimize on)
#endif

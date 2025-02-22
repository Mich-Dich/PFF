#pragma once

#ifdef PFF_ENGINE
    #define PFF_API                             __attribute__((visibility("default")))
#else
    #define PFF_API                             
#endif

//#ifdef PFF_ENGINE
//    #define PFF_HELPER_API                     __attribute__((visibility("default")))
//#elif defined(PFF_HELPER)
//    #define PFF_HELPER_API                     
//#else
//    #define PFF_HELPER_API
//#endif

#ifdef PFF_ENGINE
    #define PFF_API_EDITOR                      __attribute__((visibility("default")))
#elif defined(PFF_EDITOR)
    #define PFF_API_EDITOR                      
#else
    #define PFF_API_EDITOR	
#endif

#ifdef PFF_PROJECT
    #define PROJECT_API                         extern "C" __attribute__((visibility("default")))
#else
    #define PROJECT_API                         extern "C"
#endif

#if defined(PFF_ENGINE) || defined(PFF_EDITOR)
    #define USE_IN_EDITOR
#else
    #define USE_IN_EDITOR	
#endif

#define DEBUG_BREAK()                          __builtin_trap()

#ifdef CDECL
    #undef CDECL
#endif

// Function type macros.
// Functions with variable arguments (not directly supported in C++)
#define VARARGS                                 
// Standard C function (default in C++)
#define CDECL                                   
// Standard calling convention (not directly supported in C++)
#define STDCALL                                 
// Force code to be inline
#define FORCEINLINE                             inline __attribute__((always_inline))
// Force code to NOT be inline
#define FORCENOINLINE                           __attribute__((noinline))
// Indicate that the function never returns nullptr.
#define FUNCTION_NON_NULL_RETURN_START          __attribute__((returns_nonnull))

// Optimization macros.
#if defined(__clang__)
    #define PRAGMA_DISABLE_OPTIMIZATION_ACTUAL   _Pragma("clang optimize off")
    #define PRAGMA_ENABLE_OPTIMIZATION_ACTUAL    _Pragma("clang optimize on")
#else
    #define PRAGMA_DISABLE_OPTIMIZATION_ACTUAL   _Pragma("GCC optimize off")
    #define PRAGMA_ENABLE_OPTIMIZATION_ACTUAL    _Pragma("GCC optimize on")
#endif
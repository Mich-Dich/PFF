
#ifdef PFF_PLATFORM_WINDOWS
	#ifdef PFF_INSIDE_ENGINE
			#define PFF_API __declspec(dllexport)
		#else
			#define PFF_API __declspec(dllimport)
	#endif // PFF_INSIDE_ENGINE
#endif // PFF_PLATFORM_WINDOWS

#define BIT(x) (1 << x)

#define GET_VAR_NAME_STR(x) #x

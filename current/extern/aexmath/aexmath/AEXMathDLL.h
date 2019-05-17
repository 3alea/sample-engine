#ifndef AEX_NO_DLL
	#ifdef AEX_MATH_DLL_EXPORT
		#define AEX_MATH_API __declspec(dllexport)
	#else
		#define AEX_MATH_API __declspec(dllimport)
	#endif
#else
	#define AEX_MATH_API 
#endif
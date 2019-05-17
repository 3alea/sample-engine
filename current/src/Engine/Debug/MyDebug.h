#ifndef MY_DEBUG_H_
#define MY_DEBUG_H_

// This is stupid! But it can be done. This is just to show that you can undefine macros
// NOTE: _DEBUG is defined in the VS project propeties under C++ > Preprocessor
#ifdef _DEBUG
	#undef _DEBUG
	#define _DEBUG
#endif

// define logging (this can also be done through the VS project properties as explained above)
#define _LOG

#ifdef _DEBUG
	#ifdef _LOG
		// __VA_ARGS__ is a special macro that replaces all the variadic arguments passed to the macro function
		// https://msdn.microsoft.com/en-us/library/ms177415.aspx
		#define DebugPrint(...) MyLogPrintFunction(__VA_ARGS__) 
	#else
		#define DebugPrint(...) MyPrintFunction(__VA_ARGS__)
	#endif 

	#define DebugAssert(x, ...) MyAssertFunction((x), __FILE__, __LINE__, __VA_ARGS__)
	#define DebugErrorIf(x, ...)MyErrorIfFunction((x), __FILE__, __LINE__, __VA_ARGS__)
	

#else
#define DebugPrint(...)
#define DebugAssert(x, ...) MyAssertFunction((x), __FILE__, __LINE__, __VA_ARGS__)
#define DebugErrorIf(x, ...)MyErrorIfFunction((x), __FILE__, __LINE__, __VA_ARGS__)
#endif 


/* @TODO
	\fn		MyPrintFunction
	\brief	Prints the formatted string (like printf) to the stdout
			and to the Visual Studio Output window.
*/
extern void  MyPrintFunction(const char * format, ...);

/* @TODO
	\fn		MyLogPrintFunction
	\brief	Same as above but also writes to a file called Log.txt
*/
extern void  MyLogPrintFunction(const char * format, ...);


/*! @TODO
	\fn		ShowConsole
	\brief	Creates a console to display text.
*/
extern void  ShowConsole();

/*! @TODO
	\fn		HideConsole
	\brief	Hides (actually deletes) the console created using the function above.
*/
extern void  HideConsole();

/*! @TODO
	\fn		MyAssertFunction
	\brief	This function displays an error message box if the value of 'expression' is false.
*/
extern bool  MyAssertFunction(bool expression, const char * filename, int line, const char * format, ...);


/*! @TODO
\fn		MyErrorIfFunction
\brief	This function displays an error message box if the value of 'expression' is true.
*/
extern bool  MyErrorIfFunction(bool expression, const char * filename, int line, const char * format, ...);

extern void  Win32ShowLastError(const char * fnName);

// ----------------------------------------------------------------------------
#endif // MY_DEBUG_H_
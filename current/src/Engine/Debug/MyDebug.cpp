#include <Windows.h>
#include <stdio.h>
#include "MyDebug.h"
#include <strsafe.h>		// WIN32 DEBUG


#define MY_DEBUG_BUFFER_SIZE 1 << 12

/* @TODO
\fn		MyPrintFunction
\brief	Prints the formatted string (like printf) to the stdout
		and to the Visual Studio Output window. Note that this function
		should use vsprintf_s (http://www.cplusplus.com/reference/cstdio/vsprintf/)
*/
void MyPrintFunction(const char * format, ...)
{
	// temp buffer
	static char buffer[MY_DEBUG_BUFFER_SIZE] = { 0 };

	// extract the argument
	va_list args;
	va_start(args, format);
	vsprintf_s(buffer, sizeof(buffer), format, args);
	va_end(args);


	printf(buffer);	// print to stdout
	::OutputDebugString(buffer);	// print to debug console

}

/* @TODO
\fn		MyLogPrintFunction
\brief	Same as above but also writes to a file called Log.txt
*/
void MyLogPrintFunction(const char * format, ...)
{
	// variables
	static char buffer[MY_DEBUG_BUFFER_SIZE] = { 0 };	// temp buffer to write the message into
	static FILE * pLogFile = NULL;		// log file pointer

	// this will only happen once since pLogFile is static
	if (NULL == pLogFile)
	{
		fopen_s(&pLogFile, "Log.txt", "wt");
		fclose(pLogFile);
	}

	// extract the arguments & write to temp buffer
	va_list args;
	va_start(args, format);
	vsprintf_s(buffer, sizeof(buffer), format, args);
	va_end(args);

	// open the log file for appending text
	fopen_s(&pLogFile, "Log.txt", "at");

	// PRINT

	printf(buffer);					// print to stdout
	::OutputDebugString(buffer);	// print to debug console
	fprintf_s(pLogFile, buffer);	// print to file

	// close the log file
	fclose(pLogFile);
}

/*! @TODO
\fn		ShowConsole
\brief	Creates a console to display text.
*/
void ShowConsole()
{
	if (::AllocConsole())
	{
		FILE * fp;
		// TODO: redirect standard output streams to go to console output
		freopen_s(&fp, "CONOUT$", "wt", stdout);
		freopen_s(&fp, "CONOUT$", "wt", stderr);
		freopen_s(&fp, "CONIN$", "rt", stdin);
	}
	else
	{
		// TODO: Print an error message. 
	}
}

/*! @TODO
\fn		HideConsole
\brief	Hides (actually deletes) the console created using the function above.
*/
void HideConsole()
{
	::FreeConsole();
}

// helper function for the assert function
void TrimCurrentDirectoryPath(const char * filename, char * output, int output_size)
{
	static char buffer[MY_DEBUG_BUFFER_SIZE] = { 0 };

	// get the current directory path using win32 API
	// https://msdn.microsoft.com/en-us/library/windows/desktop/aa364934(v=vs.85).aspx
	::GetCurrentDirectory(sizeof(buffer), buffer);

	// convert the return value to lower case
	for (size_t i = 0; i < strlen(buffer); ++i)
		buffer[i] = tolower((int)buffer[i]);

	// get the minimum string length between the current directory and the filename
	// although we know that the filename is always be in the current directory, it is
	// good practice to safeguard our code against out of bounds array accesses
	int min_length = strlen(buffer) < strlen(filename) ? strlen(buffer) : strlen(filename);

	// compare each character from the filename and the directory until 
	// we find one that is different
	int first_different_char_index = 0;
	for (int i = 0; i < min_length; ++i, ++first_different_char_index)
	{
		if (buffer[i] != filename[i])
		{
			break;
		}
	}

	// we know the first character that is different. Simply copy the filename starting 
	// that character
	strcpy_s(output, output_size, filename + first_different_char_index);
}

/*! @TODO
\fn		MyAssertFunction
\brief	This function displays an error message box if the value of 'expression' is false.
*/
bool MyAssertFunction(bool expression, const char * filename, int line, const char * format, ...)
{
	if (!expression)
	{
		// temp string buffers
		static char buffer[MY_DEBUG_BUFFER_SIZE] = { 0 };
		static char buffer2[MY_DEBUG_BUFFER_SIZE] = { 0 };
		static char trimmed_filename[MY_DEBUG_BUFFER_SIZE] = { 0 };

		// (optional): process the filename to remove the path to the current directory
		TrimCurrentDirectoryPath(filename, trimmed_filename, sizeof(trimmed_filename));

		// extract the arguments and print formatted into a string.
		va_list args;
		va_start(args, format);
		vsprintf_s(buffer, sizeof(buffer), format, args);
		va_end(args);

		//insert the file and line this assert came from
		sprintf_s(buffer2, sizeof(buffer2), "FILE:\t%s\nLINE:\t%i\nMESSAGE:\n\n\"%s\"", trimmed_filename, line, buffer);

		// The message box function wil show a message box and will halt the execution until the user
		// presses one of the buttons shown in the message box. Different combinations of buttons
		// can be displayed in the message box but you can't fully cusomize them. Just choose from a lit
		// the list is explained here: https://msdn.microsoft.com/en-us/library/windows/desktop/ms645505(v=vs.85).aspx

		int mbResult = ::MessageBox(NULL,				// ignore this for now
									buffer2,			// the text to display in the window
									"ASSERT FAILED!",	// the title of the message box
									MB_ABORTRETRYIGNORE | MB_ICONSTOP); // the style of the message box

		switch (mbResult)
		{
		case IDABORT: // The abort button was pressed
			exit(1);
			break;
		case IDRETRY: // The retry button was pressed
			__debugbreak(); // just break the execution
			break;
		case IDIGNORE: // The ignore button was pressed
			return !expression;
		}
	}
	return !expression;
}

/*! @TODO
\fn		MyErrorIfFunction
\brief	This function displays an error message box if the value of 'expression' is true.
*/
bool MyErrorIfFunction(bool expression, const char * filename, int line, const char * format, ...)
{
	if (expression)
	{
		// temp string buffers
		static char buffer[MY_DEBUG_BUFFER_SIZE] = { 0 };
		static char buffer2[MY_DEBUG_BUFFER_SIZE] = { 0 };
		static char trimmed_filename[MY_DEBUG_BUFFER_SIZE] = { 0 };

		// (optional): process the filename to remove the path to the current directory
		TrimCurrentDirectoryPath(filename, trimmed_filename, sizeof(trimmed_filename));

		// extract the arguments and print formatted into a string.
		va_list args;
		va_start(args, format);
		vsprintf_s(buffer, sizeof(buffer), format, args);
		va_end(args);

		//insert the file and line this assert came from
		sprintf_s(buffer2, sizeof(buffer2), "FILE:\t%s\nLINE:\t%i\nMESSAGE:\n\n\"%s\"", trimmed_filename, line, buffer);

		// The message box function wil show a message box and will halt the execution until the user
		// presses one of the buttons shown in the message box. Different combinations of buttons
		// can be displayed in the message box but you can't fully cusomize them. Just choose from a lit
		// the list is explained here: https://msdn.microsoft.com/en-us/library/windows/desktop/ms645505(v=vs.85).aspx

		int mbResult = ::MessageBox(NULL,				// ignore this for now
			buffer2,			// the text to display in the window
			"ERROR!",			// the title of the message box
			MB_ABORTRETRYIGNORE | MB_ICONSTOP); // the style of the message box

		switch (mbResult)
		{
		case IDABORT: // The abort button was pressed
			exit(1);
			break;
		case IDRETRY: // The retry button was pressed
			__debugbreak(); // just break the execution
			break;
		case IDIGNORE: // The ignore button was pressed
			return expression;
		}
	}
	return expression;
}

void Win32ShowLastError(const char * fnName)
{
	// Retrieve the system error message for the last-error code
	LPSTR lpszFunction = (LPSTR)fnName;
	LPVOID lpMsgBuf;
	LPVOID lpDisplayBuf;
	DWORD dw = GetLastError();

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dw,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf,
		0, NULL);

	// Display the error message and exit the process

	lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT,
		(lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(TCHAR));
	StringCchPrintf((LPTSTR)lpDisplayBuf,
		LocalSize(lpDisplayBuf) / sizeof(TCHAR),
		TEXT("%s failed with error %d: %s"),
		lpszFunction, dw, lpMsgBuf);
	MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK);

	LocalFree(lpMsgBuf);
	LocalFree(lpDisplayBuf);
	ExitProcess(dw);
}
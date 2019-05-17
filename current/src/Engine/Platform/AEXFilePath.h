// ----------------------------------------------------------------------------
#ifndef FILE_PATH_H_
#define FILE_PATH_H_
// ----------------------------------------------------------------------------

#include "..\Core\AEXDataTypes.h"
#include "..\Utilities\AEXContainers.h"
#include <string>
// ----------------------------------------------------------------------------
// FilePath
// ----------------------------------------------------------------------------
#pragma warning (disable:4251) // dll and STL
namespace AEX
{
	struct  FilePath
	{
		std::string mDirectory;
		std::string mExtension;
		std::string mFilename;
		std::string mFullPath;

		// CTOR
		FilePath();
		FilePath(const char * path);

		// PARSE OPERATION
		void ParsePath(const char * path = NULL);

		// FILE META DATA
		s64	GetLastWriteTime();

		// Folder
		u32	ListFiles(AEX_LIST(AEX_STRING) & outList, const char * searchPattern = "*.*", bool recursive = false) const;
		u32	ListFiles(AEX_ARRAY(AEX_STRING) & outList, const char * searchPattern = "*.*", bool recursive = false) const;
		bool IsFolder() const;
		static bool Exists(const char * fullPath);
		static std::string GetCurrentDir();
		static bool Copy(const char * src_path, const char * dst_path, bool fail_if_exists);
		static bool Delete(const char * path);
		static bool CreateDir(const char * path);
	};
}
#pragma warning (default:4251) // dll and STL
// ----------------------------------------------------------------------------
#endif

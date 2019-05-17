#include "AEXFilePath.h"
#include <Windows.h>
#include <Shlwapi.h> // path file exits	
#include "..\Debug\MyDebug.h"
namespace AEX
{
	// ----------------------------------------------------------------------------
	// ----------------------------------------------------------------------------
	// FilePath
	FilePath::FilePath()
	{}
	FilePath::FilePath(const char * path)
	{
		// Parse
		ParsePath(path);
	}
	void FilePath::ParsePath(const char * path)
	{
		if (path != nullptr)
			mFullPath = path;

		// safeguard
		if (mFullPath.length() == 0)
			return;

		// replace the '/' character with '\' for unity
		std::size_t slash_pos = mFullPath.find_first_of("/");
		while (slash_pos != std::string::npos)
		{
			mFullPath.replace(slash_pos, 1, "\\");
			slash_pos = mFullPath.find_first_of("/");
		}

		// parse the mFullPath and extract

		// mDirectory
		std::size_t dir_offset = mFullPath.find_last_of("\\");
		mDirectory = mFullPath.substr(0, dir_offset + 1);

		// mExtension
		std::size_t ext_offset = mFullPath.find_last_of(".");
		if (ext_offset != std::string::npos) {
			mExtension = mFullPath.substr(ext_offset);
			// mFilename
			mFilename = mFullPath.substr(dir_offset + 1, ext_offset - dir_offset - 1);
		}
		else
		{
			mFilename = mFullPath.substr(dir_offset + 1);
		}
	}

	s64 FilePath::GetLastWriteTime()
	{
		// safeguard
		if (mFullPath.length() == 0)
			return 0;

		// Open file
		HANDLE hFN = CreateFile(
			mFullPath.c_str(),
			GENERIC_READ,
			FILE_SHARE_WRITE | FILE_SHARE_READ | FILE_SHARE_DELETE,
			NULL,
			OPEN_EXISTING,
			FILE_FLAG_BACKUP_SEMANTICS,
			NULL
		);

		// Error = invalide handle value...
		if (hFN != INVALID_HANDLE_VALUE)
		{
			::FILETIME lastWriteTime;
			if (GetFileTime(hFN, 0, 0, &lastWriteTime) == 0)
			{
				Win32ShowLastError("FilePath::LastAccessTime");
			}
			::CloseHandle(hFN);

			// return as s64. 
			// Note: address of lastWriteTime is guaranteed 
			//		to point to first member variable. 
			return *((s64*)&lastWriteTime);
		}
		return 0;
	}

	void GetAllFilesFromDirectoryRec(const char * dir, AEX_LIST(std::string) & outList, const char * pattern, bool recursive)
	{
		if (dir == NULL)
			return;

		WIN32_FIND_DATA ffd;
		HANDLE hFind = INVALID_HANDLE_VALUE;
		DWORD dwError = 0;
		std::string patternStr;
		if (pattern)
			patternStr = pattern;

		// Prepare string for use with FindFile functions.  First, copy the
		// string to a buffer, then append '\*' to the directory name.

		// Find the first file in the directory.
		std::string find = dir; find += "*";
		hFind = FindFirstFile(find.c_str(), &ffd);
		if (INVALID_HANDLE_VALUE == hFind)
		{
			return;
		}

		// List all the files in the directory with some info about them.
		do
		{
			std::string tmp = dir; tmp += ffd.cFileName;

			// ignore hidden files and folders
			if (ffd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN)
				continue;
			std::string fn = ffd.cFileName;
			if (fn == "." || fn == "..") // ignore this and up folders. and hidden svn foler
				continue;

			if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
			{
				tmp += "/";
				if (recursive) {
					GetAllFilesFromDirectoryRec(tmp.c_str(), outList, pattern, recursive);
				}
			}
			//else
			{

				if (patternStr.empty() ||
					patternStr == "*.*" ||
					patternStr == "*")
					outList.push_back(tmp); // add anything
				else // pattern isn't empty
				{
					// replace all * in there with nothing
					std::size_t star_pos = patternStr.find_first_of("*");
					while (star_pos != std::string::npos)
					{
						patternStr.erase(star_pos, 1);
						star_pos = patternStr.find_first_of("*");
					}

					if (tmp.find(patternStr) != std::string::npos)
						outList.push_back(tmp);
				}
			}
		} while (FindNextFile(hFind, &ffd) != 0);

		dwError = GetLastError();
		if (dwError != ERROR_NO_MORE_FILES)
		{
			;// DisplayErrorBox(TEXT("FindFirstFile"));
		}

		FindClose(hFind);
	}
	u32	FilePath::ListFiles(AEX_LIST(AEX_STRING) & outList, const char * searchPattern, bool recursive) const
	{
		u32 count = outList.size(); // store the count of files already in the list before searching
		GetAllFilesFromDirectoryRec(mDirectory.c_str(), outList, searchPattern, recursive);
		return outList.size() - count; // return the total of files found (exclude the ones that were already there)
	}
	u32	FilePath::ListFiles(AEX_ARRAY(AEX_STRING) & outVec, const char * searchPattern, bool recursive) const
	{
		u32 count = outVec.size(); // store the count of files already in the list before searching
		AEX_LIST(AEX_STRING) outList;
		GetAllFilesFromDirectoryRec(mDirectory.c_str(), outList, searchPattern, recursive);
		FOR_EACH(file, outList)outVec.push_back(*file);
		return outVec.size() - count; // return the total of files found (exclude the ones that were already there)
	}
	bool FilePath::IsFolder() const
	{
		return mFilename.empty() && !mDirectory.empty();
	}
	bool FilePath::Exists(const char* fullPath)
	{
		return (BOOL)1 == ::PathFileExists(fullPath);
	}
	std::string FilePath::GetCurrentDir()
	{
		static char buffer[512];
		::GetCurrentDirectory(512, buffer);
		return std::string(buffer);
	}

	namespace internal {
		bool CopyFileErrorStr(const char * src_path, const char * dst_path, bool fail_if_exists = false)
		{
			BOOL fail = fail_if_exists ? TRUE : FALSE;
			if (!::CopyFile(src_path, dst_path, fail)) {

				DWORD dw = ::GetLastError();
				switch (dw)
				{
				case ERROR_ACCESS_DENIED:
					DebugPrint("AEX::FilePath::CopyDirectory: Couldn't copy file %s\n\t%s\n\t%s\n", "ERROR_ACCESS_DENIED", src_path, dst_path);
					break;
				case ERROR_ENCRYPTION_FAILED:
					DebugPrint("AEX::FilePath::CopyDirectory: Couldn't copy file %s\n\t%s\n\t%s\n", "ERROR_ENCRYPTION_FAILED", src_path, dst_path);
					break;
				default:
					DebugPrint("AEX::FilePath::CopyDirectory: Couldn't copy file %s\n\t%s\n\t%s\n", "UNKNOWN ERROR", src_path, dst_path);
					break;
				}
				return false;
			}
			return true;
		}
		bool CopyDirectory(const char * src_path, const char * dst_path, bool recursive, bool fail_if_exists)
		{
			if (!FilePath::Exists(src_path))
				return false;

			if (!FilePath::Exists(dst_path))
				::CreateDirectory(dst_path, 0);

			FilePath dst_fp = FilePath(dst_path);

			// get the list of the files.
			std::vector<std::string>outFiles;
			FilePath(src_path).ListFiles(outFiles);

			for (u32 i = 0; i < outFiles.size(); ++i)
			{
				auto & file = outFiles[i];

				FilePath fp(file.c_str());
				if (fp.IsFolder())
				{
					// remove last char which is assumed to be a slash
					auto dir = fp.mDirectory.substr(0, fp.mDirectory.length() - 1);

					// get the last slash
					auto pos = dir.find_last_of("\\");
					if (pos == std::string::npos)
						pos = dir.find_last_of("/");


					std::string last_dir = dir.substr(pos + 1);
					std::string new_sub_path = dst_fp.mDirectory + last_dir + "/";
					if (!FilePath::Exists(new_sub_path.c_str()))
						::CreateDirectory(new_sub_path.c_str(), 0);
					if (recursive)
					{
						CopyDirectory(fp.mFullPath.c_str(), new_sub_path.c_str(), recursive, fail_if_exists);
					}
				}
				else // is file
				{
					std::string new_sub_path = dst_fp.mDirectory + fp.mFilename + fp.mExtension;
					internal::CopyFileErrorStr(file.c_str(), new_sub_path.c_str(), fail_if_exists);
				}
			}

			return true;
		}
	}
	bool FilePath::Copy(const char * src_path, const char * dst_path, bool fail_if_exists)
	{
		// validate source
		if (FilePath::Exists(src_path))
		{
			// fail if exists
			if (FilePath::Exists(dst_path) && fail_if_exists)
				return false;

			// check whether it's a file or a dir
			if (FilePath(src_path).IsFolder())
			{
				// copy the directory
				return internal::CopyDirectory(src_path, dst_path, true, fail_if_exists);
			}
			else
			{
				return internal::CopyFileErrorStr(src_path, dst_path,fail_if_exists);
			}
		}
		return false;
	}
	bool DeleteDirectory(const char * path)
	{
		// make sure that the directory exists
		if (FilePath::Exists(path) == false)
			return false;
		if (FilePath(path).IsFolder() == false)
			return false;

		// list all files in the directory
		AEX_ARRAY(std::string) outVec;
		FilePath(path).ListFiles(outVec);

		// delete each file in the folder
		for (u32 i = 0; i < outVec.size(); ++i)
		{
			// get the file or folder name
			const char * fileStr = outVec[i].c_str();

			// recursively delete the all the folders
			if (FilePath(fileStr).IsFolder())
			{
				// delete the directory recursively
				if (!DeleteDirectory(fileStr))
					return false;
			}
			else
			{
				// delete the file
				if (!::DeleteFile(fileStr)) {
					Win32ShowLastError("delete directory:: delete file");
					return false;
				}
			}
		}

		// delete the folder now that it's empty
		if (!::RemoveDirectory(path))
		{
			Win32ShowLastError("delete directory:: delete directory");
		}

		// success
		return true;
	}
	bool FilePath::Delete(const char * path)
	{
		if (FilePath::Exists(path))
		{
			if (FilePath(path).IsFolder())
			{
				if (DebugAssert(DeleteDirectory(path), "Delete:: Directory can't be deleted"))
					return false;
			}
			else
			{
				if (!::DeleteFile(path))
				{
					Win32ShowLastError("Delete File");
					return false;
				}
			}
			return true;
		}
		return false;
	}
	bool FilePath::CreateDir(const char * path)
	{
		// sanity check
		if( DebugAssert(FilePath::Exists(path) == false, "CreateDir: folder already exists \nfolder: %s", path))
			return false;

		// create the directory
		::CreateDirectory(path,0);

		// success
		return true;
	}
}
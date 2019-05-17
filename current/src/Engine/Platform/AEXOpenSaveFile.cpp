///////////////////////////////////////////////////////////////////////////////////////
///
///	\file OpenSaveFile.cpp Defines the base graphics component .
///
///	Authors:  Thomas Komair
///	Copyright 2011, Digipen Institute of Technology
/// TODO(Thomas): Test the following:
/// 1- Save with no extension
/// 2- Save with incorrect extension.
/// 3- Save with correct extension.
/// 4- Save with multiple extensions (filename.pdf.jpg.meta)
///
///////////////////////////////////////////////////////////////////////////////////////
#include "AEXOpenSaveFile.h"
#include "AEXFilePath.h"
#include <Windows.h>	// WIN32 API
#include <Shobjidl.h>	// Shell API (IFileDialog)
#include <locale>		// std::wstring_convert. Needed to convert between wstring (win32 API) and string (our API)
#include <codecvt>		// std::codecvt_utf8. Needed to convert between wstring (win32 API) and string (our API)
#include <list>			// std::string
// ----------------------------------------------------------------------------
// HELPERS

namespace AEX
{

	// ----------------------------------------------------------------------------
	/// \fn		FromWString
	/// \brief	Converts a wide character string (std::wstring) to a multi-byte 
	///			string (std::string)
	std::string FromWString(const std::wstring & in)
	{
		//setup converter
		using convert_type = std::codecvt_utf8 < wchar_t >;
		std::wstring_convert<convert_type, wchar_t> converter;

		//use converter (.to_bytes: wstr->str, .from_bytes: str->wstr)
		std::string converted_str = converter.to_bytes(in);

		// return
		return converted_str;
	}

	// ----------------------------------------------------------------------------
	/// \fn		ToWString
	/// \brief	Converts a multi-byte string (std::string) to a wide character 
	///			string (std::wstring)
	std::wstring ToWString(const std::string & in)
	{
		//setup converter
		using convert_type = std::codecvt_utf8 < wchar_t >;
		std::wstring_convert<convert_type, wchar_t> converter;

		//use converter (.to_bytes: wstr->str, .from_bytes: str->wstr)
		std::wstring converted_str = converter.from_bytes(in);

		// return
		return converted_str;
	}


	bool ExtensionsToWStringList(const char * extensions_cstr, std::list<std::wstring>& outList)
	{
		if (extensions_cstr)//sanity check
		{
			std::string extensions = extensions_cstr;
			// find the first extension and loop until
			// all are extracted
			std::size_t loc = extensions.find_first_of(";");
			while (loc != -1)// we only have one. 
			{
				std::string ext = extensions.substr(0, loc);
				outList.push_back(ToWString(ext));
				extensions = extensions.substr(loc + 1);
				loc = extensions.find_first_of(";");
			}
			outList.push_back(ToWString(extensions));
			// success
			return true;
		}
		return false;// failure
	}

	// ----------------------------------------------------------------------------
	/// \fn		SetFileTypesForDialog
	/// \brief	Processes a string representing file types and sets them 
	///			
	HRESULT		SetDialogFileTypes(IFileDialog * pfd, const std::list<std::wstring>& extension_list, COMDLG_FILTERSPEC *& rgSpec, UINT default_index = 0)
	{
		// https://msdn.microsoft.com/en-us/library/windows/desktop/bb775980(v=vs.85).aspx
		// https://msdn.microsoft.com/en-us/library/windows/desktop/bb773221(v=vs.85).aspx
		// allocate memory for the new register
		rgSpec = new COMDLG_FILTERSPEC[extension_list.size()];
		int i = 0;
		for (auto it = extension_list.begin();
			it != extension_list.end();
			++it, ++i)
		{
			rgSpec[i].pszName = it->c_str();
			rgSpec[i].pszSpec = it->c_str();
		}
		HRESULT hr = pfd->SetFileTypes(extension_list.size(), rgSpec);// pass all valid extension
		if (!SUCCEEDED(hr))
			return hr;

		if (default_index < extension_list.size()){
			hr = pfd->SetFileTypeIndex(default_index + 1);	// select the first extension
			//										 ^^^^
			// This is a one based-index instead of a zero based index!!! 
			// WHY MS WHY? https://msdn.microsoft.com/en-us/library/windows/desktop/bb775958(v=vs.85).aspx
		}
		return hr;
	}
	// ----------------------------------------------------------------------------
	//								HELPERS
	// ----------------------------------------------------------------------------


	// ----------------------------------------------------------------------------
	/// \fn		Destructor
	OpenSaveFileDlg::OpenSaveFileDlg()
	{}

	// ----------------------------------------------------------------------------
	/// \fn		Destructor
	OpenSaveFileDlg::~OpenSaveFileDlg()
	{}

	// ----------------------------------------------------------------------------
	/// \fn		Open
	/// \brief	Shows an open file dialog with the specified title
	/// \param	title - The title of the dialog
	/// \param	valid_extensions - A null-terminated string that contains
	///			the valid extensions for filtering the files to open. Each extension
	///			is separated by a ";"e.g.: "png;jpg;tga". If null, no filtering
	///			will be applied.
	bool OpenSaveFileDlg::Open(const char * title, const char * valid_extensions)
	{
		// Used to extract the extensions (if any)
		std::list<std::wstring> extension_list;
		COMDLG_FILTERSPEC * rgSpec = NULL;

		// CoCreate the File Open Dialog object.
		IFileDialog *pfd = NULL;
		HRESULT hr = CoCreateInstance(CLSID_FileOpenDialog,
			NULL,
			CLSCTX_INPROC_SERVER,
			IID_PPV_ARGS(&pfd));
		if (SUCCEEDED(hr))
		{
			// set the title
			pfd->SetTitle(ToWString(title).c_str());

			// Set extensions
			if (valid_extensions)
			{
				// extract the extensions
				ExtensionsToWStringList(valid_extensions, extension_list);

				// Set the file types
				SetDialogFileTypes(pfd, extension_list, rgSpec, 0);
			}

			// Set the options on the dialog.
			// Before setting, always get the options first in order 
			// not to override existing options.
			DWORD dwFlags;
			if (SUCCEEDED(pfd->GetOptions(&dwFlags)))
				// In this case, get shell items only for file system items.
				hr = pfd->SetOptions(dwFlags | FOS_FORCEFILESYSTEM | FOS_STRICTFILETYPES);

			// Show the dialog
			hr = pfd->Show(NULL);

			if (SUCCEEDED(hr))
			{
				// Obtain the result once the user clicks 
				// the 'Open' button.
				// The result is an IShellItem object.
				IShellItem *psiResult;
				hr = pfd->GetResult(&psiResult);
				if (SUCCEEDED(hr))
				{
					// We are just going to print out the 
					// name of the file for sample sake.
					PWSTR pszFilePath = NULL;
					hr = psiResult->GetDisplayName(SIGDN_FILESYSPATH,
						&pszFilePath);

					if (SUCCEEDED(hr))
					{
						std::string filepath = FromWString(pszFilePath);

						// get the current extension
						if (valid_extensions)
						{
							// get the current extension index
							UINT selectedExtensionIdx;
							pfd->GetFileTypeIndex(&selectedExtensionIdx);
							selectedExtensionIdx -= 1;	// This is a one based-index instead of a zero based index!!! 
							// WHY MS WHY? https://msdn.microsoft.com/en-us/library/windows/desktop/bb775958(v=vs.85).aspx

							// convert to string
							std::string selected_ext = FromWString(rgSpec[selectedExtensionIdx].pszSpec);
							if (selected_ext.find(".") != std::string::npos)
								selected_ext = selected_ext.substr(selected_ext.find_last_of(".") + 1);

							// check the extension that is already in the filename
							FilePath fp(filepath.c_str());
							std::string current_ext = fp.mExtension.substr(fp.mExtension.find_last_of(".") + 1);

							// override if necessary
							if (current_ext != selected_ext)
								filepath = fp.mDirectory + fp.mFilename + std::string(".") + selected_ext;

						}

						// Clear everything for adding
						mFiles.clear();
						mFiles.push_back(filepath);
						ResetNextFileCursor();
						CoTaskMemFree(pszFilePath);
					}
					psiResult->Release();
				}
				if (rgSpec) delete rgSpec;
				pfd->Release();
				// success
				return true;
			}
		}
		if (rgSpec) delete rgSpec;
		pfd->Release();
		return false;
	}

	// ----------------------------------------------------------------------------
	/// \fn		OpenDir
	/// \brief	Shows an open file dialog with the specified title to open a
	///			directory.
	/// \param	title - The title of the dialog
	bool OpenSaveFileDlg::OpenDir(const char * title)
	{
		// Used to extract the extensions (if any)
		std::list<std::wstring> extension_list;
		COMDLG_FILTERSPEC * rgSpec = NULL;

		// CoCreate the File Open Dialog object.
		IFileDialog *pfd = NULL;
		HRESULT hr = CoCreateInstance(CLSID_FileOpenDialog,
			NULL,
			CLSCTX_INPROC_SERVER,
			IID_PPV_ARGS(&pfd));
		if (SUCCEEDED(hr))
		{
			// set the title
			pfd->SetTitle(ToWString(title).c_str());

			// Set the options on the dialog.
			// Before setting, always get the options first in order 
			// not to override existing options.
			DWORD dwFlags;
			if (SUCCEEDED(pfd->GetOptions(&dwFlags)))
				// In this case, get shell items only for file system items.
				hr = pfd->SetOptions(dwFlags | FOS_ALLOWMULTISELECT | FOS_PICKFOLDERS);

			// Show the dialog
			hr = pfd->Show(NULL);

			if (SUCCEEDED(hr))
			{
				// Obtain the result once the user clicks 
				// the 'Open' button.
				// The result is an IShellItem object.
				IShellItem *psiResult;
				hr = pfd->GetResult(&psiResult);
				if (SUCCEEDED(hr))
				{
					// We are just going to print out the 
					// name of the file for sample sake.
					PWSTR pszFilePath = NULL;
					hr = psiResult->GetDisplayName(SIGDN_FILESYSPATH,
						&pszFilePath);

					if (SUCCEEDED(hr))
					{
						std::string filepath = FromWString(pszFilePath);

						// Clear everything for adding
						mFiles.clear();
						mFiles.push_back(filepath);
						ResetNextFileCursor();
						CoTaskMemFree(pszFilePath);
					}
					psiResult->Release();
				}
				if (rgSpec) delete rgSpec;
				pfd->Release();
				// success
				return true;
			}
		}
		if (rgSpec) delete rgSpec;
		pfd->Release();
		return false;
	}

	// ----------------------------------------------------------------------------
	/// \fn		Save
	/// \brief	Shows a save file dialog with the specified title
	/// \param	title - The title of the dialog
	/// \param	valid_extensions - A null-terminated string that contains
	///			the available extensions to save the file to. e.g.: "png;jpg;tga" 
	///			If null, no filtering will be applied
	bool OpenSaveFileDlg::Save(const char * title, const char * valid_extensions)
	{
		// Used to extract the extensions (if any)
		std::string extensions = valid_extensions;
		std::list<std::wstring> extension_list;
		COMDLG_FILTERSPEC * rgSpec = NULL;

		// CoCreate the File Open Dialog object.
		IFileSaveDialog *pfd = NULL;
		HRESULT hr = CoCreateInstance(CLSID_FileSaveDialog,
			NULL,
			CLSCTX_INPROC_SERVER,
			IID_PPV_ARGS(&pfd));

		if (SUCCEEDED(hr))
		{
			// set the title
			pfd->SetTitle(ToWString(title).c_str());

			// Set the options on the dialog.
			// Before setting, always get the options first in order 
			// not to override existing options.
			DWORD dwFlags;
			if (SUCCEEDED(pfd->GetOptions(&dwFlags)))
				// In this case, get shell items only for file system items.
				hr = pfd->SetOptions(dwFlags | FOS_FORCEFILESYSTEM /*| FOS_STRICTFILETYPES*/);

			if (valid_extensions)
			{
				// extract the extensions
				ExtensionsToWStringList(valid_extensions, extension_list);

				// Set the file types
				SetDialogFileTypes(pfd, extension_list, rgSpec, 0);

				// Set default extension
				hr = pfd->SetDefaultExtension(extension_list.front().c_str()); // set default extension as the first one passed
			}


			// Show the dialog
			hr = pfd->Show(NULL);
			if (SUCCEEDED(hr))
			{
				// Obtain the result once the user clicks 
				// the 'Open' button.
				// The result is an IShellItem object.
				IShellItem *psiResult;
				hr = pfd->GetResult(&psiResult);

				if (SUCCEEDED(hr))
				{
					PWSTR pszFilePath = NULL;
					hr = psiResult->GetDisplayName(SIGDN_FILESYSPATH,
						&pszFilePath);

					if (SUCCEEDED(hr))
					{
						std::string filepath = FromWString(pszFilePath);

						// get the current extension
						if (valid_extensions)
						{
							// get the current extension index
							UINT selectedExtensionIdx;
							pfd->GetFileTypeIndex(&selectedExtensionIdx);
							selectedExtensionIdx -= 1;	// This is a one based-index instead of a zero based index!!! 
							// WHY MS WHY? https://msdn.microsoft.com/en-us/library/windows/desktop/bb775958(v=vs.85).aspx
							// convert to string
							std::string selected_ext = FromWString(rgSpec[selectedExtensionIdx].pszSpec);
							if (selected_ext.find(".") != std::string::npos)
								selected_ext = selected_ext.substr(selected_ext.find_last_of(".") + 1);

							// check the extension that is already in the filename
							FilePath fp(filepath.c_str());
							std::string current_ext = fp.mExtension.substr(fp.mExtension.find_last_of(".") + 1);

							// override if necessary
							if (current_ext != selected_ext)
								filepath = fp.mDirectory + fp.mFilename + std::string(".") + selected_ext;

						}

						// Clear everything for adding
						mFiles.clear();
						mFiles.push_back(filepath);
						ResetNextFileCursor();
						CoTaskMemFree(pszFilePath);
					}
					psiResult->Release();
				}
				if (rgSpec) delete rgSpec;
				pfd->Release();
				// success
				return true;
			}
		}
		if (rgSpec) delete rgSpec;
		pfd->Release();
		return false;
	}

	// ----------------------------------------------------------------------------
	/// \fn		GetFiles
	/// \brief	Returns all the files selected by the user since the last call
	///			to open/save.
	/// \ret	A const vector of std::string containing the full paths of all
	///			the files selected by the user.
	const OpenSaveFileDlg::StringArray & OpenSaveFileDlg::GetFiles()
	{
		return mFiles;
	}

	// ----------------------------------------------------------------------------
	/// \fn		GetNextFilePath
	/// \brief	Retrieves the next file full path.
	/// \param	filepath - Out parameter
	/// \param	advance_to_next - if true, will move to the next file (or end).
	///			if false, then it doesn't advance the cursor
	bool OpenSaveFileDlg::GetNextFilePath(std::string & filepath, bool advance_to_next)
	{
		if (mCursor == mFiles.end())
			return false;

		// copy the filepath in the output parameter
		filepath = *mCursor;

		// advance to next file - if any
		if (advance_to_next)
			mCursor++;

		// success
		return true;
	}

	// ----------------------------------------------------------------------------
	/// \fn		GetNextFileName
	/// \brief	Retrieves the next file name (no directory included). 
	/// \param	filename - Out parameter
	/// \param	advance_to_next - if true, will move to the next file (or end).
	///			if false, then it doesn't advance the cursor
	bool OpenSaveFileDlg::GetNextFileName(std::string &filename, bool advance_to_next)
	{
		// try to get the filepath
		std::string filepath;
		if (GetNextFilePath(filepath, advance_to_next))
		{
			// process the filepath
			FilePath fp(filepath.c_str());

			// copy the filename only in the output parameter
			filename = fp.mFilename;

			// success
			return true;
		}
		// failure
		return false;
	}

	// ----------------------------------------------------------------------------
	/// \fn		GetNextFileDirectory
	/// \brief	Retrieves the directory of the next file. 
	/// \param	directory - Out parameter
	/// \param	advance_to_next - if true, will move to the next file (or end).
	///			if false, then it doesn't advance the cursor
	bool OpenSaveFileDlg::GetNextFileDirectory(std::string &directory, bool advance_to_next)
	{
		// try to get the filepath
		std::string filepath;
		if (GetNextFilePath(filepath, advance_to_next))
		{
			// process the filepath
			FilePath fp(filepath.c_str());

			// copy the filename only in the output parameter
			directory = fp.mDirectory;

			// success
			return true;
		}
		// failure
		return false;
	}

	// ----------------------------------------------------------------------------
	/// \fn		ResetNextFileCursor
	/// \brief	Resets the cursor so that the next call to GetNextFile... functions
	///			starts from the begininng again.
	void OpenSaveFileDlg::ResetNextFileCursor()
	{
		if (mFiles.size())
			mCursor = mFiles.begin();
		else
			mCursor = mFiles.end();
	}
}
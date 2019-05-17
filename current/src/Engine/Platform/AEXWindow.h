// ----------------------------------------------------------------------------
// File: WindowClass.h
// Project: Thomas Komair
// Purpose: Defines a class that wraps around a Win32 window.
// Author: Alpha Engine
// Copyright DigiPen Institute of Technology, 2013. All rights reserved
// ----------------------------------------------------------------------------

#ifndef AEX_WINDOW_H_
#define AEX_WINDOW_H_

#include "..\Utilities\AEXContainers.h"
#include "..\Core\AEXCore.h"
#include <Windows.h>
#pragma warning (disable:4251) // dll and STL

namespace AEX
{
	// ----------------------------------------------------------------------------
	// ----------------------------------------------------------------------------
	// CLASS: AEWINDOW
	class Window : public IBase
	{
		friend class WindowManager;

		// RTTI
		AEX_RTTI_DECL(Window, IBase);

		// ------------------------------------------------------------------------
		// PUBLIC INTERFACE
	public:
		Window(const char * title, u32 width, u32 height, HINSTANCE instance, int show, HWND parentWin = NULL);
		virtual ~Window();


		// Getters
		void		GetWindowTitle(AEX_STRING & outTitle);
		void		GetPos(u32* px, u32 *py);
		u32			GetWidth();
		u32			GetHeight();
		u32			GetMetricWidth();
		u32			GetMetricHeight();
		bool		GetFullScreen();
		HWND		GetHandle();
		HINSTANCE	GetInstance();

		// Setters
		void		SetTitle(const char * title);
		void		SetPos(u32 x, u32 y);
		void		SetClientWidth(u32 width, bool updateWin32 = true);
		void		SetClientHeight(u32 height, bool updateWin32 = true);
		void		SetClientSize(u32 w, u32 h, bool updateWin32 = true);
		void		SetFullScreen(bool fullscreen);

		// Show/Hide
		void		Show(bool show);
		bool		Active();	 // show hi

		// Destroy
		bool		Exists();
		
	private:

		void CreateWin32Window();
		void DestroyWin32Window();

		// win32
		HWND hWindowHandle;			// win32 handle to the window
		HWND hParentWinHandle;		// win32 handle to the parent window
		HINSTANCE  hInstance;		// app instance

		// window dimensions
		u32 metricWidth;
		u32 metricHeight;
		u32 clientWidth;
		u32 clientHeight;
		u32 lastClientWidth;
		u32 lastClientHeight;
		u32 lastPosX, lastPosY;
		bool bfullScreen;

		// title
		std::string winTitle;

		// state info
		bool	exists;
		bool	isActive;
		bool	bDragDropEnabled;
	};


	// ----------------------------------------------------------------------------
	// ----------------------------------------------------------------------------
	// CLASS: Window Manager
	class WindowManager : public ISystem
	{
		AEX_RTTI_DECL(WindowManager, ISystem);
		AEX_SINGLETON(WindowManager);

	public:

		// ISystem
		virtual bool Initialize();
		virtual void Update();
		virtual ~WindowManager();

		// Window Access
		Window* NewWindow(const char * name, u32 w, u32 h, u32 show = SW_SHOW);
		Window* GetMainWindow();
		Window* GetWindow(HWND hWin);	// Get one of the registered windows
		WNDCLASSEX GetWindowClass();
	private:
		// Win32 Specific
		void DoWin32Events();				// Update the events
		void RegisterWin32Class();			// Register Class
		void UnregisterWin32Class();			// Unregister Class

		// Default window procedure
		static LRESULT CALLBACK AEDefWinProc(HWND hWin, UINT msg, WPARAM wp, LPARAM lp);
		
	private:
		std::map<HWND, Window*> mWindows;
		bool			mbIsAEWinClassRegistered;
		WNDCLASSEX   mWinClass;		// window class associated to this window
		Window*	mMainWindow;	// the main window for the engine
		HINSTANCE  mhInstance;		// app instance
	};

}
#pragma warning (default:4251) // dll and STL
	// Easy access to singleton
#define aexWindowMgr (WindowManager::Instance())
// ----------------------------------------------------------------------------
#endif
// ----------------------------------------------------------------------------
// File: WindowClass.cpp
// Project: Thomas Komair
// Purpose: Defines a class that wraps around a Win32 window.
// Author: Alpha Engine
// Copyright DigiPen Institute of Technology, 2013. All rights reserved
// ----------------------------------------------------------------------------
#include "..\Debug\MyDebug.h"
#include "AEXWindow.h"
#include "AEXInput.h"
#include <strsafe.h>		// WIN32 DEBUG
namespace AEX
{

	// ----------------------------------------------------------------------------
	// ----------------------------------------------------------------------------
#pragma region// WINDOW MANAGER

	WindowManager::WindowManager()
		: mMainWindow(NULL)
		, mbIsAEWinClassRegistered(false)
		, mhInstance(::GetModuleHandle(NULL))
	{}
	// ----------------------------------------------------------------------------
	bool WindowManager::Initialize()
	{
		// Create a window class
		RegisterWin32Class();

		// Create main window
		mMainWindow = NewWindow("AEX", 1920/2, 1080/2);

		return true;
	}
	// ----------------------------------------------------------------------------
	WindowManager::~WindowManager()
	{
		// delete every window we have
		FOR_EACH(it, mWindows)
			delete it->second;

		mWindows.clear();
		UnregisterWin32Class();
	}
	// ----------------------------------------------------------------------------
	void WindowManager::Update()
	{
		DoWin32Events();
	}
	// ----------------------------------------------------------------------------
	Window* WindowManager::NewWindow(const char * name, u32 w, u32 h, u32 show)
	{
		// create new window and replace old one (if any)
		Window * pWin = new Window(name, w, h, mhInstance, show);
		if (mWindows.find(pWin->GetHandle()) != mWindows.end()) // find by win32 handle
			delete mWindows[pWin->GetHandle()];

		// save pointer
		mWindows[pWin->GetHandle()] = pWin;

		// return
		return pWin;
	}
	Window* WindowManager::GetMainWindow()
	{
		if (mMainWindow)
			return mMainWindow;
		return NULL;
	}
	// ----------------------------------------------------------------------------
	// Get one of the registered windows
	Window* WindowManager::GetWindow(HWND hWin)
	{
		std::map<HWND, Window*>::iterator it = mWindows.find(hWin);
		if (it != mWindows.end())
		{
			return it->second;
		}
		return NULL; // NOT FOUND
	}
	// ----------------------------------------------------------------------------
	// Get the window class registered with the OS
	WNDCLASSEX WindowManager::GetWindowClass()
	{
		return mWinClass;
	}
	// ----------------------------------------------------------------------------
	void WindowManager::DoWin32Events()
	{
		MSG msg;

		// handle window messages
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	// ----------------------------------------------------------------------------
	// Win Class registration
	void WindowManager::RegisterWin32Class()
	{
		if (!mbIsAEWinClassRegistered)
		{
			// Our window class
			mWinClass.cbSize = sizeof(WNDCLASSEX);
			mWinClass.lpszClassName = "Window";
			mWinClass.style = CS_HREDRAW | CS_VREDRAW;
			mWinClass.lpfnWndProc = WindowManager::AEDefWinProc;
			mWinClass.hInstance = GetModuleHandle(NULL);
			mWinClass.cbClsExtra = 0;
			mWinClass.cbWndExtra = 0;

			// cursor, icon, menu
			mWinClass.hCursor = LoadCursor(NULL, IDC_ARROW);
			mWinClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
			mWinClass.lpszMenuName = NULL;
			mWinClass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
			mWinClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);

			if (0 == RegisterClassEx(&mWinClass))
			{
				Win32ShowLastError("constructor");
			}

			mbIsAEWinClassRegistered = true;
		}
	}
	void WindowManager::UnregisterWin32Class()
	{
		UnregisterClass(mWinClass.lpszClassName, GetModuleHandle(NULL));
	}

	// ----------------------------------------------------------------------------
	// ----------------------------------------------------------------------------
	// WIN 32
	LRESULT CALLBACK WindowManager::AEDefWinProc(HWND hWin, UINT msg, WPARAM wp, LPARAM lp)
	{
		HDC dc;
		PAINTSTRUCT ps;
		bool dodefault = false;

		switch (msg)
		{
		case WM_DESTROY:
			// get the window and set its exists parameter to false
			if (Window * win = WindowManager::Instance()->GetWindow(hWin))
				win->exists = false;
			break;
		case WM_CREATE:
			return 0;
		case WM_PAINT:
			// when the window is drawn
			dc = BeginPaint(hWin, &ps);
			// Cleans up the painting process
			EndPaint(hWin, &ps);
			break;
			// called any time the window is moved
		case WM_MOVE:
			// Invalidate the rect to force a redraw
			InvalidateRect(hWin, NULL, FALSE);
			break;
			// called any time the window is resized
		case WM_SIZE:
		{
			u32 w = LOWORD(lp);
			u32 h = HIWORD(lp);
			if (auto pWin = WindowManager::Instance()->GetWindow(hWin))
			{
				pWin->SetClientSize(w, h, false);
			}

		}break;

		default:
			dodefault = true;
			break;
		}

		// Handle input messages
		if (Input::Instance()->HandleWin32Message(msg, wp, lp))
			return 0;

		// no one has handled the message, do default
		if (dodefault)
			return DefWindowProc(hWin, msg, wp, lp);

		// it was handled
		return 0;
	}

#pragma endregion

	// ----------------------------------------------------------------------------
	// ----------------------------------------------------------------------------
#pragma region// WINDOW OBJECT

	// Constructor
	Window::Window(const char * title, u32 width, u32 height, HINSTANCE instance, int show, HWND parentWin)
		: hWindowHandle(NULL)
		, hParentWinHandle(parentWin)
		, bfullScreen(false)
	{
		// store relevant data
		exists = true;
		winTitle = title;
		clientWidth = width;
		clientHeight = height;
		hInstance = instance;

		// Create Window
		CreateWin32Window();

		// Show window
		::ShowWindow(hWindowHandle, show);
		::UpdateWindow(hWindowHandle);

	}
	Window::~Window()
	{
		DestroyWin32Window();
	}
	void Window::CreateWin32Window()
	{

		RECT winrect = { 0, 0, (LONG)clientWidth, (LONG)clientHeight };

		if (hParentWinHandle)
		{
			// save the metrics
			AdjustWindowRect(&winrect, WS_CHILD, false);
			metricWidth = winrect.right - winrect.left;
			metricHeight = winrect.bottom - winrect.top;

			hWindowHandle = CreateWindow(
				WindowManager::Instance()->GetWindowClass().lpszClassName,			// what class of window?
				winTitle.c_str(),				// title
				WS_CHILD,			// dflt style
				0,								// pos
				0,								// pos
				metricWidth,
				metricHeight,
				hParentWinHandle,
				NULL,
				this->hInstance,
				NULL);
		}
		else
		{
			// save the metrics
			AdjustWindowRect(&winrect, WS_OVERLAPPEDWINDOW, false);
			metricWidth = winrect.right - winrect.left;
			metricHeight = winrect.bottom - winrect.top;

			WindowManager * mgr = WindowManager::Instance();
			hWindowHandle = CreateWindow(
				mgr->GetWindowClass().lpszClassName,			// what class of window?
				winTitle.c_str(),				// title
				WS_OVERLAPPEDWINDOW,			// dflt style
				0,								// pos
				0,								// pos
				metricWidth,
				metricHeight,
				NULL,
				NULL,
				this->hInstance,
				NULL);
		}
		if (NULL == hWindowHandle)
		{
			Win32ShowLastError("CreateWindow: ");
		}
	}

	// Destroy
	void Window::DestroyWin32Window()
	{
		// erase the window
		if (hWindowHandle)
		{
			// WIN32 API
			::DestroyWindow(hWindowHandle);
			hWindowHandle = NULL;
		}
	}
	bool		Window::Exists()
	{
		return exists;
	}

	// Show/Hide
	void		Window::Show(bool show)
	{
		isActive = show;
		if (isActive)
			::ShowWindow(hWindowHandle, SW_SHOW);
		else
			::ShowWindow(hWindowHandle, SW_HIDE);
	}
	bool		Window::Active()
	{
		return isActive;
	}

	// Getters
	void		Window::GetWindowTitle(AEX_STRING & outTitle)
	{
		static char buffer[512];
		int actualSize = ::GetWindowText(hWindowHandle, buffer, 512);
		outTitle = buffer;
	}
	u32			Window::GetWidth()
	{
		return clientWidth;
	}
	u32			Window::GetHeight()
	{
		return clientHeight;
	}
	u32			Window::GetMetricWidth()
	{
		return metricWidth;
	}
	u32			Window::GetMetricHeight()
	{
		return metricHeight;
	}
	HWND		Window::GetHandle()
	{
		return hWindowHandle;
	}
	HINSTANCE	Window::GetInstance()
	{
		return hInstance;
	}
	void		Window::GetPos(u32* px, u32 *py)
	{
		::RECT winRect;
		::GetWindowRect(GetHandle(), &winRect);
		*px = (u32)winRect.left;
		*py = (u32)winRect.top;
	}

	void		Window::SetPos(u32 x, u32 y)
	{
		::SetWindowPos(GetHandle(), 0, x, y, 0, 0, SWP_NOSIZE);
	}
	void		Window::SetTitle(const char * title)
	{
		if (title)
			::SetWindowText(hWindowHandle, title);
	}
	void		Window::SetClientWidth(u32 width, bool updateWin32)
	{
		SetClientSize(width, clientHeight, updateWin32);
	}
	void		Window::SetClientHeight(u32 height, bool updateWin32)
	{
		SetClientSize(clientWidth, height, updateWin32);
	}
	void		Window::SetClientSize(u32 w, u32 h, bool updateWin32)
	{
		clientWidth = w;
		clientHeight = h;
		RECT winrect = { 0, 0, (LONG)clientWidth, (LONG)clientHeight };
		AdjustWindowRect(&winrect, WS_OVERLAPPEDWINDOW, false);
		metricWidth = winrect.right - winrect.left;
		metricHeight = winrect.bottom - winrect.top;

		if (updateWin32)
			//::SetWindowPos(this->GetHandle(), 0, 0, 0, clientWidth, clientHeight, SWP_NOMOVE);
		::SetWindowPos(this->GetHandle(), 0, 0, 0, metricWidth, metricHeight, SWP_NOMOVE);
	}

	void Window::SetFullScreen(bool fullscreen)
	{

		if (bfullScreen == fullscreen)
			return;

		// toggle
		bfullScreen = fullscreen;

		// get win handle
		auto hwnd = GetHandle();

		if (bfullScreen) {
			int scSize[2] = { ::GetSystemMetrics(SM_CXFULLSCREEN), ::GetSystemMetrics(SM_CYFULLSCREEN) };
			lastClientWidth	 = clientWidth;
			lastClientHeight = clientHeight;
			GetPos(&lastPosX, &lastPosY);
			SetClientSize(scSize[0], scSize[1]);
			SetPos(0, 0);
			::SetWindowLong(hwnd, GWL_STYLE, WS_POPUP);

		}
		// windowed
		else {
			SetClientSize(lastClientWidth, lastClientHeight);
			SetPos(lastPosX, lastPosY);
			::SetWindowLong(hwnd, GWL_STYLE, WS_OVERLAPPEDWINDOW);
		}
		const int swpFlags = SWP_FRAMECHANGED | SWP_SHOWWINDOW;
		::SetWindowPos(hwnd, 0, 0, 0, 0, 0, swpFlags | SWP_NOMOVE | SWP_NOSIZE);

	}

	bool		Window::GetFullScreen()
	{
		return bfullScreen;
	}

	#pragma endregion
}
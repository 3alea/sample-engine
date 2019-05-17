// ---------------------------------------------------------------------------
// Project Name		:	Alpha Engine
// File Name		:	AEInput.h
// Author			:	Sun Tjen Fam
// Creation Date	:	2008/01/31
// Purpose			:	declaration for input stuff
// History			:
// - 2008/01/31		:	- initial implementation
// ---------------------------------------------------------------------------

#ifndef AEX_INPUT_H
#define AEX_INPUT_H
// ---------------------------------------------------------------------------
// Defines/Enums


#define AEX_INPUT_KEY_NUM	256
#define AEX_INPUT_MOUSE_NUM 3

#include "..\Utilities\AEXContainers.h"
#include "..\Core\AEXCore.h"
#include <Windows.h>
#include <aexmath\AEXVec2.h>

#pragma warning (disable:4251) // dll and STL
namespace AEX
{

	// ---------------------------------------------------------------------------
	// class definitions
	class  Input : public ISystem
	{
		AEX_RTTI_DECL(Input, ISystem);
		AEX_SINGLETON(Input);

	public:
		enum Keys
		{
			eAny = -1,
			eMouseLeft = 0,
			eMouseRight = 1,
			eMouseMiddle = 2,
			eControl = 17,
			eShift = 16,
			eTab = 9,
			eAlt = 10
		};

	public:

		// ISystem
		virtual bool Initialize(); 
		virtual void Update();
		virtual ~Input();

		// Input
		bool HandleWin32Message(UINT msg, WPARAM wParam, LPARAM lParam);
		bool AnyKey();
		bool AnyKeyTriggered();
		bool KeyPressed(int key);
		bool KeyTriggered(int key);
		bool MousePressed(int button);
		bool MouseTriggered(int button);

		AEVec2 GetMousePos();
		AEVec2 GetPrevMousePos();
		AEVec2 GetMouseMovement();
		f32		GetMouseWheel();
		bool MouseInWindow();

		const std::vector<u8> & AllKeyPressed();
		const std::vector<u8> & AllKeyTriggered();

	private:

		u8	mKeyCurr[AEX_INPUT_KEY_NUM];
		u8	mKeyPrev[AEX_INPUT_KEY_NUM];
		u8	mMouseCurr[AEX_INPUT_MOUSE_NUM];
		u8	mMousePrev[AEX_INPUT_MOUSE_NUM];
		f32 mMouseWheel;
		AEVec2 mMousePos;			// Mouse Position in Centered Coordinates
		AEVec2 mMousePrevPos;		// Previous Mouse Position in Centered Coordinates
		AEVec2 mMouseDiff;		// Mouse movement this update: Prev - Curr
		bool		 mMouseInWindow;	// Specifies whether the mouse is contained insisde the window


		bool				mAnyKey;	 // Any key is pressed. 
		bool				mAnyKeyTriggered;	 // Any key is pressed. 
		std::vector<u8>	mKeyPressed; // keys pressed last frame.
		std::vector<u8>	mKeyTriggered; // keys pressed last frame.
	};
}
#pragma warning (default:4251) // dll and STL

// Easy access to singleton
#define aexInput (Input::Instance())
// ---------------------------------------------------------------------------

#endif // AE_INPUT_H


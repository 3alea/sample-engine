// ----------------------------------------------------------------------------
// Project: GAM300 - Sample Engine
// File:	IBase.h
// Purpose:	Header for IBase class
// Author:	Thomas Komair
//
// Copyright DigiPen Institute of Technology
// ----------------------------------------------------------------------------
#ifndef AEX_BASE_H_
#define AEX_BASE_H_

#include <string>				// string
#include "AEXDataTypes.h"
#include "AEXRtti.h"			// RTTI

namespace AEX
{
	// ----------------------------------------------------------------------------
	// CLASS:	IBase
	// PURPOSE:	This class provides the base for all the classes in the engine 
	//			It provides basic services such as RTTI and Messages, smart pointers, etc...
	class IBase
	{
		AEX_RTTI_DECL_BASE(IBase);
	public:
		// public virtual destructor 
		virtual ~IBase() {}

		// ----------------------------------------------------------------------------
		#pragma region// NAME & UID
	public:
		const char * GetName()			{ return mName.c_str(); }
		void SetName(const char * name)	{ mName = name; }
		u32			 GetUID()			{ return mUID; }
	protected:
		std::string mName;	// Non-Unique name
		u32			mUID;	// Unique number ID
		#pragma endregion
		
	protected:
		IBase() {} // only accessible from child classes. can't construct an IBase explicitly.
	};
}
// ----------------------------------------------------------------------------
#endif
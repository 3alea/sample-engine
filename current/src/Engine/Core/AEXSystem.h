///////////////////////////////////////////////////////////////////////////////////////
///
///	\file System.h Defines the system interface.
///
///	Authors:  Chris Peters
///	Copyright 2010, Digipen Institute of Technology
///
///////////////////////////////////////////////////////////////////////////////////////
#pragma once //Makes sure this header is only included once

#include "..\Utilities\AEXContainers.h"
#include "AEXBase.h"

namespace AEX
{
	///System is a pure virtual base class (which is to say, an interface) that is
	///the base class for all systems used by the game. 
	class ISystem : public IBase
	{
		AEX_RTTI_DECL(ISystem, IBase);

	public:

		///All systems are updated every game frame.
		virtual void Update(){};

		///Initialize the system.
		virtual bool Initialize(){ return true; };
		
		///All systems need a virtual destructor to have their destructor called 
		virtual ~ISystem(){}	

		/// Singleton support
		virtual AEX::ISystem** RetrieveInstancePtr() { return nullptr; }
		virtual AEX::ISystem* RetrieveInstance()	 { return nullptr; }
	};
}

// Add this macro to make the class singleton. Although there are better ways
// to implement the singleton pattern, this one is quick and easy. Warning!
// the default, copy constructor and assignment operators are made private
#define AEX_SINGLETON(classname)\
	public:\
	static classname** InstancePtr()\
	{\
		static classname * singleton = nullptr;\
		if(singleton == nullptr)\
			singleton = new classname();\
		return &singleton;\
	}\
	static classname* Instance()\
	{\
		classname * singleton = *InstancePtr();\
		return singleton;\
	}\
	virtual AEX::ISystem** RetrieveInstancePtr()\
	{\
		return reinterpret_cast<AEX::ISystem**>(classname::InstancePtr());\
	}\
	virtual AEX::ISystem* RetrieveInstance()\
	{\
		return dynamic_cast<AEX::ISystem*>(classname::Instance());\
	}\
	static void ReleaseInstance()\
	{\
		delete Instance();\
		(*InstancePtr()) = NULL;\
	}\
	private:\
	classname();\
	classname(const classname &);\
	const classname & operator=(const classname &);

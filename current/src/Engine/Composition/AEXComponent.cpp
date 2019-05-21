// ----------------------------------------------------------------------------
// Project: GAM300 - Sample Engine
// File:	IComp.h
// Purpose:	Header file for the IComp API
// Author:	Thomas Komair
//
// Copyright DigiPen Institute of Technology
// ----------------------------------------------------------------------------
#include "AEXComponent.h"
#include "AEXGameObject.h"
namespace AEX
{
	// ----------------------------------------------------------------------------
	// Constructors
	IComp::IComp()
		: mOwner(NULL)
		, mbEnabled(true)
	{}
	IComp::~IComp()
	{}
	GameObject* IComp::GetOwner(void)
	{
		// return a shared pointer
		return mOwner;
	}

	// ----------------------------------------------------------------------------
	// PUBLIC - State Methods - By default they do nothing in this component
	//void IComp::OnCreate()
	//{}
	void IComp::Initialize()
	{}
	void IComp::Shutdown()
	{}
	void IComp::Update()
	{}

	// ----------------------------------------------------------------------------
	bool IComp::IsEnabled()
	{
		return mbEnabled;
	}
	void IComp::SetEnabled(bool enabled)
	{
		mbEnabled = enabled;
	}
}
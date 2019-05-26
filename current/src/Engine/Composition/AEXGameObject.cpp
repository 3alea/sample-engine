// ----------------------------------------------------------------------------
// Project: GAM300 - Sample Engine
// File:	IComp.h
// Purpose:	Header file for the IComp API
// Author:	Thomas Komair
//
// Copyright DigiPen Institute of Technology
// ----------------------------------------------------------------------------
#include "AEXComposition.h"

namespace AEX
{

	// ----------------------------------------------------------------------------
	// ----------------------------------------------------------------------------
	// AEXOBJECT

	GameObject::GameObject()
		: IBase()
	{}
	GameObject::~GameObject()
	{}

	// ----------------------------------------------------------------------------
	#pragma region// STATE METHODS
	
	void GameObject::SetEnabled(bool enabled) // Call Set Enabled on all components
	{
		// avoid redundant work
		if (mbEnabled == enabled)
			return;

		// call base method
		mbEnabled = enabled;

		// delegate to components
		FOR_EACH(it, mComps)
			(*it)->SetEnabled(enabled);
	}
	void GameObject::Initialize()
	{
		// Initialize all comps 
		FOR_EACH(it, mComps)
			(*it)->Initialize();
	}
	void GameObject::Shutdown()
	{
		// shutdown all comps 
		FOR_EACH(it, mComps)
			(*it)->Shutdown();
	}

	#pragma endregion

	// ----------------------------------------------------------------------------
	#pragma region// COMPONENT MANAGEMENT

	// Find Component
	u32 GameObject::GetCompCount() const
	{
		return mComps.size();
	}
	IComp* GameObject::GetComp(u32 index) const
	{
		if (index < GetCompCount())
			return mComps[index];
		return NULL;
	}
	IComp* GameObject::GetComp(const char * type) const
	{
		// go throught the components and look for the same type
		for (auto it = mComps.begin(); it != mComps.end(); ++it)
		{
			std::string typeName = (*it)->GetType().GetName();
			if (typeName == type)
				return (*it);
		}
		return NULL;
	}
	IComp* GameObject::GetComp(const Rtti & type) const
	{
		// go throught the components and look for the same type
		for (auto it = mComps.begin(); it != mComps.end(); ++it)
		{
			if ((*it)->GetType().IsExactly(type))
				return (*it);
		}
		return NULL;
	}
	IComp* GameObject::GetCompName(const char * compName, const char *compType) const
	{
		for (auto it = mComps.begin(); it != mComps.end(); ++it)
		{
			// found a match with the name
			if (strcmp((*it)->GetName(), compName) == 0)
			{
				// not same type -> continue
				if (compType && strcmp(compType, (*it)->GetType().GetName()) != 0)
					continue;
				// same type or don't care about type -> return
				return (*it);
			}
		}
		return NULL;
	}

	// Add/Remove by address
	IComp * GameObject::AddComp(IComp* pComp)
	{
		if (pComp) {
			pComp->mOwner = this;
			mComps.push_back(pComp);
		}
		return pComp;
	}
	void GameObject::RemoveComp(IComp* pComp)
	{
		if (!pComp)
			return;
		//pComp->Shutdown(); //TODO: make sure that we indeed don't need that
		// NOTE this will create memory leaks.
		for (auto it = mComps.begin(); it != mComps.end(); ++it)
		{
			if ((*it) == pComp)
			{
				pComp->mOwner = NULL;
				mComps.erase(it);
				return;
			}
		}
	}

	// Removes first component encoutered that match the search criteria
	void GameObject::RemoveCompType(const char * compType)
	{
		RemoveComp(GetComp(compType));
	}
	void GameObject::RemoveCompType(const Rtti & compType)
	{
		RemoveComp(GetComp(compType));
	}
	void GameObject::RemoveCompName(const char * compName, const char * compType)
	{
		RemoveComp(GetCompName(compName, compType));
	}

	// Removes all components encoutered that match the search criteria
	void GameObject::RemoveAllCompType(const char * compType)
	{
		IComp* pComp = GetComp(compType);
		while (pComp)
		{
			RemoveComp(pComp);
			pComp = GetComp(compType);
		}
	}
	void GameObject::RemoveAllCompType(const Rtti & compType)
	{
		IComp* pComp = GetComp(compType);
		while (pComp)
		{
			RemoveComp(pComp);
			pComp = GetComp(compType);
		}
	}
	void GameObject::RemoveAllCompName(const char * compName, const char * compType)
	{
		IComp* pComp = GetCompName(compName, compType);
		while (pComp)
		{
			RemoveComp(pComp);
			pComp = GetCompName(compName, compType);
		}
	}

	void GameObject::RemoveAllComp()
	{
		while (mComps.size())
		{
			mComps.back()->Shutdown();
			delete mComps.back();
			mComps.pop_back();
		}
	}

	#pragma endregion

}
// ----------------------------------------------------------------------------
// Project: GAM300 - Sample Engine
// File:	IComp.h
// Purpose:	Header file for the IComp API
// Author:	Thomas Komair
//
// Copyright DigiPen Institute of Technology
// ----------------------------------------------------------------------------
#include "AEXComposition.h"
#include "..\Core\AEXFactory.h"
#include "..\Utilities\AEXSerializer.h"

namespace AEX
{

	// ----------------------------------------------------------------------------
	// ----------------------------------------------------------------------------
	// AEXOBJECT

	// RTTI
	AEX_RTTI_IMPL(GameObject, IComp);
	
	GameObject::GameObject()
		: IComp()
	{}
	GameObject::~GameObject()
	{}

	// ----------------------------------------------------------------------------
	#pragma region// STATE METHODS
	
	void GameObject::SetEnabled(bool enabled) // Call Set Enabled on all components
	{
		// call base method
		IComp::SetEnabled(enabled);

		// delegate to components
		FOR_EACH(it, mComps)
			(*it)->SetEnabled(enabled);
	}
	//void GameObject::OnCreate()
	//{
	//	FOR_EACH(it, mComps)
	//		(*it)->OnCreate();
	//}
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
	int GameObject::GetCompCount()
	{
		return (int)mComps.size();
	}
	IComp* GameObject::GetComp(int index)
	{
		if (index >= 0 && index < GetCompCount())
			return mComps[index];
		return NULL;
	}
	IComp* GameObject::GetComp(const char * type)
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
	IComp* GameObject::GetComp(const AEXRtti & type)
	{
		// go throught the components and look for the same type
		for (auto it = mComps.begin(); it != mComps.end(); ++it)
		{
			if ((*it)->GetType().IsExactly(type))
				return (*it);
		}
		return NULL;
	}
	IComp* GameObject::GetCompName(const char * compName, const char *compType)
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
	void GameObject::AddComp(IComp* pComp)
	{
		if (!pComp)
			return;
		pComp->mOwner = this;
		mComps.push_back(pComp);
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
	void GameObject::RemoveCompType(const AEXRtti & compType)
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
	void GameObject::RemoveAllCompType(const AEXRtti & compType)
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

	#pragma endregion

	// ----------------------------------------------------------------------------
	#pragma region// SERIALIZATION

	void GameObject::StreamRead(ISerializer * serializer)
	{
		// Read the object data
		serializer->StreamRead("Name", mName);

		if (ISerializer * compRootNode = serializer->FirstChildNode("Components"))
		{
			// Read the components
			ISerializer * compNode = compRootNode->FirstChildNode();

			while (compNode) // we have components
			{
				// Check if the component exits already
				IComp * comp = GetComp(compNode->GetName());

				// If not create the component using the factory
				if (nullptr == comp)
				{
					// create with factory
					comp = dynamic_cast<IComp*>(Factory::Instance()->Create(compNode->GetName()));

					// add to the object
					AddComp(reinterpret_cast<IComp*>(comp));
				}

				// Can't create the component-> either component is unknown
				// or something horribly wrong has happened.
				if (nullptr == comp)
					continue;

				// serialize the component
				comp->StreamRead(compNode);

				// tell the serializer we're finished with this section
				compNode = compNode->NextSiblingNode();
			}
		}
	}

	void GameObject::StreamWrite(ISerializer * serializer)
	{
		// Write the object name
		serializer->StreamWrite("Name", mName);

		// create a container node for the components 
		// so that it's easy to find later
		ISerializer * compRootNode = serializer->BeginNode("Components");

		// if the object has components -> write them
		if (compRootNode && mComps.size())
		{
			FOR_EACH(it, mComps)
			{
				// get component
				IComp * pComp = *it;
				
				// create a new section
				ISerializer * compNode = compRootNode->BeginNode(pComp->GetType().GetName());

				// serialize the component
				if (compNode)
				{
					// serializer
					pComp->StreamWrite(compNode);

					// tell the serializer we're done with this section
					compRootNode->EndNode(compNode);
				}
			}
			serializer->EndNode(compRootNode);
		}
	}

	#pragma endregion
}
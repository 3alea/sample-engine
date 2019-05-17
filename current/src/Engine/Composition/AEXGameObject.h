// ----------------------------------------------------------------------------
// Project: GAM300 - Sample Engine
// File:	IComp.h
// Purpose:	Header file for the IComp API
// Author:	Thomas Komair
//
// Copyright DigiPen Institute of Technology
// ----------------------------------------------------------------------------
#ifndef AEX_OBJECT_H_
#define AEX_OBJECT_H_
// ----------------------------------------------------------------------------
#include "AEXComponent.h"

#pragma warning (disable:4251) // dll and STL
namespace AEX
{
	class AEX_API GameObject : public IComp
	{
		// Base stuff
		AEX_RTTI_DECL;		// RTTI

	public:

		GameObject();
		virtual ~GameObject();

		// State Methods
		virtual void SetEnabled(bool enabled); // Call Set Enabled on all components
		virtual void Initialize();	// Calls initialize on all components
		virtual void Shutdown();
		// --------------------------------------------------------------------
		#pragma region// COMPONENT MANAGEMENT

		// Getters
		int GetCompCount();
		IComp* GetComp(int index);
		IComp* GetComp(const char * type);
		IComp* GetComp(const AEXRtti & type);
		IComp* GetCompName(const char * compName, const char *compType = NULL);

		// template
		template<class T>
		T* GetComp(const char * name = NULL);

		template<class T>
		T* GetCompDerived(const char * name = NULL);

		// Add/Remove by address
		void AddComp(IComp * pComp);
		void RemoveComp(IComp * pComp);

		// Removes first component encoutered that match the search criteria
		void RemoveCompType(const char * compType);
		void RemoveCompType(const AEXRtti & compType);
		void RemoveCompName(const char * compName, const char * compType = NULL);

		// Removes all components encoutered that match the search criteria
		void RemoveAllCompType(const char * compType);
		void RemoveAllCompType(const AEXRtti & compType);
		void RemoveAllCompName(const char * compName, const char * compType = NULL);

		// Remove all components
		void RemoveAllComp();
		#pragma endregion

		// --------------------------------------------------------------------
		#pragma region// SERIALIZATION
		virtual void StreamRead(ISerializer * serializer);
		virtual void StreamWrite(ISerializer * serializer);
		#pragma endregion

	protected:
		AEX_PTR_ARRAY(IComp) mComps;
	};

	template<class T>
	T* GameObject::GetComp(const char * compName)
	{
		// go throught the components and look for the same type
		for (auto it = mComps.begin(); it != mComps.end(); ++it)
		{
			if ((*it)->GetType().IsExactly(T::TYPE))
			{
				// not same name -> continue
				if (compName && strcmp(compName, (*it)->GetName()) != 0)
					continue;

				// same name or don't care about the name
				// return
				return (T*)(*it);
			}
		}
		return NULL; // not found
	}
	template<class T>
	T* GameObject::GetCompDerived(const char * compName)
	{
		// go throught the components and look for the same type
		for (auto it = mComps.begin(); it != mComps.end(); ++it)
		{
			if ((*it)->GetType().IsDerived(T::TYPE))
			{
				// not same name -> continue
				if (compName && strcmp(compName, (*it)->GetName()) != 0)
					continue;

				// same name or don't care about the name
				// return
				return (T*)(*it);
			}
		}
		return NULL; // not found
	}
}
#pragma warning (default:4251) // dll and STL

// ----------------------------------------------------------------------------
#endif
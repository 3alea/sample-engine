#include "AEXRtti.h"

namespace AEX
{
	std::map<std::string, Rtti> Rtti::Types;

	const Rtti & Rtti::RttiAdd(const char * typeName, const char * parentName)
	{
		// TODO: insert return statement here

			// parent isn't assumed to exists. so this might be the first time
			// the parent type is encountered; (it depends on the call). 
		auto & parentIt = Types[parentName];
		parentIt.mName = parentName;

		std::map<std::string, Rtti>::iterator it = Types.find(typeName);
		if (it == Types.end()) {

			auto & ref = Types[typeName];
			ref.mpBaseType = &parentIt;
			ref.mName = typeName;

			// restore iterator
			it = Types.find(typeName);
		}

		// add to parent
		parentIt.mChildren[typeName] = &it->second;

		return it->second;
	}

	// ----------------------------------------------------------------------------
	// constructor
	Rtti::Rtti(const char * name, const Rtti * pBaseType)
	{
		// store the name
		mName = name;
		// remove the "class " prefix, returned by typeid().name() function
		mName = mName.substr(mName.find_first_of(" ")+1); 

		// store the pointer to the base type
		mpBaseType = pBaseType;
	}

	Rtti::Rtti()
		: mName("no_name")
		, mpBaseType(nullptr)
	{
	}

	// ----------------------------------------------------------------------------
	// getters
	const char * Rtti::GetName() const
	{
		return mName.c_str();
	}

	// ----------------------------------------------------------------------------
	// compares this with address of otherType
	bool Rtti::IsExactly(const Rtti & otherType) const
	{
		return this == &otherType;
	}

	// ----------------------------------------------------------------------------
	// walks up the hierarchy comparing with address of otherType
	bool Rtti::IsDerived(const Rtti & otherType) const
	{
		const Rtti * temp = this;
		while (temp)
		{
			if (temp == &otherType)
			{
				return true;
			}

			temp = temp->mpBaseType;
		}
		return false;
	}
}
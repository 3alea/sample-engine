#ifndef AEX_RTTI_H_
#define AEX_RTTI_H_

#include <string>
#include <vector>
#include <map>

#pragma warning (disable:4251) // dll and STL: https://msdn.microsoft.com/en-us/library/esew7y1w.aspx
namespace AEX
{
	// ----------------------------------------------------------------------------
	// RTTI
	class Rtti
	{
	private:
		std::string		mName;
		const Rtti	* mpBaseType;
		std::map<std::string, Rtti*> mChildren;
		
	public:
		static std::map<std::string, Rtti> Types;
		template<typename type> static const Rtti &  RttiAdd()
		{
			return RttiAdd(GetTypeName<type>().c_str(), "NULL");
		}

		template<typename type, typename parent> static const Rtti &  RttiAdd()
		{
			return RttiAdd(GetTypeName<type>().c_str(), GetTypeName<parent>().c_str());
		}

		static const Rtti & RttiAdd(const char * typeName, const char * parentName);

	public:
		// constructor
		Rtti(const char * name, const Rtti * pBaseType);
		Rtti();

		// getters
		const char * GetName() const;

		// compares this with address of otherType
		bool IsExactly(const Rtti & otherType) const;

		// walks up the hierarchy comparing with address of otherType
		bool IsDerived(const Rtti & otherType) const;

		// convert from c++ rtti to ours
		template<typename T>
		static std::string GetTypeName()
		{
			// remove class
			std::string out = typeid(T).name();
			std::size_t c = out.find("class ");
			while (c != std::string::npos)
			{
				auto s = out.begin() + c;
				auto e = s + strlen("class ");
				out.replace(s, e, "");
				c = out.find("class ");
			}
			// remove struct
			c = out.find("struct ");
			while (c != std::string::npos)
			{
				auto s = out.begin() + c;
				auto e = s + strlen("struct ");
				out.replace(s, e, "");
				c = out.find("struct ");
			}
			return out;
		}
	};


}

// Each class that supports Rtti must declare a new static
// member variable and therefore must include this code
// in its declaration. This maccro will save some time 
#define AEX_RTTI_DECL(thisType, parentType)											\
	public:									\
		virtual const AEX::Rtti& GetType() const						\
		{	return AEX::Rtti::RttiAdd<thisType, parentType>();\
		}\
		static const AEX::Rtti& TYPE()\
{return AEX::Rtti::RttiAdd<thisType, parentType>();\
}\

#define AEX_RTTI_DECL_BASE(thisType)											\
	public:									\
		virtual const AEX::Rtti& GetType() const						\
		{	return AEX::Rtti::RttiAdd<thisType>();											\
		}\
		static const AEX::Rtti& TYPE()\
{return AEX::Rtti::RttiAdd<thisType>();\
}\

#pragma warning (default:4251) // dll and STL
// ----------------------------------------------------------------------------

#endif
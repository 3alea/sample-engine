///////////////////////////////////////////////////////////////////////////////////////
///
///	\file AEXContainers.h
///	Container classes used by the engine.
///	
///	Authors: Chris Peters, Benjamin Ellinger
///	Copyright 2010, Digipen Institute of Technology
///
///////////////////////////////////////////////////////////////////////////////////////
#include <string>
#include <list>
#include <vector>
#include <map>
#include <set>

// ----------------------------------------------------------------------------
//	MACCRO
#define AEX_CONTAINERS(classname) 											\
	public:																	\
		typedef std::vector<classname> ARRAY;								\
		typedef std::list<classname>	LIST;								\
		typedef std::map<std::string, classname> DICTIONARY;				\
		typedef std::vector<classname*> PTR_ARRAY;							\
		typedef std::list<classname*>	PTR_LIST;							\
		typedef std::map<std::string, classname *> PTR_DICTIONARY;			\

#define AEX_ARRAY(classname) std::vector<classname> 
#define AEX_LIST(classname) std::list<classname> 
#define AEX_DICTIONARY(classname) std::map<std::string, classname> 
#define AEX_PTR_ARRAY(classname) std::vector<classname*> 
#define AEX_PTR_LIST(classname) std::list<classname*> 
#define AEX_PTR_DICTIONARY(classname) std::map<std::string, classname*> 
#define AEX_HASH_MAP(type1, type2) std::map<type1, type2>
#define	AEX_STRING std::string

#ifndef FOR_EACH
	#define FOR_EACH(itName, container)												\
		for(auto itName = (container).begin(); itName != (container).end(); ++itName)	

#ifndef FOR_REV_EACH
#define FOR_REV_EACH(itName, container)												\
for (auto itName = (container).rbegin(); itName != (container).rend(); ++itName)
#endif

#endif // AEX_CONTAINERS_H_

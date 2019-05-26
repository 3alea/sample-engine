#include <aexmath\AEXMath.h>
#include "src\Engine\AEX.h"
#include "JsonDemo.h"
#include <iostream>
#include <fstream>
using namespace AEX;
using std::cout;
using std::endl;

#include "json.hpp"
using json = nlohmann::json;

#pragma region Lesson: Json 101
void GettingStartedWithJson()
{
	json value;

	// write stuff in the json value

	// json value can be implicitly casted
	// to many different types. (ints, doubles, strings, etc...
	value["hello"] = "world";
	value["hI"] = PI;
	value["pI"] = PI;

	// the json::[] operator returns a new json
	value["complex_val"]["PI"] = PI;

	// a json value can also be an array
	json & array_val = value["array_val"];
	for (u32 i = 0; i < 5; ++i)
		array_val.push_back(i);

	// a json value can also be a mixed array
	json & mixed_array_val = value["mixed_array_val"];
	for (u32 i = 0; i < 5; ++i) {
		if (i % 2 == 0)// even -> push int
			mixed_array_val.push_back(i);
		else
			mixed_array_val.push_back(std::string("hello"));
	}

	// a json array keeps the order intact (i.e. no alphabetical ordering).
	json & string_array_val = value["string_array"];
	string_array_val.push_back("ff");
	string_array_val.push_back("aa");
	string_array_val.push_back("zz");
	string_array_val.push_back("bb");

	json delayed; delayed = "hahahah";
	value["test"] = delayed;

	// save to a file. 
	std::ofstream outFile("test.json");
	if (outFile.good() && outFile.is_open())
	{
		outFile << std::setw(4) << value;
		outFile.close();
	}
}
#pragma endregion

#pragma region Lesson: ISerializable Interface
struct ISerializable
{
	virtual void ToJson(json & val) = 0;
	virtual void FromJson(json & val) = 0;
};

void ToJsonVec2(json & val, const AEVec2 & in)
{
	val["x"] = in.x;
	val["y"] = in.y;
}

class MyTransform : public IComp, public ISerializable
{
	AEX_RTTI_DECL(MyTransform, IComp);
public:
	MyTransform() :IComp() {}
	
	void ToJson(json & val) {
		ToJsonVec2(val["position"], pos);
		ToJsonVec2(val["scale"], scale);
		val["rotation"] = rotation;
	}
	void FromJson(json & val){
		// sanity
		if (auto it = val.find("rotation") != val.end())
			rotation = val["rotation"];
	}

public:
	AEVec2 pos, scale;
	f32 rotation = 0.0f;
};

void TestReadWriteWithJson()
{
	MyTransform tr;

	// load
	{
		json value;
		std::ifstream inFile("input.json");
		if (inFile.good() && inFile.is_open())
		{
			inFile >> value;
			inFile.close();
			tr.FromJson(value);
		}
	}

	// save
	{
		json value;

		tr.ToJson(value);

		// save to a file. 
		std::ofstream outFile("test.json");
		if (outFile.good() && outFile.is_open())
		{
			outFile << std::setw(4) << value;
			outFile.close();
		}
	}
}
#pragma endregion

#pragma region Lesson: Factory System
struct ICreator {
	virtual IBase * Create() = 0; // pure virtual 
};
template<typename T>
struct TCreator : public ICreator {
	virtual IBase * Create()
	{
		return new T();
	}
};
class Factory : public ISystem
{
	AEX_RTTI_DECL(Factory, ISystem);
	AEX_SINGLETON(Factory);
private: 
	std::map<std::string, ICreator*> mCreators;
public:
	void Register(const char * typeName, ICreator * creator)
	{
		if (mCreators.find(typeName) == mCreators.end()) // no duplicate
			mCreators[typeName] = creator;
	}
	IBase * Create(const char * typeName)
	{
		// IMPORTANT: FIND THE CREATOR HERE
		if (mCreators.find(typeName) != mCreators.end())
			return mCreators[typeName]->Create();
		// NO CREATOR REGISTERED
		return NULL;
	}

	template <typename T> void Register() {
		Register(T::TYPE().GetName(), new TCreator<T>());
		// TODO: handle duplicate creator (avoid memory leaks)
	}
	template <typename T> T* Create() {
		return dynamic_cast<T*>(Create(T::TYPE().GetName()));
	}
};
Factory::Factory() :ISystem() {}
#define aexFactory (Factory::Instance())

#pragma region Tests
void SaveObjectToJson(GameObject * go, json & val)
{
	val["name"] = go->GetName();
	json & comps = val["comps"];
	for (u32 i = 0; i < go->GetComps().size(); ++i)
	{
		json compVal;
		IComp * comp = go->GetComp(i);
		compVal["type"] = comp->GetType().GetName();
		// write the component 
		// TODO(implement ToJson in ALL components). 
		// comp->ToJson(compVal);
		// BIG HACK REMOVE ME!!!!
		if (auto tr = dynamic_cast<MyTransform *>(comp))
		{
			tr->ToJson(compVal);
		}
		comps.push_back(compVal);
	}
}
void LoadObjectFromJson(GameObject * obj, json & val)
{
	obj->Shutdown(); // clear everything before loading. 
	obj->SetName(val["name"].get<std::string>().c_str());
	json & comps = *val.find("comps");
	for (auto it = comps.begin(); it != comps.end(); ++it)
	{
		json & compVal = *it;
		std::string typeName = compVal["type"].get<std::string>();
		IBase * newComp = aexFactory->Create(typeName.c_str());
		if (auto tr = dynamic_cast<MyTransform *>(newComp))
			tr->FromJson(compVal);
		obj->AddComp((IComp*)newComp);
	}
}
void Test_SerializeJsonObject()
{
	aexFactory->Register<GameObject>();
	aexFactory->Register<MyTransform>();
	aexFactory->Register<TransformComp>();
	
	GameObject go;

	// load
	{
		json value;
		std::ifstream inFile("test.json");
		if (inFile.good() && inFile.is_open())
		{
			inFile >> value;
			inFile.close();
			LoadObjectFromJson(&go, value);
		}
	}
	// save
	{
		json value;
		SaveObjectToJson(&go, value);
		// save to a file. 
		std::ofstream outFile("copy.json");
		if (outFile.good() && outFile.is_open())
		{
			outFile << std::setw(4) << value;
			outFile.close();
		}
	}

}
#pragma endregion
#pragma endregion

#pragma region Lesson(NEW): Overload Stream Operators)

#pragma region Part 1: Stream Operators For Json
// overload the stream operator for json for known types

// basic types - write
json & operator<<(json &j, const int & val) { j = val; return j; }
json & operator<<(json &j, const float & val) { j = val;return j; }
json & operator<<(json &j, const double & val) { j = val;return j; }
json & operator<<(json &j, const bool & val) { j = val; return j;}
json & operator<<(json &j, const std::string & val) { j = val; return j;}


// basic types - read
int & operator>>(const json &j, int & val) { val = j; return val; }
float & operator>>(const json &j, float & val) { val = j; return val; }
double& operator>>(const json &j, double & val) { val = j; return val; }
bool & operator>>(const json &j, bool & val) { val = j; return val; }
std::string & operator>>(const json &j, std::string & val) { val = j.get<std::string>(); return val; }

// complex types - write
nlohmann::json& operator<<(nlohmann::json& j, const AEX::AEVec2 & v)
{
	j["x"] = v.x;
	j["y"] = v.y;
	return j;
}
nlohmann::json& operator<<(nlohmann::json& j, const AEX::AEVec3 & v)
{
	j["x"] = v.x;
	j["y"] = v.y;
	j["z"] = v.z;
	return j;
}
nlohmann::json& operator<<(nlohmann::json& j, const AEX::Transform & tr)
{
	j["pos"] << tr.mTranslation;
	j["posZ"] << tr.mTranslationZ;
	j["sca"] << tr.mScale;
	j["rot"] = tr.mOrientation;
	return j;
}
nlohmann::json& operator<<(nlohmann::json& j, const AEX::TransformComp& mtr)
{
	j["local"] << mtr.mLocal;
	return j;
}

// complex types - read
AEX::AEVec2& operator>>(const nlohmann::json& j, AEX::AEVec2& v)
{
	if (j.find("x") != j.end())
		v.x = j["x"];
	if (j.find("y") != j.end())
		v.y = j["y"];
	return v;
}
AEX::AEVec3& operator>>(const nlohmann::json& j, AEX::AEVec3& v)
{
	if (j.find("x") != j.end())
		v.x = j["x"];
	if (j.find("y") != j.end())
		v.y = j["y"];
	if (j.find("z") != j.end())
		v.z = j["z"];
	return v;
}
AEX::Transform& operator>>(const nlohmann::json& j, AEX::Transform& mtr)
{
	if (j.find("pos") != j.end())
		j["pos"] >> mtr.mTranslation;
	if (j.find("posZ") != j.end())
		j["posZ"] >> mtr.mTranslationZ;
	if (j.find("sca") != j.end())
		j["sca"] >> mtr.mScale;
	if (j.find("rot") != j.end())
		mtr.mOrientation = j["rot"];
	return mtr;
}
AEX::TransformComp& operator>>(const nlohmann::json& j, AEX::TransformComp& mtr)
{
	if(j.find("local") != j.end())
		j["local"] >> mtr.mLocal;
	return mtr;
}

#pragma region TESTS
void test_stream()
{
	json j;
	j["test"] = "hello";
	AEX::TransformComp tr;
	j["transform"] << tr;
	std::cout << std::setw(4) << j << '\n';

	// modify jason. note: here I'm using json::find() so that if it's not there it crashes (hard fail).
	(*(*(*j.find("transform")).find("local")).find("rot")) = 123456.0f;

	// dump into string to compare results. 
	std::string check = j.dump();
	std::cout << std::setw(4) << j << '\n';

	// seriaizer back transform with stream operator
	j["transform"] >> tr;

	// dump the modified transform to another json
	json cmp; 
	cmp << tr;
	std::string cmpstr = cmp.dump();
	bool diff = strcmp(check.c_str(),cmpstr.c_str()) != 0;
	std::cout << std::setw(4) << cmp << '\n';
	std::cout << (diff ? "DIFF GOOOD\n" : "DIFF BAAAD\n");
	
}
#pragma endregion
#pragma endregion

#pragma region Part 2: New ISerializable 
struct ISerializable2
{
	virtual json& operator<< (json&j) const = 0;
	virtual void operator>>(json&j) = 0;
	virtual std::ostream& operator << (std::ostream & o) const
	{
		json j;
		this->operator<<(j);
		return o << j;
	}
};

struct IComp2 : public AEX::IComp, public ISerializable2
{
	virtual json& operator<< (json&j)  const
	{
		return j;
	}
	virtual void operator>>(json&j) {
	}
	virtual std::ostream& operator << (std::ostream & o) const
	{
		json j;
		this->operator<<(j);
		return o << j;
	}
	friend nlohmann::json& operator<<(nlohmann::json& j, const IComp2& comp)
	{
		comp.operator<<(j);
		return j;
	}
	friend IComp2& operator>>(nlohmann::json& j, IComp2& comp)
	{
		comp.operator>>(j);
		return comp;
	}
	friend std::ostream& operator << (std::ostream & o, const IComp2& comp)
	{
		json j;
		comp.operator<<(j);
		return o << j;
	}
};
class GameObject2 : public AEX::GameObject, public ISerializable2
{
public:
	virtual json& operator<< (json&j)  const
	{
		// serialize name (todo: implement this as a Property). 
		j["name"] = mName;

		// serialize components
		json & comps = j["comps"];
		for (u32 i = 0; i < this->GetComps().size(); ++i)
		{
			json compJson;
			IComp * comp = GetComp(i);
			compJson["__type"] = comp->GetType().GetName();

			// write the component  using stream operators

			// note only IComp3 implement it
			if (auto comp2 = dynamic_cast<IComp2*>(comp))
			{
				comp2->operator<<(compJson);
			}

			// add to json array
			comps.push_back(compJson);
		}

		return j;
	}
	virtual void operator>>(json&j) {

		// get components
		Shutdown(); // clear everything before loading. 

		// read name (this should really be another property).
		SetName(j["name"].get<std::string>().c_str());

		// read comps and allocate them
		json & comps = *j.find("comps");
		for (auto it = comps.begin(); it != comps.end(); ++it)
		{
			// get json object for that component
			json & compJson = *it;

			// read type and create with factory
			std::string typeName = compJson["__type"].get<std::string>();
			IBase * newComp = aexFactory->Create(typeName.c_str());

			// error check - Factory couldn't allocate memory
			if (newComp == nullptr)
				continue;

			// only stream components deriving from IComp2
			if (auto comp2 = dynamic_cast<IComp2*>(newComp))
				comp2->operator>>(compJson);

			// add new comp object
			AddComp((IComp*)newComp);
		}
	}
	friend nlohmann::json& operator<<(nlohmann::json& j, const GameObject2& obj)
	{
		obj.operator<<(j);
		return j;
	}
	friend GameObject2& operator>>(nlohmann::json& j, GameObject2& obj)
	{
		obj.operator>>(j);
		return obj;
	}
	virtual std::ostream& operator << (std::ostream & o) const
	{
		json j;
		this->operator<<(j);
		return o << j;
	}
	friend std::ostream& operator<< (std::ostream & o, const GameObject2& obj)
	{
		json j;
		obj.operator<<(j);
		return o << j;
	}
};

#pragma region TESTS
struct StreamComp : public IComp2
{
	AEX_RTTI_DECL(StreamComp, IComp2);

	// test vars
	int life;
	float duration;
	bool alive;
	Transform local;

	// Overload stream operators for json
	virtual json& operator <<(json& j)const
	{
		// write local
		j["local"] << local;
		j["life"] << life;
		j["duration"] << duration;
		j["alive"] << alive;
		return j;
	}
	virtual void operator >>(json& j)
	{
		if (j.find("local") != j.end()) j["local"] >> local;
		if (j.find("life") != j.end()) j["life"] >> life;
		if (j.find("duration") != j.end()) j["duration"] >> duration;
		if (j.find("alive") != j.end()) j["alive"] >> alive;
	}
};
void test_stream_object()
{
	// register new comp with factory
	aexFactory->Register<StreamComp>();

	cout << "\n--------------TEST WRITE-----------------\n";

	// test object
	GameObject2 go;

	// create new comp on it
	go.NewComp<StreamComp>();
	cout << std::setw(4) << go;

	// do modifications
	go.GetComp<StreamComp>()->alive = false;
	go.GetComp<StreamComp>()->local.mTranslation = AEVec2(10,10);
	go.GetComp<StreamComp>()->duration = 12345.6789f;
	go.GetComp<StreamComp>()->life = 999999;
	cout << std::setw(4) << go;
	
	cout << "\n--------------TEST READ-----------------\n";

	json j;
	j << go;

	// stream in new object and print
	GameObject2 go2;
	j >> go2;
	cout << std::setw(4) << go2;
}
#pragma endregion	// TESTS
#pragma endregion	// Part 2: New ISerializable 
#pragma endregion	// Lesson: Stream operators

#pragma region Lesson(Advanced): Relfection and Properties.
/*!
	/details
	We know we're going to put the serialization interface in the IBase class
	or at the very least, for the purist, we'll integrate serialziation at the IComp 
	class level. 

	While the system in the previous lesson is robust enough to handle serialization
	we'll see that for custom components, a lot of manual labor will be involved
		
		j["health"] = ...
		j["vision_range"] = ...
		j["bla_bla_bla"] = ...

	This is where properties come in, they help get rid of the tedious and error-prone
	task of saving the *relevant* variables of the custom component, therefore saving 
	time. An example is the Zero Engine [Property] tag. In Unity, setting a variable as public
	will achieve the same effect, while making it private flags it and the serialization process
	simply skips it. 

		// Zilch Script
		[Property] var health : Integer;	// saved
		var counter : Integer;			// not saved

	So how do we go about this? Well, if we want to automatize the writing of the variables to json,
	we'll need to store them somewhere, so that the writing process goes over each of the properties
	and write them to json. 
	
	Now we know we'll have container of some types for our properties. Properties are in essence a variable
	with a flag, so that it's saved by the automatic serialization. Also, properties should be able to contain any type
	so we'll need *templates* to encapsulate the variable that we want to save. Usage code would look like this:
		
		struct MyCustomComp : public IComp
		{
			Property<int> health;	// saved 
			int counter;			// not saved
		};

			
	because we are also going to *USE* the variable in the code for logic, ideally, the Property would behave exactly
	like the variable. It is a proxy. Concretely, it will act like a *pointer or reference* to the type. Similar to iterators in STL.


		struct MyCustomComp
		{
			//...
			void Update()
			{
				if(health < 0) ...
				health++;
				health += 2;
			}
		};
	
	Let's define the property structure

		template<typename T> struct Property
		{
			T val; // THE value the propety represent. Memory, basically.
			
			// conversion operators
			int& operator(T)();
			int& operator=(const T & rhs);
		};

	Then, we add the serialization as friend functions,	thus overloading the << operators for the property of type T(more here). 
	These functions will write to the json value using the << operator. This means that for this to work `typename T` must also
	overload them. For this to work, we also need to overload the << and >> operators of atomic data types (int, float, bool...) done above.

		template <typename T> struct Property
		{
			// value

			// conversion operators

			// stream operators overloads
			friend nlohmann::json& operator<<(nlohmann::json& j, const Property<T>& prop);
			friend Property<T>& operator>>(nlohmann::json& j, const Property<T>& prop);
		};
	
	see #test_property_1 for sample usage. 

	From the restults of #test_property_1, we see that it doesn't save the name of the property, 
	but just the value. Funnily enough, the json acts as a proxy, just like the property. 
	We'll want to add the name of the property, as declared in the custom struct. 

		property<int> health; // json -> {"health" = ...}

	So we need to store the name of the variable somewhere. We could add a variable of type 
	string to the property and save that in the stream functions. However, we can leverage 
	the container of our properties to actually store that info. By choosing a key-value container
	like a map<string, Property*>, where the value is a pointer to a base interface that overloads the	
*/
#include <unordered_map>

class PROP_MAP : public ISerializable2
{
public:
	std::unordered_map<std::string, ISerializable2*> _props_;
	// overload stream operators
	friend nlohmann::json& operator<<(nlohmann::json& j, const PROP_MAP& properties)
	{
		for (auto prop : properties._props_)
		{
			// note i need to call the explicit operator here because the
			// property woould resolve in the value itself and call one of the global 
			// functions instead.
			prop.second->operator<<(j[prop.first]);
		}
		return j;
	}
	friend PROP_MAP& operator>>(nlohmann::json& j, PROP_MAP& properties)
	{
		for (auto prop : properties._props_)
		{
			prop.second->operator>>(j[prop.first]);
		}
		return properties;
	}
	friend std::ostream& operator << (std::ostream & o, const PROP_MAP& properties)
	{
		json j;
		o << properties.operator<<(j);

		return o;
	}
	virtual json& operator<< (json&j) const
	{
		for( auto prop : this->_props_)
		{
			prop.second->operator<<(j[prop.first]);
		}
		return j;
	}
	virtual void operator>>(json&j)
	{
		for (auto prop : this->_props_)
		{
			prop.second->operator>>(j[prop.first]);
		}
	}
	virtual std::ostream& operator << (std::ostream & o) const
	{
		json j;
		o << this->operator<<(j);

		return o;
	}
};

/*!
	/details

	Property class acts as a proxy for variable typename T. It's a proxy
	this is a common programming pattern.

	Additionally, Property implements the ISerializable2 interface to, again
	act as a proxy for `val`. This means that whatever the type of `val` is, it 
	must also implement the serializable interface. Note that for basic data types, 
	specifically int, `float`, `double`, `bool` and `string`.
*/
template <typename T> struct Property : public ISerializable2
{

	// value
	T val;
	
	// constructors
	Property() {}
	Property(const char * name, PROP_MAP& prop_map)
	{
		// dont' allow duplicates
		assert(prop_map._props_.find(name) == prop_map._props_.end());
		prop_map._props_[name] = this;
	}
	Property(const char * name, const T &value, PROP_MAP& prop_map)
		:Property(name, prop_map)
	{

		val = value;
	}

	// conversion operators
	operator T() const { return val; }
	T& operator=(const T & rhs) { val = rhs; return val; }

	// overload stream operators
	friend nlohmann::json& operator<<(nlohmann::json& j, const Property<T>& prop)
	{
		j << prop.val;
		return j;
	}
	friend Property<T>& operator>>(nlohmann::json& j, Property<T>& prop)
	{
		j >> prop.val;
		return prop;
	}
	friend std::ostream& operator << (std::ostream & o, const Property<T>&prop)
	{
		json j;
		j << prop.val;

		o << j;
		return o;
	}

	virtual json& operator<< (json&j) const
	{
		j << val;
		return j;
	}
	virtual void operator>>(json&j){
		j >> val;

	}
	virtual std::ostream& operator << (std::ostream & o)  const
	{
		json j;
		j << val;
		
		o << j;
		return o ;
	}
};

// these maccros will help when declaring properties. 
// they leverage moderne C++ initializer lists to initialize the values (PROP_VAL and PROP_VAL_EX)
// see test functions for sample usage

// properties container passed explicitly. 
#define PROP_EX(_type_, _name_, _properties_) Property<_type_> _name_ {#_name_, _properties_}
#define PROP_VAL_EX(_type_, _name_, val, _properties_) Property<_type_> _name_ {#_name_, val, _properties_}

// these assume that a container variable called 'properties' exists in the local scope of the code
// that uses these maccros. This is intended to work with the IComp3 class example below. 
#define PROP(_type_, _name_) PROP_EX(_type_, _name_, properties)
#define PROP_VAL(_type_, _name_, val)  PROP_VAL_EX(_type_, _name_, val, properties)


#pragma region TEST
void test_property_1()
{
	{
		Property<int> health;
		health = 10;
		cout << health << "\n\n";

		// test json serialization
		json j;
		j << health;
		cout << std::setw(4) << j;

		// modify json and set health
		j = 12345;
		j >> health;
		cout << health << "\n\n";
	}
	// test with custom transform
	{
		Property<TransformComp> comp;
		json j;
		j << comp;
		cout << std::setw(4) << j;


		// modify json and set health
		j["local"]["pos"]["x"] = 3.1456;
		j >> comp;
		json jcmp;
		jcmp << comp;
		cout << std::setw(4) << jcmp;
	}
};
void test_property_2()
{
	// now we want to test the automatic serialization using
	// just a map, that we will fill automatically using the 
	// custom constructors.


	{
		PROP_MAP properties;

		PROP_EX(int, i, properties);
		PROP(float, f);	// IMPORTANT this maccros assumes properties exists. it's meant to add to  be used in classes.
		PROP(double, d);
		PROP(bool, b);
		PROP(std::string, str);

		// test
		str = "hello";
		i = 10;
		f = 3.14f;
		d = 68.98765432131;
		b = false;

		PROP_VAL(int, ii, 1234);
		PROP_VAL(float, ff, 180.0f);
		PROP_VAL(double, dd, 1234.1234567890);
		PROP_VAL(bool, bb, true);
		PROP_VAL(std::string, sstr, "hello");

		json j;
		properties.operator<< (j);
		cout << std::setw(4) << j;

		j["str"] = "READING WORKS";
		j["f"] = 666.666f;
		properties.operator>>(j);
		properties.operator<<(cout << std::setw(4));
	}
}


/*!	-- test 3 -- 
	
	this test demonstrate the convenience of properties.

	Notice how by just deriving from IComp3, the structures
	automaticall inherit all the serialization functions
	so stream operator works in any circumstance.

	Inheritance is easy and properties are written as if
	part of the derived class (see struct compB)

	Aggregation also works because IComp3 ALSO DERIVES from ISerializable2.
	Therefore, it can also be stored in a PROP_MAP container which allows us
	to make components complex properties (struct/class containings properties).
	The output is as expected

*/

struct IComp3 : public AEX::IComp, public ISerializable2
{
	PROP_MAP properties;

	virtual json& operator<< (json&j)  const
	{
		properties.operator<< (j);
		return j;
	}
	virtual void operator>>(json&j) {
		properties.operator>>(j);
	}
	virtual std::ostream& operator << (std::ostream & o) const
	{
		json j;
		this->operator<<(j);
		return o << j;
	}
	friend nlohmann::json& operator<<(nlohmann::json& j, const IComp3& comp)
	{
		comp.operator<<(j);
		return j;
	}
	friend IComp3& operator>>(nlohmann::json& j, IComp3& comp)
	{
		comp.operator>>(j);
		return comp;
	}

};

// basic usage
struct compA : public IComp3
{
	AEX_RTTI_DECL(compA, IComp3);
	PROP_VAL(int, life, 10);
	PROP_VAL(bool, isDead, true);
	PROP_VAL(std::string, name, "Billy Jean");
};

// inhertiance
struct compB : public compA
{
	AEX_RTTI_DECL(compB, compA);
	PROP_VAL(float, ff, 2.45f);
};

// aggregation
struct compC : public IComp3
{
	AEX_RTTI_DECL(compC, IComp3);
	PROP(compA, a);
};

void test_property_3()
{
	compA A;
	compB B;
	compC C;

	// test standard manips
	A.life = 999999;
	A.isDead = false;
	A.name = "bla bla";

	// direct manip example
	Property<float> * ff_prop = dynamic_cast<Property<float>*>(B.properties._props_["ff"]);
	ff_prop->val = 66666.6666f;

	// copy complex prop
	C.a = A; 

	cout << "--------------TEST WRITE-----------------\n";

	// explicity call the stream write operator
	json j;
	A.operator<<(j["a"]);
	B.operator<<(j["b"]);
	C.operator<<(j["c"]);
	std::cout << std::setw(4) << j;

	cout << "--------------TEST READ -----------------\n";
	
	// make copies and stream in. 
	compA AA;
	compB BB;
	compC CC;

	// explict call to stream in operator
	AA.operator>>(j["a"]);
	BB.operator>>(j["b"]);
	CC.operator>>(j["c"]);

	// explicit call to stream out operator for std::ostream
	AA.operator<<(std::cout << std::setw(4));
	BB.operator<<(std::cout << std::setw(4));
	CC.operator<<(std::cout << std::setw(4));

}
// -- test 4
class GameObject3 : public AEX::GameObject, public ISerializable2
{
public:
	PROP_MAP properties; 
	virtual json& operator<< (json&j)  const
	{
		// serialize properties
		properties.operator<< (j);

		// serialize name (todo: implement this as a Property). 
		j["name"] = mName;

		// serialize components
		json & comps = j["comps"];
		for (u32 i = 0; i < this->GetComps().size(); ++i)
		{
			json compJson;
			IComp * comp = GetComp(i);
			compJson["__type"] = comp->GetType().GetName();
			
			// write the component  using stream operators

			// note only IComp3 implement it
			if (auto comp2 = dynamic_cast<IComp3*>(comp))
			{
				comp2->operator<<(compJson);
			}

			// add to json array
			comps.push_back(compJson);
		}

		return j;
	}
	virtual void operator>>(json&j) {

		// read properties
		properties.operator>>(j);

		// get components
		Shutdown(); // clear everything before loading. 

		// read name (this should really be another property).
		SetName(j["name"].get<std::string>().c_str());

		// read comps and allocate them
		json & comps = *j.find("comps");
		for (auto it = comps.begin(); it != comps.end(); ++it)
		{
			// get json object for that component
			json & compJson = *it;

			// read type and create with factory
			std::string typeName = compJson["__type"].get<std::string>();
			IBase * newComp = aexFactory->Create(typeName.c_str());

			// error check - Factory couldn't allocate memory
			if (newComp == nullptr)
				continue;

			// only stream components deriving from IComp3
			if (auto comp2 = dynamic_cast<IComp3*>(newComp))
				comp2->operator>>(compJson);

			// add new comp object
			AddComp((IComp*)newComp);
		}
	}
	virtual std::ostream& operator << (std::ostream & o) const
	{
		json j;
		this->operator<<(j);
		return o << j;
	}
	friend nlohmann::json& operator<<(nlohmann::json& j, const GameObject3& obj)
	{
		obj.operator<<(j);
		return j;
	}
	friend GameObject3& operator>>(nlohmann::json& j, GameObject3& obj)
	{
		obj.operator>>(j);
		return obj;
	}
};
void test_property_object()
{
	cout << "--------------TEST WRITE-----------------\n";
	// test write
	GameObject3 go;
	go.NewComp<compA>();
	go.NewComp<compB>();
	go.NewComp<compC>();
	go.operator<<(cout << std::setw(4));

	cout << "--------------TEST READ -----------------\n";
	
	// register comps
	aexFactory->Register<compA>();
	aexFactory->Register<compB>();
	aexFactory->Register<compC>();
	
	// write first object to json
	json j;
	go.operator<<(j);

	// stream json into new object and print
	GameObject3 go2;
	go2.operator>>(j);
	go2.operator<<(cout << std::setw(4));

}
#pragma endregion
#pragma endregion

#pragma region Gamestate functions - they just call the particular tests functions
void JsonDemo::Initialize()
{
	//Test_SerializeJsonObject();		// test factory
	//test_stream();					// test stream API
	test_stream_object();				// test tream API with Facory system
	//test_property_1();				// basic property usage
	//test_property_2();				// simple automatic serialization 
	//test_property_3();				// property composition
	//test_property_object();		// GameObjectserialization
	cout << "\n\n\n\n";
	exit(1);
}
void JsonDemo::LoadResources()
{
}
void JsonDemo::Update()
{
}
void JsonDemo::Render()
{

}
#pragma endregion

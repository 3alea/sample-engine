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

// overload the stream operator for json for basic types (will be useful later)
json & operator<<(json &j, const int & val) { j = val; return j; }
json & operator<<(json &j, const float & val) { j = val;return j; }
json & operator<<(json &j, const double & val) { j = val;return j; }
json & operator<<(json &j, const bool & val) { j = val; return j;}
json & operator<<(json &j, const std::string & val) { j = val; return j;}

int & operator>>(const json &j, int & val) { val = j; return val; }
float & operator>>(const json &j, float & val) { val = j; return val; }
double& operator>>(const json &j, double & val) { val = j; return val; }
bool & operator>>(const json &j, bool & val) { val = j; return val; }
std::string & operator>>(const json &j, std::string & val) { val = j.get<std::string>(); return val; }

nlohmann::json& operator<<(nlohmann::json& j, const AEX::AEVec2 & v)
{
	j["x"] = v.x;
	j["y"] = v.y;
	return j;
}
AEX::AEVec2& operator>>(const nlohmann::json& j, AEX::AEVec2& v)
{
	if (j.find("x") != j.end())
		v.x = j["x"];
	if (j.find("y") != j.end())
		v.y = j["y"];
	return v;
}
nlohmann::json& operator<<(nlohmann::json& j, const AEX::AEVec3 & v)
{
	j["x"] = v.x;
	j["y"] = v.y;
	j["z"] = v.z;
	return j;
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


// overload the stream operator
nlohmann::json& operator<<(nlohmann::json& j, const AEX::Transform & tr)
{
	j["pos"] << tr.mTranslation;
	j["posZ"] << tr.mTranslationZ;
	j["sca"] << tr.mScale;
	j["rot"] = tr.mOrientation;
	return j;
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

nlohmann::json& operator<<(nlohmann::json& j, const AEX::TransformComp& mtr)
{
	j["local"] << mtr.mLocal;
	return j;
}
AEX::TransformComp& operator>>(const nlohmann::json& j, AEX::TransformComp& mtr)
{
	if(j.find("local") != j.end())
		j["local"] >> mtr.mLocal;
	return mtr;
}


#pragma region TESTS
void TestStream()
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

#pragma region Lesson: Advanced streaming and Properties.
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
struct ISerializable2
{
	virtual json& operator<< (json&j) = 0;
	virtual void operator>>(json&j) = 0;
	virtual std::ostream& operator << (std::ostream & o)
	{
		return o;
	}

};
class PROP_MAP : public ISerializable2
{
public:
	std::unordered_map<std::string, ISerializable2*> _props_;
	// overload stream operators
	friend nlohmann::json& operator<<(nlohmann::json& j, PROP_MAP& properties)
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
	friend std::ostream& operator << (std::ostream & o, PROP_MAP& properties)
	{
		json j;
		o << properties.operator<<(j);

		return o;
	}
	virtual json& operator<< (json&j)
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
	virtual std::ostream& operator << (std::ostream & o)
	{
		json j;
		o << this->operator<<(j);

		return o;
	}
};

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

	virtual json& operator<< (json&j)
	{
		j << val;
		return j;
	}
	virtual void operator>>(json&j){
		j >> val;

	}
	//virtual std::ostream& operator << (std::ostream & o)
	//{
	//	json j;
	//	j << val;
	//	
	//	o << j;
	//	return o ;
	//}
};

#define PROP_EX(_type_, _name_, _properties_) Property<_type_> _name_ {#_name_, _properties_}
#define PROP_VAL_EX(_type_, _name_, val, _properties_) Property<_type_> _name_ {#_name_, val, _properties_}

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
void test_property_3()
{
	struct IComp2 : public AEX::IComp, public ISerializable2
	{
		PROP_MAP properties;

		virtual json& operator<< (json&j)
		{
			j << properties;
			properties.operator<< (j);
			return j;
		}
		virtual void operator>>(json&j) {
			properties.operator>>(j);

		}
		virtual std::ostream& operator << (std::ostream & o)
		{
			properties.operator<<(o << std::setw(4));
			return o;
		}

	};

	struct compA : public IComp2
	{
		PROP_VAL(int, life, 10);
		PROP_VAL(bool, isDead, true);
	};

	struct compB : public IComp2
	{
		//enum Enum { enum_1, enum_2, enum_3 };
		//PROP_VAL(Enum, myEnum, enum_2);
		PROP_VAL(std::string, mName, "Billy Jean");
	};

	// inhertiance
	struct compC : public compA
	{
		PROP_VAL(float, ff, 2.45f);
		//PROP(compA, a);
		//PROP(compB, b);
	};

	// aggregation
	struct compD : public IComp2
	{
		PROP(compC, a);
		//PROP(compB, b);
	};

	compA A;
	compB B;
	compC C;

	json j;
	A.operator<<(j["a"]);
	B.operator<<(j["b"]);
	C.operator<<(j["c"]);
	std::cout << std::setw(4) << j;
}

#pragma endregion
#pragma endregion

// ----------------------------------------------------------------------------
// GAMESTATE FUNCTIONS
void JsonDemo::Initialize()
{
	
	//Test_SerializeJsonObject();		// test factory
	//TestStream();						// test stream API
	//test_property_1();
	//test_property_2();
	test_property_3();
	cout << "\n\n\n\n";
	exit(1);
}
void JsonDemo::LoadResources()
{
}
void JsonDemo::Update()
{
	// get main window dimensions
	auto mainWin = aexWindowMgr->GetMainWindow();
	auto winWH = AEVec2(f32(mainWin->GetWidth()), f32(mainWin->GetHeight()));

	// control the engine
	if (aexInput->KeyTriggered('B'))
		aexTime->LockFrameRate(!aexTime->FrameRateLocked());
	if (aexInput->KeyTriggered('V'))
		aexGraphics->SetVSyncEnabled(!aexGraphics->GetVSyncEnabled());
	if (aexInput->KeyPressed(VK_OEM_PLUS))
		aexTime->SetMaxFrameRate(aexTime->GetMaxFrameRate() + 1.0);
	if (aexInput->KeyPressed(VK_OEM_MINUS))
		aexTime->SetMaxFrameRate(aexTime->GetMaxFrameRate() - 1.0);
	if (aexInput->KeyTriggered('F'))
		mainWin->SetFullScreen(!mainWin->GetFullScreen());

	f32 fps = (f32)aexTime->GetFrameRate();
	std::string wintitle = "Simple Demo - FPS: "; wintitle += std::to_string(fps);
	if (aexTime->FrameRateLocked())	wintitle += "(LOCKED)";
	wintitle += " - VSYNC: ";	wintitle +=	aexGraphics->GetVSyncEnabled() ? "ON" : "OFF";
	wintitle += " - Controls: FPS: 'B', '+/-'. VSYNC: 'V'";
	aexWindowMgr->GetMainWindow()->SetTitle(wintitle.c_str());


	Logic::Instance()->Update();

}
void JsonDemo::Render()
{
	auto mainWin = aexWindowMgr->GetMainWindow();
	auto winWH = AEVec2(f32(mainWin->GetWidth()), f32(mainWin->GetHeight()));

	aexGraphics->SetViewport(0, 0, s32(winWH.x), s32(winWH.y));
	aexGraphics->SetClearColor(Color(0.7f,0.7f,0.7f,1.0f));
	aexGraphics->ClearFrameBuffer();

	auto mp = aexInput->GetMousePos();
	aexGraphics->DrawCircle(mp.x, mp.y, 50, Color(0,0,0,1));

	aexGraphics->Present();
}

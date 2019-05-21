#include <aexmath\AEXMath.h>
#include "src\Engine\AEX.h"
#include "JsonDemo.h"
#include <iostream>
#include <fstream>
using namespace AEX;

#include "json.hpp"
using json = nlohmann::json;

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

// ----------------------------------------------------------------------------
// GAMESTATE FUNCTIONS
void JsonDemo::Initialize()
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

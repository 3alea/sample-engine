#include "src\Engine\AEX.h"
#include "SimpleDemo.h"
#include <iostream>

using namespace AEX;

namespace AEX
{
	class IComp : public IBase
	{
		AEX_RTTI_DECL(IComp, IBase);
		friend class GameObject; // friends can't touch privates
	public: 
		IComp() : IBase(), mOwner(NULL), mbEnabled(true) {} // do nothing for now.
		virtual void Initialize() {}
		virtual void Update() {}
		virtual void Shutdown() {}
		virtual ~IComp() {}

		GameObject * GetOwner() { return mOwner; }

		// TODO: 
		void Enable(bool enabled) { mbEnabled = enabled; }
	protected:
		GameObject *	mOwner;		// only one owner!!
		bool			mbEnabled;	// ENABLED OR NOT
	};

	class GameObject : public IBase
	{
		AEX_RTTI_DECL(GameObject, IBase);
	public: 
		GameObject() :IBase() {} // do nothing for now.
		virtual void Initialize() {
			for (u32 i = 0; i < mComponents.size(); ++i)
				mComponents[i]->Initialize();
		}
		virtual void Shutdown() {
			for (u32 i = 0; i < mComponents.size(); ++i)
				mComponents[i]->Shutdown();
		}
		virtual ~GameObject() {
			ClearComps();
		}
		void Enable(bool enabled){
			for (u32 i = 0; i < mComponents.size(); ++i)
				mComponents[i]->Enable(enabled);
		}

		// component management
		IComp * AddComp(IComp* newComp) 
		{
			for (u32 i = 0; i < mComponents.size(); ++i)
				if (mComponents[i] == newComp)
					return newComp;
			mComponents.push_back(newComp);
			newComp->mOwner = this;
			return newComp;
		}
		void RemoveComp(IComp * toRemove) 
		{
			for (u32 i = 0; i < mComponents.size(); ++i)
				if (mComponents[i] == toRemove) {
					delete toRemove;
					mComponents.erase(mComponents.begin() + i);
					return;
				}
		}
		void ClearComps() {
			while (mComponents.size()){
				delete mComponents.back(); 
				mComponents.pop_back();
			}
		}
		IComp * GetComp(const char * compTypeName) 
		{
			for (u32 i = 0; i < mComponents.size(); ++i) {
				std::string tmpType = mComponents[i]->GetType().GetName();
				if ( tmpType == compTypeName)
					return mComponents[i];
			}
		}
		u32 GetCompCount() { return mComponents.size(); }

		// debug only!!
		std::vector<IComp*> &GetComps() { return mComponents; }

	private:
		std::vector<IComp*> mComponents;
	public:

		template<typename T>
		T * GetComp() {
			return dynamic_cast<T*>(GetComp(T::TYPE().GetName()));
		}

		template<typename T>
		T * NewComp(){
			return dynamic_cast<T*>(AddComp(new T));
		}
	};


	class Logic :public ISystem
	{
		AEX_RTTI_DECL(Logic, ISystem);
		AEX_SINGLETON(Logic);

	public:
		virtual void Update()
		{
			FOR_EACH(comp, mComps)
				/*TODO:if((*comp)->Enabled())*/ 
				(*comp)->Update();
		}

		// component management
		void AddComp(IComp * logicComp){
			mComps.remove(logicComp); // no duplicates
			mComps.push_back(logicComp); 
		}
		void RemoveComp(IComp * logicComp){
			mComps.remove(logicComp);
		}
		void ClearComps(){
			mComps.clear();
		}
	private:
		std::list<IComp *> mComps;
	};
	Logic::Logic() {}

	class LogicComp : public IComp
	{
		AEX_RTTI_DECL(LogicComp, IComp);
	public:
		LogicComp() : IComp() {}
		void Initialize() { 
			Logic::Instance()->AddComp(this); 
		}
		void Shutdown() {
			Logic::Instance()->RemoveComp(this);
		}
	};

	class TransformComp : public IComp
	{
		AEX_RTTI_DECL(TransformComp, IComp);
	public:
		TransformComp() :IComp() {}
		AEX::Transform mLocal;
	};

	class DummyComp : public LogicComp
	{
		AEX_RTTI_DECL(DummyComp, LogicComp);
	public: 
		DummyComp() : LogicComp() {}

		void Initialize() {
			LogicComp::Initialize();
			std::cout << "Dummy " << GetName() << " Initialize" << std::endl;
		}
		void Shutdown() {
			std::cout << "Dummy " << GetName() << " Shutdown" << std::endl;
			LogicComp::Shutdown();
		}
		void Update() {
			if(mbEnabled)
				std::cout << "Dummy " << GetName() << " Update" << std::endl;
		}
	};

}


void TestGameObjects()
{
	GameObject go;
	go.AddComp(new DummyComp());
	go.AddComp(new TransformComp());
	go.Initialize();
	auto comps = go.GetComps();
	for (u32 i = 0; i < go.GetCompCount(); ++i)
		comps[i]->Update();
	go.Enable(false);
	for (u32 i = 0; i < go.GetCompCount(); ++i)
		comps[i]->Update();
	go.Enable(true);
	for (u32 i = 0; i < go.GetCompCount(); ++i)
		comps[i]->Update();
	go.Shutdown();
}

void TestGetComp()
{
	GameObject go;
	
	// adding like this is tedious.
	DummyComp * tr = go.NewComp<DummyComp>();
	tr->SetName("HelloWorld");
	go.Initialize();
	tr->Update();

	DummyComp * tr2 = go.GetComp<DummyComp>();
	tr2->Update();
	
	__debugbreak();
}

void TestLogicSystem()
{
	GameObject * go = new GameObject();
	auto d = go->NewComp<DummyComp>();
	go->Initialize();
}

// HELPER FUNCTIONS DECLARATIONS
void DrawSprite(AEVec3 pos, AEVec2 scale, f32 rot, Texture * tex);


// ----------------------------------------------------------------------------
// GAMESTATE FUNCTIONS
void DemoGameState::Initialize()
{
	//TestGameObjects();
	//TestGetComp();
	TestLogicSystem();
}
void DemoGameState::LoadResources()
{
	// Images
	aexGraphics->LoadTexture(".\\data\\Images\\Default.png");

	// Models
	aexGraphics->LoadModel(".\\data\\Models\\Quad.model");
}
void DemoGameState::Update()
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
void DemoGameState::Render()
{
	auto mainWin = aexWindowMgr->GetMainWindow();
	auto winWH = AEVec2(f32(mainWin->GetWidth()), f32(mainWin->GetHeight()));

	aexGraphics->SetViewport(0, 0, s32(winWH.x), s32(winWH.y));
	aexGraphics->SetClearColor(Color(0.7f,0.7f,0.7f,1.0f));
	aexGraphics->ClearFrameBuffer();

	auto mp = aexInput->GetMousePos();
	aexGraphics->DrawCircle(mp.x, mp.y, 50, Color(0,0,0,1));

	static f32 angle = 0.0f;
	angle += 35.0f * (f32)aexTime->GetFrameTime();
	DrawSprite(AEVec3(0, 0, 0), AEVec2(100, 100), DegToRad(angle), aexGraphics->GetTexture("Default.png"));

	aexGraphics->Present();
}

// ----------------------------------------------------------------------------
// HELPER FUNCTIONS
void DrawSprite(AEVec3 pos, AEVec2 scale, f32 rot, Texture * tex)
{
	// sanity check -> only draw if we have a quad object created
	if (auto quad = aexGraphics->GetModel("Quad.model"))
	{
		auto mainWin = aexWindowMgr->GetMainWindow();
		auto winWH = AEVec2(f32(mainWin->GetWidth()), f32(mainWin->GetHeight()));

		// compute the model to world matrix
		AEMtx44 model = AEMtx44::Translate(pos.x, pos.y, pos.z) * AEMtx44::RotateXYZ(0, 0, rot) * AEMtx44::Scale(scale.x, scale.y, 1.0f);

		// compute the world to cam matrix (camera is centered at (0,0, 20))
		AEMtx44 cam = AEMtx44::Translate(0, 0, -20);

		// compute the cam to screen matrix
		AEMtx44 proj = AEMtx44::OrthoProjGL(winWH.x, winWH.y, 0.01f, 1000.0f);

		// upload to shader
		auto shader = aexGraphics->GetShaderProgram("TextureMap.shader");
		if (shader)
		{
			shader->Bind();
			shader->SetShaderUniform("mtxViewProj", &(proj * cam));
			shader->SetShaderUniform("mtxModel", &model);
		}

		// set texture if it exists and set it in the shader
		if (tex) {
			tex->Bind();
			int texUnit = 0;
			shader->SetShaderUniform("ts_diffuse", &texUnit);
		}

		// draw the quad
		quad->Draw();

		// unbind everything
		if (shader)shader->Unbind();
		if (tex)tex->Unbind();
		
	}
}
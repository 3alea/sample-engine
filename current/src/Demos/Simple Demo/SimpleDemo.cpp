#include "src\Engine\AEX.h"
#include "SimpleDemo.h"
#include <iostream>

using namespace AEX;

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



void TestGameObjects()
{
	GameObject go;
	go.AddComp(new DummyComp());
	go.AddComp(new TransformComp());
	go.Initialize();
	auto comps = go.GetComps();
	for (u32 i = 0; i < go.GetCompCount(); ++i)
		comps[i]->Update();
	go.SetEnabled(false);
	for (u32 i = 0; i < go.GetCompCount(); ++i)
		comps[i]->Update();
	go.SetEnabled(true);
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
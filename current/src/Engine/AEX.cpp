#include "AEX.h"
namespace AEX{
	AEXEngine::AEXEngine(){}
	AEXEngine::~AEXEngine()
	{
		Graphics::ReleaseInstance();
		FRC::ReleaseInstance();
		Input::ReleaseInstance();
		WindowManager::ReleaseInstance();
	}
	bool AEXEngine::Initialize()
	{
		// note, here'were creating and initializing at the same
		// time. by typing the maccro, we're creating the singleton
		// pointer which is returned, we then call initialize on it.
		if (!aexWindowMgr->Initialize())return false;
		if (!aexInput->Initialize())return false;
		if (!aexTime->Initialize())return false;
		if (!aexGraphics->Initialize())return false;

		// Frame rate controller options.
		aexTime->LockFrameRate(true);
		aexTime->SetMaxFrameRate(60.0);
		aexGraphics->SetVSyncEnabled(true);

		// all good -> return true
		return true;
	}
	void AEXEngine::Run(IGameState *gameState)
	{
		// Sanity check
		if (!gameState)
			return;

		// load game state resources
		gameState->LoadResources();

		// initialize game state
		gameState->Initialize();

		// Reset timer
		aexTime->Reset();

		// run the game loop
		while (aexInput->KeyTriggered(VK_ESCAPE) == false &&
			aexWindowMgr->GetMainWindow()->Exists())
		{
			aexTime->StartFrame();
			aexWindowMgr->Update();		// Process OS messages and respond to window events.
			aexInput->Update();			// Process Input specific messages. 
			// 
			// TODO: add physics, collisions, interpolations, etc...
			// 
			gameState->Update();
			gameState->Render(); 
			aexTime->EndFrame();

			// poor man's reset
			if (Input::Instance()->KeyPressed('R'))
			{
				gameState->Shutdown();
				gameState->Initialize();
			}
		}

		gameState->Shutdown();

		// unload resources
		gameState->FreeResources();

		// delete gameState
		delete gameState;
	}
}
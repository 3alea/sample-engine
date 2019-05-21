#pragma once
#include "src\Engine\Logic\AEXGameState.h"
// demo game state
class JsonDemo : public AEX::IGameState
{
	virtual void Initialize();
	virtual void LoadResources();
	virtual void Update();
	virtual void Render();
};
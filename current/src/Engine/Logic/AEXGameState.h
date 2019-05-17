#pragma once
#include "..\Core\AEXBase.h"

namespace AEX
{
	// Game State
	class IGameState : public IBase
	{
		AEX_RTTI_DECL(IGameState, IBase);

	public:
		// State inherited
		virtual void Update(){}
		virtual void Initialize(){}
		virtual void Shutdown(){}
		virtual ~IGameState(){}

		// Helper functions
		virtual void LoadResources(){}
		virtual void FreeResources(){}

		// Custom Render
		virtual void Render(){}
	};
}

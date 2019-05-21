#ifndef AEX_H_
#define AEX_H_

#include "Debug\MyDebug.h"
#include "Core\AEXCore.h"
#include "Platform\AEXPlatform.h"
#include "Composition\AEXComposition.h"
#include "Scene\AEXTransformComp.h"
#include "Logic\AEXGameState.h"
#include "Logic\AEXLogic.h"
#include "Graphics\AEXGraphics.h"
#include "Utilities\AEXUtils.h"


// Easy access to singleton
#define aexEngine (AEX::AEXEngine::Instance())
#pragma warning (disable:4251) // dll and STL
namespace AEX{
	class AEXEngine : public ISystem
	{
		AEX_RTTI_DECL(AEXEngine, ISystem);
		AEX_SINGLETON(AEXEngine);
	public:
		virtual ~AEXEngine();
		virtual bool Initialize();
		void Run(IGameState*gameState = nullptr);
	};
}
#pragma warning (default:4251) // dll and STL

#endif
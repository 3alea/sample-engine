#pragma once
#include "..\Core\AEXCore.h"
#include "..\Composition\AEXComponent.h"

namespace AEX
{
	class LogicComp : public IComp
	{
		AEX_RTTI_DECL(LogicComp, IComp);
	public:
		LogicComp();
		void Initialize();
		void Shutdown();
	};

	class Logic :public ISystem
	{
		AEX_RTTI_DECL(Logic, ISystem);
		AEX_SINGLETON(Logic);

	public:
		virtual void Update();

		// component management
		void AddComp(IComp * logicComp);
		void RemoveComp(IComp * logicComp);
		void ClearComps();
	private:
		std::list<IComp *> mComps;
	};
}
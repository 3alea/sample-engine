#include "AEXLogic.h"

namespace AEX
{
	//-------------------------------------------------------------------------
	#pragma region // Base Logic Component

	LogicComp::LogicComp() : IComp() {}
	void LogicComp::Initialize() {
		Logic::Instance()->AddComp(this);
	}
	void LogicComp::Shutdown() {
		Logic::Instance()->RemoveComp(this);
	}
	#pragma endregion

	//-------------------------------------------------------------------------
	#pragma region // Logic System
	Logic::Logic() {}

	void Logic::Update()
	{
		FOR_EACH(comp, mComps)
			/*TODO:if((*comp)->Enabled())*/
			(*comp)->Update();
	}

	// component management
	void Logic::AddComp(IComp * logicComp) {
		mComps.remove(logicComp); // no duplicates
		mComps.push_back(logicComp);
	}
	void Logic::RemoveComp(IComp * logicComp) {
		mComps.remove(logicComp);
	}
	void Logic::ClearComps() {
		mComps.clear();
	}

	#pragma endregion
}
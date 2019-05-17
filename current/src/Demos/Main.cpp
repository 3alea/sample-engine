#include "src\Engine\AEX.h"
#include <iostream>
#include "Simple Demo\SimpleDemo.h"

void main(void)
{
	aexEngine->Initialize();
	aexEngine->Run(new DemoGameState);
	AEX::AEXEngine::ReleaseInstance();
}

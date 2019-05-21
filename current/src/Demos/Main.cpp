#include "src\Engine\AEX.h"
#include <iostream>
#include "Simple Demo\SimpleDemo.h"
#include "JsonDemo\JsonDemo.h"

void main(void)
{
	aexEngine->Initialize();
	aexEngine->Run(new JsonDemo);
	AEX::AEXEngine::ReleaseInstance();
}

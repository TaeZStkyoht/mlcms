#include "MockCore.hpp"

using namespace std;

using namespace MockCore;

Core Core::Create()
{
	if (callbackCreate)
		callbackCreate();

	Core core;
	core._gprcClient = make_unique<GrpcClient>(string());
	return core;
}

void Core::Start() const
{
	if (callbackStart)
		callbackStart();
}

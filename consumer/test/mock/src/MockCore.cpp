#include "MockCore.hpp"

using namespace std;

using namespace MockCore;

Core Core::Create(span<const char* const> arguments)
{
	if (callbackCreate)
		callbackCreate(arguments);

	Core core;
	core._grpcServer = make_unique<GrpcServer>(string());
	return core;
}

void Core::Start() const
{
	if (callbackStart)
		callbackStart();
}

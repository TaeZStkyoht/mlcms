#include "Core.hpp"

#include "middleware/GrpcPorts.hpp"

#include <format>

using namespace std;

Core Core::Create()
{
	Core core;

	core._gprcClient = make_unique<GrpcClient>(format("127.0.0.1:{}", GrpcPorts::providerToCommunicationLayer));

	return core;
}

void Core::Start() const
{
	_gprcClient->Start();
}

#include "Core.hpp"

#include "middleware/GrpcPorts.hpp"

#include <format>

using namespace std;

Core Core::Create()
{
	Core core;

	core._grpcServer = make_unique<GrpcServer>(format("127.0.0.1:{}", GrpcPorts::communicationLayerToConsumer));

	return core;
}

void Core::Start() const
{
	_grpcServer->Start();
}

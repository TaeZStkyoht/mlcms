#include "Core.hpp"

#include "middleware/GrpcPorts.hpp"

#include <atomic>
#include <format>

using namespace std;

extern atomic_bool run;

Core Core::Create(span<const char* const> arguments)
{
	Core core;

	core._grpcServer =
		make_unique<GrpcServer>(format("127.0.0.1:{}", arguments.size() > 1 ? arguments[1] : to_string(GrpcPorts::communicationLayerToConsumer)));

	return core;
}

void Core::Start() const
{
	if (!_grpcServer->Start())
		run = false;
}

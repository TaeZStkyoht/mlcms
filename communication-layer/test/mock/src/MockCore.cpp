#include "MockCore.hpp"

using namespace std;

using namespace MockCore;

Core Core::Create(span<const char* const> arguments)
{
	if (callbackCreate)
		callbackCreate(arguments);

	Core core;
	core._grpcServer = make_unique<GrpcServer>(string(), shared_ptr<MessageRequestPusher>());
	core._messageRequestSender = make_unique<MessageRequestSender>(shared_ptr<MessageRequestPuller>(), vector<shared_ptr<GrpcClient>>());
	core._observer = make_unique<Observer>(shared_ptr<MessageRequestObserver>(), vector<shared_ptr<AvailabilityProvider>>());
	return core;
}

void Core::Start() const
{
	if (callbackStart)
		callbackStart();
}

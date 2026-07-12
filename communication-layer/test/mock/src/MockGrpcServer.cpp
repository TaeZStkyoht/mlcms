#include "MockGrpcServer.hpp"

#include "GrpcServer.hpp"

using namespace std;

using namespace MockGrpcServer;

struct GrpcServer::Impl final {};

GrpcServer::GrpcServer(string, shared_ptr<MessageRequestPusher>)
{
}

GrpcServer::~GrpcServer() = default;

bool GrpcServer::Start() const
{
	if (callbackStart)
		return callbackStart();
	return {};
}

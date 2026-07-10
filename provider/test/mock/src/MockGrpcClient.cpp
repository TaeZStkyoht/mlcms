#include "MockGrpcClient.hpp"

#include "GrpcClient.hpp"

using namespace std;

using namespace MockGrpcClient;

struct GrpcClient::Impl final {};

GrpcClient::GrpcClient(string)
{
}

GrpcClient::~GrpcClient() = default;

void GrpcClient::Start()
{
	if (callbackStart)
		callbackStart();
}

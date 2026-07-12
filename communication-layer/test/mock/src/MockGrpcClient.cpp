#include "MockGrpcClient.hpp"

#include "GrpcClient.hpp"

using namespace std;
using namespace chrono;

using namespace MockGrpcClient;

struct GrpcClient::Impl final {};

GrpcClient::GrpcClient(string)
{
}

GrpcClient::~GrpcClient() = default;

bool GrpcClient::IsAvailable() const
{
	if (callbackIsAvailable)
		return callbackIsAvailable();
	return {};
}

uint32_t GrpcClient::AverageCommunicationDuration() const
{
	if (callbackAverageCommunicationDuration)
		return callbackAverageCommunicationDuration();
	return {};
}

bool GrpcClient::SendMessage(const string& message, system_clock::time_point timestamp) const
{
	if (callbackSendMessage)
		return callbackSendMessage(message, timestamp);
	return {};
}

steady_clock::time_point GrpcClient::LastTriedTime() const
{
	if (callbackLastTriedTime)
		return callbackLastTriedTime();
	return {};
}

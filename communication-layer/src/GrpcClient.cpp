#include "GrpcClient.hpp"

#include "middleware/BaseGrpcClient.hpp"

#include "consumer.grpc.pb.h"

using namespace std;
using namespace chrono;

using namespace google::protobuf;

using namespace middleware;

class GrpcClient::Impl final : public BaseGrpcClient<cl::Consumer> {
public:
	using BaseGrpcClient::BaseGrpcClient;

	void SendMessage(string message, system_clock::time_point timestamp)
	{
		ResponseWithClientContext<Empty> clientContextWithReponse;
		_stub->SendMessage(
			&clientContextWithReponse.cc,
			[&message, timestamp] {
				cl::OuterMessageRequest outerMessageRequest;
				outerMessageRequest.set_payload(move(message));
				AssignTimePointToProtoTimestamp(outerMessageRequest.mutable_timestamp(), timestamp);
				return outerMessageRequest;
			}(),
			&clientContextWithReponse.response);
	}

private:
	static void AssignTimePointToProtoTimestamp(Timestamp* timestamp, system_clock::time_point timePoint)
	{
		const auto timePointInSeconds = time_point_cast<seconds>(timePoint);
		timestamp->set_seconds(timePointInSeconds.time_since_epoch().count());
		timestamp->set_nanos(static_cast<int32_t>(duration_cast<nanoseconds>(timePoint - timePointInSeconds).count()));
	}
};

GrpcClient::GrpcClient(string uri) : _impl(make_unique<Impl>(move(uri)))
{
}

GrpcClient::~GrpcClient() = default;

void GrpcClient::SendMessage(string message, system_clock::time_point timestamp) const
{
	_impl->SendMessage(move(message), timestamp);
}

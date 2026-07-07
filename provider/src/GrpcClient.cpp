#include "GrpcClient.hpp"

#include "middleware/BaseGrpcClient.hpp"

#include "communication-layer.grpc.pb.h"

#include <thread>

using namespace std;
using namespace chrono;

using namespace google::protobuf;

using namespace middleware;

extern const atomic_bool run;

class GrpcClient::Impl final : public BaseGrpcClient<cl::CommunicationLayer> {
public:
	using BaseGrpcClient::BaseGrpcClient;

	void Start()
	{
		_worker = jthread([this] { Work(); });
	}

private:
	void Work() const
	{
		for (; run; this_thread::yield()) {
			ResponseWithClientContext<Empty> clientContextWithReponse;
			const auto clientReadWriter = _stub->SendMessage(&clientContextWithReponse.cc, &clientContextWithReponse.response);
			if (!clientReadWriter)
				continue;

			for (; !_worker.get_stop_token().stop_requested(); this_thread::yield()) {
				cl::MessageRequest messageRequest;
				messageRequest.set_priority(
					static_cast<cl::MessageRequest_Priority>((rand() % cl::MessageRequest_Priority::MessageRequest_Priority_CRITICAL) + 1));
				messageRequest.set_sender_id(getpid());
				AssignTimePointToProtoTimestamp(messageRequest.mutable_timestamp(), system_clock::now());
				messageRequest.set_payload(string(static_cast<uint8_t>(rand() % UINT8_MAX) + 1, static_cast<uint8_t>(rand() % UINT8_MAX)));
				if (!clientReadWriter->Write(messageRequest))
					break;
			}
		}
	}

	static void AssignTimePointToProtoTimestamp(Timestamp* timestamp, system_clock::time_point timePoint)
	{
		const auto timePointInSeconds = time_point_cast<seconds>(timePoint);
		timestamp->set_seconds(timePointInSeconds.time_since_epoch().count());
		timestamp->set_nanos(static_cast<int32_t>(duration_cast<nanoseconds>(timePoint - timePointInSeconds).count()));
	}

	jthread _worker;
};

GrpcClient::GrpcClient(std::string uri) : _impl(make_unique<Impl>(move(uri)))
{
}

GrpcClient::~GrpcClient() = default;

void GrpcClient::Start()
{
	_impl->Start();
}

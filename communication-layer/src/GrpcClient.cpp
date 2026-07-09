#include "GrpcClient.hpp"

#include "middleware/BaseGrpcClient.hpp"

#include "consumer.grpc.pb.h"

#include <thread>

using namespace std;
using namespace chrono;

using namespace google::protobuf;

using namespace middleware;

class GrpcClient::Impl final : public BaseGrpcClient<cl::Consumer> {
public:
	using BaseGrpcClient::BaseGrpcClient;

	void Start()
	{
		_worker = jthread([this] { Work(); });
	}

	void SendMessage(entity::OuterMessageRequest outerMessageRequest)
	{
		const lock_guard lg(_mtxMessages);
		_messages.push(move(outerMessageRequest));
	}

	steady_clock::time_point LastTriedTime() const noexcept
	{
		return _lastTriedTime;
	}

	constexpr queue<entity::OuterMessageRequest>&& StealQueue()
	{
		const lock_guard lg(_mtxMessages);
		return move(_messages);
	}

	bool IsAvailable() const noexcept
	{
		return _connected;
	}

	uint32_t AverageCommunicationDuration() const noexcept
	{
		return static_cast<uint32_t>(_communicationDurationMovingAverage);
	}

private:
	void Work()
	{
		for (uint8_t _consecutiveErrorCount{}; !_worker.get_stop_token().stop_requested();) {
			if (!_connected) {
				this_thread::sleep_for(1s);
				continue;
			}

			unique_lock locker(_mtxMessages);
			if (_messages.empty()) {
				this_thread::yield();
				continue;
			}

			auto outerMessageRequest = move(_messages.front());
			_messages.pop();
			locker.unlock();

			if (ResponseWithClientContext<Empty> clientContextWithReponse;
				_stub
					->SendMessage(&clientContextWithReponse.cc, ConvertEntityOuterMessageRequestToProto(outerMessageRequest),
								  &clientContextWithReponse.response)
					.ok()) {
				const auto sendTime = steady_clock::now();
				const auto duration = static_cast<float>(duration_cast<microseconds>(sendTime - outerMessageRequest.queuedTimestamp).count());
				_communicationDurationMovingAverage.store(_movingAverageAlpha * duration +
														  (1 - _movingAverageAlpha) * _communicationDurationMovingAverage);
				_consecutiveErrorCount = {};
			}
			else if (++_consecutiveErrorCount >= _maxConsecutiveErrorCount) {
				{
					const lock_guard lg(_mtxMessages);
					_messages.push(move(outerMessageRequest));
				}
				Stop();
				_consecutiveErrorCount = {};
			}

			_lastTriedTime.store(steady_clock::now());
		}
	}

	static cl::OuterMessageRequest ConvertEntityOuterMessageRequestToProto(const entity::OuterMessageRequest& outerMessageRequest)
	{
		cl::OuterMessageRequest outerMessageRequestProto;
		outerMessageRequestProto.set_payload(outerMessageRequest.payload);
		AssignTimePointToProtoTimestamp(outerMessageRequestProto.mutable_timestamp(), outerMessageRequest.timestamp);
		AssignTimePointToProtoTimestamp(outerMessageRequestProto.mutable_queued_timestamp(), outerMessageRequest.queuedTimestamp);
		return outerMessageRequestProto;
	}

	static void AssignTimePointToProtoTimestamp(Timestamp* timestamp, auto timePoint)
	{
		const auto timePointInSeconds = time_point_cast<seconds>(timePoint);
		timestamp->set_seconds(timePointInSeconds.time_since_epoch().count());
		timestamp->set_nanos(static_cast<int32_t>(duration_cast<nanoseconds>(timePoint - timePointInSeconds).count()));
	}

	static constexpr uint8_t _maxConsecutiveErrorCount = 5;
	static constexpr auto _movingAverageAlpha = 0.05f;

	atomic<float> _communicationDurationMovingAverage{};
	atomic<steady_clock::time_point> _lastTriedTime{};

	queue<entity::OuterMessageRequest> _messages;
	mutex _mtxMessages;

	jthread _worker;
};

GrpcClient::GrpcClient(string uri) : _impl(make_unique<Impl>(move(uri)))
{
}

GrpcClient::~GrpcClient() = default;

void GrpcClient::Start()
{
	_impl->Start();
}

void GrpcClient::SendMessage(entity::OuterMessageRequest outerMessageRequest) const
{
	_impl->SendMessage(move(outerMessageRequest));
}

steady_clock::time_point GrpcClient::LastTriedTime() const
{
	return _impl->LastTriedTime();
}

queue<entity::OuterMessageRequest>&& GrpcClient::StealQueue()
{
	return _impl->StealQueue();
}

bool GrpcClient::IsAvailable() const
{
	return _impl->IsAvailable();
}

uint32_t GrpcClient::AverageCommunicationDuration() const
{
	return _impl->AverageCommunicationDuration();
}

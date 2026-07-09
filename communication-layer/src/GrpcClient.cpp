#include "GrpcClient.hpp"

#include "middleware/BaseGrpcClient.hpp"

#include "consumer.grpc.pb.h"

#include <numeric>

using namespace std;
using namespace chrono;

using namespace google::protobuf;

using namespace middleware;

class GrpcClient::Impl final : public BaseGrpcClient<cl::Consumer> {
public:
	using BaseGrpcClient::BaseGrpcClient;

	bool IsAvailable() const
	{
		return _isAvailable;
	}

	uint32_t AverageCommunicationDuration() const
	{
		return static_cast<uint32_t>(_communicationDurationMovingAverage);
	}

	bool SendMessage(const string& message, system_clock::time_point timestamp)
	{
		ResponseWithClientContext<Empty> clientContextWithReponse;
		const auto startTime = steady_clock::now();
		auto result = _stub
						  ->SendMessage(
							  &clientContextWithReponse.cc,
							  [&message, timestamp] {
								  cl::OuterMessageRequest outerMessageRequest;
								  outerMessageRequest.set_payload(message);
								  AssignTimePointToProtoTimestamp(outerMessageRequest.mutable_timestamp(), timestamp);
								  return outerMessageRequest;
							  }(),
							  &clientContextWithReponse.response)
						  .ok();
		{
			const auto stopTime = steady_clock::now();
			const auto duration = static_cast<float>(duration_cast<microseconds>(stopTime - startTime).count());
			_communicationDurationMovingAverage.store(_movingAverageAlpha * duration + (1 - _movingAverageAlpha) * _communicationDurationMovingAverage);
		}

		if (_isAvailable) {
			if (!result && ++_consecutiveErrorCount >= _maxConsecutiveErrorCount)
				_isAvailable = false;
		}
		else if (result)
			_isAvailable = true;

		_lastTriedTime.store(steady_clock::now());
		return result;
	}

	steady_clock::time_point LastTriedTime() const noexcept
	{
		return _lastTriedTime;
	}

private:
	static void AssignTimePointToProtoTimestamp(Timestamp* timestamp, system_clock::time_point timePoint)
	{
		const auto timePointInSeconds = time_point_cast<seconds>(timePoint);
		timestamp->set_seconds(timePointInSeconds.time_since_epoch().count());
		timestamp->set_nanos(static_cast<int32_t>(duration_cast<nanoseconds>(timePoint - timePointInSeconds).count()));
	}

	static constexpr uint8_t _maxConsecutiveErrorCount = 10;
	static constexpr auto _movingAverageAlpha = 0.05f;

	atomic<float> _communicationDurationMovingAverage{};

	atomic_uint8_t _consecutiveErrorCount{};
	atomic_bool _isAvailable = true;
	atomic<steady_clock::time_point> _lastTriedTime{};
};

GrpcClient::GrpcClient(string uri) : _impl(make_unique<Impl>(move(uri)))
{
}

GrpcClient::~GrpcClient() = default;

bool GrpcClient::IsAvailable() const
{
	return _impl->IsAvailable();
}

uint32_t GrpcClient::AverageCommunicationDuration() const
{
	return _impl->AverageCommunicationDuration();
}

bool GrpcClient::SendMessage(const string& message, system_clock::time_point timestamp) const
{
	return _impl->SendMessage(message, timestamp);
}

steady_clock::time_point GrpcClient::LastTriedTime() const
{
	return _impl->LastTriedTime();
}

void GrpcClient::Reconnect()
{
	_impl->Reconnect();
}

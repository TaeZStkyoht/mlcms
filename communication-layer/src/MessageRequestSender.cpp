#include "MessageRequestSender.hpp"

#include <algorithm>

using namespace std;
using namespace chrono;

extern const atomic_bool run;

void MessageRequestSender::Start()
{
	_worker = jthread([this] { Work(); });
}

void MessageRequestSender::Work()
{
	if (_grpcClients.empty())
		return;

	_ringBufferOfSenderThreads.resize(_grpcClients.size());

	while (run) {
		if (unique_lock locker(_mtxMessagesWillBeRetried); !_messagesWillBeRetried.empty()) {
			auto messageRequest = move(_messagesWillBeRetried.front());
			_messagesWillBeRetried.pop();
			locker.unlock();
			SendMessage(move(messageRequest));
			continue;
		}

		if (auto messageRequest = _messageRequestPuller->Pull(); messageRequest) {
			SendMessage(move(*messageRequest));
			continue;
		}

		this_thread::yield();
	}
}

void MessageRequestSender::SendMessage(entity::MessageRequest messageRequest)
{
	const lock_guard lgThreadIndexSelector(_mtxThreadIndexSelector);
	_ringBufferOfSenderThreads[_threadIndex] = jthread([this, messageRequestPriv = move(messageRequest)] { SendMessageWork(move(messageRequestPriv)); });
	RingBufferNextIndex(_threadIndex, _ringBufferOfSenderThreads.size());
}

void MessageRequestSender::SendMessageWork(entity::MessageRequest messageRequest)
{
	const auto usableGrpcClientIndex = GetNextUsableGrpcClientIndex();
	const auto sendResult = _grpcClients[usableGrpcClientIndex]->SendMessage(messageRequest.payload, messageRequest.timestamp);
	_grpcClientsInUse[usableGrpcClientIndex].store(false);
	if (!sendResult) {
		const lock_guard lg(_mtxMessagesWillBeRetried);
		_messagesWillBeRetried.push(move(messageRequest));
	}
}

uint8_t MessageRequestSender::GetNextUsableGrpcClientIndex()
{
	const lock_guard lg(_mtxGrpcIndexSelector);

	while (run) {
		if (const auto candidateIndex = TryGetFromAvailables(); candidateIndex) {
			_lastUsedGrpcClientIndex = *candidateIndex;
			break;
		}

		if (const auto candidateIndex = TryGetFromNonAvailables(); candidateIndex) {
			_lastUsedGrpcClientIndex = *candidateIndex;
			break;
		}

		this_thread::yield();
	}

	_grpcClientsInUse[_lastUsedGrpcClientIndex].store(true);
	_lastUsedGrpcClients[RingBufferNextIndex(_lastUsedGrpcClientsEmplacerIndex, _lastUsedGrpcClients.size())] = _lastUsedGrpcClientIndex;
	return _lastUsedGrpcClientIndex;
}

optional<uint8_t> MessageRequestSender::TryGetFromAvailables() const
{
	// Check available clients' sending speed as frequency
	float totalCommunicationFrequencyOfAvailableGrpcClients{};
	vector<pair<float, uint8_t>> mapRateOfCommunicationFrequencyOfAvailableGrpcClients;
	for (uint8_t i = 0; i < _grpcClients.size(); ++i) {
		if (_grpcClients[i]->IsAvailable()) {
			const float frequency = 1 / static_cast<float>(_grpcClients[i]->AverageCommunicationDuration());
			totalCommunicationFrequencyOfAvailableGrpcClients += frequency;
			mapRateOfCommunicationFrequencyOfAvailableGrpcClients.emplace_back(frequency, i);
		}
	}

	multimap<float, uint8_t> utilizationRate;
	// Select if client's relative sending frequency rate is greater than recent occurance rate and clients currently not in used.
	for (auto [frequency, grpcClientIndex] : mapRateOfCommunicationFrequencyOfAvailableGrpcClients) {
		if (!_grpcClientsInUse.at(grpcClientIndex).load()) {
			const auto recentUsageRate = FrequencyOfGrpcClientIndexOnLastUsages(grpcClientIndex);
			const auto relativeRateBasedOnCommunicationSpeed = frequency / totalCommunicationFrequencyOfAvailableGrpcClients;
			utilizationRate.emplace(recentUsageRate / relativeRateBasedOnCommunicationSpeed, grpcClientIndex);
		}
	}

	// Basically, try to select the one that can be utilized mostly.
	if (!utilizationRate.empty())
		return utilizationRate.begin()->second;

	return {};
}

optional<uint8_t> MessageRequestSender::TryGetFromNonAvailables() const
{
	uint8_t i = _lastUsedGrpcClientIndex;
	RingBufferNextIndex(i, _grpcClients.size());
	for (; i < _grpcClients.size() + _lastUsedGrpcClientIndex; ++i)
		if (const auto realIndex = static_cast<uint8_t>(i % _grpcClients.size());
			!_grpcClients[realIndex]->IsAvailable() && steady_clock::now() - _grpcClients[realIndex]->LastTriedTime() > _retryTimeForNonAvailable)
			return realIndex;
	return {};
}

float MessageRequestSender::FrequencyOfGrpcClientIndexOnLastUsages(uint8_t grpcClientIndex) const
{
	return static_cast<float>(ranges::count_if(
			   _lastUsedGrpcClients, [grpcClientIndex](uint8_t usedGrpcClientIndex) noexcept { return usedGrpcClientIndex == grpcClientIndex; })) /
		   static_cast<float>(_lastUsedGrpcClients.size());
}

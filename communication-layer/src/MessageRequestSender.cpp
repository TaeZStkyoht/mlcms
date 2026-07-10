#include "MessageRequestSender.hpp"

#include <queue>

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
	_threadIndex = static_cast<uint8_t>((_threadIndex + 1) % _ringBufferOfSenderThreads.size());
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
		_lastUsedGrpcClientIndex = static_cast<uint8_t>((_lastUsedGrpcClientIndex + 1) % _grpcClients.size());

		if (_grpcClientsInUse[_lastUsedGrpcClientIndex].load()) {
			this_thread::yield();
			continue;
		}

		if (_grpcClients[_lastUsedGrpcClientIndex]->IsAvailable())
			break;

		if (steady_clock::now() - _grpcClients[_lastUsedGrpcClientIndex]->LastTriedTime() > 10s) {
			_grpcClients[_lastUsedGrpcClientIndex]->Reconnect();
			break;
		}
	}

	_grpcClientsInUse[_lastUsedGrpcClientIndex].store(true);
	return _lastUsedGrpcClientIndex;
}

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
	const uint8_t usableGrpcClientIndex = GetNextUsableGrpcClientIndex();

	_ringBufferOfSenderThreads[_threadIndex] = jthread([this, usableGrpcClientIndex, messageRequestPriv = move(messageRequest)] {
		unique_lock locker(_mtxGrpcClients);
		const auto sendResult = _grpcClients[usableGrpcClientIndex]->SendMessage(messageRequestPriv.payload, messageRequestPriv.timestamp);
		locker.unlock();
		if (!sendResult) {
			const lock_guard lg(_mtxMessagesWillBeRetried);
			_messagesWillBeRetried.push(move(messageRequestPriv));
		}
	});

	_threadIndex = static_cast<uint8_t>((_threadIndex + 1) % _ringBufferOfSenderThreads.size());
}

uint8_t MessageRequestSender::GetNextUsableGrpcClientIndex()
{
	for (const lock_guard lg(_mtxGrpcClients); run;
		 _lastUsedGrpcClientIndex = static_cast<uint8_t>((_lastUsedGrpcClientIndex + 1) % _grpcClients.size())) {
		if (_grpcClients[_lastUsedGrpcClientIndex]->IsAvailable())
			break;

		if (steady_clock::now() - _grpcClients[_lastUsedGrpcClientIndex]->LastTriedTime() > 10s) {
			_grpcClients[_lastUsedGrpcClientIndex]->Reconnect();
			break;
		}
	}

	return _lastUsedGrpcClientIndex;
}

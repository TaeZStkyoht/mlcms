#include "MessageRequestSender.hpp"

#include <iostream>
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

	for (auto grpcClient : _grpcClients)
		grpcClient->Start();

	auto lastPrintTime = steady_clock::now();

	while (run) {
		if (!_messagesWillBeRetried.empty()) {
			auto messageRequest = move(_messagesWillBeRetried.front());
			_messagesWillBeRetried.pop();
			SendMessage(messageRequest.timestamp, move(messageRequest.payload));
		}

		if (auto messageRequest = _messageRequestPuller->Pull(); messageRequest)
			SendMessage(messageRequest->timestamp, move(messageRequest->payload));

		for (auto grpcClient : _grpcClients)
			if (!grpcClient->IsAvailable())
				for (auto&& stealedQueue = grpcClient->StealQueue(); !stealedQueue.empty(); stealedQueue.pop())
					_messagesWillBeRetried.push(move(stealedQueue.front()));

		if (steady_clock::now() - lastPrintTime > 1s) {
			cout << "_messagesWillBeRetried: " << _messagesWillBeRetried.size() << endl;
			lastPrintTime = steady_clock::now();
		}

		this_thread::yield();
	}
}

void MessageRequestSender::SendMessage(system_clock::time_point timestamp, string payload)
{
	_grpcClients[GetNextUsableGrpcClientIndex()]->SendMessage({
		.timestamp = timestamp,
		.queuedTimestamp = steady_clock::now(),
		.payload = move(payload),
	});
}

uint8_t MessageRequestSender::GetNextUsableGrpcClientIndex()
{
	for (; run; _lastUsedGrpcClientIndex = static_cast<uint8_t>((_lastUsedGrpcClientIndex + 1) % _grpcClients.size()))
		if (_grpcClients[_lastUsedGrpcClientIndex]->IsAvailable() || steady_clock::now() - _grpcClients[_lastUsedGrpcClientIndex]->LastTriedTime() > 10s)
			break;

	return _lastUsedGrpcClientIndex;
}

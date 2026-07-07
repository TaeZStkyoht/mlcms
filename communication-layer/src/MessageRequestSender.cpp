#include "MessageRequestSender.hpp"

using namespace std;

extern const atomic_bool run;

void MessageRequestSender::Start()
{
	_worker = jthread([this] { Work(); });
}

void MessageRequestSender::Work()
{
	while (run) {
		if (auto messageRequest = _messageRequestPuller->Pull(); messageRequest) {
			_grpcClient->SendMessage(move(messageRequest->payload), messageRequest->timestamp);
			continue;
		}

		this_thread::yield();
	}
}

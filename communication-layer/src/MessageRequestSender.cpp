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
		auto messageRequest = _messageRequestPuller->Pull();
		if (!messageRequest) {
			this_thread::yield();
			continue;
		}
	}
}

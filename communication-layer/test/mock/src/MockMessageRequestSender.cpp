#include "MockMessageRequestSender.hpp"

#include "MessageRequestSender.hpp"

using namespace std;

using namespace MockMessageRequestSender;

void MessageRequestSender::Start()
{
	if (callbackStart)
		callbackStart();
}

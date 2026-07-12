#include "MessageRequestHolder.hpp"

using namespace std;

using namespace entity;

QueueSize MessageRequestHolder::Size() const
{
	return {};
}

void MessageRequestHolder::Push(MessageRequest)
{
}

optional<entity::MessageRequest> MessageRequestHolder::Pull()
{
	return {};
}

#include "MessageRequestHolder.hpp"

using namespace std;

QueueSize MessageRequestHolder::Size() const
{
	return {};
}

void MessageRequestHolder::Push(entity::MessageRequest)
{
}

optional<entity::MessageRequest> MessageRequestHolder::Pull()
{
	return {};
}

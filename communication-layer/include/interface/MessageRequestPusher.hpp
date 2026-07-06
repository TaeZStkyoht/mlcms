#pragma once

#include "entity/MessageRequest.hpp"

class MessageRequestPusher {
public:
	virtual ~MessageRequestPusher() = default;

	virtual void Push(entity::MessageRequest messageRequest) = 0;
};

#pragma once

#include "entity/QueueSize.hpp"

class MessageRequestObserver {
public:
	virtual ~MessageRequestObserver() = default;

	virtual entity::QueueSize Size() const = 0;
};

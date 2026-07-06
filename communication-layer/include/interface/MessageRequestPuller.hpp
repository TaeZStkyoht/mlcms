#pragma once

#include "entity/MessageRequest.hpp"

#include <optional>

class MessageRequestPuller {
public:
	virtual ~MessageRequestPuller() = default;

	virtual std::optional<entity::MessageRequest> Pull() = 0;
};

#pragma once

#include "interface/MessageRequestPusher.hpp"

#include <gmock/gmock.h>

class MockMessageRequestPusher final : public MessageRequestPusher {
public:
	MOCK_METHOD(void, Push, (entity::MessageRequest), (override));
};

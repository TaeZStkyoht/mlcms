#pragma once

#include "interface/MessageRequestPuller.hpp"

#include <gmock/gmock.h>

class MockMessageRequestPuller final : public MessageRequestPuller {
public:
	MOCK_METHOD(std::optional<entity::MessageRequest>, Pull, (), (override));
};

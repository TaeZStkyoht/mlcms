#pragma once

#include "interface/MessageRequestObserver.hpp"

#include <gmock/gmock.h>

class MockMessageRequestObserver final : public MessageRequestObserver {
public:
	MOCK_METHOD(entity::QueueSize, Size, (), (const, override));
};

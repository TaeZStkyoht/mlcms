#pragma once

#include "interface/AvailabilityProvider.hpp"

#include <gmock/gmock.h>

class MockAvailabilityProvider final : public AvailabilityProvider {
public:
	MOCK_METHOD(bool, IsAvailable, (), (const, override));
	MOCK_METHOD(uint32_t, AverageCommunicationDuration, (), (const, override));
};

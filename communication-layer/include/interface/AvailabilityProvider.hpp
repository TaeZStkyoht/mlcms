#pragma once

#include <cstdint>

class AvailabilityProvider {
public:
	virtual ~AvailabilityProvider() = default;

	virtual bool IsAvailable() const = 0;
	virtual uint32_t AverageCommunicationDuration() const = 0;
};

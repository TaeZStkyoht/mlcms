#pragma once

#include "interface/AvailabilityProvider.hpp"

#include <chrono>
#include <memory>
#include <string>

class GrpcClient final : public AvailabilityProvider {
public:
	[[nodiscard]] explicit GrpcClient(std::string uri);
	~GrpcClient();

	bool IsAvailable() const override;
	uint32_t AverageCommunicationDuration() const override;

	bool SendMessage(const std::string& message, std::chrono::system_clock::time_point timestamp) const;

	std::chrono::steady_clock::time_point LastTriedTime() const;

private:
	class Impl;
	std::unique_ptr<Impl> _impl;
};

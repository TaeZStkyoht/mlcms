#pragma once

#include "interface/AvailabilityProvider.hpp"

#include "entity/OuterMessageRequest.hpp"

#include <chrono>
#include <memory>
#include <queue>
#include <string>

class GrpcClient final : public AvailabilityProvider {
public:
	[[nodiscard]] GrpcClient(std::string uri);
	~GrpcClient();

	void Start();

	void SendMessage(entity::OuterMessageRequest outerMessageRequest) const;
	std::chrono::steady_clock::time_point LastTriedTime() const;
	std::queue<entity::OuterMessageRequest>&& StealQueue();

	bool IsAvailable() const override;
	uint32_t AverageCommunicationDuration() const override;

private:
	class Impl;
	std::unique_ptr<Impl> _impl;
};

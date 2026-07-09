#pragma once

#include "interface/AvailabilityProvider.hpp"
#include "interface/MessageRequestObserver.hpp"

#include <memory>
#include <thread>
#include <vector>

class Observer final {
public:
	Observer(std::shared_ptr<MessageRequestObserver> messageRequestObserver,
			 std::vector<std::shared_ptr<AvailabilityProvider>> availabilityProvider) noexcept
		: _messageRequestObserver(messageRequestObserver), _availabilityProvider(std::move(availabilityProvider))
	{
	}

	void Start();

private:
	void Work();

	std::shared_ptr<MessageRequestObserver> _messageRequestObserver;
	std::vector<std::shared_ptr<AvailabilityProvider>> _availabilityProvider;

	std::jthread _worker;
};

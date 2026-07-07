#pragma once

#include "interface/MessageRequestObserver.hpp"

#include <memory>
#include <thread>

class Observer final {
public:
	explicit Observer(std::shared_ptr<MessageRequestObserver> messageRequestObserver) noexcept : _messageRequestObserver(messageRequestObserver)
	{
	}

	void Start();

private:
	void Work();

	std::shared_ptr<MessageRequestObserver> _messageRequestObserver;

	std::jthread _worker;
};

#pragma once

#include "interface/MessageRequestPuller.hpp"

#include <memory>
#include <thread>

class MessageRequestSender final {
public:
	explicit MessageRequestSender(std::shared_ptr<MessageRequestPuller> messageRequestPuller) noexcept : _messageRequestPuller(messageRequestPuller)
	{
	}

	void Start();

private:
	void Work();

	std::shared_ptr<MessageRequestPuller> _messageRequestPuller;

	std::jthread _worker;
};

#pragma once

#include "interface/MessageRequestPuller.hpp"

#include "GrpcClient.hpp"

#include <memory>
#include <thread>

class MessageRequestSender final {
public:
	MessageRequestSender(std::shared_ptr<MessageRequestPuller> messageRequestPuller, std::shared_ptr<GrpcClient> grpcClient) noexcept
		: _messageRequestPuller(messageRequestPuller), _grpcClient(grpcClient)
	{
	}

	void Start();

private:
	void Work();

	std::shared_ptr<MessageRequestPuller> _messageRequestPuller;
	std::shared_ptr<GrpcClient> _grpcClient;

	std::jthread _worker;
};

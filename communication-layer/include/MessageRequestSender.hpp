#pragma once

#include "interface/MessageRequestPuller.hpp"

#include "GrpcClient.hpp"

#include <map>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>

class MessageRequestSender final {
public:
	MessageRequestSender(std::shared_ptr<MessageRequestPuller> messageRequestPuller, std::vector<std::shared_ptr<GrpcClient>> grpcClients) noexcept
		: _messageRequestPuller(messageRequestPuller), _grpcClients(move(grpcClients))
	{
	}

	void Start();

private:
	void Work();

	void SendMessage(std::chrono::system_clock::time_point timestamp, std::string payload);
	uint8_t GetNextUsableGrpcClientIndex();

	std::shared_ptr<MessageRequestPuller> _messageRequestPuller;
	std::vector<std::shared_ptr<GrpcClient>> _grpcClients;

	std::queue<entity::OuterMessageRequest> _messagesWillBeRetried;
	uint8_t _lastUsedGrpcClientIndex{};

	std::jthread _worker;
};

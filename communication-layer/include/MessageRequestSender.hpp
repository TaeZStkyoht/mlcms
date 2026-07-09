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

	void SendMessage(entity::MessageRequest messageRequest);
	uint8_t GetNextUsableGrpcClientIndex();

	std::shared_ptr<MessageRequestPuller> _messageRequestPuller;

	std::vector<std::shared_ptr<GrpcClient>> _grpcClients;
	std::mutex _mtxGrpcClients;

	std::queue<entity::MessageRequest> _messagesWillBeRetried;
	std::mutex _mtxMessagesWillBeRetried;

	uint8_t _threadIndex{};
	uint8_t _lastUsedGrpcClientIndex{};

	std::vector<std::jthread> _ringBufferOfSenderThreads;
	std::jthread _worker;
};

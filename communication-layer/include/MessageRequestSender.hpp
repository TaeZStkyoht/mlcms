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
	void SendMessageWork(entity::MessageRequest messageRequest);
	uint8_t GetNextUsableGrpcClientIndex();

	std::shared_ptr<MessageRequestPuller> _messageRequestPuller;
	std::vector<std::shared_ptr<GrpcClient>> _grpcClients;

	std::map<uint8_t, std::atomic_bool> _grpcClientsInUse;
	std::queue<entity::MessageRequest> _messagesWillBeRetried;

	uint8_t _lastUsedGrpcClientIndex{};
	uint8_t _threadIndex{};

	std::mutex _mtxMessagesWillBeRetried;
	std::mutex _mtxGrpcIndexSelector;
	std::mutex _mtxThreadIndexSelector;

	std::vector<std::jthread> _ringBufferOfSenderThreads;
	std::jthread _worker;
};

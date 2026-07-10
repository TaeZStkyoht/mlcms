#pragma once

#include "interface/MessageRequestPuller.hpp"

#include "GrpcClient.hpp"

#include <map>
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
	std::optional<uint8_t> TryGetFromAvailables() const;
	std::optional<uint8_t> TryGetFromNonAvailables() const;
	float FrequencyOfGrpcClientIndexOnLastUsages(uint8_t grpcClientIndex) const;

	template <typename T>
	static constexpr T RingBufferNextIndex(T& currentIndex, auto exclusiveMaximum) noexcept
	{
		return currentIndex = static_cast<T>((currentIndex + 1) % exclusiveMaximum);
	}

	std::shared_ptr<MessageRequestPuller> _messageRequestPuller;
	std::vector<std::shared_ptr<GrpcClient>> _grpcClients;

	std::map<uint8_t, std::atomic_bool> _grpcClientsInUse;
	std::queue<entity::MessageRequest> _messagesWillBeRetried;
	std::array<uint8_t, 1000> _lastUsedGrpcClients{};

	uint16_t _lastUsedGrpcClientsEmplacerIndex{};
	uint8_t _lastUsedGrpcClientIndex{};
	uint8_t _threadIndex{};

	std::mutex _mtxMessagesWillBeRetried;
	std::mutex _mtxGrpcIndexSelector;
	std::mutex _mtxThreadIndexSelector;

	std::vector<std::jthread> _ringBufferOfSenderThreads;
	std::jthread _worker;
};

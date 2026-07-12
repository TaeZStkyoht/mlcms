#pragma once

#include "entity/PriorityCredit.hpp"

#include "interface/MessageRequestObserver.hpp"
#include "interface/MessageRequestPuller.hpp"
#include "interface/MessageRequestPusher.hpp"

#include <mutex>
#include <queue>

class MessageRequestHolder final : public MessageRequestObserver, public MessageRequestPusher, public MessageRequestPuller {
public:
	explicit constexpr MessageRequestHolder(entity::PriorityCredit priorityCredit) noexcept
		: _priorityCreditDefault(priorityCredit), _priorityCredit(_priorityCreditDefault)
	{
	}

private:
	entity::QueueSize Size() const override;
	void Push(entity::MessageRequest messageRequest) override;
	std::optional<entity::MessageRequest> Pull() override;

	entity::PriorityCredit _priorityCreditDefault;
	entity::PriorityCredit _priorityCredit;

	std::queue<entity::MessageRequest> _messageRequestsCritical;
	std::queue<entity::MessageRequest> _messageRequestsHigh;
	std::queue<entity::MessageRequest> _messageRequestsNormal;
	std::queue<entity::MessageRequest> _messageRequestsLow;
	mutable std::mutex _mtx;
};

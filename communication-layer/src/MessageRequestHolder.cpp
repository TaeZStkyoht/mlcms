#include "MessageRequestHolder.hpp"

#include "middleware/Logger.hpp"

using namespace std;

using namespace middleware;

using enum Logger::Level;

static const Logger logger = Logger::GetLoggerByCategory("MessageRequestHolder");

QueueSize MessageRequestHolder::Size() const
{
	const lock_guard lg(_mtx);
	return QueueSize{
		.critical = _messageRequestsCritical.size(),
		.high = _messageRequestsHigh.size(),
		.normal = _messageRequestsNormal.size(),
		.low = _messageRequestsLow.size(),
	};
}

void MessageRequestHolder::Push(entity::MessageRequest messageRequest)
{
	switch (const lock_guard lg(_mtx); messageRequest.priority) {
		using enum entity::MessageRequest::Priority;
	[[unlikely]] default:
		logger << WARN << "Unexpected priority has come. Set to LOW. Original value: " << static_cast<uint16_t>(messageRequest.priority);
		messageRequest.priority = LOW;
		[[fallthrough]];
	case LOW:
		_messageRequestsLow.push(move(messageRequest));
		break;
	case NORMAL:
		_messageRequestsNormal.push(move(messageRequest));
		break;
	case HIGH:
		_messageRequestsHigh.push(move(messageRequest));
		break;
	case CRITICAL:
		_messageRequestsCritical.push(move(messageRequest));
		break;
	}
}

optional<entity::MessageRequest> MessageRequestHolder::Pull()
{
	{
		const lock_guard locker(_mtx);

		const auto popFromQueue = [](queue<entity::MessageRequest>& queueToBePopped) {
			auto messageRequest = make_optional(move(queueToBePopped.front()));
			queueToBePopped.pop();
			return messageRequest;
		};

		if (!_messageRequestsCritical.empty())
			return popFromQueue(_messageRequestsCritical);

		const auto checkCredit = [](uint8_t& credit) noexcept -> bool {
			if (credit) {
				--credit;
				return true;
			}
			return {};
		};

		if (!_messageRequestsHigh.empty() && checkCredit(_priorityCredit.high))
			return popFromQueue(_messageRequestsHigh);

		if (!_messageRequestsNormal.empty() && checkCredit(_priorityCredit.normal))
			return popFromQueue(_messageRequestsNormal);

		if (!_messageRequestsLow.empty() && checkCredit(_priorityCredit.low))
			return popFromQueue(_messageRequestsLow);
	}

	_priorityCredit = _priorityCreditDefault;
	return {};
}

#include "Core.hpp"

#include "MessageRequestHolder.hpp"

#include "middleware/GrpcPorts.hpp"

#include <format>

#include <cstring>

using namespace std;

Core Core::Create(span<const char* const> arguments)
{
	Core core;

	const auto messageRequestHolder = make_shared<MessageRequestHolder>([arguments] {
		entity::PriorityCredit priorityCredit;
		const auto fillPriorityCredit = [arguments, argumentSize = arguments.size()](uint8_t index, uint8_t& priorityCreditItem) {
			bool indexIsProper = argumentSize > index;
			if (indexIsProper)
				priorityCreditItem = static_cast<uint8_t>(stoi(arguments[index]));
			return indexIsProper;
		};
		fillPriorityCredit(1, priorityCredit.high) && fillPriorityCredit(2, priorityCredit.normal) && fillPriorityCredit(3, priorityCredit.low);
		return priorityCredit;
	}());

	core._grpcServer = make_unique<GrpcServer>(format("127.0.0.1:{}", GrpcPorts::providerToCommunicationLayer), messageRequestHolder);

	core._messageRequestSender = make_unique<MessageRequestSender>(messageRequestHolder);

	core._observer = make_unique<Observer>(messageRequestHolder);

	return core;
}

void Core::Start() const
{
	_grpcServer->Start();
	_messageRequestSender->Start();
	_observer->Start();
}

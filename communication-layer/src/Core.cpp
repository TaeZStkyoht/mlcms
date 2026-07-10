#include "Core.hpp"

#include "MessageRequestHolder.hpp"

#include "middleware/GrpcPorts.hpp"

#include <format>

#include <cstring>

using namespace std;

extern atomic_bool run;

Core Core::Create(span<const char* const> arguments)
{
	Core core;

	const auto messageRequestHolder = make_shared<MessageRequestHolder>([arguments] {
		entity::PriorityCredit priorityCredit;
		const auto fillPriorityCredit = [arguments](uint8_t index, uint8_t& priorityCreditItem) {
			bool indexIsProper = arguments.size() > index;
			if (indexIsProper)
				priorityCreditItem = static_cast<uint8_t>(stoi(arguments[index]));
			return indexIsProper;
		};
		fillPriorityCredit(1, priorityCredit.high) && fillPriorityCredit(2, priorityCredit.normal) && fillPriorityCredit(3, priorityCredit.low);
		return priorityCredit;
	}());

	core._grpcServer = make_unique<GrpcServer>(format("127.0.0.1:{}", GrpcPorts::providerToCommunicationLayer), messageRequestHolder);

	vector<shared_ptr<GrpcClient>> grpcClients;
	for (uint8_t i = 0; i < (arguments.size() > 4 ? stoi(arguments[4]) : 10); ++i)
		grpcClients.push_back(make_shared<GrpcClient>(format("127.0.0.1:{}", GrpcPorts::communicationLayerToConsumer + i)));

	core._messageRequestSender = make_unique<MessageRequestSender>(messageRequestHolder, grpcClients);

	core._observer = make_unique<Observer>(messageRequestHolder, vector<shared_ptr<AvailabilityProvider>>{grpcClients.begin(), grpcClients.end()});

	return core;
}

void Core::Start() const
{
	if (!_grpcServer->Start()) {
		run = false;
		return;
	}
	_messageRequestSender->Start();
	_observer->Start();
}

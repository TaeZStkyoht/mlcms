#include "GrpcServer.hpp"

#include "middleware/BaseGrpcServer.hpp"
#include "middleware/Logger.hpp"

#include "communication-layer.grpc.pb.h"

#include <google/protobuf/util/time_util.h>

using namespace std;
using namespace chrono;
using namespace filesystem;

using namespace grpc;
using namespace google::protobuf;

using namespace middleware;

using enum Logger::Level;

extern const atomic_bool run;

static const Logger logger = Logger::getLoggerByCategory("GrpcServer");

class GrpcServer::Impl final : public cl::CommunicationLayer::Service, public BaseGrpcServer {
public:
	Impl(string url, shared_ptr<MessageRequestPusher> messageRequestPusher) noexcept
		: BaseGrpcServer(this, move(url)), _messageRequestPusher(messageRequestPusher)
	{
	}

private:
	Status SendMessage(ServerContext* context, ServerReader<cl::MessageRequest>* reader, Empty*) override
	{
		logger << DEBUG << "Client (" << context << ") connected and allowed to stream";

		while (run && !context->IsCancelled()) {
			if (cl::MessageRequest messageRequest; reader->Read(&messageRequest)) {
				_messageRequestPusher->Push({
					.priority = static_cast<entity::MessageRequest::Priority>(messageRequest.priority()),
					.sender_id = messageRequest.sender_id(),
					.timestamp = system_clock::time_point(nanoseconds(util::TimeUtil::TimestampToNanoseconds(messageRequest.timestamp()))),
					.payload = messageRequest.payload(),
				});
			}
		}

		return Status::OK;
	}

	shared_ptr<MessageRequestPusher> _messageRequestPusher;
};

GrpcServer::GrpcServer(string url, shared_ptr<MessageRequestPusher> messageRequestPusher) : _impl(make_unique<Impl>(move(url), messageRequestPusher))
{
}

GrpcServer::~GrpcServer() = default;

bool GrpcServer::Start() const
{
	return _impl->Start();
}

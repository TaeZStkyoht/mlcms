#include "GrpcServer.hpp"

#include "middleware/BaseGrpcServer.hpp"
#include "middleware/Logger.hpp"

#include "consumer.grpc.pb.h"

#include <google/protobuf/util/time_util.h>

using namespace std;
using namespace chrono;
using namespace filesystem;

using namespace grpc;
using namespace google::protobuf;

using namespace middleware;

using enum Logger::Level;

extern const atomic_bool run;

static const Logger logger = Logger::GetLoggerByCategory("GrpcServer");

class GrpcServer::Impl final : public cl::Consumer::Service, public BaseGrpcServer {
public:
	explicit Impl(string url) noexcept : BaseGrpcServer(this, move(url))
	{
	}

private:
	Status SendMessage(ServerContext*, const cl::OuterMessageRequest*, Empty*)
	{
		return Status::OK;
	}
};

GrpcServer::GrpcServer(string url) : _impl(make_unique<Impl>(move(url)))
{
}

GrpcServer::~GrpcServer() = default;

bool GrpcServer::Start() const
{
	return _impl->Start();
}

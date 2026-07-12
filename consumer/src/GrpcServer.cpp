#include "GrpcServer.hpp"

#include "middleware/BaseGrpcServer.hpp"

#include "consumer.grpc.pb.h"

using namespace std;

using namespace grpc;
using namespace google::protobuf;

using namespace middleware;

extern const atomic_bool run;

class GrpcServer::Impl final : public cl::Consumer::Service, public BaseGrpcServer {
public:
	explicit Impl(string url) noexcept : BaseGrpcServer(this, move(url))
	{
	}

private:
	Status SendMessage(ServerContext*, const cl::OuterMessageRequest*, Empty*) override
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

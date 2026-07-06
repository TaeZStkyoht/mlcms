#include "BaseGrpcServer.hpp"

#include "Logger.hpp"

#include <grpcpp/server_builder.h>

using namespace std;

using namespace grpc;

using namespace middleware;

using enum Logger::Level;

static const Logger logger = Logger::getLoggerByCategory("BaseGrpcServer");

static constexpr auto GetShutdownTimespec(std::chrono::milliseconds timeoutDurationForShutdown)
{
	const auto timeoutSecs = std::chrono::duration_cast<std::chrono::seconds>(timeoutDurationForShutdown);
	return gpr_timespec{
		.tv_sec = timeoutSecs.count(),
		.tv_nsec = static_cast<int32_t>(std::chrono::duration_cast<std::chrono::nanoseconds>(timeoutDurationForShutdown - timeoutSecs).count()),
		.clock_type = gpr_clock_type::GPR_TIMESPAN,
	};
}

BaseGrpcServer::~BaseGrpcServer()
{
	if (_server) {
		_server->Shutdown(GetShutdownTimespec(_timeoutDurationForShutdown));
		::logger << DEBUG << "Grpc server shutdown.";
	}
}

bool BaseGrpcServer::Start()
{
	::logger << INFO << "Connect to " << _address;

	_server = ServerBuilder().AddListeningPort(_address, InsecureServerCredentials()).RegisterService(_service).BuildAndStart();

	if (!_server) {
		::logger << ERROR << "Server cannot be created.";
		return {};
	}

	_thread = jthread([this] { _server->Wait(); });
	return true;
}

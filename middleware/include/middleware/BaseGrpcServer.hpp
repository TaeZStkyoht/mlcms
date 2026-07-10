#pragma once

#include <grpcpp/server.h>

#include <thread>

namespace middleware {
	class BaseGrpcServer {
	public:
		BaseGrpcServer(grpc::Service* service, std::string address,
					   std::chrono::milliseconds timeoutDurationForShutdown = std::chrono::milliseconds(100)) noexcept
			: _service(service), _address(move(address)), _timeoutDurationForShutdown(timeoutDurationForShutdown)
		{
		}

		virtual ~BaseGrpcServer();

		[[nodiscard]] bool Start();

	private:
		grpc::Service* _service;
		std::string _address;
		std::chrono::milliseconds _timeoutDurationForShutdown;

		std::unique_ptr<grpc::Server> _server;

		std::jthread _thread;
	};
}

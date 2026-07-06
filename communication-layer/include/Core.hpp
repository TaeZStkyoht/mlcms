#pragma once

#include "GrpcServer.hpp"
#include "MessageRequestSender.hpp"

#include <atomic>

class Core final {
public:
	static Core Create(std::span<const char* const> arguments);

	void Start() const;

private:
	std::unique_ptr<GrpcServer> _grpcServer;
	std::unique_ptr<MessageRequestSender> _messageRequestSender;
};

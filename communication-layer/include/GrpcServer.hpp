#pragma once

#include "interface/MessageRequestPusher.hpp"

#include <memory>
#include <string>

class GrpcServer final {
public:
	GrpcServer(std::string uri, std::shared_ptr<MessageRequestPusher> messageRequestPusher);
	~GrpcServer();

	bool Start() const;

private:
	class Impl;
	std::unique_ptr<Impl> _impl;
};

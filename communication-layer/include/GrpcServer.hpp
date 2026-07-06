#pragma once

#include "interface/MessageRequestPusher.hpp"

#include <memory>
#include <string>

class GrpcServer final {
public:
	[[nodiscard]] GrpcServer(std::string uri, std::shared_ptr<MessageRequestPusher> messageRequestPusher);
	~GrpcServer();

	void Start() const;

private:
	class Impl;
	std::unique_ptr<Impl> _impl;
};

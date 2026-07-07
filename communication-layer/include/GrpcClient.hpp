#pragma once

#include <chrono>
#include <memory>
#include <string>

class GrpcClient final {
public:
	[[nodiscard]] GrpcClient(std::string uri);
	~GrpcClient();

	void SendMessage(std::string message, std::chrono::system_clock::time_point timestamp) const;

private:
	class Impl;
	std::unique_ptr<Impl> _impl;
};

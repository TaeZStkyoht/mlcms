#pragma once

#include <memory>
#include <string>

class GrpcClient final {
public:
	[[nodiscard]] explicit GrpcClient(std::string uri);
	~GrpcClient();

	void Start();

private:
	class Impl;
	std::unique_ptr<Impl> _impl;
};

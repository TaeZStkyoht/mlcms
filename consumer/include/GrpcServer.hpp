#pragma once

#include <memory>
#include <string>

class GrpcServer final {
public:
	[[nodiscard]] GrpcServer(std::string uri);
	~GrpcServer();

	void Start() const;

private:
	class Impl;
	std::unique_ptr<Impl> _impl;
};

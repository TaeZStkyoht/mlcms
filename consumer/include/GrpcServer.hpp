#pragma once

#include <memory>
#include <string>

class GrpcServer final {
public:
	explicit GrpcServer(std::string uri);
	~GrpcServer();

	bool Start() const;

private:
	class Impl;
	std::unique_ptr<Impl> _impl;
};

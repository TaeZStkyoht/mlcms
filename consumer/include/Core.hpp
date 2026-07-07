#pragma once

#include "GrpcServer.hpp"

class Core final {
public:
	static Core Create();

	void Start() const;

private:
	std::unique_ptr<GrpcServer> _grpcServer;
};

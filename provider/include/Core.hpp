#pragma once

#include "GrpcClient.hpp"

class Core final {
public:
	static Core Create();

	void Start() const;

private:
	std::unique_ptr<GrpcClient> _gprcClient;
};

#pragma once

#include <functional>

namespace MockGrpcServer {
	inline std::function<bool()> callbackStart;

	inline void Clear() noexcept
	{
		callbackStart = {};
	}
}

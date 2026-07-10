#pragma once

#include <functional>

namespace MockGrpcClient {
	inline std::function<void()> callbackStart;

	inline void Clear() noexcept
	{
		callbackStart = {};
	}
}

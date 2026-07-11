#pragma once

#include <chrono>
#include <functional>

namespace middleware::MockSleepWrapper {
	inline std::function<void(std::chrono::nanoseconds)> callbackSleep;

	inline void Clear() noexcept
	{
		callbackSleep = {};
	}
}

#pragma once

#include <functional>

namespace MockObserver {
	inline std::function<void()> callbackStart;

	inline void Clear() noexcept
	{
		callbackStart = {};
	}
}

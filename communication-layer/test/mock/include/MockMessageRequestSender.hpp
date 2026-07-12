#pragma once

#include <functional>

namespace MockMessageRequestSender {
	inline std::function<void()> callbackStart;

	inline void Clear() noexcept
	{
		callbackStart = {};
	}
}

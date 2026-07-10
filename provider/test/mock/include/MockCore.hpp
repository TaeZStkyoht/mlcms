#pragma once

#include "Core.hpp"

#include <functional>

namespace MockCore {
	inline std::function<void()> callbackCreate;
	inline std::function<void()> callbackStart;

	inline void Clear() noexcept
	{
		callbackCreate = {};
		callbackStart = {};
	}
}

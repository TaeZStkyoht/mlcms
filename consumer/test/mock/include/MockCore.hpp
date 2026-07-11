#pragma once

#include "Core.hpp"

#include <functional>

namespace MockCore {
	inline std::function<void(std::span<const char* const>)> callbackCreate;
	inline std::function<void()> callbackStart;

	inline void Clear() noexcept
	{
		callbackCreate = {};
		callbackStart = {};
	}
}

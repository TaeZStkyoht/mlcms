#pragma once

#include <log4cpp/Priority.hh>

#include <functional>

namespace middleware::FakeLog4cpp {
	inline std::function<const std::string&()> callbackCategoryExists;
	inline std::function<void(const std::string&, const std::string&, const std::string&, log4cpp::Priority::Value)> callbackLoggingEventCtor;

	inline void Clear() noexcept
	{
		callbackCategoryExists = {};
		callbackLoggingEventCtor = {};
	}
}

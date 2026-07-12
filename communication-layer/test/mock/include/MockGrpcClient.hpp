#pragma once

#include <chrono>
#include <functional>
#include <string>

#include <cstdint>

namespace MockGrpcClient {
	inline std::function<bool()> callbackIsAvailable;
	inline std::function<uint32_t()> callbackAverageCommunicationDuration;
	inline std::function<bool(const std::string&, std::chrono::system_clock::time_point)> callbackSendMessage;
	inline std::function<std::chrono::steady_clock::time_point()> callbackLastTriedTime;

	inline void Clear() noexcept
	{
		callbackIsAvailable = {};
		callbackAverageCommunicationDuration = {};
		callbackSendMessage = {};
		callbackLastTriedTime = {};
	}
}

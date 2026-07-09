#pragma once

#include <chrono>
#include <string>

namespace entity {
	struct OuterMessageRequest final {
		std::chrono::system_clock::time_point timestamp;
		std::chrono::steady_clock::time_point queuedTimestamp;
		std::string payload;
	};
}

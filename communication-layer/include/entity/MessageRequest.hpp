#pragma once

#include <chrono>
#include <vector>

#include <cstdint>

namespace entity {
	struct MessageRequest final {
		enum class Priority : uint8_t {
			NONE = 0,
			LOW = 1,
			NORMAL = 2,
			HIGH = 3,
			CRITICAL = 4,
		};

		Priority priority;
		uint32_t sender_id;
		std::chrono::system_clock::time_point timestamp;
		std::vector<uint8_t> payload;
	};
}

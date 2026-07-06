#pragma once

#include <cstdint>

namespace entity {
	struct PriorityCredit final {
		uint8_t high = 10;
		uint8_t normal = 5;
		uint8_t low = 2;
	};
}

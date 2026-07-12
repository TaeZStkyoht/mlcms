#pragma once

#include <cstddef>

namespace entity {
	struct QueueSize final {
		size_t critical;
		size_t high;
		size_t normal;
		size_t low;
	};
}

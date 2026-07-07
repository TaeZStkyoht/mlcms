#pragma once

#include <cstddef>

struct QueueSize final {
	size_t critical;
	size_t high;
	size_t normal;
	size_t low;
};

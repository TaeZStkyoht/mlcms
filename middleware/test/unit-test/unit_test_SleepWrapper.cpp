#include "SleepWrapper.hpp"

#include <gtest/gtest.h>

using namespace std;
using namespace chrono;

TEST(SleepWrapper, Sleep)
{
	constexpr auto sleepDuration = 10ms;

	const auto startTime = steady_clock::now();
	SleepWrapper::Sleep(sleepDuration);
	const auto endTime = steady_clock::now();

	EXPECT_GE(endTime - startTime, sleepDuration);
}

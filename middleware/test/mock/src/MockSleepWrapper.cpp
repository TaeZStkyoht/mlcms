#include "MockSleepWrapper.hpp"

#include "SleepWrapper.hpp"

using namespace std::chrono;

using namespace middleware;

using namespace MockSleepWrapper;

void SleepWrapper::Sleep(nanoseconds sleepDuration)
{
	if (callbackSleep)
		callbackSleep(sleepDuration);
}

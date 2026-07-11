#include "SleepWrapper.hpp"

#include <thread>

using namespace std;
using namespace chrono;

void SleepWrapper::Sleep(nanoseconds sleepDuration)
{
	this_thread::sleep_for(sleepDuration);
}

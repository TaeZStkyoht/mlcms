#include "MockObserver.hpp"

#include "Observer.hpp"

using namespace std;

using namespace MockObserver;

void Observer::Start()
{
	if (callbackStart)
		callbackStart();
}

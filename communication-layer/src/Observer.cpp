#include "Observer.hpp"

#include "middleware/Logger.hpp"

using namespace std;

using namespace middleware;

using enum Logger::Level;

static const Logger logger = Logger::getLoggerByCategory("Observer");

void Observer::Start()
{
	_worker = jthread([this] { Work(); });
}

void Observer::Work()
{
	for (; !_worker.get_stop_token().stop_requested(); this_thread::sleep_for(1s)) {
		const auto queueSize = _messageRequestObserver->Size();

		logger << DEBUG << "Queue sizes (crit, high, normal, low): " << queueSize.critical << ", " << queueSize.high << ", " << queueSize.normal << ", "
			   << queueSize.low;
	}
}

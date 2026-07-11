#include "Observer.hpp"

#include "middleware/Logger.hpp"
#include "middleware/SleepWrapper.hpp"

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
	for (; !_worker.get_stop_token().stop_requested(); SleepWrapper::Sleep(1s)) {
		const auto queueSize = _messageRequestObserver->Size();

		logger << DEBUG << "Queue sizes (crit, high, normal, low): " << queueSize.critical << ", " << queueSize.high << ", " << queueSize.normal << ", "
			   << queueSize.low;

		{
			const auto logStream = logger << DEBUG;
			logStream << "Channel Average Communication: ";
			for (size_t i = 0; i < _availabilityProvider.size(); ++i)
				logStream << i << ": "
						  << (_availabilityProvider[i]->IsAvailable() ? to_string(_availabilityProvider[i]->AverageCommunicationDuration()) : "N/A")
						  << ", ";
		}
	}
}

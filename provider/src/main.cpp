#include "Core.hpp"

#include "middleware/Logger.hpp"

#include <csignal>

#include <thread>

using namespace std;
using namespace chrono;

using namespace middleware;

using enum Logger::Level;

static const Logger logger = Logger::getLoggerByCategory("main");

atomic_bool run = true;

int main()
{
	const auto signalHandler = [](int) {
		logger << INFO << "signal caught!";
		run = false;
	};
	signal(SIGTERM, signalHandler);
	signal(SIGINT, signalHandler);

	Logger::enableStdoutLogging();
	Logger::setLogLevel(DEBUG);

	const auto core = Core::Create();
	core.Start();

	for (;; this_thread::sleep_for(500ms))
		if (!run) [[unlikely]]
			break;

	return EXIT_SUCCESS;
}

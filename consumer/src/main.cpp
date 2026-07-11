#include "Core.hpp"

#include "middleware/Logger.hpp"
#include "middleware/SleepWrapper.hpp"

#include <atomic>

#include <csignal>

using namespace std;
using namespace chrono;

using namespace middleware;

using enum Logger::Level;

static const Logger logger = Logger::GetLoggerByCategory("main");

atomic_bool run = true;

int main(int argc, const char* argv[])
{
	const auto signalHandler = [](int) {
		logger << INFO << "signal caught!";
		run = false;
	};
	signal(SIGTERM, signalHandler);
	signal(SIGINT, signalHandler);

	Logger::EnableStdoutLogging();
	Logger::SetLogLevel(DEBUG);

	const auto core = Core::Create({argv, argv + argc});
	core.Start();

	for (;; SleepWrapper::Sleep(500ms))
		if (!run) [[unlikely]]
			break;

	return EXIT_SUCCESS;
}

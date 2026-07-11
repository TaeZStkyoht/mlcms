#include "FakeLogger.hpp"

using namespace std;
using namespace chrono;

using namespace log4cpp;

using namespace middleware;

void FakeLogger::ClearByCategoryName(string_view categoryName)
{
	const lock_guard lg(_logsMutex);
	_logs.erase(remove_if(_logs.begin(), _logs.end(), [categoryName](const LogInfo& logInfo) noexcept { return logInfo._category->name == categoryName; }),
				_logs.end());
}

bool FakeLogger::CheckAnyLog(string_view category, Logger::Level logLevel, string_view message, bool exactMessage)
{
	const lock_guard lg(_logsMutex);
	return any_of(FakeLogger::_logs.begin(), FakeLogger::_logs.end(),
				  [&](const LogInfo& item) noexcept { return item.Match(category, logLevel, message, exactMessage); });
}

bool FakeLogger::CheckLastLog(string_view category, Logger::Level logLevel, string_view message, bool exactMessage) noexcept
{
	const lock_guard lg(_logsMutex);
	return !FakeLogger::_logs.empty() && FakeLogger::_logs.back().Match(category, logLevel, message, exactMessage);
}

bool FakeLogger::LogInfo::Match(string_view category, Logger::Level logLevel, string_view message, bool exactMessage) const noexcept
{
	return _category->name == category && _logLevel == logLevel && [&]() noexcept {
		if (exactMessage)
			return _message == message;
		return _message.find(message) != string::npos;
	}();
}

void Logger::LogEvent(Logger::Level logLevel, milliseconds timestamp, const string& message) const
{
	const lock_guard lg(FakeLogger::_logsMutex);
	FakeLogger::_logs.emplace_back(&_category, logLevel, timestamp, message);
}

Logger Logger::GetLoggerByCategory(const string& categoryName)
{
	const lock_guard lg(FakeLogger::_categoriesMutex);
	return Logger(*FakeLogger::_categories.emplace_back(make_unique<log4cpp::Category>(categoryName)));
}

void Logger::SetLogLevel(Logger::Level)
{
	// Doesn't need to be defined
}

void Logger::EnableFileLogging(const string&)
{
	// Doesn't need to be defined
}

void Logger::EnableStdoutLogging()
{
	// Doesn't need to be defined
}

void Logger::EnableSysLogLogging(const string&)
{
	// Doesn't need to be defined
}

Logger::LogStream::LogStream(Logger::Level level, Category& category)
{
	const lock_guard lg(FakeLogger::_logsMutex);
	FakeLogger::_logs.emplace_back(&category, level, milliseconds(), string());
}

Logger::LogStream::~LogStream()
{
	if (const lock_guard lg(FakeLogger::_logsMutex); !FakeLogger::_logs.empty())
		FakeLogger::_logs.back()._timestamp = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
}

template <>
const Logger::LogStream& Logger::LogStream::operator<<(const stringstream& t) const
{
	if (const lock_guard lg(FakeLogger::_logsMutex); !FakeLogger::_logs.empty())
		FakeLogger::_logs.back()._message += t.str();
	return *this;
}

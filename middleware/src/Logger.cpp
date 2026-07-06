#include "Logger.hpp"

#include <log4cpp/Category.hh>
#include <log4cpp/FileAppender.hh>
#include <log4cpp/NDC.hh>
#include <log4cpp/OstreamAppender.hh>
#include <log4cpp/PatternLayout.hh>
#include <log4cpp/SyslogAppender.hh>

using namespace std;
using namespace chrono;
using namespace middleware;
using namespace log4cpp;

static constexpr Priority::PriorityLevel logLevelToPriority(Logger::Level level) noexcept
{
	switch (level) {
	[[unlikely]] default:
		[[fallthrough]];
	case Logger::Level::DEBUG:
		return Priority::DEBUG;
	case Logger::Level::INFO:
		return Priority::INFO;
	case Logger::Level::NOTICE:
		return Priority::NOTICE;
	case Logger::Level::WARN:
		return Priority::WARN;
	case Logger::Level::ERROR:
		return Priority::ERROR;
	case Logger::Level::CRIT:
		return Priority::CRIT;
	case Logger::Level::ALERT:
		return Priority::ALERT;
	}
}

void Logger::logEvent(Logger::Level logLevel, milliseconds timestamp, const string& message) const
{
	LoggingEvent event(_category.getName(), message, NDC::get(), logLevelToPriority(logLevel));
	event.timeStamp = TimeStamp(static_cast<unsigned int>(timestamp.count() / 1000), static_cast<unsigned int>((timestamp.count() % 1000) * 1000));
	_category.callAppenders(event);
}

Logger Logger::getLoggerByCategory(const string& categoryName)
{
	Category* category = Category::exists(categoryName);
	if (category == nullptr)
		category = &Category::getInstance(categoryName);
	return Logger(*category);
}

void Logger::setLogLevel(Logger::Level level)
{
	Category::getRoot().setPriority(logLevelToPriority(level));
}

void Logger::enableFileLogging(const string& logFileName)
{
	auto appender = make_unique<FileAppender>("FileAppender", logFileName);
	auto layout = make_unique<PatternLayout>();
	layout->setConversionPattern("%d %-7p(%t) %-9c: %m%n");
	appender->setLayout(layout.release());
	Category::getRoot().addAppender(appender.release());
}

void Logger::enableStdoutLogging()
{
	auto appender = make_unique<OstreamAppender>("Console", &cout);
	auto layout = make_unique<PatternLayout>();
	layout->setConversionPattern("%d %-7p(%t) %-9c: %m%n");
	appender->setLayout(layout.release());
	Category::getRoot().addAppender(appender.release());
}

void Logger::enableSysLogLogging(const string& syslogName)
{
	auto appender = make_unique<SyslogAppender>("SysLog", syslogName);
	auto layout = make_unique<PatternLayout>();
	layout->setConversionPattern("%-7p (%t) %-9c: %m%n");
	appender->setLayout(layout.release());
	Category::getRoot().addAppender(appender.release());
}

Logger::LogStream::LogStream(Logger::Level level, Category& category) : _categoryStream(make_unique<CategoryStream>(category, logLevelToPriority(level)))
{
}

Logger::LogStream::~LogStream() = default;

template <>
const Logger::LogStream& Logger::LogStream::operator<<(const stringstream& t) const
{
	*_categoryStream << t.str();
	return *this;
}

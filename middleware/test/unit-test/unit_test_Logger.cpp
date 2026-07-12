#include "Logger.hpp"

#include "FakeLog4Cpp.hpp"

#include <log4cpp/Category.hh>
#include <log4cpp/FileAppender.hh>
#include <log4cpp/OstreamAppender.hh>
#include <log4cpp/PatternLayout.hh>
#include <log4cpp/SyslogAppender.hh>

#include <gtest/gtest.h>

using namespace std;

using namespace testing;

using namespace log4cpp;

using namespace middleware;

class LoggerFixture : public Test {
protected:
	void SetUp() noexcept final
	{
		FakeLog4cpp::Clear();
	}

	struct ProxyLogger final {
		log4cpp::Category& _category;
	};

	struct LayoutAppenderProxy final : public LayoutAppender {
		Layout& getLayout()
		{
			return LayoutAppender::_getLayout();
		}
	};

	struct FileAppenderProxy final : public FileAppender {
		constexpr const string& fileName() const noexcept
		{
			return _fileName;
		}
	};

	struct SyslogAppenderProxy final : public SyslogAppender {
		constexpr const string& syslogName() const noexcept
		{
			return _syslogName;
		}
	};

	template <typename AppenderType, typename = std::enable_if_t<std::is_base_of<Appender, AppenderType>::value>>
	static void checkAppender(const string& appenderName, const string& conversionPattern, AppenderType*& appender)
	{
		Appender* const appenderBase = Category::getRoot().getAppender(appenderName);
		ASSERT_NE(appenderBase, nullptr);
		appender = dynamic_cast<AppenderType*>(appenderBase);
		ASSERT_NE(appender, nullptr);
		const auto layoutAppender = dynamic_cast<LayoutAppender*>(appenderBase);
		ASSERT_NE(layoutAppender, nullptr);
		const auto patternLayout = dynamic_cast<PatternLayout*>(&static_cast<LayoutAppenderProxy*>(layoutAppender)->getLayout());
		ASSERT_NE(patternLayout, nullptr);
		EXPECT_EQ(patternLayout->getConversionPattern(), conversionPattern);
	}
};

TEST_F(LoggerFixture, LogEventWithDEBUG)
{
	FakeLog4cpp::callbackLoggingEventCtor = [](const string&, const string&, const string&, Priority::Value priority) noexcept {
		EXPECT_EQ(priority, Priority::DEBUG);
	};
	Logger logger = Logger::GetLoggerByCategory({});
	logger.LogEvent(Logger::Level::DEBUG, {}, {});
}

TEST_F(LoggerFixture, LogEventWithINFO)
{
	FakeLog4cpp::callbackLoggingEventCtor = [](const string&, const string&, const string&, Priority::Value priority) noexcept {
		EXPECT_EQ(priority, Priority::INFO);
	};
	Logger logger = Logger::GetLoggerByCategory({});
	logger.LogEvent(Logger::Level::INFO, {}, {});
}

TEST_F(LoggerFixture, LogEventWithNOTICE)
{
	FakeLog4cpp::callbackLoggingEventCtor = [](const string&, const string&, const string&, Priority::Value priority) noexcept {
		EXPECT_EQ(priority, Priority::NOTICE);
	};
	Logger logger = Logger::GetLoggerByCategory({});
	logger.LogEvent(Logger::Level::NOTICE, {}, {});
}

TEST_F(LoggerFixture, LogEventWithWARN)
{
	FakeLog4cpp::callbackLoggingEventCtor = [](const string&, const string&, const string&, Priority::Value priority) noexcept {
		EXPECT_EQ(priority, Priority::WARN);
	};
	Logger logger = Logger::GetLoggerByCategory({});
	logger.LogEvent(Logger::Level::WARN, {}, {});
}

TEST_F(LoggerFixture, LogEventWithERROR)
{
	FakeLog4cpp::callbackLoggingEventCtor = [](const string&, const string&, const string&, Priority::Value priority) noexcept {
		EXPECT_EQ(priority, Priority::ERROR);
	};
	Logger logger = Logger::GetLoggerByCategory({});
	logger.LogEvent(Logger::Level::ERROR, {}, {});
}

TEST_F(LoggerFixture, LogEventWithCRIT)
{
	FakeLog4cpp::callbackLoggingEventCtor = [](const string&, const string&, const string&, Priority::Value priority) noexcept {
		EXPECT_EQ(priority, Priority::CRIT);
	};
	Logger logger = Logger::GetLoggerByCategory({});
	logger.LogEvent(Logger::Level::CRIT, {}, {});
}

TEST_F(LoggerFixture, LogEventWithALERT)
{
	FakeLog4cpp::callbackLoggingEventCtor = [](const string&, const string&, const string&, Priority::Value priority) noexcept {
		EXPECT_EQ(priority, Priority::ALERT);
	};
	Logger logger = Logger::GetLoggerByCategory({});
	logger.LogEvent(Logger::Level::ALERT, {}, {});
}

TEST_F(LoggerFixture, GetLoggerByCategoryCategoryExists)
{
	const string name = "cat1";
	FakeLog4cpp::callbackCategoryExists = [&name]() noexcept -> const string& { return name; };

	const Logger logger = Logger::GetLoggerByCategory({});
	EXPECT_EQ(reinterpret_cast<const ProxyLogger&>(logger)._category.getName(), name);
}

TEST_F(LoggerFixture, GetLoggerByCategoryCategoryDoesNotExist)
{
	const Logger logger = Logger::GetLoggerByCategory({});
	EXPECT_EQ(reinterpret_cast<const ProxyLogger&>(logger)._category.getName(), string());
}

TEST_F(LoggerFixture, SetLogLevel)
{
	Logger::SetLogLevel(Logger::Level::DEBUG);
	EXPECT_EQ(Category::getRoot().getPriority(), Priority::DEBUG);
}

TEST_F(LoggerFixture, EnableFileLogging)
{
	const string fileName = "file1";
	Logger::EnableFileLogging(fileName);
	FileAppender* appender;
	ASSERT_NO_FATAL_FAILURE({ checkAppender("FileAppender", "%d %-7p(%t) %-9c: %m%n", appender); });
	EXPECT_EQ(static_cast<FileAppenderProxy*>(appender)->fileName(), fileName);
}

TEST_F(LoggerFixture, EnableStdoutLogging)
{
	Logger::EnableStdoutLogging();
	OstreamAppender* appender;
	ASSERT_NO_FATAL_FAILURE({ checkAppender("Console", "%d %-7p(%t) %-9c: %m%n", appender); });
}

TEST_F(LoggerFixture, EnableSysLogLogging)
{
	const string syslogName = "mlcms";
	Logger::EnableSysLogLogging(syslogName);
	SyslogAppender* appender;
	ASSERT_NO_FATAL_FAILURE({ checkAppender("SysLog", "%-7p (%t) %-9c: %m%n", appender); });
	EXPECT_EQ(static_cast<SyslogAppenderProxy*>(appender)->syslogName(), syslogName);
}

TEST_F(LoggerFixture, LogStream)
{
	const Logger logger = Logger::GetLoggerByCategory({});
	const auto logStream = logger << Logger::Level::DEBUG;

	constexpr string_view str1 = "word1";
	constexpr string_view str2 = " word2";
	logStream << str1;
	logStream << str2;

	struct LogStreamProxy final {
		std::unique_ptr<log4cpp::CategoryStream> _categoryStream;
	};

	struct CategoryStreamProxy final {
		Category& _category;
		Priority::Value _priority;
		union {
			std::ostringstream* _buffer;
#if LOG4CPP_HAS_WCHAR_T != 0
			std::wostringstream* _wbuffer;
#endif
		};
	};

	const auto getExpected = [str1, str2] {
		stringstream ss;
		ss << str1 << str2;
		return ss.str();
	};

	EXPECT_EQ(reinterpret_cast<const CategoryStreamProxy&>(*reinterpret_cast<const LogStreamProxy&>(logStream)._categoryStream)._buffer->str(),
			  getExpected());
}

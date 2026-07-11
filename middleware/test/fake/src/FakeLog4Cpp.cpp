#include "FakeLog4Cpp.hpp"

#include <log4cpp/Category.hh>
#include <log4cpp/FileAppender.hh>
#include <log4cpp/NDC.hh>
#include <log4cpp/OstreamAppender.hh>
#include <log4cpp/PatternLayout.hh>
#include <log4cpp/SyslogAppender.hh>

using namespace std;

using namespace middleware;

namespace log4cpp {
#pragma region Needed Callbacks to test
#pragma region Category
	Category* Category::exists(const string&)
	{
		if (FakeLog4cpp::callbackCategoryExists) {
			if (const string& name = FakeLog4cpp::callbackCategoryExists(); !name.empty()) {
				static Category category(name, {});
				return &category;
			}
		}
		return nullptr;
	}
#pragma endregion

#pragma region LoggingEvent
	LoggingEvent::LoggingEvent(const string& category, const string& message, const string& ndc, Priority::Value priority)
	{
		if (FakeLog4cpp::callbackLoggingEventCtor)
			FakeLog4cpp::callbackLoggingEventCtor(category, message, ndc, priority);
	}
#pragma endregion
#pragma endregion

#pragma region Not needed to be mocked functions
#pragma region NDC
	const string& NDC::get()
	{
		static const string ndc;
		return ndc;
	}
#pragma endregion

#pragma region TimeStamp
	TimeStamp::TimeStamp() = default;
	TimeStamp::TimeStamp(unsigned int, unsigned int)
	{
	}
#pragma endregion

#pragma region Category
	Category::Category(const string& name, Category*, Priority::Value) : _name(name)
	{
	}
	Category& Category::getInstance(const string& name)
	{
		static Category category(name, {});
		return category;
	}
	Category& Category::getRoot()
	{
		static Category category({}, {});
		return category;
	}
	const string& Category::getName() const noexcept
	{
		return _name;
	}
	void Category::setPriority(Priority::Value priority)
	{
		_priority = priority;
	}
	Priority::Value Category::getPriority() const noexcept
	{
		return _priority;
	}
	Priority::Value Category::getChainedPriority() const noexcept
	{
		return {};
	}
	bool Category::isPriorityEnabled(Priority::Value) const noexcept
	{
		return {};
	}
	void Category::addAppender(Appender* appender)
	{
		_appender.emplace(appender);
	}
	void Category::addAppender(Appender& appender)
	{
		_appender.emplace(&appender);
	}
	Appender* Category::getAppender() const
	{
		return _appender.empty() ? nullptr : *_appender.begin();
	}
	Appender* Category::getAppender(const string& name) const
	{
		for (const auto appender : _appender)
			if (appender->getName() == name)
				return appender;
		return nullptr;
	}
	AppenderSet Category::getAllAppenders() const
	{
		return _appender;
	}
	void Category::removeAllAppenders()
	{
	}
	void Category::removeAppender(Appender*)
	{
	}
	bool Category::ownsAppender(Appender*) const noexcept
	{
		return {};
	}
	void Category::callAppenders(const LoggingEvent&) noexcept
	{
	}
	void Category::setAdditivity(bool)
	{
	}
	bool Category::getAdditivity() const noexcept
	{
		return {};
	}
	Category* Category::getParent() noexcept
	{
		return {};
	}
	const Category* Category::getParent() const noexcept
	{
		return {};
	}
	void Category::log(Priority::Value, const char*, ...) noexcept
	{
	}
	void Category::log(Priority::Value, const string&) noexcept
	{
	}
	void Category::logva(Priority::Value, const char*, va_list) noexcept
	{
	}
	CategoryStream Category::getStream(Priority::Value priority)
	{
		return CategoryStream(*this, priority);
	}
	CategoryStream Category::operator<<(Priority::Value priority)
	{
		return CategoryStream(*this, priority);
	}
	void Category::_logUnconditionally(Priority::Value, const char*, va_list) noexcept
	{
	}
	void Category::_logUnconditionally2(Priority::Value, const string&) noexcept
	{
	}
	bool Category::ownsAppender(Appender*, OwnsAppenderMap::iterator&) noexcept
	{
		return {};
	}
#pragma endregion

#pragma region CategoryStream
	CategoryStream::CategoryStream(Category& category, Priority::Value) : _category(category), _buffer(nullptr)
	{
	}
#pragma endregion

#pragma region PatternLayout
	PatternLayout::PatternLayout() = default;
	string PatternLayout::format(const LoggingEvent&)
	{
		return {};
	}
	void PatternLayout::setConversionPattern(const string& conversionPattern)
	{
		_conversionPattern = conversionPattern;
	}
	string PatternLayout::getConversionPattern() const
	{
		return _conversionPattern;
	}
	void PatternLayout::clearConversionPattern()
	{
	}
#pragma endregion

#pragma region Appender
	Appender::Appender(const string& name) : _name(name)
	{
	}
#pragma endregion

#pragma region AppenderSkeleton
	AppenderSkeleton::AppenderSkeleton(const string& name) : Appender(name)
	{
	}
	void AppenderSkeleton::doAppend(const LoggingEvent&)
	{
	}
	bool AppenderSkeleton::reopen()
	{
		return {};
	}
	void AppenderSkeleton::setThreshold(Priority::Value)
	{
	}
	Priority::Value AppenderSkeleton::getThreshold()
	{
		return {};
	}
	void AppenderSkeleton::setFilter(Filter*)
	{
	}
	Filter* AppenderSkeleton::getFilter()
	{
		return {};
	}
#pragma endregion

#pragma region LayoutAppender
	LayoutAppender::LayoutAppender(const string& name) : AppenderSkeleton(name)
	{
	}
	bool LayoutAppender::requiresLayout() const
	{
		return {};
	}
	void LayoutAppender::setLayout(Layout* layout)
	{
		_layout = layout;
	}
	Layout& LayoutAppender::_getLayout()
	{
		return *_layout;
	}
#pragma endregion

#pragma region FileAppender
	FileAppender::FileAppender(const string& name, const string& fileName, bool, mode_t) : LayoutAppender(name), _fileName(fileName)
	{
	}
	bool FileAppender::reopen()
	{
		return {};
	}
	void FileAppender::close()
	{
	}
	void FileAppender::setAppend(bool)
	{
	}
	bool FileAppender::getAppend() const
	{
		return {};
	}
	void FileAppender::setMode(mode_t)
	{
	}
	mode_t FileAppender::getMode() const
	{
		return {};
	}
	void FileAppender::_append(const LoggingEvent&)
	{
	}
#pragma endregion

#pragma region OstreamAppender
	OstreamAppender::OstreamAppender(const string& name, ostream*) : LayoutAppender(name)
	{
	}
	bool OstreamAppender::reopen()
	{
		return {};
	}
	void OstreamAppender::close()
	{
	}
	void OstreamAppender::_append(const LoggingEvent&)
	{
	}
#pragma endregion

#pragma region SyslogAppender
	SyslogAppender::SyslogAppender(const string& name, const string& syslogName, int) : LayoutAppender(name), _syslogName(syslogName)
	{
	}
	bool SyslogAppender::reopen()
	{
		return {};
	}
	void SyslogAppender::close()
	{
	}
	void SyslogAppender::open()
	{
	}
	void SyslogAppender::_append(const LoggingEvent&)
	{
	}
#pragma endregion

#pragma region Destructors
	Appender::~Appender() = default;
	AppenderSkeleton::~AppenderSkeleton() = default;
	LayoutAppender::~LayoutAppender() = default;
	FileAppender::~FileAppender() = default;
	OstreamAppender::~OstreamAppender() = default;
	SyslogAppender::~SyslogAppender() = default;
	PatternLayout::~PatternLayout() = default;
	Appender::AppenderMapStorageInitializer::AppenderMapStorageInitializer() = default;
	Appender::AppenderMapStorageInitializer::~AppenderMapStorageInitializer() = default;
	CategoryStream::~CategoryStream() = default;
	Category::~Category() = default;
#pragma endregion
#pragma endregion
} // namespace log4cpp

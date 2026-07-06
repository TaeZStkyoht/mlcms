#pragma once

#include <chrono>
#include <memory>
#include <sstream>

// Forward declerations
namespace log4cpp {
	class Category;
	class CategoryStream;
}

namespace middleware {
	class Logger final {
	public:
		enum class Level : uint8_t {
			DEBUG,
			INFO,
			NOTICE,
			WARN,
			ERROR,
			CRIT,
			ALERT,
		};

	private:
		class LogStream final {
		public:
			LogStream(Logger::Level level, log4cpp::Category& category);

			~LogStream();

			template <typename T>
			const LogStream& operator<<(const T& t) const
			{
				std::stringstream s;
				s << t;
				(*this) << s;
				return *this;
			}

		private:
			std::unique_ptr<log4cpp::CategoryStream> _categoryStream;
		};

	public:
		void logEvent(Level level, std::chrono::milliseconds timestamp, const std::string& message) const;
		LogStream operator<<(Level level) const
		{
			return LogStream(level, _category);
		}

		static Logger getLoggerByCategory(const std::string& categoryName);
		static void setLogLevel(Level level);
		static void enableFileLogging(const std::string& logFileName);
		static void enableStdoutLogging();
		static void enableSysLogLogging(const std::string& syslogName);

	private:
		explicit constexpr Logger(log4cpp::Category& category) noexcept : _category(category)
		{
		}

		log4cpp::Category& _category;
	};

	template <>
	const Logger::LogStream& Logger::LogStream::operator<<(const std::stringstream& t) const;
}

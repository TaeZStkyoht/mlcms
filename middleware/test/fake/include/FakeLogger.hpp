#pragma once

#include "middleware/Logger.hpp"

#include <mutex>
#include <vector>

namespace log4cpp {
	struct Category final {
		explicit Category(std::string name) noexcept : name(std::move(name))
		{
		}

		std::string name;
	};
	struct CategoryStream {};
}

namespace middleware {
	class FakeLogger final {
		friend class Logger;

	public:
		static void Clear() noexcept
		{
			const std::lock_guard lg(_logsMutex);
			_logs.clear();
		}

		static void ClearByCategoryName(std::string_view categoryName);

		static bool CheckAnyLog(std::string_view category, Logger::Level logLevel, std::string_view message, bool exactMessage = true);
		static bool CheckLastLog(std::string_view category, Logger::Level logLevel, std::string_view message, bool exactMessage = true) noexcept;

	private:
		class LogInfo final {
			friend class FakeLogger;
			friend class Logger;

		public:
			LogInfo(log4cpp::Category* category, Logger::Level logLevel, std::chrono::milliseconds timestamp, std::string message) noexcept
				: _category(category), _logLevel(logLevel), _timestamp(timestamp), _message(std::move(message))
			{
			}

			bool Match(std::string_view category, Logger::Level logLevel, std::string_view message, bool exactMessage) const noexcept;

		private:
			log4cpp::Category* _category;
			Logger::Level _logLevel;
			std::chrono::milliseconds _timestamp;
			std::string _message;
		};

		inline static std::vector<std::unique_ptr<log4cpp::Category>> _categories;
		inline static std::vector<LogInfo> _logs;
		inline static std::mutex _categoriesMutex;
		inline static std::mutex _logsMutex;
	};
}

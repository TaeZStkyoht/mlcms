#include "MockCore.hpp"

#include "FakeLogger.hpp"

#include <gtest/gtest.h>

int fake_main(int, const char*[]);
#define main(a, b) fake_main(a, b)
#include "../../src/main.cpp"

using namespace std;

using namespace middleware;

TEST(main, CoreOkAndRaiseSignalSIGTERMAndReturnSuccess)
{
	bool mockCoreCallbackCreateCalled = false;
	MockCore::callbackCreate = [&mockCoreCallbackCreateCalled](span<const char* const>) noexcept { mockCoreCallbackCreateCalled = true; };

	bool mockCoreCallbackStartCalled = false;
	MockCore::callbackStart = [&mockCoreCallbackStartCalled] noexcept {
		mockCoreCallbackStartCalled = true;
		raise(SIGTERM);
	};

	EXPECT_EQ(fake_main({}, {}), EXIT_SUCCESS);

	EXPECT_TRUE(mockCoreCallbackCreateCalled);
	EXPECT_TRUE(mockCoreCallbackStartCalled);
	EXPECT_TRUE(FakeLogger::checkAnyLog("main", Logger::Level::INFO, "signal caught!"));
}

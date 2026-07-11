#include "MockCore.hpp"

#include "FakeLogger.hpp"

#include <gtest/gtest.h>

int fake_main();
#define main() fake_main()
#include "../../src/main.cpp"

using namespace std;

using namespace middleware;

TEST(main, CoreOkAndRaiseSignalSIGTERMAndReturnSuccess)
{
	bool mockCoreCallbackCreateCalled = false;
	MockCore::callbackCreate = [&mockCoreCallbackCreateCalled] noexcept { mockCoreCallbackCreateCalled = true; };

	bool mockCoreCallbackStartCalled = false;
	MockCore::callbackStart = [&mockCoreCallbackStartCalled] noexcept {
		mockCoreCallbackStartCalled = true;
		raise(SIGTERM);
	};

	EXPECT_EQ(fake_main(), EXIT_SUCCESS);

	EXPECT_TRUE(mockCoreCallbackCreateCalled);
	EXPECT_TRUE(mockCoreCallbackStartCalled);
	EXPECT_TRUE(FakeLogger::CheckAnyLog("main", Logger::Level::INFO, "signal caught!"));
}

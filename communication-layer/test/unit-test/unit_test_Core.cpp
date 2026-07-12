#include "Core.hpp"

#include "MockGrpcServer.hpp"

#include <gtest/gtest.h>

using namespace std;

using namespace testing;

atomic_bool run;

class CoreFixture : public Test {
private:
	void SetUp() final
	{
		run = true;

		MockGrpcServer::Clear();
	}
};

TEST_F(CoreFixture, GrpcServerCouldNotBeStarted)
{
	Core core({});
	core.Start();

	EXPECT_FALSE(run);
}

TEST_F(CoreFixture, CreateAndStart)
{
	bool mockGrpcServerCallbackStartCalled = false;
	MockGrpcServer::callbackStart = [&mockGrpcServerCallbackStartCalled] noexcept {
		mockGrpcServerCallbackStartCalled = true;
		return true;
	};

	Core core({});
	core.Start();

	EXPECT_TRUE(mockGrpcServerCallbackStartCalled);
	EXPECT_TRUE(run);
}

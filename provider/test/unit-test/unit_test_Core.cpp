#include "Core.hpp"

#include "MockGrpcClient.hpp"

#include <gtest/gtest.h>

TEST(Core, CreateAndStart)
{
	bool mockGrpcClientCallbackStartCalled = false;
	MockGrpcClient::callbackStart = [&mockGrpcClientCallbackStartCalled] noexcept { mockGrpcClientCallbackStartCalled = true; };

	Core core({});
	core.Start();

	EXPECT_TRUE(mockGrpcClientCallbackStartCalled);
}

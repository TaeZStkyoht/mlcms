#include "BaseGrpcServer.hpp"

#include "temp.grpc.pb.h"

#include "FakeLogger.hpp"

#include <gtest/gtest.h>

using namespace std;

using namespace testing;

using namespace temp;

using namespace middleware;

using enum Logger::Level;

struct TempServer final : public Temp::Service, public BaseGrpcServer {
	explicit TempServer(string address) noexcept : BaseGrpcServer(this, move(address))
	{
	}
};

class BaseGrpcServerFixture : public Test {
protected:
	static constexpr string_view logCategory = "BaseGrpcServer";

private:
	void SetUp() noexcept final
	{
		FakeLogger::Clear();
	}
};

TEST_F(BaseGrpcServerFixture, Start)
{
	{
		const string address = "127.0.0.1:50000";
		TempServer tempServer(address);
		ignore = tempServer.Start();
		EXPECT_TRUE(FakeLogger::CheckLastLog(logCategory, INFO, "Connect to " + address));
		this_thread::sleep_for(100ms);
	}
	EXPECT_TRUE(FakeLogger::CheckLastLog(logCategory, DEBUG, "Grpc server shutdown."));
}

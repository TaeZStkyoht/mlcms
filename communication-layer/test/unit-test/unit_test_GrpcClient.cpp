#include "GrpcClient.hpp"

#include "middleware/BaseGrpcServer.hpp"

#include "consumer.grpc.pb.h"

#include <gtest/gtest.h>

using namespace std;
using namespace chrono;

using namespace ::testing;

using namespace grpc;
using namespace google::protobuf;

using namespace middleware;

class GrpcServer final : public cl::Consumer::Service, public BaseGrpcServer {
public:
	explicit GrpcServer(string url) noexcept : BaseGrpcServer(this, move(url))
	{
	}

private:
	Status SendMessage(ServerContext*, const cl::OuterMessageRequest*, Empty*)
	{
		return Status::OK;
	}
};

class GrpcClientFixture : public Test {
protected:
	optional<GrpcServer> grpcServer{url};
	GrpcClient grpcClient{url};

	static constexpr uint8_t maxConsecutiveErrorCount = 10;

	inline static const string url = "127.0.0.1:50051";
};

TEST_F(GrpcClientFixture, ServerIsNotStarted)
{
	EXPECT_FALSE(grpcClient.SendMessage({}, {}));
}

TEST_F(GrpcClientFixture, ServerIsStarted)
{
	ignore = grpcServer->Start();

	EXPECT_TRUE(grpcClient.SendMessage({}, {}));
	EXPECT_TRUE(grpcClient.IsAvailable());
	EXPECT_GT(grpcClient.AverageCommunicationDuration(), 0);
}

TEST_F(GrpcClientFixture, ServerIsStartedButThenConnectionLost)
{
	ignore = grpcServer->Start();

	EXPECT_TRUE(grpcClient.SendMessage({}, {}));

	grpcServer.reset();

	EXPECT_TRUE(grpcClient.IsAvailable());

	constexpr auto waitDuration = 100ms;

	this_thread::sleep_for(waitDuration);

	EXPECT_FALSE(grpcClient.SendMessage({}, {}));

	EXPECT_LE(steady_clock::now() - grpcClient.LastTriedTime(), waitDuration);

	EXPECT_TRUE(grpcClient.IsAvailable());

	for (uint8_t i = maxConsecutiveErrorCount; i--;)
		EXPECT_FALSE(grpcClient.SendMessage({}, {}));

	EXPECT_FALSE(grpcClient.IsAvailable());
}

TEST_F(GrpcClientFixture, ServerIsStartedButThenConnectionLostAndThenStartedAgain)
{
	ignore = grpcServer->Start();

	EXPECT_TRUE(grpcClient.SendMessage({}, {}));

	grpcServer.reset();

	for (uint8_t i = maxConsecutiveErrorCount; i--;)
		EXPECT_FALSE(grpcClient.SendMessage({}, {}));

	grpcServer.emplace(url);

	ignore = grpcServer->Start();

	this_thread::sleep_for(2s);

	EXPECT_TRUE(grpcClient.SendMessage({}, {}));
	EXPECT_TRUE(grpcClient.IsAvailable());
}

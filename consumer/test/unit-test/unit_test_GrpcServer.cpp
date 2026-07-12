#include "GrpcServer.hpp"

#include "middleware/BaseGrpcClient.hpp"

#include "consumer.grpc.pb.h"

#include <gtest/gtest.h>

using namespace std;

using namespace testing;

using namespace google::protobuf;

using namespace middleware;

class GrpcClient final : public BaseGrpcClient<cl::Consumer> {
public:
	using BaseGrpcClient::BaseGrpcClient;

	bool SendMessage()
	{
		ResponseWithClientContext<Empty> clientContextWithReponse;
		return _stub->SendMessage(&clientContextWithReponse.cc, {}, &clientContextWithReponse.response).ok();
	}
};

class GrpcServerFixture : public Test {
protected:
	GrpcServer grpcServer{url};
	GrpcClient grpcClient{url};

private:
	inline static const string url = "127.0.0.1:50051";
};

TEST_F(GrpcServerFixture, GrpcServerIsNotStarted)
{
	EXPECT_FALSE(grpcClient.SendMessage());
}

TEST_F(GrpcServerFixture, GrpcServerIsStarted)
{
	grpcServer.Start();

	EXPECT_TRUE(grpcClient.SendMessage());
}

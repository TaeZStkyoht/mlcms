#include "GrpcServer.hpp"

#include "middleware/BaseGrpcClient.hpp"

#include "consumer.grpc.pb.h"

#include <gtest/gtest.h>

using namespace std;

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

static const string url = "127.0.0.1:50051";

TEST(GrpcServer, GrpcServerIsNotStarted)
{
	GrpcClient grpcClient(url);
	EXPECT_FALSE(grpcClient.SendMessage());
}

TEST(GrpcServer, GrpcServerIsStarted)
{
	GrpcServer grpcServer(url);
	grpcServer.Start();

	GrpcClient grpcClient(url);
	EXPECT_TRUE(grpcClient.SendMessage());
}

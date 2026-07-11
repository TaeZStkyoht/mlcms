#include "GrpcClient.hpp"

#include "middleware/BaseGrpcServer.hpp"

#include "communication-layer.grpc.pb.h"

#include <gtest/gtest.h>

using namespace std;
using namespace chrono;

using namespace ::testing;

using namespace grpc;
using namespace google::protobuf;

using namespace middleware;

static vector<cl::MessageRequest> messageRequests;

class GrpcServer final : public cl::CommunicationLayer::Service, public BaseGrpcServer {
public:
	explicit GrpcServer(string url) noexcept : BaseGrpcServer(this, move(url))
	{
	}

private:
	Status SendMessage(ServerContext*, ServerReader<cl::MessageRequest>* reader, Empty*) override
	{
		for (cl::MessageRequest messageRequest; reader->Read(&messageRequest);)
			messageRequests.push_back(move(messageRequest));

		return Status::OK;
	}
};

class GrpcClientFixture : public Test {
protected:
	optional<GrpcServer> grpcServer{url};
	optional<GrpcClient> grpcClient{url};

private:
	void SetUp() noexcept final
	{
		messageRequests.clear();
	}

	inline static const string url = "127.0.0.1:50050";
};

TEST_F(GrpcClientFixture, ServerIsNotStarted)
{
	grpcClient->Start();

	this_thread::sleep_for(100ms);

	grpcClient.reset();

	EXPECT_TRUE(messageRequests.empty());
}

TEST_F(GrpcClientFixture, ServerIsStarted)
{
	ignore = grpcServer->Start();
	grpcClient->Start();

	this_thread::sleep_for(100ms);

	grpcClient.reset();
	grpcServer.reset();

	EXPECT_FALSE(messageRequests.empty());
}

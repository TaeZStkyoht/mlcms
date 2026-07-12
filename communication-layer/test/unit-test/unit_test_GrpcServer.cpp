#include "GrpcServer.hpp"

#include "MockMessageRequestPusher.hpp"

#include "FakeLogger.hpp"

#include "middleware/BaseGrpcClient.hpp"

#include "communication-layer.grpc.pb.h"

using namespace std;

using namespace testing;

using namespace google::protobuf;

using namespace middleware;

using enum Logger::Level;

atomic_bool run;

class GrpcClient final : public BaseGrpcClient<cl::CommunicationLayer> {
public:
	using BaseGrpcClient::BaseGrpcClient;

	bool Send() const
	{
		ResponseWithClientContext<Empty> clientContextWithReponse;
		if (const auto clientReadWriter = _stub->SendMessage(&clientContextWithReponse.cc, &clientContextWithReponse.response); clientReadWriter)
			return clientReadWriter->Write({});
		return {};
	}
};

class GrpcServerFixture : public Test {
protected:
	shared_ptr<MockMessageRequestPusher> messageRequestPusher = make_shared<MockMessageRequestPusher>();
	GrpcServer grpcServer{url, messageRequestPusher};
	GrpcClient grpcClient{url};

	static constexpr string_view logCategory = "GrpcServer";

private:
	void SetUp() noexcept final
	{
		run = true;

		FakeLogger::Clear();
	}

	inline static const string url = "127.0.0.1:50050";
};

TEST_F(GrpcServerFixture, ServerIsNotStarted)
{
	EXPECT_FALSE(grpcClient.Send());

	EXPECT_FALSE(FakeLogger::CheckAnyLog(logCategory, DEBUG, ") connected and allowed to stream", false));
}

TEST_F(GrpcServerFixture, ServerIsStarted)
{
	EXPECT_CALL(*messageRequestPusher, Push(_)).WillRepeatedly(Return());

	grpcServer.Start();

	EXPECT_TRUE(grpcClient.Send());

	this_thread::sleep_for(100ms);

	EXPECT_TRUE(FakeLogger::CheckAnyLog(logCategory, DEBUG, ") connected and allowed to stream", false));
}

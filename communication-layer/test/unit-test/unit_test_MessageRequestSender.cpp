#include "MessageRequestSender.hpp"

#include "MockGrpcClient.hpp"

#include "MockMessageRequestPuller.hpp"

using namespace std;
using namespace chrono;

using namespace testing;

atomic_bool run;

class MessageRequestSenderFixture : public Test {
protected:
	shared_ptr<MockMessageRequestPuller> messageRequestPuller = make_shared<MockMessageRequestPuller>();
	shared_ptr<GrpcClient> grpcClient1 = make_shared<GrpcClient>(url);
	shared_ptr<GrpcClient> grpcClient2 = make_shared<GrpcClient>(url);
	optional<MessageRequestSender> messageRequestSender = make_optional<MessageRequestSender>(messageRequestPuller, vector{grpcClient1, grpcClient2});

private:
	void SetUp() noexcept final
	{
		run = true;

		MockGrpcClient::Clear();
	}

	inline static const string url = "127.0.0.1:50051";
};

TEST_F(MessageRequestSenderFixture, Some)
{
	EXPECT_CALL(*messageRequestPuller, Pull()).WillRepeatedly([] -> entity::MessageRequest {
		this_thread::sleep_for(1ms);
		return {};
	});

	// Start with unavailable clients
	bool mockGrpcClientCallbackSendMessageCalled = false;
	MockGrpcClient::callbackSendMessage = [&mockGrpcClientCallbackSendMessageCalled](const string&, system_clock::time_point) noexcept {
		mockGrpcClientCallbackSendMessageCalled = true;
		return false;
	};

	bool mockGrpcClientCallbackIsAvailableCalled = false;
	MockGrpcClient::callbackIsAvailable = [&mockGrpcClientCallbackIsAvailableCalled] noexcept {
		mockGrpcClientCallbackIsAvailableCalled = true;
		return false;
	};

	bool mockGrpcClientCallbackAverageCommunicationDurationCalled = false;
	MockGrpcClient::callbackAverageCommunicationDuration = [&mockGrpcClientCallbackAverageCommunicationDurationCalled] noexcept -> uint32_t {
		mockGrpcClientCallbackAverageCommunicationDurationCalled = true;
		return {};
	};

	messageRequestSender->Start();

	this_thread::sleep_for(5s);

	// Then with available clients
	MockGrpcClient::callbackSendMessage = [&mockGrpcClientCallbackSendMessageCalled](const string&, system_clock::time_point) noexcept {
		mockGrpcClientCallbackSendMessageCalled = true;
		MockGrpcClient::callbackLastTriedTime = [] { return steady_clock::now(); };
		return true;
	};

	MockGrpcClient::callbackIsAvailable = [&mockGrpcClientCallbackIsAvailableCalled] noexcept {
		mockGrpcClientCallbackIsAvailableCalled = true;
		return true;
	};

	MockGrpcClient::callbackAverageCommunicationDuration = [&mockGrpcClientCallbackAverageCommunicationDurationCalled] noexcept {
		mockGrpcClientCallbackAverageCommunicationDurationCalled = true;
		return 58;
	};

	this_thread::sleep_for(5s);

	run = false;
	messageRequestSender.reset();

	EXPECT_TRUE(mockGrpcClientCallbackSendMessageCalled);
	EXPECT_TRUE(mockGrpcClientCallbackIsAvailableCalled);
	EXPECT_TRUE(mockGrpcClientCallbackAverageCommunicationDurationCalled);
}

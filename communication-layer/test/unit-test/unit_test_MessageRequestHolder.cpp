#include "MessageRequestHolder.hpp"

#include "FakeLogger.hpp"

#include <gtest/gtest.h>

using namespace std;

using namespace testing;

using namespace middleware;

using enum Logger::Level;

class MessageRequestHolderFixture : public Test {
protected:
	MessageRequestHolder messageRequestHolder{{}};

	static constexpr string_view logCategory = "MessageRequestHolder";

	static constexpr entity::MessageRequest CreateMessageRequest(entity::MessageRequest::Priority priority) noexcept
	{
		entity::MessageRequest messageRequest;
		messageRequest.priority = priority;
		return messageRequest;
	}

private:
	void SetUp() noexcept final
	{
		FakeLogger::Clear();
	}
};

TEST_F(MessageRequestHolderFixture, FillAndEmptyQueue)
{
	MessageRequestPusher& messageRequestPusher = messageRequestHolder;
	MessageRequestPuller& messageRequestPuller = messageRequestHolder;
	MessageRequestObserver& messageRequestObserver = messageRequestHolder;

	using enum entity::MessageRequest::Priority;

	messageRequestPusher.Push(CreateMessageRequest(NONE));
	messageRequestPusher.Push(CreateMessageRequest(LOW));
	messageRequestPusher.Push(CreateMessageRequest(NORMAL));
	messageRequestPusher.Push(CreateMessageRequest(HIGH));
	messageRequestPusher.Push(CreateMessageRequest(CRITICAL));

	{
		const auto messageRequest = messageRequestPuller.Pull();
		ASSERT_TRUE(messageRequest);
		EXPECT_EQ(messageRequest->priority, CRITICAL);
	}

	{
		const auto messageRequest = messageRequestPuller.Pull();
		ASSERT_TRUE(messageRequest);
		EXPECT_EQ(messageRequest->priority, HIGH);
	}

	{
		const auto messageRequest = messageRequestPuller.Pull();
		ASSERT_TRUE(messageRequest);
		EXPECT_EQ(messageRequest->priority, NORMAL);
	}

	{
		const auto messageRequest = messageRequestPuller.Pull();
		ASSERT_TRUE(messageRequest);
		EXPECT_EQ(messageRequest->priority, LOW);
	}

	{
		const auto messageRequest = messageRequestPuller.Pull();
		ASSERT_TRUE(messageRequest);
		EXPECT_EQ(messageRequest->priority, LOW);
	}

	EXPECT_FALSE(messageRequestPuller.Pull());

	EXPECT_TRUE(FakeLogger::CheckAnyLog(logCategory, WARN, "Unexpected priority has come. Set to LOW. Original value: ", false));
}

#include "Observer.hpp"

#include "MockAvailabilityProvider.hpp"
#include "MockMessageRequestObserver.hpp"

#include "FakeLogger.hpp"

using namespace std;

using namespace testing;

using namespace middleware;

using enum Logger::Level;

class ObserverFixture : public Test {
protected:
	shared_ptr<MockMessageRequestObserver> messageRequestObserver = make_shared<MockMessageRequestObserver>();
	shared_ptr<MockAvailabilityProvider> availabilityProvider = make_shared<MockAvailabilityProvider>();
	Observer observer{messageRequestObserver, {availabilityProvider}};

	static constexpr string_view logCategory = "Observer";

private:
	void SetUp() noexcept final
	{
		FakeLogger::Clear();
	}
};

TEST_F(ObserverFixture, ObserverMetrics)
{
	EXPECT_CALL(*messageRequestObserver, Size()).WillRepeatedly(Return(QueueSize()));
	EXPECT_CALL(*availabilityProvider, IsAvailable()).WillRepeatedly(Return(true));
	EXPECT_CALL(*availabilityProvider, AverageCommunicationDuration()).WillRepeatedly(Return(58));

	observer.Start();

	this_thread::sleep_for(100ms);

	EXPECT_TRUE(FakeLogger::CheckAnyLog(logCategory, DEBUG, "Queue sizes (crit, high, normal, low): ", false));
	EXPECT_TRUE(FakeLogger::CheckAnyLog(logCategory, DEBUG, "Channel Average Communication: ", false));
}

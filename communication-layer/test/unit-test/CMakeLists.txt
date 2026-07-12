set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin/test_binary)

find_package(GTest REQUIRED)

link_libraries(
	GTest::gtest_main
	FakeLogger
	MockSleepWrapper
)

add_executable(${PROJECT_NAME}-unit_test_main unit_test_main.cpp
	$<TARGET_OBJECTS:${PROJECT_NAME}-MockCore>
	$<TARGET_OBJECTS:${PROJECT_NAME}-MockGrpcServer>
)

add_executable(${PROJECT_NAME}-unit_test_Core unit_test_Core.cpp
	$<TARGET_OBJECTS:${PROJECT_NAME}-Core>
	$<TARGET_OBJECTS:${PROJECT_NAME}-MockGrpcServer>
	$<TARGET_OBJECTS:${PROJECT_NAME}-MockMessageRequestSender>
	$<TARGET_OBJECTS:${PROJECT_NAME}-MockGrpcClient>
	$<TARGET_OBJECTS:${PROJECT_NAME}-StubMessageRequestHolder>
	$<TARGET_OBJECTS:${PROJECT_NAME}-MockObserver>
)

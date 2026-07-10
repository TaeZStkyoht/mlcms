set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin/test_binary)

find_package(GTest REQUIRED)

link_libraries(
	GTest::gtest
	GTest::gtest_main
	FakeLogger
)

add_executable(${PROJECT_NAME}-unit_test_Core unit_test_Core.cpp
	$<TARGET_OBJECTS:${PROJECT_NAME}-Core>
	$<TARGET_OBJECTS:${PROJECT_NAME}-MockGrpcClient>
)

add_executable(${PROJECT_NAME}-unit_test_main unit_test_main.cpp
	$<TARGET_OBJECTS:${PROJECT_NAME}-MockCore>
	$<TARGET_OBJECTS:${PROJECT_NAME}-MockGrpcClient>
)

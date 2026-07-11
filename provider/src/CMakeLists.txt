add_library(${PROJECT_NAME}-GrpcClient OBJECT GrpcClient.cpp)
add_library(${PROJECT_NAME}-Core OBJECT Core.cpp)

add_dependencies(${PROJECT_NAME}-GrpcClient communication-layer-proto)

add_executable(${PROJECT_NAME} main.cpp
	$<TARGET_OBJECTS:${PROJECT_NAME}-Core>
	$<TARGET_OBJECTS:${PROJECT_NAME}-GrpcClient>
	$<TARGET_OBJECTS:SleepWrapper>
	$<TARGET_OBJECTS:communication-layer-proto>
)
target_link_libraries(${PROJECT_NAME}
	gRPC::grpc++_unsecure
	Logger
	log4cpp
)

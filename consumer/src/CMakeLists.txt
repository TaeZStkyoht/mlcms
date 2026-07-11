add_library(${PROJECT_NAME}-GrpcServer OBJECT GrpcServer.cpp)
add_library(${PROJECT_NAME}-Core OBJECT Core.cpp)

add_dependencies(${PROJECT_NAME}-GrpcServer consumer-proto)

add_executable(${PROJECT_NAME} main.cpp
	$<TARGET_OBJECTS:${PROJECT_NAME}-Core>
	$<TARGET_OBJECTS:${PROJECT_NAME}-GrpcServer>
	$<TARGET_OBJECTS:BaseGrpcServer>
	$<TARGET_OBJECTS:Logger>
	$<TARGET_OBJECTS:SleepWrapper>
	$<TARGET_OBJECTS:consumer-proto>
)
target_link_libraries(${PROJECT_NAME}
	gRPC::grpc++_unsecure
	log4cpp
)

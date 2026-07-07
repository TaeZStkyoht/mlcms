add_library(${PROJECT_NAME}-MessageRequestHolder OBJECT MessageRequestHolder.cpp)
add_library(${PROJECT_NAME}-GrpcServer OBJECT GrpcServer.cpp)
add_library(${PROJECT_NAME}-GrpcClient OBJECT GrpcClient.cpp)
add_library(${PROJECT_NAME}-MessageRequestSender OBJECT MessageRequestSender.cpp)
add_library(${PROJECT_NAME}-Observer OBJECT Observer.cpp)
add_library(${PROJECT_NAME}-Core OBJECT Core.cpp)

add_dependencies(${PROJECT_NAME}-GrpcServer communication-layer-proto)
add_dependencies(${PROJECT_NAME}-GrpcClient consumer-proto)

add_executable(${PROJECT_NAME} main.cpp
	$<TARGET_OBJECTS:${PROJECT_NAME}-Core>
	$<TARGET_OBJECTS:${PROJECT_NAME}-Observer>
	$<TARGET_OBJECTS:${PROJECT_NAME}-MessageRequestSender>
	$<TARGET_OBJECTS:${PROJECT_NAME}-GrpcClient>
	$<TARGET_OBJECTS:${PROJECT_NAME}-GrpcServer>
	$<TARGET_OBJECTS:${PROJECT_NAME}-MessageRequestHolder>
	$<TARGET_OBJECTS:BaseGrpcServer>
	$<TARGET_OBJECTS:Logger>
	$<TARGET_OBJECTS:communication-layer-proto>
	$<TARGET_OBJECTS:consumer-proto>
)
target_link_libraries(${PROJECT_NAME}
	gRPC::grpc++_unsecure
	log4cpp
)

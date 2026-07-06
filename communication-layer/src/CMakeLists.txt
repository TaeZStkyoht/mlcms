add_protobuf(communication-layer)

add_library(MessageRequestHolder OBJECT MessageRequestHolder.cpp)
add_library(GrpcServer OBJECT GrpcServer.cpp)
add_library(MessageRequestSender OBJECT MessageRequestSender.cpp)
add_library(Core OBJECT Core.cpp)

add_dependencies(GrpcServer communication-layer-proto)

add_executable(${PROJECT_NAME} main.cpp
	$<TARGET_OBJECTS:Core>
	$<TARGET_OBJECTS:MessageRequestSender>
	$<TARGET_OBJECTS:GrpcServer>
	$<TARGET_OBJECTS:BaseGrpcServer>
	$<TARGET_OBJECTS:MessageRequestHolder>
	$<TARGET_OBJECTS:Logger>
	$<TARGET_OBJECTS:communication-layer-proto>
)
target_link_libraries(${PROJECT_NAME}
	gRPC::grpc++_unsecure
	log4cpp
)

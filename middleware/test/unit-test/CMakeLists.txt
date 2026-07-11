set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin/test_binary)

find_package(GTest REQUIRED)

link_libraries(
	GTest::gtest_main
)

add_executable(unit_test_Logger unit_test_Logger.cpp
	$<TARGET_OBJECTS:Logger>
	$<TARGET_OBJECTS:FakeLog4Cpp>
)

add_executable(unit_test_SleepWrapper unit_test_SleepWrapper.cpp
	$<TARGET_OBJECTS:SleepWrapper>
)

function(add_protobuf2 NAMEPATH)
    get_filename_component(NAME ${NAMEPATH} NAME)
    get_filename_component(PATH ${NAMEPATH} DIRECTORY)
    add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/${NAME}.pb.cc ${CMAKE_BINARY_DIR}/${NAME}.pb.h
                   COMMAND  protoc -I${CMAKE_CURRENT_SOURCE_DIR}/protobufs --cpp_out=${CMAKE_BINARY_DIR}
                   --proto_path=${CMAKE_CURRENT_SOURCE_DIR}/protobufs ${CMAKE_CURRENT_SOURCE_DIR}/protobufs/${NAME}.proto)
    execute_process(COMMAND grep -lR "^\\s*service .*{" ${CMAKE_CURRENT_SOURCE_DIR}/protobufs/${NAME}.proto
                    RESULT_VARIABLE IS_GRPC
                    OUTPUT_QUIET)
    if (IS_GRPC STREQUAL "0")
        add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/${NAME}.grpc.pb.cc ${CMAKE_BINARY_DIR}/${NAME}.grpc.pb.h
                   COMMAND  protoc -I${CMAKE_CURRENT_SOURCE_DIR}/protobufs --grpc_out=generate_mock_code=true:${CMAKE_BINARY_DIR} --plugin=protoc-gen-grpc=${GRPC_CPP_PLUGIN} ${CMAKE_CURRENT_SOURCE_DIR}/protobufs/${NAME}.proto)
        set_source_files_properties(${CMAKE_BINARY_DIR}/${NAME}.grpc.pb.cc PROPERTIES GENERATED true)
        set_source_files_properties(${CMAKE_BINARY_DIR}/${NAME}.grpc.pb.h  PROPERTIES GENERATED true)
        set(${NAME}_GRPC ${CMAKE_BINARY_DIR}/${NAME}.grpc.pb.cc)
    else ()
        set(${NAME}_GRPC "")
    endif()
    set_source_files_properties(${CMAKE_BINARY_DIR}/${NAME}.pb.cc PROPERTIES GENERATED true)
    set_source_files_properties(${CMAKE_BINARY_DIR}/${NAME}.pb.h  PROPERTIES GENERATED true)
    add_library(${NAME}-proto      OBJECT   ${CMAKE_BINARY_DIR}/${NAME}.pb.cc  ${${NAME}_GRPC}      )

    target_compile_options(${NAME}-proto PUBLIC -w)
endfunction()

add_protobuf2(temp)

add_library(unit_test_BaseGrpcServer_Object OBJECT unit_test_BaseGrpcServer.cpp)
target_include_directories(unit_test_BaseGrpcServer_Object PRIVATE
	${CMAKE_BINARY_DIR}
)

add_dependencies(unit_test_BaseGrpcServer_Object temp-proto)

add_executable(unit_test_BaseGrpcServer
	$<TARGET_OBJECTS:unit_test_BaseGrpcServer_Object>
	$<TARGET_OBJECTS:BaseGrpcServer>
	$<TARGET_OBJECTS:temp-proto>
)
target_link_libraries(unit_test_BaseGrpcServer
	FakeLogger
	gRPC::grpc++_unsecure
)

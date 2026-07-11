cmake_minimum_required(VERSION 3.20)

project(mlcms)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "Debug")
endif()

message(STATUS "Build Type: " ${CMAKE_BUILD_TYPE})

execute_process(
	COMMAND sh -c "git describe --always --tags --long --dirty"
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
	OUTPUT_VARIABLE PROGRAM_VERSION
	OUTPUT_STRIP_TRAILING_WHITESPACE
)

message(STATUS "Building ${CMAKE_PROJECT_NAME} ${PROGRAM_VERSION}")

add_compile_definitions(VERSION_INFO="${PROGRAM_VERSION}")
add_compile_definitions(CMAKE_PROJECT_NAME="${CMAKE_PROJECT_NAME}")

if(NOT EXISTS ${PROJECT_SOURCE_DIR}/.git/hooks/pre-commit)
	message(STATUS "Activating pre-commit git hook...")
	execute_process(COMMAND ln -s -f ../../.hooks/pre-commit .git/hooks/pre-commit WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -Wconversion \
					-Wold-style-cast \
					-Wswitch-default \
					-Wduplicated-cond \
					-Wnull-dereference \
					-Wno-unknown-pragmas \
")

if(PLATFORM MATCHES "x86")
	set(CMAKE_CXX_FLAGS_DEBUG "-ggdb --coverage")
endif()

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

set(CMAKE_MODULE_PATH cmake-modules)

find_package(gRPC CONFIG REQUIRED)
find_program(GRPC_CPP_PLUGIN grpc_cpp_plugin REQUIRED)

function(add_protobuf NAMEPATH)
    get_filename_component(NAME ${NAMEPATH} NAME)
    get_filename_component(PATH ${NAMEPATH} DIRECTORY)
    add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/${NAME}.pb.cc ${CMAKE_BINARY_DIR}/${NAME}.pb.h
                   COMMAND  protoc -I${CMAKE_SOURCE_DIR}/protobufs --cpp_out=${CMAKE_BINARY_DIR}
                   --proto_path=${CMAKE_SOURCE_DIR}/protobufs ${CMAKE_SOURCE_DIR}/protobufs/${NAME}.proto)
    execute_process(COMMAND grep -lR "^\\s*service .*{" ${CMAKE_SOURCE_DIR}/protobufs/${NAME}.proto
                    RESULT_VARIABLE IS_GRPC
                    OUTPUT_QUIET)
    if (IS_GRPC STREQUAL "0")
        add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/${NAME}.grpc.pb.cc ${CMAKE_BINARY_DIR}/${NAME}.grpc.pb.h
                   COMMAND  protoc -I${CMAKE_SOURCE_DIR}/protobufs --grpc_out=generate_mock_code=true:${CMAKE_BINARY_DIR} --plugin=protoc-gen-grpc=${GRPC_CPP_PLUGIN} ${CMAKE_SOURCE_DIR}/protobufs/${NAME}.proto)
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

add_subdirectory(middleware)

include_directories(
	middleware/include
)
include_directories(${CMAKE_BINARY_DIR}) # For Proto

add_protobuf(communication-layer)
add_protobuf(consumer)

add_subdirectory(provider)
add_subdirectory(communication-layer)
add_subdirectory(consumer)

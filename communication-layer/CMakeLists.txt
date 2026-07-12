project(${CMAKE_PROJECT_NAME}-communication-layer)

add_compile_definitions(PROJECT_NAME="${PROJECT_NAME}")

include_directories(include)

add_subdirectory(src)
add_subdirectory(test)

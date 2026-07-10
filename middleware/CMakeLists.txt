project(middleware)

add_compile_definitions(PROJECT_NAME="${PROJECT_NAME}")

include_directories(
	include
	include/middleware
	internal-include
)

add_subdirectory(src)
add_subdirectory(test)

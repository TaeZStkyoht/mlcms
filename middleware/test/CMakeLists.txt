include_directories(fake/include)
include_directories(fake/internal-include)
include_directories(mock/include)

add_subdirectory(fake)
add_subdirectory(mock)
add_subdirectory(unit-test)

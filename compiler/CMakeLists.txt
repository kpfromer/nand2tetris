project (JackCompiler)

cmake_minimum_required (VERSION 3.14 FATAL_ERROR)
set (CMAKE_CXX_STANDARD 11)

# Include source files
file(GLOB SRC_FILES
    "src/*.h"
    "src/*.cpp"
)
add_executable (JackCompiler ${SRC_FILES})

# Include Boost Library
find_package(Boost 1.69 REQUIRED COMPONENTS filesystem system)
include_directories(${Boost_INCLUDE_DIRS})
target_link_libraries(JackCompiler ${Boost_LIBRARIES})
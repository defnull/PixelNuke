project(PixelNuke)

cmake_minimum_required(VERSION 2.8)

list(APPEND CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall")
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -pg")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -pg")


file(GLOB SOURCE_FILES "*.h" "*.cpp" Makefile "dep/lodepng/lodepng.*")
add_executable(pixelnuke ${SOURCE_FILES})

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)

set(GLFW_BUILD_EXAMPLES OFF)
set(GLFW_BUILD_TESTS OFF)
add_subdirectory(dep/glfw)
include_directories(dep/glfw/include)

#set(EVENT__DISABLE_BENCHMARK ON)
#set(EVENT__DISABLE_TESTS ON)
#set(EVENT__DISABLE_REGRESS ON)
#set(EVENT__DISABLE_SAMPLES ON)
#add_subdirectory(dep/libevent)
#include_directories(dep/libevent/include)

target_link_libraries(pixelnuke glfw GLEW event event_pthreads ${GLFW_LIBRARIES})


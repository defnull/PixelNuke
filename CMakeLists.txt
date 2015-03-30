project(PixelNuke)

cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

file(GLOB SOURCE_FILES "*.h" "*.cpp" Makefile "dep/lodepng/lodepng.*")
add_executable(PixelNuke ${SOURCE_FILES})

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)

add_subdirectory(dep/glfw)
include_directories(dep/glfw/include)

find_package(LibEvent REQUIRED)
	
#add_subdirectory(dep/libevent)
#include_directories(dep/libevent/include)

target_link_libraries(PixelNuke glfw GLEW event event_pthreads ${GLFW_LIBRARIES})


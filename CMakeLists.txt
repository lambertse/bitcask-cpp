
project(bitcask_cpp)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

cmake_minimum_required(VERSION 3.0)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CRC32C_BUILD_TESTS OFF)
set(CRC32C_BUILD_TESTS OFF)
set(CRC32C_USE_GLOG OFF)
set(CRC32C_INSTALL OFF)

# Set up third-party dependencies
include(FetchContent)
FetchContent_Declare(
  crc32c
  GIT_REPOSITORY https://github.com/google/crc32c.git
  GIT_TAG        main
  )
FetchContent_MakeAvailable(crc32c)
#
FetchContent_Declare(
  googletest
    URL https://github.com/google/googletest/archive/refs/tags/release-1.11.0.zip
  )
FetchContent_MakeAvailable(googletest)
include_directories(${googletest_SOURCE_DIR}/googletest/include/)

# Include all header files in the include directory
file(GLOB HEADER_FILES "${CMAKE_CURRENT_SOURCE_DIR}/include/*/*.hpp")

# Include all source files in the src directory
file(GLOB SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
# Add all source files to the library
add_library(${PROJECT_NAME} ${SOURCE_FILES} ${HEADER_FILES})
target_link_libraries(${PROJECT_NAME} PRIVATE crc32c)

add_subdirectory(test)



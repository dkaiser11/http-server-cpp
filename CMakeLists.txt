cmake_minimum_required(VERSION 3.14)

project(http-server-cpp)

# Set C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find source files (excluding main.cpp for library)
file(GLOB_RECURSE LIB_SOURCES src/*.cpp src/*.hpp)
list(REMOVE_ITEM LIB_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)

# Create a library from non-main sources (if any)
if(LIB_SOURCES)
    add_library(http_server_lib ${LIB_SOURCES})
    target_include_directories(http_server_lib PUBLIC "src")
endif()

# Main executable
add_executable(server src/main.cpp)
target_include_directories(server PRIVATE "src")

# Link library to executable if it exists
if(LIB_SOURCES)
    target_link_libraries(server http_server_lib)
endif()

# Enable testing
enable_testing()

# Fetch Google Test
include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        v1.14.0
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Find test files
file(GLOB_RECURSE TEST_SOURCES tests/*.cpp tests/*.hpp)

# Create test executable if test files exist
if(TEST_SOURCES)
    add_executable(tests ${TEST_SOURCES})
    target_link_libraries(tests gtest_main gmock_main)
    target_include_directories(tests PRIVATE "src")
    
    # Link with the library if it exists
    if(LIB_SOURCES)
        target_link_libraries(tests http_server_lib)
    endif()
    
    # Register tests with CTest
    include(GoogleTest)
    gtest_discover_tests(tests)
endif()
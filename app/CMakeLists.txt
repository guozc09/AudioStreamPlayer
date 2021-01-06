cmake_minimum_required(VERSION 3.10)

message(STATUS "***********************************************************")
message(STATUS "CMAKE AudioStreamPlayerApp")
message(STATUS "***********************************************************")

include_directories(${PROJECT_SOURCE_DIR}/sdk/include)

link_directories(${PROJECT_BINARY_DIR}/lib)

# add the executable
add_executable(AudioStreamPlayerApp
    ASPLogImpl.cpp
    main.cpp)

target_link_libraries(AudioStreamPlayerApp libAudioStreamPlayerSdk.a)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 添加对gdb的支持
set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")
set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

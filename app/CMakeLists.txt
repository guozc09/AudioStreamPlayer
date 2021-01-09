cmake_minimum_required(VERSION 3.10)

message(STATUS "***********************************************************")
message(STATUS "CMAKE AudioStreamPlayerApp")
message(STATUS "***********************************************************")

set(3RD_INCLUDE_PATH ${PROJECT_SOURCE_DIR}/3rd/FFmpeg/include
                     ${PROJECT_SOURCE_DIR}/3rd/apache-log4cxx-0.11.0/include)
set(3RD_LINK_PATH ${PROJECT_SOURCE_DIR}/3rd/FFmpeg/lib
                  ${PROJECT_SOURCE_DIR}/3rd/apache-log4cxx-0.11.0/lib)

include_directories(${PROJECT_SOURCE_DIR}/sdk/include ${3RD_INCLUDE_PATH})
link_directories(${PROJECT_BINARY_DIR}/lib ${3RD_LINK_PATH})

# add the executable
aux_source_directory(${PROJECT_SOURCE_DIR}/app MAIN_SRC)
add_executable(AudioStreamPlayerApp ${MAIN_SRC})

target_link_libraries(AudioStreamPlayerApp libAudioStreamPlayerSdk.a)
set(MFFMPEG_LIBS avutil swresample)
target_link_libraries(AudioStreamPlayerApp ${MFFMPEG_LIBS} log4cxx)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 添加对gdb的支持
set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")
set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

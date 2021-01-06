cmake_minimum_required(VERSION 3.10)

message(STATUS "***********************************************************")
message(STATUS "CMAKE AudioStreamPlayerSdk")
message(STATUS "***********************************************************")

set(FFmpegInclude ${PROJECT_SOURCE_DIR}/3rd/FFmpeg/include)
set(LINK_PATH ${PROJECT_SOURCE_DIR}/3rd/FFmpeg/lib)

include_directories(${PROJECT_SOURCE_DIR}/sdk/include ${FFmpegInclude})
link_directories(${LINK_PATH})

set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)
aux_source_directory(${PROJECT_SOURCE_DIR}/sdk/src STATIC_SRC)
add_library(AudioStreamPlayerSdk STATIC ${STATIC_SRC})

set(mFFmpeg avutil swresample)
target_link_libraries(AudioStreamPlayerSdk ${mFFmpeg})

# specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 添加对gdb的支持
set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")
set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

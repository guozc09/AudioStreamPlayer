cmake_minimum_required(VERSION 3.10)

message(STATUS "***********************************************************")
message(STATUS "CMAKE AudioStreamPlayerSdk")
message(STATUS "***********************************************************")

set(3RD_INCLUDE_PATH ${PROJECT_SOURCE_DIR}/3rd/FFmpeg/include
                     ${PROJECT_SOURCE_DIR}/3rd/apache-log4cxx-0.11.0/include)
set(3RD_LINK_PATH ${PROJECT_SOURCE_DIR}/3rd/FFmpeg/lib
                  ${PROJECT_SOURCE_DIR}/3rd/apache-log4cxx-0.11.0/lib)

include_directories(${PROJECT_SOURCE_DIR}/sdk/include ${3RD_INCLUDE_PATH})
link_directories(${3RD_LINK_PATH})

set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)
aux_source_directory(${PROJECT_SOURCE_DIR}/sdk/src STATIC_SRC)
add_library(AudioStreamPlayerSdk STATIC ${STATIC_SRC})

set(MFFMPEG_LIBS avutil swresample)
target_link_libraries(AudioStreamPlayerSdk ${MFFMPEG_LIBS} log4cxx)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 添加对gdb的支持
set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")
set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

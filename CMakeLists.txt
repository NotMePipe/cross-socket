cmake_minimum_required(VERSION 3.10)
project(CrossSocket)

# Require C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Allow user to choose shared or static build (default: shared)
option(BUILD_SHARED_LIBS "Build shared libraries instead of static ones" ON)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

set(SOURCES
    src/Socket.cpp
    src/CrossSocketUtils.cpp
    src/SocketManager.cpp
)

add_library(CrossSocket ${SOURCES})

if(WIN32)
    target_link_libraries(CrossSocket ws2_32)
endif()

target_include_directories(CrossSocket PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

set_target_properties(CrossSocket PROPERTIES VERSION 2.0 SOVERSION 1)

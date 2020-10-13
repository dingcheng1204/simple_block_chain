cmake_minimum_required(VERSION 3.1)
PROJECT(block_chain_app)


SET(CMAKE_BUILD_TYPE "Debug")
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

SET(CMAKE_CXX_STANDARD 14)
SET(CMAKE_CXX_FLAGS -pthread)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)
SET(CMAKE_CXX_EXTENSIONS OFF)
add_executable(block_chain_app main.cpp)

set(Boost_USE_STATIC_LIBS OFF) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF) 
find_package(Boost REQUIRED COMPONENTS
regex
filesystem
system
)

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS}) 
    target_link_libraries(block_chain_app ${Boost_LIBRARIES})
endif()
#include_directories($(CMAKE_SOURCE_DIR))


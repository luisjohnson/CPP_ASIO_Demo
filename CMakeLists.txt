cmake_minimum_required(VERSION 3.21)
project(CPP_ASIO_Demo)

set(CMAKE_CXX_STANDARD 17)

find_package(Boost COMPONENTS thread chrono REQUIRED)

add_executable(${PROJECT_NAME} src/main.cpp)

target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})
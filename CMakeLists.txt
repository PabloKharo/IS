cmake_minimum_required(VERSION 3.10)

project(IS)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(search)
include_directories(search/search_engine)

find_package(cpprestsdk REQUIRED)

add_executable(IS
    search/main.cpp
    search/search_engine/save_data.cpp
    search/search_engine/load_data.cpp
    search/search_engine/search.cpp
    search/search_engine/index.cpp
)

target_link_libraries(IS PRIVATE cpprestsdk::cpprest)

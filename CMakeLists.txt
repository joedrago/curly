cmake_minimum_required(VERSION 3.5)

project(curly)

set(CURL_LIBRARY "-lcurl") 
find_package(CURL REQUIRED) 
add_executable (curly main.cpp)
include_directories(${CURL_INCLUDE_DIR})
target_link_libraries(curly ${CURL_LIBRARIES})


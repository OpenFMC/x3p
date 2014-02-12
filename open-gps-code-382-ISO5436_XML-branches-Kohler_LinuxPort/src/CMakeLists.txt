project(ISO5436_2)

cmake_minimum_required(VERSION 2.8)


 
 
add_subdirectory(zlib) 
include_directories(${CMAKE_CURRENT_BINARY_DIR}/zlib)
add_subdirectory(zlib/contrib/minizip) 
add_subdirectory(ISO5436_2_XML) 
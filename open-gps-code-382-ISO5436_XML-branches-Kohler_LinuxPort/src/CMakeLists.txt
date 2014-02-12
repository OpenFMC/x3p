project(ISO5436_2)

cmake_minimum_required(VERSION 2.8)

INCLUDE (FindPkgConfig)

find_package(ZLIB) 

if ( ZLIB_FOUND )
    include_directories( ${ZLIB_INCLUDE_DIRS} )
else ( ZLIB_FOUND )
    add_subdirectory(zlib) 
    include_directories(${CMAKE_CURRENT_BINARY_DIR}/zlib)
    set(ZLIB_LIBRARIES "zlib")
endif( ZLIB_FOUND )

PKG_CHECK_MODULES(MINIZIP minizip)
if ( MINIZIP_FOUND )
    include_directories( ${MINIZIP_INCLUDE_DIRS} )
else ( MINIZIP_FOUND )
    add_subdirectory(zlib/contrib/minizip) 
    set(MINIZIP_LIBRARIES "zlibwapi")
endif ( MINIZIP_FOUND )
 
add_subdirectory(ISO5436_2_XML) 

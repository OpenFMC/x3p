# 
# Attempt to find the xsd application in various places. If found, the full
# path will be in XSD_EXECUTABLE. Look in the usual locations, as well as in
# the 'bin' directory in the path given in the XSD_ROOT environment variable.
# 

IF (MINIZIP_INCLUDE_DIR AND MINIZIP_LIB AND MINIZIP_LIB_DEBUG)
# in cache already
SET(MINIZIP_FIND_QUIETLY TRUE)
ENDIF (MINIZIP_INCLUDE_DIR AND MINIZIP_LIB AND MINIZIP_LIB_DEBUG)

FIND_PATH(MINIZIP_INCLUDE_DIR minizip.c
  PATHS ${CMAKE_SOURCE_DIR}/zlib/contrib/minizip
  ${CMAKE_SOURCE_DIR}/../zlib/contrib/minizip
  ${CMAKE_SOURCE_DIR}/../../zlib/contrib/minizip
)

set (SearchPathDebugMinizip
      ${CMAKE_SOURCE_DIR}/zlib/contrib/minizip/.libs;
      ${CMAKE_SOURCE_DIR}/../zlib/contrib/minizip/.libs;
      ${CMAKE_SOURCE_DIR}/../../zlib/contrib/minizip/.libs;
      ${CMAKE_BINARY_DIR};
      ${CMAKE_BINARY_DIR}/zlib;
      ${CMAKE_BINARY_DIR}/minizip;
      ${CMAKE_BINARY_DIR}/zlib/Debug;
      ${CMAKE_BINARY_DIR}/minizip/Debug;
      ${CMAKE_BINARY_DIR}/zlib/Release;
      ${CMAKE_BINARY_DIR}/minizip/Release;
      ${CMAKE_BINARY_DIR}/../zlib;
      ${CMAKE_BINARY_DIR}/../minizip;
      ${CMAKE_BINARY_DIR}/../zlib/Debug;
      ${CMAKE_BINARY_DIR}/../minizip/Debug
)

set (SearchPathReleaseMinizip
      ${CMAKE_SOURCE_DIR}/zlib/contrib/minizip/.libs;
      ${CMAKE_SOURCE_DIR}/../zlib/contrib/minizip/.libs;
      ${CMAKE_SOURCE_DIR}/../../zlib/contrib/minizip/.libs;
      ${CMAKE_BINARY_DIR};
      ${CMAKE_BINARY_DIR}/zlib;
      ${CMAKE_BINARY_DIR}/minizip;
      ${CMAKE_BINARY_DIR}/zlib/Debug;
      ${CMAKE_BINARY_DIR}/minizip/Debug;
      ${CMAKE_BINARY_DIR}/zlib/Release;
      ${CMAKE_BINARY_DIR}/minizip/Release;
      ${CMAKE_BINARY_DIR}/../zlib;
      ${CMAKE_BINARY_DIR}/../minizip;
      ${CMAKE_BINARY_DIR}/../zlib/Release;
      ${CMAKE_BINARY_DIR}/../minizip/Release
)

if (WIN32)
   FIND_PATH(MINIZIP_LIB_DEBUG zlibwapi.lib
      PATHS ${SearchPathDebugMinizip}
   )
else()
   FIND_PATH(MINIZIP_LIB_DEBUG libzlibwapi.a
     PATHS ${SearchPathDebugMinizip}
   )   
endif ()

if (WIN32)
   FIND_PATH(MINIZIP_LIB zlibwapi.lib
     PATHS ${SearchPathReleaseMinizip}
   )
else()
   FIND_PATH(MINIZIP_LIB libzlibwapi.a
     PATHS ${SearchPathReleaseMinizip}
   )
endif ()

if (NOT MINIZIP_INCLUDE_DIR)
  message (FATAL_ERROR "Unable to find minizip include files (minizip.c)")
else (NOT MINIZIP_INCLUDE_DIR)
  if(NOT MINIZIP_FIND_QUIETLY)
    message (STATUS "Found minizip: " ${MINIZIP_INCLUDE_DIR})
    message (STATUS "             : " ${MINIZIP_LIB})    
    message (STATUS "             : " ${MINIZIP_LIB_DEBUG})
  endif (NOT MINIZIP_FIND_QUIETLY)
endif (NOT MINIZIP_INCLUDE_DIR)

if (NOT MINIZIP_LIB_DEBUG)
  message (FATAL_ERROR "Unable to find minizip debug libraray")
  SET(MINIZIP_LIB_DEBUG)
  UNSET(MINIZIP_LIB_DEBUG CACHE)
endif (NOT MINIZIP_LIB_DEBUG)

if (NOT MINIZIP_LIB AND NOT MINIZIP_LIB_DEBUG)
  message (FATAL_ERROR "Unable to find minizip libraray")
  SET(MINIZIP_LIB)
  UNSET(MINIZIP_LIB CACHE)
  SET(MINIZIP_LIB_DEBUG)
  UNSET(MINIZIP_LIB CACHE_DEBUG)
endif ()
#
# General CMake package configuration.
#
INCLUDE( FindPackageHandleStandardArgs )
FIND_PACKAGE_HANDLE_STANDARD_ARGS( MINIZIP DEFAULT_MSG MINIZIP_INCLUDE_DIR )

MARK_AS_ADVANCED( MINIZIP_INCLUDE_DIR MINIZIP_LIB MINIZIP_LIB_DEBUG)
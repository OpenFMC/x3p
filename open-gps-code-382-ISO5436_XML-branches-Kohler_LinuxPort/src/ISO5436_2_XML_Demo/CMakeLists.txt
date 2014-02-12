project(ISO5436_2-demo)

cmake_minimum_required(VERSION 2.8)

OPTION(BUILD_SHARED_LIBS "Build shared library." ON)

IF(BUILD_SHARED_LIBS)
  SET(LIBRARY_TYPE SHARED)
ELSE(BUILD_SHARED_LIBS)
  SET(LIBRARY_TYPE STATIC)
ENDIF(BUILD_SHARED_LIBS)

SET (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR})

# default build types are None, Debug, Release, RelWithDebInfo and MinRelSize
IF (DEFINED CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE} CACHE STRING "Choose the type of build, options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel.")
ELSE(CMAKE_BUILD_TYPE)
  SET (CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build,
      options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release
      RelWithDebInfo MinSizeRel.")
ENDIF (DEFINED CMAKE_BUILD_TYPE)

find_package(XSD REQUIRED)

find_package(Xerces REQUIRED)

find_package(Minizip REQUIRED)

FIND_PATH(ISO5436_2_PATH iso5436_2.xsd
  PATHS $ENV{OPENGPS_DIR}
  ./ISO5436_2_XML
  ./x3p
  ../ISO5436_2_XML
  ../x3p
)

set (SearchPathReleaseISO5436
      $ENV{OPENGPS_DIR};
      ${CMAKE_BINARY_DIR}/ISO5436_2_XML;
      ${CMAKE_BINARY_DIR}/ISO5436_2_XML/Release;
      ${CMAKE_BINARY_DIR}/x3p;
      ${CMAKE_BINARY_DIR}/x3p/Release;
      ${CMAKE_BINARY_DIR}/../ISO5436_2_XML;
      ${CMAKE_BINARY_DIR}/../ISO5436_2_XML/Release;
      ${CMAKE_BINARY_DIR}/../x3p;
      ${CMAKE_BINARY_DIR}/../x3p/Release
)

set (SearchPathDebugISO5436
      $ENV{OPENGPS_DIR};
      ${CMAKE_BINARY_DIR}/ISO5436_2_XML;
      ${CMAKE_BINARY_DIR}/ISO5436_2_XML/Debug;
      ${CMAKE_BINARY_DIR}/x3p;
      ${CMAKE_BINARY_DIR}/x3p/Debug;
      ${CMAKE_BINARY_DIR}/../ISO5436_2_XML;
      ${CMAKE_BINARY_DIR}/../ISO5436_2_XML/Debug;
      ${CMAKE_BINARY_DIR}/../x3p;
      ${CMAKE_BINARY_DIR}/../x3p/Debug
)

if (WIN32)
   if(BUILD_SHARED_LIBS)
      MESSAGE(STATUS "Build shared win debug")
      if (${CMAKE_BUILD_TYPE} STREQUAL Debug)
         FIND_PATH(ISO5436_2_LIB_PATH iso5436-2-xml.dll
            PATHS ${SearchPathDebugISO5436}
         )
      else ()
         FIND_PATH(ISO5436_2_LIB_PATH iso5436-2-xml.dll
            PATHS ${SearchPathReleaseISO5436}
         )     
      endif ()
   else()
      MESSAGE(STATUS "Build static win debug")     
      if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
         FIND_PATH(ISO5436_2_LIB_PATH iso5436-2-xml.lib
            PATHS ${SearchPathDebugISO5436}
         )
      else ()
         FIND_PATH(ISO5436_2_LIB_PATH iso5436-2-xml.lib
            PATHS ${SearchPathReleaseISO5436}
         )
      endif ()
   endif()
else()
   if(BUILD_SHARED_LIBS)
      MESSAGE(STATUS "Build shared")
      if (CMAKE_BUILD_TYPE STREQUAL Debug)
         FIND_PATH(ISO5436_2_LIB_PATH libiso5436-2-xml.so
            PATHS ${SearchPathDebugISO5436}
         )
      else ()
         FIND_PATH(ISO5436_2_LIB_PATH libiso5436-2-xml.so
            PATHS ${SearchPathReleaseISO5436}
         )
      endif ()
   else()
      MESSAGE(STATUS "Build static")
      if (CMAKE_BUILD_TYPE STREQUAL Debug)
         FIND_PATH(ISO5436_2_LIB_PATH libiso5436-2-xml.a
            PATHS ${SearchPathDebugISO5436}
         )
      else ()
         FIND_PATH(ISO5436_2_LIB_PATH libiso5436-2-xml.a
            PATHS ${SearchPathReleaseISO5436}
         )
      endif ()
   endif()
endif()
   
INCLUDE_DIRECTORIES( "../../include"
	  "../../include/opengps/cxx"
     "${XSD_INCLUDE_DIR}"
)

add_definitions(-DUNICODE -D_UNICODE)

add_executable(iso5436-2-xml-demo ISO5436_2_XML_Demo.cxx)

if (NOT WIN32)
   set(CMAKE_SHARED_LINKER_FLAGS "-Wl,-R./:./lib")
   set(CMAKE_MODULE_LINKER_FLAGS "-Wl,-R./:./lib")
else()
   set(CMAKE_EXE_LINKER_FLAGS "-Wl,-R./:./lib")
endif ()
   
IF(BUILD_SHARED_LIBS)
   IF(WIN32)
      ADD_CUSTOM_TARGET( copyzlib ALL)
      ADD_CUSTOM_COMMAND( TARGET copyzlib POST_BUILD
            COMMAND copy
            ARGS  /Y ${MINIZIP_LIB}/*.dll ${CMAKE_CURRENT_BINARY_DIR}/
            COMMAND copy 
            ARGS /Y ${ISO5436_2_LIB_PATH}/*.dll ${CMAKE_CURRENT_BINARY_DIR}/
            DEPENDS ISO5436-2-demo)
   ENDIF(WIN32)
   IF(UNIX)
      ADD_CUSTOM_TARGET( copyzlib ALL)
      ADD_CUSTOM_COMMAND( TARGET copyzlib POST_BUILD
            COMMAND /bin/mkdir
            ARGS -p ${CMAKE_CURRENT_BINARY_DIR}/lib/
            COMMAND /bin/cp
            ARGS -uf ${MINIZIP_LIB}/*.so* ${CMAKE_CURRENT_BINARY_DIR}/lib/
            COMMAND /bin/cp 
            ARGS -uf ${ISO5436_2_LIB_PATH}/*.so* ${CMAKE_CURRENT_BINARY_DIR}/lib/
            DEPENDS ISO5436-2-demo)
   ENDIF(UNIX)
ENDIF(BUILD_SHARED_LIBS)

if (WIN32)
   if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
      MESSAGE(STATUS ${XERCESC_LIBRARY})
      target_link_libraries(iso5436-2-xml-demo ${ISO5436_2_LIB_PATH}/iso5436-2-xml.lib)
      target_link_libraries(iso5436-2-xml-demo ${MINIZIP_LIB_DEBUG}/zlibwapi.lib)
      target_link_libraries(iso5436-2-xml-demo ${XERCESC_LIBRARY})      
    else ()
      target_link_libraries(iso5436-2-xml-demo ${ISO5436_2_LIB_PATH}/iso5436-2-xml.lib)
      target_link_libraries(iso5436-2-xml-demo ${MINIZIP_LIB}/zlibwapi.lib)
      target_link_libraries(iso5436-2-xml-demo ${XERCESC_LIBRARY})      
    endif ()
else ()
   if (BUILD_SHARED_LIBS)
      set (LIB_EXT so)
   else ()
      set (LIB_EXT a)
   endif ()
   if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
      target_link_libraries(iso5436-2-xml-demo ${XERCESC_LIBRARY})
      target_link_libraries(iso5436-2-xml-demo ${MINIZIP_LIB_DEBUG}/libzlibwapi.${LIB_EXT})
      target_link_libraries(iso5436-2-xml-demo ${ISO5436_2_LIB_PATH}/libiso5436-2-xml.${LIB_EXT})
      target_link_libraries(iso5436-2-xml-demo libz.${LIB_EXT})
   else ()
      target_link_libraries(iso5436-2-xml-demo ${XERCESC_LIBRARY})
      target_link_libraries(iso5436-2-xml-demo libz.${LIB_EXT})
      target_link_libraries(iso5436-2-xml-demo ${MINIZIP_LIB}/libzlibwapi.${LIB_EXT})
      target_link_libraries(iso5436-2-xml-demo ${ISO5436_2_LIB_PATH}/libiso5436-2-xml.${LIB_EXT})
   endif () 
 endif ()
 
 if (DEFINED MSVC_VERSION)
  set_target_properties(iso5436-2-xml-demo PROPERTIES COMPILE_FLAGS "/Zc:wchar_t")
endif ()
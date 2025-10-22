cmake_minimum_required(VERSION 3.21...3.28)
project(openGPS VERSION 0.4.2)

macro(set_warning_levels target)
  if (MSVC)
    target_compile_options(${target} PRIVATE /W4 /wd4251 /wd4275)
    target_compile_definitions(${target} PRIVATE _CRT_NONSTDC_NO_DEPRECATE _CRT_SECURE_NO_DEPRECATE _CRT_NONSTDC_NO_WARNINGS)
  else()
    target_compile_options(${target} PRIVATE -Wall -Wextra -pedantic)
  endif()
endmacro()

option(BUILD_SHARED_LIBS "Build using shared libraries" ON)
option(BUILD_DEMO "Build the demo application" OFF)
option(BUILD_MATLAB_TOOLBOX "Build the MATLAB toolbar" OFF)
option(PACK_XSD_RUNTIME "Package XSD runtime files" OFF)

find_package(XercesC 3.2 REQUIRED)

if(BUILD_SHARED_LIBS)
  # minizip and zlib provided by external repository
  add_subdirectory(src/zlibwapi)
  add_library(iso5436_2::minizip ALIAS zlibwapi)
else()
  # not explicitly references but build will fail if not present
  find_package(ZLIB REQUIRED)

  # minizip provided by vcpkg
  find_package(unofficial-minizip CONFIG)
  if(unofficial-minizip_FOUND)
    add_library(iso5436_2::minizip ALIAS unofficial::minizip::minizip)
  else()
    # minizip provided by conan
    find_package(minizip)
    if (minizip_FOUND)
      add_library(iso5436_2::minizip ALIAS minizip::minizip)
    else()
      # minizip provided by Linux
      find_package(PkgConfig REQUIRED)
      pkg_check_modules(minizip REQUIRED IMPORTED_TARGET minizip)
      add_library(iso5436_2::minizip ALIAS PkgConfig::minizip)
    endif()
  endif()
endif()

add_subdirectory(src/ISO5436_2_XML)
if(BUILD_DEMO)
  include(CTest)
  add_subdirectory(src/ISO5436_2_XML_Demo)
endif()

if(BUILD_MATLAB_TOOLBOX)
  # At least MATLAB 2020b is required for UTF-8 support
  find_package(Matlab 9.9.0 REQUIRED COMPONENTS MX_LIBRARY)
  add_subdirectory(src/MATLAB)
endif()

set(CPACK_ARCHIVE_COMPONENT_INSTALL ON)

if(WIN32)
  set(CPACK_GENERATOR ZIP)
else()
  set(CPACK_GENERATOR TGZ)
endif()

include(CPack)

if(BUILD_DEMO)
  set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT iso5436_2_xml_demo)
else()
  set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT iso5436_2_xml)
endif()

project(zlibwapi)

include(FetchContent)
option(ZLIB_BUILD_EXAMPLES "Enable Zlib Examples" OFF)
FetchContent_Declare(zlibext GIT_REPOSITORY https://github.com/madler/zlib.git GIT_TAG v1.3.1 EXCLUDE_FROM_ALL)
FetchContent_MakeAvailable(zlibext)

set_property(TARGET zlibstatic PROPERTY POSITION_INDEPENDENT_CODE ON)

set(minizip_source_dir "${zlibext_SOURCE_DIR}/contrib/minizip")

set(header_files
  "${minizip_source_dir}/ioapi.h"
  "${minizip_source_dir}/unzip.h"
  "${minizip_source_dir}/zip.h"
  "${minizip_source_dir}/mztools.h"
)

set(source_files
  "${minizip_source_dir}/ioapi.c"
  "${minizip_source_dir}/unzip.c"
  "${minizip_source_dir}/zip.c"
  "${minizip_source_dir}/mztools.c"
)

add_library(${PROJECT_NAME} STATIC)
target_link_libraries(${PROJECT_NAME} PUBLIC zlibstatic)
target_include_directories(${PROJECT_NAME} PUBLIC "${minizip_source_dir}")
target_sources(${PROJECT_NAME} PRIVATE ${source_files} ${header_files})
if(WIN32)
  target_sources(${PROJECT_NAME} PRIVATE "${minizip_source_dir}/iowin32.c" "${minizip_source_dir}/iowin32.h")
endif()
if(MSVC)
  target_compile_definitions(${PROJECT_NAME} PRIVATE UNICODE _UNICODE)
endif()
set_property(TARGET ${PROJECT_NAME} PROPERTY POSITION_INDEPENDENT_CODE TRUE)

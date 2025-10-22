project(iso5436_2_xml VERSION ${CMAKE_PROJECT_VERSION})

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR})

if(WIN32)
  set(CMAKE_DEBUG_POSTFIX "_d")
endif()

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

configure_file("cxx/version.h.in" "cxx/version.h" @ONLY)
# WORKAROUND: create output dir for the XSD compiler, otherwise build will fail on Linux
configure_file("xsd_Licence_Header.c" "opengps/cxx/xsd_Licence_Header.c" COPYONLY)
if(WIN32 AND BUILD_SHARED_LIBS)
  configure_file("iso5436_2.rc.in" "iso5436_2.rc" @ONLY)
endif()

find_package(XSD REQUIRED)

XSD_SCHEMA(iso5436_2_xsd "${CMAKE_CURRENT_SOURCE_DIR}/iso5436_2.xsd" --prologue-file "${CMAKE_CURRENT_SOURCE_DIR}/xsd_Licence_Header.c" --generate-doxygen --generate-ostream --generate-serialization --char-type wchar_t --generate-comparison --generate-from-base-ctor --namespace-map http://www.opengps.eu/2008/ISO5436_2=OpenGPS::Schemas::ISO5436_2 --export-symbol _OPENGPS_EXPORT --cxx-suffix _xsd.cxx --hxx-suffix _xsd.hxx --output-dir "${CMAKE_CURRENT_BINARY_DIR}/opengps/cxx")

set(c_header_files
  "c/data_point_c.hxx"
  "c/iso5436_2_handle_c.hxx"
  "c/messages_c.hxx"
  "c/point_iterator_c.hxx"
  "c/point_vector_c.hxx"
)

source_group("Header Files/c" FILES ${c_header_files})

set(cxx_header_files
  "cxx/binary_lsb_point_vector_reader_context.hxx"
  "cxx/binary_lsb_point_vector_writer_context.hxx"
  "cxx/binary_msb_point_vector_reader_context.hxx"
  "cxx/binary_msb_point_vector_writer_context.hxx"
  "cxx/binary_point_vector_reader_context.hxx"
  "cxx/binary_point_vector_writer_context.hxx"
  "cxx/data_point_impl.hxx"
  "cxx/data_point_parser.hxx"
  "cxx/data_point_parser_impl.hxx"
  "cxx/environment.hxx"
  "cxx/inline_validity.hxx"
  "cxx/iso5436_2_container.hxx"
  "cxx/missing_data_point_parser.hxx"
  "cxx/point_buffer.hxx"
  "cxx/point_buffer_impl.hxx"
  "cxx/point_validity_provider.hxx"
  "cxx/point_vector_iostream.hxx"
  "cxx/point_vector_parser.hxx"
  "cxx/point_vector_parser_builder.hxx"
  "cxx/point_vector_proxy.hxx"
  "cxx/point_vector_proxy_context.hxx"
  "cxx/point_vector_proxy_context_list.hxx"
  "cxx/point_vector_proxy_context_matrix.hxx"
  "cxx/point_vector_reader_context.hxx"
  "cxx/point_vector_writer_context.hxx"
  "cxx/stdafx.hxx"
  "cxx/valid_buffer.hxx"
  "cxx/version.h.in"
  "cxx/vector_buffer.hxx"
  "cxx/vector_buffer_builder.hxx"
  "cxx/win32_environment.hxx"
  "cxx/linux_environment.hxx"
  "cxx/xml_point_vector_reader_context.hxx"
  "cxx/xml_point_vector_writer_context.hxx"
  "cxx/zip_stream_buffer.hxx"
)

source_group("Header Files/cxx" FILES ${cxx_header_files})

set(xyssl_header_files
  "xyssl/md5.h"
)

source_group("Header Files/xyssl" FILES ${xyssl_header_files})

set(public_header_files
  "../../include/opengps/data_point.h"
  "../../include/opengps/data_point_type.h" 
  "../../include/opengps/info.h"
  "../../include/opengps/iso5436_2.h"
  "../../include/opengps/messages.h" 
  "../../include/opengps/opengps.h"
  "../../include/opengps/point_iterator.h"
  "../../include/opengps/point_vector.h" 
)

source_group("Header Files/opengps" FILES ${public_header_files})

set(public_cxx_header_files
  "../../include/opengps/cxx/data_point.hxx"
  "../../include/opengps/cxx/exceptions.hxx" 
  "../../include/opengps/cxx/info.hxx"
  "../../include/opengps/cxx/iso5436_2.hxx"
  "../../include/opengps/cxx/iso5436_2_handle.hxx"
  "../../include/opengps/cxx/iso5436_2_xsd_utils.hxx"
  "../../include/opengps/cxx/opengps.hxx"
  "../../include/opengps/cxx/point_iterator.hxx"
  "../../include/opengps/cxx/point_vector.hxx"
  "../../include/opengps/cxx/point_vector_base.hxx" 
  "../../include/opengps/cxx/string.hxx" 
)

source_group("Header Files/opengps/cxx" FILES ${public_cxx_header_files})

set(c_source_files
  "c/data_point_c.cxx"
  "c/info_c.cxx"
  "c/iso5436_2_c.cxx"
  "c/messages_c.cxx"
  "c/point_iterator_c.cxx"
  "c/point_vector_c.cxx"
)

source_group("Source Files/c" FILES ${c_source_files})

set(cxx_source_files
  "cxx/binary_lsb_point_vector_reader_context.cxx"
  "cxx/binary_lsb_point_vector_writer_context.cxx"
  "cxx/binary_msb_point_vector_reader_context.cxx"
  "cxx/binary_msb_point_vector_writer_context.cxx"
  "cxx/binary_point_vector_reader_context.cxx"
  "cxx/binary_point_vector_writer_context.cxx"
  "cxx/data_point_impl.cxx"
  "cxx/data_point_proxy.cxx"
  "cxx/environment.cxx"
  "cxx/exceptions.cxx"
  "cxx/info.cxx"
  "cxx/iso5436_2.cxx"
  "cxx/iso5436_2_container.cxx"
  "cxx/iso5436_2_xsd_utils.cxx"
  "cxx/missing_data_point_parser.cxx"
  "cxx/point_buffer.cxx"
  "cxx/point_iterator.cxx"
  "cxx/point_validity_provider.cxx"
  "cxx/point_vector.cxx"
  "cxx/point_vector_iostream.cxx"
  "cxx/point_vector_parser.cxx"
  "cxx/point_vector_parser_builder.cxx"
  "cxx/point_vector_proxy.cxx"
  "cxx/point_vector_proxy_context.cxx"
  "cxx/point_vector_proxy_context_list.cxx"
  "cxx/point_vector_proxy_context_matrix.cxx"
  "cxx/string.cxx"
  "cxx/valid_buffer.cxx"
  "cxx/vector_buffer.cxx"
  "cxx/vector_buffer_builder.cxx"
  "cxx/win32_environment.cxx"
  "cxx/linux_environment.cxx"
  "cxx/xml_point_vector_reader_context.cxx"
  "cxx/xml_point_vector_writer_context.cxx"
  "cxx/zip_stream_buffer.cxx"
)

source_group("Source Files/cxx" FILES ${cxx_source_files})

set(xyssl_source_files
  "xyssl/md5.c"
)

source_group("Source Files/xyssl" FILES ${xyssl_source_files})

add_library(${PROJECT_NAME})
set_warning_levels(${PROJECT_NAME})
set_property(TARGET ${PROJECT_NAME} PROPERTY POSITION_INDEPENDENT_CODE ON)

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_14)
set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${PROJECT_VERSION} SOVERSION ${PROJECT_VERSION_MAJOR})

# WORKAROUND: set anything to private, even public headers, othewise it causes errors during export
# even with BUILD_INTERFACE expressions
target_sources(${PROJECT_NAME}
  PRIVATE
  "iso5436_2.xsd"
  ${c_source_files}
  ${cxx_source_files}
  ${xyssl_source_files}
  ${iso5436_2_xsd_SOURCES}
  ${c_header_files}
  ${cxx_header_files}
  ${xyssl_header_files}
  ${public_cxx_header_files}
  ${iso5436_2_xsd_HEADERS}
  ${public_header_files}
)
if(WIN32 AND BUILD_SHARED_LIBS)
  target_sources(${PROJECT_NAME}
    PRIVATE
    "iso5436_2.rc.in"
    "${CMAKE_CURRENT_BINARY_DIR}/iso5436_2.rc"
  )
endif()

target_link_libraries(${PROJECT_NAME}
  PRIVATE
  iso5436_2::minizip
  PUBLIC
  XercesC::XercesC
)

target_include_directories(${PROJECT_NAME}
  PRIVATE
  "${CMAKE_CURRENT_BINARY_DIR}/cxx"
  PUBLIC
  "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/../../include>"
  "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/../../../../include/opengps/cxx>"
  "$<BUILD_INTERFACE:${XSD_INCLUDE_DIR}>"
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>"
  "$<INSTALL_INTERFACE:include>"
)
if(PACK_XSD_RUNTIME)
  target_include_directories(${PROJECT_NAME}
    PUBLIC
    "$<INSTALL_INTERFACE:include/opengps>"
  )
endif()

target_compile_definitions(${PROJECT_NAME} PRIVATE XSD_CXX_TREE_FLOAT_PRECISION_MAX XSD_CXX_TREE_DOUBLE_PRECISION_MAX XSD_CXX_TREE_DECIMAL_PRECISION_MAX PUBLIC UNICODE _UNICODE)
if(BUILD_SHARED_LIBS)
  target_compile_definitions(${PROJECT_NAME} PRIVATE BUILD_ISO5436_2_XML_DLL PUBLIC SHARED_OPENGPS_LIBRARY)
else()
  target_compile_definitions(${PROJECT_NAME} PRIVATE BUILD_ISO5436_2_XML)
endif()

install(TARGETS ${PROJECT_NAME} EXPORT ${PROJECT_NAME}Targets COMPONENT ${PROJECT_NAME})
install(FILES ${public_header_files} DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/opengps" COMPONENT ${PROJECT_NAME})	
install(FILES ${public_cxx_header_files} ${iso5436_2_xsd_HEADERS} "${CMAKE_CURRENT_BINARY_DIR}/cxx/version.h" DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/opengps/cxx" COMPONENT ${PROJECT_NAME})
if(PACK_XSD_RUNTIME)
  install(DIRECTORY "${XSD_INCLUDE_DIR}/xsd" DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/opengps" COMPONENT ${PROJECT_NAME})	
endif()
install(FILES iso5436_2.xsd DESTINATION "${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME}" COMPONENT ${PROJECT_NAME})

install(EXPORT ${PROJECT_NAME}Targets
  FILE ${PROJECT_NAME}Targets.cmake
  NAMESPACE OPENGPS::
  DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}"
  COMPONENT ${PROJECT_NAME}
)

install(FILES "../../Licence/openGPS_Licence.txt" DESTINATION "${CMAKE_INSTALL_DOCDIR}" RENAME "copyright" COMPONENT ${PROJECT_NAME})

configure_package_config_file("${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
  INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}"
)

write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
  COMPATIBILITY AnyNewerVersion
)

install(FILES
  "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
  "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
  DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}"
  COMPONENT ${PROJECT_NAME}
)

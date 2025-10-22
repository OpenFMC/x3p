project(importexportx3p)

include(GNUInstallDirs)

matlab_add_mex(NAME openX3P_mex SRC "openX3P_mex.cpp" "X3PUtilities.h" "X3PUtilities.cpp" R2018a)
set_warning_levels(openX3P_mex)
target_link_libraries(openX3P_mex iso5436_2_xml)

matlab_add_mex(NAME writeX3P_mex SRC "writeX3P_mex.cpp" "X3PUtilities.h" "X3PUtilities.cpp" R2018a)
set_warning_levels(writeX3P_mex)
target_link_libraries(writeX3P_mex iso5436_2_xml)

install(DIRECTORY "../X3PToolbox" DESTINATION "." CONFIGURATIONS Release RelWithDebInfo COMPONENT x3ptoolbox)
install(FILES "../../ISO5436_2_XML/iso5436_2.xsd" DESTINATION "X3PToolbox/private" CONFIGURATIONS Release RelWithDebInfo COMPONENT x3ptoolbox)
install(TARGETS openX3P_mex DESTINATION "X3PToolbox/private" CONFIGURATIONS Release RelWithDebInfo COMPONENT x3ptoolbox)
install(TARGETS writeX3P_mex RUNTIME_DEPENDENCY_SET writeX3P_mex_deps DESTINATION "X3PToolbox/private" CONFIGURATIONS Release RelWithDebInfo COMPONENT x3ptoolbox)
if(WIN32)
  add_custom_command(TARGET writeX3P_mex POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy -t $<TARGET_FILE_DIR:writeX3P_mex> $<TARGET_RUNTIME_DLLS:writeX3P_mex>
    COMMAND_EXPAND_LISTS
  )
  install(RUNTIME_DEPENDENCY_SET writeX3P_mex_deps DESTINATION "X3PToolbox/private"
    PRE_EXCLUDE_REGEXES [[api-ms-win-.*]] [[ext-ms-.*]] [[vcruntime.*]] [[msvcp.*]] [[.*32\..*]] [[libm.*]]
    POST_EXCLUDE_REGEXES [[.*/system32/.*\.dll]] CONFIGURATIONS Release RelWithDebInfo COMPONENT x3ptoolbox
  )
endif()
install(FILES "../../../Licence/openGPS_Licence.txt" DESTINATION "X3PToolbox/private" CONFIGURATIONS Release RelWithDebInfo COMPONENT x3ptoolbox)

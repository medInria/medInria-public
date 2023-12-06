################################################################################
#
# medInria
#
# Copyright (c) INRIA 2013 - 2018. All rights reserved.
# See LICENSE.txt for details.
#
#  This software is distributed WITHOUT ANY WARRANTY; without even
#  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
#  PURPOSE.
#
################################################################################

include(CMakePackageConfigHelpers)

macro(set_lib_install_rules_generic target dest)

################################################################################
#
# Usage: set_lib_install_rules(target, header1, header2, header3 ...)
# set rules for the library designed by the target, and add all the additional
# header to {CMAKE_PREFIX_INSTALL}/include during install step.
#
################################################################################

get_property(GENERATOR_MULTI_CONFIG GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)

if(${GENERATOR_MULTI_CONFIG})
  set_target_properties( ${target} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG          ${dest}/medInria/bin)
  set_target_properties( ${target} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE        ${dest}/medInria/bin)
  set_target_properties( ${target} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL     ${dest}/medInria/bin)
  set_target_properties( ${target} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${dest}/medInria/bin)

  set_target_properties( ${target} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_DEBUG          ${dest}/medInria/lib)
  set_target_properties( ${target} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_RELEASE        ${dest}/medInria/lib)
  set_target_properties( ${target} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_MINSIZEREL     ${dest}/medInria/lib)
  set_target_properties( ${target} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_RELWITHDEBINFO ${dest}/medInria/lib)

  set_target_properties( ${target} PROPERTIES LIBRARY_OUTPUT_DIRECTORY_DEBUG          ${dest}/medInria/lib)
  set_target_properties( ${target} PROPERTIES LIBRARY_OUTPUT_DIRECTORY_RELEASE        ${dest}/medInria/lib)
  set_target_properties( ${target} PROPERTIES LIBRARY_OUTPUT_DIRECTORY_MINSIZEREL     ${dest}/medInria/lib)
  set_target_properties( ${target} PROPERTIES LIBRARY_OUTPUT_DIRECTORY_RELWITHDEBINFO ${dest}/medInria/lib)
else()
  set_target_properties( ${target} PROPERTIES RUNTIME_OUTPUT_DIRECTORY                ${dest}/medInria/bin)
  set_target_properties( ${target} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY                ${dest}/medInria/lib)
  set_target_properties( ${target} PROPERTIES LIBRARY_OUTPUT_DIRECTORY                ${dest}/medInria/lib)
endif()

install(TARGETS ${target} 
        EXPORT  ${target}Targets

        RUNTIME   DESTINATION bin
        LIBRARY   DESTINATION lib
        ARCHIVE   DESTINATION lib
        FRAMEWORK DESTINATION lib
        INCLUDES  DESTINATION include
        RESOURCE  DESTINATION resources/${target}
  )

install(FILES ${${TARGET_NAME}_HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${TARGET_NAME})
#message("---> ${CMAKE_INSTALL_INCLUDEDIR} --\n")
write_basic_package_version_file(
  "${CMAKE_BINARY_DIR}/medInria/lib/cmake/${TARGET_NAME}/${TARGET_NAME}ConfigVersion.cmake"
  VERSION ${medInria_VERSION}
  COMPATIBILITY AnyNewerVersion
)

export(EXPORT ${TARGET_NAME}Targets
  FILE "${CMAKE_BINARY_DIR}/medInria/lib/cmake/${TARGET_NAME}/${TARGET_NAME}Targets.cmake"
  NAMESPACE ${PROJECT_NAME}::
)

set(PACKAGE_INIT "@PACKAGE_INIT@")
configure_file(${CMAKE_SOURCE_DIR}/cmake/Config.cmake.in ${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in @ONLY)
configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in
  "${CMAKE_BINARY_DIR}/medInria/lib/cmake/${TARGET_NAME}/${TARGET_NAME}Config.cmake"
  INSTALL_DESTINATION lib/cmake/${TARGET_NAME}
  NO_SET_AND_CHECK_MACRO
  NO_CHECK_REQUIRED_COMPONENTS_MACRO
)

#set(conf $<$<CONFIG:Debug>:"Debug">$<$<CONFIG:Release>:"Release">$<$<CONFIG:MinSizeRel>:"MinSizeRel">$<$<CONFIG:RelWithDebInfo>:"Release">)
set(ConfigPackageLocation lib/cmake/${TARGET_NAME})
install(
  EXPORT ${TARGET_NAME}Targets
  FILE ${TARGET_NAME}Targets.cmake
  NAMESPACE ${PROJECT_NAME}::
  DESTINATION ${ConfigPackageLocation}
)


install(FILES
  ${CMAKE_BINARY_DIR}/medInria/lib/cmake/${TARGET_NAME}/${TARGET_NAME}Config.cmake
  ${CMAKE_BINARY_DIR}/medInria/lib/cmake/${TARGET_NAME}/${TARGET_NAME}ConfigVersion.cmake
  DESTINATION lib/cmake/${TARGET_NAME}
  )
  
## #############################################################################
## Add header wich have to be exposed in the include dir of the install tree
## #############################################################################

if(${ARGC} GREATER 2)
  install(FILES ${ARGN}
    DESTINATION include/${target}
    )
endif()

endmacro()


macro(set_lib_install_rules target)
  set_lib_install_rules_generic(${target} ${CMAKE_BINARY_DIR} ${ARGN})
endmacro()

macro(set_lib_install_rules_external target)
  set_lib_install_rules_generic(${target} ${medInria_DIR} ${ARGN})
endmacro()

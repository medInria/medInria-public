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

macro(set_exe_install_rules target)

################################################################################
#
# Usage: set_exe_install_rules(target)
# set rules for the executable designed by the target
#
################################################################################
get_property(GENERATOR_MULTI_CONFIG GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)

if(${GENERATOR_MULTI_CONFIG})
  set_target_properties( ${target} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG          ${CMAKE_BINARY_DIR}/${platformType}Debug/bin)
  set_target_properties( ${target} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE        ${CMAKE_BINARY_DIR}/${platformType}Release/bin)
  set_target_properties( ${target} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL     ${CMAKE_BINARY_DIR}/${platformType}MinSizeRel/bin)
  set_target_properties( ${target} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_BINARY_DIR}/${platformType}RelWithDebInfo/bin)
else()
  set_target_properties( ${target} PROPERTIES RUNTIME_OUTPUT_DIRECTORY                ${CMAKE_BINARY_DIR}/bin)
endif()

install(TARGETS ${target}
  RUNTIME DESTINATION bin
  BUNDLE  DESTINATION bin
  )

## #############################################################################
##  Mac bundle specificites
## #############################################################################

if (APPLE)
  set(MACOSX_BUNDLE_BUNDLE_NAME
    ${target}
    )
  set(MACOSX_BUNDLE_ICON_FILE
    medInria.icns
    )
  set(MACOSX_BUNDLE_SHORT_VERSION_STRING
    ${${target}_VERSION}
    )
  set(MACOSX_BUNDLE_BUNDLE_VERSION
    ${${target}_VERSION}
    )
  set(MACOSX_BUNDLE_LONG_VERSION_STRING
    "Version ${${target}_VERSION}"
    )
  set(${target}_RESOURCE_DIR
    ${CMAKE_BINARY_DIR}/bin/${target}.app/Contents/Resources
    )
  add_custom_command(TARGET ${target} POST_BUILD
    COMMAND ${CMAKE_COMMAND} ARGS -E make_directory ${${target}_RESOURCE_DIR}
    COMMAND ${CMAKE_COMMAND} ARGS -E copy ${CMAKE_CURRENT_SOURCE_DIR}/resources/${target}.icns ${${target}_RESOURCE_DIR}
    )

  install(CODE "
  execute_process(COMMAND
    \${QT_BINARY_DIR}/macdeployqt \${CMAKE_INSTALL_PREFIX}/bin/${target}.app
    )
    execute_process(COMMAND
      \${dtk_DIR}/bin/dtkDeploy \${CMAKE_INSTALL_PREFIX}/bin/${target}.app -inject-dir=\${CMAKE_CURRENT_BINARY_DIR}/plugins
      )
  ")
endif()

endmacro()


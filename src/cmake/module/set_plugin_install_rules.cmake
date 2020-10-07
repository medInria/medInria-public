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


################################################################################
############################## NOT LEGACY ######################################

macro(set_plugin_install_rules target)
################################################################################
#
# Usage: set_plugin_install_rules(target)
# set rules for the plugin designed by the target
#
################################################################################
get_property(GENERATOR_MULTI_CONFIG GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)

if(${GENERATOR_MULTI_CONFIG})
  set_target_properties(${target} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/plugins )
  set_target_properties( ${target} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG          ${CMAKE_BINARY_DIR}/${platformType}Debug/bin/plugins)
  set_target_properties( ${target} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE        ${CMAKE_BINARY_DIR}/${platformType}Release/bin/plugins)
  set_target_properties( ${target} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL     ${CMAKE_BINARY_DIR}/${platformType}MinSizeRel/bin/plugins)
  set_target_properties( ${target} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_BINARY_DIR}/${platformType}RelWithDebInfo/bin/plugins)
  
  set_target_properties( ${target} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_DEBUG          ${CMAKE_BINARY_DIR}/${platformType}Debug/bin/plugins)
  set_target_properties( ${target} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_RELEASE        ${CMAKE_BINARY_DIR}/${platformType}Release/bin/plugins)
  set_target_properties( ${target} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_MINSIZEREL     ${CMAKE_BINARY_DIR}/${platformType}MinSizeRel/bin/plugins)
  set_target_properties( ${target} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_BINARY_DIR}/${platformType}RelWithDebInfo/bin/plugins)
  
  set_target_properties( ${target} PROPERTIES LIBRARY_OUTPUT_DIRECTORY_DEBUG          ${CMAKE_BINARY_DIR}/${platformType}Debug/bin/plugins)
  set_target_properties( ${target} PROPERTIES LIBRARY_OUTPUT_DIRECTORY_RELEASE        ${CMAKE_BINARY_DIR}/${platformType}Release/bin/plugins)
  set_target_properties( ${target} PROPERTIES LIBRARY_OUTPUT_DIRECTORY_MINSIZEREL     ${CMAKE_BINARY_DIR}/${platformType}MinSizeRel/bin/plugins)
  set_target_properties( ${target} PROPERTIES LIBRARY_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_BINARY_DIR}/${platformType}RelWithDebInfo/bin/plugins)
else()
  set_target_properties( ${target} PROPERTIES RUNTIME_OUTPUT_DIRECTORY                ${CMAKE_BINARY_DIR}/bin/plugins)
  set_target_properties( ${target} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY                ${CMAKE_BINARY_DIR}/bin/plugins)
  set_target_properties( ${target} PROPERTIES LIBRARY_OUTPUT_DIRECTORY                ${CMAKE_BINARY_DIR}/bin/plugins)
endif()

install(TARGETS ${target}
  RUNTIME DESTINATION bin/plugins
  LIBRARY DESTINATION bin/plugins
  )

endmacro()



################################################################################
##############################   LEGACY   ######################################

macro(set_plugin_install_rules_legacy target)
################################################################################
#
# Usage: set_plugin_install_rules_legacy(target)
# set rules for the plugin legacy designed by the target
#
################################################################################
get_property(GENERATOR_MULTI_CONFIG GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)

if(${GENERATOR_MULTI_CONFIG})
  set_target_properties( ${target} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG          ${CMAKE_BINARY_DIR}/${platformType}Debug/bin/plugins_legacy)
  set_target_properties( ${target} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE        ${CMAKE_BINARY_DIR}/${platformType}Release/bin/plugins_legacy)
  set_target_properties( ${target} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL     ${CMAKE_BINARY_DIR}/${platformType}MinSizeRel/bin/plugins_legacy)
  set_target_properties( ${target} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_BINARY_DIR}/${platformType}RelWithDebInfo/bin/plugins_legacy)
  
  set_target_properties( ${target} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_DEBUG          ${CMAKE_BINARY_DIR}/${platformType}Debug/bin/plugins_legacy)
  set_target_properties( ${target} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_RELEASE        ${CMAKE_BINARY_DIR}/${platformType}Release/bin/plugins_legacy)
  set_target_properties( ${target} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_MINSIZEREL     ${CMAKE_BINARY_DIR}/${platformType}MinSizeRel/bin/plugins_legacy)
  set_target_properties( ${target} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_BINARY_DIR}/${platformType}RelWithDebInfo/bin/plugins_legacy)
  
  set_target_properties( ${target} PROPERTIES LIBRARY_OUTPUT_DIRECTORY_DEBUG          ${CMAKE_BINARY_DIR}/${platformType}Debug/bin/plugins_legacy)
  set_target_properties( ${target} PROPERTIES LIBRARY_OUTPUT_DIRECTORY_RELEASE        ${CMAKE_BINARY_DIR}/${platformType}Release/bin/plugins_legacy)
  set_target_properties( ${target} PROPERTIES LIBRARY_OUTPUT_DIRECTORY_MINSIZEREL     ${CMAKE_BINARY_DIR}/${platformType}MinSizeRel/bin/plugins_legacy)
  set_target_properties( ${target} PROPERTIES LIBRARY_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_BINARY_DIR}/${platformType}RelWithDebInfo/bin/plugins_legacy)
else()
  set_target_properties( ${target} PROPERTIES RUNTIME_OUTPUT_DIRECTORY                ${CMAKE_BINARY_DIR}/bin/plugins_legacy)
  set_target_properties( ${target} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY                ${CMAKE_BINARY_DIR}/bin/plugins_legacy)
  set_target_properties( ${target} PROPERTIES LIBRARY_OUTPUT_DIRECTORY                ${CMAKE_BINARY_DIR}/bin/plugins_legacy)
endif()

install(TARGETS ${target}
  RUNTIME DESTINATION bin/plugins_legacy
  LIBRARY DESTINATION bin/plugins_legacy
  )
endmacro()
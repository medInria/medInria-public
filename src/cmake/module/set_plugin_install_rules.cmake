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
# Usage: set_plugin_install_rules(target [PYTHON])
# set rules for the plugin designed by the target
#
################################################################################

cmake_parse_arguments(ARG "PYTHON" "" "" ${ARGN})

if(ARG_PYTHON)
    set(plugin_subdir python_plugins)
else()
    set(plugin_subdir plugins)
endif()

get_property(GENERATOR_MULTI_CONFIG GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)

if(${GENERATOR_MULTI_CONFIG})
  set_target_properties( ${target} PROPERTIES RUNTIME_OUTPUT_DIRECTORY                ${CMAKE_BINARY_DIR}/bin/${plugin_subdir} )
  set_target_properties( ${target} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG          ${CMAKE_BINARY_DIR}/${platformType}Debug/bin/${plugin_subdir})
  set_target_properties( ${target} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE        ${CMAKE_BINARY_DIR}/${platformType}Release/bin/${plugin_subdir})
  set_target_properties( ${target} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL     ${CMAKE_BINARY_DIR}/${platformType}MinSizeRel/bin/${plugin_subdir})
  set_target_properties( ${target} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_BINARY_DIR}/${platformType}RelWithDebInfo/bin/${plugin_subdir})
  
  set_target_properties( ${target} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_DEBUG          ${CMAKE_BINARY_DIR}/${platformType}Debug/bin/${plugin_subdir})
  set_target_properties( ${target} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_RELEASE        ${CMAKE_BINARY_DIR}/${platformType}Release/bin/${plugin_subdir})
  set_target_properties( ${target} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_MINSIZEREL     ${CMAKE_BINARY_DIR}/${platformType}MinSizeRel/bin/${plugin_subdir})
  set_target_properties( ${target} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_BINARY_DIR}/${platformType}RelWithDebInfo/bin/${plugin_subdir})
  
  set_target_properties( ${target} PROPERTIES LIBRARY_OUTPUT_DIRECTORY_DEBUG          ${CMAKE_BINARY_DIR}/${platformType}Debug/bin/${plugin_subdir})
  set_target_properties( ${target} PROPERTIES LIBRARY_OUTPUT_DIRECTORY_RELEASE        ${CMAKE_BINARY_DIR}/${platformType}Release/bin/${plugin_subdir})
  set_target_properties( ${target} PROPERTIES LIBRARY_OUTPUT_DIRECTORY_MINSIZEREL     ${CMAKE_BINARY_DIR}/${platformType}MinSizeRel/bin/${plugin_subdir})
  set_target_properties( ${target} PROPERTIES LIBRARY_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_BINARY_DIR}/${platformType}RelWithDebInfo/bin/${plugin_subdir})
else()
  set_target_properties( ${target} PROPERTIES RUNTIME_OUTPUT_DIRECTORY                ${CMAKE_BINARY_DIR}/bin/${plugin_subdir})
  set_target_properties( ${target} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY                ${CMAKE_BINARY_DIR}/bin/${plugin_subdir})
  set_target_properties( ${target} PROPERTIES LIBRARY_OUTPUT_DIRECTORY                ${CMAKE_BINARY_DIR}/bin/${plugin_subdir})
endif()

install(TARGETS ${target}
  RUNTIME DESTINATION bin/${plugin_subdir}
  LIBRARY DESTINATION bin/${plugin_subdir}
  )

endmacro()


################################################################################
##############################   LEGACY   ######################################
macro(set_plugin_install_rules_legacy_generic target dest)

cmake_parse_arguments(ARG "PYTHON" "" "" ${ARGN})

if(ARG_PYTHON)
    set(plugin_subdir python_plugins)
else()
    set(plugin_subdir plugins_legacy)
endif()

get_property(GENERATOR_MULTI_CONFIG GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)

if(${GENERATOR_MULTI_CONFIG})
  set_target_properties( ${target} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG          ${dest}/${platformType}Debug/bin/${plugin_subdir})
  set_target_properties( ${target} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE        ${dest}/${platformType}Release/bin/${plugin_subdir})
  set_target_properties( ${target} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL     ${dest}/${platformType}MinSizeRel/bin/${plugin_subdir})
  set_target_properties( ${target} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${dest}/${platformType}RelWithDebInfo/bin/${plugin_subdir})
																						
  set_target_properties( ${target} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_DEBUG          ${dest}/${platformType}Debug/bin/${plugin_subdir})
  set_target_properties( ${target} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_RELEASE        ${dest}/${platformType}Release/bin/${plugin_subdir})
  set_target_properties( ${target} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_MINSIZEREL     ${dest}/${platformType}MinSizeRel/bin/${plugin_subdir})
  set_target_properties( ${target} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_RELWITHDEBINFO ${dest}/${platformType}RelWithDebInfo/bin/${plugin_subdir})
																						
  set_target_properties( ${target} PROPERTIES LIBRARY_OUTPUT_DIRECTORY_DEBUG          ${dest}/${platformType}Debug/bin/${plugin_subdir})
  set_target_properties( ${target} PROPERTIES LIBRARY_OUTPUT_DIRECTORY_RELEASE        ${dest}/${platformType}Release/bin/${plugin_subdir})
  set_target_properties( ${target} PROPERTIES LIBRARY_OUTPUT_DIRECTORY_MINSIZEREL     ${dest}/${platformType}MinSizeRel/bin/${plugin_subdir})
  set_target_properties( ${target} PROPERTIES LIBRARY_OUTPUT_DIRECTORY_RELWITHDEBINFO ${dest}/${platformType}RelWithDebInfo/bin/${plugin_subdir})
else()                                                                                  
  set_target_properties( ${target} PROPERTIES RUNTIME_OUTPUT_DIRECTORY                ${dest}/bin/${plugin_subdir})
  set_target_properties( ${target} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY                ${dest}/bin/${plugin_subdir})
  set_target_properties( ${target} PROPERTIES LIBRARY_OUTPUT_DIRECTORY                ${dest}/bin/${plugin_subdir})
endif()

install(TARGETS ${target}
  RUNTIME DESTINATION bin/${plugin_subdir}
  LIBRARY DESTINATION bin/${plugin_subdir}
  FRAMEWORK DESTINATION bin/${plugin_subdir}
  RESOURCE DESTINATION resources/${target}
  )
endmacro()

################################################################################
#
# Usage: set_plugin_install_rules_legacy(target)
# set rules for the plugin legacy designed by the target
#
################################################################################
macro(set_plugin_install_rules_legacy target)
  set_plugin_install_rules_legacy_generic(${target} ${CMAKE_BINARY_DIR} ${ARGN})
endmacro()

################################################################################
#
# Usage: set_plugin_install_rules_legacy_external(target)
# set rules for the external plugin legacy designed by the target
#
################################################################################
macro(set_plugin_install_rules_legacy_external target)
  set_plugin_install_rules_legacy_generic(${target} ${medInria_ROOT} ${ARGN})
endmacro()

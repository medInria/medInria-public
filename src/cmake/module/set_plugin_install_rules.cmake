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
##############################   LEGACY   ######################################
macro(set_plugin_install_rules_generic target dest legacy)
get_property(GENERATOR_MULTI_CONFIG GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)

if(${GENERATOR_MULTI_CONFIG})
  set_target_properties( ${target} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG          ${dest}/bin/plugins${legacy})
  set_target_properties( ${target} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE        ${dest}/bin/plugins${legacy})
  set_target_properties( ${target} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL     ${dest}/bin/plugins${legacy})
  set_target_properties( ${target} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${dest}/bin/plugins${legacy})

  set_target_properties( ${target} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_DEBUG          ${dest}/lib/plugins${legacy})
  set_target_properties( ${target} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_RELEASE        ${dest}/lib/plugins${legacy})
  set_target_properties( ${target} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_MINSIZEREL     ${dest}/lib/plugins${legacy})
  set_target_properties( ${target} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_RELWITHDEBINFO ${dest}/lib/plugins${legacy})

  set_target_properties( ${target} PROPERTIES LIBRARY_OUTPUT_DIRECTORY_DEBUG          ${dest}/bin/plugins${legacy})
  set_target_properties( ${target} PROPERTIES LIBRARY_OUTPUT_DIRECTORY_RELEASE        ${dest}/bin/plugins${legacy})
  set_target_properties( ${target} PROPERTIES LIBRARY_OUTPUT_DIRECTORY_MINSIZEREL     ${dest}/bin/plugins${legacy})
  set_target_properties( ${target} PROPERTIES LIBRARY_OUTPUT_DIRECTORY_RELWITHDEBINFO ${dest}/bin/plugins${legacy})
else()
  set_target_properties( ${target} PROPERTIES RUNTIME_OUTPUT_DIRECTORY                ${dest}/bin/plugins${legacy})
  set_target_properties( ${target} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY                ${dest}/lib/plugins${legacy})
  set_target_properties( ${target} PROPERTIES LIBRARY_OUTPUT_DIRECTORY                ${dest}/bin/plugins${legacy})
endif()

install(TARGETS ${target}
  RUNTIME DESTINATION bin/plugins${legacy}
  LIBRARY DESTINATION bin/plugins${legacy}
  ARCHIVE DESTINATION lib/plugins${legacy}
  )
endmacro()



################################################################################
#
# Usage: set_plugin_install_rules(target)
# set rules for the plugin designed by the target
#
################################################################################
macro(set_plugin_install_rules target)
  set_plugin_install_rules_generic(${target} ${MEDINRIA_PATH} "")
endmacro()

################################################################################
#
# Usage: set_plugin_install_rules_legacy(target)
# set rules for the plugin legacy designed by the target
#
################################################################################
macro(set_plugin_install_rules_legacy target)
  set_plugin_install_rules_generic(${target} ${MEDINRIA_PATH} "_legacy") 
endmacro()

################################################################################
#
# Usage: set_plugin_install_rules_legacy_external(target)
# set rules for the external plugin legacy designed by the target
#
################################################################################
macro(set_plugin_install_rules_legacy_external target)
  set_plugin_install_rules_generic(${target} ${MEDINRIA_PATH} "_legacy")
endmacro()


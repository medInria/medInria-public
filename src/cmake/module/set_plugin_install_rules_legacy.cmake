################################################################################
#
# medInria
#
# Copyright (c) INRIA 2013 - 2014. All rights reserved.
# See LICENSE.txt for details.
#
#  This software is distributed WITHOUT ANY WARRANTY; without even
#  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
#  PURPOSE.
#
################################################################################

macro(set_plugin_install_rules_legacy
  target
  )

################################################################################
#
# Usage: set_plugin_install_rules(target)
# set rules for the plugin designed by the target
#
################################################################################


set_target_properties(${target} PROPERTIES
  LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/plugins_legacy/
  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/plugins_legacy/
  ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/plugins_legacy/
  )


install(TARGETS ${project_name}
  RUNTIME DESTINATION plugins_legacy
  LIBRARY DESTINATION plugins_legacy
  ARCHIVE DESTINATION plugins_legacy
  )

endmacro()

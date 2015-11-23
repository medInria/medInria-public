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

macro(set_plugin_install_rules
  target
  )

################################################################################
#
# Usage: set_plugin_install_rules(target)
# set rules for the plugin designed by the target
#
################################################################################

set_target_properties(${target} PROPERTIES
  LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/plugins/
  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/plugins/
  ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/plugins/
  DEFINE_SYMBOL MEDPLUGINS_EXPORT
  )

install(TARGETS ${target}
  RUNTIME DESTINATION plugins
  LIBRARY DESTINATION plugins
  ARCHIVE DESTINATION plugins
  )

endmacro()

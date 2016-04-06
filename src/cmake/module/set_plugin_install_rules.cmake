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

if(WIN32)
  set_target_properties(${target} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/plugins
    )
else()
  set_target_properties(${target} PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/plugins
    )
endif()


install(TARGETS ${target}
  RUNTIME DESTINATION bin/plugins
  LIBRARY DESTINATION bin/plugins
  )

endmacro()

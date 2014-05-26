################################################################################
#
# medInria
#
# Copyright (c) INRIA 2013. All rights reserved.
# See LICENSE.txt for details.
# 
#  This software is distributed WITHOUT ANY WARRANTY; without even
#  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
#  PURPOSE.
#
################################################################################

################################################################################
#
# Usage: set_plugin_install_rules(target)
# set rules for the plugin designed by the target 
#
################################################################################

macro(set_plugin_install_rules
  project_name 
  )
  
install(TARGETS ${project_name}
  RUNTIME DESTINATION plugins
  LIBRARY DESTINATION plugins
  ARCHIVE DESTINATION lib
  )

endmacro()

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

macro(set_exe_install_rules
  project_name 
  )

install(TARGETS ${project_name}
  RUNTIME DESTINATION bin
  BUNDLE  DESTINATION bin
  )

endmacro()


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

macro(set_lib_install_rules
  project_name
  headers_list
  )
  
set(MEDINRIA_BINARY_DIR
  ${${project_name}_BINARY_DIR}/lib
  ${MEDINRIA_BINARY_DIR}
  )
  
install(TARGETS ${project_name}
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  )


## #############################################################################
## Add header wich have to be exposed in the include dir of the install tree
## #############################################################################

if(${ARGC} GREATER 1)
  set(headers ${ARGV})
  list(REMOVE_ITEM headers ${project_name})
  install(FILES ${headers}
    DESTINATION include/${project_name}
    )
endif()

endmacro()


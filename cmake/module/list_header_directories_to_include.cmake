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
# Usage: list_header_directories_to_include(project_name, header_path1, header_path2 ...)
# list all the different path finded in  all given header_path, and add them to
# ${project_name}_INCLUDE_DIRS 
#
################################################################################

macro(list_header_directories_to_include
  project_name 
  headers
  )

## #############################################################################
## List header directores to include
## #############################################################################

foreach(headers_path ${ARGV})
    get_filename_component(dir_path ${headers_path} PATH)
    set(${project_name}_INCLUDE_DIRS
        ${dir_path} 
        ${${project_name}_INCLUDE_DIRS} 
        )
endforeach()

list(REMOVE_DUPLICATES ${project_name}_INCLUDE_DIRS)


## #############################################################################
## add it to the parent scope for future usage.
## #############################################################################

set(${project_name}_INCLUDE_DIRS 
    ${${project_name}_INCLUDE_DIRS}
    PARENT_SCOPE
    )

endmacro()

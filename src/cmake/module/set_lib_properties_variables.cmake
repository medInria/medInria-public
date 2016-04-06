################################################################################
#
# medInria
#
# Copyright (c) INRIA 2013 - 2015. All rights reserved.
# See LICENSE.txt for details.
#
#  This software is distributed WITHOUT ANY WARRANTY; without even
#  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
#  PURPOSE.
#
################################################################################

macro(set_lib_properties_variables
  target_names
  )

################################################################################
#
# Usage: set_lib_include_properties_variables(target_name1, target_name2 ...)
# create a variable ${target_name}_INCLUDE_DIRS for each input target
#
################################################################################

## #############################################################################
## Get include directories properties
## #############################################################################

foreach(target_name ${ARGV})
    get_property(${target_name}_INCLUDE_DIRS TARGET ${target_name} PROPERTY INTERFACE_INCLUDE_DIRECTORIES)
    if (WIN32)
        get_property(${target_name}_LIBRARY_DIR TARGET ${target_name} PROPERTY RUNTIME_OUTPUT_DIRECTORY)
    else()
        get_property(${target_name}_LIBRARY_DIR TARGET ${target_name} PROPERTY LIBRARY_OUTPUT_DIRECTORY)
    endif()
endforeach()

endmacro()
